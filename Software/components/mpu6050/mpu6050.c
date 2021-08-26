/**
 * @file mpu6050.c
 * 
 * @author
 * Gabriel Boni Vicari (133192@upf.br)
 * GEPID - Grupo de Pesquisa em Cultura Digital (http://gepid.upf.br/)
 * Universidade de Passo Fundo (http://www.upf.br/)
 *
 * @copyright 2018 Jeff Rowberg
 *
 * @brief MPU6050 library for ESP32 ESP-IDF.
 */

#include "mpu6050.h"
#include <string.h>

const char *TAG_MPU6050 = "MPU6050";

#define PI (3.14159265358979323846f)
#define GYRO_MEAS_ERROR (PI * (60.0f / 180.0f))
#define GYRO_MEAS_DRIFT (PI * (1.0f / 180.0f))
#define BETA (sqrt(3.0f / 4.0f) * GYRO_MEAS_ERROR)
#define ZETA (sqrt(3.0f / 4.0f) * GYRO_MEAS_DRIFT)

float quart[4] = {1.0f, 0.0f, 0.0f, 0.0f};
float delta_t = 0.0f;
float pitch, yaw, roll;
int last_update = 0, first_update = 0, now = 0;

void mpu6050_init()
{
    mpu6050_device_address = MPU6050_DEVICE;
    mpu6050_set_clock_source(MPU6050_CLOCK_PLL_ZGYRO);
    mpu6050_set_full_scale_gyro_range(MPU6050_GYRO_FULL_SCALE_RANGE_2000);
    mpu6050_set_full_scale_accel_range(MPU6050_ACCEL_FULL_SCALE_RANGE_8);
    mpu6050_set_sleep_enabled(0);
}

bool mpu6050_test_connection()
{
    return (mpu6050_get_device_id() == 0x34);
}

const char *mpu6050_get_tag()
{
    return (TAG_MPU6050);
}

uint8_t mpu6050_get_aux_vddio_level()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_YG_OFFS_TC,
        MPU6050_TC_PWR_MODE_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_aux_vddio_level(uint8_t level)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_YG_OFFS_TC,
        MPU6050_TC_PWR_MODE_BIT,
        level);
}

uint8_t mpu6050_get_rate()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_SMPLRT_DIV,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_rate(uint8_t rate)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_SMPLRT_DIV,
        rate);
}

uint8_t mpu6050_get_external_frame_sync()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_CONFIG,
        MPU6050_CFG_EXT_SYNC_SET_BIT,
        MPU6050_CFG_EXT_SYNC_SET_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_external_frame_sync(uint8_t sync)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_CONFIG,
        MPU6050_CFG_EXT_SYNC_SET_BIT,
        MPU6050_CFG_EXT_SYNC_SET_LENGTH,
        sync);
}

uint8_t mpu6050_get_dlpf_mode()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_CONFIG,
        MPU6050_CFG_DLPF_CFG_BIT,
        MPU6050_CFG_DLPF_CFG_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_dlpf_mode(uint8_t mode)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_CONFIG,
        MPU6050_CFG_DLPF_CFG_BIT,
        MPU6050_CFG_DLPF_CFG_LENGTH,
        mode);
}

uint8_t mpu6050_get_full_scale_gyro_range()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_GYRO_CONFIG,
        MPU6050_GCONFIG_FS_SEL_BIT,
        MPU6050_GCONFIG_FS_SEL_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_full_scale_gyro_range(uint8_t range)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_GYRO_CONFIG,
        MPU6050_GCONFIG_FS_SEL_BIT,
        MPU6050_GCONFIG_FS_SEL_LENGTH,
        range);
}

uint8_t mpu6050_get_accel_x_self_test_factory_trim()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_SELF_TEST_X,
        &buffer[0]);
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_SELF_TEST_A,
        &buffer[1]);

    return ((buffer[0] >> 3) | ((buffer[1] >> 4) & 0x03));
}

uint8_t mpu6050_get_accel_y_self_test_factory_trim()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_SELF_TEST_Y,
        &buffer[0]);
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_SELF_TEST_A,
        &buffer[1]);

    return ((buffer[0] >> 3) | ((buffer[1] >> 2) & 0x03));
}

uint8_t mpu6050_get_accel_z_self_test_factory_trim()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_SELF_TEST_Z,
        2,
        buffer);

    return ((buffer[0] >> 3) | (buffer[1] & 0x03));
}

uint8_t mpu6050_get_gyro_x_self_test_factory_trim()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_SELF_TEST_X,
        buffer);

    return ((buffer[0] & 0x1F));
}

uint8_t mpu6050_get_gyro_y_self_test_factory_trim()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_SELF_TEST_Y,
        buffer);

    return ((buffer[0] & 0x1F));
}

uint8_t mpu6050_get_gyro_z_self_test_factory_trim()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_SELF_TEST_Z,
        buffer);

    return ((buffer[0] & 0x1F));
}

bool mpu6050_get_accel_x_self_test()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_CONFIG,
        MPU6050_ACONFIG_XA_ST_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_accel_x_self_test(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_CONFIG,
        MPU6050_ACONFIG_XA_ST_BIT,
        enabled);
}

bool mpu6050_get_accel_y_self_test()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_CONFIG,
        MPU6050_ACONFIG_YA_ST_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_accel_y_self_test(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_CONFIG,
        MPU6050_ACONFIG_YA_ST_BIT,
        enabled);
}

bool mpu6050_get_accel_z_self_test()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_CONFIG,
        MPU6050_ACONFIG_ZA_ST_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_accel_z_self_test(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_CONFIG,
        MPU6050_ACONFIG_ZA_ST_BIT,
        enabled);
}

uint8_t mpu6050_get_full_scale_accel_range()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_CONFIG,
        MPU6050_ACONFIG_AFS_SEL_BIT,
        MPU6050_ACONFIG_AFS_SEL_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_full_scale_accel_range(uint8_t range)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_CONFIG,
        MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH,
        range);
}

uint8_t mpu6050_get_dhpf_mode()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_CONFIG,
        MPU6050_ACONFIG_ACCEL_HPF_BIT, MPU6050_ACONFIG_ACCEL_HPF_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_dhpf_mode(uint8_t mode)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_CONFIG,
        MPU6050_ACONFIG_ACCEL_HPF_BIT,
        MPU6050_ACONFIG_ACCEL_HPF_LENGTH,
        mode);
}

uint8_t mpu6050_get_freefall_detection_threshold()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_FF_THR,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_freefall_detection_threshold(uint8_t threshold)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_FF_THR,
        threshold);
}

uint8_t mpu6050_get_freefall_detection_duration()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_FF_DUR,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_freefall_detection_duration(uint8_t duration)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_FF_DUR,
        duration);
}

uint8_t mpu6050_get_motion_detection_threshold()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_THR,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_motion_detection_threshold(uint8_t threshold)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_THR,
        threshold);
}

uint8_t mpu6050_get_motion_detection_duration()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DUR,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_motion_detection_duration(uint8_t duration)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DUR,
        duration);
}

uint8_t mpu6050_get_zero_motion_detection_threshold()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_ZRMOT_THR,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_zero_motion_detection_threshold(uint8_t threshold)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_ZRMOT_THR,
        threshold);
}

uint8_t mpu6050_get_zero_motion_detection_duration()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_ZRMOT_DUR,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_zero_motion_detection_duration(uint8_t duration)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_ZRMOT_DUR,
        duration);
}

