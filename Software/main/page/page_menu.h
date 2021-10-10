/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-02 09:59:54
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-09-18 10:40:29
 * @FilePath     : \esp-idf\pro\ESP_MASTER\Software\main\page\page_menu.h
 */
/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-02 09:59:37
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-02 09:59:37
 * @FilePath     : \esp-idf\KSDIY_ESPCAM\main\page_menu.c
 */
#ifndef _PAGEMENU_
#define _PAGEMENU_

#ifdef __cplusplus
extern "C"
{
#endif
/*********************
* INCLUDES
*********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "../../lvgl/lvgl.h"
#include "../../lv_ex_conf.h"
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif
/**
  * @brief  让一个变量增加或者减去一个值，在大于最大值后从最小值开始，小于最小值后从最大值开始
  * @param  src:被控变量
  * @param  plus:增加的值
  * @param  min:最小值
  * @param  max:最大值
  * @retval 无
  */
#define __ValuePlus(src, plus, min, max) \
	do                                   \
	{                                    \
		int __value_temp = (src);        \
		__value_temp += (plus);          \
		if (__value_temp < (min))        \
			__value_temp = (max);        \
		else if (__value_temp > (max))   \
			__value_temp = (min);        \
		(src) = __value_temp;            \
	} while (0)

#define __Sizeof(arr) (sizeof(arr) / sizeof(arr[0]))

typedef enum
{
	MOVE_NONE = 0,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_UP,
	MOVE_DOWN
} Move_DirTypeDef;

typedef enum
{
	Disp_Home = -2,//主页
	Disp_Menu = -1,//菜单选择界面
	Disp_Cam = 0,//摄像头显示
	Disp_Color,//颜色识别
	Disp_Face,//人脸识别
	Disp_Baidu,//百度AI 物体和文字识别
	Disp_IMU,//陀螺仪
	Disp_FFT,//音乐频谱
	Disp_Daily,//每日一句
	Disp_Game_2048,//2048游戏
	Disp_Game_Snake,//贪吃蛇游戏
	Disp_Music,//音乐播放界面 采用lvgl的music demo
	Disp_Calendar,//日历
	Disp_Setting,//设置界面 可设置摄像头翻转   空调类型选择
	Disp_About,//关于介绍
} Display_Page; //当前界面枚举

typedef struct
{
	const void *img_buffer; //图标图像数据
	const char *text;		//文字描述
	lv_obj_t *img;			//图标img对象
	bool focus;				//图标当前是否被聚焦

} ICON_TypeDef;

void app_menu_create(void);
void app_menu_anim_Vexit(uint8_t id, bool dir);
void app_menu_anim_Hexit(uint8_t id, bool dir);

void page_menu_start();
void page_menu_end();
uint8_t Menu_Choose(void);
void move_task_menu(uint8_t dir);
#endif
