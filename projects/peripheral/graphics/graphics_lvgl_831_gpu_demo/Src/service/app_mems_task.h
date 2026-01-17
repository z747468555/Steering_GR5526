#ifndef __APP_MEMS_TASK_H_
#define __APP_MEMS_TASK_H_
#include "osal_task.h"

//步数更新周期 单位/s
#define PEDOMETER_PERIOD    1
//久坐提醒状态更新周期 单位/s
#define SIT_REMIND_PERIOD   10
//SIT_REMIND_PERIOD * SIT_THRESHOLD的时间为久坐提醒的时间
#define SIT_THRESHOLD       2

//启动步数检测，并每10s更新一次步数
//现在步数检测的参数设置得比较敏感, 如果需要调参, 在QMI_Enable_Pedometer()中去调
void app_service_pedometer(void *args);


//久坐提醒
void app_service_sit_remind(void *args);

//
uint32_t step_get(void);

void step_clear(void);

void app_mems_init(void);

#endif // !__APP_MEMS_TASK_H_

