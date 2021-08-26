/************************************************************
  * @brief   ��������
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    button.c
  ***********************************************************/
#include "bsp_button.h"

/*******************************************************************
 *                          ��������                               
 *******************************************************************/

static struct button *Head_Button = NULL;
bool long_press = false;
/*******************************************************************
 *                         ��������     
 *******************************************************************/
static char *StrnCopy(char *dst, const char *src, uint32_t n);
static void Print_Btn_Info(Button_t *btn);
static void Add_Button(Button_t *btn);

/************************************************************
  * @brief   ��������
	* @param   name : ��������
	* @param   btn : �����ṹ��
  * @param   read_btn_level : ������ƽ��ȡ��������Ҫ�û��Լ�ʵ�ַ���uint8_t���͵ĵ�ƽ
  * @param   btn_trigger_level : ����������ƽ
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Button_Create(const char *name,
                   Button_t *btn,
                   uint8_t (*read_btn_level)(void),
                   uint8_t btn_trigger_level)
{
  if (btn == NULL)
  {
  }

  // memset(btn, 0, sizeof(struct button)); //����ṹ����Ϣ�������û���֮ǰ���

  StrnCopy(btn->Name, name, BTN_NAME_MAX); /* ������������ */

  btn->Button_State = NONE_TRIGGER;                  //����״̬
  btn->Button_Last_State = NONE_TRIGGER;             //������һ��״̬
  btn->Button_Trigger_Event = NONE_TRIGGER;          //���������¼�
  btn->Read_Button_Level = read_btn_level;           //��������ƽ����
  btn->Button_Trigger_Level = btn_trigger_level;     //����������ƽ
  btn->Button_Last_Level = btn->Read_Button_Level(); //������ǰ��ƽ
  btn->Debounce_Time = 0;

  //USART3_Put_String("Button create success!\r\n");

  Add_Button(btn); //������ʱ����ӵ���������

  Print_Btn_Info(btn); //��ӡ��Ϣ
}

/************************************************************
  * @brief   ���������¼���ص�����ӳ����������
	* @param   btn : �����ṹ��
	* @param   btn_event : ���������¼�
  * @param   btn_callback : ��������֮��Ļص�����������Ҫ�û�ʵ��
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
void Button_Attach(Button_t *btn, Button_Event btn_event, Button_CallBack btn_callback)
{
  if (btn == NULL)
  {
  }

  if (BUTTON_ALL_RIGGER == btn_event)
  {
    for (uint8_t i = 0; i < number_of_event - 1; i++)
      btn->CallBack_Function[i] = btn_callback; //�����¼������Ļص����������ڴ������¼�
  }
  else
  {
    btn->CallBack_Function[btn_event] = btn_callback; //�����¼������Ļص����������ڴ������¼�
  }
}

/************************************************************
  * @brief   ɾ��һ���Ѿ������İ���
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Button_Delete(Button_t *btn)
{
  struct button **curr;
  for (curr = &Head_Button; *curr;)
  {
    struct button *entry = *curr;
    if (entry == btn)
    {
      *curr = entry->Next;
    }
    else
    {
      curr = &entry->Next;
    }
  }
}

/************************************************************
  * @brief   ��ȡ�����������¼�
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
void Get_Button_EventInfo(Button_t *btn)
{
  //�����¼������Ļص����������ڴ������¼�
  for (uint8_t i = 0; i < number_of_event - 1; i++)
  {
    if (btn->CallBack_Function[i] != 0)
    {

      printf("Button_Event:%d", i);
    }
  }
}

uint8_t Get_Button_Event(Button_t *btn)
{
  return (uint8_t)(btn->Button_Trigger_Event);
}

/************************************************************
  * @brief   ��ȡ�����������¼�
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
uint8_t Get_Button_State(Button_t *btn)
{
  return (uint8_t)(btn->Button_State);
}

/************************************************************
  * @brief   �������ڴ�����
  * @param   btn:����İ���
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    ������һ�����ڵ��ô˺�������������Ϊ20~50ms
  ***********************************************************/
