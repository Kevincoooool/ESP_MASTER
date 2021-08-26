#!/usr/bin/env python3

import os

lvgldirname = os.path.abspath('..')
lvgldirname = os.path.basename(lvgldirname)
lvgldirname = '"' + lvgldirname + '"'

base_defines = '"-DLV_CONF_PATH=' + lvgldirname +'/tests/lv_test_conf.h -DLV_BUILD_TEST"'
optimization = '"-O3 -g0"'


def build(name, defines):
  global base_defines, optimization

  print("=============================")
  print(name)
  print("=============================")
  
  d_all = base_defines[:-1] + " ";
  
  for d in defines:
    d_all += " -D" + d + "=" + str(defines[d])
  
  d_all += '"'
  cmd = "make -j8 BIN=test.bin LVGL_DIR_NAME=" + lvgldirname + " DEFINES=" + d_all + " OPTIMIZATION=" + optimization
  
  print("---------------------------")
  print("Clean")
  print("---------------------------")
  os.system("make clean LVGL_DIR_NAME=" + lvgldirname)
  os.system("rm -f ./test.bin")
  print("---------------------------")
  print("Build")
  print("---------------------------")
  ret = os.system(cmd)
  if(ret != 0): 
    print("BUILD ERROR! (error code  " + str(ret) + ")")
    exit(1)
  
  print("---------------------------")
  print("Run")
  print("---------------------------")
  ret = os.system("./test.bin")
  if(ret != 0): 
    print("RUN ERROR! (error code  " + str(ret) + ")")
    exit(1)
  
  print("---------------------------")
  print("Finished")
  print("---------------------------")


minimal_monochrome = {
  "LV_DPI":40,
  "LV_MEM_SIZE":4*1024,
  "LV_HOR_RES_MAX":128,
  "LV_VER_RES_MAX":64,
  "LV_COLOR_DEPTH":1,
  "LV_USE_GROUP":0,
  "LV_USE_ANIMATION":0,
  "LV_ANTIALIAS":0,
  "LV_GPU":0,
  "LV_USE_FILESYSTEM":0,
  "LV_USE_IMG_TRANSFORM":0,
  "LV_USE_API_EXTENSION_V6":0,
  "LV_USE_USER_DATA":0,
  "LV_USE_USER_DATA_FREE":0,
  "LV_USE_LOG":0,
  "LV_USE_THEME_EMPTY":0,   
  "LV_USE_THEME_MATERIAL":0,   
  "LV_USE_THEME_MONO":1,   
  "LV_USE_THEME_TEMPLATE":0,   
  "LV_THEME_DEFAULT_INIT": "\\\"lv_theme_mono_init\\\"",
  "LV_THEME_DEFAULT_COLOR_PRIMARY":      "\\\"LV_COLOR_RED\\\"",
  "LV_THEME_DEFAULT_COLOR_SECONDARY":    "\\\"LV_COLOR_BLUE\\\"",
  "LV_THEME_DEFAULT_FLAG"         :     "0",
  "LV_THEME_DEFAULT_FONT_SMALL"    :     "\\\"&lv_font_unscii_8\\\"",
  "LV_THEME_DEFAULT_FONT_NORMAL"   :     "\\\"&lv_font_unscii_8\\\"",
  "LV_THEME_DEFAULT_FONT_SUBTITLE" :     "\\\"&lv_font_unscii_8\\\"",
  "LV_THEME_DEFAULT_FONT_TITLE"    :     "\\\"&lv_font_unscii_8\\\"",
  "LV_LOG_PRINTF":0,
  "LV_USE_DEBUG":0,
  "LV_USE_ASSERT_NULL":0,
  "LV_USE_ASSERT_MEM":0,
  "LV_USE_ASSERT_STR":0,
  "LV_USE_ASSERT_OBJ":0,
  "LV_USE_ASSERT_STYLE":0,
  "LV_FONT_MONTSERRAT_12":0,
  "LV_FONT_MONTSERRAT_16":0,
  "LV_FONT_MONTSERRAT_22":0,
  "LV_FONT_MONTSERRAT_28":0,
  "LV_FONT_MONTSERRAT_12_SUBPX":0,
  "LV_FONT_MONTSERRAT_28_COMPRESSED":0,
  "LV_FONT_UNSCII_8":1,
  "LV_USE_BIDI": 0,
  "LV_USE_OBJ_REALIGN": 0,
  "LV_USE_ARC":0,
  "LV_USE_BAR":1,
  "LV_USE_BTN":1,
  "LV_USE_BTNM":0,
  "LV_USE_CALENDAR":0,
  "LV_USE_CANVAS":0,
  "LV_USE_CHECKBOX":0,
  "LV_USE_CHART":0,
  "LV_USE_CONT":1,
  "LV_USE_CPICKER":0,
  "LV_USE_DROPDOWN":0,
  "LV_USE_GAUGE":0,
  "LV_USE_IMG":1,
  "LV_USE_IMGBTN":0,
  "LV_USE_KEYBOARD":0,
  "LV_USE_LABEL":1,
  "LV_USE_LED":0,
  "LV_USE_LINE":0,
  "LV_USE_LIST":0,
  "LV_USE_LINEMETER":0,
  "LV_USE_OBJMASK":0,
  "LV_USE_MBOX":0,
  "LV_USE_PAGE":0,
  "LV_USE_SPINNER":0,
  "LV_USE_ROLLER":0,
  "LV_USE_SLIDER":0,
  "LV_USE_SPINBOX":0,
  "LV_USE_SWITCH":0,
  "LV_USE_TEXTAREA":0,
  "LV_USE_TABLE":0,
  "LV_USE_TABVIEW":0,
  "LV_USE_TILEVIEW":0,
  "LV_USE_WIN":0 
}

