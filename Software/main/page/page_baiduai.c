/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-25 15:00:15
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_baiduai.c
 */
#include "page_baiduai.h"
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
#define TAG "BAIDUAI"

#define TOKEN_URL "https://aip.baidubce.com/oauth/2.0/token"

#define grant_type "client_credentials"
static char *img_client_id = "wVsguVqncQExZFyVMOAyC13Y";
static char *img_client_secret = "25w2HZIS8auv4AZ4AGPossNYxwoDY4lx";
static char *img_access_token = "24.b628db3b9827bc8b207ce963f6d3418f.2592000.1632236042.282335-24170395";

static char *ocr_client_id = "6Fu8TqQgiI5t1GBALHPov4F9";
static char *ocr_client_secret = "z4HXgNq6GhQ5mA6hWef134Bx29T8TFO1";

static char *ocr_access_token = "24.749f3ab52b98a75fe41a32d1d0b9363f.2592000.1632236105.282335-24513061";

char New_img_token[100] = {0};
char New_ocr_token[100] = {0};
char *detect_class[] = {
	"v2/advanced_general",
	"v1/object_detect",
	"v1/animal",
	"v1/plant",
	"v2/logo",
	"v1/classify/ingredient",
	"v2/dish",
	"v1/redwine",
	"v1/currency",
	"v1/landmark",
	"v1/facade",
};
char str_res[500] = {0};
static esp_err_t img_access_token_http_event_handler(esp_http_client_event_t *evt)
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
		// printf("data %.*s\n", evt->data_len, (char *)evt->data);
		strncpy(str_res, (char *)evt->data, 400);
		strcat(str_res, "\"}");
		printf("str_res   %s\n", str_res);
		cJSON *json_root = cJSON_Parse(str_res);
		cJSON *json_access_token = cJSON_GetObjectItem(json_root, "access_token");
		printf("%s\n", json_access_token->valuestring);
		// New_token = malloc(strlen(json_access_token->valuestring));
		strcpy(New_img_token, json_access_token->valuestring);

		printf("New_img_token: %s\n", New_img_token);
		if (json_root != NULL)
		{
			cJSON_Delete(json_root);
		}
		memset(str_res, 0, sizeof(str_res));
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
static esp_err_t ocr_access_token_http_event_handler(esp_http_client_event_t *evt)
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
		// printf("%.*s\n", evt->data_len, (char *)evt->data);

		strncpy(str_res, (char *)evt->data, 400);
		strcat(str_res, "\"}");
		printf("str_res   %s\n", str_res);
		cJSON *json_root = cJSON_Parse(str_res);
		cJSON *json_access_token = cJSON_GetObjectItem(json_root, "access_token");
		printf("%s\n", json_access_token->valuestring);
		// New_token = malloc(strlen(json_access_token->valuestring));
		strcpy(New_ocr_token, json_access_token->valuestring);

		printf("New_ocr_token: %s\n", New_ocr_token);
		if (json_root != NULL)
		{
			cJSON_Delete(json_root);
		}
		memset(str_res, 0, sizeof(str_res));
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
char detect_things[255];
extern lv_obj_t *label_speech;
static esp_err_t img_http_event_handler(esp_http_client_event_t *evt)
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
		cJSON *result_num = cJSON_GetObjectItem(json_root, "result_num");

		printf("result_num = %d\n", result_num->valueint);
		if (result_num->valueint == 0)
		{
			cJSON_Delete(json_root);
			return ESP_OK;
		}
		cJSON *result = cJSON_GetObjectItem(json_root, "result");
		cJSON *result1 = cJSON_GetArrayItem(result, 0);
		cJSON *keyword = cJSON_GetObjectItem(result1, "keyword");
		printf("keyword = %s\n", keyword->valuestring);
		strcpy((char *)detect_things, keyword->valuestring);

		char now_time[256];
		sprintf((char *)now_time, "识别到:%s", keyword->valuestring);
		lv_label_set_text(label_speech, now_time);
		vTaskDelay(2000);

		if (json_root != NULL)
		{
			cJSON_Delete(json_root);
		}

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
static esp_err_t ocr_http_event_handler(esp_http_client_event_t *evt)
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
		cJSON *result_num = cJSON_GetObjectItem(json_root, "words_result_num");

		printf("words_result_num = %d\n", result_num->valueint);
		if (result_num->valueint == 0)
		{
			cJSON_Delete(json_root);
			return ESP_OK;
		}
		char now_result1[512];
		cJSON *result = cJSON_GetObjectItem(json_root, "words_result");
		for (size_t i = 0; i < result_num->valueint; i++)
		{
			// char now_result1[512]={0};
			cJSON *result1 = cJSON_GetArrayItem(result, i);
			cJSON *keyword = cJSON_GetObjectItem(result1, "words");
			printf("keyword %d= %s\n", i, keyword->valuestring);
			strcat(now_result1, keyword->valuestring);
		}

		// strcpy((char *)detect_things, keyword->valuestring);

		// sprintf((char *)now_result1, "识别到:%s", keyword->valuestring);
		lv_label_set_text(label_speech, now_result1);
		vTaskDelay(2000);

		if (json_root != NULL)
		{
			cJSON_Delete(json_root);
		}

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
void get_img_token(void)
{

	ESP_LOGI(TAG, "获取百度识图token");
	char *url_buf = NULL;
	// while (1)
	// {

	url_buf = malloc(sizeof(char) * 255);
	/*                       开始上传百度AI               */
	esp_http_client_config_t config = {
		//.url = TOKEN_URL,
		.event_handler = img_access_token_http_event_handler,
		.buffer_size = 4 * 1024,
		.timeout_ms = 8000,
	};
	sprintf(url_buf, "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%s&client_secret=%s", img_client_id, img_client_secret);
	config.url = url_buf;
	printf("URL: %s \n", config.url);
	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_method(client, HTTP_METHOD_GET);
	// esp_http_client_set_header(client, "Accept", "*/*");
	// esp_http_client_set_header(client, "Accept-Encoding", "identity");
	// esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	// esp_http_client_set_header(client, "Connection", "keep-alive");
	// esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");

	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
		esp_http_client_cleanup(client);
		free(url_buf);
		url_buf = NULL;
		// break;
	}
	else
	{
		free(url_buf);
		url_buf = NULL;
		ESP_LOGE(TAG, "HTTP POST request failed: %d", err);
	}
	// }
	// vTaskDelete(NULL);
	ESP_LOGI(TAG, "baidu End");
}
void get_ocr_token(void)
{

	ESP_LOGI(TAG, "baidu token Start");
	char *url_buf = NULL;
	// while (1)
	// {

	url_buf = malloc(sizeof(char) * 255);
	/*                       开始上传百度AI               */
	esp_http_client_config_t config = {
		//.url = TOKEN_URL,
		.event_handler = ocr_access_token_http_event_handler,
		.buffer_size = 4 * 1024,
		.timeout_ms = 8000,
	};
	sprintf(url_buf, "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%s&client_secret=%s", ocr_client_id, ocr_client_secret);
	config.url = url_buf;
	printf("URL: %s \n", config.url);
	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_method(client, HTTP_METHOD_GET);
	// esp_http_client_set_header(client, "Accept", "*/*");
	// esp_http_client_set_header(client, "Accept-Encoding", "identity");
	// esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	// esp_http_client_set_header(client, "Connection", "keep-alive");
	// esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");

	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
		esp_http_client_cleanup(client);
		free(url_buf);
		url_buf = NULL;
		// break;
	}
	else
	{
		free(url_buf);
		url_buf = NULL;
		ESP_LOGE(TAG, "HTTP POST request failed: %d", err);
	}
	// }
	// vTaskDelete(NULL);
	ESP_LOGI(TAG, "baidu End");
}
int baidu_img_ai(void)
{
	char *base64_buf = NULL;
	char *urlcode_buf = NULL;
	camera_fb_t *fb = NULL;
	esp_err_t res = ESP_OK;
	size_t _jpg_buf_len = 0;
	uint8_t *_jpg_buf = NULL;
	char *url_buf = malloc(sizeof(char) * 512);
	ESP_LOGE(TAG, "baidu Start");

	ESP_LOGE(TAG, "Taking picture...");
	camera_fb_t *pic = esp_camera_fb_get();

	if (!pic)
	{
		ESP_LOGE(TAG, "esp_camera_fb_get failed...");
		esp_camera_deinit();
		free(url_buf);
		url_buf = NULL;
		return;
	}
	if (!fmt2jpg(pic->buf, pic->width * pic->height * 3, pic->width, pic->height, PIXFORMAT_RGB565, 80, &_jpg_buf, &_jpg_buf_len))
	{
		ESP_LOGE(TAG, "fmt2jpg failed");
		return ESP_FAIL;
	}
	ESP_LOGE(TAG, "Picture taken! Its size was: %zu bytes", _jpg_buf_len);
	/*                       base64编码                                   */
	base64_buf = base64_encode(_jpg_buf, _jpg_buf_len);
	ESP_LOGE(TAG, "base64_encode OK,size: %d", strlen(base64_buf));
	urlcode_buf = malloc(sizeof(char) * strlen(base64_buf) * 2);
	URLEncode(base64_buf, strlen(base64_buf), urlcode_buf, strlen(base64_buf) * 2);
	ESP_LOGE(TAG, "urlencode OK,size: %d", strlen(urlcode_buf));
	/*                       开始上传百度AI                         */
	esp_http_client_config_t config = {
		//.url = BAIDU_WEB_URL,
		.event_handler = img_http_event_handler,
		.buffer_size = 4 * 1024,
		.timeout_ms = 4000,
	};
	sprintf(url_buf, "https://aip.baidubce.com/rest/2.0/image-classify/%s?access_token=%s", detect_class[0], img_access_token);
	config.url = url_buf;
	printf("URL: %s\n", config.url);
	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_method(client, HTTP_METHOD_POST);
	esp_http_client_set_header(client, "Accept", "*/*");
	esp_http_client_set_header(client, "Accept-Encoding", "identity");
	esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	esp_http_client_set_header(client, "Connection", "keep-alive");
	esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
	esp_http_client_set_post_field(client, urlcode_buf, strlen(urlcode_buf));

	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGE(TAG, "HTTP POST Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
		esp_http_client_cleanup(client);
		free(base64_buf);
		free(urlcode_buf);
		base64_buf = NULL;
		urlcode_buf = NULL;
		free(url_buf);
		url_buf = NULL;
		// esp_camera_deinit();
		ESP_LOGE(TAG, "baidu End");
	}
	else
	{
		lv_label_set_text(label_speech, "识别出错！");
		ESP_LOGE(TAG, "HTTP POST request failed: %d", err);
		esp_http_client_cleanup(client);
		free(base64_buf);
		free(urlcode_buf);
		base64_buf = NULL;
		urlcode_buf = NULL;
		free(url_buf);
		url_buf = NULL;

		// esp_camera_deinit();
		ESP_LOGE(TAG, "baidu End");
	}
	return;
}

