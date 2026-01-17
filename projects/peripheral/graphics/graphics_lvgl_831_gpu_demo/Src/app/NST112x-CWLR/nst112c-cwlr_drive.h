#ifndef __NST112x_drive_H_
#define __NST112x_drive_H_
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define NST112_IIC_ADD		0x4B
#define NST112_IIC_ID		APP_I2C_ID_3

typedef struct {
    uint8_t dev_addr;  
    bool shutdown;     
    uint8_t rate;      
    uint8_t polarity;  
    uint8_t fault;    
    uint16_t high;     
    uint16_t low;     
} sensor_cfg_t;


extern sensor_cfg_t sensor_cfg;



bool temp_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len);
bool temp_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len);
/*****
 * 功能：温度传感器初始化
 * 参数：
 *  cfg: 温度传感器句柄
 * *****/
void temp_sensor_init(void);
/**
 * 从传感器读取温度值
 * @param cfg 指向传感器配置结构体的指针，包含设备地址等配置信息
 * @return 返回读取到的温度值，单位为整数度（可能需要根据传感器规格进行转换）
 */
float temp_sensor_read_temp(sensor_cfg_t *cfg);
long Temp_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString);
#endif //

