/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-05-25 09:20:06
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-21 17:22:42
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\app_main.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_attr.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"

#include "driver/gpio.h"
#include "bsp_adc.h"
#include "nvs_flash.h"
// screen
#include "st7789.h"
#include "myFont.h"
#include "myimg.h"
// camera
#include "app_main.h"
#include "app_wifi.h"
#include "app_camera.h"
#include "app_httpd.h"
#include "app_speech_srcif.h"
#include "app_face.h"
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "lv_port_indev.h"
#include "lv_examples/src/lv_demo_music/lv_demo_music.h"
#include "web_server.h"
#include "gui_time.h"
#include "page_baiduai.h"
#include "page_menu.h"
#include "page_daily.h"
#include "page_calendar.h"
#include "page_start.h"
#include "page_game_2048.h"
#include "page_game_snake.h"
#include "page_tab.h"
#include "page_cam.h"
#include "page_home.h"
#include "page_fft.h"
#include "EasyTracer.h"
#include "qr_recoginize.h"
#include "quirc_internal.h"

#include "mpu6050.h"
#include "quirc.h"
#include "button.h"
#include "irTask.h"
#include "ir_decode.h"
#define TAG "APP_MAIN"
#define INTERVAL 400

LV_FONT_DECLARE(myFont);
LV_IMG_DECLARE(taikong001);
LV_IMG_DECLARE(taikong002);
LV_IMG_DECLARE(taikong003);
LV_IMG_DECLARE(taikong004);
LV_IMG_DECLARE(taikong005);
LV_IMG_DECLARE(taikong006);
LV_IMG_DECLARE(taikong007);
LV_IMG_DECLARE(taikong008);
LV_IMG_DECLARE(taikong009);
LV_IMG_DECLARE(taikong010);

camera_fb_t *fb = NULL;
uint8_t *dis_buf = NULL;
// const int pixel_size = sizeof(uint16_t);
// const int screen_width = 240;
// const int screen_height = 240;
// const int screen_left = 40 * pixel_size;
// const int camera_width = 320;
// EXT_RAM_ATTR uint16_t colors[240 * 240];

// TFT_t dev;
// extern uint8_t cam_en, color_en, face_en;
// void CamToScreen(TFT_t *dev, int width, int height)
// {
// 	fb = esp_camera_fb_get();
// 	for (int row = 0; row < screen_height; row++)
// 	{
// 		memcpy(colors, fb->buf + 1 + screen_left + (row * pixel_size * camera_width), pixel_size * screen_width);
// 		lcdDrawMultiPixels(dev, 0, row, screen_width, colors);
// 		// lv_disp_flush(0, 0, 240, 240, colors);
// 	}
// 	esp_camera_fb_return(fb);
// 	fb = NULL;
// }
/*
	读取画面中的每一个像素值
*/
uint16_t RGB_ReadBit16Point(unsigned short x, unsigned short y)
{
	if (fb != NULL)
		return fb->buf[2 * x + 240 * 2 * y] << 8 | fb->buf[2 * x + 240 * 2 * y + 1];
	else
		return 0;
}

