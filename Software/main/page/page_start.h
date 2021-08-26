/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-10 12:56:52
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-21 17:22:54
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_start.h
 */

#ifndef _TEST_
#define _TEST_

#ifdef __cplusplus
extern "C"
{
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
#include <stdlib.h>
#include <string.h>
#include "spage.h"
#include "page_home.h"
#include "myFont.h"
#include "lv_compment.h"
#include "esp_log.h"
#include "lv_port_indev.h"
#include "esp_err.h"
#include "gui_time.h"
#include "http.h"
#include "app_wifi.h"

    struct _ksdiy_ico
    {
        lv_obj_t *lv_wifi;   //wifi图标
        lv_obj_t *lv_clock;  //闹钟图标
        lv_obj_t *lv_button; //按键图标
        lv_obj_t *lv_web;    //mqtt图标
        lv_obj_t *lv_temp;   //mqtt图标
        lv_obj_t *lv_kv;     //mqtt图标
    };
    struct _ksdiy_state
    {
        /* data */

        uint8_t wifi : 4;       //wifi连接情况
        uint8_t sys_button : 6; //切换button使用权
        uint8_t clock : 1;
        uint8_t web_set : 1;
        uint8_t web_cam : 1;
        uint8_t cam_vflip : 1;
        uint8_t cam_hmirror : 1;
        uint8_t daiding : 1;
        uint8_t upcloud : 1;
    };
    typedef struct _clock_t
    {
        uint8_t state : 1;
        uint8_t h : 7;
        uint8_t m;
    } ksdiy_clock_t;
    struct _ksdiy_clock
    {
        /* data */
        ksdiy_clock_t clock[3];
    };
    struct _ksdiy_time
    {
        /* data */
        uint8_t h : 7;
        uint8_t m;
        uint8_t s : 1;
    };
    struct _ksdiy_fensi
    {
        int follow;
        int following;
    };
    struct _ksdiy_sys_t
    {
        page_t *page_now[8];       //当前页
        page_t *page;              //当前页
        struct _ksdiy_ico ico;     //状态栏图标
        struct _ksdiy_state state; //系统状态信息
        lv_obj_t *lv_bsgbox;       //信息通知盒
        uint32_t user_data;        //切换界面
        lv_group_t *group;         //按键组
        struct _ksdiy_clock clock; //闹钟
        struct tm timeinfo;
        struct _ksdiy_fensi wp;
    };
    void smartconfig_set(void);
    void switch_wifi(void);
    void page_init(void);
    void wifi_rate(void);
    void dev_init(void);
    void lv_message(const char *str, uint16_t t);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif // _TEST_
