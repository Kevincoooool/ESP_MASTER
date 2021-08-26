/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-03 17:07:05
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-03 17:37:39
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\button.h
 */
/*
 * @Descripttion: 
 * @version: 
 * @Author: Kevincoooool
 * @Date: 2020-06-24 14:09:23
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-04-10 15:11:23
 * @FilePath     : \H1_N32G032\USER\button.h
 */
#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "bsp_button.h"

enum
{
    BT_NONE,
    BT1_DOWN,
    BT1_DOUBLE,
    BT1_LONG,
    BT1_LONGFREE,

    BT2_DOWN,
    BT2_DOUBLE,
    BT2_LONG,
    BT2_LONGFREE,

    BT3_DOWN,
    BT3_DOUBLE,
    BT3_LONG,
    BT3_LONGFREE,
};
extern uint8_t Button_Value;
void Button_Init(void);

#endif
