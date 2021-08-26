#include "gifdec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

typedef struct Entry
{
    uint16_t length;
    uint16_t prefix;
    uint8_t suffix;
} Entry;

typedef struct Table
{
    int bulk;
    int nentries;
    Entry *entries;
} Table;

static gd_GIF *gif_open(gd_GIF *gif);
static bool f_gif_open(gd_GIF *gif, const void *path, bool is_file);
static void f_gif_read(gd_GIF *gif, void *buf, size_t len);
static int f_gif_seek(gd_GIF *gif, size_t pos, int k);
static void f_gif_close(gd_GIF *gif);

static uint16_t
read_num(gd_GIF *gif)
{
    uint8_t bytes[2];

    f_gif_read(gif, bytes, 2);
    return bytes[0] + (((uint16_t)bytes[1]) << 8);
}

gd_GIF *
gd_open_gif_file(const char *fname)
{
    gd_GIF gif_base;
    memset(&gif_base, 0, sizeof(gif_base));

    bool res = f_gif_open(&gif_base, fname, true);
    if (!res)
        return NULL;

    return gif_open(&gif_base);
}

gd_GIF *
gd_open_gif_data(const void *data)
{
    gd_GIF gif_base;
    memset(&gif_base, 0, sizeof(gif_base));

    bool res = f_gif_open(&gif_base, data, false);
    if (!res)
        return NULL;

    return gif_open(&gif_base);
}

static gd_GIF *gif_open(gd_GIF *gif_base)
{
    uint8_t sigver[3];
    uint16_t width, height, depth;
    uint8_t fdsz, bgidx, aspect;
    int gct_sz;
    /* Header */
    f_gif_read(gif_base, sigver, 3);
    if (memcmp(sigver, "GIF", 3) != 0)
    {
        fprintf(stderr, "invalid signature\n");
        goto fail;
    }
    /* Version */
    f_gif_read(gif_base, sigver, 3);
    if (memcmp(sigver, "89a", 3) != 0)
    {
        fprintf(stderr, "invalid version\n");
        goto fail;
    }
    /* Width x Height */
    width = read_num(gif_base);
    height = read_num(gif_base);
    /* FDSZ */
    f_gif_read(gif_base, &fdsz, 1);
    /* Presence of GCT */
    if (!(fdsz & 0x80))
    {
        fprintf(stderr, "no global color table\n");
        goto fail;
    }
    /* Color Space's Depth */
    depth = ((fdsz >> 4) & 7) + 1;
    /* Ignore Sort Flag. */
    /* GCT Size */
    gct_sz = 1 << ((fdsz & 0x07) + 1);
    /* Background Color Index */
    f_gif_read(gif_base, &bgidx, 1);
    /* Aspect Ratio */
    f_gif_read(gif_base, &aspect, 1);
    /* Create gd_GIF Structure. */
    gd_GIF *gif;
    gif = lv_mem_alloc(sizeof(*gif) + 256 * 4 + width * height);
    if (!gif)
    {
        goto fail;
    }
    memcpy(gif, gif_base, sizeof(gd_GIF));

    gif->palette = (uint8_t *)&gif[1];
    gif->canvas = (uint8_t *)&gif->palette[256 * 4];

    gif->width = width;
    gif->height = height;
    gif->depth = depth;
    /* Read GCT */
    gif->gct_size = gct_sz;
    f_gif_read(gif, gif->global_palette, 3 * gif->gct_size);
    /*Make the color LGL compatible (add alpha byte)*/
    uint32_t i;
    for (i = 0; i < gct_sz; i++)
    {
        gif->palette[i * 4 + 0] = gif->global_palette[i * 3 + 2];
        gif->palette[i * 4 + 1] = gif->global_palette[i * 3 + 1];
        gif->palette[i * 4 + 2] = gif->global_palette[i * 3 + 0];
        gif->palette[i * 4 + 3] = 0xFF;
    }
    gif->bgindex = bgidx;

    if (gif->bgindex)
        memset(gif->canvas, gif->bgindex, gif->width * gif->height);
    gif->anim_start = f_gif_seek(gif, 0, SEEK_CUR);
    return gif;

fail:
    f_gif_close(gif_base);
    return NULL;
}

