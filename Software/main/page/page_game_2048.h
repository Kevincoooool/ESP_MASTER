#ifndef PAGE_2048_GAME_H
#define PAGE_2048_GAME_H

#ifdef __cplusplus
extern "C" {
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


/*********************
 *      DEFINES
 *********************/
/* ��С */
#define SIZE 4

/* ���� */
#define PAGE_2048_GAME_BOX_W			(200)
#define PAGE_2048_GAME_BOX_H			(200)
#define PAGE_2048_NUMBER_BOX_W			(40)
#define PAGE_2048_NUMBER_BOX_H			(40)
#define PAGE_2048_CURRENT_SCORE_W		(40)
#define PAGE_2048_CURRENT_SCORE_H		(35)
#define PAGE_2048_BEST_SCORE_W			(40)
#define PAGE_2048_BEST_SCORE_H			(35)

/* ��ɫ */
#define PAGE_2048_GAME_BG_COLOR 		lv_color_hex(0xe8e5db) //LV_COLOR_MAKE(0xCD, 0xC1, 0xB4)
#define PAGE_2048_GAME_BOX_COLOR 		lv_color_hex(0xBBADA0)
#define PAGE_2048_NUMBER_BOX_COLOR 	lv_color_hex(0xCDC1B4)
#define PAGE_2048_NUMBER_2_COLOR 		lv_color_hex(0xeee4da)
#define PAGE_2048_NUMBER_4_COLOR 		lv_color_hex(0xeee1c9)
#define PAGE_2048_NUMBER_8_COLOR 		lv_color_hex(0xf3b27a)
#define PAGE_2048_NUMBER_16_COLOR 		lv_color_hex(0xf69664)
#define PAGE_2048_NUMBER_32_COLOR 		lv_color_hex(0xf77c5f)
#define PAGE_2048_NUMBER_64_COLOR 		lv_color_hex(0xf75f3b)
#define PAGE_2048_NUMBER_128_COLOR 	lv_color_hex(0xedd073)
#define PAGE_2048_NUMBER_256_COLOR 	lv_color_hex(0xEDCC61)
#define PAGE_2048_NUMBER_512_COLOR 	lv_color_hex(0xEDCC61)
#define PAGE_2048_NUMBER_1024_COLOR 	lv_color_hex(0xEDCC61)
#define PAGE_2048_NUMBER_2048_COLOR 	lv_color_hex(0xEDC22E)


/**********************
 *      TYPEDEFS
 **********************/
typedef struct _PAGE_2048_game {
	lv_obj_t * bg;					/* ���� */
	lv_obj_t * canvas_stage;		/* ��Ϸ��̨ */
	lv_obj_t * label_best_score;	/* չʾ��߷��� */
	lv_obj_t * label_current_score;	/* չʾ��ǰ���� */
	lv_task_t * task_handle;		/* ������ */
									 
	uint8_t game_board[SIZE][SIZE];	/* 2048���� */
	uint32_t current_score_value;	/* ��ǰ���� */
	bool play_game;					/* ��Ϸ״̬ */
} T_PAGE_2048_game, *PT_PAGE_2048_game;


/**********************
 * GLOBAL PROTOTYPES
 **********************/
void PAGE_game_2048(void);
void page_game_2048_load(void);
void page_game_2048_start(void);
void page_game_2048_end(void);
void move_task_game_2048(uint8_t move);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PAGE_2048_GAME_H */





