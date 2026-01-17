#include "HP5804.h"
#include "app_i2c.h"
#include "SEGGER_RTT.h"
#include "IIC.h"
#include "FreeRTOS_CLI.h"
#define APP_LOG_TAG "hp"
#include "app_log.h"


float HP_Read_Temp()
{
	uint8_t wdata, rdata[3] = {0, 0, 0};
	uint32_t result;
	float ret;
	
	wdata = HP_T_4096;
	IIC_Write(HP_IIC_ID, HP5804_ADDRESS, &wdata, 1);
	wdata = HP_READ_T;
	IIC_Write(HP_IIC_ID, HP5804_ADDRESS, &wdata, 1);
	IIC_Read(HP_IIC_ID, HP5804_ADDRESS, rdata, 3);
	
	result = rdata[0];
	result <<= 8;
	result |= rdata[1];
	result <<= 8;
	result |= rdata[2];
	
	ret = result;
	
	ret = ret/100;
	
	APP_LOG_INFO("HP Tem: %f", ret);
	
	return ret;
}

long HP_Read_Temp_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString)
{
	float result;

	result = HP_Read_Temp();
	
	APP_LOG_INFO( "Get Pressure: %f", result);
	return 0;
}

float HP_Read_Pres(void)
{
	uint8_t wdata, rdata[3];
	uint32_t result;
	float ret;
	wdata = HP_P_4096;
	IIC_Write_sync(HP_IIC_ID, HP5804_ADDRESS, &wdata, 1);
	wdata = HP_READ_P;
	IIC_Write_sync(HP_IIC_ID, HP5804_ADDRESS, &wdata, 1);
	IIC_Read_sync(HP_IIC_ID, HP5804_ADDRESS, rdata, 3);
	
	result = rdata[0];
	result <<= 8;
	result |= rdata[1];
	result <<= 8;
	result |= rdata[2];
	
	ret = result/1000;
	
	APP_LOG_INFO( "Get Pressure: %d.%d", result / 100, result % 100);
	return ret;
}

long HP_Read_Pres_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString)
{
	uint32_t result;

	result = HP_Read_Pres();
	
	APP_LOG_INFO( "Get Pressure: %d", (int32_t)result);
	return 0;
}

void HP_enter_lowper(void)
{
	uint8_t wdata = 6;
	IIC_Write_sync(HP_IIC_ID, HP5804_ADDRESS, &wdata, 1);
}