void Button_Cycle_Process(Button_t *btn)
{
  uint8_t current_level = (uint8_t)btn->Read_Button_Level(); //��ȡ��ǰ������ƽ
  //������ƽ�����仯
  if ((current_level != btn->Button_Last_Level) && (++(btn->Debounce_Time) >= BUTTON_DEBOUNCE_TIME))
  {
    btn->Button_Last_Level = current_level; //���µ�ǰ������ƽ
    btn->Debounce_Time = 0;                 //ȷ�����ǰ���

    //�����һ�ε�״̬������û�����µģ��ı䰴��״̬Ϊ����(�״ΰ���/˫������)
    if (((btn->Button_State == NONE_TRIGGER) || (btn->Button_State == BUTTON_DOUBLE)))
    {
      btn->Button_State = BUTTON_DOWN;
    }
    //�ͷŰ���
    else if (btn->Button_State == BUTTON_DOWN)
    {
      if (!long_press)
      {

        btn->Button_State = BUTTON_UP;
        TRIGGER_CB(BUTTON_UP); // �����ͷ�
      }
      else
      {
        long_press = false;
        btn->Button_State = BUTTON_UP;
        btn->Button_Trigger_Event = BUTTON_LONG_FREE;
      }

      //USART3_Put_String("release button !\r\n");
    }
  }

  switch (btn->Button_State)
  {
  case BUTTON_DOWN: // ����״̬
  {
    if (btn->Button_Last_Level == btn->Button_Trigger_Level) //��������
    {
#if CONTINUOS_TRIGGER //֧����������

      if (++(btn->Button_Cycle) >= BUTTON_CONTINUOS_CYCLE)
      {
        btn->Button_Cycle = 0;
        btn->Button_Trigger_Event = BUTTON_CONTINUOS;
        TRIGGER_CB(BUTTON_CONTINUOS); //����
        //USART3_Put_String("lianxu button !\r\n");
      }

#else

      btn->Button_Trigger_Event = BUTTON_DOWN;

      if (++(btn->Long_Time) >= BUTTON_LONG_TIME) //�ͷŰ���ǰ���´����¼�Ϊ����
      {
#if LONG_FREE_TRIGGER

        btn->Button_Trigger_Event = BUTTON_LONG;

#elif LONG_FREE_ENABLE
        if (long_press == false)
        {

          TRIGGER_CB(BUTTON_LONG); //����
          long_press = true;
          btn->Button_Trigger_Event = BUTTON_LONG_FREE;
        }

#else

        if (++(btn->Button_Cycle) >= BUTTON_LONG_CYCLE) //������������������
        {
          btn->Button_Cycle = 0;
          btn->Button_Trigger_Event = BUTTON_LONG;
          TRIGGER_CB(BUTTON_LONG); //����
        }
#endif

        if (btn->Long_Time == 0xFF) //����ʱ�����
        {
          btn->Long_Time = BUTTON_LONG_TIME;
        }
        //USART3_Put_String("long down count!\r\n");
      }

#endif
    }

    break;
  }

  case BUTTON_UP: // ����״̬
  {
    if (btn->Button_Trigger_Event == BUTTON_DOWN) //��������
    {
      if ((btn->Timer_Count <= BUTTON_DOUBLE_TIME) && (btn->Button_Last_State == BUTTON_DOUBLE)) // ˫��
      {
        btn->Button_Trigger_Event = BUTTON_DOUBLE;
        TRIGGER_CB(BUTTON_DOUBLE);
        //USART3_Put_String("double click button !\r\n");
        btn->Button_State = NONE_TRIGGER;
        btn->Button_Last_State = NONE_TRIGGER;
      }
      else
      {
        btn->Timer_Count = 0;
        btn->Long_Time = 0; //��ⳤ��ʧ�ܣ���0

#if (SINGLE_AND_DOUBLE_TRIGGER == 0)
        TRIGGER_CB(BUTTON_DOWN); //����
#endif
        btn->Button_State = BUTTON_DOUBLE;
        btn->Button_Last_State = BUTTON_DOUBLE;
      }
    }
#if !LONG_FREE_ENABLE
    else if (btn->Button_Trigger_Event == BUTTON_LONG)
    {
#if LONG_FREE_TRIGGER
      TRIGGER_CB(BUTTON_LONG); //����

#endif
      btn->Long_Time = 0;
      btn->Button_State = NONE_TRIGGER;
      long_press = false;
      btn->Button_Last_State = BUTTON_LONG;
      //USART3_Put_String("long click button !\r\n");
    }
#endif
    else if (btn->Button_Trigger_Event == BUTTON_LONG_FREE)
    {
#if LONG_FREE_ENABLE

      TRIGGER_CB(BUTTON_LONG_FREE); //�����ͷ�

#endif
      btn->Long_Time = 0;
      btn->Button_State = NONE_TRIGGER;
      long_press = false;
      btn->Button_Last_State = BUTTON_LONG_FREE;
      //USART3_Put_String("long_free click button !\r\n");
    }
#if CONTINUOS_TRIGGER
    else if (btn->Button_Trigger_Event == BUTTON_CONTINUOS) //����
    {
      btn->Long_Time = 0;
      TRIGGER_CB(BUTTON_CONTINUOS_FREE); //�����ͷ�
      btn->Button_State = NONE_TRIGGER;
      btn->Button_Last_State = BUTTON_CONTINUOS;
    }
#endif

    break;
  }

  case BUTTON_DOUBLE:
  {
    btn->Timer_Count++; //ʱ���¼
    if (btn->Timer_Count >= BUTTON_DOUBLE_TIME)
    {
      btn->Button_State = NONE_TRIGGER;
      btn->Button_Last_State = NONE_TRIGGER;
    }
#if SINGLE_AND_DOUBLE_TRIGGER

    if ((btn->Timer_Count >= BUTTON_DOUBLE_TIME) && (btn->Button_Last_State != BUTTON_DOWN))
    {
      btn->Timer_Count = 0;
      TRIGGER_CB(BUTTON_DOWN); //����
      btn->Button_State = NONE_TRIGGER;
      btn->Button_Last_State = BUTTON_DOWN;
    }

#endif

    break;
  }

  default:
    break;
  }
}