static void
discard_sub_blocks(gd_GIF *gif)
{
    uint8_t size;

    do
    {
        f_gif_read(gif, &size, 1);
        f_gif_seek(gif, size, SEEK_CUR);
    } while (size);
}

static void
read_plain_text_ext(gd_GIF *gif)
{

    /* Discard plain text metadata. */
    f_gif_seek(gif, 13, SEEK_CUR);
    /* Discard plain text sub-blocks. */
    discard_sub_blocks(gif);
}

static void
read_graphic_control_ext(gd_GIF *gif)
{
    uint8_t rdit;

    /* Discard block size (always 0x04). */
    f_gif_seek(gif, 1, SEEK_CUR);
    f_gif_read(gif, &rdit, 1);
    gif->gce.disposal = (rdit >> 2) & 3;
    gif->gce.input = rdit & 2;
    gif->gce.transparency = rdit & 1;
    gif->gce.delay = read_num(gif);
    f_gif_read(gif, &gif->gce.tindex, 1);
    /* Skip block terminator. */
    f_gif_seek(gif, 1, SEEK_CUR);
}

static void
read_comment_ext(gd_GIF *gif)
{
    /* Discard comment sub-blocks. */
    discard_sub_blocks(gif);
}

static void
read_application_ext(gd_GIF *gif)
{
    char app_id[8];
    char app_auth_code[3];

    /* Discard block size (always 0x0B). */
    f_gif_seek(gif, 1, SEEK_CUR);
    /* Application Identifier. */
    f_gif_read(gif, app_id, 8);
    /* Application Authentication Code. */
    f_gif_read(gif, app_auth_code, 3);
    if (!strncmp(app_id, "NETSCAPE", sizeof(app_id)))
    {
        /* Discard block size (0x03) and constant byte (0x01). */
        f_gif_seek(gif, 2, SEEK_CUR);
        gif->loop_count = read_num(gif);
        /* Skip block terminator. */
        f_gif_seek(gif, 1, SEEK_CUR);
    }
    else
    {
        discard_sub_blocks(gif);
    }
}

static void
read_ext(gd_GIF *gif)
{
    uint8_t label;

    f_gif_read(gif, &label, 1);
    switch (label)
    {
    case 0x01:
        read_plain_text_ext(gif);
        break;
    case 0xF9:
        read_graphic_control_ext(gif);
        break;
    case 0xFE:
        read_comment_ext(gif);
        break;
    case 0xFF:
        read_application_ext(gif);
        break;
    default:
        fprintf(stderr, "unknown extension: %02X\n", label);
    }
}

static Table *
new_table(int key_size)
{
    int key;
    int init_bulk = MAX(1 << (key_size + 1), 0x100);
    Table *table = lv_mem_alloc(sizeof(*table) + sizeof(Entry) * init_bulk);
    if (table)
    {
        table->bulk = init_bulk;
        table->nentries = (1 << key_size) + 2;
        table->entries = (Entry *)&table[1];
        for (key = 0; key < (1 << key_size); key++)
            table->entries[key] = (Entry){1, 0xFFF, key};
    }
    return table;
}

/* Add table entry. Return value:
 *  0 on success
 *  +1 if key size must be incremented after this addition
 *  -1 if could not realloc table */
static int
add_entry(Table **tablep, uint16_t length, uint16_t prefix, uint8_t suffix)
{
    Table *table = *tablep;
    if (table->nentries == table->bulk)
    {
        table->bulk *= 2;
        table = lv_mem_realloc(table, sizeof(*table) + sizeof(Entry) * table->bulk);
        if (!table)
            return -1;
        table->entries = (Entry *)&table[1];
        *tablep = table;
    }
    table->entries[table->nentries] = (Entry){length, prefix, suffix};
    table->nentries++;
    if ((table->nentries & (table->nentries - 1)) == 0)
        return 1;
    return 0;
}