int baidu_ocr_ai(void)
{
	char *base64_buf = NULL;
	char *urlcode_buf = NULL;
	camera_fb_t *fb = NULL;
	esp_err_t res = ESP_OK;
	size_t _jpg_buf_len = 0;
	uint8_t *_jpg_buf = NULL;
	char *url_buf = malloc(sizeof(char) * 255);
	ESP_LOGE(TAG, "baidu Start");

	ESP_LOGE(TAG, "Taking picture...");
	camera_fb_t *pic = esp_camera_fb_get();

	if (!pic)
	{
		ESP_LOGE(TAG, "esp_camera_fb_get failed...");
		esp_camera_deinit();
		free(url_buf);
		url_buf = NULL;
		return;
	}
	if (!fmt2jpg(pic->buf, pic->width * pic->height * 3, pic->width, pic->height, PIXFORMAT_RGB565, 80, &_jpg_buf, &_jpg_buf_len))
	{
		ESP_LOGE(TAG, "fmt2jpg failed");
		return ESP_FAIL;
	}
	ESP_LOGE(TAG, "Picture taken! Its size was: %zu bytes", _jpg_buf_len);
	/*                       base64编码                                   */
	base64_buf = base64_encode(_jpg_buf, _jpg_buf_len);
	ESP_LOGE(TAG, "base64_encode OK,size: %d", strlen(base64_buf));

	/*                       urlEncode                                   */
	urlcode_buf = malloc(sizeof(char) * strlen(base64_buf) * 2);

	URLEncode(base64_buf, strlen(base64_buf), urlcode_buf, strlen(base64_buf) * 2);

	ESP_LOGE(TAG, "urlencode OK,size: %d", strlen(urlcode_buf));

	/*                       开始上传百度AI                         */
	esp_http_client_config_t config = {
		//.url = BAIDU_WEB_URL,
		.event_handler = ocr_http_event_handler,
		.buffer_size = 4 * 1024,
		.timeout_ms = 4000,
	};
	sprintf(url_buf, "https://aip.baidubce.com/rest/2.0/ocr/v1/general_basic?access_token=%s", ocr_access_token);
	config.url = url_buf;
	printf("URL: %s\n", config.url);
	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_method(client, HTTP_METHOD_POST);
	esp_http_client_set_header(client, "Accept", "*/*");
	esp_http_client_set_header(client, "Accept-Encoding", "identity");
	esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	esp_http_client_set_header(client, "Connection", "keep-alive");
	esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
	esp_http_client_set_post_field(client, urlcode_buf, strlen(urlcode_buf));

	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGE(TAG, "HTTP POST Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
		esp_http_client_cleanup(client);
		free(base64_buf);
		free(urlcode_buf);
		base64_buf = NULL;
		urlcode_buf = NULL;
		free(url_buf);
		url_buf = NULL;
		// esp_camera_deinit();
		ESP_LOGE(TAG, "baidu End");
	}
	else
	{
		lv_label_set_text(label_speech, "识别出错！");
		ESP_LOGE(TAG, "HTTP POST request failed: %d", err);
		esp_http_client_cleanup(client);
		free(base64_buf);
		free(urlcode_buf);
		base64_buf = NULL;
		urlcode_buf = NULL;
		free(url_buf);
		url_buf = NULL;

		// esp_camera_deinit();
		ESP_LOGE(TAG, "baidu End");
	}
	return;
}
void baiduai_test_task(void *pvParameters)
{
	char *base64_buf = NULL;
	char *urlcode_buf = NULL;

	char *url_buf = NULL;
	ESP_LOGI(TAG, "baidu Start");

	while (1)
	{
		url_buf = malloc(sizeof(char) * 255);
		ESP_LOGI(TAG, "Taking picture...");
		camera_fb_t *pic = esp_camera_fb_get();

		ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);
		/*                       base64编码                                   */
		base64_buf = base64_encode(pic->buf, pic->len);
		ESP_LOGI(TAG, "base64_encode OK,size: %d", strlen(base64_buf));

		// for (uint32_t i = 0; i < strlen((const char *)base64_buf); i++)
		// 	printf("%c", base64_buf[i]);
		// stohex((char *)base64_buf,(char *)utf8code_buf);
		/*                       urlEncode                                   */
		urlcode_buf = malloc(sizeof(char) * strlen(base64_buf) * 2);

		URLEncode(base64_buf, strlen(base64_buf), urlcode_buf, strlen(base64_buf) * 2);

		ESP_LOGI(TAG, "urlencode OK,size: %d", strlen(urlcode_buf));

		/*                       开始上传百度AI                         */
		esp_http_client_config_t config = {
			//.url = BAIDU_WEB_URL,
			.event_handler = img_http_event_handler,
			.buffer_size = 10 * 1024,
			.timeout_ms = 8000,
		};
		sprintf(url_buf, "https://aip.baidubce.com/rest/2.0/image-classify/%s?access_token=%s", detect_class[0], img_access_token);
		config.url = url_buf;
		printf("URL: %s\n", config.url);
		esp_http_client_handle_t client = esp_http_client_init(&config);
		esp_http_client_set_method(client, HTTP_METHOD_POST);
		esp_http_client_set_header(client, "Accept", "*/*");
		esp_http_client_set_header(client, "Accept-Encoding", "identity");
		esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
		esp_http_client_set_header(client, "Connection", "keep-alive");
		esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
		esp_http_client_set_post_field(client, urlcode_buf, strlen(urlcode_buf));

		esp_err_t err = esp_http_client_perform(client);
		if (err == ESP_OK)
		{
			ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
					 esp_http_client_get_status_code(client),
					 esp_http_client_get_content_length(client));
		}
		else
		{
			ESP_LOGE(TAG, "HTTP POST request failed: %d", err);
		}

		esp_http_client_cleanup(client);
		free(base64_buf);
		free(urlcode_buf);
		base64_buf = NULL;
		urlcode_buf = NULL;
		free(url_buf);
		url_buf = NULL;
		vTaskDelay(2000 / portTICK_RATE_MS);
	}

	ESP_LOGI(TAG, "baidu End");
}

