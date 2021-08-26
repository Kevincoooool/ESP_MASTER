#ifndef PAGE_2048_SNAKE_H
#define PAGE_2048_SNAKE_H

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
#define KSDIY_GAME_SNAKE_TITLE_SPACE   (30)
#define KSDIY_GAME_SNAKE_INIT_LINE     (3)
#define KSDIY_GAME_SNAKE_MAX_LINE      (240)
#define KSDIY_GAME_SNAKE_SIZE          (20)
//#define KSDIY_GAME_SNAKE_EYE_SIZE    (5)
#define KSDIY_GAME_SNAKE_SPEED         (5)
#define KSDIY_GAME_SNAKE_SPACE         15
#define KSDIY_GAME_SNAKE_HEAD_COLOR    (LV_COLOR_YELLOW)  		// ��ͷ��ɫ
#define KSDIY_GAME_SNAKE_BODY_COLOR    (LV_COLOR_BLACK)  		// ������ɫ
#define KSDIY_GAME_SNAKE_RADIUS        (15)   					// ������Բ������radius
#define KSDIY_GAME_FOOD_RADIUS         (50)   					// ʳ��Բ������
#define KSDIY_GAME_FOOD_COLOR          (LV_COLOR_PURPLE) 	    // ʳ����ɫ
#define KSDIY_GAME_FOOD_SIZE           (KSDIY_GAME_SNAKE_SIZE)
#define KSDIY_GAME_FOOD_CHECK_SPACE    (10)
#define KSDIY_GAME_FOOD_MAX_HOR        (LV_HOR_RES - KSDIY_GAME_SNAKE_SIZE)
#define KSDIY_GAME_FOOD_MAX_VER        (LV_VER_RES - KSDIY_GAME_SNAKE_TITLE_SPACE)
#define KSDIY_GAME_SNAKE_USE_KEY       (1)  					// ʹ�ð�������


/**********************
 *      TYPEDEFS
 **********************/
// ��Ϸ����
typedef struct _KSDIY_snake {
	lv_obj_t  * bg;				// ����
	lv_obj_t  * obj_food;		// ʳ����
	lv_obj_t  * label_info;		// ��Ϸ��Ϣ
	lv_task_t * task_handle; 	// ������
	uint8_t	sroce;				// �÷�
	uint8_t len;				// ����
	uint8_t gesture;			// ��ͷ�����־
} T_KSDIY_snake, *PT_KSDIY_snake;

// ��������
typedef struct Link{
	lv_obj_t *obj;            	// ����
	lv_coord_t x;             	// x����
	lv_coord_t y;             	// y����
	struct Link *prior;       	// ָ��ֱ��ǰ��
	struct Link *next;        	// ָ��ֱ�Ӻ����
}link_snake, p_link_snake;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void page_game_snake(void);
void page_game_snake_load(void);
void page_game_snake_start(void);
void page_game_snake_end(void);
void move_task_game_snake(uint8_t move);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PAGE_2048_GAME_H */





