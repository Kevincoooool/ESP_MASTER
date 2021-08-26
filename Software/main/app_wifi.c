/* ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <string.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "app_wifi.h"
#include "page_start.h"

static const char *TAG = "app_wifi";
const int CONNECTED_BIT = BIT0;
const int ESPTOUCH_DONE_BIT = BIT1;
const int WIFI_SMART = BIT2;
const int WIFI_CONNET_BIT = BIT3; //配网连接
const int MQTT_CONNET_BIT = BIT4;
char ip_adder[20];
char ssid[40];
EventGroupHandle_t s_wifi_event_group; //wifi事件组
#define EXAMPLE_ESP_WIFI_MODE_AP 0     //TRUE:AP FALSE:STA
#define EXAMPLE_ESP_WIFI_SSID "CMCC-"
#define EXAMPLE_ESP_WIFI_PASS "99999999"
#define EXAMPLE_MAX_STA_CONN 4
#define EXAMPLE_IP_ADDR "192.168.4.1"

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        static uint32_t i = 0;
        if (i == 5)
        {
            // i = 0;
            smartconfig_set();
        }
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        i++;
        ESP_LOGI(TAG, "retry to connect to the AP");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {

        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        // uart_write_bytes(1, ip4addr_ntoa((ip4_addr_t *)&event->ip_info.ip), strlen((ip4_addr_t *)ip4addr_ntoa(&event->ip_info.ip)));
        sprintf(ip_adder, "%s", ip4addr_ntoa((ip4_addr_t *)&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE)
        ESP_LOGI(TAG, "Scan done");
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL)
        ESP_LOGI(TAG, "Found channel");
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD)
    { //获取密码
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        char password[65];

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true)
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);

        /*打开wifipass工作区并保存密码*/
        if (save_nvs("wifi_ssid", ssid) && save_nvs("wifi_pass", password))
            ESP_LOGI(TAG, "保存密码成功");

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE)
    {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " join, AID=%d",
                 MAC2STR(event->mac), event->aid);
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
        // wifi_connect_status = true;
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        // wifi_connect_status = false;
    }
}
//配网任务
void smartconfig_example_task(void *parm)
{
    ESP_LOGI(TAG, "start smartconfig。。。。。。。1");
    EventBits_t uxBits;

    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS)); //选择esptouch和airkiss配网
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));

    while (1)
    {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, false, false, portMAX_DELAY); //等待配网事件组
        // if(uxBits & CONNECTED_BIT)
        //     ESP_LOGI(TAG, "WiFi Connected to ap");
        if (uxBits & ESPTOUCH_DONE_BIT)
        {
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
            xEventGroupSetBits(s_wifi_event_group, WIFI_SMART); //发送自动配网标志
            vTaskDelete(NULL);
        }
    }
}

#if EXAMPLE_ESP_WIFI_MODE_AP
static void wifi_init_softap()
{
    tcpip_adapter_init();

    if (strcmp(EXAMPLE_IP_ADDR, "192.168.4.1"))
    {
        int a, b, c, d;
        sscanf(EXAMPLE_IP_ADDR, "%d.%d.%d.%d", &a, &b, &c, &d);
        tcpip_adapter_ip_info_t ip_info;
        IP4_ADDR(&ip_info.ip, a, b, c, d);
        IP4_ADDR(&ip_info.gw, a, b, c, d);
        IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);
        ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(WIFI_IF_AP));
        ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(WIFI_IF_AP, &ip_info));
        ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(WIFI_IF_AP));
    }

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    uint8_t mac[6];
    ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_AP, mac));

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    snprintf((char *)wifi_config.ap.ssid, 32, "KSDIY_ESP32_CAM-%x%x", mac[4], mac[5]);
    wifi_config.ap.max_connection = 1;
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;

    esp_wifi_set_ps(WIFI_PS_NONE);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s ", wifi_config.ap.ssid);
    char buf[80];
    sprintf(buf, "SSID:%s", wifi_config.ap.ssid);
    sprintf(buf, "PASSWORD:%s", wifi_config.ap.password);
}

#else

esp_err_t wifi_init_sta()
{
    char password[65];
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    wifi_config_t wifi_config;
    bzero(&wifi_config, sizeof(wifi_config_t));
    if (read_nvs("wifi_ssid", ssid) && read_nvs("wifi_pass", password)) //读取ssid
    {
        ESP_LOGI(TAG, "成功获取 SSID:%s     PASS:%s", ssid, password);
        ESP_LOGI(TAG, "get ssid %s", ssid);

        memcpy(wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
        memcpy(ssid, wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid));
        ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());
    }
    else
    {
        ESP_LOGI(TAG, "没获取到 SSID...使用默认");
        memcpy(wifi_config.sta.ssid, "CMCC-", sizeof("CMCC-"));
        memcpy(wifi_config.sta.password, "99999999", sizeof("99999999"));
        memcpy(ssid, wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid));
        ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());
        // xTaskCreate(smartconfig_example_task, "smartconfig_task", 1024 * 3, NULL, 4, NULL);
        // return ESP_FAIL;
    }

    /*进入阻塞态等待连接*/
    EventBits_t uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT, false, false, 10000 / portTICK_PERIOD_MS);
    if (uxBits & CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "WiFi Connected to ap");
        return ESP_OK;
    }
    return ESP_FAIL;
}
#endif

esp_err_t app_wifi_init()
{

    // #if EXAMPLE_ESP_WIFI_MODE_AP
    //     ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    //     wifi_init_softap();
    // #else
    s_wifi_event_group = xEventGroupCreate();
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    return wifi_init_sta();
    // #endif /*EXAMPLE_ESP_WIFI_MODE_AP*/
}

char *get_ip(void)
{
    return ip_adder;
}
char *get_ssid(void)
{
    return ssid;
}
