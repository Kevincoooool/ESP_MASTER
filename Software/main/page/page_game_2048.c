/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-15 10:53:21
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-17 15:06:49
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_game_2048.c
 */
/**
 ******************************************************************************
 * @file    PAGE_game_2048.c
 * @author  百问科技
 * @version V1.2
 * @date    2020-12-12
 * @brief	2048游戏
 ******************************************************************************
 * Change Logs:
 * Date           Author          Notes
 * 2020-12-12     zhouyuebiao     First version
 * 2021-01-25     zhouyuebiao     V1.2 
 ******************************************************************************
 * @attention
 *
 * Copyright (C) 2008-2021 深圳百问网科技有限公司<https://www.100ask.net/>
 * All rights reserved
 *
 ******************************************************************************
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "button.h"
#include "page_game_2048.h"
#include "app_anim.h"
#include "lv_compment.h"
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "lv_port_indev.h"
LV_FONT_DECLARE(myFont);
/**********************
 *  STATIC VARIABLES
 **********************/
static PT_PAGE_2048_game g_pt_PAGE_2048_game; // 数据结构体

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void PAGE_game_2048_init(void);												// 界面初始化
static void PAGE_game_2048_init_board(lv_obj_t *parent, uint8_t board[SIZE][SIZE]); // 初始化画板，展示主界面效果
static void PAGE_game_2048_init_board_number(uint8_t board[SIZE][SIZE]);			// 初始化小方块数据
static void PAGE_game_2048_draw_board(lv_obj_t *parent, char *number,
									  uint16_t canvas_w, uint16_t canvas_h,
									  uint16_t rect_dsc_x, uint16_t rect_dsc_y,
									  lv_color_t rgb32);								 // 填充绘制小方块
static void PAGE_game_2048_add_random(uint8_t board[SIZE][SIZE]);						 // 为二维数组初始化随机位置的随机数
static void PAGE_game_2048_rotate_board(uint8_t board[SIZE][SIZE]);						 // 旋转矩阵
static bool PAGE_game_2048_slide_array(uint8_t array[SIZE]);							 // 检查数组是否还有空间
static bool PAGE_game_2048_move_up(uint8_t board[SIZE][SIZE]);							 // 检查是否可以上滑动(触摸屏控制)
static bool PAGE_game_2048_move_left(uint8_t board[SIZE][SIZE]);						 // 检查是否可以左滑动(触摸屏控制)
static bool PAGE_game_2048_move_down(uint8_t board[SIZE][SIZE]);						 // 检查是否可以下滑动(触摸屏控制)
static bool PAGE_game_2048_move_right(uint8_t board[SIZE][SIZE]);						 // 检查是否可以右滑动(触摸屏控制)
static bool PAGE_game_2048_find_pair_down(uint8_t board[SIZE][SIZE]);					 // 检查是否可以右滑动
static bool PAGE_game_2048_game_ended(uint8_t board[SIZE][SIZE]);						 // 判断游戏是否应该结束
static char *PAGE_game_2048_int_to_string(int num, char *str);							 // 整型数值转为字符串
static lv_color_t PAGE_game_2048_num_to_color(uint32_t num);							 // 为数字设置对应的颜色
static uint8_t PAGE_game_2048_find_target(uint8_t array[SIZE], uint8_t x, uint8_t stop); // 检查是否应该停止滑动以避免双重合并
static uint8_t PAGE_game_2048_count_empty(uint8_t board[SIZE][SIZE]);					 // 查找当前二维数组是否有可以合并的数值(数值一样即可合并)
// 任务
static void PAGE_game_2048_game_key_up(void);	 // 按键控制上移(按键控制)
static void PAGE_game_2048_game_key_down(void);	 // 按键控制下移(按键控制)
static void PAGE_game_2048_game_key_left(void);	 // 按键控制左移(按键控制)
static void PAGE_game_2048_game_key_right(void); // 按键控制右移(按键控制)

