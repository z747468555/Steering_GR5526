
#include "nst112c-cwlr_drive.h"
#include "IIC.h"
#include "app_log.h"
//#define SENSOR_ADDRESS 0x4B // 温度传感器的I2C地址，请根据传感器具体地址进行修改
#define CONFIG_REG     0x01 // 配置寄存器的地址，需要根据传感器手册确定
#define TEMP_REG_MSB   0x03 // 温度数据高字节寄存器的地址，需要根据传感器手册确定
#define TEMP_REG_LSB   0x02 // 温度数据低字节寄存器的地址，需要根据传感器手册确定


sensor_cfg_t sensor_cfg = {
	.dev_addr = NST112_IIC_ADD,
    .shutdown = false,
    .rate = 2,        
    .polarity = 1,   
    .fault = 1,       
    .high = 80,       
    .low = 75        
};


// 定义I2C接口读写函数声明
bool temp_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len)
{
	uint8_t wdata[50];
	wdata[0] = reg_addr;
	memcpy(wdata + 1, data, len);
	IIC_Write_sync(NST112_IIC_ID, dev_addr, wdata, len + 1);
  return 0;
}
bool temp_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len)
{
  IIC_Write_sync(NST112_IIC_ID, dev_addr, &reg_addr, 1);
  IIC_Read_sync(NST112_IIC_ID, dev_addr, data, len);
  return 0;
}



/**
 * 初始化温度传感器
 * 该函数用于对温度传感器进行初始化配置。这包括但不限于设置传感器的工作模式、
 * 测量精度、采样率等。具体配置步骤应根据所使用的温度传感器的 datasheet 或手册进行。
 * 
 * 参数: 无
 * 返回值: 无
 *void init_temperature_sensor() {
    
*   }
*or
*/
// 温度传感器初始化函数
void temp_sensor_init() {
		uint16_t ret;
    // 写入配置寄存器
    uint8_t reg[2] = {0};
    //reg[0] = (sensor_cfg.shutdown << 8) | (sensor_cfg.rate << 6);
		reg[0] = 0x64;
    //reg[1] = (sensor_cfg.polarity << 10) | (sensor_cfg.fault << 8);
		reg[1] = 0xA0;
    ret = temp_i2c_write(sensor_cfg.dev_addr, 0x01, reg, 2);

    // 写入高低温限制寄存器
    uint8_t high[2] = {0};
    uint8_t low[2] = {0};
    high[0] = (sensor_cfg.high >> 8) & 0xFF;
    high[1] = sensor_cfg.high & 0xFF;
    low[0] = (sensor_cfg.low >> 8) & 0xFF;
    low[1] = sensor_cfg.low & 0xFF;
    ret = temp_i2c_write(sensor_cfg.dev_addr, 0x03, high, 2);
	if(ret)
	{
		APP_LOG_WARNING("Temp:Init failed\r\n");
	}
    ret = temp_i2c_write(sensor_cfg.dev_addr, 0x04, low, 2);
	if(ret)
	{
		APP_LOG_WARNING("Temp:Init failed\r\n");
	}
	else
	{
		APP_LOG_INFO("Temp:Init successed\r\n");
	}
}

/**
 * 从传感器读取温度值
 * @param cfg 指向传感器配置结构体的指针，包含设备地址等配置信息
 * @return 返回读取到的温度值
 */
float temp_sensor_read_temp(sensor_cfg_t *cfg) 
{
    uint8_t data[2] = {0}; // 初始化一个用于存储温度数据的数组
    float ret;
    temp_i2c_read(cfg->dev_addr, 0x00, data, 2); // 通过I2C协议从传感器读取2字节温度数据
    int16_t temp = (data[0] << 8) | data[1]; // 将读取到的两个字节数据合并为一个16位的温度值
    temp >>= 4;                 //默认是12bit模式
    ret = temp * 0.0625;
    APP_LOG_INFO("nst Temp:%f ", ret);
    return ret; // 返回温度值
}

long Temp_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString)
{
	float temp;
	temp = temp_sensor_read_temp(&sensor_cfg);
	APP_LOG_INFO("Temp:%d.%2d\r\n", (int)temp, (int)((temp - (int)temp) * 100));
	return 0;
}
