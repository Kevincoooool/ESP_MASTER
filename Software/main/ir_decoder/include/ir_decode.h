/**************************************************************************************
Filename:       ir_decode.h
Revised:        Date: 2016-10-01
Revision:       Revision: 1.0

Description:    This file provides algorithms for IR decode

Revision log:
* 2016-10-01: created by strawmanbobi
**************************************************************************************/

#ifndef _IR_DECODE_H_
#define _IR_DECODE_H_

#if defined (BOARD_PC)
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "ir_defs.h"
#include "ir_ac_control.h"
#include "ir_tv_control.h"

#define IR_DECODE_FAILED             (-1)
#define IR_DECODE_SUCCEEDED          (0)

#define IR_TYPE_STATUS               0
#define IR_TYPE_COMMANDS             1

// remote control key definition
#define KEY_TV_POWER                 0
#define KEY_TV_MUTE                  1
#define KEY_TV_UP                    2
#define KEY_TV_DOWN                  3
#define KEY_TV_LEFT                  4
#define KEY_TV_RIGHT                 5
#define KEY_TV_OK                    6
#define KEY_TV_VOL_PLUS              7
#define KEY_TV_VOL_MINUS             8
#define KEY_TV_BACK                  9
#define KEY_TV_INPUT                 10
#define KEY_TV_MENU                  11
#define KEY_TV_HOME                  12
#define KEY_TV_SETTINGS              13

#define KEY_AC_POWER                 0
#define KEY_AC_MODE_SWITCH           1
#define KEY_AC_TEMP_PLUS             2
#define KEY_AC_TEMP_MINUS            3
#define KEY_AC_WIND_SPEED            9
#define KEY_AC_WIND_SWING            10
#define KEY_AC_WIND_FIX              11

#define KEY_STB_POWER                0
#define KEY_STB_MUTE                 1
#define KEY_STB_UP                   2
#define KEY_STB_DOWN                 3
#define KEY_STB_LEFT                 4
#define KEY_STB_RIGHT                5
#define KEY_STB_OK                   6
#define KEY_STB_VOL_PLUS             7
#define KEY_STB_VOL_MINUS            8
#define KEY_STB_BACK                 9
#define KEY_STB_INPUT                10
#define KEY_STB_MENU                 11
#define KEY_STB_PAGE_UP              12
#define KEY_STB_PAGE_DOWN            13

#define KEY_NETBOX_POWER             0
#define KEY_NETBOX_UP                1
#define KEY_NETBOX_DOWN              2
#define KEY_NETBOX_LEFT              3
#define KEY_NETBOX_RIGHT             4
#define KEY_NETBOX_OK                5
#define KEY_NETBOX_VOL_PLUS          6
#define KEY_NETBOX_VOL_MINUS         7
#define KEY_NETBOX_BACK              8
#define KEY_NETBOX_MENU              9
#define KEY_NETBOX_HOME              10

#define KEY_IPTV_POWER               0
#define KEY_IPTV_MUTE                1
#define KEY_IPTV_UP                  2
#define KEY_IPTV_DOWN                3
#define KEY_IPTV_LEFT                4
#define KEY_IPTV_RIGHT               5
#define KEY_IPTV_OK                  6
#define KEY_IPTV_VOL_PLUS            7
#define KEY_IPTV_VOL_MINUS           8
#define KEY_IPTV_BACK                9
#define KEY_IPTV_INPUT               10
#define KEY_IPTV_MENU                11
#define KEY_IPTV_PAGE_UP             12
#define KEY_IPTV_PAGE_DOWN           13

#define KEY_DVD_POWER                0
#define KEY_DVD_UP                   1
#define KEY_DVD_DOWN                 2
#define KEY_DVD_LEFT                 3
#define KEY_DVD_RIGHT                4
#define KEY_DVD_OK                   5
#define KEY_DVD_VOL_PLUS             6
#define KEY_DVD_VOL_MINUS            7
#define KEY_DVD_PLAY                 8
#define KEY_DVD_PAUSE                9
#define KEY_DVD_EJECT                10
#define KEY_DVD_REWIND               11
#define KEY_DVD_FASTFORWARD          12
#define KEY_DVD_MENU                 13

#define KEY_FAN_POWER                0
#define KEY_FAN_WIND_PLUS            6
#define KEY_FAN_WIND_MUNIS           7
#define KEY_FAN_SWING                8
#define KEY_FAN_WIND_SPEED           9
#define KEY_FAN_WIND_TYPE            10

