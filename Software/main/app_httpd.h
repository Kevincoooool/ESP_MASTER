/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-18 15:35:45
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-06-18 15:43:00
 * @FilePath     : \esp-idf\KSDIY_ESPCAM\main\app_httpd.h
 */
// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _CAMERA_HTTPD_H_
#define _CAMERA_HTTPD_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdio.h"
void app_httpd_main();
uint8_t start_cam_web();
void stop_cam_web();
#ifdef __cplusplus
}
#endif

#endif /* _CAMERA_HTTPD_H_ */
