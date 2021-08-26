/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-17 15:03:18
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_music.c
 */
#include "page_music.h"
#include "page_start.h"
#include "page_menu.h"
#include "app_main.h"
#include "button.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "lv_compment.h"
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "lv_port_indev.h"

#include "lv_examples/src/lv_demo_music/lv_demo_music.h"
#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"

#include "lv_gif.h"
#define ANIEND                      \
	while (lv_anim_count_running()) \
		lv_task_handler(); //等待动画完成
/*此页面窗口*/
static lv_obj_t *appWindow;


/**
 * @descripttion: 创建标题栏
 * @param {*}
 * @return {*}
 */
static void Title_Create()
{
	LV_FONT_DECLARE(number);
	appWindow = lv_cont_create(lv_scr_act(), NULL);
	static lv_style_t style_cont;
	lv_style_set_pad_left(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(&style_cont, LV_STATE_DEFAULT, 255);
	lv_style_set_pad_top(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_width(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_opa(&style_cont, LV_STATE_DEFAULT, 255);
	lv_style_set_bg_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK); //设置屏幕背景
	lv_obj_add_style(appWindow, LV_BTN_PART_MAIN, &style_cont);			  /*Default button style*/
	lv_obj_set_pos(appWindow, 0, 0);
	lv_obj_set_size(appWindow, 240, 240);

	
}
compment_t music_music;
void page_music_load()
{
	Title_Create();

	obj_add_anim(
		appWindow,								  //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //动画函数
		lv_anim_speed_to_time(300, 0, 50), //动画速度
		240,					  //起始值
		0,  //结束值
		lv_anim_path_linear						  //动画特效:模拟弹性物体下落
	);

	ANIEND
	my_demo_music_start(appWindow);
}
void page_music_end(void)
{
	my_demo_music_end(appWindow);
	obj_add_anim(
		appWindow,								  //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //动画函数
		lv_anim_speed_to_time(300, 0, 50), //动画速度
		0,					  //起始值
		240,  //结束值
		lv_anim_path_linear						  //动画特效:模拟弹性物体下落
	);
	ANIEND
	lv_obj_del(appWindow);
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
}

void page_music_start(void)
{
	//获取芯片可用内存
	printf(" page_cam_start    esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//获取从未使用过的最小内存
	printf(" page_cam_start    esp_get_minmusicm_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	page_music_load();
}
