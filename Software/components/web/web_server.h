/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-09 15:01:27
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-06-10 14:16:00
 * @FilePath     : \esp-idf\KSDIY_ESPCAM\components\web\file_server.h
 */
#ifndef _HTTP_FILE_
#define _HTTP_FILE_
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "esp_err.h"
#include "esp_log.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + CONFIG_SPIFFS_OBJ_NAME_LEN)

/* Max size of an individual file. Make sure this
 * value is same as that set in upload_script.html */
#define MAX_FILE_SIZE   (1000*1024) // 200 KB
#define MAX_FILE_SIZE_STR "1000KB"

/* Scratch buffer size */
#define SCRATCH_BUFSIZE  8192
struct file_server_data {
    /* 文件基路径 */
    char base_path[ESP_VFS_PATH_MAX + 1];

    /* 临时文件缓冲区 */
    char scratch[SCRATCH_BUFSIZE];
};
esp_err_t index_html_get_handler(httpd_req_t *req);
esp_err_t favicon_get_handler(httpd_req_t *req);
esp_err_t http_resp_dir_html(httpd_req_t *req, const char *dirpath);
esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filename);
esp_err_t start_file_server(const char *base_path);//开启文件服务
esp_err_t up_post_handler(httpd_req_t *req);
void stop_webserver();
#endif