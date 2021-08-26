
#ifndef _COMPMENT_
#define _COMPMENT_

#ifdef __cplusplus
extern "C" {
#endif
/*********************
* INCLUDES
*********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
//#include "lv_ex_conf.h"
#else
#include "../../lvgl/lvgl.h"
#include "../../lv_ex_conf.h"
#endif
#define ANIEND  while(lv_anim_count_running())lv_task_handler();//等待动画完成
#define Move_mode uint8_t
typedef struct _compment_t
{
    int16_t start;
    int16_t end;
    lv_obj_t * obj;
    uint16_t start_t;
    uint16_t end_t;
    lv_anim_path_cb_t cb;
    lv_anim_exec_xcb_t funcb;
    lv_anim_path_t path;
    lv_anim_t a;
    struct _compment_t *next;

}compment_t;
void lv_add_compment(compment_t *compment1,compment_t *compment2);
void move_free(compment_t *compment);
void obj_move(compment_t *compment,Move_mode mode);
#ifdef __cplusplus
} /* extern "C" */
#endif




#endif // _TEST_
