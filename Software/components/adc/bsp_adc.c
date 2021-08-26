/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-04 16:13:33
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-17 14:49:39
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\components\adc\bsp_adc.c
 */
#include "bsp_adc.h"


#define NO_OF_SAMPLES 64  //Multisampling

static const adc_channel_t channel = ADC_CHANNEL_5; //GPIO34 if ADC1

void adc_init()
{

    esp_err_t r;

    gpio_num_t adc_gpio_num;

    r = adc1_pad_get_io_num(channel, &adc_gpio_num);
    assert(r == ESP_OK);

    printf("ADC2 channel %d @ GPIO %d\n", channel, adc_gpio_num);

    //be sure to do the init before using adc2.
    printf("adc2_init...\n");
    adc1_config_channel_atten(channel, ADC_ATTEN_11db);

    vTaskDelay(2 / portTICK_PERIOD_MS);

    printf("start conversion.\n");
}
uint32_t get_adc(void)
{
    int read_raw;
    //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++)
    {
        read_raw = adc1_get_raw(channel); //从单个通道获取ADC1读数。
        read_raw += read_raw;
    }
    read_raw /= NO_OF_SAMPLES;

    return read_raw;
}
uint16_t touch_chart[][3] =
    {
        {0, 55, 65},
        {1, 75, 85},
        {2, 0, 2},

};

uint8_t Get_Adc_Button()
{
    int adc_value = get_adc();
    // static uint64_t last_time = 0;
    for (uint8_t i = 0; i < 3; i++)
    {
        // if (adc_value >= touch_chart[i][1] && adc_value <= touch_chart[i][2] && esp_timer_get_time() - last_time > 200000) //限制进入时间
        if (adc_value >= touch_chart[i][1] && adc_value <= touch_chart[i][2])
        {
            // printf("adc_button: %d\n", i + 1);
            // last_time = esp_timer_get_time();
            return i + 1;
        }
    }
    return 0;
}
