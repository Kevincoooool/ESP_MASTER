/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-08 09:39:46
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_daily.h
 */


#ifndef _page_daily_
#define _page_daily_

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
#endif
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_camera.h"
#include "esp_http_client.h"



void page_daily_start(void);
void page_daily_end(void);
void move_task_daily(uint8_t move);
#ifdef __cplusplus
} /* extern "C" */
#endif




#endif // _TEST_


