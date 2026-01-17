#include "service_server.h"
#include "app_service_simulate.h"
#include "service_hr_bo.h"
#include "service_sport.h"
#include "service_environment.h"
#include "app_rtc.h"
#include "service_setting.h"
#include "service_body.h"
#include "app_log.h"
#include "service_ctrl.h"
#include "app_service_task.h"
#include "Motor.h"

//获取手环参数
void wristband_get_parameter(wristband_parameters_t *para)
{
	app_sport_t sport_data;
	app_environment_t environment_data;
	
	app_sport_get_data(&sport_data);
	app_environment_get_data(&environment_data);
	
	para->hr = app_heart_rate_get_current();
	para->bo = app_bo_get_current();
	para->step = sport_data.step;
	para->pres = environment_data.pressure;
	para->temp = environment_data.temperature;
	para->light_level = environment_data.visibility;
	para->power = app_environment_get_power();
}

//获取当前时间
void wristband_get_time(wristband_time_t *time)
{
	app_rtc_time_t current_time;
	app_rtc_get_time(&current_time);
	time->day = current_time.date;
	time->hour = current_time.hour;
	time->min = current_time.min;
	time->month = current_time.mon;
	time->sec = current_time.sec;
	time->week = current_time.week;
	time->year = current_time.year;
}

//写入消息(服务器下发)
bool wristband_set_msg( char *msg, uint32_t len)
{
	uint8_t send_data[APP_SERVICE_EVT_QUEUE_DATA_SIZE];
	app_notice_add( (int8_t*)msg, len);
	app_motor_shake( MOTOR_SHAKE_SHORT_DELAY, false);
	send_data[0] = SERVICE_EVT_SCREEN_ON;
	*(uint32_t*)(send_data+1) = 10;
	osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
	return true;
}

//设置手环时间
bool wristband_set_time( wristband_time_t *time)
{
	app_rtc_time_t current_time;
	current_time.date	= time->day;
	current_time.hour	= time->hour;
	current_time.min	= time->min; 
	current_time.mon	= time->month;
	current_time.sec	= time->sec;
	current_time.week	= time->week;
	current_time.year	= time->year;
	current_time.ms		= 0;
	
	service_setting_renew_time(&current_time);
	return true;
}

//设置SN号
bool wristband_set_sn( uint16_t sn)
{
	service_setting_set_sn(sn);
	return true;
}

//获取SN号
uint16_t wristband_get_sn(void)
{
	return service_setting_get_sn();
}

//获取版本号

char device_mac[16] = "test";
//设置MAC, 13字节
bool wristband_set_mac( char * mac, uint8_t len)
{
	memcpy( device_mac, mac, len);
	return true;
}

//获取MAC， 13字节
bool wristband_get_mac(char * mac, uint8_t len)
{
	memcpy( mac, device_mac, len);
	return true;
}

//写入精力(服务器下发)
void wristband_set_energy( uint8_t *data, uint8_t len)
{
	app_body_set_energy( data, len);
}

//写入情绪(服务器下发)
void wristband_set_emotion( uint8_t *data, uint8_t len)
{
	app_body_set_emotion( data, len);
}

//写入心理压力(服务器下发)
void wristband_set_press( uint8_t *data, uint8_t len)
{
	app_body_set_psychology( data, len);
}

//写入视觉疲劳(服务器下发)
void wristband_set_fatigue( uint8_t *data, uint8_t len)
{
	app_body_set_vision( data, len);
}

//写入脑力(服务器下发)
void wristband_set_mental( uint8_t *data, uint8_t len)
{
	app_body_set_mental( data, len);
}

//设置二维码
void wristband_set_qrcode( uint8_t *qr_code, uint8_t len)
{
	service_set_qr_code(qr_code);
}

//电机控制
void motor_pwr_set( uint8_t sta)
{
	if(sta)
	{
		Motor_Start();
	}
	else
	{
		Motor_Stop();
	}
}