/*
 *  函数名：   void PAGE_ _tool_widget_test(void)
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 应用入口
*/
void PAGE_game_2048(void)
{
	/* 申请内存 */
	g_pt_PAGE_2048_game = (T_PAGE_2048_game *)malloc(sizeof(T_PAGE_2048_game));

	/* 初始化桌面背景 */
	g_pt_PAGE_2048_game->bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_size(g_pt_PAGE_2048_game->bg, LV_HOR_RES, LV_VER_RES);
	lv_obj_set_y(g_pt_PAGE_2048_game->bg, 0);

	/* 初始化游戏舞台 */
	g_pt_PAGE_2048_game->canvas_stage = lv_obj_create(g_pt_PAGE_2048_game->bg, NULL);
	lv_obj_set_style_local_radius(g_pt_PAGE_2048_game->canvas_stage, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);						  // 设置圆角
	lv_obj_set_style_local_bg_color(g_pt_PAGE_2048_game->canvas_stage, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, PAGE_2048_GAME_BOX_COLOR); //设置颜色
	lv_obj_set_size(g_pt_PAGE_2048_game->canvas_stage, PAGE_2048_GAME_BOX_W, PAGE_2048_GAME_BOX_H);
	lv_obj_align(g_pt_PAGE_2048_game->canvas_stage, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

	/* 初始化主界面 */
	g_pt_PAGE_2048_game->play_game = true;
	g_pt_PAGE_2048_game->current_score_value = 0;
	PAGE_game_2048_init();
}

/*
 *  函数名：   static void PAGE_ _tool_music_player_init(void)
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 应用界面初始化
*/
static void PAGE_game_2048_init(void)
{

	/* 最高记录显示区域 */
	lv_obj_t *obj_best_source = lv_obj_create(g_pt_PAGE_2048_game->bg, NULL);
	lv_obj_set_size(obj_best_source, 65, 35);
	lv_obj_align(obj_best_source, g_pt_PAGE_2048_game->canvas_stage, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);

	/* 最高记录标题 */
	lv_obj_t *best_source_tip_text = lv_label_create(obj_best_source, NULL); /* 创建标签 */
	lv_label_set_text(best_source_tip_text, "BEST");
	lv_obj_align(best_source_tip_text, NULL, LV_ALIGN_IN_TOP_MID, 0, 0); /* 居中 */

	/* 最高记录分数 */
	g_pt_PAGE_2048_game->label_best_score = lv_label_create(obj_best_source, NULL); /* 创建标签 */
	lv_label_set_text(g_pt_PAGE_2048_game->label_best_score, "1024");
	lv_obj_align(g_pt_PAGE_2048_game->label_best_score, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0); /* 居中 */

	/* 当前分数显示区域 */
	lv_obj_t *obj_current_source = lv_obj_create(g_pt_PAGE_2048_game->bg, NULL);
	lv_obj_set_size(obj_current_source, 65, 35);
	lv_obj_align(obj_current_source, g_pt_PAGE_2048_game->canvas_stage, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

	/* 当前分提示 */
	lv_obj_t *current_source_tip_text = lv_label_create(obj_current_source, NULL); /* 创建标签 */
	lv_label_set_text(current_source_tip_text, "SORCE");
	lv_obj_align(current_source_tip_text, NULL, LV_ALIGN_IN_TOP_MID, 0, 0); /* 居中 */

	/* 当前分数 */
	g_pt_PAGE_2048_game->label_current_score = lv_label_create(obj_current_source, NULL); /* 创建标签 */
	lv_label_set_text(g_pt_PAGE_2048_game->label_current_score, "0");
	lv_obj_align(g_pt_PAGE_2048_game->label_current_score, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0); /* 当前画布居中 */

	/* 初始化游戏数字 */
	PAGE_game_2048_init_board_number(g_pt_PAGE_2048_game->game_board);

	/* 初始化游戏舞台中的每个方格块 */
	PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
}

/*
 *  函数名：   static void PAGE_game_2048_init_board(lv_obj_t * parent, uint8_t board[SIZE][SIZE])
 *  输入参数： 父对象
 *  返回值：   需要操作的数组
 *  函数作用： 初始化画板，展示主界面效果
*/
static void PAGE_game_2048_init_board(lv_obj_t *parent, uint8_t board[SIZE][SIZE])
{
	/* 清除之前所有的子对象 */
	lv_obj_clean(parent);

	/* 当前分数 */
	char str_current_score[8] = {0};
	lv_label_set_text(g_pt_PAGE_2048_game->label_current_score, PAGE_game_2048_int_to_string(g_pt_PAGE_2048_game->current_score_value, str_current_score));

	for (int y = 0; y < SIZE; y++)
	{
		for (int x = 0; x < SIZE; x++)
		{
			if (board[x][y] != 0)
			{
				//sprintf(tmp_char, "%d", board[x][y]);
				char tmp_char[8];
				snprintf(tmp_char, 8, "%u", (uint32_t)1 << board[x][y]);
				PAGE_game_2048_draw_board(parent, tmp_char,
										  PAGE_2048_NUMBER_BOX_W, PAGE_2048_NUMBER_BOX_H,
										  (8 * (y + 1)) + (PAGE_2048_NUMBER_BOX_W * (y + 1 - 1)), (8 * (x + 1)) + (PAGE_2048_NUMBER_BOX_W * (x + 1 - 1)), PAGE_game_2048_num_to_color((uint32_t)1 << board[x][y]));
			}
			else
			{
				PAGE_game_2048_draw_board(parent, "",
										  PAGE_2048_NUMBER_BOX_W, PAGE_2048_NUMBER_BOX_H,
										  (8 * (y + 1)) + (PAGE_2048_NUMBER_BOX_W * (y + 1 - 1)), (8 * (x + 1)) + (PAGE_2048_NUMBER_BOX_W * (x + 1 - 1)), PAGE_2048_NUMBER_BOX_COLOR);
			}
		}
	}
}

/*
 *  函数名：   static void PAGE_game_2048_draw_board(lv_obj_t * parent, char * number, \
														 uint16_t canvas_w, uint16_t canvas_h, \
														 uint16_t rect_dsc_x, uint16_t rect_dsc_y, \
														 lv_color_t rgb32)
 *  输入参数： 绘制所依托的父对象
 *  输入参数： 展示的数值
 *  输入参数： 宽度
 *  输入参数： 高度
 *  输入参数： x轴上的偏移值
 *  输入参数： y轴上的偏移值
 *  输入参数： 方块背景颜色
 *  返回值：   无
 *  函数作用： 填充绘制小方块
*/
static void PAGE_game_2048_draw_board(lv_obj_t *parent, char *number,
									  uint16_t canvas_w, uint16_t canvas_h,
									  uint16_t rect_dsc_x, uint16_t rect_dsc_y,
									  lv_color_t rgb32)
{
	/* 创建方格块 */
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_size(obj, canvas_w, canvas_h);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, rgb32);
	//lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);			  // 设置圆角
	lv_obj_set_style_local_border_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0); // 边框透明度
	lv_obj_set_style_local_text_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &myFont);	  // 字体大小

	lv_obj_align(obj, NULL, LV_ALIGN_IN_TOP_LEFT, rect_dsc_x, rect_dsc_y);

	/* 数字 */
	lv_obj_t *icon = lv_label_create(obj, NULL); /* 创建标签 */

	lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
	lv_obj_set_style_local_text_font(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_label_set_text(icon, number);

	lv_obj_align(icon, NULL, LV_ALIGN_CENTER, 0, 0); /* 当前画布居中 */
}

