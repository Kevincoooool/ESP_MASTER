#ifndef _BSP_BUTTON_H
#define _BSP_BUTTON_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp_system.h"
#include <esp_err.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include "freertos/task.h"

/** �������±��ú�
	*  ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
	*  ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
	*/
#define KEY_ON 1
#define KEY_OFF 0

#define BTN_NAME_MAX 32 //�������Ϊ32�ֽ�

/* ��������ʱ��40ms, �����������Ϊ20ms
 ֻ��������⵽40ms״̬�������Ϊ��Ч����������Ͱ��������¼�
*/

/* �Ƿ�֧�ֵ���&˫��ͬʱ���ڴ��������ѡ�����궨��Ļ�����˫�����ص���ֻ�����������ӳ���Ӧ��
   ��Ϊ�����жϵ���֮���Ƿ񴥷���˫�������ӳ�ʱ����˫�����ʱ�� BUTTON_DOUBLE_TIME��
   ���������������궨�壬���鹤����ֻ���ڵ���/˫���е�һ����������˫����Ӧ��ʱ��ᴥ��һ�ε�����
   ��Ϊ˫����������һ�ΰ��²����ͷ�֮��Ų����� */

#define SINGLE_AND_DOUBLE_TRIGGER 1

//#define CONTINUOS_TRIGGER    //�Ƿ�֧�����������������Ļ��Ͳ�Ҫ��ⵥ˫���볤����

/* �Ƿ�֧�ֳ����ͷŲŴ��������������궨�壬��ô�����ͷ�֮��Ŵ������γ�����
   �����ڳ���ָ��ʱ���һֱ�������������������� BUTTON_LONG_CYCLE ���� */

#define LONG_FREE_TRIGGER 0
//�Ƿ�֧�ֳ��������д���һ�γ���  ����ɿ��ٳ��������ͷ�
#define LONG_FREE_ENABLE 1

#ifndef BUTTON_DEBOUNCE_TIME
#define BUTTON_DEBOUNCE_TIME 1 //����ʱ��      (n-1)*��������
#endif

#ifndef BUTTON_CONTINUOS_CYCLE
#define BUTTON_CONTINUOS_CYCLE 1 //������������ʱ��  (n-1)*��������
#endif

#ifndef BUTTON_LONG_CYCLE
#define BUTTON_LONG_CYCLE 1 //������������ʱ��  (n-1)*��������
#endif

#ifndef BUTTON_DOUBLE_TIME
#define BUTTON_DOUBLE_TIME 4 //˫�����ʱ��  (n-1)*��������  ������200-600ms
#endif

#ifndef BUTTON_LONG_TIME
#define BUTTON_LONG_TIME 20 /* ����n��((n-1)*�������� ms)����Ϊ�����¼� */
#endif

#define TRIGGER_CB(event)            \
  if (btn->CallBack_Function[event]) \
  btn->CallBack_Function[event]((Button_t *)btn)

typedef void (*Button_CallBack)(void *); /* ���������ص���������Ҫ�û�ʵ�� */

typedef enum
{
  BUTTON_DOWN = 0,
  BUTTON_UP,
  BUTTON_DOUBLE,
  BUTTON_LONG,
  BUTTON_LONG_FREE,
  BUTTON_CONTINUOS,
  BUTTON_CONTINUOS_FREE,
  BUTTON_ALL_RIGGER,
  number_of_event, /* �����ص����¼� */
  NONE_TRIGGER
} Button_Event;

/*
	ÿ��������Ӧ1��ȫ�ֵĽṹ�������
	���Ա������ʵ���˲��Ͷ��ְ���״̬�������
*/
typedef struct button
{
  /* ������һ������ָ�룬ָ���жϰ����ַ��µĺ��� */
  uint8_t (*Read_Button_Level)(void); /* ��ȡ������ƽ��������Ҫ�û�ʵ�� */

  char Name[BTN_NAME_MAX];

  uint8_t Button_State : 4;         /* ������ǰ״̬�����»��ǵ��� */
  uint8_t Button_Last_State : 4;    /* ��һ�εİ���״̬�������ж�˫�� */
  uint8_t Button_Trigger_Level : 2; /* ����������ƽ */
  uint8_t Button_Last_Level : 2;    /* ������ǰ��ƽ */

  uint8_t Button_Trigger_Event; /* ���������¼���������˫���������� */

  Button_CallBack CallBack_Function[number_of_event];

  uint8_t Button_Cycle; /* ������������ */

  uint8_t Timer_Count;   /* ��ʱ */
  uint8_t Debounce_Time; /* ����ʱ�� */

  uint8_t Long_Time; /* �������³���ʱ�� */

  struct button *Next;

} Button_t;

/* ���ⲿ���õĺ������� */

void Button_Create(const char *name,
                   Button_t *btn,
                   uint8_t (*read_btn_level)(void),
                   uint8_t btn_trigger_level);

void Button_Attach(Button_t *btn, Button_Event btn_event, Button_CallBack btn_callback);

void Button_Cycle_Process(Button_t *btn);

void Button_Process(void);

void Button_Delete(Button_t *btn);

void Search_Button(void);

void Get_Button_EventInfo(Button_t *btn);
uint8_t Get_Button_Event(Button_t *btn);
uint8_t Get_Button_State(Button_t *btn);
void Button_Process_CallBack(void *btn);

#endif