bool mpu6050_get_temp_fifo_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_TEMP_FIFO_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_temp_fifo_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_TEMP_FIFO_EN_BIT,
        enabled);
}

bool mpu6050_get_x_gyro_fifo_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_XG_FIFO_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_x_gyro_fifo_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_XG_FIFO_EN_BIT,
        enabled);
}

bool mpu6050_get_y_gyro_fifo_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_YG_FIFO_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_y_gyro_fifo_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_YG_FIFO_EN_BIT,
        enabled);
}

bool mpu6050_get_z_gyro_fifo_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_ZG_FIFO_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_z_gyro_fifo_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_ZG_FIFO_EN_BIT,
        enabled);
}

bool mpu6050_get_accel_fifo_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_ACCEL_FIFO_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_accel_fifo_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_ACCEL_FIFO_EN_BIT,
        enabled);
}

bool mpu6050_get_slave_2_fifo_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_SLV2_FIFO_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_2_fifo_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_SLV2_FIFO_EN_BIT,
        enabled);
}

bool mpu6050_get_slave_1_fifo_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_SLV1_FIFO_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_1_fifo_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_SLV1_FIFO_EN_BIT,
        enabled);
}

bool mpu6050_get_slave_0_fifo_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_SLV0_FIFO_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_0_fifo_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_EN,
        MPU6050_SLV0_FIFO_EN_BIT,
        enabled);
}

bool mpu6050_get_multi_master_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_CTRL,
        MPU6050_MULT_MST_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_multi_master_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_CTRL,
        MPU6050_MULT_MST_EN_BIT,
        enabled);
}

bool mpu6050_get_wait_for_external_sensor_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_CTRL,
        MPU6050_WAIT_FOR_ES_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_wait_for_external_sensor_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_CTRL,
        MPU6050_WAIT_FOR_ES_BIT,
        enabled);
}

bool mpu6050_get_slave_3_fifo_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_CTRL,
        MPU6050_SLV_3_FIFO_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_3_fifo_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_CTRL,
        MPU6050_SLV_3_FIFO_EN_BIT,
        enabled);
}

bool mpu6050_get_slave_read_write_transition_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_CTRL,
        MPU6050_I2C_MST_P_NSR_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_read_write_transition_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_CTRL,
        MPU6050_I2C_MST_P_NSR_BIT,
        enabled);
}

uint8_t mpu6050_get_master_clock_speed()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_CTRL,
        MPU6050_I2C_MST_CLK_BIT,
        MPU6050_I2C_MST_CLK_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_master_clock_speed(uint8_t speed)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_CTRL,
        MPU6050_I2C_MST_CLK_BIT,
        MPU6050_I2C_MST_CLK_LENGTH,
        speed);
}

uint8_t mpu6050_get_slave_address(uint8_t num)
{
    if (num > 3)
        return (0);

    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_ADDR + num * 3,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_address(uint8_t num, uint8_t address)
{
    if (num > 3)
        return;

    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_ADDR + num * 3,
        address);
}

uint8_t mpu6050_get_slave_register(uint8_t num)
{
    if (num > 3)
        return (0);

    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_REG + num * 3,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_register(uint8_t num, uint8_t reg)
{
    if (num > 3)
        return;

    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_REG + num * 3,
        reg);
}

bool mpu6050_get_slave_enabled(uint8_t num)
{
    if (num > 3)
        return (0);

    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_CTRL + num * 3,
        MPU6050_I2C_SLV_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_enabled(uint8_t num, bool enabled)
{
    if (num > 3)
        return;

    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_CTRL + num * 3,
        MPU6050_I2C_SLV_EN_BIT,
        enabled);
}

bool mpu6050_get_slave_word_byte_swap(uint8_t num)
{
    if (num > 3)
        return (0);

    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_CTRL + num * 3,
        MPU6050_I2C_SLV_BYTE_SW_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_word_byte_swap(uint8_t num, bool enabled)
{
    if (num > 3)
        return;

    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_CTRL + num * 3,
        MPU6050_I2C_SLV_BYTE_SW_BIT,
        enabled);
}

bool mpu6050_get_slave_write_mode(uint8_t num)
{
    if (num > 3)
        return (0);

    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_CTRL + num * 3,
        MPU6050_I2C_SLV_REG_DIS_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_write_mode(uint8_t num, bool mode)
{
    if (num > 3)
        return;

    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_CTRL + num * 3,
        MPU6050_I2C_SLV_REG_DIS_BIT,
        mode);
}

bool mpu6050_get_slave_word_group_offset(uint8_t num)
{
    if (num > 3)
        return (0);

    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_CTRL + num * 3,
        MPU6050_I2C_SLV_GRP_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_word_group_offset(uint8_t num, bool enabled)
{
    if (num > 3)
        return;

    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_CTRL + num * 3,
        MPU6050_I2C_SLV_GRP_BIT,
        enabled);
}

uint8_t mpu6050_get_slave_data_length(uint8_t num)
{
    if (num > 3)
        return (0);

    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_CTRL + num * 3,
        MPU6050_I2C_SLV_LEN_BIT,
        MPU6050_I2C_SLV_LEN_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_data_length(uint8_t num, uint8_t length)
{
    if (num > 3)
        return;

    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_CTRL + num * 3,
        MPU6050_I2C_SLV_LEN_BIT,
        MPU6050_I2C_SLV_LEN_LENGTH,
        length);
}

uint8_t mpu6050_get_slave_4_address()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_ADDR,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_4_address(uint8_t address)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_ADDR,
        address);
}

uint8_t mpu6050_get_slave_4_register()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_REG,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_4_register(uint8_t reg)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_REG,
        reg);
}

void mpu6050_set_slave_4_output_byte(uint8_t data)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_DO,
        data);
}

bool mpu6050_get_slave_4_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_CTRL,
        MPU6050_I2C_SLV4_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_4_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_CTRL,
        MPU6050_I2C_SLV4_EN_BIT,
        enabled);
}

bool mpu6050_get_slave_4_interrupt_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_CTRL,
        MPU6050_I2C_SLV4_INT_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_4_interrupt_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_CTRL,
        MPU6050_I2C_SLV4_INT_EN_BIT,
        enabled);
}

bool mpu6050_get_slave_4_write_mode()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_CTRL,
        MPU6050_I2C_SLV4_REG_DIS_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_4_write_mode(bool mode)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_CTRL,
        MPU6050_I2C_SLV4_REG_DIS_BIT,
        mode);
}

uint8_t mpu6050_get_slave_4_master_delay()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_CTRL,
        MPU6050_I2C_SLV4_MST_DLY_BIT,
        MPU6050_I2C_SLV4_MST_DLY_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_4_master_delay(uint8_t delay)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_CTRL,
        MPU6050_I2C_SLV4_MST_DLY_BIT,
        MPU6050_I2C_SLV4_MST_DLY_LENGTH,
        delay);
}

uint8_t mpu6050_get_slave_4_input_byte()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV4_DI,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_passthrough_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_STATUS,
        MPU6050_MST_PASS_THROUGH_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_slave_4_is_done()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_STATUS,
        MPU6050_MST_I2C_SLV4_DONE_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_lost_arbitration()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_STATUS,
        MPU6050_MST_I2C_LOST_ARB_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_slave_4_nack()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_STATUS,
        MPU6050_MST_I2C_SLV4_NACK_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_slave_3_nack()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_STATUS,
        MPU6050_MST_I2C_SLV3_NACK_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_slave_2_nack()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_STATUS,
        MPU6050_MST_I2C_SLV2_NACK_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_slave_1_nack()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_STATUS,
        MPU6050_MST_I2C_SLV1_NACK_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_slave_0_nack()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_STATUS,
        MPU6050_MST_I2C_SLV0_NACK_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_interrupt_mode()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_INT_LEVEL_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_interrupt_mode(bool mode)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_INT_LEVEL_BIT,
        mode);
}

