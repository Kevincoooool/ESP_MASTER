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

#include "object_detection.h"
#include "math.h"
#include "esp_image.hpp"

inline fptp_t __get_shift_factor(int exponent)
{
    fptp_t factor = 1.0;

    if (exponent > 0)
        factor = (1 << exponent);
    if (exponent < 0)
        factor = 1.0 / (1 << -exponent);

    return factor;
}

inline fptp_t __fast_exp(double x, int steps)
{
    x = 1.0 + x / (1 << steps);
    for (int i = 0; i < steps; i++)
        x *= x;
    return x;
}

inline qtp_t __desigmoid(fptp_t value, int exponent)
{
    return -log(1 / value - 1) * (1 << exponent);
}

void *__ab_get_boxes(detection_stage_result_t *stage, detection_model_config_t *model_config, detection_stage_config_t *stage_config, int stage_index)
{
    int score_threshold = __desigmoid(model_config->score_threshold, stage[stage_index].score->exponent);
    int stride = stage_config[stage_index].stride;
    int project_offset = stage_config[stage_index].project_offset;
    int **anchors = stage_config[stage_index].anchors_shape;

    fptp_t y_resize_scale = model_config->y_resize_scale;
    fptp_t x_resize_scale = model_config->x_resize_scale;

    qtp_t *score = stage[stage_index].score->item;
    qtp_t *box_offset = stage[stage_index].box_offset->item;
    int box_offset_shift = -stage[stage_index].box_offset->exponent;

#if CONFIG_DETECT_WITH_LANDMARK
    qtp_t *landmark_offset = stage[stage_index].landmark_offset->item;
    int landmark_offset_shift = -stage[stage_index].landmark_offset->exponent;
#endif

    image_box_t *redundant = (image_box_t *)dl_lib_calloc(stage[stage_index].score->h * stage[stage_index].score->w * stage[stage_index].score->n, sizeof(image_box_t), 0);
    image_list_t *valid_list = (image_list_t *)dl_lib_calloc(1, sizeof(image_list_t), 0);

    /*
        score format is (anchor_num, h, w, cls), box is (anchor_num, h, w, 4);
        while in the memory layout is (h, w, anchor, cls) and (h, w, anchor, 4)
    */
    for (size_t y = 0; y < stage[stage_index].score->h; y++)
    {
        for (size_t x = 0; x < stage[stage_index].score->w; x++)
        {
            for (size_t a = 0; a < stage[stage_index].score->n; a++)
            {
                int max_score = *score++;
                int max_score_c = 0;
                for (size_t c = 1; c < stage[stage_index].score->c; c++)
                {
                    if (max_score < *score)
                    {
                        max_score = *score;
                        max_score_c = c;
                    }
                    score++;
                }
                if (max_score > score_threshold)
                {
                    redundant[valid_list->len].category = max_score_c;
                    redundant[valid_list->len].score = max_score;
                    int center_y = (y * stride + project_offset) * y_resize_scale;
                    int center_x = (x * stride + project_offset) * x_resize_scale;
                    int anchor_h = anchors[a][0] * y_resize_scale;
                    int anchor_w = anchors[a][1] * x_resize_scale;
                    redundant[valid_list->len].box.box_p[0] = center_x - anchor_w / 2 + ((anchor_w * box_offset[0]) >> box_offset_shift);
                    redundant[valid_list->len].box.box_p[1] = center_y - anchor_h / 2 + ((anchor_h * box_offset[1]) >> box_offset_shift);
                    redundant[valid_list->len].box.box_p[2] = center_x + anchor_w / 2 + ((anchor_w * box_offset[2]) >> box_offset_shift);
                    redundant[valid_list->len].box.box_p[3] = center_y + anchor_h / 2 + ((anchor_h * box_offset[3]) >> box_offset_shift);
#if CONFIG_DETECT_WITH_LANDMARK
                    redundant[valid_list->len].landmark.landmark_p[0] = center_x - anchor_w / 2 + ((anchor_w * landmark_offset[0]) >> landmark_offset_shift);
                    redundant[valid_list->len].landmark.landmark_p[1] = center_y - anchor_h / 2 + ((anchor_h * landmark_offset[1]) >> landmark_offset_shift);
                    redundant[valid_list->len].landmark.landmark_p[2] = center_x - anchor_w / 2 + ((anchor_w * landmark_offset[2]) >> landmark_offset_shift);
                    redundant[valid_list->len].landmark.landmark_p[3] = center_y - anchor_h / 2 + ((anchor_h * landmark_offset[3]) >> landmark_offset_shift);
                    redundant[valid_list->len].landmark.landmark_p[4] = center_x - anchor_w / 2 + ((anchor_w * landmark_offset[4]) >> landmark_offset_shift);
                    redundant[valid_list->len].landmark.landmark_p[5] = center_y - anchor_h / 2 + ((anchor_h * landmark_offset[5]) >> landmark_offset_shift);
                    redundant[valid_list->len].landmark.landmark_p[6] = center_x - anchor_w / 2 + ((anchor_w * landmark_offset[6]) >> landmark_offset_shift);
                    redundant[valid_list->len].landmark.landmark_p[7] = center_y - anchor_h / 2 + ((anchor_h * landmark_offset[7]) >> landmark_offset_shift);
                    redundant[valid_list->len].landmark.landmark_p[8] = center_x - anchor_w / 2 + ((anchor_w * landmark_offset[8]) >> landmark_offset_shift);
                    redundant[valid_list->len].landmark.landmark_p[9] = center_y - anchor_h / 2 + ((anchor_h * landmark_offset[9]) >> landmark_offset_shift);
#endif
                    valid_list->len++;
                }
                box_offset += 4;
#if CONFIG_DETECT_WITH_LANDMARK
                landmark_offset += LANDMARKS_NUM;
#endif
            }
        }
    }

    if (0 == valid_list->len)
    {
        dl_lib_free(redundant);
        dl_lib_free(valid_list);
        return NULL;
    }

    image_box_t *compact = (image_box_t *)dl_lib_calloc(valid_list->len, sizeof(image_box_t), 0);
    size_t i = 0;
    for (; i < valid_list->len - 1; i++)
    {
        compact[i] = redundant[i];
        compact[i].next = &(compact[i + 1]);
    }
    compact[i] = redundant[i];
    dl_lib_free(redundant);

    valid_list->head = compact;
    valid_list->origin_head = compact;

    return valid_list;
}

