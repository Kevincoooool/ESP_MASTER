/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-22 17:39:38
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-06-22 17:51:23
 * @FilePath     : \esp-idf\KSDIY_ESPCAM\main\qr_recoginize.h
 */
/*
 * qr_recoginize.h
 *
 *  Created on: 2017年12月31日
 *      Author: sky
 */

#ifndef MAIN_QR_RECOGINIZE_H_
#define MAIN_QR_RECOGINIZE_H_

enum{
	RECONGIZE_OK,
	RECONGIZE_FAIL
};

// void qr_recoginze(void *pdata);
//int qr_recoginze() ;
void dump_info(struct quirc *q);
void qr_recoginze(uint8_t *pdata,int32_t len) ;
#endif /* MAIN_QR_RECOGINIZE_H_ */
