#ifndef _APP_ANIM_H_
#define _APP_ANIM_H_

#include "lvgl.h"


void obj_add_anim(lv_obj_t *obj, lv_anim_exec_xcb_t exec_cb, uint16_t time, lv_coord_t start, lv_coord_t end, lv_anim_path_cb_t path_cb);
void img_anim_exec_xcb(void *obj, lv_anim_value_t value);





#endif