void *__ap_get_boxes(detection_stage_result_t *stage, detection_model_config_t *model_config, detection_stage_config_t *stage_config, int stage_index)
{
    int score_threshold = __desigmoid(model_config->score_threshold, stage[stage_index].score->exponent);
    int stride = stage_config[stage_index].stride;
    int project_offset = stage_config[stage_index].project_offset;

    fptp_t y_resize_scale = model_config->y_resize_scale;
    fptp_t x_resize_scale = model_config->x_resize_scale;

    qtp_t *score = stage[stage_index].score->item;
    qtp_t *box_offset = stage[stage_index].box_offset->item;
    fptp_t box_offset_shift_factor = __get_shift_factor(stage[stage_index].box_offset->exponent);

    image_box_t *redundant = (image_box_t *)dl_lib_calloc(stage[stage_index].score->h * stage[stage_index].score->w, sizeof(image_box_t), 0);
    image_list_t *valid_list = (image_list_t *)dl_lib_calloc(1, sizeof(image_list_t), 0);

    for (size_t y = 0; y < stage[stage_index].score->h; y++)
    {
        for (size_t x = 0; x < stage[stage_index].score->w; x++)
        {
            int max_score = *score++;
            int max_score_c = 0;
            for (size_t c = 1; c < stage[stage_index].score->c; c++)
            {
                if (max_score < *score)
                {
                    max_score = *score;
                    max_score_c = c;
                }
                score++;
            }
            if (max_score > score_threshold)
            {
                redundant[valid_list->len].category = max_score_c;
                redundant[valid_list->len].score = max_score;
                qtp_t center_y = y * stride + project_offset;
                qtp_t center_x = x * stride + project_offset;
                redundant[valid_list->len].box.box_p[0] = (center_x - __fast_exp(box_offset[0] * box_offset_shift_factor, 8)) * x_resize_scale;
                redundant[valid_list->len].box.box_p[1] = (center_y - __fast_exp(box_offset[1] * box_offset_shift_factor, 8)) * y_resize_scale;
                redundant[valid_list->len].box.box_p[2] = (center_x + __fast_exp(box_offset[2] * box_offset_shift_factor, 8)) * x_resize_scale;
                redundant[valid_list->len].box.box_p[3] = (center_y + __fast_exp(box_offset[3] * box_offset_shift_factor, 8)) * y_resize_scale;
                valid_list->len++;
            }
            box_offset += 4;
        }
    }

    if (0 == valid_list->len)
    {
        dl_lib_free(redundant);
        dl_lib_free(valid_list);
        return NULL;
    }

    image_box_t *compact = (image_box_t *)dl_lib_calloc(valid_list->len, sizeof(image_box_t), 0);
    size_t i = 0;
    for (; i < valid_list->len - 1; i++)
    {
        compact[i] = redundant[i];
        compact[i].next = &(compact[i + 1]);
    }
    compact[i] = redundant[i];
    dl_lib_free(redundant);

    valid_list->head = compact;
    valid_list->origin_head = compact;

    return valid_list;
}