/************************************************************
  * @brief   �����ķ�ʽɨ�谴�������ᶪʧÿ������
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    �˺���Ҫ���ڵ��ã�����20-50ms����һ��
  ***********************************************************/
void Button_Process(void)
{
  struct button *pass_btn;
  for (pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
  {
    Button_Cycle_Process(pass_btn);
  }
}

/************************************************************
  * @brief   ��������
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Search_Button(void)
{
  struct button *pass_btn;
  for (pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
  {
    printf("button node have %s", pass_btn->Name);
  }
}

/**************************** �������ڲ����ú��� ********************/

/************************************************************
  * @brief   ����ָ�������ַ���
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
static char *StrnCopy(char *dst, const char *src, uint32_t n)
{
  if (n != 0)
  {
    char *d = dst;
    const char *s = src;
    do
    {
      if ((*d++ = *s++) == 0)
      {
        while (--n != 0)
          *d++ = 0;
        break;
      }
    } while (--n != 0);
  }
  return (dst);
}

/************************************************************
  * @brief   ��ӡ���������Ϣ
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
static void Print_Btn_Info(Button_t *btn)
{

  printf("\nbutton struct information:\n\
              btn->Name:%s \n\
              btn->Button_State:%d \n\
              btn->Button_Trigger_Event:%d \n\
              btn->Button_Trigger_Level:%d \n\
              btn->Button_Last_Level:%d \n\
              ",
         btn->Name,
         btn->Button_State,
         btn->Button_Trigger_Event,
         btn->Button_Trigger_Level,
         btn->Button_Last_Level);
  Search_Button();
}
/************************************************************
  * @brief   ʹ�õ�����������������
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
static void Add_Button(Button_t *btn)
{
  btn->Next = Head_Button;
  Head_Button = btn;
}
