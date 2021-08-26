/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-04 16:13:33
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-08 14:29:20
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\app_face.c
 */
#include "app_face.h"
#include "app_main.h"
#include "page_start.h"
#include "spage.h"
#include "fd_forward.h"
#include "dl_lib.h"
#include "fb_gfx.h"
#include "app_camera.h"
#include "esp_http_server.h"
#include "button.h"
static const char *TAG = "FACE";

#define UpAlign4(n) (((n) + 3) & ~3)
#define UpAlign8(n) (((n) + 7) & ~7)

#define ENROLL_CONFIRM_TIMES 5
#define FACE_ID_SAVE_NUMBER 7

extern lv_obj_t *img_cam; //要显示图像
extern lv_group_t *group_button;
extern lv_img_dsc_t img_dsc;
extern uint8_t cam_en, face_en;

#define FACE_COLOR_WHITE 0x00FFFFFF
#define FACE_COLOR_BLACK 0x00000000
#define FACE_COLOR_RED 0x000000FF
#define FACE_COLOR_GREEN 0x0000FF00
#define FACE_COLOR_BLUE 0x00FF0000
#define FACE_COLOR_YELLOW (FACE_COLOR_RED | FACE_COLOR_GREEN)
#define FACE_COLOR_CYAN (FACE_COLOR_BLUE | FACE_COLOR_GREEN)
#define FACE_COLOR_PURPLE (FACE_COLOR_BLUE | FACE_COLOR_RED)

#define ENROLL_CONFIRM_TIMES 1
#define FACE_ID_SAVE_NUMBER 10

face_id_list st_face_list = {0};
int g_is_enrolling = 0;
int g_is_deleting = 0;
dl_matrix3du_t *aligned_face = NULL;

//static void oneshot_timer_callback(void* arg);

const char *number_suffix(int32_t number)
{
    uint8_t n = number % 10;

    if (n == 0)
        return "zero";
    else if (n == 1)
        return "st";
    else if (n == 2)
        return "nd";
    else if (n == 3)
        return "rd";
    else
        return "th";
}

static void rgb_print(dl_matrix3du_t *image_matrix, uint32_t color, const char *str)
{
    fb_data_t fb;
    fb.width = image_matrix->w;
    fb.height = image_matrix->h;
    fb.data = image_matrix->item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    fb_gfx_print(&fb, (fb.width - (strlen(str) * 14)) / 2, 10, color, str);
}
static void rgb_print2(camera_fb_t *image_matrix, uint32_t color, const char *str)
{
    fb_data_t fb;
    fb.width = image_matrix->width;
    fb.height = image_matrix->height;
    fb.data = image_matrix->buf;
    fb.bytes_per_pixel = 2;

    fb.format = FB_RGB565;
    fb_gfx_print(&fb, (fb.width - (strlen(str) * 14)) / 2, 10, color, str);
}

static int rgb_printf(dl_matrix3du_t *image_matrix, uint32_t color, const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            return 0;
        }
    }
    vsnprintf(temp, len + 1, format, arg);
    va_end(arg);
    rgb_print2(image_matrix, color, temp);
    if (len > 64)
    {
        free(temp);
    }
    return len;
}

static void draw_face_boxes(dl_matrix3du_t *image_matrix, box_array_t *boxes)
{
    int x, y, w, h, i;
    uint32_t color = FACE_COLOR_YELLOW;
    fb_data_t fb;
    fb.width = image_matrix->w;
    fb.height = image_matrix->h;
    fb.data = image_matrix->item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    for (i = 0; i < boxes->len; i++)
    {
        // rectangle box
        x = (int)boxes->box[i].box_p[0];
        y = (int)boxes->box[i].box_p[1];
        w = (int)boxes->box[i].box_p[2] - x + 1;
        h = (int)boxes->box[i].box_p[3] - y + 1;
        fb_gfx_drawFastHLine(&fb, x, y, w, color);
        fb_gfx_drawFastHLine(&fb, x, y + h - 1, w, color);
        fb_gfx_drawFastVLine(&fb, x, y, h, color);
        fb_gfx_drawFastVLine(&fb, x + w - 1, y, h, color);
#if 0
        // landmark
        int x0, y0, j;
        for (j = 0; j < 10; j+=2) {
            x0 = (int)boxes->landmark[i].landmark_p[j];
            y0 = (int)boxes->landmark[i].landmark_p[j+1];
            fb_gfx_fillRect(&fb, x0, y0, 3, 3, color);
        }
#endif
    }
}

