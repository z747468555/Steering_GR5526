#ifndef __LIGHT_H_
#define __LIGHT_H_

#include "board_SK.h"


/*******BATTERY ADC IO CONFIG***********************/
#define APP_ADC_BAT_INPUT_PIN               APP_IO_PIN_2
#define APP_ADC_BAT_INPUT_PIN_MUX           APP_IO_MUX_7
#define APP_ADC_BAT_INPUT_SRC               ADC_INPUT_SRC_IO2

/*******LIGHT ADC IO CONFIG***********************/
#define APP_ADC_LIT_INPUT_PIN               APP_IO_PIN_3
#define APP_ADC_LIT_INPUT_PIN_MUX           APP_IO_MUX_7
#define APP_ADC_LIT_INPUT_SRC               ADC_INPUT_SRC_IO3
typedef enum{
	USER_ADC_BAT,
	USER_ADC_LIT,
	USER_ADC_MAX,
}user_adc_type_t;
/******
 * 功能：ADC初始化
 * 参数：
 *  adc_type：选择要初始化的adc设备，可填USER_ADC_BAT-电池, USER_ADC_LIT-光敏
 * 
 * ********/
void Adc_Init(user_adc_type_t dev);

//获取ADC值, 直接使用, 代码中有初始化
double app_adc_get_volt(user_adc_type_t dev);

long Light_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString);

uint8_t light_get(void);

uint8_t light_measure(void);
#endif


