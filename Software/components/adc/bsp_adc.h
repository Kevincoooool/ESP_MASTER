/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-04 16:52:39
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-06-04 19:11:49
 * @FilePath     : \esp-idf\KSDIY_ESPCAM\main\app_adc.h
 */
#ifndef _APP_ADC_
#define _APP_ADC_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
// #include "esp_adc_cal.h"

void adc_init();
uint32_t get_adc();
uint8_t Get_Adc_Button();
#endif