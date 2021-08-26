/*
  * ESPRESSIF MIT License
  *
  * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
  *
  * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
  * it is free of charge, to any person obtaining a copy of this software and associated
  * documentation files (the "Software"), to deal in the Software without restriction, including
  * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
  * to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all copies or
  * substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  *
  */
#include <string.h>
#include <math.h>
#include "esp_system.h"
#include "pe_forward.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define HD_LITE_FEATURE_MAP_NUM 1

void od_image_sort_insert_by_score(od_image_list_t *image_sorted_list, const od_image_list_t *insert_list)
{ /*{{{*/
    if (insert_list == NULL || insert_list->head == NULL)
        return;
    od_image_box_t *box = insert_list->head;
    if (NULL == image_sorted_list->head)
    {
        image_sorted_list->head = insert_list->head;
        box = insert_list->head->next;
        image_sorted_list->head->next = NULL;
    }
    od_image_box_t *head = image_sorted_list->head;

    while (box)
    {
        // insert in head
        if (box->score > head->score)
        {
            od_image_box_t *tmp = box;
            box = box->next;
            tmp->next = head;
            head = tmp;
        }
        else
        {
            od_image_box_t *curr = head->next;
            od_image_box_t *prev = head;
            while (curr)
            {
                if (box->score > curr->score)
                {
                    od_image_box_t *tmp = box;
                    box = box->next;
                    tmp->next = curr;
                    prev->next = tmp;
                    break;
                }
                prev = curr;
                curr = curr->next;
            }
            // insert in tail
            if (NULL == curr)
            {
                od_image_box_t *tmp = box;
                box = box->next;
                tmp->next = NULL;
                prev->next = tmp;
            }
        }
    }
    image_sorted_list->head = head;
    image_sorted_list->len += insert_list->len;
} /*}}}*/

od_image_list_t *od_image_get_valid_boxes(fptp_t *score,
                                    fptp_t *cls,
                                    fptp_t *boxes,
                                    int height,
                                    int width,
                                    int anchor_number,
                                    fptp_t score_threshold,
                                    fptp_t resize_scale,
                                    int padding_w,
                                    int padding_h)
{ /*{{{*/
    typedef struct
    {
        int x;
        int y;
        int index;
        uc_t anchor_index;
    } valid_index_t;
    valid_index_t *valid_indexes = (valid_index_t *)dl_lib_calloc(width * height, sizeof(valid_index_t), 0);
    int valid_count = 0;
    int index = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (size_t c = 0; c < anchor_number; c++)
            {
                if (score[index] > score_threshold)
                {
                    valid_indexes[valid_count].x = x;
                    valid_indexes[valid_count].y = y;
                    valid_indexes[valid_count].index = index;
                    valid_indexes[valid_count].anchor_index = c;
                    valid_count++;
                }
                index++;
            }
        }
    }

    if (0 == valid_count)
    {
        dl_lib_free(valid_indexes);
        return NULL;
    }

    od_image_box_t *valid_box = (od_image_box_t *)dl_lib_calloc(valid_count, sizeof(od_image_box_t), 0);
    od_image_list_t *valid_list = (od_image_list_t *)dl_lib_calloc(1, sizeof(od_image_list_t), 0);
    valid_list->head = valid_box;
    valid_list->origin_head = valid_box;
    valid_list->len = valid_count;

    for (int i = 0; i < valid_count; i++)
    {
        valid_box[i].score = score[valid_indexes[i].index];

        valid_box[i].box.box_p[0] = ((boxes[4*valid_indexes[i].index] - 0.5*boxes[4*valid_indexes[i].index + 2])-padding_w)/resize_scale;
        valid_box[i].box.box_p[1] = ((boxes[4*valid_indexes[i].index + 1] - 0.5*boxes[4*valid_indexes[i].index + 3])-padding_h)/resize_scale;
        valid_box[i].box.box_p[2] = ((boxes[4*valid_indexes[i].index] + 0.5*boxes[4*valid_indexes[i].index + 2])-padding_w)/resize_scale;
        valid_box[i].box.box_p[3] = ((boxes[4*valid_indexes[i].index + 1] + 0.5*boxes[4*valid_indexes[i].index + 3])-padding_h)/resize_scale;

        valid_box[i].cls = (qtp_t)cls[valid_indexes[i].index];

        valid_box[i].next = &(valid_box[i + 1]);
    }
    valid_box[valid_count - 1].next = NULL;

    dl_lib_free(valid_indexes);

    return valid_list;
} /*}}}*/