static uint16_t
get_key(gd_GIF *gif, int key_size, uint8_t *sub_len, uint8_t *shift, uint8_t *byte)
{
    int bits_read;
    int rpad;
    int frag_size;
    uint16_t key;

    key = 0;
    for (bits_read = 0; bits_read < key_size; bits_read += frag_size)
    {
        rpad = (*shift + bits_read) % 8;
        if (rpad == 0)
        {
            /* Update byte. */
            if (*sub_len == 0)
                f_gif_read(gif, sub_len, 1); /* Must be nonzero! */
            f_gif_read(gif, byte, 1);
            (*sub_len)--;
        }
        frag_size = MIN(key_size - bits_read, 8 - rpad);
        key |= ((uint16_t)((*byte) >> rpad)) << bits_read;
    }
    /* Clear extra bits to the left. */
    key &= (1 << key_size) - 1;
    *shift = (*shift + key_size) % 8;
    return key;
}

/* Compute output index of y-th input line, in frame of height h. */
static int
interlaced_line_index(int h, int y)
{
    int p; /* number of lines in current pass */

    p = (h - 1) / 8 + 1;
    if (y < p) /* pass 1 */
        return y * 8;
    y -= p;
    p = (h - 5) / 8 + 1;
    if (y < p) /* pass 2 */
        return y * 8 + 4;
    y -= p;
    p = (h - 3) / 4 + 1;
    if (y < p) /* pass 3 */
        return y * 4 + 2;
    y -= p;
    /* pass 4 */
    return y * 2 + 1;
}

/* Decompress image pixels.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static int read_image_data(gd_GIF *gif, int interlace)
{
    uint8_t sub_len=0, shift=0, byte=0;
    int init_key_size=0, key_size=0, table_is_full=0;
    int frm_off=0, str_len=0, p=0, x=0, y=0;
    uint16_t key=0, clear=0, stop=0;
    int ret=0;
    Table *table;
    Entry entry={0};
    off_t start, end;

    f_gif_read(gif, &byte, 1);
    key_size = (int)byte;
    start = f_gif_seek(gif, 0, SEEK_CUR);
    discard_sub_blocks(gif);
    end = f_gif_seek(gif, 0, SEEK_CUR);
    f_gif_seek(gif, start, SEEK_SET);
    clear = 1 << key_size;
    stop = clear + 1;
    table = new_table(key_size);
    key_size++;
    init_key_size = key_size;
    sub_len = shift = 0;
    key = get_key(gif, key_size, &sub_len, &shift, &byte); /* clear code */
    frm_off = 0;
    ret = 0;
    while (1)
    {
        if (key == clear)
        {
            key_size = init_key_size;
            table->nentries = (1 << (key_size - 1)) + 2;
            table_is_full = 0;
        }
        else if (!table_is_full)
        {
            ret = add_entry(&table, str_len + 1, key, entry.suffix);
            //printf("gif ret %d\r\n",ret);
            if (ret == -1)
            {
                lv_mem_free(table);
                return -1;
            }
            if (table->nentries == 0x1000)
            {
                ret = 0;
                table_is_full = 1;
            }
        }
        key = get_key(gif, key_size, &sub_len, &shift, &byte);
        if (key == clear)
            continue;
        if (key == stop)
            break;
        if (ret == 1)
            key_size++;
        entry = table->entries[key];
        str_len = entry.length;
        while (1)
        {
            p = frm_off + entry.length - 1;
            x = p % gif->fw;
            y = p / gif->fw;
            if (interlace)
                y = interlaced_line_index((int)gif->fh, y);
            if (!gif->gce.transparency || entry.suffix != gif->gce.tindex)
            {
                gif->canvas[(gif->fy + y) * gif->width + gif->fx + x] = entry.suffix;
            }
            if (entry.prefix == 0xFFF)
                break;
            else
                entry = table->entries[entry.prefix];
        }
        frm_off += str_len;
        if (key < table->nentries - 1 && !table_is_full)
            table->entries[table->nentries - 1].suffix = entry.suffix;
    }
    lv_mem_free(table);
    f_gif_read(gif, &sub_len, 1); /* Must be zero! */
    f_gif_seek(gif, end, SEEK_SET);
    return 0;
}

