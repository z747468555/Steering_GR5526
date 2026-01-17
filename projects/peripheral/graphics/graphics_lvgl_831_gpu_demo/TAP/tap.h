#ifndef _TAP_H_
#define _TAP_H_

#include "board_SK.h"

#define TAP_IIC     APP_I2C_ID_1

#define TAP_VERSION	1			//1指第二版触摸屏，0指第一版触摸屏
#if TAP_VERSION
#define TAP_ADD     0x70 >> 1
#else
#define TAP_ADD			0x15
#endif

#define TAP_RST_IO_TYPE		    APP_IO_TYPE_GPIOB
#define TAP_RST_IO_PIN          APP_IO_PIN_13
#define TAP_INT_IO_TYPE         APP_IO_TYPE_GPIOC
#define TAP_INT_IO_PIN          APP_IO_PIN_0
#define POSITION_ADD						0x03
#define STATUS_ADD							0x02
void TAP_Test(void);
void TAP_Init(void(* _irq_notify)(void));
bool Tap_Read_Pointer(int16_t  * x, int16_t * y);
void Tap_Deinit(void);
#endif

