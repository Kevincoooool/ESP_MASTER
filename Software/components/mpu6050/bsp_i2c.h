#ifndef APP_I2C_H
#define APP_I2C_H 

#include <stdio.h>
#include "esp_system.h"
#include <esp_err.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "driver/i2c.h"


/*
===========================
宏定义
=========================== 
*/
#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

// //I2C
// #define I2C_SCL_IO       	32									//SCL->IO33
// #define I2C_SDA_IO         	33              					//SDA->IO32

// #define I2C_SCL_IO       	25									//SCL->IO33
// #define I2C_SDA_IO         	26              					//SDA->IO32

// #define I2C_SDA_IO         	12              					//SDA->IO32
// #define I2C_SCL_IO       	14									//SCL->IO33

#define I2C_MASTER_SCL       		19					//SCL->IO17
#define I2C_MASTER_SDA         		21              		//SDA->IO16
#define I2C_MASTER_NUM I2C_NUMBER(0) /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 		100000        /*!< I2C master clock frequency */

#define WRITE_BIT           I2C_MASTER_WRITE 					/*!< I2C master write */
#define READ_BIT            I2C_MASTER_READ  					/*!< I2C master read */
#define ACK_CHECK_EN        0x1              					/*!< I2C master will check ack from slave主机检查从机的ACK*/
#define ACK_CHECK_DIS       0x0              					/*!< I2C master will not check ack from slave 主机不检查从机的ACK*/
#define ACK_VAL             0x0              					/*!< I2C ack value 应答*/
#define NACK_VAL            0x1              					/*!< I2C nack value 不应答*/
#define WRITE_CMD      			0X00
#define WRITE_DATA     			0X40

// i2c 初始化
esp_err_t bsp_i2c_master_init(void);
esp_err_t bsp_i2c_master_deinit(void);
int bsp_oled_write_cmd(uint8_t addr,uint8_t command);
int bsp_oled_write_data(uint8_t addr,uint8_t data);
int bsp_oled_write_long_data(uint8_t addr,uint8_t *data,uint16_t len);

/**
 * @brief Select the register in the device where data will be read from.
 *
 * @param device_address I2C slave device address.
 * @param register_address Address of the first register to read from.
 */
void select_register(uint8_t device_address, uint8_t register_address);

/**
 * @brief Read multiple bytes from 8-bit registers.
 *
 * @param device_address I2C slave device address.
 * @param register_address Address of the first register to read from.
 * @param size Number of registers to read.
 * @param data Buffer to store the read data in.
 * 
 * @return Status of read operation.
 */
int8_t esp32_i2c_read_bytes
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t size,
    uint8_t* data
);

/**
 * @brief Read single byte from an 8-bit register.
 *
 * @param device_address I2C slave device address.
 * @param register_address Address of the register to read from.
 * @param data Container to store the byte read from register.
 * 
 * @return Status of read operation.
 */
int8_t esp32_i2c_read_byte
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t* data
);

/**
 * @brief Read multiple bits from an 8-bit register.
 *
 * @param device_address I2C slave device address.
 * @param register_address Address of the register to read from.
 * @param bit_start First bit position to read (0-7).
 * @param size Number of bits to read (Not more than 8).
 * @param data Container to store the right-aligned value.
 * 
 * @return Status of read operation.
 */
int8_t esp32_i2c_read_bits
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t bit_start,
    uint8_t size,
    uint8_t* data
);

/**
 * @brief Read single bit from an 8-bit register.
 *
 * @param device_address I2C slave device address.
 * @param register_address Address of the register to read from.
 * @param bit_number Bit position to read (0-7).
 * @param data Container to store the bit.
 * 
 * @return Status of read operation.
 */
int8_t esp32_i2c_read_bit
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t bit_number,
    uint8_t* data
);

/**
 * @brief Write multiple bytes to 8-bit registers.
 *
 * @param device_address I2C slave device address.
 * @param register_address Address of the first register to write to.
 * @param size Number of bytes to write.
 * @param data Array of bytes to write.
 * 
 * @return Status of write operation.
 */
bool esp32_i2c_write_bytes
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t size,
    uint8_t* data
);

/**
 * @brief Write single byte to an 8-bit register.
 *
 * @param device_address I2C slave device address.
 * @param register_address Address of the register to write to.
 * @param dat: Array of bytes to write.
 * 
 * @return Status of write operation.
 */
bool esp32_i2c_write_byte
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t data
);

/**
 * @brief Write multiple bits to an 8-bit register.
 *
 * @param device_address I2C slave device address.
 * @param register_address Address of the register to write to.
 * @param bit_star: First bit position to write (0-7).
 * @param size Number of bits to write (Not more than 8).
 * @param data Right-aligned value to write.
 * 
 * @return Status of write operation.
 */
bool esp32_i2c_write_bits
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t bit_start,
    uint8_t size,
    uint8_t data
);

/**
 * @brief Write single bit to an 8-bit register.
 *
 * @param device_address I2C slave device address.
 * @param register_address Address of the register to write to.
 * @param bit_number Bit position to write (0-7).
 * @param data Bit value to write.
 * 
 * @return Status of write operation.
 */
bool esp32_i2c_write_bit
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t bit_number,
    uint8_t data
);

/**
 * @brief Write word to an 8-bit register.
 *
 * @param device_address I2C slave device address.
 * @param register_address Address of the register to write to.
 * @param data Word to write.
 * 
 * @return Status of write operation.
 */
int8_t esp32_i2c_write_word
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t data
);
#endif