extern lv_obj_t *cont_head;
lv_obj_t *label_speech;
static void create_demo_application(void)
{
	lv_demo_music();
	// lv_demo_widgets();
	// lv_demo_keypad_encoder();
	// lv_demo_benchmark();
	// lv_demo_stress();
}
extern struct _ksdiy_sys_t ksdiy_sys_t;
extern page_t page_manage[8];
static Display_Page Disp = Disp_Home; //当前界面
extern uint8_t fft_en;
void page_switch()
{
	uint32_t user_data;
	user_data = Button_Value;

	if (user_data)
	{
		if (Button_Value == BT1_LONG || Button_Value == BT3_LONG)
		{
			
		}
		else
			Button_Value = 0;

		if (user_data == BT1_DOWN || user_data == BT3_DOWN)
		{
			switch (Disp)
			{
			case Disp_Home:
				move_task_home(user_data);
				break;
			case Disp_Menu:
				move_task_menu(user_data);
				break;
			case Disp_Cam:
				// move_task_cam(user_data);
				break;
			case Disp_Color:
				move_task_color(user_data);
				break;
			case Disp_Face:
				((user_data == BT1_DOWN) ? (g_state = START_DELETE) : (g_state = START_RECOGNITION));
				// move_task_cam(user_data);
				break;
			case Disp_Baidu:
				move_task_baiduai(user_data);
				break;
			case Disp_FFT:
				move_task_fft(user_data);
				break;
			case Disp_Daily:
				move_task_daily(user_data);
				break;
			case Disp_Game_2048:
				move_task_game_2048(user_data);
				break;
			case Disp_Game_Snake:
				move_task_game_snake(user_data);
				break;
			case Disp_Calendar:
				// move_task_calendar(user_data);
				encoder_handler((user_data == BT1_DOWN) ? 2 : 3);
				break;
			case Disp_Setting:
				encoder_handler((user_data == BT1_DOWN) ? 2 : 3);
				// move_task_calendar(user_data);
				break;
			default:
				break;
			}
		}
		else if (user_data == BT1_LONG || user_data == BT3_LONG)
		{
			switch (Disp)
			{
			case Disp_Home:
				// move_task_home(user_data);
				break;
			case Disp_Menu:

				move_task_menu(user_data);
				break;
			case Disp_Cam:
				// move_task_cam(user_data);
				break;
			case Disp_Color:
				// move_task_color(user_data);
				break;
			case Disp_Face:
				// ((user_data == BT1_DOWN) ? (g_state = START_DELETE) : (g_state = START_RECOGNITION));
				// move_task_cam(user_data);
				break;
			case Disp_Baidu:

				// move_task_baiduai(user_data);
				break;
			case Disp_Daily:
				move_task_daily(user_data);
				break;

			case Disp_Calendar:
				// move_task_calendar(user_data);
				// encoder_handler((user_data == BT1_DOWN) ? 2 : 3);
				break;
			case Disp_Setting:
				encoder_handler((user_data == BT1_LONG) ? 2 : 3);
				// move_task_calendar(user_data);
				break;
			default:
				break;
			}
		}
		else if (user_data == BT2_DOWN)
		{
			switch (Disp)
			{
			case Disp_Home:
				// move_task_home(user_data);

				break;
			case Disp_Menu:
				page_menu_end();
				page_manage[Menu_Choose()].start();
				Disp = Menu_Choose();
				switch (Menu_Choose())
				{
				case Disp_Cam:
					cam_en = 1;
					break;
				case Disp_Color:
					cam_en = 1;
					color_en = 1;
					break;
				case Disp_Face:
					face_en = 1;
					g_state = START_RECOGNITION;
					xTaskCreatePinnedToCore(&Face_DEC, "Face_DEC", 1024 * 4, NULL, 5, NULL, 0);
					break;
				case Disp_Baidu:
					baiduai_en = 1;
					break;
				case Disp_FFT:
					fft_en = 1;
					xTaskCreatePinnedToCore(&FFT_Task, "FFT_Task", 1024 * 8, NULL, 6, NULL, 0);
					break;
				case Disp_About:

					break;

				default:
					break;
				}
				break;
			case Disp_Face:
				// move_task_cam(user_data);
				g_state = START_ENROLL;
				break;
			case Disp_Daily:
				move_task_daily(user_data);
				break;
			case Disp_Calendar:
				// move_task_calendar(user_data);
				encoder_handler(1);
				break;
			case Disp_Setting:
				encoder_handler(1);
				// move_task_calendar(user_data);
				break;
			case Disp_Baidu:
				move_task_baiduai(user_data);
				break;
			default:
				break;
			}
		}
		else if (user_data == BT1_DOUBLE || user_data == BT3_DOUBLE)
		{
			switch (Disp)
			{
			case Disp_Game_2048:
				move_task_game_2048(user_data);
				break;
			case Disp_Game_Snake:
				move_task_game_snake(user_data);
				break;
				default:
				break;
				// if (Disp != Disp_Menu && Disp != Disp_Home)
				// {
				// 	page_manage[Menu_Choose()].end();
				// 	page_menu_start();
				// 	Disp = Disp_Menu;
				// }
			}
		}
		else if (user_data == BT2_LONG)
		{
			if (Disp == Disp_Menu || Disp == Disp_Home)
			{
				static uint8_t set = 1;
				if (set)
				{
					ksdiy_sys_t.state.sys_button = 1; //使用lvgl按键机制
					page_home_end();
					page_menu_start();
					Disp = Disp_Menu;
					set = 0;
				}
				else
				{
					ksdiy_sys_t.state.sys_button = 0; //退出lvgl按键机制
					page_menu_end();
					page_home_start();
					Disp = Disp_Home;
					set = 1;
				}
			}
			else
			{
				cam_en = 0, color_en = 0, face_en = 0;
				page_manage[Menu_Choose()].end();
				page_menu_start();
				Disp = Disp_Menu;
			}
		}
		printf("Disp: %d\n", Disp);
	}
}
void button_task(void *arg)
{
	Button_Init();
	while (1)
	{
		Button_Process();
		vTaskDelay(30 / portTICK_PERIOD_MS);
	}
}
#define LV_TICK_PERIOD_MS 1

