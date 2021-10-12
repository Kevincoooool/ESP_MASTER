/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-09-30 16:34:21
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-10-12 11:34:49
 * @FilePath     : \esp_master\components\esp32-camera\driver\private_include\sccb.h
 */
/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * SCCB (I2C like) driver.
 *
 */
#ifndef __SCCB_H__
#define __SCCB_H__
#include <stdint.h>
int SCCB_Init(int pin_sda, int pin_scl);
int SCCB_Deinit(void);
uint8_t SCCB_Probe();
uint8_t SCCB_Read(uint8_t slv_addr, uint8_t reg);
uint8_t SCCB_Write(uint8_t slv_addr, uint8_t reg, uint8_t data);
uint8_t SCCB_Read16(uint8_t slv_addr, uint16_t reg);
uint8_t SCCB_Write16(uint8_t slv_addr, uint16_t reg, uint8_t data);
#endif // __SCCB_H__
