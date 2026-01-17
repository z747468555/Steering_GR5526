#include "app_service_task.h"
#include "app_sys_manager.h"
#include "service_notice.h"
#include "service_setting.h"
#include "service_environment.h"
#include "service_body.h"
#include "service_ctrl.h"
#include "service_key.h"
#include "app_rtc.h"
#include "charge.h"
#include "ui_win_message.h"
#include "app_service_simulate.h"
#include "app_3220_task.h"
#include "QMI8658A.h"
#include "service_hr_bo.h"
#include "app_mems_task.h"
#include "service_sport.h"
#include "LED.h"
#include "service_server.h"
#include "lv_user_task.h"
#include "service_hr_bo.h"
#include "service_user_data.h"
#include "user_app.h"
#include "transport_scheduler.h"
#include "gus.h"
#include "drv_adapter_display.h"
#include "app_power_manager.h"
#include "FreeRTOS.h"
#include "task.h"
#include "service_ota.h"

osal_queue_handle_t app_service_evt_queue;

void QMI_update_acc(void);

void gh3220_power_off(void);
void gh3220_reset(void);

void app_rtc_evt_handler(app_rtc_evt_t *p_evt)
{
	switch(p_evt->type)
	{
		case APP_RTC_EVT_TICK_ALARM:
			clock_run_per_sec();
			break;
		default:
			break;
	}
}

bool app_power_off_without_rtc = false;
bool app_power_off_without_rtc_status_get()
{
	return app_power_off_without_rtc;
}	

extern uint8_t power_on_flag;
void app_service_evt_task(void * args)
{
	uint8_t recv_data[APP_SERVICE_EVT_QUEUE_DATA_SIZE];
	uint8_t send_data[APP_SERVICE_EVT_QUEUE_DATA_SIZE];
	
	/* 开机检测是否电池唤醒 */
	if(APP_IO_PIN_SET == app_io_read_pin( APP_KEY_3_IO_TYPE, APP_KEY_3_PIN))
	{
		send_data[0] = SERVICE_EVT_SHUTDOWN;
		osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
	}
	
	while(1)
	{
		
		if(OSAL_SUCCESS == osal_queue_receive( app_service_evt_queue, recv_data, OSAL_MAX_DELAY))
		{
			//handle msg
			switch(recv_data[0])
			{
				case SERVICE_EVT_OTA_CONTINUE:
					service_ota_continue();
					break;
				case SERVICE_EVT_OTA_WRITE:
					{
						service_ota_write();
					}
					break;
				case SERVICE_EVT_SHUT_WITHOUT_RTC:
					//删除任务
					app_power_off_without_rtc = true;
					
					//gh3220_power_off();
					osal_task_delay(3000);
				
					gh3220_reset();
					
					//gh3220_power_off();
					//sys_state_switch(SYS_STATE_SLEEP);
					//drv_adapter_disp_on(false);
				
					break;
				case SERVICE_EVT_SHUTDOWN:
					//Flash 存储
					service_data_save();
					APP_LOG_INFO("Service_data_save done!");
					//关机
					APP_LOG_INFO("enter ship mode");
					Enter_Ship_Mode();
					break;
				case SERVICE_EVT_SYSRESET:
					//Flash 存储
					service_data_save();
					APP_LOG_INFO("Service_data_save done!");
					//重启
					APP_LOG_INFO("system reset");
					hal_nvic_system_reset();
					break;
				case SERVICE_EVT_SETBL:
					BackLight_Set(recv_data[1]);
					break;
				case SERVICE_EVT_UPDATE_TEMP:
					app_environment_update_temp();
					break;
				case SERVICE_EVT_UPDATE_PRE:
					app_environment_update_pressure();
					break;
				case SERVICE_EVT_UPDATE_VISIBILITY:
					app_environment_update_visibility();
					break;
				case SERVICE_EVT_UPDATE_POWER:
					app_environment_update_power();
					break;
				case SERVICE_EVT_UPDATE_STEP:
					app_sport_step_update();
					break;
				case SERVICE_EVT_UPDATE_CHARGE:
					break;
				case SERVICE_EVT_SCREEN_ON:
				{
					uint32_t delay;
					delay = *(uint32_t*)(recv_data+1);
					osal_task_delay_ms(delay);
					screen_wakeup();
					break;
				}
				case SERVICE_EVT_LEAN_ON:
					//app_hr_bo_lean_on_set(true);
					break;
				case SERVICE_EVT_LEAN_OFF:
					//app_hr_bo_lean_on_set(false);
					break;
				case SERVICE_EVT_CLEAR_STEP:
					app_sport_step_clear();
					break;
				case SERVICE_EVT_UPDATE_ACC:
					QMI_update_acc();
					break;
				default:
					break;
			}
		}
	}
}

