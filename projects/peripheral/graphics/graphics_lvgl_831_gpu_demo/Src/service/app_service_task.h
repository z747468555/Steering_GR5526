#ifndef __APP_SERVICE_TASK_H__
#define __APP_SERVICE_TASK_H__
#include "osal.h"

#define APP_SERVICE_EVT_QUEUE_DEPTH			16
#define APP_SERVICE_EVT_QUEUE_DATA_SIZE		64

typedef enum
{
	SERVICE_EVT_SHUTDOWN,			//关机
	SERVICE_EVT_SYSRESET,			//系统重启
	SERVICE_EVT_SHUT_WITHOUT_RTC,	//关机并保留时钟
	SERVICE_EVT_SETBL,				//设置背光
	SERVICE_EVT_UPDATE_TEMP,		//更新温度
	SERVICE_EVT_UPDATE_PRE,			//更新气压
	SERVICE_EVT_UPDATE_VISIBILITY,	//更新环境光
	SERVICE_EVT_UPDATE_POWER,		//更新电量
	SERVICE_EVT_UPDATE_STEP,		//更新步数
	SERVICE_EVT_UPDATE_CHARGE,		//更新充电状态
	SERVICE_EVT_SCREEN_ON,			//亮屏
	SERVICE_EVT_LEAN_ON,			// lean on 
	SERVICE_EVT_LEAN_OFF,			// lean off
	SERVICE_EVT_CLEAR_STEP,			// 清步数
	SERVICE_EVT_UPDATE_ACC,			//更新ACC值
	SERVICE_EVT_OTA_WRITE,			//写入OTA数据
	SERVICE_EVT_OTA_CONTINUE,		//OTA断点续传
} service_evt_e;

#define MOTOR_SHAKE_SHORT_DELAY		100
#define MOTOR_SHAKE_MIDIUM_DELAY	3000
#define MOTOR_SHAKE_LONG_DELAY		3000

extern osal_queue_handle_t app_service_evt_queue;

void app_service_task_create(void);

bool app_power_off_without_rtc_status_get(void);

#endif

