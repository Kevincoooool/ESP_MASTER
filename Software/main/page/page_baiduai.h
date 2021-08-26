/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-17 16:26:47
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_baiduai.h
 */


#ifndef _page_baiduai_
#define _page_baiduai_

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

extern uint8_t baiduai_en, color_en, face_en;
int baidu_img_ai(void);
int baidu_ocr_ai(void);
void get_img_token(void);
void get_ocr_token(void);
void BaiduAI_Task(void *pvParameters);
void page_baiduai_start(void);
void page_baiduai_end(void);
void move_task_baiduai(uint8_t move);

#ifdef __cplusplus
} /* extern "C" */
#endif




#endif // _TEST_