bool mpu6050_get_interrupt_drive()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_INT_OPEN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_interrupt_drive(bool drive)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_INT_OPEN_BIT,
        drive);
}

bool mpu6050_get_interrupt_latch()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_LATCH_INT_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_interrupt_latch(bool latch)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_LATCH_INT_EN_BIT,
        latch);
}

bool mpu6050_get_interrupt_latch_clear()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_INT_RD_CLEAR_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_interrupt_latch_clear(bool clear)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_INT_RD_CLEAR_BIT,
        clear);
}

bool mpu6050_get_fsync_interrupt_level()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_fsync_interrupt_level(bool level)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT,
        level);
}

bool mpu6050_get_fsync_interrupt_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_FSYNC_INT_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_fsync_interrupt_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_FSYNC_INT_EN_BIT,
        enabled);
}

bool mpu6050_get_i2c_bypass_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_I2C_BYPASS_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_i2c_bypass_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_I2C_BYPASS_EN_BIT,
        enabled);
}

bool mpu6050_get_clock_output_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_CLKOUT_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_clock_output_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_PIN_CFG,
        MPU6050_INTCFG_CLKOUT_EN_BIT,
        enabled);
}

uint8_t mpu6050_get_int_enabled()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_int_enabled(uint8_t enabled)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        enabled);
}

bool mpu6050_get_int_freefall_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_FF_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_int_freefall_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_FF_BIT,
        enabled);
}

bool mpu6050_get_int_motion_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_MOT_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_int_motion_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_MOT_BIT,
        enabled);
}

bool mpu6050_get_int_zero_motion_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_ZMOT_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_int_zero_motion_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_ZMOT_BIT,
        enabled);
}

bool mpu6050_get_int_fifo_buffer_overflow_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_FIFO_OFLOW_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_int_fifo_buffer_overflow_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_FIFO_OFLOW_BIT,
        enabled);
}

bool mpu6050_get_int_i2c_master_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_I2C_MST_INT_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_int_i2c_master_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_I2C_MST_INT_BIT,
        enabled);
}

bool mpu6050_get_int_data_ready_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_DATA_RDY_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_int_data_ready_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_DATA_RDY_BIT,
        enabled);
}

uint8_t mpu6050_get_int_status()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_STATUS,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_int_freefall_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_STATUS,
        MPU6050_INTERRUPT_FF_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_int_motion_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_STATUS,
        MPU6050_INTERRUPT_MOT_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_int_zero_motion_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_STATUS,
        MPU6050_INTERRUPT_ZMOT_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_int_fifo_buffer_overflow_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_STATUS,
        MPU6050_INTERRUPT_FIFO_OFLOW_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_int_i2c_master_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_STATUS,
        MPU6050_INTERRUPT_I2C_MST_INT_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_int_data_ready_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_STATUS,
        MPU6050_INTERRUPT_DATA_RDY_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_get_acceleration(mpu6050_acceleration_t *data)
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_XOUT_H,
        6,
        buffer);
    data->accel_x = (((int16_t)buffer[0]) << 8) | buffer[1];
    data->accel_y = (((int16_t)buffer[2]) << 8) | buffer[3];
    data->accel_z = (((int16_t)buffer[4]) << 8) | buffer[5];
}

int16_t mpu6050_get_acceleration_x()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_XOUT_H,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

int16_t mpu6050_get_acceleration_y()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_YOUT_H,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

int16_t mpu6050_get_acceleration_z()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_ZOUT_H,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

int16_t mpu6050_get_temperature()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_TEMP_OUT_H,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

void mpu6050_get_rotation(mpu6050_rotation_t *data)
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_GYRO_XOUT_H,
        6,
        buffer);
    data->gyro_x = (((int16_t)buffer[0]) << 8) | buffer[1];
    data->gyro_y = (((int16_t)buffer[2]) << 8) | buffer[3];
    data->gyro_z = (((int16_t)buffer[4]) << 8) | buffer[5];
}

int16_t mpu6050_get_rotation_x()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_GYRO_XOUT_H,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

int16_t mpu6050_get_rotation_y()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_GYRO_YOUT_H,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

int16_t mpu6050_get_rotation_z()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_GYRO_ZOUT_H,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

void mpu6050_get_motion(
    mpu6050_acceleration_t *data_accel,
    mpu6050_rotation_t *data_gyro)
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_ACCEL_XOUT_H,
        14,
        buffer);

    data_accel->accel_x = (((int16_t)buffer[0]) << 8) | buffer[1];
    data_accel->accel_y = (((int16_t)buffer[2]) << 8) | buffer[3];
    data_accel->accel_z = (((int16_t)buffer[4]) << 8) | buffer[5];
    data_gyro->gyro_x = (((int16_t)buffer[8]) << 8) | buffer[9];
    data_gyro->gyro_y = (((int16_t)buffer[10]) << 8) | buffer[11];
    data_gyro->gyro_z = (((int16_t)buffer[12]) << 8) | buffer[13];
}

uint8_t mpu6050_get_external_sensor_byte(int position)
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_EXT_SENS_DATA_00 + position,
        buffer);

    return (buffer[0]);
}

uint16_t mpu6050_get_external_sensor_word(int position)
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_EXT_SENS_DATA_00 + position,
        2,
        buffer);

    return ((((uint16_t)buffer[0]) << 8) | buffer[1]);
}

uint32_t mpu6050_get_external_sensor_dword(int position)
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_EXT_SENS_DATA_00 + position,
        4,
        buffer);

    return (
        (((uint32_t)buffer[0]) << 24) |
        (((uint32_t)buffer[1]) << 16) |
        (((uint16_t)buffer[2]) << 8) |
        buffer[3]);
}

uint8_t mpu6050_get_motion_status()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_STATUS,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_x_negative_motion_detected()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_STATUS,
        MPU6050_MOTION_MOT_XNEG_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_x_positive_motion_detected()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_STATUS,
        MPU6050_MOTION_MOT_XPOS_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_y_negative_motion_detected()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_STATUS,
        MPU6050_MOTION_MOT_YNEG_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_y_positive_motion_detected()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_STATUS,
        MPU6050_MOTION_MOT_YPOS_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_z_negative_motion_detected()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_STATUS,
        MPU6050_MOTION_MOT_ZNEG_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_z_positive_motion_detected()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_STATUS,
        MPU6050_MOTION_MOT_ZPOS_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_zero_motion_detected()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_STATUS,
        MPU6050_MOTION_MOT_ZRMOT_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_output_byte(uint8_t num, uint8_t data)
{
    if (num > 3)
        return;

    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_SLV0_DO + num,
        data);
}

bool mpu6050_get_external_shadow_delay_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_DELAY_CTRL,
        MPU6050_DLYCTRL_DELAY_ES_SHADOW_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_external_shadow_delay_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_DELAY_CTRL,
        MPU6050_DLYCTRL_DELAY_ES_SHADOW_BIT,
        enabled);
}

