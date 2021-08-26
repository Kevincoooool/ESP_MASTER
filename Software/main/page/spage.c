/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-05 09:04:44
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-17 14:54:33
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\spage.c
 */

#include "spage.h"
#include "esp_log.h"
#include "page_menu.h"
#include "page_home.h"
#include "page_wakeup.h"
#include "page_cam.h"
#include "page_fft.h"
#include "page_start.h"
#include "page_calendar.h"
#include "page_baiduai.h"
#include "page_game_2048.h"
#include "page_game_snake.h"
#include "page_tab.h"
#include "page_daily.h"
#include "page_imu.h"
#include "page_about.h"
#include "page_music.h"
#include "page_fft.h"
#include "updata.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "spage.h"
#include "esp_err.h"
#include "cJSON.h"
#include "mqtt_client.h"
#include "app_camera.h"
#include "web_server.h"
#include "app_wifi.h"
extern struct _ksdiy_sys_t ksdiy_sys_t;
page_t *page_head;
extern lv_obj_t *cont_head;
extern lv_group_t *group_button;
page_t *add_page(page_t *page)
{
    if (page == NULL) //页头初始化
    {
        page = (page_t *)malloc(sizeof(page_t));
        page->index = 0;
        page->next = NULL;
        page->last = NULL;
        ESP_LOGI("SYSTEM", "添加页面0");
        return page;
    }
    page_t *p, *now; //添加到page后
    now = page;
    while (now->next != NULL) //找到同级菜单最后的表
    {
        now = now->next;
    }
    p = (page_t *)malloc(sizeof(page_t));
    now->next = p;
    p->last = now;
    p->next = NULL;
    p->index = (now->index) + 1;
    ESP_LOGI("SYSTEM", "添加页面%d", p->index);
    return p;
}
page_t page_manage[20];

/**
 * @descripttion: 建立菜单页
 * @param {*}
 * @return {*}
 */
