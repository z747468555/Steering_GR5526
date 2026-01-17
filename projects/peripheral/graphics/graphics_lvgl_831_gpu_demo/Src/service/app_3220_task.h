#ifndef __APP_3220_TASK_H_
#define __APP_3220_TASK_H_
#include "osal_task.h"

#define LEAD_ON_OFF_FUNC	1

void app_3220_task(void *p_arg);

uint8_t heart_rate_get(void);

uint8_t spo2_get(void);

void ECG_Start(void);

void ECG_Stop(void);

//将心率 血氧 ECG相关的变量全部清零
void Clear_Vars(void);

uint8_t ecg_hr_get(void);

void ecg_hr_set(uint8_t hr);

void cardiogram_report_record(uint8_t hr);

bool gh3220_ecg_print_get(void);

void gh3220_ecg_print_en_timer_start(void);

void gh3220_ecg_print_set(bool sta);
#endif


