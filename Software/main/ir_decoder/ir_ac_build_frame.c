/**************************************************************************************
Filename:       ir_irframe.c
Revised:        Date: 2016-10-01
Revision:       Revision: 1.0

Description:    This file provides algorithms for IR frame build

Revision log:
* 2016-10-01: created by strawmanbobi
**************************************************************************************/

#if defined (BOARD_PC)
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#pragma ide diagnostic ignored "readability-redundant-declaration"
#endif

#include "include/ir_ac_build_frame.h"
#include "include/ir_decode.h"

extern t_ac_protocol* context;


//return bit number per byte,default value is 8
UINT8 bits_per_byte(UINT8 index)
{
    UINT8 i = 0;
    UINT8 size = 0;

    if (context->bit_num_cnt == 0)
        return 8; //defaut value

    if (context->bit_num_cnt >= MAX_BITNUM)
        size = MAX_BITNUM;
    else
        size = (UINT8) context->bit_num_cnt;

    for (i = 0; i < size; i++)
    {
        if (context->bit_num[i].pos == index)
            return (UINT8) context->bit_num[i].bits;
        if (context->bit_num[i].pos > index)
            return 8;
    }
    return 8;
}

UINT16 add_delaycode(UINT8 index)
{
    UINT16 i = 0;
    UINT16 j = 0;
    UINT8 size = 0;
    UINT8 tail_delay_code = 0;
    UINT16 tail_pos = 0;

    if (context->dc_cnt != 0)
    {
        size = (UINT8) context->dc_cnt;

        for (i = 0; i < size; i++)
        {
            if (context->dc[i].pos == index)
            {
                for (j = 0; j < context->dc[i].time_cnt; j++)
                {
                    context->time[context->code_cnt++] = context->dc[i].time[j];
                }
            }
            else if (context->dc[i].pos == -1)
            {
                tail_delay_code = 1;
                tail_pos = i;
            }
        }
    }

    if ((context->last_bit == 0) && (index == (ir_hex_len - 1)))
    {
        context->time[context->code_cnt++] = context->one.low; //high
    }

    if (context->dc_cnt != 0)
    {
        if ((index == (ir_hex_len - 1)) && (tail_delay_code == 1))
        {
            for (i = 0; i < context->dc[tail_pos].time_cnt; i++)
            {
                context->time[context->code_cnt++] = context->dc[tail_pos].time[i];
            }
        }
    }

    return context->dc[i].time_cnt;
}

UINT16 create_ir_frame()
{
    UINT16 i = 0, j = 0;
    UINT8 bit_num = 0;
    UINT8 *ir_data = ir_hex_code;
    UINT8 mask = 0;
    UINT16 frame_length = 0;

    context->code_cnt = 0;

    // boot code
    for (i = 0; i < context->boot_code.len; i++)
    {
        context->time[context->code_cnt++] = context->boot_code.data[i];
    }

    for (i = 0; i < ir_hex_len; i++)
    {
        bit_num = bits_per_byte((UINT8) i);
        for (j = 0; j < bit_num; j++)
        {
            if (context->endian == 0)
                mask = (UINT8) ((1 << (bit_num - 1)) >> j);
            else
                mask = (UINT8) (1 << j);

            if (ir_data[i] & mask)
            {
                context->time[context->code_cnt++] = context->one.low;
                context->time[context->code_cnt++] = context->one.high;
            }
            else
            {
                context->time[context->code_cnt++] = context->zero.low;
                context->time[context->code_cnt++] = context->zero.high;
            }
        }
        add_delaycode((UINT8) i);
    }

    frame_length = context->code_cnt;

    for (i = 0; i < (context->repeat_times - 1); i++)
    {
        for (j = 0; j < frame_length; j++)
        {
            context->time[context->code_cnt++] = context->time[j];
        }
    }

    return context->code_cnt;
}