page_t *lv_page_menu_init(void)
{
    group_button = lv_group_create();
    page_head = add_page(page_head); //建立链表头部
    page_head->start = page_home_start;
    page_head->end = page_home_end;
    static page_t *page2;
    page2 = add_page(page_head);
    page2->start = page_game_snake_start;
    page2->end = page_game_snake_end;
    static page_t *page4;
    page4 = add_page(page_head);
    page4->start = page_music_start;
    page4->end = page_music_end;
    page_manage[Disp_Cam].start = page_cam_start;
    page_manage[Disp_Cam].end = page_cam_end;
    page_manage[Disp_Color].start = page_cam_start;
    page_manage[Disp_Color].end = page_cam_end;
    page_manage[Disp_Face].start = page_cam_start;
    page_manage[Disp_Face].end = page_cam_end;
    page_manage[Disp_Baidu].start = page_baiduai_start;
    page_manage[Disp_Baidu].end = page_baiduai_end;
    page_manage[Disp_IMU].start = page_imu_start;
    page_manage[Disp_IMU].end = page_imu_end;
    page_manage[Disp_FFT].start = page_fft_start;
    page_manage[Disp_FFT].end = page_fft_end;
    page_manage[Disp_Daily].start = page_daily_start;
    page_manage[Disp_Daily].end = page_daily_end;
    page_manage[Disp_Game_2048].start = page_game_2048_start;
    page_manage[Disp_Game_2048].end = page_game_2048_end;
    page_manage[Disp_Game_Snake].start = page_game_snake_start;
    page_manage[Disp_Game_Snake].end = page_game_snake_end;
    page_manage[Disp_Music].start = page_music_start;
    page_manage[Disp_Music].end = page_music_end;
    page_manage[Disp_Calendar].start = page_calendar_start;
    page_manage[Disp_Calendar].end = page_calendar_end;
    page_manage[Disp_Setting].start = page_tab_start;
    page_manage[Disp_Setting].end = page_tab_end;
    page_manage[Disp_About].start = page_about_start;
    page_manage[Disp_About].end = page_about_end;

    return page_head;
}
void save_clock(uint8_t clock_index, uint8_t s, uint8_t h, uint8_t m)
{
    char index[10];
    char info[30];
    if (h > 23 || m > 60 || s > 2)
    {
        ESP_LOGI("CLOCK", "保存时间格式错误1 s:%d,h:%d,m:%d", s, h, m);
        return;
    }
    sprintf(info, "{\"s\":%d,\"h\":%d,\"m\":%d}", s, h, m);
    sprintf(index, "ck%d", clock_index);
    save_nvs((const char *)index, info);
}
void lv_page_clock_init() //闹钟信息获取
{
    //char info[15];
    // if(read_nvs("t_all",info))//获取哔哩哔哩uid
    // {
    //     sprintf(str,"%s%s%s",BILIBILI_REQUEST1,uid,BILIBILI_REQUEST2);
    //     ESP_LOGI(TAG_HTTP,"获取到uid");
    // }
}
void lv_ico_web_show(uint8_t state)
{
    if (state)
    {
        ksdiy_sys_t.ico.lv_web = lv_label_create(cont_head, NULL);
        lv_label_set_recolor(ksdiy_sys_t.ico.lv_web, true);
        lv_obj_set_style_local_text_color(ksdiy_sys_t.ico.lv_web, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
        lv_label_set_text(ksdiy_sys_t.ico.lv_web, LV_SYMBOL_DIRECTORY);
    }
    else
    {
        if (ksdiy_sys_t.ico.lv_web != NULL)
            lv_obj_del(ksdiy_sys_t.ico.lv_web);
    }
}
void lv_ico_clock_show(uint8_t state)
{
    if (state)
    {
        ksdiy_sys_t.ico.lv_clock = lv_label_create(cont_head, NULL);
        lv_obj_set_style_local_text_color(ksdiy_sys_t.ico.lv_clock, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
        lv_label_set_recolor(ksdiy_sys_t.ico.lv_clock, true);
        lv_label_set_text(ksdiy_sys_t.ico.lv_clock, LV_SYMBOL_BELL);
    }
    else
    {
        if (ksdiy_sys_t.ico.lv_clock != NULL)
            lv_obj_del(ksdiy_sys_t.ico.lv_clock);
    }
}
void lv_ico_temp_show(uint8_t state)
{
    if (state)
    {
        ksdiy_sys_t.ico.lv_temp = lv_label_create(cont_head, NULL);
        lv_label_set_recolor(ksdiy_sys_t.ico.lv_temp, true);
        lv_obj_set_style_local_text_color(ksdiy_sys_t.ico.lv_temp, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
        lv_label_set_text(ksdiy_sys_t.ico.lv_temp, "#3CB371 " LV_SYMBOL_REFRESH);
    }
    else
    {
        if (ksdiy_sys_t.ico.lv_temp != NULL)
            lv_obj_del(ksdiy_sys_t.ico.lv_temp);
    }
}
void web_temp_save(uint8_t i)
{
    if (i)
    {
        save_nvs("temp", "y");
        lv_message("温湿度上传开启", 1000);
    }
    else
    {
        save_nvs("temp", "n");
        lv_message("温湿度上传关闭", 1000);
    }
}
void web_switch_save(uint8_t i)
{
    if (i)
    {
        save_nvs("web_set", "y");
        lv_message("WEB开启", 1000);
    }
    else
    {
        save_nvs("web_set", "n");
        lv_message("WEB关闭", 1000);
    }
}
void cam_switch_save(uint8_t i)
{
    if (i)
    {
        save_nvs("cam_set", "y");
        lv_message("CAM WEB开启", 1000);
    }
    else
    {
        save_nvs("cam_set", "n");
        lv_message("CAM WEB关闭", 1000);
    }
}
void cam_vflip_switch_save(uint8_t i)
{
    if (i)
    {
        save_nvs("cam_vflip", "y");
        lv_message("vflip开启", 1000);
    }
    else
    {
        save_nvs("cam_vflip", "n");
        lv_message("vflip关闭", 1000);
    }
}
void cam_hmirror_switch_save(uint8_t i)
{
    if (i)
    {
        save_nvs("cam_hmirror", "y");
        lv_message("hmirror开启", 1000);
    }
    else
    {
        save_nvs("cam_hmirror", "n");
        lv_message("hmirror关闭", 1000);
    }
}
void clock_switch_save(uint8_t i)
{
    if (i)
    {
        save_nvs("clock", "y");
        lv_message("闹钟开启", 1000);
    }
    else
    {
        save_nvs("clock", "n");
        lv_message("闹钟关闭", 1000);
    }
}
void lv_ksdiy_web_init()
{
    char info[5];
    if (read_nvs("web_set", info)) //读取web状态
    {
        if (!strcmp("y", info)) //开启web
        {
            if (start_file_server("/spiffs") != ESP_OK) //启动文件系统
            {
                ksdiy_sys_t.state.web_set = 0;
                ESP_LOGI("web_set", "web_set启动失败");
            }
            ESP_LOGI("web_set", "打开web_set服务");
            ksdiy_sys_t.state.web_set = 1;
        }
        else //关闭
        {
            save_nvs("web_set", "n");
            ksdiy_sys_t.state.web_set = 0;
        }
    }
    else //第一次使用未选择
    {
        ESP_LOGI("web_set", "web_set没启动");
        ksdiy_sys_t.state.web_set = 0;
    }
    if (ksdiy_sys_t.state.web_set) //显示图标
    {
        lv_ico_web_show(1);
    }
}
void lv_ksdiy_cam_init()
{
    // app_camera_init();
    char info[5];
    if (read_nvs("cam_vflip", info)) //读取web状态
    {
        if (!strcmp("y", info)) //开启web
        {

            ksdiy_sys_t.state.cam_vflip = 1;
            sensor_t *s = esp_camera_sensor_get();
            s->set_vflip(s, ksdiy_sys_t.state.cam_vflip);
            ESP_LOGI("web_set", "cam_vflip打开");
        }

        else //关闭
        {
            ksdiy_sys_t.state.cam_vflip = 0;
            sensor_t *s = esp_camera_sensor_get();
            s->set_vflip(s, ksdiy_sys_t.state.cam_vflip);
            ESP_LOGI("web_set", "cam_vflip关闭");
        }
    }
    else //第一次使用未选择
    {
        ESP_LOGI("web_set", "cam_vflip没设置过");
        ksdiy_sys_t.state.cam_vflip = 0;
        ksdiy_sys_t.state.cam_hmirror = 0;
    }
    if (read_nvs("cam_hmirror", info)) //读取web状态
    {
        if (!strcmp("y", info)) //开启web
        {

            ksdiy_sys_t.state.cam_hmirror = 1;
            sensor_t *s = esp_camera_sensor_get();
            s->set_hmirror(s, ksdiy_sys_t.state.cam_hmirror);
            ESP_LOGI("web_set", "cam_hmirror打开");
        }

        else //关闭
        {
            ksdiy_sys_t.state.cam_hmirror = 0;
            sensor_t *s = esp_camera_sensor_get();
            s->set_hmirror(s, ksdiy_sys_t.state.cam_hmirror);
            ESP_LOGI("web_set", "cam_hmirror关闭");
        }
    }

    else //第一次使用未选择
    {
        ESP_LOGI("web_set", "cam_hmirror没设置过");
        ksdiy_sys_t.state.cam_vflip = 0;
        ksdiy_sys_t.state.cam_hmirror = 0;
    }
    // esp_camera_deinit();
}
void ksdiy_sys_info_init()
{
    ksdiy_sys_t.page = lv_page_menu_init(); //建立菜单页
    ksdiy_sys_t.state.sys_button = 0;
    ksdiy_sys_t.user_data = 0;
    ksdiy_sys_t.wp.follow = get_value_ksdiyfs(1);
    ksdiy_sys_t.wp.following = get_value_ksdiyfs(0);
    lv_ksdiy_web_init();
    // lv_ksdiy_cam_init();
    // app_menu_create();
}