void update_detection_model(detection_model_t *model, fptp_t resize_scale, fptp_t score_threshold, fptp_t nms_threshold, int image_height, int image_width)
{
    if (model->model_type == Anchor_Box)
        model->get_boxes = __ab_get_boxes;
    else if (model->model_type == Anchor_Point)
        model->get_boxes = __ap_get_boxes;

    model->model_config.resized_height = round(image_height * resize_scale);
    model->model_config.resized_width = round(image_width * resize_scale);
    model->model_config.y_resize_scale = (fptp_t)image_height / (fptp_t)model->model_config.resized_height;
    model->model_config.x_resize_scale = (fptp_t)image_width / (fptp_t)model->model_config.resized_width;
    model->model_config.score_threshold = score_threshold;
    model->model_config.nms_threshold = nms_threshold;
    model->model_config.with_landmark = false;
    model->model_config.free_image = true;

    int short_side = min(model->model_config.resized_height, model->model_config.resized_width);
    model->model_config.enabled_top_k = 0;
    for (size_t i = 0; i < model->stage_number; i++)
    {
        if (short_side >= model->stage_config[i].boundary)
            model->model_config.enabled_top_k++;
        else
            break;
    }
    assert(model->model_config.enabled_top_k > 0);
}

box_array_t *detect_object(dl_matrix3du_t *image, detection_model_t *model)
{
    // resize image
    dl_matrix3dq_t *resized_image = dl_matrix3dq_alloc(1, model->model_config.resized_width, model->model_config.resized_height, image->c, 0);
    Image<qtp_t>::resize_to_rgb888(resized_image->item, 0, resized_image->h, 0, resized_image->w, resized_image->c, image->item, image->h, image->w, resized_image->w, 0, IMAGE_RESIZE_MEAN);

    // net operation
    detection_stage_result_t *stage_result = model->op(resized_image, &model->model_config);

    // filter by score
    image_list_t **origin_head = (image_list_t **)dl_lib_calloc(model->model_config.enabled_top_k, sizeof(image_list_t *), 0);
    image_list_t all_box_list = {NULL, NULL, 0};
    for (size_t i = 0; i < model->model_config.enabled_top_k; i++)
    {
        origin_head[i] = (image_list_t *)model->get_boxes(stage_result, &model->model_config, model->stage_config, i);

        if (origin_head[i])
            image_sort_insert_by_score(&all_box_list, origin_head[i]);

        free_detection_stage_result(stage_result[i]);
    }
    dl_lib_free(stage_result);

    // nms
    image_nms_process(&all_box_list, model->model_config.nms_threshold, false);

    // build up result
    box_array_t *targets_list = NULL;
    if (all_box_list.len)
    {
        targets_list = (box_array_t *)dl_lib_calloc(1, sizeof(box_array_t), 0);
        targets_list->len = all_box_list.len;
        targets_list->category = (uint8_t *)dl_lib_calloc(targets_list->len, sizeof(uint8_t), 0);
        targets_list->score = (fptp_t *)dl_lib_calloc(targets_list->len, sizeof(fptp_t), 0);
        targets_list->box = (box_t *)dl_lib_calloc(targets_list->len, sizeof(box_t), 0);
#if CONFIG_DETECT_WITH_LANDMARK
        targets_list->landmark = (landmark_t *)dl_lib_calloc(targets_list->len, sizeof(landmark_t), 0);
#endif

        image_box_t *t = all_box_list.head;
        for (int i = 0; i < all_box_list.len; i++, t = t->next)
        {
            targets_list->category[i] = t->category;
            targets_list->score[i] = t->score;
            targets_list->box[i] = t->box;
#if CONFIG_DETECT_WITH_LANDMARK
            targets_list->landmark[i] = t->landmark;
#endif
        }
    }

    for (int i = 0; i < model->model_config.enabled_top_k; i++)
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
