/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-05-25 14:56:58
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-06-11 17:23:04
 * @FilePath     : \esp-idf\KSDIY_ESPCAM\main\app_main.h
 */
#include "app_camera.h"
#include "app_wifi.h"
#include "app_speech_srcif.h"

#define VERSION "0.9.0"

typedef enum
{
    WAIT_FOR_WAKEUP,
    WAIT_FOR_CONNECT,
    START_DETECT,
    START_RECOGNITION,
    START_ENROLL,
    START_DELETE,

} en_fsm_state;

extern en_fsm_state g_state;
extern int g_is_enrolling;
extern int g_is_deleting;