all_obj_minimal_features = {
  "LV_DPI":60,
  "LV_MEM_SIZE":12*1024,
  "LV_HOR_RES_MAX":320,
  "LV_VER_RES_MAX":240,
  "LV_COLOR_DEPTH":8,
  "LV_USE_GROUP":0,
  "LV_USE_ANIMATION":0,
  "LV_ANTIALIAS":0,
  "LV_GPU":0,
  "LV_USE_FILESYSTEM":0,
  "LV_USE_IMG_TRANSFORM":0,
  "LV_USE_API_EXTENSION_V6":0,
  "LV_USE_USER_DATA":0,
  "LV_USE_USER_DATA_FREE":0,
  "LV_USE_LOG":0,
  "LV_USE_THEME_MATERIAL":1,   
  "LV_THEME_DEFAULT_INIT": "\\\"lv_theme_material_init\\\"",
  "LV_THEME_DEFAULT_COLOR_PRIMARY":      "\\\"LV_COLOR_RED\\\"",
  "LV_THEME_DEFAULT_COLOR_SECONDARY":    "\\\"LV_COLOR_BLUE\\\"",
  "LV_THEME_DEFAULT_FLAG"         :     "\\\"LV_THEME_MATERIAL_FLAG_LIGHT\\\"",
  "LV_THEME_DEFAULT_FONT_SMALL"    :     "\\\"&lv_font_montserrat_16\\\"",
  "LV_THEME_DEFAULT_FONT_NORMAL"   :     "\\\"&lv_font_montserrat_16\\\"",
  "LV_THEME_DEFAULT_FONT_SUBTITLE" :     "\\\"&lv_font_montserrat_16\\\"",
  "LV_THEME_DEFAULT_FONT_TITLE"    :     "\\\"&lv_font_montserrat_16\\\"",
  "LV_USE_DEBUG":0,
  "LV_USE_ASSERT_NULL":0,
  "LV_USE_ASSERT_MEM":0,
  "LV_USE_ASSERT_STR":0,
  "LV_USE_ASSERT_OBJ":0,
  "LV_USE_ASSERT_STYLE":0,
  "LV_FONT_MONTSERRAT_12":0,
  "LV_FONT_MONTSERRAT_16":1,
  "LV_FONT_MONTSERRAT_22":0,
  "LV_FONT_MONTSERRAT_28":0,
  "LV_FONT_MONTSERRAT_12_SUBPX":0,
  "LV_FONT_MONTSERRAT_28_COMPRESSED":0,
  "LV_FONT_UNSCII_8":0,
  "LV_USE_BIDI": 0,
  "LV_USE_OBJ_REALIGN": 0,
  "LV_USE_EXT_CLICK_AREA":"LV_EXT_CLICK_AREA_TINY",
  "LV_USE_ARC":1,
  "LV_USE_BAR":1,
  "LV_USE_BTN":1,
  "LV_USE_BTNM":1,
  "LV_USE_CALENDAR":1,
  "LV_USE_CANVAS":1,
  "LV_USE_CHECKBOX":1,
  "LV_USE_CHART":1,
  "LV_USE_CONT":1,
  "LV_USE_CPICKER":1,
  "LV_USE_DROPDOWN":1,
  "LV_USE_GAUGE":1,
  "LV_USE_IMG":1,
  "LV_USE_IMGBTN":1,
  "LV_USE_KEYBOARD":1,
  "LV_USE_LABEL":1,
  "LV_USE_LED":1,
  "LV_USE_LINE":1,
  "LV_USE_LIST":1,
  "LV_USE_LINEMETER":1,
  "LV_USE_OBJMASK":1,
  "LV_USE_MBOX":1,
  "LV_USE_PAGE":1,
  "LV_USE_SPINNER":0, #Disabled beacsue needs anim
  "LV_USE_ROLLER":1,
  "LV_USE_SLIDER":1,
  "LV_USE_SPINBOX":1,
  "LV_USE_SWITCH":1,
  "LV_USE_TEXTAREA":1,
  "LV_USE_TABLE":1,
  "LV_USE_TABVIEW":1,
  "LV_USE_TILEVIEW":1,
  "LV_USE_WIN":1 
}