#define KEY_PROJECTOR_POWER          0
#define KEY_PROJECTOR_UP             1
#define KEY_PROJECTOR_DOWN           2
#define KEY_PROJECTOR_LEFT           3
#define KEY_PROJECTOR_RIGHT          4
#define KEY_PROJECTOR_OK             5
#define KEY_PROJECTOR_VOL_PLUS       6
#define KEY_PROJECTOR_VOL_MINUS      7
#define KEY_PROJECTOR_ZOOM_OUT       8
#define KEY_PROJECTOR_MENU           9
#define KEY_PROJECTOR_ZOOM_IN        10
#define KEY_PROJECTOR_BACK           11

#define KEY_STEREO_POWER             0
#define KEY_STEREO_UP                1
#define KEY_STEREO_DOWN              2
#define KEY_STEREO_LEFT              3
#define KEY_STEREO_RIGHT             4
#define KEY_STEREO_OK                5
#define KEY_STEREO_VOL_PLUS          6
#define KEY_STEREO_VOL_MINUS         7
#define KEY_STEREO_MUTE              8
#define KEY_STEREO_MENU              9

#define KEY_BULB_POWER               0
#define KEY_BULB_COLOR_1             1
#define KEY_BULB_COLOR_2             2
#define KEY_BULB_COLOR_3             3
#define KEY_BULB_COLOR_4             4
#define KEY_BULB_COLOR_0             5
#define KEY_BULB_BRIGHT_PLUS         6
#define KEY_BULB_BRIGHT_MINUS        7
#define KEY_BULB_BRIGHT_POWER_ON     8
#define KEY_BULB_BRIGHT_RAINBOW      9
#define KEY_BULB_BRIGHT_POWER_OFF    10

#define KEY_CLEANROBOT_POWER         0
#define KEY_CLEANROBOT_FOWWARD       1
#define KEY_CLEANROBOT_BACKWARD      2
#define KEY_CLEANROBOT_LEFT          3
#define KEY_CLEANROBOT_RIGHT         4
#define KEY_CLEANROBOT_START         5
#define KEY_CLEANROBOT_STOP          6
#define KEY_CLEANROBOT_AUTO          8
#define KEY_CLEANROBOT_SPOT          9
#define KEY_CLEANROBOT_SPEED         10
#define KEY_CLEANROBOT_TIMER         11
#define KEY_CLEANROBOT_CHARGE        12
#define KEY_CLEANROBOT_PRESERVE      13

#define KEY_AIRCLEANER_POWER         0
#define KEY_AIRCLEANER_ION           5
#define KEY_AIRCLEANER_AUTO          8
#define KEY_AIRCLEANER_WIND_SPEED    9
#define KEY_AIRCLEANER_MODE_SWITCH   10
#define KEY_AIRCLEANER_TIMER         11
#define KEY_AIRCLEANER_LIGHT         12
#define KEY_AIRCLEANER_FORCE         13

#define KEY_DYSON_POWER              0
#define KEY_DYSON_WIND_SPEED_PLUS    1
#define KEY_DYSON_WIND_SPEED_MINUS   2
#define KEY_DYSON_TIMER_MINUS        3
#define KEY_DYSON_TIMER_PLUS         4
#define KEY_DYSON_AUTO               5
#define KEY_DYSON_TEMP_PLUS          6
#define KEY_DYSON_TEMP_MINUS         7
#define KEY_DYSON_SWING              8
#define KEY_DYSON_DIFFUSION          9
#define KEY_DYSON_FAV                10
#define KEY_DYSON_TIMER              11
#define KEY_DYSON_SLEEP              12
#define KEY_DYSON_COOL               13

#define STANDARD_KEY_COUNT           14
#define CHANNEL_KEY_COUNT            10

typedef enum
{
    REMOTE_CATEGORY_NONE = 0,
    REMOTE_CATEGORY_AC = 1,
    REMOTE_CATEGORY_TV = 2,
    REMOTE_CATEGORY_STB = 3,
    REMOTE_CATEGORY_NETBOX = 4,
    REMOTE_CATEGORY_IPTV = 5,
    REMOTE_CATEGORY_DVD = 6,
    REMOTE_CATEGORY_FAN = 7,
    REMOTE_CATEGORY_PROJECTOR = 8,
    REMOTE_CATEGORY_STEREO = 9,
    REMOTE_CATEGORY_LIGHT = 10,
    REMOTE_CATEGORY_BSTB = 11,
    REMOTE_CATEGORY_CLEANING_ROBOT = 12,
    REMOTE_CATEGORY_AREMOTE_CLEANER = 13,
	REMOTE_CATEGORY_DYSON = 14,
    REMOTE_CATEGORY_NEXT,
    REMOTE_CATEGORY_MAX = 64,
} t_remote_category;

typedef enum
{
    SUB_CATEGORY_QUATERNARY = 0,
    SUB_CATEGORY_HEXADECIMAL = 1,
    SUB_CATEGORY_NEXT = 2,
    SUB_CATEGORY_MAX = 7,
} t_remote_sub_category;

