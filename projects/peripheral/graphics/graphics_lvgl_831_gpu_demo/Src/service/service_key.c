#define APP_LOG_TAG	"key"
#include "app_log.h"
#include "service_key.h"
#include "app_service_task.h"
#include "app_service_simulate.h"
#include "ui_win_message.h"
#include "ui_win_manager.h"
#include "lv_user_task.h"
#include "service_server.h"
#include "FreeRTOS.h"
#include "Timers.h"
#include "service_setting.h"
#include "app_sys_manager.h"

TimerHandle_t key_long_press_detect_timer_handle = NULL;

static uint32_t key_long_press_detect_cnt = 0;
uint8_t power_on_flag = 0;

void service_key_process(uint8_t key_id, app_key_click_type_t key_click_type)
{
	
	APP_LOG_INFO("key_id=%d.type=%d,tick=%d\r\n",key_id,key_click_type,xTaskGetTickCount());
	
	switch(key_id)
	{
		//右上
		case APP_KEY_1_ID:
			if(app_power_off_without_rtc_status_get())
			{
				break;
			}
			if(APP_KEY_SINGLE_CLICK == key_click_type)
			{
				//返回表盘
				screen_wakeup();
				send_msg_to_ui_high( EVENT_UPPER_RIGHT_KEY_CLICK, EVENT_UPPER_RIGHT_KEY_CLICK);
			}
			break;
		//左上
		case APP_KEY_2_ID:
			/* 软关机状态 */
			if(app_power_off_without_rtc_status_get())
			{
				break;
			}
			if(APP_KEY_LONG_CLICK == key_click_type)
			{
				//SOS
				screen_wakeup();
				app_sos_set_status(true,true);
			}
			break;
		case APP_KEY_3_ID:
			
			if(APP_KEY_SINGLE_CLICK == key_click_type && SYS_STATE_ACTIVE == sys_state_get())
			{
				/* 软关机状态 */
				if(app_power_off_without_rtc_status_get())
				{
					break;
				}
				sys_state_switch(SYS_STATE_SCREEN_OFF);
			}
			else if(APP_KEY_SINGLE_CLICK == key_click_type && SYS_STATE_SCREEN_OFF == sys_state_get() || SYS_STATE_SLEEP == sys_state_get())
			{
				/* 软关机状态 */
				if(app_power_off_without_rtc_status_get())
				{
					break;
				}
				screen_wakeup();
				sys_state_switch(SYS_STATE_ACTIVE);
			}
			if(APP_KEY_LONG_CLICK == key_click_type)
			{
				/* 软关机状态 */
				if(app_power_off_without_rtc_status_get())
				{
					/* 重启 */
					service_sys_reset();
				}
				else
				{
					screen_wakeup();
					BaseType_t xHigherPriorityTaskWoken = pdFALSE;
					/* 进入关机页面 */
					send_msg_to_ui_high( EVENT_SYSTEM_SHUT_DOWN, EVENT_SYSTEM_SHUT_DOWN);

					xTimerStartFromISR( key_long_press_detect_timer_handle, &xHigherPriorityTaskWoken);
				}
			}
			break;
		default:
			break;
	}
}

void key_long_press_detect_timer_callback(TimerHandle_t pxTimer)
{
	 if(APP_IO_PIN_SET == app_io_read_pin( APP_KEY_3_IO_TYPE, APP_KEY_3_PIN))
	 {
		 xTimerStop( key_long_press_detect_timer_handle, portMAX_DELAY);
		 return;
	 }

	if(key_long_press_detect_cnt<30)
	{
		key_long_press_detect_cnt++;
		return;
	}
	
	{
		uint8_t send_data[APP_SERVICE_EVT_QUEUE_DATA_SIZE];
		send_data[0] = SERVICE_EVT_SHUTDOWN;
		osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
	}
}


void service_key_init(void)
{
	app_key_gpio_t app_key_inst[3];

    app_key_inst[0].gpio_type    = APP_KEY_1_IO_TYPE;
    app_key_inst[0].gpio_pin     = APP_KEY_1_PIN;
    app_key_inst[0].trigger_mode = APP_KEY_1_TRIGGER_MODE;
    app_key_inst[0].pull         = APP_KEY_1_PULL_MODE;
    app_key_inst[0].key_id       = APP_KEY_1_ID;

    app_key_inst[1].gpio_type    = APP_KEY_2_IO_TYPE;
    app_key_inst[1].gpio_pin     = APP_KEY_2_PIN;
    app_key_inst[1].trigger_mode = APP_KEY_2_TRIGGER_MODE;
    app_key_inst[1].pull         = APP_KEY_2_PULL_MODE;
    app_key_inst[1].key_id       = APP_KEY_2_ID;
	
	app_key_inst[2].gpio_type    = APP_KEY_3_IO_TYPE;
    app_key_inst[2].gpio_pin     = APP_KEY_3_PIN;
    app_key_inst[2].trigger_mode = APP_KEY_3_TRIGGER_MODE;
    app_key_inst[2].pull         = APP_KEY_3_PULL_MODE;
    app_key_inst[2].key_id       = APP_KEY_3_ID;

	
    app_key_init(app_key_inst, 3, service_key_process);
	key_long_press_detect_timer_handle = xTimerCreate( "motor", 100, pdTRUE,  0, key_long_press_detect_timer_callback);
}