/* Read image.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static int
read_image(gd_GIF *gif)
{
    uint8_t fisrz;
    int interlace;

    /* Image Descriptor. */
    gif->fx = read_num(gif);
    gif->fy = read_num(gif);
    gif->fw = read_num(gif);
    gif->fh = read_num(gif);
    f_gif_read(gif, &fisrz, 1);
    interlace = fisrz & 0x40;
    /* Ignore Sort Flag. */
    /* Local Color Table? */
    if (fisrz & 0x80)
    {
        /* Read LCT */
        uint16_t lct_size = 1 << ((fisrz & 0x07) + 1);
        uint8_t local_palette[256 * 3];
        f_gif_read(gif, local_palette, 3 * lct_size);
        uint32_t i;
        for (i = 0; i < lct_size; i++)
        {
            gif->palette[i * 4 + 0] = local_palette[i * 3 + 2];
            gif->palette[i * 4 + 1] = local_palette[i * 3 + 1];
            gif->palette[i * 4 + 2] = local_palette[i * 3 + 0];
            gif->palette[i * 4 + 3] = 0xFF;
        }
    }
    else
    {
        uint32_t i;
        for (i = 0; i < gif->gct_size; i++)
        {
            gif->palette[i * 4 + 0] = gif->global_palette[i * 3 + 2];
            gif->palette[i * 4 + 1] = gif->global_palette[i * 3 + 1];
            gif->palette[i * 4 + 2] = gif->global_palette[i * 3 + 0];
            gif->palette[i * 4 + 3] = 0xFF;
        }
    }

    /*Make the transparent BG color transparent in the palette*/
    if (gif->gce.transparency)
        gif->palette[gif->gce.tindex * 4 + 3] = 0x00;

    /* Image Data. */
    return read_image_data(gif, interlace);
}

static void
dispose(gd_GIF *gif)
{
    int i, y;
    switch (gif->gce.disposal)
    {
    case 2: /* Restore to background color. */
        i = gif->fy * gif->width + gif->fx;
        for (y = 0; y < gif->fh; y++)
        {
            memset(&gif->canvas[i], gif->bgindex, gif->fw);
            i += gif->width;
        }
        break;
    case 3: /* Restore to previous, i.e., don't update canvas.*/
        break;
    }
}

/* Return 1 if got a frame; 0 if got GIF trailer; -1 if error. */
int gd_get_frame(gd_GIF *gif)
{
    char sep;

    dispose(gif);
    f_gif_read(gif, &sep, 1);
    while (sep != ',')
    {
        if (sep == ';')
            return 0;
        if (sep == '!')
            read_ext(gif);
        else
            return -1;
        f_gif_read(gif, &sep, 1);
    }
    if (read_image(gif) == -1)
        return -1;
    return 1;
}

void gd_rewind(gd_GIF *gif)
{
    f_gif_seek(gif, gif->anim_start, SEEK_SET);
}

void gd_close_gif(gd_GIF *gif)
{
    f_gif_close(gif);
    lv_mem_free(gif);
}

static bool f_gif_open(gd_GIF *gif, const void *path, bool is_file)
{
    gif->f_rw_p = 0;
    gif->data = NULL;


    if (is_file)
    {

        return false;

    }
    else
    {
        gif->data = path;
        return true;
    }
}

static void f_gif_read(gd_GIF *gif, void *buf, size_t len)
{

    {
        memcpy(buf, &gif->data[gif->f_rw_p], len);
        gif->f_rw_p += len;
    }
}

static int f_gif_seek(gd_GIF *gif, size_t pos, int k)
{

    if (k == SEEK_CUR)
        gif->f_rw_p += pos;
    else if (k == SEEK_SET)
        gif->f_rw_p = pos;
    return gif->f_rw_p;

}

static void f_gif_close(gd_GIF *gif)
{

}