/**
 * function     get_lib_version
 *
 * description: get version of library
 *
 * parameters:  N/A
 *
 * returns:     the string contains library version
 *
 */
extern const char* get_lib_version();

/**
 * function     ir_file_open
 *
 * description: open IR binary code from file
 *
 * parameters:  category (in) - category ID get from indexing API
 *              sub_category (in) - subcategory ID get from indexing API
 *              file_name (in) - file name of IR binary
 *
 * returns:     IR_DECODE_SUCCEEDED / IR_DECODE_FAILED
 *
 */
extern INT8 ir_file_open(const UINT8 category, const UINT8 sub_category, const char* file_name);

/**
 * function     ir_binary_open
 *
 * description: open IR binary code from buffer
 *
 * parameters:  category (in) - category ID get from indexing API
 *              sub_category (in) - subcategory ID get from indexing API
 *              binary (in) - pointer to binary buffer
 *              bin_length (in) - binary buffer size
 *
 * returns:     IR_DECODE_SUCCEEDED / IR_DECODE_FAILED
 */
extern INT8 ir_binary_open(const UINT8 category, const UINT8 sub_category, UINT8* binary, UINT16 bin_length);

/**
 * function     ir_decode
 *
 * description: decode IR binary into INT16 array which indicates the IR levels
 *
 * parameters:  key_code (in) - the code of pressed key
 *              user_data (out) - output decoded data in INT16 array format
 *              ac_status(in) - pointer to AC status (optional)
 *              change_wind_direction (in) - if control changes wind direction for AC (for AC only)
 *
 * returns:     length of decoded data (0 indicates decode failure)
 */
extern UINT16 ir_decode(UINT8 key_code, UINT16* user_data, t_remote_ac_status* ac_status, BOOL change_wind_direction);

/**
 * function     ir_close
 *
 * description: close IR binary code
 *
 * parameters:  N/A
 *
 * returns:     IR_DECODE_SUCCEEDED / IR_DECODE_FAILED
 */
extern INT8 ir_close();

/**
 * function     get_temperature_range
 *
 * description: get the supported temperature range [min, max] for the opened AC IR binary
 *
 * parameters:  ac_mode (in) specify in which AC mode the application need to get temperature info
 *              temp_min (out) the min temperature supported in a specified AC mode
 *              temp_max (out) the max temperature supported in a specified AC mode
 *
 * returns:     IR_DECODE_SUCCEEDED / IR_DECODE_FAILED
 */
extern INT8 get_temperature_range(UINT8 ac_mode, INT8 *temp_min, INT8 *temp_max);

/**
 * function     get_supported_mode
 *
 * description: get supported mode for the opened AC IR binary
 *
 * parameters:  supported_mode (out) mode supported by the remote in lower 5 bits
 *
 * returns:     IR_DECODE_SUCCEEDED / IR_DECODE_FAILED
 */
extern INT8 get_supported_mode(UINT8 *supported_mode);

/**
 * function     get_supported_wind_speed
 *
 * description: get supported wind speed levels for the opened AC IR binary in certain mode
 *
 * parameters:  ac_mode (in) specify in which AC mode the application need to get wind speed info
 *              supported_wind_speed (out) wind speed supported by the remote in lower 4 bits
 *
 * returns:     IR_DECODE_SUCCEEDED / IR_DECODE_FAILED
 */
extern INT8 get_supported_wind_speed(UINT8 ac_mode, UINT8 *supported_wind_speed);

/**
 * function     get_supported_swing
 *
 * description: get supported swing functions for the opened AC IR binary in certain mode
 *
 * parameters:  ac_mode (in) specify in which AC mode the application need to get swing info
 *              supported_swing (out) swing supported by the remote in lower 2 bits
 *
 * returns:     IR_DECODE_SUCCEEDED / IR_DECODE_FAILED
 */
extern INT8 get_supported_swing(UINT8 ac_mode, UINT8 *supported_swing);

/**
 * function     get_supported_wind_direction
 *
 * description: get supported wind directions for the opened AC IR binary in certain mode
 *
 * parameters:  supported_wind_direction (out) swing supported by the remote in lower 2 bits
 *
 * returns:     IR_DECODE_SUCCEEDED / IR_DECODE_FAILED
 */
extern INT8 get_supported_wind_direction(UINT8 *supported_wind_direction);


// private extern function
#if (defined BOARD_PC || defined BOARD_PC_DLL)
extern void ir_lib_free_inner_buffer();
#endif

// this function is preferred being called by JNI only
UINT16 ir_decode_combo(const UINT8 category, const UINT8 sub_category,
                       UINT8* binary, UINT16 bin_length,
                       UINT8 key_code, UINT16* user_data,
                       t_remote_ac_status* ac_status, BOOL change_wind_direction);

#ifdef __cplusplus
}
#endif

#endif // _IR_DECODE_H_