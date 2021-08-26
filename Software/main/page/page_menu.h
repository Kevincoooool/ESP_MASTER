/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-02 09:59:54
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-06 14:00:22
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_menu.h
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
	Disp_Home = -2,
	Disp_Menu = -1,
	Disp_Cam = 0,
	Disp_Color,
	Disp_Face,
	Disp_Baidu,
	Disp_IMU,
	Disp_FFT,
	Disp_Daily,
	Disp_Game_2048,
	Disp_Game_Snake,
	Disp_Music,
	Disp_Calendar,
	Disp_Setting,
	Disp_About,
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
