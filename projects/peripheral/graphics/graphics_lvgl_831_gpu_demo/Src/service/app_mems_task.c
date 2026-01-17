#include "QMI8658A.h"
#include "app_mems_task.h"

#include "app_log.h"
uint32_t QMI_Step = 0;
uint8_t Sit_Remind = 0;

uint32_t QMI_Step_Sub = 0;

uint32_t step_get()
{
	return (QMI_Get_Step()-QMI_Step_Sub);
}

void step_clear()
{
	QMI_Clear_Step();
	//QMI_Step_Sub = QMI_Get_Step();
}

void app_mems_init()
{
//	//test start 
//	qmi8658a_read_acc_by_polling_status_register( NULL, 0 ,NULL);
//	QMI_Init();
//	while(1)
//	{
//		QMI_Test();
//		osal_task_delay_ms(1000);
//	}
//	
//	while(1)
//	{
//		osal_task_delay_ms(1000);
//		APP_LOG_INFO("test finished!");
//	}
//	//test end
	QMI_Init();
	QMI_Enable_Pedometer(125);
}

//启动步数检测，并每10s更新一次步数
//现在步数检测的参数设置得比较敏感, 如果需要调参, 在QMI_Enable_Pedometer()中去调
void app_service_pedometer(void *args)
{
    while(1){
        QMI_Step = QMI_Get_Step();
		
        APP_LOG_INFO( "Step:%d\r\n", QMI_Step);
        osal_task_delay_ms(1000 * PEDOMETER_PERIOD);
    }
}


//久坐提醒, 产生提醒时, Sit_Remind会置1
void app_service_sit_remind(void *args)
{
    static uint32_t old_step = 0, now_step = 0;
    uint16_t sit_count = 0;
    old_step = QMI_Step;
    now_step = QMI_Step;
    while(1){
        old_step = now_step;
        now_step = QMI_Step;
        if(now_step == old_step){
            sit_count ++;
            if(sit_count > SIT_THRESHOLD){
                sit_count = 0;
                APP_LOG_INFO( "Sit Remind On\r\n");
                Sit_Remind = 1; //TODO
            }
        }
        osal_task_delay_ms(1000 * SIT_REMIND_PERIOD);
    }
}

