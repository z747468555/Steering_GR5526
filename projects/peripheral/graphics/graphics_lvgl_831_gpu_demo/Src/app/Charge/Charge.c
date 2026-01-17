#include "Charge.h"
#include "IIC.h"
#include "app_log.h"
#include "Light.h"

void CHARGE_INT_Handler(app_io_evt_t *p_evt);

const app_gpiote_param_t Charge_param = {
    .type = CHARGE_INT_IO_TYPE,
    .pin  = CHARGE_INT_IO_PIN,
    .mode = APP_IO_MODE_IT_FALLING,        
    .pull = APP_IO_PULLUP,            
    .io_evt_cb = CHARGE_INT_Handler
};

void CHARGE_INT_Handler(app_io_evt_t *p_evt)
{
    //SEGGER_RTT_printf(0, "Enter The Charge Int!!!!\r\n");
}


void Charge_Init(void)
{
	uint8_t wdata[3];
	wdata[0] = 0x0B;
	wdata[1] = 0x80;
	IIC_Write(CHARGE_IIC_ID, CHARGE_IIC_ADD, wdata, 2);
	delay_ms(100);
    
    app_gpiote_config(&Charge_param);

    delay_ms(100);
	
    //WD_RST = 1 CHG_CONFIG = 1
    wdata[0] = 0x01;
    wdata[1] = 0x5A;
    IIC_Write(CHARGE_IIC_ID, CHARGE_IIC_ADD, wdata, 2);
	
    //设置充电电压为4.35V
	wdata[0] = 0x04;
	wdata[1] = 0x78;
	IIC_Write(CHARGE_IIC_ID, CHARGE_IIC_ADD, wdata, 2);
	
    //关闭看门狗
	wdata[0] = 0x05;
	wdata[1] = 0x8F;
	IIC_Write(CHARGE_IIC_ID, CHARGE_IIC_ADD, wdata, 2);
	

	Set_Charge_Cur(720);
	
}

void Charge_stat_set(bool sta)
{
	uint8_t wdata[3];
	if(sta)
	{
		wdata[0] = 0x00;
		wdata[1] = 0x17;
		IIC_Write(CHARGE_IIC_ID, CHARGE_IIC_ADD, wdata, 2);
	}
	else
	{
		wdata[0] = 0x00;
		wdata[1] = 0x77;
		IIC_Write(CHARGE_IIC_ID, CHARGE_IIC_ADD, wdata, 2);
	}
}

uint8_t Get_Charge_Status(void)
{
    uint8_t status, reg_data, wdata;
    wdata = 0x08;
    IIC_Write(CHARGE_IIC_ID, CHARGE_IIC_ADD, &wdata, 1);
    IIC_Read(CHARGE_IIC_ID, CHARGE_IIC_ADD, &reg_data, 1);
    status = (reg_data & 0x18) >> 3;
	return status;
}

void Get_Charge_fault(void)
{
    uint8_t reg_data, wdata;
    wdata = 0x09;
    IIC_Write(CHARGE_IIC_ID, CHARGE_IIC_ADD, &wdata, 1);
    IIC_Read(CHARGE_IIC_ID, CHARGE_IIC_ADD, &reg_data, 1);
    delay_ms(10);
    IIC_Write(CHARGE_IIC_ID, CHARGE_IIC_ADD, &wdata, 1);
    IIC_Read(CHARGE_IIC_ID, CHARGE_IIC_ADD, &reg_data, 1);
    APP_LOG_INFO( "Charge fault:0x%x\r\n", reg_data);
}


//获取当前电量	3v - 4.35V
void Get_Charge_Num(void)
{
	double volt;
	uint8_t num;
	volt = app_adc_get_volt(USER_ADC_BAT) * 2.8 / 1.8;
	num = (uint8_t)((volt - 3.0) / 1.35 * 100);
	APP_LOG_INFO( "Charge_Num:%d%\r\n", num);
}

//获取当前电量	3v - 4.35V	v*2.8/1.8
uint8_t charge_get_power()
{
	double volt;
	uint16_t num1,num2;
	uint8_t num;
	volt = app_adc_get_volt(USER_ADC_BAT);
	APP_LOG_INFO( "V_power=%f", volt);
	volt = volt * 14 / 9;
	if(volt > 4.25)
	{
		num = 100;
	}
	else if(volt > 3.3 && volt < 4.25)  //100%-60%
	{
		num2 = (uint16_t)(volt*100);
		num = num2-325;
	}
	else if(volt < 3.3)
	{
		num =1;
	}
	return num;
}

/***
 * 功能：设置充电电流大小
 * 参数：
 *      mA：电流大小，mA为单位
 * 
 * ***/
void Set_Charge_Cur(uint16_t mA)
{
    uint8_t data, reg_data, wdata[2];
    if(mA > 3000)
        mA = 3000;
    data = mA / 60;
    wdata[0] = REG02;
    IIC_Write(CHARGE_IIC_ID, CHARGE_IIC_ADD, wdata, 1);
    IIC_Read(CHARGE_IIC_ID, CHARGE_IIC_ADD, &reg_data, 1);
    wdata[1] = (reg_data & 0xC0) + (data & 0x3F);
    IIC_Write(CHARGE_IIC_ID, CHARGE_IIC_ADD, wdata, 2);
}

//进入船模式
void Enter_Ship_Mode(void)
{
	uint8_t wdata[2];
	wdata[0] = 0x07;
	wdata[1] = 0x64;
	IIC_Write(CHARGE_IIC_ID, CHARGE_IIC_ADD, wdata, 2);
}

long Charge_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString)
{
	printf("Set Charge Current:60mA");
	Set_Charge_Cur(60);
	Get_Charge_Num();
	return 0;
}
