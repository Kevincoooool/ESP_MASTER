#ifndef WEATHER_H_
#define WEATHER_H_
#define JINTIAN "今天"
#define MINGTIAN "明天"
#define HOUTIAN "后天"
#include "lvgl/lvgl.h"
struct WEATHER{
    char * weathername;
    char * filename;
};
struct LV_WEATHER{
    char * weathername;
    const lv_img_dsc_t * img_dsc;
};
#endif