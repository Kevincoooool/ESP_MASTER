/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-17 15:04:46
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_daily.c
 */
#include "page_daily.h"
#include "page_wakeup.h"
#include "base64.h"
#include "urlcode.h"
#include "cJSON.h"
#include "page_start.h"
#include "page_menu.h"
#include "app_main.h"
#include "app_face.h"
#include "EasyTracer.h"
#include "button.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "lv_compment.h"
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "lv_port_indev.h"

#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"

#define TAG "DAILY"

#define DAILY_URL "https://api.xygeng.cn/one"
#define HEART_URL "http://www.dutangapp.cn/u/toxic?date=2021-7-13"
/*此页面窗口*/
static lv_obj_t *appWindow;
/*标题栏*/
static lv_obj_t *labelTitle;

/*标题栏分隔线*/
static lv_obj_t *lineTitle;
/*图标显示容器，用于裁剪显示*/
static lv_obj_t *contDisp;

static lv_obj_t *label_sentence;
/**
  * @brief  创建标题栏
  * @param  无
  * @retval 无
  */
 extern lv_obj_t *label_speech;
extern const uint8_t example_gif_map[]; /*Use the example gif*/
static void Title_Create()
{
	LV_FONT_DECLARE(number);
	appWindow = lv_cont_create(lv_scr_act(), NULL);
	static lv_style_t style_cont;
	lv_style_set_pad_left(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(&style_cont, LV_STATE_DEFAULT, 255);
	lv_style_set_pad_top(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_width(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_opa(&style_cont, LV_STATE_DEFAULT, 255);
	lv_style_set_bg_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK); //设置屏幕背景
	lv_obj_add_style(appWindow, LV_BTN_PART_MAIN, &style_cont);			  /*Default button style*/
	lv_obj_set_pos(appWindow, 0, 0);
	lv_obj_set_size(appWindow, 240, 240);

	labelTitle = lv_label_create(appWindow, NULL);

	static lv_style_t style_label;
	lv_style_init(&style_label);

	//Write style state: LV_STATE_DEFAULT for style_label
	lv_style_set_radius(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_dir(&style_label, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_label, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_text_font(labelTitle, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &number);
	lv_style_set_text_letter_space(&style_label, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_label, LV_STATE_DEFAULT, 0);

	lv_obj_add_style(labelTitle, LV_LABEL_PART_MAIN, &style_label);
	lv_label_set_long_mode(labelTitle, LV_LABEL_LONG_BREAK);
	lv_label_set_align(labelTitle, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_pos(labelTitle, 0, 0);
	lv_obj_set_size(labelTitle, 240, 60);
	lv_label_set_recolor(labelTitle, true);

	/*默认选中的是第二个图标*/
	lv_label_set_static_text(labelTitle, "Daily");
	lv_obj_align(labelTitle, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
	lv_obj_set_auto_realign(labelTitle, true);

	lineTitle = lv_line_create(appWindow, NULL);
	static lv_style_t style_line;
	lv_style_init(&style_line);
	lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, lv_color_make(0xFa, 0x05, 0x05));
	lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 5);
	lv_obj_add_style(lineTitle, LV_LINE_PART_MAIN, &style_line);
	lv_obj_set_pos(lineTitle, 0, 55);
	lv_obj_set_size(lineTitle, 240, 5);
	static lv_point_t screen_line3[] = {{0, 0}, {240, 0}};
	lv_line_set_points(lineTitle, screen_line3, 2);
}
extern lv_style_t style1;
//创建菜单界面
static void Cont_create(void)
{
	LV_FONT_DECLARE(myFont);
	uint8_t i;
	contDisp = lv_cont_create(appWindow, NULL);
	static lv_style_t style_cont;
	lv_style_set_pad_left(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(&style_cont, LV_STATE_DEFAULT, 255);
	lv_style_set_pad_top(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_width(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_opa(&style_cont, LV_STATE_DEFAULT, 255);
	lv_style_set_bg_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK); //设置屏幕背景
	lv_obj_add_style(contDisp, LV_BTN_PART_MAIN, &style_cont);			  /*Default button style*/
	lv_obj_set_size(contDisp, 240, 190);
	lv_obj_set_pos(contDisp, 0, 60);
	// lv_obj_t *img_gif = lv_gif_create_from_data(contDisp, example_gif_map);
	// lv_obj_set_size(img_gif, 100, 100);
	// lv_obj_set_pos(img_gif, 70, 10);

	label_sentence = lv_label_create(contDisp, NULL);

	static lv_style_t style_label;
	lv_style_init(&style_label);

	//Write style state: LV_STATE_DEFAULT for style_label
	lv_style_set_radius(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_dir(&style_label, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_label, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_text_font(label_sentence, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_style_set_text_letter_space(&style_label, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_label, LV_STATE_DEFAULT, 0);

	lv_obj_add_style(label_sentence, LV_LABEL_PART_MAIN, &style_label);
	lv_label_set_long_mode(label_sentence, LV_LABEL_LONG_BREAK);
	lv_label_set_align(label_sentence, LV_LABEL_ALIGN_CENTER);

	lv_obj_set_size(label_sentence, 240, 240);
	lv_label_set_recolor(label_sentence, true);

	lv_label_set_static_text(label_sentence, "KS-ESP32-MASTER\nKevincoooool\n"__DATE__);
	lv_obj_align(label_sentence, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_auto_realign(label_sentence, true);
}
static esp_err_t daily_http_event_handler(esp_http_client_event_t *evt)
{

	//printf("evtid = %d\n", evt->event_id);
	switch (evt->event_id)
	{
	case HTTP_EVENT_ERROR:
		ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:

		printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);
		printf("%.*s\n", evt->data_len, (char *)evt->data);

		cJSON *json_root = cJSON_Parse((char *)evt->data);
		cJSON *result_code = cJSON_GetObjectItem(json_root, "code");

		printf("result_code = %d\n", result_code->valueint);
		if (result_code->valueint != 200)
		{
			cJSON_Delete(json_root);
			return ESP_OK;
		}
		cJSON *result = cJSON_GetObjectItem(json_root, "data");
		cJSON *keyword = cJSON_GetObjectItem(result, "content");
		printf("content = %s\n", keyword->valuestring);

		lv_label_set_text(label_sentence, keyword->valuestring);

		if (json_root != NULL)
		{
			cJSON_Delete(json_root);
		}
		page_wakeup_end();
		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
		break;
	}
	return ESP_OK;
}
void get_daily(void)
{
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	lv_label_set_text(label_speech, "获取每日一句...");
	static esp_http_client_config_t config = {
		.url = DAILY_URL,
		.event_handler = daily_http_event_handler,
		.buffer_size = 3 * 1024,
		.timeout_ms = 4000,
		.path = "/get",
		.transport_type = HTTP_TRANSPORT_OVER_TCP,
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_header(client, "Accept", "*/*");
	esp_http_client_set_header(client, "Accept-Encoding", "identity");
	esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	esp_http_client_set_header(client, "Connection", "keep-alive");
	esp_http_client_set_header(client, "Content-Type", "application/json");

	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGE(TAG, "HTTP GET Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
	}
	else
	{
		page_wakeup_end();
		ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
	}

	esp_http_client_cleanup(client);
}
static esp_err_t heart_http_event_handler(esp_http_client_event_t *evt)
{

	//printf("evtid = %d\n", evt->event_id);
	switch (evt->event_id)
	{
	case HTTP_EVENT_ERROR:
		ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:

		printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);
		printf("%.*s\n", evt->data_len, (char *)evt->data);

		cJSON *json_root = cJSON_Parse((char *)evt->data);

		cJSON *result = cJSON_GetObjectItem(json_root, "data");
		cJSON *result1 = cJSON_GetArrayItem(result, 0);
		cJSON *keyword = cJSON_GetObjectItem(result1, "data");
		printf("content = %s\n", keyword->valuestring);

		lv_label_set_text(label_sentence, keyword->valuestring);
		if (json_root != NULL)
		{
			cJSON_Delete(json_root);
		}
		page_wakeup_end();
		break;

	case HTTP_EVENT_ON_FINISH:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
		break;
	}
	return ESP_OK;
}
void get_heart(void)
{
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	lv_label_set_text(label_speech, "获取心灵鸡汤...");
	static esp_http_client_config_t config = {
		.url = HEART_URL,
		.event_handler = heart_http_event_handler,
		.buffer_size = 3 * 1024,
		.timeout_ms = 4000,
		.path = "/get",
		.transport_type = HTTP_TRANSPORT_OVER_TCP,
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_header(client, "Accept", "*/*");
	esp_http_client_set_header(client, "Accept-Encoding", "identity");
	esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	esp_http_client_set_header(client, "Connection", "keep-alive");
	esp_http_client_set_header(client, "Content-Type", "application/json");

	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGE(TAG, "HTTP GET Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
	}
	else
	{
		page_wakeup_end();
		ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
	}

	esp_http_client_cleanup(client);
}
//当前界面是菜单时MOVE任务要做的事情
void move_task_daily(uint8_t move)
{

	switch (move)
	{
	case BT1_DOWN: //往上移动
		page_wakeup_start();

		get_heart();
		break;
	case BT1_LONG: //往上移动

		break;
	case BT1_LONGFREE: //往上移动
		break;
	case BT2_DOWN: //往下移动
		page_wakeup_start();

		get_daily();
		break;
	case BT3_LONG: //往下移动

		break;
	case BT3_LONGFREE: //往上移动
		break;

	default:
		break;
	}
}

void page_daily_load()
{

	Title_Create();
	Cont_create();
	obj_add_anim(
		appWindow,								  //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //动画函数
		lv_anim_speed_to_time(300, 0, 50), //动画速度
		240,					  //起始值
		0,  //结束值
		lv_anim_path_linear						  //动画特效:模拟弹性物体下落
	);
	ANIEND
	page_wakeup_start();
	get_daily();
}
void page_daily_end(void)
{

	obj_add_anim(
		appWindow,								  //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //动画函数
		lv_anim_speed_to_time(300, 0, 50), //动画速度
		0,					  //起始值
		240,  //结束值
		lv_anim_path_linear						  //动画特效:模拟弹性物体下落
	);
	ANIEND
	lv_obj_del(appWindow);
}

void page_daily_start(void)
{
	//获取芯片可用内存
	printf(" page_daily_start    esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//获取从未使用过的最小内存
	printf(" page_daily_start    esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	page_daily_load();
}
