#ifndef __CHARGE_H__
#define __CHARGE_H__
#include "board_SK.h"

#define CHARGE_IIC_ID       APP_I2C_ID_3
#define CHARGE_IIC_ADD      0x6B
#define REG02               0x02

#define CHARGE_INT_IO_PIN   APP_IO_PIN_4
#define CHARGE_INT_IO_TYPE  APP_IO_TYPE_GPIOA
#define CHARGE_INT_IO_MUX   APP_IO_MUX_7
/***
 * 功能：设置充电电流大小
 * 参数：
 *      mA：电流大小，mA为单位
 * 
 * ***/
void Set_Charge_Cur(uint16_t mA);
void Charge_Init(void);
uint8_t Get_Charge_Status(void);
void Get_Charge_fault(void);
void Get_Charge_Num(void);
void Enter_Ship_Mode(void);
long Charge_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString);
uint8_t charge_get_power(void);
void Charge_stat_set(bool sta);
#endif


