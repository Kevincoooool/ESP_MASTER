#include "http.h"
#include "updata.h"

//#define WEB_URL "/x/relation/stat?vmid=138699039"
BILIBILI wp_fans;
static const char *TAG = "HTTP";

struct DATA_WEATHER weather_data[3];

esp_err_t cjson_bilibili(char *text);

static esp_err_t bilibili_http_event_handler(esp_http_client_event_t *evt)
{

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

		//如果404或者502
		if (evt->data_len == 150 || evt->data_len == 146)
			return ESP_OK;

		// printf("%.*s\n", evt->data_len, (char *)evt->data);
		cjson_bilibili((char *)evt->data);
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
void get_bilibili(char *bili_uid)
{
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	static char post_url[256] = {0};
	memset((char *)post_url, 0, 256);
	static esp_http_client_config_t config = {
		// .url = "https://api.bilibili.com/x/relation/stat?vmid=59041601&jsonp=jsonp",
		.event_handler = bilibili_http_event_handler,
		.buffer_size = 3 * 1024,
		.timeout_ms = 4000,
		.path = "/get",
		.transport_type = HTTP_TRANSPORT_OVER_TCP,
	};
	sprintf(post_url, "https://api.bilibili.com/x/relation/stat?vmid=%s&jsonp=jsonp", bili_uid);
	config.url = post_url;
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
		ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
	}

	esp_http_client_cleanup(client);
}

esp_err_t cjson_bilibili(char *text)
{
	cJSON *root_ksdiy, *psub_ksdiy;
	char *index = strchr(text, '{');
	printf("bilibili json:%s\n", text);
	if (index == NULL)
	{
		return ESP_FAIL;
	}
	else
	{
		strcpy(text, index);
		root_ksdiy = cJSON_Parse(text);
		if (root_ksdiy != NULL)
		{
			psub_ksdiy = cJSON_GetObjectItem(root_ksdiy, "data");
			if (psub_ksdiy != NULL)
			{
				cJSON *follower = cJSON_GetObjectItem(psub_ksdiy, "follower");
				wp_fans.follower = follower->valueint;
				cJSON *following = cJSON_GetObjectItem(psub_ksdiy, "following");
				wp_fans.following = following->valueint;
				ESP_LOGI(TAG, "获取粉丝成功 follower:%d  following:%d\n", wp_fans.follower, wp_fans.following);
			}
			else
			{
				ESP_LOGI(TAG, "获取粉丝失败，请检查uid");
			}
		}
		else
		{
			return ESP_FAIL;
		}
		cJSON_Delete(root_ksdiy);
	}
	return ESP_OK;
}
esp_err_t cjson_weather(char *text)
{
	cJSON *root, *psub;
	cJSON *arrayItem;
	printf("weather json:%s\n", text);
	char *index = strchr(text, '{');
	if (index == NULL)
	{
		return ESP_FAIL;
	}
	else
	{
		strcpy(text, index);
		root = cJSON_Parse(text);
		if (root != NULL)
		{
			psub = cJSON_GetObjectItem(root, "results");
			if (psub != NULL)
			{
				arrayItem = cJSON_GetArrayItem(psub, 0);
				cJSON *now = cJSON_GetObjectItem(arrayItem, "daily");
				//ESP_LOGI(HTTP_TAG,"code_day:%s",now->valuestring);
				for (uint8_t i = 0; i < 3; i++)
				{
					cJSON *array2 = cJSON_GetArrayItem(now, i);
					cJSON *code_day = cJSON_GetObjectItem(array2, "code_day");
					//sprintf(weather_data[i].code_day,"%d",atoi(code_day->valuestring));
					weather_data[i].code_day = atoi(code_day->valuestring);
					cJSON *text_day = cJSON_GetObjectItem(array2, "text_day");
					strcpy(weather_data[i].text_day, text_day->valuestring);
					cJSON *high = cJSON_GetObjectItem(array2, "high");
					weather_data[i].high = atoi(high->valuestring);
					cJSON *low = cJSON_GetObjectItem(array2, "low");
					weather_data[i].low = atoi(low->valuestring);
					cJSON *rainfall = cJSON_GetObjectItem(array2, "rainfall");
					weather_data[i].rainfall = atof(rainfall->valuestring);
					cJSON *wind_direction_degree = cJSON_GetObjectItem(array2, "wind_direction_degree");
					weather_data[i].wind_direction_degree = atoi(wind_direction_degree->valuestring);
					cJSON *wind_direction = cJSON_GetObjectItem(array2, "wind_direction");
					strcpy(weather_data[i].wind_direction, wind_direction->valuestring);
					cJSON *wind_speed = cJSON_GetObjectItem(array2, "wind_speed");
					weather_data[i].wind_speed = atof(wind_speed->valuestring);
					cJSON *humidity = cJSON_GetObjectItem(array2, "humidity");
					weather_data[i].humidity = atoi(humidity->valuestring);
				}
			}
			else
			{
				ESP_LOGI("HTTP", "获取天气失败，请检测是否有改接口权力");
			}
		}
		else
		{
			return ESP_FAIL;
		}
		cJSON_Delete(root);
	}
	return ESP_OK;
}