#define ANIEND                      \
	while (lv_anim_count_running()) \
		lv_task_handler(); //等待动画完成

lv_obj_t *img_baiduai, *img_fft; //要显示图像
extern lv_group_t *group_button;
extern lv_img_dsc_t imgfft;
extern lv_img_dsc_t img_dsc;
extern camera_fb_t *fb;
uint8_t start_reg = 0;
void BaiduAI_Task(void *pvParameters)
{

	// /* 入口处检测一次 */
	ESP_LOGE(TAG, "BaiduAI_Task Run Run uxHighWaterMark = %d", uxTaskGetStackHighWaterMark(NULL));

	portTickType xLastWakeTime;
	static char _img_client_id[100]={0};
	static char _img_client_secret[100]={0};
	static char _img_access_token[100]={0};
	static char _ocr_client_id[100]={0};
	static char _ocr_client_secret[100]={0};
	static char _ocr_access_token[100]={0};
	if (read_nvs("img_client_id", _img_client_id))
	{
		read_nvs("img_secret", _img_client_secret);
		read_nvs("img_token", _img_access_token);
		read_nvs("ocr_client_id", _ocr_client_id);
		read_nvs("ocr_secret", _ocr_client_secret);
		read_nvs("ocr_token", _ocr_access_token);
		img_client_id = _img_client_id;
		img_client_secret = _img_client_secret;
		img_access_token = _img_access_token;
		ocr_client_id = _ocr_client_id;
		ocr_client_secret = _ocr_client_secret;
		ocr_access_token = _ocr_access_token;
	}
	else
	{
		img_client_id = "wVsguVqncQExZFyVMOAyC13Y";
	}
	printf("img_client_id :%s",img_client_id);
	while (1)
	{
		if (baiduai_en)
		{
			static int64_t last_frame = 0;
			if (!last_frame)
			{
				last_frame = esp_timer_get_time();
			}
			fb = esp_camera_fb_get();
			if (fb == NULL)
			{
				vTaskDelay(100);
				ESP_LOGE(TAG, "Get image failed!");
			}
			else
			{
				if (start_reg == 1)
				{
					lv_label_set_text(label_speech, "开始识别通用物体...");
					start_reg = 0;
					baidu_img_ai();
					page_wakeup_end();
				}
				if (start_reg == 2)
				{
					lv_label_set_text(label_speech, "开始识别文字...");
					start_reg = 0;
					baidu_ocr_ai();
					page_wakeup_end();
				}

				img_dsc.data = fb->buf;
				lv_img_set_src(img_baiduai, &img_dsc);

				esp_camera_fb_return(fb);
				fb = NULL;
				int64_t fr_end = esp_timer_get_time();
				int64_t frame_time = fr_end - last_frame;
				last_frame = fr_end;
				frame_time /= 1000;
				ESP_LOGI("esp", "MJPG:  %ums (%.1ffps)", (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time);
			}
		}
		else
		{

			vTaskDelete(NULL);
		}
	}

	while (1)
	{
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}
//当前界面是菜单时MOVE任务要做的事情
void move_task_baiduai(uint8_t move)
{

	switch (move)
	{
	case BT1_DOWN: //往上移动
		page_wakeup_start();
		start_reg = 2;

		break;
	case BT1_LONG: //往上移动

		break;
	case BT1_LONGFREE: //往上移动
		break;
	case BT2_DOWN: //往下移动
		page_wakeup_start();
		start_reg = 1;

		break;
	case BT3_LONG: //往下移动

		break;
	case BT3_LONGFREE: //往上移动
		break;

	default:
		break;
	}
}

void baiduai_init(void)
{

	img_baiduai = lv_img_create(lv_scr_act(), NULL);
	static lv_style_t style_img;
	lv_style_init(&style_img);

	//Write style state: LV_STATE_DEFAULT for style_img
	lv_style_set_image_recolor(&style_img, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_image_recolor_opa(&style_img, LV_STATE_DEFAULT, 0);
	lv_style_set_image_opa(&style_img, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(img_baiduai, LV_IMG_PART_MAIN, &style_img);
	lv_obj_set_pos(img_baiduai, 0, 0);
	lv_obj_set_size(img_baiduai, 240, 240);
}
lv_task_t *task_baiduai;
void page_baiduai_load()
{
	app_camera_init();
	baiduai_init();

	obj_add_anim(
		img_baiduai,					   //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x,  //动画函数
		lv_anim_speed_to_time(300, 0, 50), //动画速度
		240,							   //起始值
		0,								   //结束值
		lv_anim_path_ease_out			   //动画特效:模拟弹性物体下落
	);
	ANIEND
	xTaskCreatePinnedToCore(&BaiduAI_Task, "BaiduAI_Task", 1024 * 3, NULL, 5, NULL, 0);
}
void page_baiduai_end(void)
{
	baiduai_en = 0, color_en = 0, face_en = 0;
	vTaskDelay(500);
	obj_add_anim(
		img_baiduai,					   //动画对象
		(lv_anim_exec_xcb_t)lv_obj_set_x,  //动画函数
		lv_anim_speed_to_time(300, 0, 50), //动画速度
		0,								   //起始值
		240,							   //结束值
		lv_anim_path_ease_out			   //动画特效:模拟弹性物体下落
	);
	ANIEND
	lv_obj_del(img_baiduai);
	esp_camera_deinit();
}
extern en_fsm_state g_state;

void page_baiduai_start(void)
{
	//获取芯片可用内存
	printf(" page_baiduai_start    esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//获取从未使用过的最小内存
	printf(" page_baiduai_start    esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());

	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	page_baiduai_load();
}
