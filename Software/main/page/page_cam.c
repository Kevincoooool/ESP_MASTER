/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-10-09 14:55:12
 * @FilePath     : \esp_master\main\page\page_cam.c
 */
#include "page_cam.h"
#include "page_start.h"
#include "page_menu.h"
#include "app_main.h"
#include "app_face.h"
#include "EasyTracer.h"
#include "button.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "lv_compment.h"
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "lv_port_indev.h"

#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"
#define TAG "PAGE_CAM"
#define ANIEND                      \
    while (lv_anim_count_running()) \
        lv_task_handler(); //等待动画完成

lv_obj_t *img_cam; //要显示图像
lv_group_t *group_button;
extern lv_img_dsc_t imgfft;
extern lv_img_dsc_t img_dsc;
extern camera_fb_t *fb;
RESULT Resu;
uint8_t color_type = 0;
TARGET_CONDI Condition[3] = {
    {50, 120, 70, 250, 10, 180, 40, 40, 120, 120},  //绿色
    {180, 255, 70, 250, 10, 180, 40, 40, 120, 120}, //红色
    {130, 170, 70, 250, 10, 180, 40, 40, 120, 120}, //蓝色
};

void Cam_Task(void *pvParameters)
{

    // /* 入口处检测一次 */
    ESP_LOGI(TAG, "Run Run uxHighWaterMark = %d", uxTaskGetStackHighWaterMark(NULL));
    // FILE *fp = NULL;
    portTickType xLastWakeTime;

    while (1)
    {
        if (cam_en)
        {
            static int64_t last_frame = 0;
            if (!last_frame)
            {
                last_frame = esp_timer_get_time();
            }
            fb = esp_camera_fb_get();
            if (fb == NULL)
            {
                vTaskDelay(100);
                ESP_LOGE(TAG, "Get image failed!");
            }
            else
            {
                if (Trace(&Condition[color_type], &Resu) && color_en == 1)
                {
                    printf("x:%d y:%d w:%d h:%d ", Resu.x, Resu.y, Resu.w, Resu.h);
                    if (Resu.x > 0 && Resu.y > 0)
                    {
                        draw_fillRect(fb, Resu.x - Resu.w / 2, Resu.y - Resu.h / 2, Resu.w, Resu.h);
                    }
                }
                // static uint16_t i = 0;
                // if (i == 0)
                // {
                // ESP_LOGI(TAG, "Reading file");
                // fp = fopen("/spiffs/output.rgb", "r");
                // if (fp == NULL)
                // {
                // 	ESP_LOGE(TAG, "Failed to open file for reading");
                // 	return;
                // }
                // printf("打开文件成功\n");
                // fgets((char *)dis_buf, 100 * 100 * 2, fp);
                // fclose(fp);

                // //ffmpeg -t 30 -i bad_apple_30.mp4 -vf "fps=10,scale=-1:100:flags=lanczos,crop=100:in_h:(in_w-100)/2:0,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" -c:v rawvideo -pix_fmt rgb565be output.rgb

                // fgets((char *)dis_buf, 100 * 100 * 2, fp);

                img_dsc.data = fb->buf;
                lv_img_set_src(img_cam, &img_dsc);

                // i++;
                // if (i == 400)
                // {
                // 	i = 0;
                // 	fclose(fp);
                // }
                esp_camera_fb_return(fb);
                fb = NULL;
                int64_t fr_end = esp_timer_get_time();
                int64_t frame_time = fr_end - last_frame;
                last_frame = fr_end;
                frame_time /= 1000;
                ESP_LOGI("esp", "MJPG:  %ums (%.1ffps)", (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time);
                // vTaskDelayUntil(&xLastWakeTime, (20 / portTICK_RATE_MS));
            }
        }
        else
        {
            if (fb)
                free(fb);
            fb = NULL;
            vTaskDelete(NULL);
        }
    }

    // never reach
    while (1)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
//当前界面是菜单时MOVE任务要做的事情
void move_task_cam(uint8_t move)
{

    switch (move)
    {
    case BT1_DOWN: //往上移动
        // color_type--;
        // color_type = color_type <= 0 ? 0 : color_type;
        break;
    case BT1_LONG: //往上移动

        break;
    case BT1_LONGFREE: //往上移动
        break;
    case BT3_DOWN: //往下移动
        // color_type++;
        // color_type = color_type >= 2 ? 2 : color_type;
        break;
    case BT3_LONG: //往下移动

        break;
    case BT3_LONGFREE: //往上移动
        break;

    default:
        break;
    }
}
//当前界面是菜单时MOVE任务要做的事情
void move_task_color(uint8_t move)
{

    switch (move)
    {
    case BT1_DOWN: //往上移动
        color_type--;
        color_type = color_type < 0 ? 0 : color_type;
        break;
    case BT1_LONG: //往上移动

        break;
    case BT1_LONGFREE: //往上移动
        break;
    case BT3_DOWN: //往下移动
        color_type++;
        color_type = color_type > 2 ? 2 : color_type;
        break;
    case BT3_LONG: //往下移动

        break;
    case BT3_LONGFREE: //往上移动
        break;

    default:
        break;
    }
}
void imgcam_init(void)
{

    img_cam = lv_img_create(lv_scr_act(), NULL);
    static lv_style_t style_img;
    lv_style_init(&style_img);

    //Write style state: LV_STATE_DEFAULT for style_img
    lv_style_set_image_recolor(&style_img, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_image_recolor_opa(&style_img, LV_STATE_DEFAULT, 0);
    lv_style_set_image_opa(&style_img, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(img_cam, LV_IMG_PART_MAIN, &style_img);
    lv_obj_set_pos(img_cam, 0, 0);
    lv_obj_set_size(img_cam, 240, 240);
}
lv_task_t *task_cam;

void page_cam_load()
{
    app_camera_init();
    imgcam_init();
    obj_add_anim(
        img_cam,                           //动画对象
        (lv_anim_exec_xcb_t)lv_obj_set_x,  //动画函数
        lv_anim_speed_to_time(300, 0, 50), //动画速度
        240,                               //起始值
        0,                                 //结束值
        lv_anim_path_linear                //动画特效:模拟弹性物体下落
    );
    ANIEND
    xTaskCreatePinnedToCore(&Cam_Task, "Cam_Task", 1024 * 5, NULL, 14, NULL, 0);
}
void page_cam_end(void)
{
    cam_en = 0, color_en = 0, face_en = 0;
    vTaskDelay(500);
    esp_camera_deinit();
    obj_add_anim(
        img_cam,                           //动画对象
        (lv_anim_exec_xcb_t)lv_obj_set_x,  //动画函数
        lv_anim_speed_to_time(300, 0, 50), //动画速度
        0,                                 //起始值
        240,                               //结束值
        lv_anim_path_linear                //动画特效:模拟弹性物体下落
    );
    ANIEND
    lv_obj_del(img_cam);
}
extern en_fsm_state g_state;

void page_cam_start(void)
{
    //获取芯片可用内存
    printf(" page_cam_start    esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
    //获取从未使用过的最小内存
    printf(" page_cam_start    esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());

    printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    page_cam_load();
}


