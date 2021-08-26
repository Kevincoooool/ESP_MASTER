#include "weather.h"
LV_IMG_DECLARE(weather0);
LV_IMG_DECLARE(weather1);
LV_IMG_DECLARE(weather2);
LV_IMG_DECLARE(weather4);
LV_IMG_DECLARE(weather5);
LV_IMG_DECLARE(weather6);
LV_IMG_DECLARE(weather10);
LV_IMG_DECLARE(weather11);
LV_IMG_DECLARE(weather13);
LV_IMG_DECLARE(weather14);
LV_IMG_DECLARE(weather15);
LV_IMG_DECLARE(weather16);
LV_IMG_DECLARE(weather19);
LV_IMG_DECLARE(weather20);
LV_IMG_DECLARE(weather21);
LV_IMG_DECLARE(weather22);
LV_IMG_DECLARE(weather24);
LV_IMG_DECLARE(weather26);
LV_IMG_DECLARE(weather23);
LV_IMG_DECLARE(weather28);
LV_IMG_DECLARE(weather30);
LV_IMG_DECLARE(weather31);
LV_IMG_DECLARE(weather32);
LV_IMG_DECLARE(weather35);
LV_IMG_DECLARE(weather36);
LV_IMG_DECLARE(weather40);
// const struct WEATHER weather[40]={//https://docs.seniverse.com/api/start/code.html
//     {"晴","0.png"},//0
//     {"晴","1.png"},
//     {"晴","0.png"},
//     {"晴","1.png"},//3
//     {"多云","4.png"},//4
//     {"晴间多云","5.png"},//5
//     {"晴间多云","6.png"},//6
//     {"大部多云","5.png"},//7
//     {"大部多云","6.png"},//8
//     {"阴","4.png"},//9
//     {"阵雨","10.png"},//10
//     {"雷阵雨","11.png"},//11
//     {"雷雨冰雹","12.png"},//12
//     {"小雨","13.png"},//13
//     {"中雨","14.png"},//14
//     {"大雨","16.png"},//15
//     {"暴雨","16.png"},//16
//     {"大暴雨","15.png"},//17
//     {"特大暴雨","18.png"},//18
//     {"冻雨","19.png"},//19
//     {"雨夹雪","20.png"},//20
//     {"阵雪","21.png"},//21
//     {"小雪","22.png"},//22
//     {"中雪","23.png"},//23
//     {"大雪","24.png"},//24
//     {"暴雪","24.png"},//25
//     {"浮尘","26.png"},//26
//     {"扬沙","26.png"},//27
//     {"沙尘暴","28.png"},//28
//     {"强沙尘暴","28.png"},//29
//     {"雾","30.png"},//30
//     {"霾","31.png"},//31
//     {"风","32.png"},//32
//     {"大风","32.png"},//33
//     {"飓风","32.png"},//34
//     {"热带风","35.png"},//35
//     {"龙卷风","36.png"},//36
//     {"冷","40.png"},//37
//     {"热","40.png"},//38
//     {"未知","40.png"},//39
// };
const struct LV_WEATHER lv_weather[40]={//https://docs.seniverse.com/api/start/code.html
    {"晴",&weather0},//0
    {"晴",&weather1},
    {"晴",&weather0},
    {"晴",&weather1},//3
    {"多云",&weather4},//4
    {"晴间多云",&weather5},//5
    {"晴间多云",&weather6},//6
    {"大部多云",&weather5},//7
    {"大部多云",&weather6},//8
    {"阴",&weather4},//9
    {"阵雨",&weather10},//10
    {"雷阵雨",&weather11},//11
    {"雷雨冰雹",&weather11},//12
    {"小雨",&weather13},//13
    {"中雨",&weather14},//14
    {"大雨",&weather16},//15
    {"暴雨",&weather16},//16
    {"大暴雨",&weather15},//17
    {"特大暴雨",&weather15},//18
    {"冻雨",&weather19},//19
    {"雨夹雪",&weather20},//20
    {"阵雪",&weather21},//21
    {"小雪",&weather22},//22
    {"中雪",&weather23},//23
    {"大雪",&weather24},//24
    {"暴雪",&weather24},//25
    {"浮尘",&weather26},//26
    {"扬沙",&weather26},//27
    {"沙尘暴",&weather28},//28
    {"强沙尘暴",&weather28},//29
    {"雾",&weather30},//30
    {"霾",&weather32},//31
    {"风",&weather32},//32
    {"大风",&weather32},//33
    {"飓风",&weather32},//34
    {"热带风",&weather35},//35
    {"龙卷风",&weather36},//36
    {"冷",&weather40},//37
    {"热",&weather40},//38
    {"未知",&weather40},//39
};