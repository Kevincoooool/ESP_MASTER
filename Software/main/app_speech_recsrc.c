/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-05-25 16:03:28
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-08 15:14:39
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\app_speech_recsrc.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "xtensa/core-macros.h"
#include "esp_partition.h"
#include "app_speech_srcif.h"
#include "driver/i2s.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "app_main.h"
#include "esp_ns.h"
#include "esp_agc.h"
static void i2s_init(void)
{
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX,        //the mode must be set according to DSP configuration
        .sample_rate = 16000,                         //must be the same as DSP configuration
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, //must be the same as DSP configuration
        .bits_per_sample = 32,                        //must be the same as DSP configuration
        .communication_format = I2S_COMM_FORMAT_I2S,
        .dma_buf_count = 2,
        .dma_buf_len = 300,
        .intr_alloc_flags =  ESP_INTR_FLAG_LEVEL3,
    };
    i2s_pin_config_t pin_config = {
        .bck_io_num = IIS_SCLK,    // IIS_SCLK
        .ws_io_num = IIS_LCLK,    // IIS_LCLK
        .data_out_num = IIS_DSIN, // IIS_DSIN
        .data_in_num = IIS_DOUT   // IIS_DOUT
    };
    i2s_driver_install(1, &i2s_config, 0, NULL);
    i2s_set_pin(1, &pin_config);
    i2s_zero_dma_buffer(1);
}

void recsrcTask(void *arg)
{
    i2s_init();

    src_cfg_t *cfg = (src_cfg_t *)arg;
    size_t samp_len = cfg->item_size * 2 * sizeof(int) / sizeof(int16_t);

    int *samp = malloc(samp_len);

    size_t read_len = 0;

    int16_t *agc_out = malloc(samp_len);

    void *agc_inst = esp_agc_open(3, 16000);
    set_agc_config(agc_inst, 10, 1, 2);

    while (1)
    {
        // if (g_state != WAIT_FOR_WAKEUP)
        // {
        //     vTaskDelay(1000 / portTICK_PERIOD_MS);
        //     continue;
        // }

        i2s_read(1, samp, samp_len, &read_len, portMAX_DELAY);
        for (int x = 0; x < cfg->item_size / 4; x++)
        {
            int s1 = ((samp[x * 4] + samp[x * 4 + 1]) >> 13) & 0x0000FFFF;
            int s2 = ((samp[x * 4 + 2] + samp[x * 4 + 3]) << 3) & 0xFFFF0000;
            samp[x] = s1 | s2;
        }
        esp_agc_process(agc_inst, samp, agc_out,samp_len, 16000);
        xQueueSend(*cfg->queue, samp, portMAX_DELAY);
    }

    vTaskDelete(NULL);
}
