/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-17 15:05:09
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_calendar.c
 */
#include "page_calendar.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "lv_compment.h"
#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"
#include "button.h"
#include "page_start.h"
#define ANIEND                      \
    while (lv_anim_count_running()) \
        lv_task_handler(); //等待动画完成
extern lv_obj_t *scr;
extern lv_obj_t *scr_body;
lv_obj_t *calendar;
extern lv_group_t *group_button;
extern struct _ksdiy_sys_t ksdiy_sys_t;
void ksdiy_calendar(void)
{
    calendar = lv_calendar_create(lv_scr_act(), NULL);
    lv_obj_set_size(calendar, 240, 240);
    lv_obj_align(calendar, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_pad_all(calendar, LV_CALENDAR_PART_HEADER, LV_STATE_DEFAULT, LV_DPI / 32);
    lv_obj_set_style_local_pad_bottom(calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STATE_DEFAULT, LV_DPI / 32);
    lv_obj_set_style_local_outline_pad(calendar, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, LV_DPI / 32);
    lv_obj_set_style_local_pad_all(calendar, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_text_font(calendar, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, lv_theme_get_font_small());

    /*Set today's date*/
    lv_calendar_date_t today;

    today.year = ksdiy_sys_t.timeinfo.tm_year+1900;
    today.month = ksdiy_sys_t.timeinfo.tm_mon+1;
    today.day = ksdiy_sys_t.timeinfo.tm_mday;
    lv_calendar_set_today_date(calendar, &today);
    lv_calendar_set_showed_date(calendar, &today);
    // group_button=lv_group_create();
    lv_group_remove_all_objs(group_button);
    lv_group_add_obj(group_button, calendar); //加入解码组
    // // lv_group_add_obj(group_button, tabview);//加入解码组 
    lv_button_set_group(group_button);
    // lv_button_set_group(group_button);
}

void page_calendar_load()
{
    ksdiy_calendar();
    obj_add_anim(
		calendar,								  //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //动画函数
		lv_anim_speed_to_time(300, 0, 50), //动画速度
		240,					  //起始值
		0,  //结束值
		lv_anim_path_linear						  //动画特效:模拟弹性物体下落
	);
   
    ANIEND
}
void page_calendar_end()
{
    obj_add_anim(
		calendar,								  //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //动画函数
		lv_anim_speed_to_time(300, 0, 50), //动画速度
		0,					  //起始值
		240,  //结束值
		lv_anim_path_linear						  //动画特效:模拟弹性物体下落
	);
    ANIEND
    lv_obj_del(calendar);
}
void page_calendar_start()
{
    //获取芯片可用内存
    printf("     esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
    //获取从未使用过的最小内存
    printf("     esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    page_calendar_load();
}
void move_task_calendar(uint8_t move)
{

    switch (move)
    {

    case BT1_LONG: //往上移动

        break;
    case BT1_LONGFREE: //往上移动

        break;
    case BT3_LONG: //往下移动

        break;
    case BT3_LONGFREE: //往上移动


        break;
    case BT1_DOWN: //往上移动
        encoder_handler(2);
        break;
    case BT3_DOWN: //往下移动
        encoder_handler(3);
        break;
    default:
        break;
    }
}