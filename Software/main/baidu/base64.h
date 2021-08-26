/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-07 18:22:35
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-07 18:24:19
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\baidu\base64.h
 */
/*
 * @Descripttion : 
 * @version      : 
 * @Author       : Kevincoooool
 * @Date         : 2021-01-07 14:40:25
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-01-16 16:52:38
 * @FilePath     : \esp-adf\roobo_demo\baiduai\main\base64.h
 */
/*base64.h*/  
#ifndef _BASE64_H  
#define _BASE64_H  
  
#include <stdlib.h>  
#include <string.h>  
#include <stdio.h>  

#ifdef __cplusplus
extern "C" {
#endif

char *base64_encode(uint8_t *str,uint32_t img_len);  
  
uint8_t *bae64_decode(uint8_t *code);  
  
#endif  