bool mpu6050_get_slave_delay_enabled(uint8_t num)
{
    if (num > 4)
        return (0);

    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_DELAY_CTRL,
        num,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_slave_delay_enabled(uint8_t num, bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_I2C_MST_DELAY_CTRL,
        num,
        enabled);
}

void mpu6050_reset_gyroscope_path()
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_SIGNAL_PATH_RESET,
        MPU6050_PATHRESET_GYRO_RESET_BIT,
        1);
}

void mpu6050_reset_accelerometer_path()
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_SIGNAL_PATH_RESET,
        MPU6050_PATHRESET_ACCEL_RESET_BIT,
        1);
}

void mpu6050_reset_temperature_path()
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_SIGNAL_PATH_RESET,
        MPU6050_PATHRESET_TEMP_RESET_BIT,
        1);
}

uint8_t mpu6050_get_accelerometer_power_on_delay()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_CTRL,
        MPU6050_DETECT_ACCEL_DELAY_BIT,
        MPU6050_DETECT_ACCEL_DELAY_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_accelerometer_power_on_delay(uint8_t delay)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_CTRL,
        MPU6050_DETECT_ACCEL_DELAY_BIT,
        MPU6050_DETECT_ACCEL_DELAY_LENGTH,
        delay);
}

uint8_t mpu6050_get_freefall_detection_counter_decrement()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_CTRL,
        MPU6050_DETECT_FF_COUNT_BIT,
        MPU6050_DETECT_FF_COUNT_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_freefall_detection_counter_decrement(uint8_t decrement)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_CTRL,
        MPU6050_DETECT_FF_COUNT_BIT,
        MPU6050_DETECT_FF_COUNT_LENGTH,
        decrement);
}

uint8_t mpu6050_get_motion_detection_counter_decrement()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_CTRL,
        MPU6050_DETECT_MOT_COUNT_BIT,
        MPU6050_DETECT_MOT_COUNT_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_motion_detection_counter_decrement(uint8_t decrement)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_MOT_DETECT_CTRL,
        MPU6050_DETECT_MOT_COUNT_BIT,
        MPU6050_DETECT_MOT_COUNT_LENGTH,
        decrement);
}

bool mpu6050_get_fifo_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_USER_CTRL,
        MPU6050_USERCTRL_FIFO_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_fifo_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_USER_CTRL,
        MPU6050_USERCTRL_FIFO_EN_BIT,
        enabled);
}

bool mpu6050_get_i2c_master_mode_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_USER_CTRL,
        MPU6050_USERCTRL_I2C_MST_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_i2c_master_mode_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_USER_CTRL,
        MPU6050_USERCTRL_I2C_MST_EN_BIT,
        enabled);
}

void mpu6050_switch_spie_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_USER_CTRL,
        MPU6050_USERCTRL_I2C_IF_DIS_BIT,
        enabled);
}

void mpu6050_reset_fifo()
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_USER_CTRL,
        MPU6050_USERCTRL_FIFO_RESET_BIT,
        1);
}

void mpu6050_reset_sensors()
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_USER_CTRL,
        MPU6050_USERCTRL_SIG_COND_RESET_BIT,
        1);
}

void mpu6050_reset()
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_1,
        MPU6050_PWR1_DEVICE_RESET_BIT,
        1);
}

bool mpu6050_get_sleep_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_1,
        MPU6050_PWR1_SLEEP_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_sleep_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_1,
        MPU6050_PWR1_SLEEP_BIT,
        enabled);
}

bool mpu6050_get_wake_cycle_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_1,
        MPU6050_PWR1_CYCLE_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_wake_cycle_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_1,
        MPU6050_PWR1_CYCLE_BIT,
        enabled);
}

bool mpu6050_get_temp_sensor_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_1,
        MPU6050_PWR1_TEMP_DIS_BIT,
        buffer);

    return (buffer[0] == 0);
}

void mpu6050_set_temp_sensor_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_1,
        MPU6050_PWR1_TEMP_DIS_BIT,
        !enabled);
}

uint8_t mpu6050_get_clock_source()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_1,
        MPU6050_PWR1_CLKSEL_BIT,
        MPU6050_PWR1_CLKSEL_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_clock_source(uint8_t source)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_1,
        MPU6050_PWR1_CLKSEL_BIT,
        MPU6050_PWR1_CLKSEL_LENGTH,
        source);
}

uint8_t mpu6050_get_wake_frequency()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_LP_WAKE_CTRL_BIT,
        MPU6050_PWR2_LP_WAKE_CTRL_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_wake_frequency(uint8_t frequency)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_LP_WAKE_CTRL_BIT,
        MPU6050_PWR2_LP_WAKE_CTRL_LENGTH,
        frequency);
}

bool mpu6050_get_standby_x_accel_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_STBY_XA_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_standby_x_accel_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_STBY_XA_BIT,
        enabled);
}

bool mpu6050_get_standby_y_accel_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_STBY_YA_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_standby_y_accel_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_STBY_YA_BIT,
        enabled);
}

bool mpu6050_get_standby_z_accel_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_STBY_ZA_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_standby_z_accel_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_STBY_ZA_BIT,
        enabled);
}

bool mpu6050_get_standby_x_gyro_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_STBY_XG_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_standby_x_gyro_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_STBY_XG_BIT,
        enabled);
}

bool mpu6050_get_standby_y_gyro_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_STBY_YG_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_standby_y_gyro_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_STBY_YG_BIT,
        enabled);
}

bool mpu6050_get_standby_z_gyro_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_STBY_ZG_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_standby_z_gyro_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_PWR_MGMT_2,
        MPU6050_PWR2_STBY_ZG_BIT,
        enabled);
}

uint16_t mpu6050_get_fifo_count()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_COUNTH,
        2,
        buffer);

    return ((((uint16_t)buffer[0]) << 8) | buffer[1]);
}

uint8_t mpu6050_get_fifo_byte()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_R_W,
        buffer);

    return (buffer[0]);
}

void mpu6050_get_fifo_bytes(uint8_t *data, uint8_t length)
{
    if (length > 0)
    {
        esp32_i2c_read_bytes(
            mpu6050_device_address,
            MPU6050_REGISTER_FIFO_R_W,
            length,
            data);
    }
    else
        *data = 0;
}

void mpu6050_set_fifo_byte(uint8_t data)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_FIFO_R_W,
        data);
}

uint8_t mpu6050_get_device_id()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_WHO_AM_I,
        MPU6050_WHO_AM_I_BIT,
        MPU6050_WHO_AM_I_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_device_id(uint8_t id)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_WHO_AM_I,
        MPU6050_WHO_AM_I_BIT,
        MPU6050_WHO_AM_I_LENGTH,
        id);
}

uint8_t mpu6050_get_otp_bank_valid()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_XG_OFFS_TC,
        MPU6050_TC_OTP_BNK_VLD_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_otp_bank_valid(int8_t enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_XG_OFFS_TC,
        MPU6050_TC_OTP_BNK_VLD_BIT,
        enabled);
}

int8_t mpu6050_get_x_gyro_offset_tc()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_XG_OFFS_TC,
        MPU6050_TC_OFFSET_BIT,
        MPU6050_TC_OFFSET_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_x_gyro_offset_tc(int8_t offset)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_XG_OFFS_TC,
        MPU6050_TC_OFFSET_BIT,
        MPU6050_TC_OFFSET_LENGTH,
        offset);
}

