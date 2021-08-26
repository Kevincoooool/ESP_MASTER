/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-06 14:22:21
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_fft.h
 */


#ifndef _page_fft_
#define _page_fft_

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
void move_task_fft(uint8_t move);
void page_fft_start(void);
void page_fft_end(void);
void FFT_Task(void *arg);

#ifdef __cplusplus
} /* extern "C" */
#endif




#endif // _TEST_


