#include "../../../lv_examples.h"
#include <stdio.h>
#if LV_USE_TEXTAREA && LV_USE_KEYBOARD

static void ta_event_cb(lv_obj_t * ta, lv_event_t event);

static lv_obj_t * kb;

/**
 * Automatically format text like a clock. E.g. "12:34"
 * Add the ':' automatically.
 */
void lv_ex_textarea_3(void)
{
    /* Create the text area */
    lv_obj_t * ta = lv_textarea_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(ta, ta_event_cb);
    lv_textarea_set_accepted_chars(ta, "0123456789:");
    lv_textarea_set_max_length(ta, 5);
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_text(ta, "");

    /* Create a keyboard*/
    kb = lv_keyboard_create(lv_scr_act(), NULL);
    lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 2);
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUM);
    lv_keyboard_set_textarea(kb, ta);
}

static void ta_event_cb(lv_obj_t * ta, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        const char * txt = lv_textarea_get_text(ta);
        if(txt[0] >= '0' && txt[0] <= '9' &&
            txt[1] >= '0' && txt[1] <= '9' &&
            txt[2] != ':')
        {
            lv_textarea_set_cursor_pos(ta, 2);
            lv_textarea_add_char(ta, ':');
        }
    }
}

#endif