int8_t mpu6050_get_y_gyro_offset_tc()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_YG_OFFS_TC,
        MPU6050_TC_OFFSET_BIT,
        MPU6050_TC_OFFSET_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_y_gyro_offset_tc(int8_t offset)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_YG_OFFS_TC,
        MPU6050_TC_OFFSET_BIT,
        MPU6050_TC_OFFSET_LENGTH,
        offset);
}

int8_t mpu6050_get_z_gyro_offset_tc()
{
    esp32_i2c_read_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_ZG_OFFS_TC,
        MPU6050_TC_OFFSET_BIT,
        MPU6050_TC_OFFSET_LENGTH,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_z_gyro_offset_tc(int8_t offset)
{
    esp32_i2c_write_bits(
        mpu6050_device_address,
        MPU6050_REGISTER_ZG_OFFS_TC,
        MPU6050_TC_OFFSET_BIT,
        MPU6050_TC_OFFSET_LENGTH,
        offset);
}

int8_t mpu6050_get_x_fine_gain()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_X_FINE_GAIN,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_x_fine_gain(int8_t gain)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_X_FINE_GAIN,
        gain);
}

int8_t mpu6050_get_y_fine_gain()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_Y_FINE_GAIN,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_y_fine_gain(int8_t gain)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_Y_FINE_GAIN,
        gain);
}

int8_t mpu6050_get_z_fine_gain()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_Z_FINE_GAIN,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_z_fine_gain(int8_t gain)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_Z_FINE_GAIN,
        gain);
}

int16_t mpu6050_get_x_accel_offset()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_XA_OFFS_H,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

void mpu6050_set_x_accel_offset(int16_t offset)
{
    esp32_i2c_write_word(
        mpu6050_device_address,
        MPU6050_REGISTER_XA_OFFS_H,
        offset);
}

int16_t mpu6050_get_y_accel_offset()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_YA_OFFS_H,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

void mpu6050_set_y_accel_offset(int16_t offset)
{
    esp32_i2c_write_word(
        mpu6050_device_address,
        MPU6050_REGISTER_YA_OFFS_H,
        offset);
}

int16_t mpu6050_get_z_accel_offset()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_ZA_OFFS_H,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

void mpu6050_set_z_accel_offset(int16_t offset)
{
    esp32_i2c_write_word(
        mpu6050_device_address,
        MPU6050_REGISTER_ZA_OFFS_H,
        offset);
}

int16_t mpu6050_get_x_gyro_offset()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_XG_OFFS_USRH,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

void mpu6050_set_x_gyro_offset(int16_t offset)
{
    esp32_i2c_write_word(
        mpu6050_device_address,
        MPU6050_REGISTER_XG_OFFS_USRH,
        offset);
}

int16_t mpu6050_get_y_gyro_offset()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_YG_OFFS_USRH,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

void mpu6050_set_y_gyro_offset(int16_t offset)
{
    esp32_i2c_write_word(
        mpu6050_device_address,
        MPU6050_REGISTER_YG_OFFS_USRH,
        offset);
}

int16_t mpu6050_get_z_gyro_offset()
{
    esp32_i2c_read_bytes(
        mpu6050_device_address,
        MPU6050_REGISTER_ZG_OFFS_USRH,
        2,
        buffer);

    return ((((int16_t)buffer[0]) << 8) | buffer[1]);
}

void mpu6050_set_z_gyro_offset(int16_t offset)
{
    esp32_i2c_write_word(
        mpu6050_device_address,
        MPU6050_REGISTER_ZG_OFFS_USRH,
        offset);
}

bool mpu6050_get_int_pll_ready_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_PLL_RDY_INT_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_int_pll_ready_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_PLL_RDY_INT_BIT,
        enabled);
}

bool mpu6050_get_int_dmp_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_DMP_INT_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_int_dmp_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_ENABLE,
        MPU6050_INTERRUPT_DMP_INT_BIT,
        enabled);
}

bool mpu6050_get_dmp_int_5_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_DMP_INT_STATUS,
        MPU6050_DMPINT_5_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_dmp_int_4_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_DMP_INT_STATUS,
        MPU6050_DMPINT_4_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_dmp_int_3_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_DMP_INT_STATUS,
        MPU6050_DMPINT_3_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_dmp_int_2_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_DMP_INT_STATUS,
        MPU6050_DMPINT_2_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_dmp_int_1_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_DMP_INT_STATUS,
        MPU6050_DMPINT_1_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_dmp_int_0_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_DMP_INT_STATUS,
        MPU6050_DMPINT_0_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_int_ppl_ready_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_STATUS,
        MPU6050_INTERRUPT_PLL_RDY_INT_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_int_dmp_status()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_INT_STATUS,
        MPU6050_INTERRUPT_DMP_INT_BIT,
        buffer);

    return (buffer[0]);
}

bool mpu6050_get_dmp_enabled()
{
    esp32_i2c_read_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_USER_CTRL,
        MPU6050_USERCTRL_DMP_EN_BIT,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_dmp_enabled(bool enabled)
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_USER_CTRL,
        MPU6050_USERCTRL_DMP_EN_BIT,
        enabled);
}

void mpu6050_reset_dmp()
{
    esp32_i2c_write_bit(
        mpu6050_device_address,
        MPU6050_REGISTER_USER_CTRL,
        MPU6050_USERCTRL_DMP_RESET_BIT,
        1);
}

uint8_t mpu6050_get_dmp_config_1()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_DMP_CFG_1,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_dmp_config_1(uint8_t config)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_DMP_CFG_1,
        config);
}

uint8_t mpu6050_get_dmp_config_2()
{
    esp32_i2c_read_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_DMP_CFG_2,
        buffer);

    return (buffer[0]);
}

void mpu6050_set_dmp_config_2(uint8_t config)
{
    esp32_i2c_write_byte(
        mpu6050_device_address,
        MPU6050_REGISTER_DMP_CFG_2,
        config);
}

float mpu6050_get_accel_res(uint8_t accel_scale)
{
    float accel_res = 0;

    switch (accel_scale)
    {
    case 0:
        accel_res = 2.0 / 32768.0;
        break;
    case 1:
        accel_res = 4.0 / 32768.0;
        break;
    case 2:
        accel_res = 8.0 / 32768.0;
        break;
    case 3:
        accel_res = 16.0 / 32768.0;
        break;
    }

    return (accel_res);
}

float mpu6050_get_gyro_res(uint8_t gyro_scale)
{
    float gyro_res = 0;

    switch (gyro_scale)
    {
    case 0:
        gyro_res = 250.0 / 32768.0;
        break;
    case 1:
        gyro_res = 500.0 / 32768.0;
        break;
    case 2:
        gyro_res = 1000.0 / 32768.0;
        break;
    case 3:
        gyro_res = 2000.0 / 32768.0;
        break;
    }

    return (gyro_res);
}