/*
 *  函数名：   static char* PAGE_game_2048_int_to_string(int num, char *str)
 *  输入参数： 需要转换的整型数值
 *  返回值：   转换后的字符串
 *  函数作用： 整型数值转为字符串
*/
static char *PAGE_game_2048_int_to_string(int num, char *str)
{
	int i = 0;	 //指示填充str
	if (num < 0) //如果num为负数，将num变正
	{
		num = -num;
		str[i++] = '-';
	}
	//转换
	do
	{
		str[i++] = num % 10 + 48; //取num最低位 字符0~9的ASCII码是48~57；简单来说数字0+48=48，ASCII码对应字符'0'
		num /= 10;				  //去掉最低位
	} while (num);				  //num不为0继续循环

	str[i] = '\0';

	//确定开始调整的位置
	int j = 0;
	if (str[0] == '-') //如果有负号，负号不用调整
	{
		j = 1; //从第二位开始调整
		++i;   //由于有负号，所以交换的对称轴也要后移1位
	}
	//对称交换
	for (; j < i / 2; j++)
	{
		//对称交换两端的值 其实就是省下中间变量交换a+b的值：a=a+b;b=a-b;a=a-b;
		str[j] = str[j] + str[i - 1 - j];
		str[i - 1 - j] = str[j] - str[i - 1 - j];
		str[j] = str[j] - str[i - 1 - j];
	}

	return str; //返回转换后的值
}

