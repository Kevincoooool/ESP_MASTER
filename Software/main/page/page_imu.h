/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-06 14:22:21
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_imu.h
 */


#ifndef _page_imu_
#define _page_imu_

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
extern uint8_t cam_en, color_en, face_en,imu_en;
void Imu_Task(void *pvParameters);
void page_imu_start(void);
void page_imu_end(void);
// void move_task_cam(uint8_t move);
#ifdef __cplusplus
} /* extern "C" */
#endif




#endif // _TEST_


