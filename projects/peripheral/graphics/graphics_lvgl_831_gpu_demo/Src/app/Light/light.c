#include "light.h"
#include "app_adc.h"
#include "app_adc_dma.h"
#define APP_LOG_TAG	"light"
#include "app_log.h"
#include "osal.h"
#include "Charge.h"
#include "gus.h"
/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
volatile uint16_t covn_done = 0;
volatile uint16_t bat_covn_done = 0;
app_adc_params_t adc_params = {
    .pin_cfg = {
        .channel_n = {
            .type = APP_IO_TYPE_MSIO,
            .mux  = APP_ADC_BAT_INPUT_PIN_MUX,
            .pin  = APP_ADC_BAT_INPUT_PIN,
        },
    },
    .dma_cfg = {
        .dma_instance = DMA0,
        .dma_channel  = DMA_Channel2,
    },
    .init = {
        .input_mode = ADC_INPUT_SINGLE,
        .channel_n  = APP_ADC_BAT_INPUT_SRC,
        .ref_source = ADC_REF_SRC_BUF_INT,
        .ref_value  = ADC_REF_VALUE_1P6,
        .clock      = ADC_CLK_8M,
    },
};
/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void app_evt_handler(app_adc_evt_t * p_evt)
{
    if (p_evt->type == APP_ADC_EVT_CONV_CPLT)
    {
        covn_done = 1;
    }
}

void Adc_Init(user_adc_type_t dev)
{
	app_adc_deinit();
	//电池ADC初始化
	if(dev == USER_ADC_BAT)
	{
		adc_params.pin_cfg.channel_n.pin = APP_ADC_BAT_INPUT_PIN;
		adc_params.init.channel_n = APP_ADC_BAT_INPUT_SRC;
		app_adc_init(&adc_params, app_evt_handler);
	}
	//光敏ADC初始化
	else if(dev == USER_ADC_LIT)
	{
		adc_params.pin_cfg.channel_n.pin = APP_ADC_LIT_INPUT_PIN;
		adc_params.init.channel_n = APP_ADC_LIT_INPUT_SRC;
		app_adc_init(&adc_params, app_evt_handler);
	}
	app_adc_dma_deinit();
	if(APP_DRV_SUCCESS!= app_adc_dma_init(&adc_params))
	{
		APP_LOG_INFO("adc init fail!");
	}
}

uint8_t light_get()
{
	double val;
	uint8_t ret;
	
	//disable cha stat
	Charge_stat_set(false);
	val = app_adc_get_volt(USER_ADC_LIT);
	
	//enable cha stat
	Charge_stat_set(true);
	
	if(val>3.3)
	{
		return 1;
	}
	else if(val<2.21)
	{
		return 12;
	}
	else
	{
		ret = (12 - (val-2.21)/0.0988);
		if(ret<1)
			ret = 1;
	}
	return ret;
}


long Light_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString)
{
	double val;
	val = light_get();
	APP_LOG_INFO("light level:%d", val);
	return 0;
}

double app_adc_get_volt(user_adc_type_t dev)
{
	uint16_t voltage[2];
	double v_test;
	Adc_Init(dev);
	covn_done = 0;
	app_adc_dma_conversion_async(voltage, 1);
	while(covn_done == 0)
	{
		osal_task_delay_ms(10);
	}
	app_adc_voltage_intern(voltage,&v_test, 1);
	return v_test;
}

#define TEST_CONV_LENGTH 2
uint16_t conversion[TEST_CONV_LENGTH];

uint8_t light_measure(void)
{
    double vbat;

	adc_params.pin_cfg.channel_n.pin = APP_ADC_LIT_INPUT_PIN;
    adc_params.init.channel_n = APP_ADC_LIT_INPUT_SRC;
    app_adc_deinit();
    app_adc_init(&adc_params, app_evt_handler);
    app_adc_dma_deinit();
    app_adc_dma_init(&adc_params);

    covn_done = 0;
    app_adc_dma_conversion_async(conversion, TEST_CONV_LENGTH);
    while(covn_done == 0)
	{
		osal_task_delay_ms(10);
	}

    uint32_t aver = 0;
    for(uint32_t i = 0; i<TEST_CONV_LENGTH; i++)
    {
        aver = aver + conversion[i];
    }
    aver = aver/(TEST_CONV_LENGTH);
    APP_LOG_INFO("Average light = %d", aver);
    app_adc_vbat_conv((uint16_t *)&aver, &vbat, 1);
    APP_LOG_INFO("light= %0.3f V", vbat);
	
	return (12 - (uint8_t)(vbat*3.64));
}


#define TEST_CONV_LENGTH 2
uint16_t conversion2[TEST_CONV_LENGTH];

uint8_t power_measure(void)
{
    double vbat;
	
	//
	
	//

	adc_params.pin_cfg.channel_n.pin = APP_ADC_BAT_INPUT_PIN;
    adc_params.init.channel_n = APP_ADC_LIT_INPUT_SRC;
    app_adc_deinit();
    app_adc_init(&adc_params, app_evt_handler);
    app_adc_dma_deinit();
    app_adc_dma_init(&adc_params);

    covn_done = 0;
    app_adc_dma_conversion_async(conversion2, TEST_CONV_LENGTH);
    while(covn_done == 0)
	{
		osal_task_delay_ms(10);
	}

    uint32_t aver = 0;
    for(uint32_t i = 0; i<TEST_CONV_LENGTH; i++)
    {
        aver = aver + conversion2[i];
    }
    aver = aver/(TEST_CONV_LENGTH);
    APP_LOG_INFO("Average power = %d", aver);
    app_adc_vbat_conv((uint16_t *)&aver, &vbat, 1);
    APP_LOG_INFO("power= %0.3f V", vbat);
	
	return (12 - (uint8_t)(vbat*3.64));
}