void od_image_nms_process(od_image_list_t *image_list, fptp_t nms_threshold)
{ /*{{{*/
    /**** Init ****/
    int num_supressed = 0;
    od_image_box_t *head = image_list->head;

    /**** Compute Box Area ****/
    fptp_t kept_box_area = 0;
    fptp_t other_box_area = 0;

    /**** Compare IOU ****/
    od_image_box_t *kept_box = head;
    while (kept_box)
    {
        od_image_box_t *other_box = kept_box->next;
        od_image_box_t *prev = kept_box;
        while (other_box)
        {

            box_t inter_box;
            inter_box.box_p[0] = DL_IMAGE_MAX(kept_box->box.box_p[0], other_box->box.box_p[0]);
            inter_box.box_p[1] = DL_IMAGE_MAX(kept_box->box.box_p[1], other_box->box.box_p[1]);
            inter_box.box_p[2] = DL_IMAGE_MIN(kept_box->box.box_p[2], other_box->box.box_p[2]);
            inter_box.box_p[3] = DL_IMAGE_MIN(kept_box->box.box_p[3], other_box->box.box_p[3]);

            fptp_t inter_w, inter_h;
            image_get_width_and_height(&inter_box, &inter_w, &inter_h);

            if (inter_w > 0 && inter_h > 0)
            {

                image_get_area(&(kept_box->box), &kept_box_area);
                image_get_area(&(other_box->box), &other_box_area);

                fptp_t inter_area = inter_w * inter_h;
                fptp_t iou = inter_area / (kept_box_area + other_box_area - inter_area);
                if (iou > nms_threshold)
                {
                    num_supressed++;
                    // Delete duplicated box
                    // Here we cannot free a single box, because these boxes are allocated by calloc, we need to free all the calloced memory together.
                    prev->next = other_box->next;
                    other_box = other_box->next;
                    continue;
                }
            }
            prev = other_box;
            other_box = other_box->next;
        }
        kept_box = kept_box->next;
    }

    image_list->len -= num_supressed;
} /*}}}*/


od_box_array_t *hand_detection_forward(dl_matrix3du_t *image, hd_config_t hd_config)
{
    /**
     * @brief resize image
     * 
     */

    int preprocess_mode = 0;
    dl_matrix3dq_t *hd_image_input = image_resize_normalize_quantize(image->item, image->w, image->h, hd_config.target_size, INPUT_EXPONENT, preprocess_mode);

    /**
     * @brief net operation
     * 
     */
#if CONFIG_XTENSA_IMPL
    #if CONFIG_HD_LITE1
        detection_result_t **hd_results = hd_lite1_q(hd_image_input, DL_XTENSA_IMPL);
    #else
        detection_result_t **hd_results = hd_nano1_q(hd_image_input, DL_XTENSA_IMPL);
    #endif
#else
    #if CONFIG_HD_LITE1
        detection_result_t **hd_results = hd_lite1_q(hd_image_input, DL_C_IMPL);
    #else
        detection_result_t **hd_results = hd_nano1_q(hd_image_input, DL_C_IMPL);
    #endif
#endif
    /**
     * @brief filter by score
     * 
     */
    od_image_list_t **origin_head = (od_image_list_t **)dl_lib_calloc(HD_LITE_FEATURE_MAP_NUM, sizeof(od_image_list_t *), 0);
    od_image_list_t all_box_list = {NULL};

    for (size_t i = 0; i < HD_LITE_FEATURE_MAP_NUM; i++)
    {
        //yolo_result_print(yolo_results[i]);
        fptp_t *cls = hd_results[i]->cls->item;
        fptp_t *score = hd_results[i]->score->item;
        fptp_t *boxes = hd_results[i]->boxes->item;
        fptp_t resize_scale = (image->w >= image->h)?((float)(hd_config.target_size)/image->w):((float)(hd_config.target_size)/image->h);
        // int padding_w = (target_size - resize_scale*image->w)/2;
        // int padding_h = (target_size - resize_scale*image->h)/2;

        origin_head[i] = od_image_get_valid_boxes(score,
                                               cls,
                                               boxes,
                                               hd_results[i]->cls->n,
                                               hd_results[i]->cls->w,
                                               hd_results[i]->cls->h,
                                               hd_config.score_threshold,
                                               resize_scale,
                                               0,
                                               0);
        if (origin_head[i]){
            od_image_sort_insert_by_score(&all_box_list, origin_head[i]);
        }

        // detection_result_free(hd_results[i]);
    }
    // dl_lib_free(hd_results);
    detection_results_free(hd_results, HD_LITE_FEATURE_MAP_NUM);

    /**
     * @brief nms
     * 
     */
    od_image_nms_process(&all_box_list, hd_config.nms_threshold);

    /**
     * @brief build up result
     * 
     */
    od_box_array_t *targets_list = NULL;
    if (all_box_list.len)
    {
        // printf("all_box_list：%d \n",all_box_list.len);
        targets_list = (od_box_array_t *)dl_lib_calloc(1, sizeof(od_box_array_t), 0);
        targets_list->len = all_box_list.len;
        targets_list->score = (fptp_t *)dl_lib_calloc(targets_list->len, sizeof(fptp_t), 0);
        targets_list->box = (box_t *)dl_lib_calloc(targets_list->len, sizeof(box_t), 0);
        targets_list->cls = (qtp_t *)dl_lib_calloc(targets_list->len, sizeof(qtp_t), 0);

        od_image_box_t *t = all_box_list.head;
        for (int i = 0; i < all_box_list.len; i++, t = t->next)
        {
            targets_list->box[i] = t->box;
            targets_list->cls[i] = t->cls;
            targets_list->score[i] = t->score;
            // printf("score: %f, class: %d\n", t->score, t->cls);
        }
    }

    for (int i = 0; i < HD_LITE_FEATURE_MAP_NUM; i++)
    {
        if (origin_head[i])
        {
            dl_lib_free(origin_head[i]->origin_head);
            dl_lib_free(origin_head[i]);
        }
    }
    dl_lib_free(origin_head);

    return targets_list;
}