void mpu6050_calibrate(float *accel_bias_res, float *gyro_bias_res)
{
    int32_t accel_bias[3] = {0, 0, 0};
    int32_t gyro_bias[3] = {0, 0, 0};
    int32_t accel_bias_reg[3] = {0, 0, 0};
    uint16_t accel_temp[3] = {0, 0, 0};
    uint16_t gyro_temp[3] = {0, 0, 0};
    uint8_t mask_bit[3] = {0, 0, 0};
    uint32_t mask = 1uL;
    uint16_t gyro_sensitivity = 131;
    uint16_t accel_sensitivity = 16384;
    uint8_t tmp_data[12];
    uint16_t packet_count;

    mpu6050_reset();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    mpu6050_set_clock_source(MPU6050_CLOCK_PLL_XGYRO);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // Configure device for bias calculation:
    mpu6050_set_int_enabled(0);
    mpu6050_set_fifo_enabled(0);
    mpu6050_set_accel_fifo_enabled(0);
    mpu6050_set_z_gyro_fifo_enabled(0);
    mpu6050_set_y_gyro_fifo_enabled(0);
    mpu6050_set_x_gyro_fifo_enabled(0);
    mpu6050_set_temp_fifo_enabled(0);
    mpu6050_set_clock_source(MPU6050_CLOCK_INTERNAL);
    mpu6050_set_multi_master_enabled(0);
    mpu6050_set_fifo_enabled(0);
    mpu6050_set_i2c_master_mode_enabled(0);
    mpu6050_reset_sensors();
    vTaskDelay(15 / portTICK_PERIOD_MS);

    // Configure MPU6050 gyro and accelerometer for bias calculation:
    mpu6050_set_rate(0x00); // Set sample rate to 1 kHz.
    mpu6050_set_dlpf_mode(MPU6050_DLPF_BW_188);
    mpu6050_set_full_scale_accel_range(MPU6050_ACCEL_FULL_SCALE_RANGE_2);
    mpu6050_set_full_scale_gyro_range(MPU6050_GYRO_FULL_SCALE_RANGE_250);

    /**
     * Configure FIFO to capture data for bias calculation.
     */

    // Enable gyroscope and accelerometer sensors for FIFO:
    mpu6050_set_fifo_enabled(1);
    mpu6050_set_accel_fifo_enabled(1);
    mpu6050_set_z_gyro_fifo_enabled(1);
    mpu6050_set_y_gyro_fifo_enabled(1);
    mpu6050_set_x_gyro_fifo_enabled(1);
    vTaskDelay(80 / portTICK_PERIOD_MS); // Accumulate 80 samples in 80 ms.

    // At end of sample accumulation, turn off FIFO sensor read:
    mpu6050_set_fifo_enabled(0);
    mpu6050_set_accel_fifo_enabled(0);
    mpu6050_set_z_gyro_fifo_enabled(0);
    mpu6050_set_y_gyro_fifo_enabled(0);
    mpu6050_set_x_gyro_fifo_enabled(0);
    mpu6050_set_temp_fifo_enabled(0);

    // Sets of full gyro and accelerometer data for averaging:
    packet_count = mpu6050_get_fifo_count() / 12;

    for (int i = 0; i < packet_count; i++)
    {
        // Read data for averaging:
        mpu6050_get_fifo_bytes(&tmp_data[0], 6);
        accel_temp[0] = (int16_t)(((int16_t)tmp_data[0] << 8) | tmp_data[1]);
        accel_temp[1] = (int16_t)(((int16_t)tmp_data[2] << 8) | tmp_data[3]);
        accel_temp[2] = (int16_t)(((int16_t)tmp_data[4] << 8) | tmp_data[5]);
        gyro_temp[0] = (int16_t)(((int16_t)tmp_data[6] << 8) | tmp_data[7]);
        gyro_temp[1] = (int16_t)(((int16_t)tmp_data[8] << 8) | tmp_data[9]);
        gyro_temp[2] = (int16_t)(((int16_t)tmp_data[10] << 8) | tmp_data[11]);

        // Sum individual 16-bit biases to get accumulated signed 32-bit biases:
        accel_bias[0] += (int32_t)accel_temp[0];
        accel_bias[1] += (int32_t)accel_temp[1];
        accel_bias[2] += (int32_t)accel_temp[2];
        gyro_bias[0] += (int32_t)gyro_temp[0];
        gyro_bias[1] += (int32_t)gyro_temp[1];
        gyro_bias[2] += (int32_t)gyro_temp[2];
    }

    // Normalize sums to get average count biases:
    accel_bias[0] /= (int32_t)packet_count;
    accel_bias[1] /= (int32_t)packet_count;
    accel_bias[2] /= (int32_t)packet_count;
    gyro_bias[0] /= (int32_t)packet_count;
    gyro_bias[1] /= (int32_t)packet_count;
    gyro_bias[2] /= (int32_t)packet_count;

    // Remove gravity from the z-axis accelerometer bias calculation:
    if (accel_bias[2] > 0L)
        accel_bias[2] -= (int32_t)accel_sensitivity;
    else
        accel_bias[2] += (int32_t)accel_sensitivity;

    /**
     * Construct the gyro biases for push to the hardware gyro bias registers,
     * which are reset to zero upon device startup:
     */

    // Divide by 4 to get 32.9 LSB per deg/s to expected bias input format.
    // Biases are additive, so change sign on calculated average gyro biases.
    tmp_data[0] = (-gyro_bias[0] / 4 >> 8) & 0xFF;
    tmp_data[1] = (-gyro_bias[0] / 4) & 0xFF;
    tmp_data[2] = (-gyro_bias[1] / 4 >> 8) & 0xFF;
    tmp_data[3] = (-gyro_bias[1] / 4) & 0xFF;
    tmp_data[4] = (-gyro_bias[2] / 4 >> 8) & 0xFF;
    tmp_data[5] = (-gyro_bias[2] / 4) & 0xFF;

    // Push gyro biases to hardware registers:
    mpu6050_set_x_gyro_offset(((int16_t)tmp_data[0]) << 8 | tmp_data[1]);
    mpu6050_set_y_gyro_offset(((int16_t)tmp_data[2]) << 8 | tmp_data[3]);
    mpu6050_set_z_gyro_offset(((int16_t)tmp_data[4]) << 8 | tmp_data[5]);

    // Construct gyro bias in deg/s for later manual subtraction:
    gyro_bias_res[0] = (float)gyro_bias[0] / (float)gyro_sensitivity;
    gyro_bias_res[1] = (float)gyro_bias[1] / (float)gyro_sensitivity;
    gyro_bias_res[2] = (float)gyro_bias[2] / (float)gyro_sensitivity;

    /**
     * Construct the accelerometer biases for push to the hardware accelerometer
     * bias registers. These registers contain factory trim values which must be
     * added to the calculated accelerometer biases; on boot up these registers
     * will hold non-zero values. In addition, bit 0 of the lower byte must be
     * preserved since it is used for temperature compensation calculations.
     * Accelerometer bias registers expect bias input as 2048 LSB per g, so that
     * the accelerometer biases calculated above must be divided by 8.
     */

    // Read factory accelerometer trim values:
    tmp_data[0] = mpu6050_get_x_accel_offset();
    tmp_data[1] = mpu6050_get_y_accel_offset();
    tmp_data[2] = mpu6050_get_z_accel_offset();

    for (int i = 0; i < 3; i++)
    {
        // If temperature compensation bit is set, record that in mask_bit:
        if (accel_bias_reg[i] & mask)
            mask_bit[i] = 0x01;
    }

    /**
     * Construct total accelerometer bias, including calculated average
     * accelerometer bias from above (Subtract calculated averaged accelerometer
     * bias scaled to 2048 LSB/g (16g full scale).
     */

    accel_bias_reg[0] -= (accel_bias[0] / 8);
    accel_bias_reg[1] -= (accel_bias[1] / 8);
    accel_bias_reg[2] -= (accel_bias[2] / 8);

    tmp_data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
    tmp_data[1] = (accel_bias_reg[0]) & 0xFF;
    tmp_data[1] = tmp_data[1] | mask_bit[0];
    tmp_data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
    tmp_data[3] = (accel_bias_reg[1]) & 0xFF;
    tmp_data[3] = tmp_data[3] | mask_bit[1];
    tmp_data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
    tmp_data[5] = (accel_bias_reg[2]) & 0xFF;
    tmp_data[5] = tmp_data[5] | mask_bit[2];

    // Push accelerometer biases to hardware registers:
    mpu6050_set_x_accel_offset(((int16_t)tmp_data[0]) << 8 | tmp_data[1]);
    mpu6050_set_y_accel_offset(((int16_t)tmp_data[2]) << 8 | tmp_data[3]);
    mpu6050_set_z_accel_offset(((int16_t)tmp_data[4]) << 8 | tmp_data[5]);

    // Output scaled accelerometer biases for subtraction in the main program:
    accel_bias_res[0] = (float)accel_bias[0] / (float)accel_sensitivity;
    accel_bias_res[1] = (float)accel_bias[1] / (float)accel_sensitivity;
    accel_bias_res[2] = (float)accel_bias[2] / (float)accel_sensitivity;
}