all_obj_all_features = {
  "LV_DPI":100,
  "LV_MEM_SIZE":32*1024,
  "LV_HOR_RES_MAX":480,
  "LV_VER_RES_MAX":320,
  "LV_COLOR_DEPTH":32,
  "LV_COLOR_SCREEN_TRANSP":1,
  "LV_USE_GROUP":1,
  "LV_USE_ANIMATION":1,
  "LV_ANTIALIAS":1,
  "LV_GPU":1,
  "LV_USE_FILESYSTEM":1,
  "LV_USE_IMG_TRANSFORM":1,
  "LV_USE_API_EXTENSION_V6":1,
  "LV_USE_USER_DATA":1,
  "LV_USE_USER_DATA_FREE":0,
  "LV_USE_LOG":1,
  "LV_USE_THEME_MATERIAL":1,   
  "LV_USE_THEME_EMPTY":1,  
  "LV_USE_THEME_MONO":1, 
  "LV_USE_THEME_TEMPLATE":1,  
  "LV_THEME_DEFAULT_INIT": "\\\"lv_theme_material_init\\\"",
  "LV_THEME_DEFAULT_COLOR_PRIMARY":      "\\\"LV_COLOR_RED\\\"",
  "LV_THEME_DEFAULT_COLOR_SECONDARY":    "\\\"LV_COLOR_BLUE\\\"",
  "LV_THEME_DEFAULT_FLAG"         :     "\\\"LV_THEME_MATERIAL_FLAG_LIGHT\\\"",
  "LV_THEME_DEFAULT_FONT_SMALL"    :     "\\\"&lv_font_montserrat_12\\\"",
  "LV_THEME_DEFAULT_FONT_NORMAL"   :     "\\\"&lv_font_montserrat_16\\\"",
  "LV_THEME_DEFAULT_FONT_SUBTITLE" :     "\\\"&lv_font_montserrat_22\\\"",
  "LV_THEME_DEFAULT_FONT_TITLE"    :     "\\\"&lv_font_montserrat_28\\\"",  
  "LV_LOG_PRINTF":0,
  "LV_USE_DEBUG":0,
  "LV_USE_ASSERT_NULL":0,
  "LV_USE_ASSERT_MEM":0,
  "LV_USE_ASSERT_STR":0,
  "LV_USE_ASSERT_OBJ":0,
  "LV_USE_ASSERT_STYLE":0,
  "LV_FONT_MONTSERRAT_12":1,
  "LV_FONT_MONTSERRAT_16":1,
  "LV_FONT_MONTSERRAT_22":1,
  "LV_FONT_MONTSERRAT_28":1,
  "LV_FONT_MONTSERRAT_12_SUBPX":1,
  "LV_FONT_MONTSERRAT_28_COMPRESSED":1,
  "LV_FONT_UNSCII_8":1,
  "LV_USE_ARC":1,
  "LV_USE_BAR":1,
  "LV_USE_BTN":1,
  "LV_USE_BTNM":1,
  "LV_USE_CALENDAR":1,
  "LV_USE_CANVAS":1,
  "LV_USE_CHECKBOX":1,
  "LV_USE_CHART":1,
  "LV_USE_CONT":1,
  "LV_USE_CPICKER":1,
  "LV_USE_DROPDOWN":1,
  "LV_USE_GAUGE":1,
  "LV_USE_IMG":1,
  "LV_USE_IMGBTN":1,
  "LV_USE_KEYBOARD":1,
  "LV_USE_LABEL":1,
  "LV_USE_LED":1,
  "LV_USE_LINE":1,
  "LV_USE_LIST":1,
  "LV_USE_LINEMETER":1,
  "LV_USE_OBJMASK":1,
  "LV_USE_MBOX":1,
  "LV_USE_PAGE":1,
  "LV_USE_SPINNER":1,
  "LV_USE_ROLLER":1,
  "LV_USE_SLIDER":1,
  "LV_USE_SPINBOX":1,
  "LV_USE_SWITCH":1,
  "LV_USE_TEXTAREA":1,
  "LV_USE_TABLE":1,
  "LV_USE_TABVIEW":1,
  "LV_USE_TILEVIEW":1,
  "LV_USE_WIN":1 
}


