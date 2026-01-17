#ifndef __HP5804_H_
#define __HP5804_H_

#include "stdint.h"

#define HP_READ_PT			0x10
#define HP_READ_AT			0x11
#define HP_READ_A				0x31
#define HP_READ_T				0x32
#define HP_READ_P				0x30
#define HP_IIC_ID				APP_I2C_ID_3
#define HP5804_ADDRESS			0x76//		0xEC>>1
#define HP_P_4096           0x40            //测量压强, 采样率4096
#define HP_T_4096           0x50            //测量温度, 采样率4096

extern float Temp, Pres, Alti;	//温度, 压力, 海拔
extern volatile uint8_t I2C1_tdone;
extern volatile uint8_t I2C1_rdone;


float HP_Read_Pres(void);

float HP_Read_Temp(void);

long HP_Read_Pres_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString);

void HP_enter_lowper(void);

#endif