void mpu6050_self_test(float *destination)
{
    uint8_t self_test[6];
    float factory_trim[6];

    // Configure the accelerometer for self-test:
    mpu6050_set_accel_x_self_test(true);
    mpu6050_set_accel_y_self_test(true);
    mpu6050_set_accel_z_self_test(true);
    mpu6050_set_full_scale_accel_range(MPU6050_ACCEL_FULL_SCALE_RANGE_8);
    mpu6050_set_full_scale_gyro_range(MPU6050_GYRO_FULL_SCALE_RANGE_250);

    self_test[0] = mpu6050_get_accel_x_self_test_factory_trim();
    self_test[1] = mpu6050_get_accel_y_self_test_factory_trim();
    self_test[2] = mpu6050_get_accel_z_self_test_factory_trim();
    self_test[3] = mpu6050_get_gyro_x_self_test_factory_trim();
    self_test[4] = mpu6050_get_gyro_y_self_test_factory_trim();
    self_test[5] = mpu6050_get_gyro_z_self_test_factory_trim();

    // Process results to allow final comparison with factory set values:
    factory_trim[0] = (4096.0f * 0.34f) * (pow((0.92f / 0.34f), ((self_test[0] - 1.0f) / 30.0f)));
    factory_trim[1] = (4096.0f * 0.34f) * (pow((0.92f / 0.34f), ((self_test[1] - 1.0f) / 30.0f)));
    factory_trim[2] = (4096.0f * 0.34f) * (pow((0.92f / 0.34f), ((self_test[2] - 1.0f) / 30.0f)));
    factory_trim[3] = (25.0f * 131.0f) * (pow(1.046f, (self_test[3] - 1.0f)));
    factory_trim[4] = (-25.0f * 131.0f) * (pow(1.046f, (self_test[4] - 1.0f)));
    factory_trim[5] = (25.0f * 131.0f) * (pow(1.046f, (self_test[5] - 1.0f)));

    // Report results as a ratio of "(STR - FT) / FT" (The change from Factory
    // Trim of the Self-Test Response).
    // To get to percent, must multiply by 100 and subtract result from 100.
    for (int i = 0; i < 6; i++)
        destination[i] = 100.0f + 100.0f * (self_test[i] - factory_trim[i]) / factory_trim[i];
}

void mpu6050_madgwick_quaternion_update(
    float accel_x,
    float accel_y,
    float accel_z,
    float gyro_x,
    float gyro_y,
    float gyro_z)
{
    float func_1, func_2, func_3;
    float j_11o24, j_12o23, j_13o22, j_14o21, j_32, j_33;
    float q_dot_1, q_dot_2, q_dot_3, q_dot_4;
    float hat_dot_1, hat_dot_2, hat_dot_3, hat_dot_4;
    float gyro_x_err, gyro_y_err, gyro_z_err;
    float gyro_x_bias, gyro_y_bias, gyro_z_bias;
    float norm;

    float half_q1 = 0.5f * quart[0];
    float half_q2 = 0.5f * quart[1];
    float half_q3 = 0.5f * quart[2];
    float half_q4 = 0.5f * quart[3];
    float double_q1 = 2.0f * quart[0];
    float double_q2 = 2.0f * quart[1];
    float double_q3 = 2.0f * quart[2];
    float double_q4 = 2.0f * quart[3];

    // Normalise accelerometer measurement:
    norm = sqrt(accel_x * accel_x + accel_y * accel_y + accel_z * accel_z);

    // Handle NaN:
    if (norm == 0.0f)
        return;

    norm = 1.0f / norm;
    accel_x *= norm;
    accel_y *= norm;
    accel_z *= norm;

    // Compute the objective function and Jacobian:
    func_1 = double_q2 * quart[3] - double_q1 * quart[2] - accel_x;
    func_2 = double_q1 * quart[1] + double_q3 * quart[3] - accel_y;
    func_3 = 1.0f - double_q2 * quart[3] - double_q3 * quart[2] - accel_z;
    j_11o24 = double_q3;
    j_12o23 = double_q4;
    j_13o22 = double_q1;
    j_14o21 = double_q2;
    j_32 = 2.0f * j_14o21;
    j_33 = 2.0f * j_11o24;

    // Compute the gradient (matrix multiplication):
    hat_dot_1 = j_14o21 * func_2 - j_11o24 * func_1;
    hat_dot_2 = j_12o23 * func_1 + j_13o22 * func_2 - j_32 * func_3;
    hat_dot_3 = j_12o23 * func_2 - j_33 * func_3 - j_13o22 * func_1;
    hat_dot_4 = j_14o21 * func_1 + j_11o24 * func_2;

    // Normalize the gradient:
    norm = sqrt(hat_dot_1 * hat_dot_1 + hat_dot_2 * hat_dot_2 + hat_dot_3 * hat_dot_3 + hat_dot_4 * hat_dot_4);
    hat_dot_1 /= norm;
    hat_dot_2 /= norm;
    hat_dot_3 /= norm;
    hat_dot_4 /= norm;

    // Compute estimated gyroscope biases:
    gyro_x_err = double_q1 * hat_dot_2 - double_q2 * hat_dot_1 - double_q3 * hat_dot_4 + double_q4 * hat_dot_3;
    gyro_y_err = double_q1 * hat_dot_3 + double_q2 * hat_dot_4 - double_q3 * hat_dot_1 - double_q4 * hat_dot_2;
    gyro_z_err = double_q1 * hat_dot_4 - double_q2 * hat_dot_3 + double_q3 * hat_dot_2 - double_q4 * hat_dot_1;

    // Compute and remove gyroscope biases:
    gyro_x_bias += gyro_x_err * delta_t * ZETA;
    gyro_y_bias += gyro_y_err * delta_t * ZETA;
    gyro_z_bias += gyro_z_err * delta_t * ZETA;

    // Compute the quaternion derivative:
    q_dot_1 = -half_q2 * gyro_x - half_q3 * gyro_y - half_q4 * gyro_z;
    q_dot_2 = half_q1 * gyro_x + half_q3 * gyro_z - half_q4 * gyro_y;
    q_dot_3 = half_q1 * gyro_y - half_q2 * gyro_z + half_q4 * gyro_x;
    q_dot_4 = half_q1 * gyro_z + half_q2 * gyro_y - half_q3 * gyro_x;

    // Compute then integrate estimated quaternion derivative:
    quart[0] += (q_dot_1 - (BETA * hat_dot_1)) * delta_t;
    quart[1] += (q_dot_2 - (BETA * hat_dot_2)) * delta_t;
    quart[2] += (q_dot_3 - (BETA * hat_dot_3)) * delta_t;
    quart[3] += (q_dot_4 - (BETA * hat_dot_4)) * delta_t;

    // Normalize the quaternion:
    norm = sqrt(quart[0] * quart[0] + quart[1] * quart[1] + quart[2] * quart[2] + quart[3] * quart[3]);
    norm = 1.0f / norm;
    quart[0] *= norm;
    quart[1] *= norm;
    quart[2] *= norm;
    quart[3] *= norm;

    // pitch = asin(2*q1q3 - 2*q0q2)*57.30f;
	// 	roll = fast_atan2(2*q2q3 + 2*q0q1, -2*q1q1-2*q2q2 + 1)*57.30f; 
	// 	yaw = -fast_atan2(2*q1q2 + 2*q0q3, -2*q2q2-2*q3q3 + 1)*57.30f; 
}
void LPF_1(float hz,float time,float in,float *out)  
{
	*out += ( 1 / ( 1 + 1 / ( hz *6.28f *time ) ) ) *( in - *out );
}
void limit_filter(float T,float hz,_lf_t *data,float in)
{
	float abs_t;
	LPF_1(hz,T,	 in,&(data->lpf_1)); 
	abs_t = ABS(data->lpf_1);
	data->out = LIMIT(in,-abs_t,abs_t);

}


