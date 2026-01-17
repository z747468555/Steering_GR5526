#ifndef __SERVICE_HR_BO_H__
#define __SERVICE_HR_BO_H__
#include "app_service_simulate.h"

#define HR_SAMPLE_DURATION		2		//心率采样间隔 		S
#define HR_SAMPLE_CYCLE			3600	//心率采样记录总时长	S

#define APP_HR_BO_MAX_LEN		30		//心率血氧记录数据长度

#define APP_HRV_MAX_CNT			18		
uint8_t app_bo_get_current(void);


void app_hr_bo_update(void);

void app_cardiogram_updated( int16_t *inbuf, uint32_t len);

void service_hr_bo_data_init(bool sta);

void app_hr_bo_lean_on_set(bool sta);

bool app_hr_bo_lean_on_get(void);

void app_hrv_update( uint8_t num, uint32_t *rri);

void app_hrv_get( uint8_t *rri);

void app_hr_bo_valid_set(void);

void app_hr_bo_valid_clear(void);

#endif