SemaphoreHandle_t xGuiSemaphore;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
// EXT_RAM_ATTR lv_color_t buf1[DISP_BUF_SIZE];
// EXT_RAM_ATTR lv_color_t buf2[DISP_BUF_SIZE];
static void guiTask(void *pvParameter)
{

	(void)pvParameter;
	xGuiSemaphore = xSemaphoreCreateMutex();
	lv_init();
	lvgl_driver_init();

	lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
	lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);

	// static lv_color_t buf1[DISP_BUF_SIZE];
	// static lv_color_t buf2[DISP_BUF_SIZE];
	static lv_disp_buf_t disp_buf;
	uint32_t size_in_px = DISP_BUF_SIZE;
	lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = disp_driver_flush;
	disp_drv.buffer = &disp_buf;
	lv_disp_drv_register(&disp_drv);

	/* Create and start a periodic timer interrupt to call lv_tick_inc */
	const esp_timer_create_args_t periodic_timer_args = {
		.callback = &lv_tick_task,
		.name = "periodic_gui"};
	esp_timer_handle_t periodic_timer;
	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
	//按键初始化
	lv_port_indev_init();
	/* Create the demo application */
	// create_demo_application();
	page_init();

	while (1)
	{
		vTaskDelay(10 / portTICK_RATE_MS);
		/* Try to take the semaphore, call lvgl related function on success */
		page_switch();
		// encoder_handler(Get_Adc_Button());
		if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
		{
			lv_task_handler();
			xSemaphoreGive(xGuiSemaphore);
		}
	}

	/* A task should NEVER return */
	vTaskDelete(NULL);
}
static void lv_tick_task(void *arg)
{
	(void)arg;
	lv_tick_inc(LV_TICK_PERIOD_MS);
}
static void SPIFFS_Directory(char *path)
{
	DIR *dir = opendir(path);
	assert(dir != NULL);
	while (true)
	{
		struct dirent *pe = readdir(dir);
		if (!pe)
			break;
		ESP_LOGI(__FUNCTION__, "d_name=%s d_ino=%d d_type=%x", pe->d_name, pe->d_ino, pe->d_type);
	}
	closedir(dir);
}
en_fsm_state g_state = WAIT_FOR_WAKEUP;
void esp_print_tasks(void)
{
	char *pbuffer = (char *)calloc(1, 2048);
	printf("--------------- vTaskList   heap:%u ---------------------\r\n", esp_get_free_heap_size());
	vTaskList(pbuffer);
	printf("%s", pbuffer);
	printf("----------------------------------------------\r\n");
	free(pbuffer);

	pbuffer = (char *)calloc(1, 2048);
	printf("--------------- vTaskGetRunTimeStats  heap:%u ---------------------\r\n", esp_get_free_heap_size());
	vTaskGetRunTimeStats(pbuffer);
	printf("%s", pbuffer);
	printf("----------------------------------------------\r\n");
	free(pbuffer);
}
void app_main(void)
{
	ESP_LOGI(TAG, "Initializing SPIFFS");
	// spiffs_driver_init();
	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/spiffs",
		.partition_label = NULL,
		.max_files = 8,
		.format_if_mount_failed = true};
	esp_err_t ret = esp_vfs_spiffs_register(&conf);
	if (ret != ESP_OK)
	{
		if (ret == ESP_FAIL)
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		else if (ret == ESP_ERR_NOT_FOUND)
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		else
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
		return;
	}
	SPIFFS_Directory("/spiffs/");

	// Initialize NVS
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

	ESP_ERROR_CHECK(ret);
	xTaskCreatePinnedToCore(guiTask, "guiTask", 1024 * 8, NULL, 5, NULL, 1);

	// vTaskDelay(500 / portTICK_PERIOD_MS);
	// spi_master_init(&dev, 23, 18, 5, 4, -1, -1);
	// lcdInit(&dev, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, 80);
	// lcdFillScreen(&dev, RED);
	adc_init();
	
	ESP_LOGI("esp-eye", "Please say 'Hi LeXin' to the board");
	ESP_LOGI("esp-eye", "Version " VERSION);
	xTaskCreatePinnedToCore(&button_task, "button_task", 1024 * 2, NULL, 18, NULL, 0);
	app_speech_wakeup_init();
	g_state = WAIT_FOR_WAKEUP;
	IR_init();
    xTaskCreatePinnedToCore(rmt_ir_txTask, "ir_tx", 2048, NULL, 5, &ir_tx_handle, 0);

	
	// while (1)
	// {

	// 	static uint8_t i = 0;
	// 	i++;
	// 	if (i == 20)
	// 	{
	// 		// int adc_value = get_adc();
	// 		// printf("adc_value: %d\n", get_adc());
	// 		// i = 0;
	// 		esp_print_tasks();
	// 		// ac_open(1);
	// 	}
	// 	vTaskDelay(5 / portTICK_PERIOD_MS);
	// }
}
