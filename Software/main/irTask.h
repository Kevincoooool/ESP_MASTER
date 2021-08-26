#ifndef _IRTASK_
#define _IRTASK_
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"
#include "mynvs.h"
#include "ir_decode.h"
#include "esp_spiffs.h"

#include "driver/gpio.h"

#define RMT_RX_ACTIVE_LEVEL  0   /*!< If we connect with a IR receiver, the data is active low */
#define RMT_TX_CARRIER_EN    1   /*!< Enable carrier for IR transmitter test with IR led */

#define RMT_TX_CHANNEL    1     /*!< RMT channel for transmitter */
#define RMT_TX_GPIO_NUM  27     /*!< GPIO number for transmitter signal */
#define RMT_RX_CHANNEL    3     /*!< RMT channel for receiver */
#define RMT_RX_GPIO_NUM  4     /*!< GPIO number for receiver */
#define RMT_CLK_DIV      100    /*!< RMT counter clock divider  计数器每80M/100=1.25us计数1次*/
#define RMT_TICK_10_US    (80000000/RMT_CLK_DIV/100000)   /*!< RMT counter value for 10 us.(Source clock is APB clock) 可以定时10us*/

#define HEADER_HIGH_9000US    9000                         /*!< NEC protocol header: positive 9ms */
#define HEADER_LOW_4500US     4500                         /*!< NEC protocol header: negative 4.5ms*/
#define HEADER_HIGH_4300US    4300
#define HEADER_LOW_4300US    4300
#define HEADER_HIGH_7300US    7300
#define HEADER_LOW_5800US    5800
#define HEADER_HIGH_3000US   3000
#define HEADER_LOW_3000US   3000
#define HEADER_HIGH_4500US   4500



#define NEC_CONNECT_HIGH_US     646                         /*!< NEC protocol CONNECT: positive 0.6ms*/
#define NEC_CONNECT_LOW_US     20000                          /*!< NEC protocol CONNECT: negative 2ms*/
#define NEC_BIT_ONE_HIGH_US    646                         /*!< NEC protocol data bit 1: positive 0.56ms */
#define NEC_BIT_ONE_LOW_US    1643   /*!< NEC protocol data bit 1: negative 1.69ms */
#define NEC_BIT_ZERO_HIGH_US   646                         /*!< NEC protocol data bit 0: positive 0.56ms */
#define NEC_BIT_ZERO_LOW_US   516  /*!< NEC protocol data bit 0: negative 0.56ms */
#define NEC_BIT_END            646                         /*!< NEC protocol end: positive 0.56ms */
#define NEC_BIT_MARGIN         200                          /*!< NEC parse margin time */

#define NEC_ITEM_DURATION(d)  ((d & 0x7fff)*10/RMT_TICK_10_US)  /*!< Parse duration time from memory register value */
#define NEC_DATA_ITEM_NUM   70  /*!< NEC code item number: header + 35bit data + connect +32bit +end*/
#define RMT_TX_DATA_NUM  2    /*!< NEC tx test data number */
#define rmt_item32_tIMEOUT_US  21000   /*!< RMT receiver timeout value(us) 由于连接码的时间长度大约为20600us所以设置时间长点 */


enum ac_band
{
    band_gree = 0,
    band_meidi = 1,
    songxia = 2,
    band_dajin = 3,
    band_haier = 4,
    band_haixin = 5,
    band_aux = 6,
    band_max
};
enum ac_pro_code
{
    code_1 = 0,
    code_2 = 1,
    code_3 = 2,
    code_4 = 3,
    code_5 = 4,
   
    code_maxa
};

//品牌+协议编码 = 红外码库
struct AC_Control
{
    uint8_t code;
    t_remote_ac_status status;  //空调控制结构体
};

//空调结构体，用于控制空调
static struct AC_Control ac_handle; 

//接收到的红外信号
struct RX_signal
{
    uint32_t item_num;  //item数量
    uint32_t lowlevel;  //低电平时间 us
    uint32_t highlevel_1;   //高电平1的时间
    uint32_t highlevel_0;   //高电平0的时间
    uint32_t encode;    //由0和1组成的编码
};




TaskHandle_t ir_tx_handle;
uint8_t ac_set_code_lib(uint8_t band, uint8_t pro_code);
int ac_set_temp(int temp);
int ac_open(bool open);
int ac_set_wind_speed(int speed);
int ac_set_swing(bool open);
void ac_set_type(uint8_t ac);
void ac_set_type1(uint8_t ac,uint8_t code);
void ir_study();    //开启学习
int IR_init();  //初始化
int storage_init(); //存储系统，文件系统初始化


void rmt_ir_txTask(void *agr);
void rmt_ir_rxTask(void *agr);
#endif