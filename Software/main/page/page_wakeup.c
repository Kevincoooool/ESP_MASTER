/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-06 12:17:36
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_wakeup.c
 */
#include "page_wakeup.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "lv_compment.h"
#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"
LV_FONT_DECLARE(myFont);
#define ANIEND                      \
    while (lv_anim_count_running()) \
        lv_task_handler(); //等待动画完成
extern lv_obj_t *scr;
extern lv_obj_t *scr_body;
extern lv_obj_t *label_speech;
extern lv_group_t *group_button;
void wakeup_init(void)
{

    label_speech = lv_label_create(scr, NULL);
	/*Modify the Label's text*/
	
	static lv_style_t style_label_speech;
	lv_style_init(&style_label_speech);

	//Write style state: LV_STATE_DEFAULT for style_label_speech
	lv_style_set_radius(&style_label_speech, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_label_speech, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_bg_grad_color(&style_label_speech, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_bg_grad_dir(&style_label_speech, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_label_speech, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_label_speech, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    lv_obj_set_style_local_text_font(label_speech, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_style_set_text_letter_space(&style_label_speech, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_label_speech, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_label_speech, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_label_speech, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_label_speech, LV_STATE_DEFAULT, 0);

	lv_obj_add_style(label_speech, LV_LABEL_PART_MAIN, &style_label_speech);
    lv_label_set_long_mode(label_speech,LV_LABEL_LONG_BREAK);
	lv_obj_set_pos(label_speech, 0, 240);
	lv_obj_set_size(label_speech, 240, 50);
    lv_label_set_recolor(label_speech, true);
    lv_label_set_text(label_speech, "#0000ff 你好# \n我是酷世DIY制作的ESP32-CAM");
}
compment_t wake_anim;
void page_wakeup_load()
{
    wakeup_init();
    wake_anim.start = 240;
    wake_anim.end = 190;
    wake_anim.obj = label_speech;
    wake_anim.start_t = 300;
    wake_anim.end_t = 300;
    wake_anim.cb = (void *)lv_anim_path_bounce;
    wake_anim.funcb = (void *)lv_obj_set_y;
    wake_anim.next = NULL;
    obj_move(&wake_anim, 1);
    ANIEND
}
void page_wakeup_end()
{
    obj_move(&wake_anim, 0);
    ANIEND
    lv_obj_del(label_speech);
}
void page_wakeup_start()
{
    //获取芯片可用内存
    printf("     esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
    //获取从未使用过的最小内存
    printf("     esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
    page_wakeup_load();
}
