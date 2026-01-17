#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "watcher.h"

#define APP_LOG_TAG "hrbo"
#include "app_log.h"
#include "board_SK.h"

#ifndef GH3X2X_AFE_CODE
#include "gh_drv.h"
#include "gh_demo.h"
#include "gh_demo_config.h"
#include "SEGGER_RTT.h"
#else
#include "gh3x2x_afe_user.h"
#include "gh3x2x_afe_drv.h"
//#include "gh3x2x_afe_common.h"
//#include "gh3x2x_afe_reg.h"
#endif

#define GH3X2X_APP_TASK_STACK_SIZE             ( 512 * 4 )//unit : word


//佩戴监测buff
uint8_t ADT_index = 0;	//数据存放指针
// (bit0-bit1)：佩戴状态（0默认，1佩戴，2脱落，3非活体）；
// (bit2)：疑似脱落标记（0正常，1疑似脱落）；
uint8_t ADT[10];

//心电ECG buff
uint8_t ECG_index = 0;
//一次更新两个数据
uint8_t ECG[256];

//心率buff
uint8_t HR_index = 0;
uint8_t HR[10];

//血氧饱和度buff
uint8_t SPO2_index = 0;
uint8_t SPO2[256];


SemaphoreHandle_t g_semGh3x2x = NULL;
GU8 mode_switch = 0x00;

#ifndef GH3X2X_AFE_CODE
extern void GhMultiSensorWearDetStart(void);
static void gh3x2x_demo_app_task(void *p_arg)
{
	GS8 ret;
    Gh3x2xDemoInit();
//    vTaskDelay(5000);
    Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_ADT);
    
//     GhMultiSensorWearDetStart();
    while(1)
    {
           ret = GH3X2X_CommunicateConfirm() ;
			APP_LOG_INFO("GH3X2X Test ret: %d\r\n", ret);
//        if(pdTRUE == xSemaphoreTake(g_semGh3x2x, portMAX_DELAY))
//        {
            Gh3x2xDemoInterruptProcess();
//        }
        vTaskDelay(2000);
    }
}
#else

static void gh3x2x_demo_app_task(void *p_arg)
{
    GH3X2X_AFE_Init();

    GH3X2X_AFE_StartSampling(0x01|0x02);
    
    while(1)
    {

        if(pdTRUE == xSemaphoreTake(g_semGh3x2x, portMAX_DELAY))
        {
            if(1 == g_uchGh3x2xAfeIntCallBackIsCalled)
            {
                GH3X2X_AFE_ChipPro();
            }
            
            switch(mode_switch)
            {
                case 0x01:
                GH3X2X_AFE_StopSampling();
                GH3X2X_AFE_StartSampling(0x01|0x02);
                GH3X2X_AFE_SetSlotLedDrvCurrent(0, 0, 5);
                GH3X2X_AFE_SetSlotLedDrvCurrent(1, 0, 25);
                break;
                case 0x02:
                GH3X2X_AFE_StopSampling();
                GH3X2X_AFE_StartSampling(0x04|0x08);
                GH3X2X_AFE_SetSlotLedDrvCurrent(2, 0, 15);
                GH3X2X_AFE_SetSlotLedDrvCurrent(3, 0, 15);
                break;
                case 0x03:
                GH3X2X_AFE_StopSampling();
                GH3X2X_AFE_StartSampling(0x02|0x04|0x08);
                GH3X2X_AFE_SetSlotLedDrvCurrent(1, 0, 0);
                GH3X2X_AFE_SetSlotLedDrvCurrent(2, 0, 32);
                GH3X2X_AFE_SetSlotLedDrvCurrent(3, 0, 32);
                break;
                case 0x04:
                GH3X2X_AFE_StopSampling();
                break;
            }
            mode_switch = 0x00;
        }
    }
}
#endif


void GH3X2X_TaskCreate(void)
{
    g_semGh3x2x = xSemaphoreCreateBinary();
    xTaskCreate(gh3x2x_demo_app_task, "gh3x2x_task", GH3X2X_APP_TASK_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
    
}

void gh3x2x_app_key_evt_handler(uint8_t key_id, app_key_click_type_t key_click_type)
{
    if(key_id == 0)
    {
        if(key_click_type == APP_KEY_SINGLE_CLICK)
        {
            mode_switch = 0x01;
        }
        else if(key_click_type == APP_KEY_DOUBLE_CLICK)
        {
            mode_switch = 0x03;
        }
    }
    else if(key_id == 1)
    {
        if(key_click_type == APP_KEY_SINGLE_CLICK)
        {
            mode_switch = 0x02;
        }
        else if(key_click_type == APP_KEY_DOUBLE_CLICK)
        {
            mode_switch = 0x04;
        }
    }
    xSemaphoreGive(g_semGh3x2x);
}

void gh3x2x_interupt_request(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(g_semGh3x2x, &xHigherPriorityTaskWoken);
}