static inline dl_matrix3dq_t *dl_matrix3dq_from_3du(dl_matrix3du_t *m, int exponent, int shift_offset)
{
    dl_matrix3dq_t *out = dl_matrix3dq_alloc(m->n, m->w, m->h, m->c, exponent);
    qtp_t *o_item = out->item;
    uint8_t *m_item = m->item;
    int shift = (-exponent)-shift_offset;
    int count=(m->n)*(m->w)*(m->h)*(m->c);
    if(shift>=0){
        for(int i=0;i<count;i++){
            *o_item++ = ((qtp_t)(*(m_item++)))<<shift;
        }
    }else{
        shift = -shift;
        for(int i=0;i<count;i++){
            *o_item++ = ((qtp_t)(*(m_item++)))>>shift;
        }
    }
    return out;
    
}


dl_matrix3d_t *handpose_estimation_forward(dl_matrix3du_t *image, od_box_array_t *od_boxes, int target_size)
{
    int landmark_num = 21;
    dl_matrix3d_t *landmarks = dl_matrix3d_alloc(od_boxes->len, 1, landmark_num, 2);
    float dilat_ratio = 1.2;
    int hp_exponent = INPUT_EXPONENT;
    int shift_offset = 8;
    for(int i=0; i<od_boxes->len; i++){
        float x = od_boxes->box[i].box_p[0];
        float y = od_boxes->box[i].box_p[1];
        float w = od_boxes->box[i].box_p[2] - x + 1;
        float h = od_boxes->box[i].box_p[3] - y + 1;
        float ox = 0.0;
        float oy = 0.0;
        if(w>h){
            oy = (dilat_ratio*w - h)/2.0;
            ox = (dilat_ratio-1)*w/2.0;
        }else{
            ox = (dilat_ratio*h - w)/2.0;
            oy = (dilat_ratio-1)*h/2.0;
        }
        int x1 = (int)(max(0, od_boxes->box[i].box_p[0] - ox));
        int y1 = (int)(max(0, od_boxes->box[i].box_p[1] - oy));
        int x2 = (int)(min(image->w, od_boxes->box[i].box_p[2] + ox));
        int y2 = (int)(min(image->h, od_boxes->box[i].box_p[3] + oy));
        w = x2 - x1;
        h = y2 - y1;

        float scale = 0.0;
        int target_w, target_h = 0;
        int dw = 0;
        int dh = 0;
        if(w >= h){
            scale = (float)(target_size) / w;
            target_w = target_size;
            target_h = (int)(h*scale);
            dh = (target_size - target_h)/2;
        }else{
            scale = (float)(target_size) / w;
            scale = target_size / h;
            target_w = (int)(w*scale);
            target_h = target_size;
            dw = (target_size - target_w)/2;
        }
        float srcx[3] = {x1, x2, x2};
        float srcy[3] = {y1, y2, y1};
        float dstx[3] = {dw, dw+target_w, dw+target_w};
        float dsty[3] = {dh, dh+target_h, dh};
        Matrix *M = get_affine_transform(srcx, srcy, dstx, dsty);
        dl_matrix3du_t *hp_input_image_u = dl_matrix3du_alloc(1, target_size, target_size, image->c);
        warp_affine(image, hp_input_image_u, M);
        matrix_free(M);
        dl_matrix3dq_t *hp_input_image = dl_matrix3dq_from_3du(hp_input_image_u, hp_exponent, shift_offset);
        dl_matrix3du_free(hp_input_image_u);
#if CONFIG_XTENSA_IMPL
    #if CONFIG_HD_LITE1
        dl_matrix3d_t *landmark = hp_lite1_q(hp_input_image, DL_XTENSA_IMPL);
    #else
        dl_matrix3d_t *landmark = hp_nano1_ls16_q(hp_input_image, DL_XTENSA_IMPL);
    #endif
#else
    #if CONFIG_HD_LITE1
        dl_matrix3d_t *landmark = hp_lite1_q(hp_input_image, DL_C_IMPL);
    #else
        dl_matrix3d_t *landmark = hp_nano1_ls16_q(hp_input_image, DL_C_IMPL);
    #endif
#endif
        for(int j=0; j<landmark_num; j++){
            landmarks->item[i*(landmark_num*2)+j*2] = (landmark->item[j*2])/scale + x1;
            landmarks->item[i*(landmark_num*2)+j*2+1] = landmark->item[j*2+1]/scale + y1;
        }
        dl_matrix3d_free(landmark);
    }
    return landmarks;
    
}


