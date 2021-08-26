#include "bsp_i2c.h"

/*
===========================
函数定义
=========================== 
*/


/** 
 * bsp_i2c 初始化
 * @param[in]   NULL
 * @retval      
 *              NULL                              
 * @par         修改日志 
 *               Ver0.0.1:
                     hans, 2019/09/08, 初始化版本\n 
 *               Ver0.0.2:
                     hans, 2019/09/08, 统一编程风格\n 
 */
esp_err_t bsp_i2c_master_init(void)
{
    //i2c配置结构体
    // i2c_config_t conf;
    // conf.mode = I2C_MODE_MASTER;                        //I2C模式
    // conf.sda_io_num = I2C_MASTER_SDA;                   //SDA IO映射
    // conf.sda_pullup_en = GPIO_PULLUP_ENABLE;            //SDA IO模式
    // conf.scl_io_num = I2C_MASTER_SCL;                   //SCL IO映射
    // conf.scl_pullup_en = GPIO_PULLUP_ENABLE;            //SCL IO模式
    // conf.master.clk_speed = I2C_MASTER_FREQ_HZ;         //I2C CLK频率
    // i2c_param_config(I2C_MASTER_NUM, &conf);            //设置I2C
    // //注册I2C服务即使能
    // return i2c_driver_install(I2C_MASTER_NUM, conf.mode,0,0,0);

	    i2c_config_t conf= {
        .mode = I2C_MODE_MASTER
       ,.sda_io_num = 21
       ,.sda_pullup_en = GPIO_PULLUP_ENABLE
       ,.scl_io_num = 19
       ,.scl_pullup_en = GPIO_PULLUP_ENABLE
       ,.master.clk_speed = 100000
    };

    i2c_param_config(0, &conf);
    return i2c_driver_install(0, conf.mode, 0, 0, 0);
}
esp_err_t bsp_i2c_master_deinit(void)
{
    return i2c_driver_delete(I2C_MASTER_NUM);
}


/** 
 * 向oled写命令
 * @param[in]   command
 * @retval      
 *              - ESP_OK                              
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 *               Ver0.0.2:
                     hx-zsj, 2018/08/07, 统一编程风格\n 
 */

int bsp_oled_write_cmd(uint8_t addr,uint8_t command)
{
    int ret;
    //配置oled的寄存器
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();                                   //新建操作I2C句柄
    ret = i2c_master_start(cmd);                                                    //启动I2C
    ret = i2c_master_write_byte(cmd, addr |WRITE_BIT , ACK_CHECK_EN);               //发地址+写+检查ack
    ret = i2c_master_write_byte(cmd, WRITE_CMD, ACK_CHECK_EN);                      //发数据高8位+检查ack
    ret = i2c_master_write_byte(cmd,command, ACK_CHECK_EN);                         //发数据低8位+检查ack
    ret = i2c_master_stop(cmd);                                                     //停止I2C
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 100 / portTICK_RATE_MS);        //I2C发送
    i2c_cmd_link_delete(cmd);                                                       //删除I2C句柄
    if (ret != ESP_OK) 
    {
        return ret;
    }
    return ret;
}

/** 
 * 向oled写数据
 * @param[in]   data
 * @retval      
 *              - ESP_OK                              
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 */
int bsp_oled_write_data(uint8_t addr,uint8_t data)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret = i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, addr | WRITE_BIT, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, WRITE_DATA, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
    ret = i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 100 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) 
    {
        return ret;
    }
    return ret;
}

/** 
 * 向oled写长数据
 * @param[in]   data   要写入的数据
 * @param[in]   len     数据长度
 * @retval      
 *              - ESP_OK                              
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 */
int bsp_oled_write_long_data(uint8_t addr,uint8_t *data,uint16_t len)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret = i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, addr | WRITE_BIT, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, WRITE_DATA, ACK_CHECK_EN);
    ret = i2c_master_write(cmd, data, len,ACK_CHECK_EN);
    ret = i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) 
    {
        return ret;
    }
    return ret;    
}



void select_register(uint8_t device_address, uint8_t register_address)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_address << 1) | I2C_MASTER_WRITE, 1);
    i2c_master_write_byte(cmd, register_address, 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

int8_t esp32_i2c_read_bytes
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t size,
    uint8_t* data
)
{
    select_register(device_address, register_address);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_address << 1) | I2C_MASTER_READ, 1);

    if (size > 1)
        i2c_master_read(cmd, data, size - 1, 0);

    i2c_master_read_byte(cmd, data + size - 1, 1);

    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return (size);
}

int8_t esp32_i2c_read_byte
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t* data
)
{
    return (esp32_i2c_read_bytes(device_address, register_address, 1, data));
}

int8_t esp32_i2c_read_bits
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t bit_start,
    uint8_t size,
    uint8_t* data
)
{
    uint8_t bit;
    uint8_t count;

    if ((count = esp32_i2c_read_byte(device_address, register_address, &bit))) {
        uint8_t mask = ((1 << size) - 1) << (bit_start - size + 1);

        bit &= mask;
        bit >>= (bit_start - size + 1);
        *data = bit;
    }

    return (count);
}

int8_t esp32_i2c_read_bit
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t bit_number,
    uint8_t* data
)
{
    uint8_t bit;
    uint8_t count = esp32_i2c_read_byte(device_address, register_address, &bit);

    *data = bit & (1 << bit_number);

    return (count);
}

bool esp32_i2c_write_bytes
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t size,
    uint8_t* data
)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_address << 1) | I2C_MASTER_WRITE, 1);
    i2c_master_write_byte(cmd, register_address, 1);
    i2c_master_write(cmd, data, size - 1, 0);
    i2c_master_write_byte(cmd, data [size - 1], 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return (true);
}

bool esp32_i2c_write_byte
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t data
)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_address << 1) | I2C_MASTER_WRITE, 1);
    i2c_master_write_byte(cmd, register_address, 1);
    i2c_master_write_byte(cmd, data, 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return (true);
}

bool esp32_i2c_write_bits
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t bit_start,
    uint8_t size,
    uint8_t data
)
{
    uint8_t bit = 0;

    if (esp32_i2c_read_byte(device_address, register_address, &bit) != 0) {
        uint8_t mask = ((1 << size) - 1) << (bit_start - size + 1);
        data <<= (bit_start - size + 1);
        data &= mask;
        bit &= ~(mask);
        bit |= data;
        return (esp32_i2c_write_byte(device_address, register_address, bit));
    }
    else
        return (false);
}

bool esp32_i2c_write_bit
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t bit_number,
    uint8_t data
)
{
    uint8_t bit;

    esp32_i2c_read_byte(device_address, register_address, &bit);

    if (data != 0)
        bit = (bit | (1 << bit_number));
    else
        bit = (bit & ~(1 << bit_number));

    return (esp32_i2c_write_byte(device_address, register_address, bit));
}

int8_t esp32_i2c_write_word
(
    uint8_t device_address,
    uint8_t register_address,
    uint8_t data
)
{
    uint8_t data_1[] = {(uint8_t) (data >> 8), (uint8_t) (data & 0xFF)};

    esp32_i2c_write_bytes(device_address, register_address, 2, data_1);

    return (1);
}
