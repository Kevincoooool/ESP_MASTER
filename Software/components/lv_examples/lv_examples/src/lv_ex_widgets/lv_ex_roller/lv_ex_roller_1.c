#include "../../../lv_examples.h"
#include <stdio.h>
#if LV_USE_ROLLER

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        printf("Selected month: %s\n", buf);
    }
}


void lv_ex_roller_1(void)
{
    lv_obj_t *roller1 = lv_roller_create(lv_scr_act(), NULL);
    lv_roller_set_options(roller1,
                        "January\n"
                        "February\n"
                        "March\n"
                        "April\n"
                        "May\n"
                        "June\n"
                        "July\n"
                        "August\n"
                        "September\n"
                        "October\n"
                        "November\n"
                        "December",
                        LV_ROLLER_MODE_INIFINITE);

    lv_roller_set_visible_row_count(roller1, 4);
    lv_obj_align(roller1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(roller1, event_handler);
}

#endif