dl_matrix3d_t *handpose_estimation_forward2(uint16_t *simage, od_box_array_t *od_boxes, int dw, int sw, int sh, dl_conv_mode mode)
{
    int hp_exponent = INPUT_EXPONENT;
    int shift = -8 - hp_exponent;
    float scale = 0.0;
    int landmark_num = 21;
    if(od_boxes){
        dl_matrix3d_t *landmarks = dl_matrix3d_alloc(od_boxes->len, 1, landmark_num, 2);
        float dilat_ratio = 1.2;
        for(int i=0; i<od_boxes->len; i++){
            dl_matrix3dq_t *image_input = dl_matrix3dq_alloc(1, dw, dw, 3, hp_exponent);
            float x = od_boxes->box[i].box_p[0];
            float y = od_boxes->box[i].box_p[1];
            float w = od_boxes->box[i].box_p[2] - x + 1;
            float h = od_boxes->box[i].box_p[3] - y + 1;
            float ox = 0.0;
            float oy = 0.0;
            if(w>h){
                oy = (dilat_ratio*w - h)/2.0;
                ox = (dilat_ratio-1)*w/2.0;
            }else{
                ox = (dilat_ratio*h - w)/2.0;
                oy = (dilat_ratio-1)*h/2.0;
            }
            int x1 = (int)(max(0, od_boxes->box[i].box_p[0] - ox));
            int y1 = (int)(max(0, od_boxes->box[i].box_p[1] - oy));
            int x2 = (int)(min(sw, od_boxes->box[i].box_p[2] + ox));
            int y2 = (int)(min(sh, od_boxes->box[i].box_p[3] + oy));
            w = x2 - x1;
            h = y2 - y1;
            scale = (float)(max(w, h))/dw;
            
            image_crop_shift_fast(image_input->item, simage, dw, sw, sh, x1, y1, x2, y2, shift);
            dl_matrix3d_t *landmark = hp_nano1_ls16_q(image_input, mode);
            // ets_printf("x1:%d, y1:%d, x2:%d, y2:%d \n", x1, y1, x2, y2); 
            // printf("scale: %f\n", scale);
            for(int j=0; j<landmark_num; j++){
                landmarks->item[i*(landmark_num*2)+j*2] = ((landmark->item[j*2])*scale) + x1;
                landmarks->item[i*(landmark_num*2)+j*2+1] = ((landmark->item[j*2+1])*scale) + y1;
                // ets_printf("lmks %d: %d, %d\n", j, (int)(landmark->item[j*2]), (int)(landmark->item[j*2+1]));
            }
            dl_matrix3d_free(landmark);
        }
        return landmarks;
    }else{
        dl_matrix3dq_t *image_input = dl_matrix3dq_alloc(1, dw, dw, 3, hp_exponent);
        // scale = (sw >= sh)?((float)dw/sw):((float)dw/sh);
        int tw, th = 0;
        if(sw >= sh){
            scale = (float)sw / dw;
            tw = dw;
            th = (int)(sh / scale);
        }else{
            scale = (float)sh / dw;
            tw = (int)(sw / scale);
            th = dw;
        }
        // dl_matrix3dq_t *hp_input_image = od_image_preporcess(image->item, image->w, image->h, target_size, hp_exponent, 0);
        image_resize_shift_fast(image_input->item, simage, dw, 3, sw, sh, tw, th, shift);
        dl_matrix3d_t *landmarks = hp_nano1_ls16_q(image_input, mode);
        for(int j=0; j<landmark_num; j++){
                landmarks->item[j*2] = (landmarks->item[j*2])*scale;
                landmarks->item[j*2+1] = (landmarks->item[j*2+1])*scale;
                // ets_printf("lmks %d: %d, %d\n", j, (int)(landmarks->item[j*2]), (int)(landmarks->item[j*2+1]));
            }
        return landmarks;
    }
}