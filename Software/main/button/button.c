/*
 * @Descripttion : 
 * @version      : 
 * @Author       : Kevincoooool
 * @Date         : 2021-03-25 17:15:11
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-03 17:39:00
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\button.c
 */

#include "button.h"
#include "bsp_adc.h"
/*******************************************************************
 *                          变量声明                               
 *******************************************************************/

Button_t Button_UP;
Button_t Button_ENTER;
Button_t Button_DOWN;

uint8_t Button_Value = BT_NONE;
//按键一单击
void Btn1_Down_CallBack(void *btn)
{

	Button_Value = BT1_DOWN;
	printf("Button 1 down!\n");
}
//按键一双击
void Btn1_Double_CallBack(void *btn)
{
	Button_Value = BT1_DOUBLE;
	printf("Button 1 Double!\n");
}
//按键一双击
void Btn1_Long_CallBack(void *btn)
{
	Button_Value = BT1_LONG;
	printf("Button 1 Long!\n");
}
void Btn1_Long_Free_CallBack(void *btn)
{
	Button_Value = BT1_LONGFREE;
	printf("Button 1 Long_Free!\n");
}
///////////////////////////////////////////////////////
//按键二单击
void Btn2_Down_CallBack(void *btn)
{
	Button_Value = BT2_DOWN;
	printf("Button 2 down!\n");
}
//按键一双击
void Btn2_Double_CallBack(void *btn)
{
	Button_Value = BT2_DOUBLE;
	printf("Button 2 Double!\n");
}
//按键一双击
void Btn2_Long_CallBack(void *btn)
{
	Button_Value = BT2_LONG;
	printf("Button 2Long!\n");
}
void Btn2_Long_Free_CallBack(void *btn)
{
	Button_Value = BT2_LONGFREE;
	printf("Button 2 Long_Free!\n");
}
///////////////////////////////////////////////////////
//按键三单击
void Btn3_Down_CallBack(void *btn)
{
	Button_Value = BT3_DOWN;
	printf("Button 3 down!\n");
}
//按键一双击
void Btn3_Double_CallBack(void *btn)
{
	Button_Value = BT3_DOUBLE;
	printf("Button 3 Double!\n");
}
//按键一双击
void Btn3_Long_CallBack(void *btn)
{
	Button_Value = BT3_LONG;
	printf("Button 3 Long!\n");
}
void Btn3_Long_Free_CallBack(void *btn)
{
	Button_Value = BT3_LONGFREE;
	printf("Button 3 Long_Free!\n");
}
uint8_t Read_KEY1_Level(void)
{
	return (Get_Adc_Button() == 3) ? 1 : 0;
}

uint8_t Read_KEY2_Level(void)
{
	return (Get_Adc_Button() == 1) ? 1 : 0;
}
uint8_t Read_KEY3_Level(void)
{
	return (Get_Adc_Button() == 2) ? 1 : 0;
}

void Button_Init(void)
{

	Button_Create("Button_UP",
				  &Button_UP,
				  Read_KEY1_Level,
				  KEY_ON);
	Button_Attach(&Button_UP, BUTTON_DOWN, Btn1_Down_CallBack);			//单击
	Button_Attach(&Button_UP, BUTTON_DOUBLE, Btn1_Double_CallBack);		//双击
	Button_Attach(&Button_UP, BUTTON_LONG, Btn1_Long_CallBack);			//长按按下
	Button_Attach(&Button_UP, BUTTON_LONG_FREE, Btn1_Long_Free_CallBack); //长按松开

	Button_Create("Button_ENTER",
				  &Button_ENTER,
				  Read_KEY2_Level,
				  KEY_ON);
	Button_Attach(&Button_ENTER, BUTTON_DOWN, Btn2_Down_CallBack);			//单击
	Button_Attach(&Button_ENTER, BUTTON_DOUBLE, Btn2_Double_CallBack);		//双击
	Button_Attach(&Button_ENTER, BUTTON_LONG, Btn2_Long_CallBack);			//长按按下
	Button_Attach(&Button_ENTER, BUTTON_LONG_FREE, Btn2_Long_Free_CallBack); //长按松开

	Button_Create("Button_DOWN",
				  &Button_DOWN,
				  Read_KEY3_Level,
				  KEY_ON);
	Button_Attach(&Button_DOWN, BUTTON_DOWN, Btn3_Down_CallBack);			//单击
	Button_Attach(&Button_DOWN, BUTTON_DOUBLE, Btn3_Double_CallBack);		//双击
	Button_Attach(&Button_DOWN, BUTTON_LONG, Btn3_Long_CallBack);			//长按按下
	Button_Attach(&Button_DOWN, BUTTON_LONG_FREE, Btn3_Long_Free_CallBack); //长按松开

	Get_Button_Event(&Button_UP);
	Get_Button_Event(&Button_ENTER);
	Get_Button_Event(&Button_DOWN);
}