void draw_face_boxes2(camera_fb_t *image_matrix, box_array_t *boxes)
{
    int x, y, w, h, i;
    uint32_t color = FACE_COLOR_RED;
    fb_data_t fb;
    fb.width = image_matrix->width;
    fb.height = image_matrix->height;
    fb.data = image_matrix->buf;
    fb.bytes_per_pixel = 2;
    fb.format = FB_RGB565;
    for (i = 0; i < boxes->len; i++)
    {
        // rectangle box
        x = (int)boxes->box[i].box_p[0];
        y = (int)boxes->box[i].box_p[1];
        w = (int)boxes->box[i].box_p[2] - x + 1;
        h = (int)boxes->box[i].box_p[3] - y + 1;

        fb_gfx_drawFastHLine(&fb, x, y, w, color);
        fb_gfx_drawFastHLine(&fb, x, y + h - 1, w, color);
        fb_gfx_drawFastVLine(&fb, x, y, h, color);
        fb_gfx_drawFastVLine(&fb, x + w - 1, y, h, color);
#if 1
        // landmark
        int x0, y0, j;
        for (j = 0; j < 10; j += 2)
        {
            x0 = (int)boxes->landmark[i].landmark_p[j];
            y0 = (int)boxes->landmark[i].landmark_p[j + 1];
            fb_gfx_fillRect(&fb, x0, y0, 3, 3, color);
        }
#endif
    }
}
void draw_fillRect(camera_fb_t *image_matrix, int32_t x, int32_t y, int32_t w, int32_t h)
{

    uint32_t color = FACE_COLOR_BLUE;
    fb_data_t fb;
    fb.width = image_matrix->width;
    fb.height = image_matrix->height;
    fb.data = image_matrix->buf;
    fb.bytes_per_pixel = 2;
    fb.format = FB_RGB565;

    fb_gfx_drawFastHLine(&fb, x, y, w, color);
    fb_gfx_drawFastHLine(&fb, x, y + h - 1, w, color);
    fb_gfx_drawFastVLine(&fb, x, y, h, color);
    fb_gfx_drawFastVLine(&fb, x + w - 1, y, h, color);
}

