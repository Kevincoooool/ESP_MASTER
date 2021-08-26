
#include "lvgl.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"


typedef struct
{
	uint16_t min;
	uint16_t max;
	uint8_t bpp;
	uint8_t reserved[3];
} x_header_t;
typedef struct
{
	uint32_t pos;
} x_table_t;
typedef struct
{
	uint8_t adv_w;
	uint8_t box_w;
	uint8_t box_h;
	int8_t ofs_x;
	int8_t ofs_y;
	uint8_t r;
} glyph_dsc_t;

static x_header_t __g_xbf_hd = {
	.min = 0x0020,
	.max = 0xff1a,
	.bpp = 4,
};
char *Font_buff = NULL;
// static uint8_t __g_font_buf[240]; //如bin文件存在SPI FLASH可使用此buff
static void init_font(void)
{
	FILE *ff = fopen("/spiffs/myFont.bin", "r");
	if (ff == NULL)
	{
		printf("Failed to open file for reading");
		return;
	}
	fseek(ff, 0, SEEK_END);
	long lSize = ftell(ff);
	rewind(ff);
	printf("Lsize %ld", lSize);
	static uint8_t first_in = 1;
	if (first_in == 1)
	{
		first_in = 0;
		Font_buff = (char *)malloc(sizeof(char) * lSize);
	}
	int br = fread(Font_buff, 1, lSize, ff);
	printf("Bytes read %d", br);
	fclose(ff);
}
static uint8_t *__user_font_getdata(int offset, int size)
{
	//如字模保存在SPI FLASH, SPIFLASH_Read(__g_font_buf,offset,size);
	//如字模已加载到SDRAM,直接返回偏移地址即可如:return (uint8_t*)(sdram_fontddr+offset);
	static uint8_t first_in = 1;
	if (first_in == 1)//第一次进入的时候初始化外部字体
	{
		first_in = 0;
		init_font();
	}
	return (uint8_t*)(Font_buff+offset);
	// return __g_font_buf;
}

static const uint8_t *__user_font_get_bitmap(const lv_font_t *font, uint32_t unicode_letter)
{
	if (unicode_letter > __g_xbf_hd.max || unicode_letter < __g_xbf_hd.min)
	{
		return NULL;
	}
	uint32_t unicode_offset = sizeof(x_header_t) + (unicode_letter - __g_xbf_hd.min) * 4;
	uint32_t *p_pos = (uint32_t *)__user_font_getdata(unicode_offset, 4);
	if (p_pos[0] != 0)
	{
		uint32_t pos = p_pos[0];
		glyph_dsc_t *gdsc = (glyph_dsc_t *)__user_font_getdata(pos, sizeof(glyph_dsc_t));
		return __user_font_getdata(pos + sizeof(glyph_dsc_t), gdsc->box_w * gdsc->box_h * __g_xbf_hd.bpp / 8);
	}
	return NULL;
}

static bool __user_font_get_glyph_dsc(const lv_font_t *font, lv_font_glyph_dsc_t *dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
{
	if (unicode_letter > __g_xbf_hd.max || unicode_letter < __g_xbf_hd.min)
	{
		return NULL;
	}
	uint32_t unicode_offset = sizeof(x_header_t) + (unicode_letter - __g_xbf_hd.min) * 4;
	uint32_t *p_pos = (uint32_t *)__user_font_getdata(unicode_offset, 4);
	if (p_pos[0] != 0)
	{
		glyph_dsc_t *gdsc = (glyph_dsc_t *)__user_font_getdata(p_pos[0], sizeof(glyph_dsc_t));
		dsc_out->adv_w = gdsc->adv_w;
		dsc_out->box_h = gdsc->box_h;
		dsc_out->box_w = gdsc->box_w;
		dsc_out->ofs_x = gdsc->ofs_x;
		dsc_out->ofs_y = gdsc->ofs_y;
		dsc_out->bpp = __g_xbf_hd.bpp;
		return true;
	}
	return false;
}

//AliHYAiHei-Beta,,-1
//字模高度：24
//XBF字体,外部bin文件
lv_font_t myFont = {
	.get_glyph_bitmap = __user_font_get_bitmap,
	.get_glyph_dsc = __user_font_get_glyph_dsc,
	.line_height = 24,
	.base_line = 0,
};

