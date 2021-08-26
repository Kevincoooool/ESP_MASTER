/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-06 10:45:02
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_calendar.h
 */


#ifndef _page_calendar_
#define _page_calendar_

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
void page_calendar_start(void);
void page_calendar_end(void);
void move_task_calendar(uint8_t move);
#ifdef __cplusplus
} /* extern "C" */
#endif




#endif // _TEST_


