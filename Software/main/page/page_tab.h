/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-10 18:01:14
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-06-10 18:01:42
 * @FilePath     : \esp-idf\KSDIY_ESPCAM\main\page_tab.h
 */


#ifndef _PAGETAB_
#define _PAGETAB_

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
void page_tab_start(void);
void page_tab_end(void);
#ifdef __cplusplus
} /* extern "C" */
#endif




#endif // _TEST_


