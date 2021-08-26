/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-06-05 10:34:49
 * @FilePath     : \esp-idf\KSDIY_ESPCAM\main\page_wakeup.h
 */


#ifndef _page_wakeup_
#define _page_wakeup_

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
void page_wakeup_start(void);
void page_wakeup_end(void);
#ifdef __cplusplus
} /* extern "C" */
#endif




#endif // _TEST_


