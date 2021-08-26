/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-05-25 14:57:14
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-06-10 14:12:24
 * @FilePath     : \esp-idf\KSDIY_ESPCAM\main\app_wifi.h
 */
#ifndef _APP_WIFI_H_
#define _APP_WIFI_H_

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_smartconfig.h"
#include "nvs_flash.h"
#include "updata.h"
#ifdef __cplusplus
extern "C"{
#endif

extern EventGroupHandle_t g_wifi_event_group;

esp_err_t app_wifi_init(void);
esp_err_t wifi_init(void);
esp_err_t initialise_wifi(void);
esp_err_t wifi_connet(void);
void smartconfig_example_task(void * parm);
char * get_ip(void);
char * get_ssid(void);
#ifdef __cplusplus
}
#endif
#endif