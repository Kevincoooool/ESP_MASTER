/**
 * @file lvgl_helpers.h
 */

#ifndef LVGL_HELPERS_H
#define LVGL_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>

#include "lvgl_spi_conf.h"
#include "lvgl_tft/disp_driver.h"

/*********************
 *      DEFINES
 *********************/
#if defined (CONFIG_LV_TFT_DISPLAY_CONTROLLER_ST7789)
#define DISP_BUF_SIZE  (LV_HOR_RES_MAX * 120)
#elif defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_ST7735S
#define DISP_BUF_SIZE  (LV_HOR_RES_MAX * 40)
#elif defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_HX8357
#define DISP_BUF_SIZE  (LV_HOR_RES_MAX * 40)
#elif defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SH1107
#define DISP_BUF_SIZE  (CONFIG_LV_DISPLAY_WIDTH*CONFIG_LV_DISPLAY_HEIGHT)
#elif defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_ILI9481
#define DISP_BUF_SIZE  (LV_HOR_RES_MAX * 40)
#elif defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_ILI9486
#define DISP_BUF_SIZE  (LV_HOR_RES_MAX * 40)
#elif defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_ILI9488
#define DISP_BUF_SIZE  (LV_HOR_RES_MAX * 40)
#elif defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_ILI9341
#define DISP_BUF_SIZE  (LV_HOR_RES_MAX * 64)
#elif defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306
#define DISP_BUF_SIZE  (CONFIG_LV_DISPLAY_WIDTH*CONFIG_LV_DISPLAY_HEIGHT)
#elif defined (CONFIG_LV_TFT_DISPLAY_CONTROLLER_FT81X)
#define DISP_BUF_LINES  40
#define DISP_BUF_SIZE  (LV_HOR_RES_MAX * DISP_BUF_LINES)
#elif defined (CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820)
#define DISP_BUF_SIZE (CONFIG_LV_DISPLAY_HEIGHT * IL3820_COLUMNS)
#elif defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_RA8875
#define DISP_BUF_SIZE  (LV_HOR_RES_MAX * 40)
#elif defined (CONFIG_LV_TFT_DISPLAY_CONTROLLER_GC9A01)
#define DISP_BUF_SIZE  (LV_HOR_RES_MAX * 40)
#elif defined (CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A)
#define DISP_BUF_SIZE ((CONFIG_LV_DISPLAY_HEIGHT * CONFIG_LV_DISPLAY_WIDTH) / 8) // 5KB
#elif defined (CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D)
#define DISP_BUF_SIZE ((CONFIG_LV_DISPLAY_HEIGHT * CONFIG_LV_DISPLAY_WIDTH) / 8) // 2888 bytes
#else
#error "No display controller selected"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* Initialize detected SPI and I2C bus and devices */
void lvgl_driver_init(void);

/* Initialize SPI master  */
bool lvgl_spi_driver_init(int host, int miso_pin, int mosi_pin, int sclk_pin,
    int max_transfer_sz, int dma_channel, int quadwp_pin, int quadhd_pin);
/* Initialize I2C master  */
bool lvgl_i2c_driver_init(int port, int sda_pin, int scl_pin, int speed);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVGL_HELPERS_H */