static esp_err_t weather_http_event_handler(esp_http_client_event_t *evt)
{

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

		//如果404或者502
		if (evt->data_len == 150 || evt->data_len == 146)
			return ESP_OK;

		// printf("%.*s\n", evt->data_len, (char *)evt->data);
		cjson_weather((char *)evt->data);
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
void get_weather(char *city, char *key)
{
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	static char post_url[256] = {0};
	memset((char *)post_url, 0, 256);
	static esp_http_client_config_t config = {
		// .url = "https://api.bilibili.com/x/relation/stat?vmid=59041601&jsonp=jsonp",
		.event_handler = weather_http_event_handler,
		.buffer_size = 3 * 1024,
		.timeout_ms = 4000,
		.path = "/get",
		.transport_type = HTTP_TRANSPORT_OVER_TCP,
	};
	sprintf(post_url, "https://api.seniverse.com/v3/weather/daily.json?key=%s&location=%s&language=zh-Hans&unit=c&start=0&days=5",
			key, city);
	config.url = post_url;
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
		ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
	}

	esp_http_client_cleanup(client);
}

esp_err_t read_weather()
{
	static char city[30]={0};
	static char key[30]={0};
	if (read_nvs("city", city)) //获取哔哩哔哩uid
		ESP_LOGI(TAG, "获取到城市");
	else
	{
		sprintf(city, "chengdu");
		ESP_LOGI(TAG, "没有获取到城市，默认使用成都");
	}
	if (read_nvs("pass", key)) //获取哔哩哔哩uid
		ESP_LOGI(TAG, "获取到心知密匙");
	else
	{
		save_nvs("pass", "SybN2IXZM2B_vayTP");
		sprintf(key, "SybN2IXZM2B_vayTP");
		ESP_LOGI(TAG, "没有获取心知密匙，使用默认");
	}
	get_weather(city,key);
	return ESP_OK;
}
esp_err_t read_fans()
{
	static char str[20]={0};
	static char uid[15]={0};
	if (read_nvs("uid", uid)) //获取哔哩哔哩uid
	{
		sprintf(str, "%s", uid);
		ESP_LOGI(TAG, "获取到uid");
	}
	else
	{
		sprintf(str, "%s", uid);
		save_nvs("uid", "59041601");
		ESP_LOGI(TAG, "没有uid，使用默认uid");
	}
	ESP_LOGI(TAG, "%s", str);
	get_bilibili(str);
	return ESP_OK;
}
uint16_t get_value_ksdiyfs(uint8_t i)
{
	if (i)
		return wp_fans.follower;
	else
	{
		return wp_fans.following;
	}
}
