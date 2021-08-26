/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-05-25 16:03:28
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-24 19:57:50
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\app_speech_wakeup.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "xtensa/core-macros.h"
#include "esp_partition.h"
#include "app_speech_srcif.h"
#include "sdkconfig.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "dl_lib_coefgetter_if.h"
#include "app_main.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "ringbuf.h"
#include "st7789.h"
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "page_calendar.h"
#include "page_home.h"
#include "page_wakeup.h"
#include "page_cam.h"
#include "page_tab.h"
#include "esp_ns.h"
#include "esp_agc.h"
#include "irTask.h"
#define TAG "SPEECH"
static const esp_wn_iface_t *wakenet = &WAKENET_MODEL;
static const model_coeff_getter_t *model_coeff_getter = &WAKENET_COEFF;
// MultiNet
static const esp_mn_iface_t *multinet = &MULTINET_MODEL;
model_iface_data_t *model_data_mn = NULL;

static src_cfg_t srcif;
static model_iface_data_t *model_data;

QueueHandle_t sndQueue;

static void event_wakeup_detected(int r)
{
	// assert(g_state == WAIT_FOR_WAKEUP);

	printf("%s DETECTED.\n", wakenet->get_word_name(model_data, r));

	// if (g_state == WAIT_FOR_WAKEUP)
	//     g_state = WAIT_FOR_CONNECT;
	// else if (g_state == WAIT_FOR_CONNECT)
	//     g_state = WAIT_FOR_WAKEUP;
}
uint8_t cam_en = 0, color_en = 0, face_en = 0, baiduai_en = 0;
extern lv_obj_t *label_speech;
/*
I (3588) MN: ---------------------SPEECH COMMANDS---------------------
I (3595) MN: Command ID0, phrase 0: ni shi shei
I (3600) MN: Command ID1, phrase 1: ni hao
I (3605) MN: Command ID2, phrase 2: kai deng
I (3610) MN: Command ID3, phrase 3: guan deng
I (3615) MN: Command ID4, phrase 4: guan bi she xiang tou
I (3621) MN: Command ID5, phrase 5: da kai she xiang tou
I (3627) MN: Command ID6, phrase 6: xia yi ye
I (3632) MN: Command ID7, phrase 7: shang yi ye
I (3638) MN: Command ID8, phrase 8: que ren
I (3642) MN: Command ID9, phrase 9: da kai ri li
I (3648) MN: Command ID10, phrase 10: guan bi ri li
I (3653) MN: Command ID11, phrase 11: jian kang mo shi
I (3659) MN: Command ID12, phrase 12: shui mian mo shi
I (3665) MN: Command ID13, phrase 13: da kai lan ya
I (3671) MN: Command ID14, phrase 14: guan bi lan ya
I (3676) MN: Command ID15, phrase 15: bo fang ge qu
I (3682) MN: Command ID16, phrase 16: zan ting bo fang
I (3687) MN: Command ID17, phrase 17: ding shi yi xiao shi
I (3694) MN: Command ID18, phrase 18: da kai dian deng
I (3700) MN: Command ID19, phrase 19: guan bi dian deng
I (3718) MN: ---------------------------------------------------------
*/
void speech_commands_action(int command_id)
{
	printf("Commands ID: %d.\n", command_id);
	switch (command_id)
	{
	case 0:
		lv_label_set_text(label_speech, "我是酷世DIY语音助手");
		printf("我是酷世DIY语音助手\n");
		break;
	case 1:
		lv_label_set_text(label_speech, "你好呀你好呀！");
		printf("你好你好鸭\n");
		break;
	case 2:
		page_home_end();
		lv_label_set_text(label_speech, "好的，我这去开灯");
		printf("好的，已开灯\n");
		page_tab_start();
		break;
	case 3:
		lv_label_set_text(label_speech, "关灯啦");
		printf("好的，已关灯\n");
		page_tab_end();
		page_home_start();
		break;
	case 4:
		lv_label_set_text(label_speech, "已打开");
		printf("好的，已打开\n");
		page_home_end();
		page_cam_start();
		break;
	case 5:

		page_cam_end();
		page_home_start();
		lv_label_set_text(label_speech, "已关闭");
		printf("好的，已关闭\n");
		break;

	case 9:
		page_home_end();
		page_calendar_start();
		printf("好的，已打开\n");
		break;
	case 10:
		page_calendar_end();
		page_home_start();
		printf("好的，已打开\n");
		break;
	default:
		break;
	}
}