lv_img_dsc_t img_dsc = {
    .header.always_zero = 0,
    .header.w = 240,
    .header.h = 240,
    .data_size = 240 * 240 * 2,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = NULL,
};
uint8_t face_bt = 0;
void Face_DEC(void *arg)
{
    ESP_LOGE(TAG, "Into Face_REC1");
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    dl_matrix3du_t *image_matrix = NULL;

    int face_id = -1;
    int64_t fr_start = 0;
    int64_t fr_ready = 0;
    int64_t fr_face = 0;
    int64_t fr_recognize = 0;
    int64_t fr_encode = 0;

    mtmn_config_t mtmn_config = mtmn_init_config();
    static int64_t last_frame = 0;
    if (!last_frame)
    {
        last_frame = esp_timer_get_time();
    }
    face_id_init(&st_face_list, FACE_ID_SAVE_NUMBER, ENROLL_CONFIRM_TIMES);
    aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);
    read_face_id_from_flash(&st_face_list);
    ESP_LOGE(TAG, "Into Face_REC2");
    while (1)
    {
        if (face_en)
        {
            if (g_state == START_DELETE)
            {
                int8_t left = delete_face_id_in_flash(&st_face_list);
                if (left >= 0)
                    ESP_LOGW(TAG, "%d ID Left", left);
                g_state = START_RECOGNITION;
                continue;
            }

            // Detection Start
            fb = esp_camera_fb_get();
            if (fb == NULL)
            {
                ESP_LOGE(TAG, "Camera capture failed");
                res = ESP_FAIL;
                // break;
            }
            else
            {
                fr_start = esp_timer_get_time();
                fr_ready = fr_start;
                fr_face = fr_start;
                fr_encode = fr_start;
                fr_recognize = fr_start;
                image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
                if (!image_matrix)
                {
                    ESP_LOGE(TAG, "dl_matrix3du_alloc failed");
                    res = ESP_FAIL;
                    break;
                }
                if (!fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item))
                {
                    ESP_LOGW(TAG, "fmt2rgb888 failed");
                    _jpg_buf = fb->buf;
                    _jpg_buf_len = fb->len;
                    res = ESP_FAIL;
                    dl_matrix3du_free(image_matrix);
                    break;
                }
                else
                {
                    // ESP_LOGI(TAG, "Get count 4\n");
                    fr_ready = esp_timer_get_time();
                    box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);
                    fr_face = esp_timer_get_time();
                    // Detection End
                    fr_recognize = fr_face;
                    if (net_boxes)
                    {
                        if ((g_state == START_ENROLL || g_state == START_RECOGNITION) && (align_face(net_boxes, image_matrix, aligned_face) == ESP_OK))
                        {
                            if (g_state == START_ENROLL)
                            {
                                rgb_print(image_matrix, FACE_COLOR_YELLOW, "START ENROLLING");
                                ESP_LOGD(TAG, "START ENROLLING");

                                int left_sample_face = enroll_face_id_to_flash(&st_face_list, aligned_face);
                                ESP_LOGD(TAG, "Face ID %d Enrollment: Taken the %d%s sample",
                                         st_face_list.tail,
                                         ENROLL_CONFIRM_TIMES - left_sample_face,
                                         number_suffix(ENROLL_CONFIRM_TIMES - left_sample_face));
                                // gpio_set_level(GPIO_LED_RED, 0);
                                rgb_printf(image_matrix, FACE_COLOR_CYAN, "\nThe %u%s sample",
                                           ENROLL_CONFIRM_TIMES - left_sample_face,
                                           number_suffix(ENROLL_CONFIRM_TIMES - left_sample_face));

                                if (left_sample_face == 0)
                                {
                                    ESP_LOGI(TAG, "Enrolled Face ID: %d", st_face_list.tail ? st_face_list.tail - 1 : FACE_ID_SAVE_NUMBER - 1);
                                    rgb_printf(image_matrix, FACE_COLOR_CYAN, "\n\nEnrolled Face ID: %d", st_face_list.tail - 1);
                                    g_is_enrolling = 0;
                                    g_state = START_RECOGNITION;
                                }
                            }
                            else
                            {
                                face_id = recognize_face(&st_face_list, aligned_face);
                                if (face_id >= 0)
                                {
                                    // gpio_set_level(GPIO_LED_RED, 1);
                                    rgb_printf(fb, FACE_COLOR_GREEN, "Hello ID %u", face_id);
                                }
                                else
                                {
                                    if (g_state == START_ENROLL)
                                    {

                                        rgb_print2(fb, FACE_COLOR_RED, "START ENROLLING");
                                        ESP_LOGI(TAG, "START ENROLLING");

                                        int left_sample_face = enroll_face_id_to_flash(&st_face_list, aligned_face);
                                        ESP_LOGI(TAG, "Face ID %d Enrollment: Taken the %d%s sample",
                                                 st_face_list.tail,
                                                 ENROLL_CONFIRM_TIMES - left_sample_face,
                                                 number_suffix(ENROLL_CONFIRM_TIMES - left_sample_face));
                                        // gpio_set_level(GPIO_LED_RED, 0);
                                        rgb_printf(fb, FACE_COLOR_CYAN, "\nThe %u%s sample",
                                                   ENROLL_CONFIRM_TIMES - left_sample_face,
                                                   number_suffix(ENROLL_CONFIRM_TIMES - left_sample_face));
                                        if (left_sample_face == 0)
                                        {
                                            ESP_LOGI(TAG, "Enrolled Face ID: %d", st_face_list.tail ? st_face_list.tail - 1 : FACE_ID_SAVE_NUMBER - 1);
                                            // rgb_printf(fb, FACE_COLOR_CYAN, "\n\nEnrolled Face ID: %d", st_face_list.tail - 1);
                                            g_is_enrolling = 0;
                                            g_state = START_RECOGNITION;
                                        }
                                    }
                                    rgb_print2(fb, FACE_COLOR_RED, "\nWHO?");
                                }
                            }
                        }
                        // draw_face_boxes(image_matrix, net_boxes);
                        draw_face_boxes2(fb, net_boxes);
                        dl_lib_free(net_boxes->score);
                        dl_lib_free(net_boxes->box);
                        dl_lib_free(net_boxes->landmark);
                        dl_lib_free(net_boxes);

                        fr_recognize = esp_timer_get_time();
                        // if (!fmt2jpg(image_matrix->item, fb->width * fb->height * 3, fb->width, fb->height, PIXFORMAT_RGB888, 90, &_jpg_buf, &_jpg_buf_len))
                        // {
                        //     ESP_LOGE(TAG, "fmt2jpg failed");
                        // }

                        // esp_camera_fb_return(fb);
                        // fb = NULL;
                    }
                    else
                    {
                        _jpg_buf = fb->buf;
                        _jpg_buf_len = fb->len;
                    }
                    dl_matrix3du_free(image_matrix);
                }
                

                if (fb)
                {
                    img_dsc.data = fb->buf;
                    lv_img_set_src(img_cam, &img_dsc);
                    esp_camera_fb_return(fb);
                    fb = NULL;
                    _jpg_buf = NULL;
                }
                else if (_jpg_buf != NULL)
                {
                    free(_jpg_buf);
                    _jpg_buf = NULL;
                }
                if (res != ESP_OK)
                {
                    break;
                }
            }
        }
        else
        {
            // img_dsc.data = 0;
            // vTaskDelay(1000 / portTICK_PERIOD_MS);
            vTaskDelete(NULL);
        }
    }
}
