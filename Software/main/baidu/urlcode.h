/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-07 18:22:42
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-07 18:23:52
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\baidu\urlcode.h
 */
/*
 * @Descripttion : 
 * @version      : 
 * @Author       : Kevincoooool
 * @Date         : 2021-01-07 14:40:25
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-01-12 09:01:25
 * @FilePath     : \n_esp-adf\1_take_pic_http_to_cloud\main\urlcode.h
 */
/*base64.h*/
#ifndef _URLCODE_H
#define _URLCODE_H

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

int URLEncode(const char *str, const int strSize, char *result, const int resultSize);
#endif