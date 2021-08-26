#include "../../../lv_examples.h"

void lv_ex_obj_1(void)
{
    lv_obj_t * obj1;
    obj1 = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj1, 100, 50);
    lv_obj_align(obj1, NULL, LV_ALIGN_CENTER, -60, -30);

    /*Copy the previous object and enable drag*/
    lv_obj_t * obj2;
    obj2 = lv_obj_create(lv_scr_act(), obj1);
    lv_obj_align(obj2, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_drag(obj2, true);

    static lv_style_t style_shadow;
    lv_style_init(&style_shadow);
    lv_style_set_shadow_width(&style_shadow, LV_STATE_DEFAULT, 10);
    lv_style_set_shadow_spread(&style_shadow, LV_STATE_DEFAULT, 5);
    lv_style_set_shadow_color(&style_shadow, LV_STATE_DEFAULT, LV_COLOR_BLUE);

    /*Copy the previous object (drag is already enabled)*/
    lv_obj_t * obj3;
    obj3 = lv_obj_create(lv_scr_act(), obj2);
    lv_obj_add_style(obj3, LV_OBJ_PART_MAIN, &style_shadow);
    lv_obj_align(obj3, NULL, LV_ALIGN_CENTER, 60, 30);
}
