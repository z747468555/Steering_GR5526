#include "service_environment.h"
#include "app_service_simulate.h"
#include "ui_win_message.h"
#include "ui_win_manager.h"
#include "QMI8658A.h"
#include "app_log.h"

uint16_t app_environment_pressure;
float app_environment_temperature;
uint8_t app_environment_visibility;
uint8_t app_environment_power = 0;
uint8_t app_environment_charge_status;


void app_environment_update_pressure()
{
	app_environment_pressure = (uint16_t)HP_Read_Pres();
}

void app_environment_update_temp()
{
	//app_environment_temperature = temp_sensor_read_temp(&sensor_cfg);
	app_environment_temperature = HP_Read_Temp();
}

void app_environment_update_visibility()
{
	app_environment_visibility = light_get();
	send_msg_to_ui_high( EVENT_ENVIRONMENT_CHANGE, EVENT_ENVIRONMENT_CHANGE);
}


void app_environment_update_charge()
{
	uint8_t val = Get_Charge_Status();
	if((val==0)||(val==3))
	{
		app_environment_charge_status = 0;
	}
	else
	{
		app_environment_charge_status = 1;
	}
	send_msg_to_ui_high( EVENT_CHARGER_CHANGE, EVENT_CHARGER_CHANGE);
}

uint8_t app_environment_get_charge_status(void)
{
	return app_environment_charge_status;
}

void app_environment_update_power()
{
#define CHARGE_UPDATE_LOCK_VAL	6
	static uint32_t charge_update_lock = 0;
	uint8_t val = charge_get_power();
	if(app_environment_power != val)
	{
		//更新电量
		if(app_environment_power)
		{
			if(val>app_environment_power)
			{
				if(charge_update_lock<CHARGE_UPDATE_LOCK_VAL)
				{
					charge_update_lock ++;
				}
				else
				{
					charge_update_lock = 0;
					app_environment_power ++;
					send_msg_to_ui_high( EVENT_ELETRICITY_CHANGE, EVENT_ELETRICITY_CHANGE);
				}
			}
			else
			{
				if(charge_update_lock<CHARGE_UPDATE_LOCK_VAL)
				{
					charge_update_lock ++;
				}
				else
				{
					charge_update_lock = 0;
					app_environment_power --;
					send_msg_to_ui_high( EVENT_ELETRICITY_CHANGE, EVENT_ELETRICITY_CHANGE);
				}
			}
		}
		else
		{
			app_environment_power = val;
			send_msg_to_ui_high( EVENT_ELETRICITY_CHANGE, EVENT_ELETRICITY_CHANGE);
		}
	}
	
	app_environment_update_charge();
}


uint8_t app_environment_get_power()
{
	return app_environment_power;
}


bool app_environment_get_data(app_environment_t* environment_data)
{
	environment_data->pressure = app_environment_pressure;
	environment_data->temperature = app_environment_temperature;
	environment_data->visibility = app_environment_visibility;
	APP_LOG_INFO("pre=%d,temp=%f,vis=%d",environment_data->pressure,environment_data->temperature,environment_data->visibility);
	return true;
}

void app_encironment_init()
{
	Charge_Init();
	temp_sensor_init();
}




