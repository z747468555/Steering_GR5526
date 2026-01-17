/**
 *****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief  User Periph Init Function Implementation.
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
#include "user_periph_setup.h"
#include "grx_sys.h"
#include "board_SK.h"
#include "drv_adapter_port.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "QMI8658A.h"
#include "HP5804.h"
#include "Charge.h"
#include "nst112c-cwlr_drive.h"
#include "light.h"
#include "Motor.h"
#include "LED.h"
#include "IIC.h"
#include "gh_demo.h"
#include "user_qspi_flash.h"
#include "gh_demo.h"

void vRegisterSampleCLICommands( void );
void vUARTCommandConsoleStart( uint16_t usStackSize,
                               unsigned int uxPriority );

static long task_info_cb( char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString );


static const CLI_Command_Definition_t qma8658a_test =
{
    "six-axis-test",        /* The command string to type. */
    "\r\ntask-stats:\r\n Displays a table showing the state of each FreeRTOS task\r\n",
    QMI_Test, /* The function to run. */
    0                    /* No parameters are expected. */
};


static const CLI_Command_Definition_t User_HP_Test =
{
    "HP-Test",        /* The command string to type. */
    "\r HP-test:\r\n Print an air pressure data\r\n",
    HP_Read_Pres_Test, /* The function to run. */
    0                    /* No parameters are expected. */
};
//ÉèÖÃ³äµçµçÁ÷Îª60mA, »ñÈ¡µ±Ç°µçÁ¿

static const CLI_Command_Definition_t User_Charge_Test =
{
    "Charge-Test",        /* The command string to type. */
    "\r Charge-test:\r\n Set Charge Current, Get Charge Num\r\n",
    Charge_Test, /* The function to run. */
    0                    /* No parameters are expected. */
};

//»ñÈ¡µ±Ç°ÎÂ¶È
static const CLI_Command_Definition_t User_Temp_Test =
{
    "Temp-Test",        /* The command string to type. */
    "\r Temp-test:\r\n Get Temp Num\r\n",
    Temp_Test, /* The function to run. */
    0                    /* No parameters are expected. */
};

//»ñÈ¡µ±Ç°¹âÕÕÖµ£¨µçÑ¹£©
static const CLI_Command_Definition_t User_Light_Test =
{
    "Light-Test",        /* The command string to type. */
    "\r Temp-test:\r\n Get Light Num\r\n",
    Light_Test, /* The function to run. */
    0                    /* No parameters are expected. */
};

//Ê¹Âí´ïÕñ¶¯1s
static const CLI_Command_Definition_t User_Motor_Test =
{
    "Motor-Test",        /* The command string to type. */
    "\r Temp-test:\r\n Motor wenwen 1s\r\n",
    Motor_Test, /* The function to run. */
    0                    /* No parameters are expected. */
};

//LEDÁÁÆð1s
static const CLI_Command_Definition_t User_LED_Test =
{
    "LED-Test",        /* The command string to type. */
    "\r LED-test:\r\n LED light 1s\r\n",
    LED_Test, /* The function to run. */
    0                    /* No parameters are expected. */
};

////´®¿Ú»Ø»·²âÊÔ
//static const CLI_Command_Definition_t User_UART_Test =
//{
//    "UART-Test",        /* The command string to type. */
//    "\r UART-test:\r\n UART Get 5 chars and Send back\r\n",
//    UART_Test, /* The function to run. */
//    0                    /* No parameters are expected. */
//};


static const CLI_Command_Definition_t task_info =
{
    "ps",        /* The command string to type. */
    "\r task info\r\n task info\r\n",
    task_info_cb, /* The function to run. */
    0                    /* No parameters are expected. */
};




static long task_info_cb( char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString )
{
    // 任务信息打印
    char *buffer = pvPortMalloc(2048);
    if (buffer == NULL) {
        return 0 ;
    }
    // 获取任务列表字符串
    vTaskList((char *)buffer);
    // 打印任务列表
    printf("Task\tState\tPriority\tStack\tNum\r\n");
    printf("%s\r\n", buffer);

    vTaskGetRunTimeStats(buffer);
	uint32_t sys_cpu_usage(void);
    printf("Task\tRunTime\tCPU(%d%%)\r\n", sys_cpu_usage());
    printf("%s\r\n", buffer);
    // 释放缓冲区
    vPortFree(buffer);

    // heap信息打印
    printf("Heap free:%d min_free:%d\r\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
    return 0;
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
void app_periph_init(void)
{
    board_init();

//	IIC_Init();
//	LED_Init();
//	Motor_Init();
	//UART_Init();
    //Gh3x2xDemoInit();
//	FreeRTOS_CLIRegisterCommand(&qma8658a_test);
//	FreeRTOS_CLIRegisterCommand(&User_HP_Test);
//	FreeRTOS_CLIRegisterCommand(&User_Charge_Test);
//	FreeRTOS_CLIRegisterCommand(&User_Temp_Test);
//	FreeRTOS_CLIRegisterCommand(&User_Light_Test);
//	FreeRTOS_CLIRegisterCommand(&User_Motor_Test);
//	FreeRTOS_CLIRegisterCommand(&User_LED_Test);
	//FreeRTOS_CLIRegisterCommand(&User_UART_Test);
//    FreeRTOS_CLIRegisterCommand(&task_info);
//	vRegisterSampleCLICommands(); 
//	vUARTCommandConsoleStart(1000,200);
	
//    drv_adapter_disp_register();
    drv_adapter_norflash_register();
//    drv_adapter_touchpad_register();
	
//    pwr_mgmt_mode_set(PMR_MGMT_SLEEP_MODE);
}