_imu_st imu_data = {1.0, 0.0, 0.0, 0.0, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 0, 0, 0};

void IMU_update(float dT, _xyz_f_st *gyr, _xyz_f_st *acc, _imu_st *imu)
{
	static _lf_t err_lf_x;
	static _lf_t err_lf_y;
	static _lf_t err_lf_z;
	static _xyz_f_st vec_err_i;

	float kp = 0.8f, ki = 0;

	float q0q1, q0q2, q1q1, q1q3, q2q2, q2q3, q3q3, q1q2, q0q3; //q0q0,
	float w_q=0, x_q=0, y_q=0, z_q=0;
	float acc_length=0, q_length=0;
	_xyz_f_st acc_norm;
	_xyz_f_st vec_err;
	_xyz_f_st d_angle;

	w_q = imu->w;
	x_q = imu->x;
	y_q = imu->y;
	z_q = imu->z;

	q0q1 = w_q * x_q;
	q0q2 = w_q * y_q;
	q1q1 = x_q * x_q;
	q1q3 = x_q * z_q;
	q2q2 = y_q * y_q;
	q2q3 = y_q * z_q;
	q3q3 = z_q * z_q;
	q1q2 = x_q * y_q;
	q0q3 = w_q * z_q;

	// 加速度计的读数，单位化。
	acc_length = sqrt(my_pow(acc->x) + my_pow(acc->y) + my_pow(acc->z));
	acc_norm.x = acc->x / acc_length;
	acc_norm.y = acc->y / acc_length;
	acc_norm.z = acc->z / acc_length;

	// 载体坐标下的x方向向量，单位化。
	imu->x_vec.x = 1 - (2 * q2q2 + 2 * q3q3);
	imu->x_vec.y = 2 * q1q2 - 2 * q0q3;
	imu->x_vec.z = 2 * q1q3 + 2 * q0q2;

	// 载体坐标下的y方向向量，单位化。
	imu->y_vec.x = 2 * q1q2 + 2 * q0q3;
	imu->y_vec.y = 1 - (2 * q1q1 + 2 * q3q3);
	imu->y_vec.z = 2 * q2q3 - 2 * q0q1;

	// 载体坐标下的z方向向量（等效重力向量、重力加速度向量），单位化。
	imu->z_vec.x = 2 * q1q3 - 2 * q0q2;
	imu->z_vec.y = 2 * q2q3 + 2 * q0q1;
	imu->z_vec.z = 1 - (2 * q1q1 + 2 * q2q2);

	// 计算载体坐标下的运动加速度。(与姿态解算无关)
	imu->a_acc.x = acc->x - 9800 * imu->z_vec.x;
	imu->a_acc.y = acc->y - 9800 * imu->z_vec.y;
	imu->a_acc.z = acc->z - 9800 * imu->z_vec.z;

	// 计算世界坐标下的运动加速度。(与姿态解算无关)
	imu->w_acc.x = imu->x_vec.x * imu->a_acc.x + imu->x_vec.y * imu->a_acc.y;
	imu->w_acc.y = imu->y_vec.x * imu->a_acc.x + imu->y_vec.y * imu->a_acc.y;
	imu->w_acc.z = imu->z_vec.x * imu->a_acc.x + imu->z_vec.y * imu->a_acc.y + imu->z_vec.z * imu->a_acc.z;

	// 测量值与等效重力向量的叉积（计算向量误差）。
	vec_err.x = (acc_norm.y * imu->z_vec.z - imu->z_vec.y * acc_norm.z);
	vec_err.y = -(acc_norm.x * imu->z_vec.z - imu->z_vec.x * acc_norm.z);
	vec_err.z = 0;

	//截止频率1hz的低通限幅滤波
	limit_filter(dT, 0.2f, &err_lf_x, vec_err.x);
	limit_filter(dT, 0.2f, &err_lf_y, vec_err.y);
	limit_filter(dT, 0.2f, &err_lf_z, vec_err.z);

	//误差积分
	vec_err_i.x += err_lf_x.out * dT * ki;
	vec_err_i.y += err_lf_y.out * dT * ki;
	vec_err_i.z += err_lf_z.out * dT * ki;

	// 构造增量旋转（含融合纠正）。
	d_angle.x = (gyr->x * RAD_PER_DEG + (err_lf_x.out + vec_err_i.x) * kp) * dT / 2;
	d_angle.y = (gyr->y * RAD_PER_DEG + (err_lf_y.out + vec_err_i.y) * kp) * dT / 2;
	d_angle.z = (gyr->z * RAD_PER_DEG + (err_lf_z.out + vec_err_i.z) * kp) * dT / 2;

	// 计算姿态。
	imu->w = w_q - x_q * d_angle.x - y_q * d_angle.y - z_q * d_angle.z;
	imu->x = w_q * d_angle.x + x_q + y_q * d_angle.z - z_q * d_angle.y;
	imu->y = w_q * d_angle.y - x_q * d_angle.z + y_q + z_q * d_angle.x;
	imu->z = w_q * d_angle.z + x_q * d_angle.y - y_q * d_angle.x + z_q;

	q_length = sqrt(imu->w * imu->w + imu->x * imu->x + imu->y * imu->y + imu->z * imu->z);
	imu->w /= q_length;
	imu->x /= q_length;
	imu->y /= q_length;
	imu->z /= q_length;

	imu->pit = asin(2 * q1q3 - 2 * q0q2) * 57.30f;
	imu->rol = atan2(2 * q2q3 + 2 * q0q1, -2 * q1q1 - 2 * q2q2 + 1) * 57.30f;
	imu->yaw = -atan2(2 * q1q2 + 2 * q0q3, -2 * q2q2 - 2 * q3q3 + 1) * 57.30f;
}

