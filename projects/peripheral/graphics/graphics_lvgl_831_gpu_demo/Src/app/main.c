/**
 *****************************************************************************************
 *
 * @file main.c
 *
 * @brief main function Implementation.
 *
 *****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "grx_sys.h"
#include "platform_sdk.h"
#include "osal.h"
#include "user_periph_setup.h"
#define APP_LOG_TAG "main"
#include "app_log.h"
#include "app_graphics_ospi.h"
#include "IIC.h"
#include "Charge.h"
#include "nst112c-cwlr_drive.h"
#include "QMI8658A.h"
#include "app_service_task.h"
#include "SEGGER_RTT.h"
#include "app_ecg_baremetal.h"
#include "WIFI.h"

/*
 * DEFINE
 *****************************************************************************************
 */
/* 自动启动ECG测量 */
#define AUTO_START_ECG_DELAY_MS     300
char testUart[6]= {"UART"};

/*
 * 回调函数实现
 *****************************************************************************************
 */

/**
 * @brief ECG波形数据回调函数
 * @param data ECG波形数据数组
 * @param len  数据长度
 */
static void ecg_data_handler(int16_t *data, uint32_t len)
{
    /* 打印ECG波形数据 */
    APP_LOG_INFO("ECG wave data received, len=%d", len);
    
    /* 可以在这里处理ECG波形数据，例如：
     * - 通过UART发送到上位机
     * - 存储到Flash
     * - 进行进一步分析
     */
		ecg_uart_send_raw_data(data, len);
    for (uint32_t i = 0; i < len; i++) {
        SEGGER_RTT_printf(0, "ECG,%d,%d\r\n", i, data[i]);
    }
}

/**
 * @brief ECG心率数据回调函数
 * @param hr 心率值 (bpm)
 */
static void ecg_hr_handler(uint8_t hr)
{
    APP_LOG_INFO("ECG Heart Rate: %d bpm", hr);
    
    /* 可以在这里处理心率数据，例如：
     * - 通过UART发送
     * - 触发报警
     * - 记录历史数据
     */
    SEGGER_RTT_printf(0, "HR,%d\r\n", hr);
}

void ECG_Init(void)
{
		SEGGER_RTT_printf(0,"========================================\r\n");
    SEGGER_RTT_printf(0,"GH3220 ECG Baremetal Demo\r\n");
    SEGGER_RTT_printf(0,"========================================\r\n");
    
    /* 注册ECG回调函数 */
    app_ecg_register_data_callback(ecg_data_handler);
    app_ecg_register_hr_callback(ecg_hr_handler);
    
    /* ECG模块初始化 */
    SEGGER_RTT_printf(0,"Initializing ECG module...\r\n");
    app_ecg_init();
    SEGGER_RTT_printf(0,"ECG module initialized\r\n");
	
    /* 延时后自动启动ECG测量 */
    SEGGER_RTT_printf(0,"Waiting %d ms before starting ECG...\r\n", AUTO_START_ECG_DELAY_MS);
    delay_ms(AUTO_START_ECG_DELAY_MS);
    
    SEGGER_RTT_printf(0,"Starting ECG measurement...\r\n");
    app_ecg_start();
    SEGGER_RTT_printf(0,"ECG measurement started\r\n");	
}

/**
 *****************************************************************************************
 *      DECLARATIONS
  *****************************************************************************************
**/
void lv_user_task_create(void);

/**
 *****************************************************************************************
 * @brief To create two task, the one is ble-schedule, another is watcher task
 *****************************************************************************************
 */
static void vStartTasks(void *arg)
{
	iic_mutex_create();
	app_service_task_create();
    lv_user_task_create();

    osal_task_delete(NULL);
}

/**
 *****************************************************************************************
 * @brief main function
 *****************************************************************************************
 */
int main(void)
{
//    app_graphics_adjust_dcore_policy();                             /*<Should call this firstly if using graphics modules */
    SetSerialClock(SERIAL_N96M_CLK);
    app_periph_init();                                              /*<Init user periph .*/
		UART_Init();
		SEGGER_RTT_printf(0,"board init\r\n");
		app_uart_transmit_sync(WIFI_UART_ID,(uint8_t*)"SYSINIT\r\n", strlen("SYSINIT\r\n"),2);
//    osal_task_create("create_task", vStartTasks, 1024, 10, NULL);
//    osal_task_start();
//    for (;;);     
	/*< Never perform here */
		
		ECG_Init();
    while (1)
    {
        /* ECG模块轮询处理 */
        app_ecg_poll();
    }
}