/*
 *  函数名：   static void PAGE_game_2048_init_board_number(uint8_t board[SIZE][SIZE])
 *  输入参数： 需要进行初始化的数组
 *  返回值：   无
 *  函数作用： 为二维数组初始化数值(全写0)
*/
static void PAGE_game_2048_init_board_number(uint8_t board[SIZE][SIZE])
{
	uint8_t x, y;
	for (x = 0; x < SIZE; x++)
	{
		for (y = 0; y < SIZE; y++)
		{
			board[x][y] = 0;
		}
	}

	/* 初始化两个随机位置的随机数 */
	PAGE_game_2048_add_random(board);
	PAGE_game_2048_add_random(board);

	g_pt_PAGE_2048_game->current_score_value = 0;
}

//__use_no_semihosting was requested, but _ttywrch was
// void time(int ch)
// {
// 	ch = ch;
// }

/*
 *  函数名：   static void PAGE_game_2048_add_random(uint8_t board[SIZE][SIZE])
 *  输入参数： 需要初始化随机位置的随机数的数组
 *  返回值：   无
 *  函数作用： 为二维数组初始化随机位置的随机数
*/
static void PAGE_game_2048_add_random(uint8_t board[SIZE][SIZE])
{
	static bool initialized = false;
	uint8_t x, y;
	uint8_t r, len = 0;
	uint8_t n, list[SIZE * SIZE][2];

	if (!initialized)
	{
		srand(20);
		initialized = true;
	}

	for (x = 0; x < SIZE; x++)
	{
		for (y = 0; y < SIZE; y++)
		{
			if (board[x][y] == 0)
			{
				list[len][0] = x;
				list[len][1] = y;
				len++;
			}
		}
	}

	if (len > 0)
	{
		r = rand() % len;
		x = list[r][0];
		y = list[r][1];
		n = (rand() % 10) / 9 + 1;
		board[x][y] = n;
	}
}

/*
 *  函数名：   static lv_color_t PAGE_game_2048_num_to_color(uint32_t num)
 *  输入参数： 需要设置颜色的数值
 *  返回值：   颜色值
 *  函数作用： 为数字设置对应的颜色
*/
static lv_color_t PAGE_game_2048_num_to_color(uint32_t num)
{
	switch (num)
	{
	case 2:
		return PAGE_2048_NUMBER_2_COLOR;
	case 4:
		return PAGE_2048_NUMBER_4_COLOR;
	case 8:
		return PAGE_2048_NUMBER_8_COLOR;
	case 16:
		return PAGE_2048_NUMBER_16_COLOR;
	case 32:
		return PAGE_2048_NUMBER_32_COLOR;
	case 64:
		return PAGE_2048_NUMBER_64_COLOR;
	case 128:
		return PAGE_2048_NUMBER_128_COLOR;
	case 256:
		return PAGE_2048_NUMBER_256_COLOR;
	case 512:
		return PAGE_2048_NUMBER_512_COLOR;
	case 1024:
		return PAGE_2048_NUMBER_1024_COLOR;
	case 2048:
		return PAGE_2048_NUMBER_2048_COLOR;
	default:
		return PAGE_2048_NUMBER_2048_COLOR;
	}
}

/*
 *  函数名：   static uint8_t PAGE_game_2048_find_target(uint8_t array[SIZE],uint8_t x,uint8_t stop) 
 *  输入参数： 需要检查的二维数组
 *  返回值：   目标位置
 *  函数作用： 检查是否应该停止滑动以避免双重合并
*/
static uint8_t PAGE_game_2048_find_target(uint8_t array[SIZE], uint8_t x, uint8_t stop)
{
	uint8_t t;
	// if the position is already on the first, don't evaluate
	if (x == 0)
	{
		return x;
	}
	for (t = (x - 1);; t--)
	{
		if (array[t] != 0)
		{
			if (array[t] != array[x])
			{
				// merge is not possible, take next position
				return (t + 1);
			}
			return t;
		}
		else
		{
			// we should not slide further, return this one
			if (t == stop)
			{
				return t;
			}
		}
	}
	// we did not find a
	return x;
}

