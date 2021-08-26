/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-02 09:59:37
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-07 16:26:13
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_menu.c
 */
#include "page_menu.h"
#include "page_start.h"
#include "page_cam.h"
#include "page_calendar.h"
#include "page_music.h"
#include "button.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "lv_compment.h"
#include "lvgl/lvgl.h"
#include "app_anim.h"
#include "lvgl_helpers.h"
#include "lv_port_indev.h"

#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"
#define ANIEND                      \
	while (lv_anim_count_running()) \
		lv_task_handler();
static void ICON_Grp_Move(int8_t dir);

/*此页面窗口*/
static lv_obj_t *appWindow;
/*标题栏*/
static lv_obj_t *labelTitle;
/*标题栏分隔线*/
static lv_obj_t *lineTitle;
/*图标显示容器，用于裁剪显示*/
static lv_obj_t *contDisp;
/*图标组容器*/
static lv_obj_t *contICON;
/*菜单切换速度*/
static uint16_t anim_speed = 200;
/*选中图标目标放大值*/
static uint16_t anim_target_zoom = 360;
/*选中图标目标y轴坐标*/
static uint16_t anim_target_y = 70;
/*菜单切换速度*/
static uint16_t anim_start_y = 130;
/*图标间隔*/
static const uint8_t ICON_IntervalPixel = 40;
/*图标大小*/
static const uint8_t ICON_Size = 60;
/*当前选中的图标索引*/
static int8_t ICON_NowSelIndex = 0;
/*每个图标的修正值*/
static int8_t ICON_Offset = 30;
/*图标索引最大值*/
#define ICON_MAX_INDEX (__Sizeof(ICON_Grp) - 1)

//显示图片声明
LV_IMG_DECLARE(imgcam);
LV_IMG_DECLARE(imgcolor);
LV_IMG_DECLARE(imgface);
LV_IMG_DECLARE(imgcloud);
LV_IMG_DECLARE(imgimu);
LV_IMG_DECLARE(imgspectrum);
LV_IMG_DECLARE(imgheart);
LV_IMG_DECLARE(img2048);
LV_IMG_DECLARE(imgsnake);
LV_IMG_DECLARE(imgmusic);
LV_IMG_DECLARE(imgcalendar);
LV_IMG_DECLARE(imgset);
LV_IMG_DECLARE(imgabout);
static ICON_TypeDef ICON_Grp[] =
	{
		{.img_buffer = &imgcam, .text = "Camera"},
		{.img_buffer = &imgcolor, .text = "Color"},
		{.img_buffer = &imgface, .text = "Face"},
		{.img_buffer = &imgcloud, .text = "Baidu AI"},
		{.img_buffer = &imgimu, .text = "IMU"},
		{.img_buffer = &imgspectrum, .text = "Spectrum"},
		{.img_buffer = &imgheart, .text = "Daily"},
		{.img_buffer = &img2048, .text = "2048"},
		{.img_buffer = &imgsnake, .text = "Snake"},
		{.img_buffer = &imgmusic, .text = "Music"},
		{.img_buffer = &imgcalendar, .text = "Calendar"},
		{.img_buffer = &imgset, .text = "Setting"},
		{.img_buffer = &imgabout, .text = "About"},
};

//当前界面是菜单时MOVE任务要做的事情
void move_task_menu(uint8_t dir)
{

	switch (dir)
	{
	case BT1_DOWN: //往上移动
		ICON_Grp_Move(-1);
		break;
	case BT1_LONG: //往上移动
		ICON_Grp_Move(-1);
		break;
	case BT1_LONGFREE: //往上移动
		break;
	case BT3_DOWN: //往下移动
		ICON_Grp_Move(1);
		break;
	case BT3_LONG: //往下移动
		ICON_Grp_Move(1);
		break;
	case BT3_LONGFREE: //往上移动
		break;

	default:
		break;
	}
}

