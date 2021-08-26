#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "myFont.h"
#include "lv_compment.h"
#include "esp_log.h"
#include "esp_system.h"
#include "lv_port_indev.h"
#include "web_server.h"
#include "app_wifi.h"
#include "app_httpd.h"
#include "app_camera.h"
#define ANIEND                      \
	while (lv_anim_count_running()) \
		lv_task_handler(); //等待动画完成
#include "irtask.h"
#include "page_tab.h"
#include "page_start.h"
LV_FONT_DECLARE(myFont);
extern lv_obj_t *scr;
extern lv_obj_t *scr_body;

extern httpd_handle_t server; //文件服务句柄
lv_group_t *group_button;
lv_obj_t *tabview;
lv_obj_t *tab1, *tab2, *tab3;
uint8_t clock_change = 0; //时间是否发生改变
extern struct _ksdiy_sys_t ksdiy_sys_t;
struct
{
	lv_obj_t *label_wifi;
	lv_obj_t *label_cam;
	lv_obj_t *label_cam_vflip;
	lv_obj_t *label_cam_hmirror;
	lv_obj_t *sw_wifi;
	lv_obj_t *sw_cam;
	lv_obj_t *sw_cam_vflip;
	lv_obj_t *sw_cam_hmirror;
	lv_obj_t *page;
	lv_obj_t *labe_info;
} tab1_t;
struct
{
	lv_obj_t *label_tips;
	lv_obj_t *cont;
	lv_obj_t *table;
} tab2_t;
struct
{
	lv_obj_t *label_temp;
	lv_obj_t *sw_temp;
	lv_obj_t *page;
	lv_obj_t *set_info;
} tab3_t;

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		int8_t i = lv_switch_get_state(obj);
		if (obj == tab1_t.sw_wifi) //文件设置网页管理
		{
			if (i) //开启web
			{
				if (server == NULL)
				{
					if (start_file_server("/spiffs") != ESP_OK) //启动文件系统
					{
						ksdiy_sys_t.state.web_set = 0;
					}
					else
					{
						ESP_LOGI("WEB_SET", "web服务开启");
						lv_ico_web_show(1); //显示web图标
						ksdiy_sys_t.state.web_set = 1;
						web_switch_save(1);
					}
				}
				else
				{
					ESP_LOGI("WEB_SET", "web服务已经开启");
				}
			}
			else
			{
				lv_ico_web_show(0); //删除web图标
				ksdiy_sys_t.state.web_set = 0;
				web_switch_save(0);
				stop_webserver();
				ESP_LOGI("WEB", "web服务准备关闭");
			}
		}
		else if (obj == tab1_t.sw_cam) //如果是打开摄像头网页图传
		{
			if (i) //开启web
			{
				web_camera_init();
				if (server == NULL)
				{

					if (start_cam_web() != ESP_OK) //启动摄像头web
					{
						ksdiy_sys_t.state.web_cam = 0;
					}
					else
					{
						ESP_LOGI("WEB_CAM", "WEB_CAM 服务开启");
						lv_ico_web_show(1); //显示web图标
						ksdiy_sys_t.state.web_cam = 1;
						cam_switch_save(1);
					}
				}
				else
				{
					ESP_LOGI("WEB_CAM", "WEB_CAM 服务已经开启");
				}
			}
			else
			{
				lv_ico_web_show(0); //删除web图标
				ksdiy_sys_t.state.web_cam = 0;
				cam_switch_save(0);
				stop_cam_web();
				ESP_LOGI("WEB_CAM", "WEB_CAM 服务已关闭");
			}
		}
		else if (obj == tab1_t.sw_cam_vflip) //如果是打开摄像头网页图传
		{
			// sensor_t *s = esp_camera_sensor_get();
			// s->set_vflip(s, i);
			ksdiy_sys_t.state.cam_vflip = i;
			if (i)
			{
				cam_vflip_switch_save(1);
				ESP_LOGI("WEB_CAM", "sw_cam_vflip 已打开");
			}
			else
			{
				cam_vflip_switch_save(0);
				ESP_LOGI("WEB_CAM", "sw_cam_vflip 已关闭");
			}
		}
		else if (obj == tab1_t.sw_cam_hmirror) //如果是打开摄像头网页图传
		{

			// sensor_t *s = esp_camera_sensor_get();
			// s->set_hmirror(s, i);
			ksdiy_sys_t.state.cam_hmirror = i;
			if (i)
			{
				cam_hmirror_switch_save(1);
				ESP_LOGI("WEB_CAM", "sw_cam_hmirror 已打开");
			}
			else
			{
				cam_hmirror_switch_save(0);
				ESP_LOGI("WEB_CAM", "scam_hmirror 已关闭");
			}
		}
		else if (obj == tab3_t.sw_temp)
		{
			ksdiy_sys_t.state.upcloud = i;
			lv_ico_temp_show(i);
			web_temp_save(i);
		}
	}
}
void tab_wifi_init(lv_obj_t *obj)
{

	/******wifi_switch********/
	tab1_t.label_wifi = lv_label_create(obj, NULL);
	lv_obj_set_style_local_text_font(tab1_t.label_wifi, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_label_set_text(tab1_t.label_wifi, "网页设置");
	lv_obj_set_pos(tab1_t.label_wifi, 20, 5);
	lv_label_set_align(tab1_t.label_wifi, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/

	tab1_t.sw_wifi = lv_switch_create(obj, NULL);
	if (ksdiy_sys_t.state.web_set)
		lv_switch_on(tab1_t.sw_wifi, LV_ANIM_ON);
	lv_obj_set_event_cb(tab1_t.sw_wifi, event_handler);
	lv_obj_set_size(tab1_t.sw_wifi, 30, 16);
	lv_obj_set_pos(tab1_t.sw_wifi, 170, 8);

	tab1_t.label_cam = lv_label_create(obj, NULL);
	lv_obj_set_style_local_text_font(tab1_t.label_cam, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_label_set_text(tab1_t.label_cam, "网页图传");
	lv_obj_set_pos(tab1_t.label_cam, 20, 5 + 23);
	lv_label_set_align(tab1_t.label_cam, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/

	tab1_t.sw_cam = lv_switch_create(obj, NULL);
	if (ksdiy_sys_t.state.web_cam)
		lv_switch_on(tab1_t.sw_cam, LV_ANIM_ON);
	lv_obj_set_event_cb(tab1_t.sw_cam, event_handler);
	lv_obj_set_size(tab1_t.sw_cam, 30, 16);
	lv_obj_set_pos(tab1_t.sw_cam, 170, 32);

	tab1_t.label_cam_vflip = lv_label_create(obj, NULL);
	lv_obj_set_style_local_text_font(tab1_t.label_cam_vflip, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_label_set_text(tab1_t.label_cam_vflip, "垂直翻转");
	lv_obj_set_pos(tab1_t.label_cam_vflip, 20, 5 + 23 + 23);
	lv_label_set_align(tab1_t.label_cam_vflip, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/

	tab1_t.sw_cam_vflip = lv_switch_create(obj, NULL);
	if (ksdiy_sys_t.state.cam_vflip)
		lv_switch_on(tab1_t.sw_cam_vflip, LV_ANIM_ON);
	lv_obj_set_event_cb(tab1_t.sw_cam_vflip, event_handler);
	lv_obj_set_size(tab1_t.sw_cam_vflip, 30, 16);
	lv_obj_set_pos(tab1_t.sw_cam_vflip, 170, 57);

	tab1_t.label_cam_hmirror = lv_label_create(obj, NULL);
	lv_obj_set_style_local_text_font(tab1_t.label_cam_hmirror, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_label_set_text(tab1_t.label_cam_hmirror, "水平翻转");
	lv_obj_set_pos(tab1_t.label_cam_hmirror, 20, 5 + 23 + 23 + 23);
	lv_label_set_align(tab1_t.label_cam_hmirror, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/

	tab1_t.sw_cam_hmirror = lv_switch_create(obj, NULL);
	if (ksdiy_sys_t.state.cam_hmirror)
		lv_switch_on(tab1_t.sw_cam_hmirror, LV_ANIM_ON);
	lv_obj_set_event_cb(tab1_t.sw_cam_hmirror, event_handler);
	lv_obj_set_size(tab1_t.sw_cam_hmirror, 30, 16);
	lv_obj_set_pos(tab1_t.sw_cam_hmirror, 170, 78);

	/**********page*********/
	tab1_t.page = lv_page_create(obj, NULL);
	lv_obj_set_size(tab1_t.page, 200, 100);
	lv_obj_set_pos(tab1_t.page, 20, 100);

	lv_obj_t *label = lv_label_create(tab1_t.page, NULL);
	// lv_label_set_long_mode(label, LV_LABEL_LONG_SROLL_CIRC);            /*Automatically break long lines*/
	lv_obj_set_width(label, lv_page_get_width_fit(tab1_t.page)); /*Set the label width to max value to not show hor. scroll bars*/
	lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);			 /*Center aligned lines*/
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	char str[150];
	sprintf(str, "已连接到WIFI:\n%s\nIP:%s", get_ssid(), get_ip());
	lv_label_set_text(label, (const char *)str);
}
struct _tab2_ac_t
{
	lv_obj_t *cont;
	lv_obj_t *list_type;
	lv_obj_t *list_code;
} tab2_ac_t;
void page_tab_save_ac_type()
{

	if (clock_change) //时间改变
	{
		clock_change = 0;
		ac_set_type1(lv_dropdown_get_selected(tab2_ac_t.list_type),lv_dropdown_get_selected(tab2_ac_t.list_code));
		// h1 = lv_dropdown_get_selected(tab2_ac_t.list_type);
		// h2 = lv_dropdown_get_selected(tab2_ac_t.list_code);
	}
}
static void event_handler_clock(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		clock_change = 1;
		page_tab_save_ac_type();
	}
}

void creat_list(void)
{
	/*******下滑选择*********/
	tab2_ac_t.list_type = lv_dropdown_create(tab2_ac_t.cont, NULL);
	lv_obj_set_style_local_border_opa(tab2_ac_t.list_type, LV_DROPDOWN_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_text_font(tab2_ac_t.list_type, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_obj_set_size(tab2_ac_t.list_type, 70, 50);
	lv_dropdown_set_show_selected(tab2_ac_t.list_type, true);
	lv_dropdown_set_symbol(tab2_ac_t.list_type, NULL);
	lv_dropdown_set_options(tab2_ac_t.list_type, "Gree\nMei Di\nSong Xia\nDa jin\nHaier\nHaixin\nAux");
	// lv_obj_align(tab2_ac_t.list_type, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
	lv_obj_set_pos(tab2_ac_t.list_type, 10, 0);
	lv_obj_set_event_cb(tab2_ac_t.list_type, event_handler_clock);

	tab2_ac_t.list_code = lv_dropdown_create(tab2_ac_t.cont, NULL);
	lv_obj_set_style_local_border_opa(tab2_ac_t.list_code, LV_DROPDOWN_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_text_font(tab2_ac_t.list_code, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_obj_set_size(tab2_ac_t.list_code, 70, 50);
	lv_dropdown_set_symbol(tab2_ac_t.list_code, NULL);
	lv_dropdown_set_show_selected(tab2_ac_t.list_type, true);
	lv_dropdown_set_options(tab2_ac_t.list_code, "1\n2\n3\n4\n5");
	lv_obj_set_pos(tab2_ac_t.list_code, 80, 0);
	lv_obj_set_event_cb(tab2_ac_t.list_code, event_handler_clock);

	lv_obj_set_style_local_bg_color(tab2_ac_t.list_type, LV_DROPDOWN_PART_MAIN, LV_STATE_FOCUSED, LV_COLOR_AQUA);
	lv_obj_set_style_local_bg_color(tab2_ac_t.list_code, LV_DROPDOWN_PART_MAIN, LV_STATE_FOCUSED, LV_COLOR_AQUA);
}
void tab_ac_init(lv_obj_t *obj)
{
	/*********time_switch**********/
	tab2_t.label_tips = lv_label_create(obj, NULL);
	lv_obj_set_style_local_text_font(tab2_t.label_tips, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_label_set_text(tab2_t.label_tips, "空调选择");

	lv_obj_set_pos(tab2_t.label_tips, 80, 5);

	tab2_ac_t.cont = lv_cont_create(obj, NULL);
	lv_obj_set_size(tab2_ac_t.cont, 200, 50);
	lv_obj_set_pos(tab2_ac_t.cont, 20, 40);

	creat_list();

	lv_dropdown_set_selected(tab2_ac_t.list_type, 0);      //更新时钟；
	lv_dropdown_set_selected(tab2_ac_t.list_code, 0); //更新时钟；

}
/****
*
    温湿度上传间隔
    简介：使用方法 开源地址
*
***/
void tab_set_init(lv_obj_t *obj)
{
	/*********temp**********/
	tab3_t.label_temp = lv_label_create(obj, NULL);
	lv_obj_set_style_local_text_font(tab3_t.label_temp, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_label_set_text(tab3_t.label_temp, "BILI");
	lv_obj_set_pos(tab3_t.label_temp, 30, 5);

	tab3_t.sw_temp = lv_switch_create(obj, NULL);
	if (ksdiy_sys_t.state.upcloud)
		lv_switch_on(tab3_t.sw_temp, LV_ANIM_ON);
	lv_obj_set_event_cb(tab3_t.sw_temp, event_handler);
	lv_obj_set_size(tab3_t.sw_temp, 30, 16);
	lv_obj_set_pos(tab3_t.sw_temp, 150, 5);

	/*******page*********/
	tab3_t.page = lv_page_create(obj, NULL);
	lv_obj_set_size(tab3_t.page, 200, 200);
	lv_obj_set_pos(tab3_t.page, 20, 30);

	lv_obj_t *label = lv_label_create(tab3_t.page, NULL);
	lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);			 /*Automatically break long lines*/
	lv_obj_set_width(label, lv_page_get_width_fit(tab3_t.page)); /*Set the label width to max value to not show hor. scroll bars*/
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_label_set_text(label, "这是一个开源项目\n"
							 "作者：Kevincoooool\n"
							 "https://github.com/Kevincoooool/KS_ESP32_CAM\n");
}
static void event_handler_tab(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		int8_t i = lv_tabview_get_tab_act(obj);
		switch (i)
		{
		case 0:
			lv_group_remove_all_objs(group_button);
			lv_group_add_obj(group_button, tabview);			   //加入解码组
			lv_group_add_obj(group_button, tab1_t.sw_wifi);		   //加入解码组
			lv_group_add_obj(group_button, tab1_t.sw_cam);		   //加入解码组
			lv_group_add_obj(group_button, tab1_t.sw_cam_vflip);   //加入解码组
			lv_group_add_obj(group_button, tab1_t.sw_cam_hmirror); //加入解码组
			lv_group_add_obj(group_button, tab1_t.page);		   //加入解码组

			break;
		case 1:
			lv_group_remove_all_objs(group_button);
			lv_group_add_obj(group_button, tabview); //加入解码组
			lv_group_add_obj(group_button, tab2_ac_t.list_type); //加入解码组
			lv_group_add_obj(group_button, tab2_ac_t.list_code); //加入解码组
			break;
		case 2:
			lv_group_remove_all_objs(group_button);
			lv_group_add_obj(group_button, tabview);		//加入解码组
			lv_group_add_obj(group_button, tab3_t.sw_temp); //加入解码组
			break;
		default:
			break;
		}
	}
}

void lv_tabview_init(void)
{
	/*Create a Tab view object*/
	tabview = lv_tabview_create(scr, NULL);

	static lv_style_t style1;
	lv_obj_set_style_local_pad_all(tabview, LV_TABVIEW_PART_TAB_BTN, LV_STATE_DEFAULT, LV_DPI / 12);

	lv_obj_add_style(tabview, LV_BTN_PART_MAIN, &style1); /*Default button style*/
	lv_tabview_set_btns_pos(tabview, LV_TABVIEW_TAB_POS_TOP);

	tab1 = lv_tabview_add_tab(tabview, LV_SYMBOL_WIFI);
	tab2 = lv_tabview_add_tab(tabview, LV_SYMBOL_CALL);
	tab3 = lv_tabview_add_tab(tabview, LV_SYMBOL_SETTINGS);

	tab_wifi_init(tab1);
	tab_ac_init(tab2);
	tab_set_init(tab3);
	lv_obj_set_event_cb(tabview, event_handler_tab);
}
compment_t page_tab;

static void group_init()
{

	lv_group_remove_all_objs(group_button);
	lv_group_add_obj(group_button, tabview);			   //加入解码组
	lv_group_add_obj(group_button, tab1_t.sw_wifi);		   //加入解码组
	lv_group_add_obj(group_button, tab1_t.sw_cam);		   //加入解码组
	lv_group_add_obj(group_button, tab1_t.sw_cam_vflip);   //加入解码组
	lv_group_add_obj(group_button, tab1_t.sw_cam_hmirror); //加入解码组
	lv_group_add_obj(group_button, tab1_t.page);		   //加入解码组
	lv_button_set_group(group_button);
}
static void page_load()
{
	page_tab.start = 240;
	page_tab.end = 0;
	page_tab.obj = tabview;
	page_tab.start_t = 300;
	page_tab.end_t = 300;
	page_tab.cb = (void *)lv_anim_path_ease_out;
	page_tab.funcb = (void *)lv_obj_set_x;
	page_tab.next = NULL;
	obj_move(&page_tab, 1);
	ANIEND
	group_init();
}

void page_tab_end()
{
	page_tab_save_ac_type();
	obj_move(&page_tab, 0);
	ANIEND
	lv_obj_del(tabview);
}
void page_tab_start()
{

	lv_tabview_init();
	page_load();
	ESP_LOGI("SYSTEM", "esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
}
