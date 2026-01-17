#include "app_3220_task.h"
#include "gh_demo_config.h"
#include "gh_demo.h"
#include "gh_demo_inner.h"
#include "board_SK.h"
#include "string.h"
#include "osal.h"
#include "app_log.h"
#include "led.h"
#include "gus.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "app_service_simulate.h"
#include "app_service_task.h"
#include "app_sys_manager.h"

#define GH3220_PRINT_EN_TO	3000

#define APP_3220_EVT_QUEUE_DEPTH		8
#define APP_3220_EVT_QUEUE_DATA_SIZE	8


#define APP_3220_EVT_TYPE_ECG_START		0
#define APP_3220_EVT_TYPE_ECG_STOP		1

uint8_t INT3220 = 0;
uint8_t GH3220_HR = 0;
uint8_t GH3220_SPO2 = 0;
uint8_t GH3220_ADT = 0, ECG_Index = 0;
int16_t GH3220_ECG[MEASURE_POINTS_NUM];

TimerHandle_t gh3220_ecg_print_en_timer_handle = NULL;
bool gh3220_ecg_print_en_flag = false;

osal_queue_handle_t app_3220_evt_queue;

uint8_t ecg_hr = 0xff;

bool gh3220_ecg_print_get()
{
	return gh3220_ecg_print_en_flag;
}	

void gh3220_ecg_print_set(bool sta)
{
	gh3220_ecg_print_en_flag = sta;
}

void gh3220_ecg_print_en_cb(TimerHandle_t pxTimer)
{
	gh3220_ecg_print_set(true);
}

void gh3220_ecg_print_en_timer_start()
{
	xTimerStart( gh3220_ecg_print_en_timer_handle, portMAX_DELAY);
}

uint8_t ecg_hr_get()
{
	return ecg_hr;
}

void ecg_hr_set(uint8_t hr)
{
	if((hr<=100)&&(hr>=60))
	{
		if(ecg_hr!=0xff)
		{
			ecg_hr = (ecg_hr+hr)/2;
		}
		else
		{
			ecg_hr = hr;
		}
		cardiogram_report_record(ecg_hr);
	}
}

void ecg_hr_clear()
{
	ecg_hr = 0xff;
}


//将心率 血氧 ECG相关的变量全部清零
void Clear_Vars()
{
	GH3220_HR = 0;
	GH3220_SPO2 = 0;
	GH3220_ADT = 0;
	ECG_Index = 0;
	memset(GH3220_ECG, 0, sizeof(GH3220_ECG));
}

void ECG_Start()
{
	uint8_t send_data[APP_3220_EVT_QUEUE_DATA_SIZE];
	send_data[0] = APP_3220_EVT_TYPE_ECG_START;
	osal_queue_send( app_3220_evt_queue, send_data, OSAL_MAX_DELAY);
}	

void ECG_Stop()
{
	uint8_t send_data[APP_3220_EVT_QUEUE_DATA_SIZE];
	send_data[0] = APP_3220_EVT_TYPE_ECG_STOP;
	osal_queue_send( app_3220_evt_queue, send_data, OSAL_MAX_DELAY);
}	

uint8_t heart_rate_get()
{
	return GH3220_HR;
}

uint8_t spo2_get()
{
	return GH3220_SPO2;
}

void app_3220_task(void *p_arg)
{
	uint8_t recv_data[APP_3220_EVT_QUEUE_DATA_SIZE];
	osal_queue_create( &app_3220_evt_queue, APP_3220_EVT_QUEUE_DEPTH, APP_3220_EVT_QUEUE_DATA_SIZE);
	
	gh3220_ecg_print_en_timer_handle = xTimerCreate( "gh_prt", GH3220_PRINT_EN_TO, pdFALSE,  0, gh3220_ecg_print_en_cb);
	
	Gh3x2xDemoInit();
	Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_ADT);
	osal_task_delay_ms(1000);
	
    while(1)
	{
		if(app_power_off_without_rtc_status_get())
		{
			osal_task_delay_ms(10000);
			continue;
		}
		if(INT3220)
		{
			INT3220 = 0;
			Gh3x2xDemoInterruptProcess();
		}
		
		
		if(SYS_STATE_SLEEP != sys_state_get())  //????????,????uart??
		{
			if(OSAL_SUCCESS == osal_queue_receive( app_3220_evt_queue, recv_data, 5))
			{
				switch(recv_data[0])
				{
					case APP_3220_EVT_TYPE_ECG_START:	
	#if LEAD_ON_OFF_FUNC
						Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SPO2|GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_ADT);
						Gh3x2xDemoStartSamplingWithCfgSwitch( GH3X2X_FUNCTION_LEAD_DET, 1);
	#else
						Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SPO2|GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_ADT);
						Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_ECG);
						GH3X2X_FifoWatermarkThrConfig(150);
						gh3220_ecg_print_en_timer_start();
	#endif
						break;
					case APP_3220_EVT_TYPE_ECG_STOP:
	#if LEAD_ON_OFF_FUNC					
						Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_ECG|GH3X2X_FUNCTION_LEAD_DET);
	#else
						Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_ECG);
	#endif
						Gh3x2xDemoStartSamplingWithCfgSwitch( GH3X2X_FUNCTION_ADT, 0);
						gh3220_ecg_print_set(false);
						ecg_hr_clear();
						break;
				}
			}
		}
		else
		{
			if(OSAL_SUCCESS == osal_queue_receive( app_3220_evt_queue, recv_data, 5000))
			{
				switch(recv_data[0])
				{
					case APP_3220_EVT_TYPE_ECG_START:	
	#if LEAD_ON_OFF_FUNC
						Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SPO2|GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_ADT);
						Gh3x2xDemoStartSamplingWithCfgSwitch( GH3X2X_FUNCTION_LEAD_DET, 1);
	#else
						Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SPO2|GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_ADT);
						Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_ECG);
						GH3X2X_FifoWatermarkThrConfig(150);
						gh3220_ecg_print_en_timer_start();
	#endif
						break;
					case APP_3220_EVT_TYPE_ECG_STOP:
	#if LEAD_ON_OFF_FUNC					
						Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_ECG|GH3X2X_FUNCTION_LEAD_DET);
	#else
						Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_ECG);
	#endif
						Gh3x2xDemoStartSamplingWithCfgSwitch( GH3X2X_FUNCTION_ADT, 0);
						gh3220_ecg_print_set(false);
						ecg_hr_clear();
						break;
				}
			}
		}
    }
}
