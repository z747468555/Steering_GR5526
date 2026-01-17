#ifndef __IIC_H_
#define __IIC_H_

#include "app_i2c.h"


#define IIC_ID                          APP_I2C_ID_1
#define IIC_SCL_PIN_TYPE				APP_IO_TYPE_GPIOB
#define IIC_SDA_PIN_TYPE				APP_IO_TYPE_GPIOB
#define IIC_SCL_PIN_NUM					APP_IO_PIN_14				//GPIO30
#define IIC_SDA_PIN_NUM					APP_IO_PIN_15				//GPIO31
#define IIC_SCL_PIN_MUX					APP_IO_MUX_3
#define IIC_SDA_PIN_MUX					APP_IO_MUX_3

#define IIC3_SCL_PIN_TYPE				    APP_IO_TYPE_GPIOA
#define IIC3_SDA_PIN_TYPE				    APP_IO_TYPE_GPIOA
#define IIC3_SCL_PIN_NUM					APP_IO_PIN_6				//GPIO6
#define IIC3_SDA_PIN_NUM					APP_IO_PIN_7				//GPIO7
#define IIC3_SCL_PIN_MUX					APP_IO_MUX_0
#define IIC3_SDA_PIN_MUX					APP_IO_MUX_0

#define IIC5_SCL_PIN_TYPE				    APP_IO_TYPE_GPIOA
#define IIC5_SDA_PIN_TYPE				    APP_IO_TYPE_GPIOA
#define IIC5_SCL_PIN_NUM					APP_IO_PIN_14				//GPIO14
#define IIC5_SDA_PIN_NUM					APP_IO_PIN_15				//GPIO15
#define IIC5_SCL_PIN_MUX					APP_IO_MUX_1
#define IIC5_SDA_PIN_MUX					APP_IO_MUX_1
void IIC_Init(void);
//add应传入7位地址，使用中断方式对IIC进行读写，注意不要在中断函数中调用
/********
 * ID: 使用的IIC的ID，对于每个外设都有其对应的宏定义，比如触摸屏的宏定义位TAP_ID
 * add: IIC设备的地址，注意是七位的，每个外设也有对应的宏定义，注意看.h文件
 * pdata: 数据数组的指针
 * size: 传输数据的长度
 * **********/
uint16_t IIC_Write(uint8_t ID, uint16_t add, uint8_t *pdata, uint16_t size);
uint16_t IIC_Read(uint8_t ID, uint16_t add, uint8_t *pdata, uint16_t size);
//在中断函数中使用这个函数
uint16_t IIC_Write_sync(uint8_t ID, uint16_t add, uint8_t *pdata, uint16_t size);
uint16_t IIC_Read_sync(uint8_t ID, uint16_t add, uint8_t *pdata, uint16_t size);

uint16_t IIC_Write_sync_tp(uint8_t ID, uint16_t add, uint8_t *pdata, uint16_t size);

uint16_t IIC_Read_sync_tp(uint8_t ID, uint16_t add, uint8_t *pdata, uint16_t size);

void iic_mutex_create(void);


extern volatile uint8_t I2C1_tdone;
extern volatile uint8_t I2C1_rdone;
extern volatile uint8_t I2C3_tdone;
extern volatile uint8_t I2C3_rdone;
extern volatile uint8_t I2C5_tdone;
extern volatile uint8_t I2C5_rdone;

#endif //  __IIC_H

