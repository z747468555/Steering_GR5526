#ifndef __LED_H_
#define __LED_H_
#include "board_SK.h"

/*******LED IO CONFIG FOR SK*********************/
#define APP_LED_BAT_IO                      APP_IO_PIN_3
#define APP_LED_CHG_IO                      APP_IO_PIN_5

#define LED_BAT     BSP_LED_NUM_0
#define LED_CHG     BSP_LED_NUM_1

void LED_Close(bsp_led_num_t led_num);
void LED_Open(bsp_led_num_t led_num);
void LED_Init(void);
long LED_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString);
#endif // !__LED_H_