/**
  * @brief  创建标题栏
  * @param  无
  * @retval 无
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

	labelTitle = lv_label_create(appWindow, NULL);

	static lv_style_t style_label;
	lv_style_init(&style_label);

	//Write style state: LV_STATE_DEFAULT for style_label
	lv_style_set_radius(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_dir(&style_label, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_label, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_text_font(labelTitle, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &number);
	lv_style_set_text_letter_space(&style_label, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_label, LV_STATE_DEFAULT, 0);

	lv_obj_add_style(labelTitle, LV_LABEL_PART_MAIN, &style_label);
	lv_label_set_long_mode(labelTitle, LV_LABEL_LONG_BREAK);
	lv_label_set_align(labelTitle, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_pos(labelTitle, 0, 0);
	lv_obj_set_size(labelTitle, 240, 60);
	lv_label_set_recolor(labelTitle, true);

	/*默认选中的是第二个图标*/
	lv_label_set_static_text(labelTitle, ICON_Grp[0].text);
	lv_obj_align(labelTitle, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
	lv_obj_set_auto_realign(labelTitle, true);

	lineTitle = lv_line_create(appWindow, NULL);
	static lv_style_t style_line;
	lv_style_init(&style_line);
	lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, lv_color_make(0xFa, 0x05, 0x05));
	lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 5);
	lv_obj_add_style(lineTitle, LV_LINE_PART_MAIN, &style_line);
	lv_obj_set_pos(lineTitle, 0, 55);
	lv_obj_set_size(lineTitle, 240, 5);
	static lv_point_t screen_line3[] = {{0, 0}, {240, 0}};
	lv_line_set_points(lineTitle, screen_line3, 2);
}
extern lv_style_t style1;
//创建菜单界面
static void Cont_create(void)
{
	uint8_t i;
	contDisp = lv_cont_create(appWindow, NULL);
	static lv_style_t style_cont;
	lv_style_set_pad_left(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(&style_cont, LV_STATE_DEFAULT, 255);
	lv_style_set_pad_top(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_width(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_opa(&style_cont, LV_STATE_DEFAULT, 255);
	lv_style_set_bg_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK); //设置屏幕背景
	lv_obj_add_style(contDisp, LV_BTN_PART_MAIN, &style_cont);			  /*Default button style*/
	lv_obj_set_size(contDisp, 240, 180);
	lv_obj_set_pos(contDisp, 0, 60);

	contICON = lv_cont_create(contDisp, NULL);
	static lv_style_t style_cont1;
	lv_style_set_pad_left(&style_cont1, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(&style_cont1, LV_STATE_DEFAULT, 255);
	lv_style_set_pad_top(&style_cont1, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&style_cont1, LV_STATE_DEFAULT, LV_COLOR_RED);
	lv_style_set_border_width(&style_cont1, LV_STATE_DEFAULT, 0);
	lv_style_set_border_opa(&style_cont1, LV_STATE_DEFAULT, 255);
	lv_style_set_bg_color(&style_cont1, LV_STATE_DEFAULT, LV_COLOR_RED); //设置屏幕背景

	lv_obj_add_style(contICON, LV_BTN_PART_MAIN, &style_cont);
	lv_obj_set_size(contICON, (ICON_Size + ICON_IntervalPixel) * __Sizeof(ICON_Grp) + 50, lv_obj_get_height(contDisp));
	lv_obj_set_x(contICON, lv_obj_get_width(contDisp));

	for (i = 0; i < __Sizeof(ICON_Grp); i++)
	{
		ICON_Grp[i].img = lv_img_create(contICON, NULL);			//创建img显示菜单
		lv_img_set_src(ICON_Grp[i].img, ICON_Grp[i].img_buffer);	//设置显示图片
		lv_obj_align(ICON_Grp[i].img, NULL, LV_ALIGN_CENTER, 0, 0); //设置图片显示顶端
		/*计算偏移量*/
		lv_obj_set_x(ICON_Grp[i].img, (ICON_Size + ICON_IntervalPixel) * i + ICON_Offset);
	}
}
/**
  * @brief  创建滚轮阴影
  * @param  无
  * @retval 无
  */
static void ImgShadow_Create()
{
	LV_IMG_DECLARE(ImgShadowUp);
	LV_IMG_DECLARE(ImgShadowDown);

	lv_obj_t *imgUp = lv_img_create(contDisp, NULL);
	lv_img_set_src(imgUp, &ImgShadowUp);
	lv_obj_align(imgUp, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

	lv_obj_t *imgDown = lv_img_create(contDisp, NULL);
	lv_img_set_src(imgDown, &ImgShadowDown);
	lv_obj_align(imgDown, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
}

/**
  * @brief  移动到选中图标
  * @param  iconIndex:目标图标的索引
  * @retval 无
  */
static void ICON_Grp_MoveFocus(uint8_t iconIndex)
{
	if (iconIndex > ICON_MAX_INDEX)
		return;

	/*改变标题栏的文字说明*/
	lv_label_set_static_text(labelTitle, ICON_Grp[iconIndex].text);

	/*计算目标Y坐标*/
	int16_t target_x = -(ICON_Size + ICON_IntervalPixel) * (iconIndex - 1) - ICON_Offset - 10;
	/*执行滑动动画*/
	obj_add_anim(
		contICON,								  //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //动画函数
		lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
		lv_obj_get_x(contICON),					  //起始值
		target_x,								  //结束值
		lv_anim_path_linear						  //动画特效:模拟弹性物体下落
	);
	if (iconIndex == 0)
	{
		obj_add_anim(
			ICON_Grp[iconIndex].img,				  //动画对象
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			LV_IMG_ZOOM_NONE / 1.5,					  //起始值
			anim_target_zoom,						  //结束值
			lv_anim_path_linear				  //动画特效:模拟弹性物体下落
		);
		obj_add_anim(
			ICON_Grp[iconIndex].img,				  //动画对象
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			anim_start_y,							  //起始值
			anim_target_y,							  //结束值
			lv_anim_path_linear						  //动画特效:模拟弹性物体下落
		);
		obj_add_anim(
			ICON_Grp[iconIndex + 1].img,			  //动画对象
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			anim_target_zoom,						  //起始值
			LV_IMG_ZOOM_NONE / 1.5,					  //结束值
			lv_anim_path_linear						  //动画特效:模拟弹性物体下落
		);
		obj_add_anim(
			ICON_Grp[iconIndex + 1].img,			  //动画对象
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			anim_target_y,							  //起始值
			anim_start_y,							  //结束值
			lv_anim_path_linear						  //动画特效:模拟弹性物体下落
		);
	}
	else if (iconIndex >= 1 && iconIndex < ICON_MAX_INDEX)
	{
		obj_add_anim(
			ICON_Grp[iconIndex].img,				  //动画对象
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			LV_IMG_ZOOM_NONE / 1.5,					  //起始值
			anim_target_zoom,						  //结束值
			lv_anim_path_linear				  //动画特效:模拟弹性物体下落
		);
		obj_add_anim(
			ICON_Grp[iconIndex].img,				  //动画对象
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			anim_start_y,							  //起始值
			anim_target_y,							  //结束值
			lv_anim_path_linear						  //动画特效:模拟弹性物体下落
		);
		obj_add_anim(
			ICON_Grp[iconIndex - 1].img,			  //动画对象
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			anim_target_zoom,						  //起始值
			LV_IMG_ZOOM_NONE / 1.5,					  //结束值
			lv_anim_path_linear						  //动画特效:模拟弹性物体下落
		);
		obj_add_anim(
			ICON_Grp[iconIndex - 1].img,			  //动画对象
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			anim_target_y,							  //起始值
			anim_start_y,							  //结束值
			lv_anim_path_linear						  //动画特效:模拟弹性物体下落
		);
		
		obj_add_anim(
			ICON_Grp[iconIndex + 1].img,			  //动画对象
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			anim_target_zoom,						  //起始值
			LV_IMG_ZOOM_NONE / 1.5,					  //结束值
			lv_anim_path_linear						  //动画特效:模拟弹性物体下落
		);
		obj_add_anim(
			ICON_Grp[iconIndex + 1].img,			  //动画对象
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			anim_target_y,							  //起始值
			anim_start_y,							  //结束值
			lv_anim_path_linear						  //动画特效:模拟弹性物体下落
		);
	}
	else if (iconIndex == ICON_MAX_INDEX)
	{
		obj_add_anim(
			ICON_Grp[iconIndex - 1].img,			  //动画对象
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			anim_target_zoom,						  //起始值
			LV_IMG_ZOOM_NONE / 1.5,					  //结束值
			lv_anim_path_linear						  //动画特效:模拟弹性物体下落
		);
		obj_add_anim(
			ICON_Grp[iconIndex - 1].img,			  //动画对象
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			anim_target_y,							  //起始值
			anim_start_y,							  //结束值
			lv_anim_path_linear						  //动画特效:模拟弹性物体下落
		);
		obj_add_anim(
			ICON_Grp[iconIndex].img,				  //动画对象
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			LV_IMG_ZOOM_NONE / 1.5,					  //起始值
			anim_target_zoom,						  //结束值
			lv_anim_path_linear				  //动画特效:模拟弹性物体下落
		);
		obj_add_anim(
			ICON_Grp[iconIndex].img,				  //动画对象
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //动画函数
			lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
			anim_start_y,							  //起始值
			anim_target_y,							  //结束值
			lv_anim_path_linear						  //动画特效:模拟弹性物体下落
		);
	}

}

/**
  * @brief  上下移动选中的图标
  * @param  dir:方向
  * @retval 无
  */
static void ICON_Grp_Move(int8_t dir)
{
	/*在限定范围内移动*/
	__ValuePlus(ICON_NowSelIndex, dir, 0, ICON_MAX_INDEX);

	/*移动到新图标*/
	ICON_Grp_MoveFocus(ICON_NowSelIndex);
}

compment_t menu_anim;
void page_menu_load()
{
	Title_Create();
	Cont_create();
	// ImgShadow_Create();

	obj_add_anim(
		appWindow,						  //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x, //动画函数
		300,							  //动画速度
		-240,							  //起始值
		0,								  //结束值
		lv_anim_path_linear				  //动画特效:模拟弹性物体下落
	);

	ANIEND
	/*图标滑到上一次选中的图标*/
	ICON_Grp_MoveFocus(ICON_NowSelIndex);
}
lv_task_t *task_menu;
void page_menu_end()
{
	/*图标全部滑出*/
	obj_add_anim(
		contICON,								  //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_y,		  //动画函数
		lv_anim_speed_to_time(anim_speed, 0, 50), //动画速度
		lv_obj_get_y(contICON),					  //起始值
		lv_obj_get_height(contDisp) + ICON_Size,  //结束值
		lv_anim_path_linear						  //动画特效:模拟弹性物体下落
	);
	ANIEND
	obj_add_anim(
		appWindow,						  //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x, //动画函数
		300,							  //动画速度
		0,								  //起始值
		-240,							  //结束值
		lv_anim_path_linear				  //动画特效:模拟弹性物体下落
	);
	ANIEND
	lv_obj_del(appWindow);
}

void page_menu_start()
{
	//获取芯片可用内存
	printf("page_menu_start     esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//获取从未使用过的最小内存
	printf("page_menu_start     esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	page_menu_load();
}
uint8_t Menu_Choose(void)
{
	return ICON_NowSelIndex;
}