/*
 *  函数名：   static bool PAGE_game_2048_slide_array(uint8_t array[SIZE]) 
 *  输入参数： 需要检查的数组
 *  返回值：   true - 尚有空间； false - 无多余空间
 *  函数作用： 检查数组是否还有空间
*/
static bool PAGE_game_2048_slide_array(uint8_t array[SIZE])
{
	bool success = false;
	uint8_t x, t, stop = 0;

	for (x = 0; x < SIZE; x++)
	{
		if (array[x] != 0)
		{
			t = PAGE_game_2048_find_target(array, x, stop);
			// if target is not original position, then move or merge
			if (t != x)
			{
				// if target is zero, this is a move
				if (array[t] == 0)
				{
					array[t] = array[x];
				}
				else if (array[t] == array[x])
				{
					// merge (increase power of two)
					array[t]++;
					// increase score
					g_pt_PAGE_2048_game->current_score_value += (uint32_t)1 << array[t];
					// set stop to avoid double merge
					stop = t + 1;
				}
				array[x] = 0;
				success = true;
			}
		}
	}
	return success;
}

/*
 *  函数名：   static bool PAGE_game_2048_rotate_board(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要旋转的矩阵
 *  返回值：   无
 *  函数作用： 旋转矩阵
*/
static void PAGE_game_2048_rotate_board(uint8_t board[SIZE][SIZE])
{
	uint8_t i, j, n = SIZE;
	uint8_t tmp;
	for (i = 0; i < n / 2; i++)
	{
		for (j = i; j < n - i - 1; j++)
		{
			tmp = board[i][j];
			board[i][j] = board[j][n - i - 1];
			board[j][n - i - 1] = board[n - i - 1][n - j - 1];
			board[n - i - 1][n - j - 1] = board[n - j - 1][i];
			board[n - j - 1][i] = tmp;
		}
	}
}

/*
 *  函数名：   static bool PAGE_game_2048_move_up(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要检查的二维数组
 *  返回值：   true - 可以滑动； false - 不可以滑动
 *  函数作用： 检查是否可以上滑动
*/
static bool PAGE_game_2048_move_up(uint8_t board[SIZE][SIZE])
{
	bool success = false;
	uint8_t x;
	for (x = 0; x < SIZE; x++)
	{
		success |= PAGE_game_2048_slide_array(board[x]);
	}
	return success;
}

/*
 *  函数名：   static bool PAGE_game_2048_move_left(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要检查的二维数组
 *  返回值：   true - 可以滑动； false - 不可以滑动
 *  函数作用： 检查是否可以上滑动
*/
static bool PAGE_game_2048_move_left(uint8_t board[SIZE][SIZE])
{
	bool success;
	PAGE_game_2048_rotate_board(board);
	success = PAGE_game_2048_move_up(board);
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	return success;
}

/*
 *  函数名：   static bool PAGE_game_2048_move_down(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要检查的二维数组
 *  返回值：   true - 可以滑动； false - 不可以滑动
 *  函数作用： 检查是否可以下滑动
*/
static bool PAGE_game_2048_move_down(uint8_t board[SIZE][SIZE])
{
	bool success;
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	success = PAGE_game_2048_move_up(board);
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	return success;
}

/*
 *  函数名：   static bool PAGE_game_2048_move_right(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要检查的二维数组
 *  返回值：   true - 可以滑动； false - 不可以滑动
 *  函数作用： 检查是否可以右滑动
*/
static bool PAGE_game_2048_move_right(uint8_t board[SIZE][SIZE])
{
	bool success = false;
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	success = PAGE_game_2048_move_up(board);
	PAGE_game_2048_rotate_board(board);
	return success;
}

/*
 *  函数名：   static uint8_t PAGE_game_2048_count_empty(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要查找的二维数组
 *  返回值：   true - 有可以合并的数值； false - 没有可以合并的数值
 *  函数作用： 查找当前二维数组是否有可以合并的数值(数值一样即可合并)
*/
static bool PAGE_game_2048_find_pair_down(uint8_t board[SIZE][SIZE])
{
	bool success = false;
	uint8_t x, y;
	for (x = 0; x < SIZE; x++)
	{
		for (y = 0; y < SIZE - 1; y++)
		{
			if (board[x][y] == board[x][y + 1])
				return true;
		}
	}
	return success;
}

