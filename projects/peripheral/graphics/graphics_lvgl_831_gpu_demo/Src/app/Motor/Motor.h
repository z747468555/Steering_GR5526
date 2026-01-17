#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "board_SK.h"

#define MOTOR_PIN           APP_IO_PIN_10
#define MOTOR_TYPE          APP_IO_TYPE_GPIOA
#define Motor_Start()       app_io_write_pin(MOTOR_TYPE, MOTOR_PIN, APP_IO_PIN_SET)
#define Motor_Stop()        app_io_write_pin(MOTOR_TYPE, MOTOR_PIN, APP_IO_PIN_RESET)
void Motor_Init(void);
long Motor_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString);
#endif


