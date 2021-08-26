/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-09 15:02:18
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-06-09 15:20:08
 * @FilePath     : \esp-idf\KSDIY_ESPCAM\components\read_nvs\updata.h
 */
#ifndef _UPDATA_
#define _UPDATA_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void save_web_data(char *data);
int save_nvs(const char * str,char * data);
int read_nvs(const char * str,char * data);
#endif