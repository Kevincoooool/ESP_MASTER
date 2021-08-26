/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-05 09:04:44
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-21 18:34:01
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_home.c
 */
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "lv_compment.h"
#include "esp_log.h"
#include "lv_port_indev.h"
#include "esp_err.h"
#include "gui_time.h"
#include "http.h"
#include "page_home.h"
#include "page_start.h"
#include "myFont.h"
#include "number_temp.h"
#include "number.h"
#include "myimg.h"
#include "weather.h"
#include "button.h"
#include "lv_gif.h"
LV_IMG_DECLARE(taikong001);
LV_IMG_DECLARE(taikong002);
LV_IMG_DECLARE(taikong003);
LV_IMG_DECLARE(taikong004);
LV_IMG_DECLARE(taikong005);
LV_IMG_DECLARE(taikong006);
LV_IMG_DECLARE(taikong007);
LV_IMG_DECLARE(taikong008);
LV_IMG_DECLARE(taikong009);
LV_IMG_DECLARE(taikong010);

extern struct DATA_WEATHER weather_data[3];
extern const struct LV_WEATHER lv_weather[40];
extern struct _ksdiy_sys_t ksdiy_sys_t;
#define ANIEND                      \
    while (lv_anim_count_running()) \
        lv_task_handler();
LV_IMG_DECLARE(bili_half);
LV_IMG_DECLARE(bili_half);
LV_IMG_DECLARE(kevincoooool);
LV_FONT_DECLARE(myFont);
LV_FONT_DECLARE(number);
LV_FONT_DECLARE(number_temp);

/*此页面窗口*/
static lv_obj_t *appWindow;

extern lv_obj_t *scr;
extern lv_obj_t *scr_body;
extern lv_obj_t *cont_head;

static lv_obj_t *img_weather;

lv_style_t style1;
/*****主体****/
lv_obj_t *cont_time, *cont_temp, *cont_fensi, *img_anim;
/*****时间****/
lv_obj_t *label_time1, *label_time1_shi, *label_time1_fen;
lv_obj_t *label_time2;
lv_task_t *task_time, *task_anim;
extern compment_t c22, c33, c44, canim;
/*****粉丝****/
lv_obj_t *label_fensi;
lv_obj_t *label_guanzhu;
/*****温湿度****/
lv_obj_t *cont_temp_number;
lv_obj_t *label_temp_number;
compment_t pcf, pct, pcth, panim; //动画结构体

