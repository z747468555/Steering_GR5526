#ifndef __SERVICE_USER_DATA_H__
#define __SERVICE_USER_DATA_H__
#include "service_setting.h"
#include "service_hr_bo.h"

#pragma pack (1)
typedef struct
{
	app_time_t end_tim;
	uint8_t hr_dat[APP_HR_BO_MAX_LEN];
	uint8_t bo_dat[APP_HR_BO_MAX_LEN];
	uint8_t hr_min;
	uint8_t hr_max;
	
}service_data_hr_bo_t;

typedef struct
{
	uint8_t qr_code[QR_CODE_LEN];
	uint16_t sn;
	char mac[MAC_LEN];
	char nfc[NFC_LEN];
	char tsm[TSM_LEN];
	bool mode;
	uint16_t resting_time;
	uint8_t brightness;
	app_rtc_time_t sys_time;
}service_data_dev_info_t;

typedef struct
{
	uint32_t step;
	
}service_data_sport_t;


typedef struct
{
	app_notice_t notice[20];
	char msg[20][64];
	uint32_t cnt;
}service_data_msg_t;


typedef struct
{
	app_alarm_clock_list_t alarm_clock;
}service_data_alarm_clock_t;

typedef struct
{
	uint32_t data_version;
	uint8_t  cks8;
} service_data_flash_mng_t;


typedef struct
{
	service_data_hr_bo_t hr_bo;
	service_data_dev_info_t dev_info;
	service_data_sport_t sport;
	service_data_msg_t msg;
	service_data_alarm_clock_t alarm_clock;
	service_data_flash_mng_t flash_mng;
}service_data_t;

#pragma pack ()

extern service_data_t service_data;


//存储心率血氧
void service_data_save_hr_bo( service_data_hr_bo_t *payload);
void service_data_load_hr_bo( service_data_hr_bo_t *payload);

//存储设备信息
void service_data_save_dev_info(service_data_dev_info_t *payload);
void service_data_load_dev_info(service_data_dev_info_t *payload);

//存储计步信息
void service_data_save_step(service_data_sport_t *payload);
void service_data_load_step(service_data_sport_t *payload);

//存储消息
void service_data_save_msg(service_data_msg_t *payload);
void service_data_load_msg(service_data_msg_t *payload);

//存储闹钟
void service_data_save_alarm_clock(service_data_alarm_clock_t *payload);
void service_data_load_alarm_clock(service_data_alarm_clock_t *payload);


//save all data
void service_data_save(void);

//load all data
void service_data_load(void);

void service_data_self_test(void);

#endif
