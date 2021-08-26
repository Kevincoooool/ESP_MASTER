/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-10 13:27:27
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-06 10:14:28
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_home.h
 */

#ifndef _PAGEHOME_
#define _PAGEHOME_

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
#ifdef __cplusplus
} /* extern "C" */
#endif

void page_home_start(void);
void page_home_end(void);
void update_time(void);
void update_fensi(void);
void update_temp(void);
void move_task_home(uint8_t move);
#endif // _TEST_