/*
 * 语音识别处理函数
 */
static esp_err_t asr_multinet_control(int commit_id)
{

	if (commit_id >= 0 && commit_id < ID_MAX)
	{
		switch (commit_id)
		{
		case ID0_TIAODAOERSHIDU:
			ac_set_temp(20);
			ESP_LOGI(TAG, "ID0_TIAODAOERSHIDU");
			lv_label_set_text(label_speech, "已调到20度");
			break;
		case ID1_TIAODAOERSHIYIDU:
			ac_set_temp(21);
			ESP_LOGI(TAG, "ID1_TIAODAOERSHIYIDU");
			lv_label_set_text(label_speech, "已调到21度");
			break;
		case ID2_TIAODAOERSHIERDU:
			ac_set_temp(22);
			ESP_LOGI(TAG, "ID2_TIAODAOERSHIERDU");
			lv_label_set_text(label_speech, "已调到22度");
			break;
		case ID3_TIAODAOERSHISANDU:
			ac_set_temp(23);
			ESP_LOGI(TAG, "ID3_TIAODAOERSHISANDU");
			lv_label_set_text(label_speech, "已调到23度");
			break;
		case ID4_TIAODAOERSHISIDU:
			ac_set_temp(24);
			ESP_LOGI(TAG, "ID4_TIAODAOERSHISIDU");
			lv_label_set_text(label_speech, "已调到24度");
			break;
		case ID5_TIAODAOERSHIWUDU:
			ac_set_temp(25);
			ESP_LOGI(TAG, "ID5_TIAODAOERSHIWUDU");
			lv_label_set_text(label_speech, "已调到25度");
			break;
		case ID6_TIAODAOERSHLIUIDU:
			ac_set_temp(26);
			ESP_LOGI(TAG, "ID6_TIAODAOERSHLIUIDU");
			lv_label_set_text(label_speech, "已调到26度");
			break;
		case ID7_TIAODAOERSHIQIDU:
			ac_set_temp(27);
			ESP_LOGI(TAG, "ID7_TIAODAOERSHIQIDU");
			lv_label_set_text(label_speech, "已调到27度");
			break;
		case ID8_TIAODAOERSHIBADU:
			ac_set_temp(28);
			ESP_LOGI(TAG, "ID8_TIAODAOERSHIBADU");
			lv_label_set_text(label_speech, "已调到28度");
			break;
		case ID9_QIDONGKONGTIAOSAOFENG:
			ac_set_swing(true);
			ESP_LOGI(TAG, "ID9_QIDONGKONGTIAOSAOFENG");
			lv_label_set_text(label_speech, "已打开扫风");
			break;
		case ID10_TINGZHIKONGTIAOSAOFENG:
			ac_set_swing(false);
			ESP_LOGI(TAG, "ID10_TINGZHIKONGTIAOSAOFENG");
			lv_label_set_text(label_speech, "已关闭扫风");
			break;
		case ID11_TIAODAOZIDONGFENGSU:
			ac_set_wind_speed(0);
			ESP_LOGI(TAG, "ID11_TIAODAOZIDONGFENGSU");
			break;
		case ID12_TIAODAOYIJIFENGSU:
			ac_set_wind_speed(1);
			ESP_LOGI(TAG, "ID12_TIAODAOYIJIFENGSU");
			break;
		case ID13_TIAODAOERJIFENGSU:
			ac_set_wind_speed(2);
			ESP_LOGI(TAG, "ID13_TIAODAOERJIFENGSU");
			break;
		case ID14_TIAODAOSANJIFENGSU:
			ac_set_wind_speed(3);
			ESP_LOGI(TAG, "ID14_TIAODAOSANJIFENGSU");
			break;
		case ID15_DAKAIKONGTIAO:
			ac_open(1);
			ESP_LOGI(TAG, "ID15_DAKAIKONGTIAO");
			lv_label_set_text(label_speech, "已打开空调");
			break;
		case ID16_GUANBIKONGTIAO:
			ac_open(0);
			ESP_LOGI(TAG, "ID16_GUANBIKONGTIAO");
			lv_label_set_text(label_speech, "已关闭空调");
			break;
		case ID17_GELIKONGTIAO:
			ac_set_type(0);
			ESP_LOGI(TAG, "ID17_GELIKONGTIAO");
			lv_label_set_text(label_speech, "已设置为格力空调");
			break;
		case ID18_MEIDIKONGTIAO:
			ac_set_type(1);
			ESP_LOGI(TAG, "ID18_MEIDIKONGTIAO");
			lv_label_set_text(label_speech, "已设置为美的空调");
			break;
		case ID19_AOKESIKONGTIAO:
			ac_set_type(6);
			ESP_LOGI(TAG, "ID19_AOKESIKONGTIAO");
			lv_label_set_text(label_speech, "已设置为奥克斯空调");
			break;
		case ID20_HAIERKONGTIAO:
			ac_set_type(4);
			ESP_LOGI(TAG, "ID20_HAIERKONGTIAO");
			lv_label_set_text(label_speech, "已设置为海尔空调");
			break;
		default:
			ESP_LOGI(TAG, "not supportint mode");
			break;
		}

		return ESP_OK;
	}
	return ESP_FAIL;
}
void nnTask(void *arg)
{
	int frequency = wakenet->get_samp_rate(model_data);
	int audio_chunksize = wakenet->get_samp_chunksize(model_data);
	int chunk_num = multinet->get_samp_chunknum(model_data_mn);
	printf("chunk_num = %d\n", chunk_num);

	int16_t *buffer = malloc(audio_chunksize * sizeof(int16_t));
	assert(buffer);
	int chunks = 0;
	int mn_chunks = 0;
	bool detect_flag = 0;

	while (1)
	{
		xQueueReceive(sndQueue, buffer, portMAX_DELAY);

		if (detect_flag == 0)
		{
			int r = wakenet->detect(model_data, buffer);
			if (r)
			{
				float ms = (chunks * audio_chunksize * 1000.0) / frequency;
				printf("%.2f: %s DETECTED.\n", (float)ms / 1000.0, wakenet->get_word_name(model_data, r));

				page_wakeup_start();
				detect_flag = 1;
				lv_label_set_text(label_speech, "我在听呢！\n请说命令词叭");
				printf("-----------------LISTENING-----------------\n\n");
			}
			else
			{
				vTaskDelay(2);
			}
		}
		else
		{
			int command_id = multinet->detect(model_data_mn, buffer);
			mn_chunks++;
			if (mn_chunks == chunk_num || command_id > -1)
			{
				mn_chunks = 0;
				detect_flag = 0;
				if (command_id > -1)
				{
					asr_multinet_control(command_id);
					vTaskDelay(500);
					lv_label_set_text(label_speech, "请说 Hi LeXin 唤醒我吧");
				}
				else
				{
					printf("can not recognize any speech commands\n");
					lv_label_set_text(label_speech, "刚刚没听清耶...");
				}
				printf("\n-----------awaits to be waken up-----------\n");
				page_wakeup_end();
			}
		}
		chunks++;
	}

	free(buffer);
	vTaskDelete(NULL);
}

void app_speech_wakeup_init()
{
	//Initialize NN model
	model_data = wakenet->create(model_coeff_getter, DET_MODE_95);

	int wake_word_num = wakenet->get_word_num(model_data);
	char *wake_word_list = wakenet->get_word_name(model_data, 1);
	if (wake_word_num)
		printf("wake word number = %d, word1 name = %s\n",
			   wake_word_num, wake_word_list);

	int audio_chunksize = wakenet->get_samp_chunksize(model_data);

	model_data_mn = multinet->create(&MULTINET_COEFF, 4000);
	//Initialize sound source
	sndQueue = xQueueCreate(5, (audio_chunksize * sizeof(int16_t)));
	srcif.queue = &sndQueue;
	srcif.item_size = audio_chunksize * sizeof(int16_t);

	xTaskCreatePinnedToCore(&recsrcTask, "recsrcTask", 3 * 1024, (void *)&srcif, 13, NULL, 0);

	xTaskCreatePinnedToCore(&nnTask, "nnTask", 5 * 1024, NULL, 14, NULL, 0);
}