advanced_features = {
  "LV_DPI":100,
  "LV_MEM_SIZE":4*1024*1024,
  "LV_MEM_CUSTOM":1,
  "LV_HOR_RES_MAX":800,
  "LV_VER_RES_MAX":480,
  "LV_COLOR_DEPTH":32,
  "LV_COLOR_16_SWAP":0,
  "LV_COLOR_SCREEN_TRANSP":1,
  "LV_USE_GROUP":1,
  "LV_USE_ANIMATION":1,
  "LV_ANTIALIAS":1,
  "LV_GPU":1,
  "LV_USE_FILESYSTEM":1,
  "LV_USE_IMG_TRANSFORM":1,
  "LV_USE_API_EXTENSION_V6":1,
  "LV_USE_USER_DATA":1,
  "LV_IMG_CACHE_DEF_SIZE":32,
  "LV_USE_LOG":1,
  "LV_USE_THEME_MATERIAL":1,  
  "LV_USE_THEME_EMPTY":1,  
  "LV_USE_THEME_TEMPLATE":1,  
  "LV_THEME_DEFAULT_INIT": "\\\"lv_theme_material_init\\\"",
  "LV_THEME_DEFAULT_COLOR_PRIMARY":      "\\\"LV_COLOR_RED\\\"",
  "LV_THEME_DEFAULT_COLOR_SECONDARY":    "\\\"LV_COLOR_BLUE\\\"",
  "LV_THEME_DEFAULT_FLAG"         :     "\\\"LV_THEME_MATERIAL_FLAG_LIGHT\\\"",
  "LV_THEME_DEFAULT_FONT_SMALL"    :     "\\\"&lv_font_montserrat_12\\\"",
  "LV_THEME_DEFAULT_FONT_NORMAL"   :     "\\\"&lv_font_montserrat_16\\\"",
  "LV_THEME_DEFAULT_FONT_SUBTITLE" :     "\\\"&lv_font_montserrat_22\\\"",
  "LV_THEME_DEFAULT_FONT_TITLE"    :     "\\\"&lv_font_montserrat_28\\\"", 
  "LV_LOG_PRINTF":1,
  "LV_USE_DEBUG":1,
  "LV_USE_ASSERT_NULL":1,
  "LV_USE_ASSERT_MEM":1,
  "LV_USE_ASSERT_STR":1,
  "LV_USE_ASSERT_OBJ":1,
  "LV_USE_ASSERT_STYLE":1,
  "LV_FONT_MONTSERRAT_12":1,
  "LV_FONT_MONTSERRAT_16":1,
  "LV_FONT_MONTSERRAT_22":1,
  "LV_FONT_MONTSERRAT_28":1,
  "LV_FONT_MONTSERRAT_12_SUBPX":1,
  "LV_FONT_MONTSERRAT_28_COMPRESSED":1,
  "LV_FONT_UNSCII_8":1,
  "LV_USE_BIDI": 1,
  "LV_USE_REVERSE_ARABIC_PERSIAN_CHARS":1,
  "LV_USE_OBJ_REALIGN": 1,
  "LV_FONT_FMT_TXT_LARGE":1,
  "LV_FONT_SUBPX_BGR":1,
  "LV_USE_BIDI": 1,
  "LV_USE_OBJ_REALIGN": 1,
  "LV_USE_EXT_CLICK_AREA":"LV_EXT_CLICK_AREA_FULL",
  "LV_USE_ARC":1,
  "LV_USE_BAR":1,
  "LV_USE_BTN":1,
  "LV_USE_BTNM":1,
  "LV_USE_CALENDAR":1,
  "LV_USE_CANVAS":1,
  "LV_USE_CHECKBOX":1,
  "LV_USE_CHART":1,
  "LV_USE_CONT":1,
  "LV_USE_CPICKER":1,
  "LV_USE_DROPDOWN":1,
  "LV_USE_GAUGE":1,
  "LV_USE_IMG":1,
  "LV_USE_IMGBTN":1,
  "LV_USE_KEYBOARD":1,
  "LV_USE_LABEL":1,
  "LV_USE_LED":1,
  "LV_USE_LINE":1,
  "LV_USE_LIST":1,
  "LV_USE_LINEMETER":1,
  "LV_USE_OBJMASK":1,
  "LV_USE_MBOX":1,
  "LV_USE_PAGE":1,
  "LV_USE_SPINNER":1,
  "LV_USE_ROLLER":1,
  "LV_USE_SLIDER":1,
  "LV_USE_SPINBOX":1,
  "LV_USE_SWITCH":1,
  "LV_USE_TEXTAREA":1,
  "LV_USE_TABLE":1,
  "LV_USE_TABVIEW":1,
  "LV_USE_TILEVIEW":1,
  "LV_USE_WIN":1 
}


build("Minimal monochrome", minimal_monochrome)
build("All objects, minimal features", all_obj_minimal_features)
build("All objects, all common features", all_obj_all_features)
build("All objects, with advanced features", advanced_features)
  



