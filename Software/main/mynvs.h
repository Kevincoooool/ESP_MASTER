/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-08-18 10:12:04
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-18 10:12:48
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\mynvs.h
 */
/*
 * @Author: your name
 * @Date: 2021-03-06 11:04:17
 * @LastEditTime: 2021-03-20 10:35:48
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \esp-adf\examples\myapp\off_asr\main\periph\mynvs.h
 */
#ifndef _MYNVS_H
#define _MYNVS_H

#include "nvs_flash.h"
#include "nvs.h"
#include "irTask.h"
#include "esp_wifi.h"

#define AC_DEFAULT "ac-code"
#define IR_STORAGE_NAMESPACE "ir_data"
#define WIFI_STORAGE_NAMESPACE "wifi_data"
#define AC_CODE_NAMESPACE "ac_code"

uint8_t *nvs_get_ac_lib(const char *key);
esp_err_t nvs_save_ac_code(uint8_t code, const char *key);
esp_err_t nvs_save_items(rmt_item32_t *item, size_t items_size, const char *name);
rmt_item32_t *nvs_get_items(size_t *item_size, const char *key);
esp_err_t nvs_delete_items(const char *key);



#endif