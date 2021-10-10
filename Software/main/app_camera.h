/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-05-25 09:20:06
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-10-06 14:48:12
 * @FilePath     : \esp_master\main\app_camera.h
 */
#ifndef MATERIALS_CLASSIFIER_ESP_APP_CAMERA_ESP_H_
#define MATERIALS_CLASSIFIER_ESP_APP_CAMERA_ESP_H_

#include "esp_camera.h"
#include "esp_log.h"
#include "esp_system.h"
#include "sensor.h"

/**
 * PIXFORMAT_RGB565,    // 2BPP/RGB565
 * PIXFORMAT_YUV422,    // 2BPP/YUV422
 * PIXFORMAT_GRAYSCALE, // 1BPP/GRAYSCALE
 * PIXFORMAT_JPEG,      // JPEG/COMPRESSED
 * PIXFORMAT_RGB888,    // 3BPP/RGB888
 */
#define CAMERA_PIXEL_FORMAT PIXFORMAT_RGB565

/*
 * FRAMESIZE_96X96,    // 96x96
 * FRAMESIZE_QQVGA,    // 160x120
 * FRAMESIZE_QQVGA2,   // 128x160
 * FRAMESIZE_QCIF,     // 176x144
 * FRAMESIZE_HQVGA,    // 240x176
 * FRAMESIZE_240X240    // 240x240
 * FRAMESIZE_QVGA,     // 320x240
 * FRAMESIZE_CIF,      // 400x296
 * FRAMESIZE_VGA,      // 640x480
 * FRAMESIZE_SVGA,     // 800x600
 * FRAMESIZE_XGA,      // 1024x768
 * FRAMESIZE_SXGA,     // 1280x1024
 * FRAMESIZE_UXGA,     // 1600x1200
 */
#define CAMERA_FRAME_SIZE FRAMESIZE_240X240


#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM     21
#define SIOC_GPIO_NUM     19

#define Y9_GPIO_NUM       36
#define Y8_GPIO_NUM       39
#define Y7_GPIO_NUM       34
#define Y6_GPIO_NUM       35
#define Y5_GPIO_NUM       32
#define Y4_GPIO_NUM       13
#define Y3_GPIO_NUM       25
#define Y2_GPIO_NUM       26
#define VSYNC_GPIO_NUM    14
#define HREF_GPIO_NUM     12
#define PCLK_GPIO_NUM    27
#define FLIP_CAMERA      1

#define I2C_SDA             21
#define I2C_SCL             19

#define IIS_SCK             2
#define IIS_WS              22
#define IIS_DOUT            15

#define TFT_CS_PIN          5
#define TFT_DC_PIN          4
#define TFT_MOSI_PIN        23
#define TFT_MISO_PIN        -1
#define TFT_SCLK_PIN        18
#define TFT_BL_PIN          -1

#define SDCARD_CS_PIN       0

// #define ENABLE_IP5306
#define ENABLE_TFT

#define XCLK_FREQ       20000000

#ifdef __cplusplus
extern "C" {
#endif

void app_camera_init();
void web_camera_init();
#ifdef __cplusplus
}
#endif
#endif