static void update_weather(void);
void page_task_time(lv_task_t *task)
{
    static uint8_t i = 0, h = 0;
    if (i % 2 == 0)
        lv_obj_set_hidden(label_time1, 1);
    else
        lv_obj_set_hidden(label_time1, 0);

    if (i % 20 == 0) //10s
    {
        update_temp(); //更新温湿度
        update_time(); //更新时间
    }
    // if (i % 500 == 0) //十分钟
    //     read_fans();
    // if (i % (1000) == 0) //12小时
    // {
    //     read_weather();
    // }
    if (i % 120 == 0) //一分钟
    {
        i = 0;
        h++;
        if (h % 3 == 0) //3分钟，更新粉丝数
            update_fensi();
        if (h == h * 60 * 8) //8小时
        {
            update_weather(); //更新天气
            h = 0;
        }
    }
    i++;
}
void update_time()
{
    char now_time[40];
    sprintf((char *)now_time, "#FFFFFF %02d#", ksdiy_sys_t.timeinfo.tm_hour);
    lv_label_set_text(label_time1_shi, (const char *)now_time);
    sprintf((char *)now_time, "#FFFFFF %02d#", ksdiy_sys_t.timeinfo.tm_min);
    lv_label_set_text(label_time1_fen, (const char *)now_time);
    //星期与日历
    switch (ksdiy_sys_t.timeinfo.tm_wday)
    {
    case 0:
        sprintf((char *)now_time, "星期日\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
    case 1:
        sprintf((char *)now_time, "星期一\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
    case 2:
        sprintf((char *)now_time, "星期二\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
    case 3:
        sprintf((char *)now_time, "星期三\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
    case 4:
        sprintf((char *)now_time, "星期四\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
    case 5:
        sprintf((char *)now_time, "星期五\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
    case 6:
        sprintf((char *)now_time, "星期六\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;

    default:
        sprintf((char *)now_time, "星期日\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
        break;
    }
    lv_label_set_text(label_time2, (const char *)now_time);
}
void page_home_time(compment_t *comp)
{
    cont_time = lv_cont_create(scr_body, NULL);
    lv_cont_set_fit2(cont_time, LV_FIT_NONE, LV_FIT_NONE);
    lv_obj_set_size(cont_time, 130, 130);
    lv_obj_set_pos(cont_time, 0, 10);
    lv_obj_add_style(cont_time, LV_BTN_PART_MAIN, &style1); /*Default button style*/
    //lv_cont_set_layout(cont_time, LV_LAYOUT_CENTER);
    //小时
    label_time1_shi = lv_label_create(cont_time, NULL);
    lv_label_set_recolor(label_time1_shi, true);
    lv_obj_set_pos(label_time1_shi, 2, 0);
    lv_label_set_align(label_time1_shi, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_obj_set_style_local_text_font(label_time1_shi, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &number);

    //点
    label_time1 = lv_label_create(cont_time, NULL);
    lv_label_set_recolor(label_time1, true);
    lv_obj_set_pos(label_time1, 58, -5);
    lv_label_set_align(label_time1, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_obj_set_style_local_text_font(label_time1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &number);
    lv_label_set_text(label_time1, "#FFA500 :#");
    //分钟
    label_time1_fen = lv_label_create(cont_time, NULL);
    lv_label_set_recolor(label_time1_fen, true);
    lv_obj_set_pos(label_time1_fen, 68, 0);
    lv_label_set_align(label_time1_fen, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_obj_set_style_local_text_font(label_time1_fen, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &number);

    //星期及日期
    label_time2 = lv_label_create(cont_time, NULL);
    lv_label_set_recolor(label_time2, true);
    lv_obj_set_pos(label_time2, 0, 70);
    lv_label_set_align(label_time2, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_obj_set_style_local_text_font(label_time2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
    lv_obj_set_style_local_text_color(label_time2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    update_time(); //更新数据
    //动画
    comp->start = -120;
    comp->end = 0;
    comp->obj = cont_time;
    comp->start_t = 500;
    comp->end_t = 500;
    comp->cb = lv_anim_path_ease_out;
    comp->funcb = (void *)lv_obj_set_x;
    comp->next = NULL;
}
lv_obj_t *img_anim;
extern const uint8_t taikongren_map[]; 
void page_home_anim(compment_t *comp)
{

    img_anim = lv_img_create(scr_body, NULL);
    //Write style LV_IMG_PART_MAIN for WIN_img0
    static lv_style_t style_img;
    lv_style_init(&style_img);

    //Write style state: LV_STATE_DEFAULT for style_img
    lv_style_set_image_recolor(&style_img, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_image_recolor_opa(&style_img, LV_STATE_DEFAULT, 0);
    lv_style_set_image_opa(&style_img, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(img_anim, LV_IMG_PART_MAIN, &style_img);
    lv_obj_set_pos(img_anim, 70, 60);
    lv_img_set_src(img_anim, &taikong001);


    // img_anim = lv_gif_create_from_data(scr_body, taikongren_map);
	// lv_obj_set_size(img_anim, 100, 100);
	// lv_obj_set_pos(img_anim, 70, 50);
    
    //动画
    comp->start = 240;
    comp->end = 70;
    comp->obj = img_anim;
    comp->start_t = 500;
    comp->end_t = 500;
    comp->cb = lv_anim_path_ease_out;
    comp->funcb = (void *)lv_obj_set_y;
    comp->next = NULL;
}
void update_fensi()
{
    char now_time[40];
    sprintf((char *)now_time, "粉丝:%d\n关注:%d", ksdiy_sys_t.wp.follow, ksdiy_sys_t.wp.following);
    lv_label_set_text(label_fensi, (const char *)now_time);
}
void page_home_fensi(compment_t *comp)
{

    cont_fensi = lv_cont_create(scr_body, NULL);
    lv_cont_set_fit2(cont_fensi, LV_FIT_NONE, LV_FIT_NONE);
    lv_obj_set_size(cont_fensi, 100, 48);
    lv_obj_set_pos(cont_fensi, 70, 240 - 80);
    lv_obj_add_style(cont_fensi, LV_BTN_PART_MAIN, &style1); /*Default button style*/
    lv_cont_set_layout(cont_fensi, LV_LAYOUT_CENTER);
    //主题
    static lv_style_t label_fensi_style;
    lv_style_init(&label_fensi_style);
    lv_style_set_bg_opa(&label_fensi_style, LV_STATE_DEFAULT, 0);
    lv_style_set_text_opa(&label_fensi_style, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_text_color(&label_fensi_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    //粉丝
    label_fensi = lv_label_create(cont_fensi, NULL);
    lv_label_set_recolor(label_fensi, true);
    lv_label_set_align(label_fensi, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_obj_set_style_local_text_font(label_fensi, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
    lv_obj_add_style(label_fensi, LV_LABEL_PART_MAIN, &label_fensi_style);
    update_fensi();

    //动画
    comp->start = 240;
    comp->end = 240 - 80;
    comp->obj = cont_fensi;
    comp->start_t = 500;
    comp->end_t = 500;
    comp->cb = lv_anim_path_ease_out;
    comp->funcb = (void *)lv_obj_set_y;
    comp->next = NULL;
}
void update_temp()
{
    //更新温湿度
    char now_time[40];
    sprintf(now_time, "△%d℃\n▽%d℃", weather_data[0].high, weather_data[0].low);
    // printf("now temp:\n%s\n", now_time);
    lv_label_set_text(label_temp_number, (const char *)now_time);
}
static void update_weather()
{
    //天气图片
    // char file[30];
    // uint8_t i = weather_data[0].code_day;
    ESP_LOGI("WEATHER", "更新天气");

    // lv_img_dsc_t *p = (lv_img_dsc_t *)lv_weather[i].img_dsc;
    // static lv_obj_t *imgtq;
    // imgtq = lv_img_create(cont_temp, NULL);

    lv_img_set_src(img_weather, lv_weather[weather_data[0].code_day].img_dsc);
}
void page_home_temp(compment_t *comp)
{

    //主cont体
    cont_temp = lv_cont_create(scr_body, NULL);
    lv_cont_set_fit2(cont_temp, LV_FIT_NONE, LV_FIT_NONE);
    lv_obj_set_size(cont_temp, 110, 50);
    lv_obj_set_pos(cont_temp, 130, 15);
    lv_obj_add_style(cont_temp, LV_BTN_PART_MAIN, &style1); /*Default button style*/

    img_weather = lv_img_create(cont_temp, NULL);
    uint8_t i = weather_data[0].code_day;
    lv_img_dsc_t *p = (lv_img_dsc_t *)lv_weather[i].img_dsc;
    lv_img_set_src(img_weather, p);
    lv_obj_set_pos(img_weather, 60, 0);
    //风格
    static lv_style_t label_temp_style;
    lv_style_init(&label_temp_style);
    lv_style_set_text_opa(&label_temp_style, LV_STATE_DEFAULT, 255);
    lv_style_set_text_color(&label_temp_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    //百分比
    label_temp_number = lv_label_create(cont_temp, NULL);
    // lv_obj_set_size(label_temp_number, 70, 50);
    // lv_obj_set_pos(label_temp_number, 130, 15);
    lv_obj_set_size(label_temp_number, 60, 50);
    lv_obj_set_pos(label_temp_number, 8, 0);
    lv_label_set_align(label_temp_number, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_obj_set_style_local_text_font(label_temp_number, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
    lv_obj_add_style(label_temp_number, LV_LABEL_PART_MAIN, &label_temp_style);

    update_temp();
    update_weather();
    //加载动画
    comp->start = 240;
    comp->end = 130;
    comp->obj = cont_temp;
    comp->start_t = 500;
    comp->end_t = 500;
    comp->cb = lv_anim_path_ease_out;
    comp->funcb = (void *)lv_obj_set_x;
    comp->next = NULL;
}

void page_home_ksdiy()
{

    static lv_obj_t *img22;
    img22 = lv_img_create(scr_body, NULL);
    lv_img_set_src(img22, &bili_half);
    lv_obj_set_pos(img22, -120, 240 - 80);

    static lv_obj_t *img33;
    img33 = lv_img_create(scr_body, NULL);
    lv_img_set_src(img33, &bili_half);
    lv_obj_set_pos(img33, 240, 240 - 80);

    c22.start = -120;
    c22.end = 10;
    c22.obj = img22;
    c22.start_t = 500;
    c22.end_t = 500;
    c22.cb = lv_anim_path_ease_out;
    c22.funcb = (void *)lv_obj_set_x;
    c22.next = NULL;

    c33.start = 240;
    c33.end = 180;
    c33.obj = img33;
    c33.start_t = 500;
    c33.end_t = 500;
    c33.cb = lv_anim_path_ease_out;
    c33.funcb = (void *)lv_obj_set_x;
    c33.next = NULL;
}


void page_home_load()
{

    lv_style_set_bg_color(&style1, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_color(&style1, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_width(&style1, LV_STATE_DEFAULT, 0);
    lv_style_set_border_opa(&style1, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_opa(&style1, LV_STATE_DEFAULT, 0);
    page_home_ksdiy();
    page_home_temp(&pcth);
    page_home_fensi(&pcf);
    page_home_time(&pct);
    page_home_anim(&panim);

    lv_add_compment(&c22, &c33);
    lv_add_compment(&c33, &c44);
    lv_add_compment(&c44, &pcf);
    lv_add_compment(&pcf, &pct);
    lv_add_compment(&pct, &pcth);
    lv_add_compment(&pcth, &panim);

    obj_move(&c22, 1);
    ANIEND
}
void page_home_end()
{
    lv_task_del(task_time);
    lv_task_del(task_anim);
    lv_task_handler();
    obj_move(&c22, 0);

    ANIEND
    lv_obj_clean(scr_body); //清空页面scr_body

    ESP_LOGI("SYSTEM", "esp_get_free_heap_size : %d  ", esp_get_free_heap_size());
}

void page_anim_time(lv_task_t *task)
{
    static uint8_t i = 1;
    switch (i)
    {
    case 1:
        lv_img_set_src(img_anim, &taikong001);
        break;
    case 2:
        lv_img_set_src(img_anim, &taikong002);
        break;
    case 3:
        lv_img_set_src(img_anim, &taikong003);
        break;
    case 4:
        lv_img_set_src(img_anim, &taikong004);
        break;
    case 5:
        lv_img_set_src(img_anim, &taikong005);
        break;
    case 6:
        lv_img_set_src(img_anim, &taikong006);
        break;
    case 7:
        lv_img_set_src(img_anim, &taikong007);
        break;
    case 8:
        lv_img_set_src(img_anim, &taikong008);
        break;
    case 9:
        lv_img_set_src(img_anim, &taikong009);
        break;
    case 10:
        lv_img_set_src(img_anim, &taikong010);
        break;
    default:
        break;
    }

    if (i == 10)
        i = 1;
    i++;
}
void page_home_start()
{
    page_home_load();
    task_time = lv_task_create(page_task_time, 500, LV_TASK_PRIO_MID, NULL); //创建任务
    task_anim = lv_task_create(page_anim_time, 100, LV_TASK_PRIO_MID, NULL); //创建任务

    ESP_LOGI("SYSTEM", "esp_get_free_heap_size : %d ", esp_get_free_heap_size());
}
extern struct _ksdiy_sys_t ksdiy_sys_t;
void move_task_home(uint8_t move)
{
    switch (move)
    {

    case BT1_DOWN: //往上移动
        if (ksdiy_sys_t.page->last != NULL && ksdiy_sys_t.state.sys_button == 0)
        {
            ksdiy_sys_t.page->end();
            ksdiy_sys_t.page = ksdiy_sys_t.page->last;
            ESP_LOGI("SYSTEM", "界面%d", ksdiy_sys_t.page->index);
            ksdiy_sys_t.page->start();
        }
        else
        {
            ESP_LOGI("SYSTEM", "已到第一个界面尽头");
            return;
        }
        break;
    case BT1_DOUBLE: //往上移动

        break;
    case BT1_LONG: //往上移动

        break;
    case BT1_LONGFREE: //往上移动

        break;
    case BT2_DOWN:

        break;
    case BT3_LONG: //往下移动

        break;
    case BT3_LONGFREE: //往上移动

        break;

    case BT3_DOWN: //往下移动
        if (ksdiy_sys_t.page->next != NULL && ksdiy_sys_t.state.sys_button == 0)
        {
            ksdiy_sys_t.page->end();
            ESP_LOGI("SYSTEM", "切换菜单");
            ESP_LOGI("SYSTEM", "界面%d", ksdiy_sys_t.page->index);
            ksdiy_sys_t.page = ksdiy_sys_t.page->next;
            ksdiy_sys_t.page->start();
        }
        else
        {
            ESP_LOGI("SYSTEM", "已到最后界面尽头");
            return;
        }
        break;

    default:
        break;
    }
}