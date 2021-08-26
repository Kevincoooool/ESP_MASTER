/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-10 13:58:24
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-17 14:54:57
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\spage.h
 */
#ifndef _SPAGE_
#define _SPAGE_
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "app_anim.h"
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "../../lvgl/lvgl.h"
#include "../../lv_ex_conf.h"
#endif



typedef struct _page_t
{
    struct _page_t * last;
    unsigned char index;
    void (*start)();//开始函数
    void (*end)();//退出函数
    struct _page_t * next;
}page_t;
page_t * add_page(page_t * page);
page_t * lv_page_menu_init();

void lv_ksdiy_web_init(void);
void lv_ksdiy_cam_init(void);
void clock_switch_save(uint8_t i);
void web_switch_save(uint8_t i);
void cam_switch_save(uint8_t i);
void cam_vflip_switch_save(uint8_t i);
void cam_hmirror_switch_save(uint8_t i);
void web_temp_save(uint8_t i);
void ksdiy_sys_info_init(void);

void lv_ico_web_show(uint8_t state);
void lv_ico_clock_show(uint8_t state);
void lv_ico_temp_show(uint8_t state);
void save_clock(uint8_t clock_index,uint8_t s,uint8_t h,uint8_t m);
#endif // _TEST_