void app_service_run_task(void * args)
{
	static uint32_t tick;
	uint8_t send_data[APP_SERVICE_EVT_QUEUE_DATA_SIZE];
	
//	osal_task_delay_ms(10000);
//	cardiogram_set(true);
//	osal_task_delay_ms(40000);
//	cardiogram_set(false);
//	
//	
//	while(1)
//	{
//		osal_task_delay_ms(10000);
//	}
	
	app_mems_init();
	
	app_brightness_restore();
	
	osal_task_delay_ms(1000);
	
	while(1)
	{		

		if(app_power_off_without_rtc_status_get())
		{
			osal_task_delay_ms(10000);
			continue;
		}
		
		if(app_hr_bo_lean_on_get() == false)
		{
//			//update acc
//			{
//				send_data[0] = SERVICE_EVT_UPDATE_ACC;
//				osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
//			}
			
			//LED
			if(tick%1000 == 0)
			{
				service_led_flash_once();
			}
			
			
			//温度
			if(tick%1000 == 0)
			{
				send_data[0] = SERVICE_EVT_UPDATE_TEMP;
				osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
			}
			
			//心率血氧
			if(tick%200 == 100)
			{
				app_hr_bo_update();
			}
			
			//气压
			if(tick%1000 == 200)
			{
				send_data[0] = SERVICE_EVT_UPDATE_PRE;
				osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
			}
			
			//光感
			if(tick%500 == 350)
			{
				send_data[0] = SERVICE_EVT_UPDATE_VISIBILITY;
				osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
			}
			
			//电量
			if(tick%1000 == 400)
			{
				send_data[0] = SERVICE_EVT_UPDATE_POWER;
				osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
			}
			
			//计步
			if(tick%500 == 450)
			{
				send_data[0] = SERVICE_EVT_UPDATE_STEP;
				osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
			}
		}
		
		//断点续传
		{
			static bool ota_continue_detected = false;
			if(tick%1000 == 990)
			{
				if(ota_continue_detected == false)
				{
					APP_LOG_INFO("ota_continue trig");
					send_data[0] = SERVICE_EVT_OTA_CONTINUE;
					osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
					ota_continue_detected = true;
				}	
			}
			
		}
		
//		//test code
//		if(tick%2000 == 1990)
//		{
//			service_setting_set_sn(110);
//		}
		
		if(SYS_STATE_SLEEP != sys_state_get())
		{
			osal_task_delay_ms(10);
		}
		else
		{
			osal_task_delay_ms(1000);
		}
		
		
		tick ++;
	}
}

void app_service_init()
{
	osal_queue_create( &app_service_evt_queue, APP_SERVICE_EVT_QUEUE_DEPTH, APP_SERVICE_EVT_QUEUE_DATA_SIZE);
	service_ctrl_init();
	service_key_init();
	app_encironment_init();
	app_rtc_init(app_rtc_evt_handler);
	app_notice_init();
	app_setting_init();
	app_body_init();
	
	//加载Flash数据
}

STACK_HEAP_INIT(heaps_table);
void app_ble_task(void *arg)
{
	uint8_t s_ble_tx_data[] = {"System on"};
    ble_stack_init(ble_evt_handler, &heaps_table);    
    while (1)
	{
		if(app_power_off_without_rtc_status_get())
		{
			osal_task_delay_ms(10000);
			continue;
		}
        app_log_flush();
        transport_schedule();
        pwr_mgmt_schedule();
        osal_task_delay(100);
    }
    
}


void service_ota_init(void);
void app_service_task_create(void)
{
	service_ota_init();
	app_service_init();
    osal_task_create("service_evt", app_service_evt_task, TASK_SIZE_SERVICE_EVT, TASK_PRIO_EVT_SERVICE, &g_task_handle.app_service_evt_handle);
    osal_task_create("service_run", app_service_run_task, TASK_SIZE_SERVICE_RUN, TASK_PRIO_RUN_SERVICE, &g_task_handle.app_service_run_handle);
	service_server_task_create();
	osal_task_create("3220 Test", app_3220_task, 2048*3, 6, &g_task_handle.app_3220_handle);
    //osal_task_create("BLE Test", app_ble_task, 2048, 5, &g_task_handle.ble_test);
}




