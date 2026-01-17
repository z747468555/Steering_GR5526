#include "service_ctrl.h"
#include "osal.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "LED.h"

TimerHandle_t service_ctrl_led_timer_handle = NULL;

void service_ctrl_motor_shake( uint32_t delay)
{
	Motor_Start();
	osal_task_delay_ms(delay);
	Motor_Stop();
}

void service_led_timer_cb(TimerHandle_t pxTimer)
{
	LED_Close(LED_BAT);
}

void service_led_flash_once()
{
	LED_Open(LED_BAT);
	xTimerStart( service_ctrl_led_timer_handle, portMAX_DELAY);
}

void service_ctrl_init()
{
	service_ctrl_led_timer_handle = xTimerCreate( "led", 1, pdFALSE,  0, service_led_timer_cb);
	Motor_Init();
}