/*
 *  函数名：   static uint8_t PAGE_game_2048_count_empty(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要统计的二维数组
 *  返回值：   空闲的方格个数
 *  函数作用： 统计空闲的方格个数
*/
static uint8_t PAGE_game_2048_count_empty(uint8_t board[SIZE][SIZE])
{
	uint8_t x, y;
	uint8_t count = 0;
	for (x = 0; x < SIZE; x++)
	{
		for (y = 0; y < SIZE; y++)
		{
			if (board[x][y] == 0)
			{
				count++;
			}
		}
	}
	return count;
}
static void game_message(const char *str, uint16_t t) //系统通知盒
{

	lv_obj_t *mbox1 = lv_msgbox_create(g_pt_PAGE_2048_game->bg, NULL); //box全局通知
	lv_obj_set_style_local_text_font(mbox1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_msgbox_set_text(mbox1, str);
	lv_obj_set_size(mbox1, 240, 50);
	lv_msgbox_start_auto_close(mbox1, t);
	lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
}
// 游戏结束

/*
 *  函数名：   static bool PAGE_game_2048_game_ended(uint8_t board[SIZE][SIZE])
 *  输入参数： 无
 *  返回值：   true - 游戏结束， false - 可继续游戏
 *  函数作用： 判断游戏是否应该结束
*/
static bool PAGE_game_2048_game_ended(uint8_t board[SIZE][SIZE])
{
	bool ended = true;
	if (PAGE_game_2048_count_empty(board) > 0)
		return false;
	if (PAGE_game_2048_find_pair_down(board))
		return false;
	PAGE_game_2048_rotate_board(board);
	if (PAGE_game_2048_find_pair_down(board))
		ended = false;
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	game_message("GAME OVER", 10000);
	return ended;
}

/*
 *  函数名：   static void PAGE_game_2048_game_key_right(void)
 *  输入参数： 无
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 按键控制上移
*/
static void PAGE_game_2048_game_key_up(void)
{
	if (PAGE_game_2048_move_up(g_pt_PAGE_2048_game->game_board))
	{
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_add_random(g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		if (PAGE_game_2048_game_ended(g_pt_PAGE_2048_game->game_board))
		{
			printf("         GAME OVER          \n");
			game_message("GAME OVER", 10000);
		}
	}
}

/*
 *  函数名：   static void PAGE_game_2048_game_key_right(void)
 *  输入参数： 无
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 按键控制下移
*/
static void PAGE_game_2048_game_key_down(void)
{
	if (PAGE_game_2048_move_down(g_pt_PAGE_2048_game->game_board))
	{
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_add_random(g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		if (PAGE_game_2048_game_ended(g_pt_PAGE_2048_game->game_board))
		{
			printf("         GAME OVER          \n");
			game_message("GAME OVER", 10000);
		}
	}
}

/*
 *  函数名：   static void PAGE_game_2048_game_key_right(void)
 *  输入参数： 无
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 按键控制左移
*/
static void PAGE_game_2048_game_key_left(void)
{
	if (PAGE_game_2048_move_left(g_pt_PAGE_2048_game->game_board))
	{
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_add_random(g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		if (PAGE_game_2048_game_ended(g_pt_PAGE_2048_game->game_board))
		{
			printf("         GAME OVER          \n");
			game_message("GAME OVER", 10000);
		}
	}
}

/*
 *  函数名：   static void PAGE_game_2048_game_key_right(void)
 *  输入参数： 无
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 按键控制右移
*/
static void PAGE_game_2048_game_key_right(void)
{
	if (PAGE_game_2048_move_right(g_pt_PAGE_2048_game->game_board))
	{
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_add_random(g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		if (PAGE_game_2048_game_ended(g_pt_PAGE_2048_game->game_board))
		{
			printf("         GAME OVER          \n");
			game_message("GAME OVER", 10000);
		}
	}
}

/*
 *  函数名：   static void event_handler_back_to_home(lv_obj_t * obj, lv_event_t event)
 *  输入参数： 触发事件的对象
 *  输入参数： 触发的事件类型
 *  返回值：   无
 *  函数作用： 返回桌面事件处理函数
*/
static void event_handler_back_to_home(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_CLICKED)
	{
		if (g_pt_PAGE_2048_game->task_handle != NULL)
			lv_task_del(g_pt_PAGE_2048_game->task_handle); /* 删除任务 */
		if (g_pt_PAGE_2048_game->canvas_stage != NULL)
			lv_obj_del(g_pt_PAGE_2048_game->canvas_stage); /* 删除游戏舞台 */
		if (g_pt_PAGE_2048_game->label_best_score != NULL)
			lv_obj_del(g_pt_PAGE_2048_game->label_best_score); /* 删除最高分数对象 */
		if (g_pt_PAGE_2048_game->label_current_score != NULL)
			lv_obj_del(g_pt_PAGE_2048_game->label_current_score); /* 删除当前分数对象 */
		if (g_pt_PAGE_2048_game->bg != NULL)
			lv_obj_del(g_pt_PAGE_2048_game->bg); /* 删除背景 */

		/* 释放内存 */
		free(g_pt_PAGE_2048_game);

		/* 清除lv_layer_top */
		lv_obj_set_click(lv_layer_top(), false);
		lv_obj_clean(lv_layer_top());
		lv_obj_set_event_cb(lv_layer_top(), NULL); /* 分配事件处理 */

		/* 清空屏幕并返回桌面 */
		// PAGE_ _anim_out_all(lv_scr_act(), 0);
		// PAGE_ _demo_home(0);
	}
}

void page_game_2048_load(void)
{
	PAGE_game_2048();

	obj_add_anim(
		g_pt_PAGE_2048_game->bg,						  //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x, //动画函数
		300,							  //动画速度
		-240,							  //起始值
		0,								  //结束值
		lv_anim_path_linear				  //动画特效:模拟弹性物体下落
	);
	ANIEND
}
void page_game_2048_end(void)
{
	obj_add_anim(
		g_pt_PAGE_2048_game->bg,						  //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x, //动画函数
		300,							  //动画速度
		0,							  //起始值
		-240,								  //结束值
		lv_anim_path_linear				  //动画特效:模拟弹性物体下落
	);
	ANIEND
	// lv_obj_del(game_2048);
	// if (g_pt_PAGE_2048_game->task_handle != NULL)
	// 	lv_task_del(g_pt_PAGE_2048_game->task_handle); /* 删除任务 */
	if (g_pt_PAGE_2048_game->canvas_stage != NULL)
		lv_obj_del(g_pt_PAGE_2048_game->canvas_stage); /* 删除游戏舞台 */
	if (g_pt_PAGE_2048_game->label_best_score != NULL)
		lv_obj_del(g_pt_PAGE_2048_game->label_best_score); /* 删除最高分数对象 */
	if (g_pt_PAGE_2048_game->label_current_score != NULL)
		lv_obj_del(g_pt_PAGE_2048_game->label_current_score); /* 删除当前分数对象 */
	if (g_pt_PAGE_2048_game->bg != NULL)
		lv_obj_del(g_pt_PAGE_2048_game->bg); /* 删除背景 */

	/* 释放内存 */
	free(g_pt_PAGE_2048_game);
}
void page_game_2048_start(void)
{
	//获取芯片可用内存
	printf("     esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//获取从未使用过的最小内存
	printf("     esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	page_game_2048_load();
}
void move_task_game_2048(uint8_t move)
{

	switch (move)
	{

	case BT1_LONG: //往上移动
		PAGE_game_2048_game_key_down();
		break;
	case BT1_LONGFREE: //往上移动

		break;
	case BT3_LONG: //往下移动
		PAGE_game_2048_game_key_up();
		break;
	case BT3_LONGFREE: //往上移动

		break;
	case BT1_DOWN: //往上移动
		PAGE_game_2048_game_key_up();
		break;
	case BT3_DOWN: //往下移动
		PAGE_game_2048_game_key_down();
		break;
	case BT1_DOUBLE: //往上移动
		PAGE_game_2048_game_key_left();
		break;
	case BT3_DOUBLE: //往下移动
		PAGE_game_2048_game_key_right();
		break;
	default:
		break;
	}
}