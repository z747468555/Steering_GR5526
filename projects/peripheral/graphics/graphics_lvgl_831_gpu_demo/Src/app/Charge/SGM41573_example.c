#include "SGM41573.h"


///***************1s电池充电示例*********/

void SGM41573_Battery_chargingtest()
{
	unsigned char SL_Read_Reg=0xff;	
	//1.初始化通信接口
	SL_SGM41573_I2c_Init();
	
	//2.检查是否能读取芯片DEVICE_ID
	SL_SGM41573_deviceID_Test();
	
	//3.复位芯片内部寄存器
	SL_SGM41573_RESET();
	SL_SGM41573_Delay_Ms(1000);
	SL_SGM41573_I2c_Read(SGM41573_REG_CHARGE_CURRENT_L, 2, &SL_Read_Reg);	
			
	//4.禁用看门狗以及开关频率设置：800K（需要结合硬件电感而定）
	SL_SGM41573_I2c_Write_Onereg(0x01,0x87);
	SL_SGM41573_I2c_Read(0x01, 1, &SL_Read_Reg);	
	//禁用AUTO_WAKEUP_EN
	SL_SGM41573_I2c_Write_tworeg(0x30,0x1002);
	SL_SGM41573_I2c_Read(0x30, 2, &SL_Read_Reg);	
	
  //5.输入电压限制:4608mv+3200mv=7808mv----原厂推荐[必须16位写入]
  SL_SGM41573_I2c_Write_tworeg(0x0A,0x0012);
	SL_SGM41573_I2c_Read(0x0A, 2, &SL_Read_Reg);	
	
	//6.输入电流限制----6.350A
	//禁用ILIM_HIZ，输入电流限制由 REG0x0F/0E 设置。
	SL_SGM41573_I2c_Write_Onereg(0x32,0x37);
	//电流限制6.350A
	SL_SGM41573_I2c_Write_tworeg(0x0E,0x007f);	
	SL_SGM41573_I2c_Read(0x0E, 2, &SL_Read_Reg);	
			
  //7,电池充电配置
  //最小系统电压设置：3584mV（1s要求----手册要求）
  //SL_SGM41573_I2c_Write_tworeg(0x0C,0x000E);	

	//充电电压设置:4200mV
	SL_SGM41573_I2c_Write_tworeg(0x04,0x6810);
	SL_SGM41573_I2c_Read(0x04, 2, &SL_Read_Reg);	
		
	//充电电流设置：5152mA
	SL_SGM41573_I2c_Write_tworeg(0x02,0x0014);
	SL_SGM41573_I2c_Read(0x02, 2, &SL_Read_Reg);	

  //8.开启内部ADC监测----连续转换，2.04V[1S]
  SL_SGM41573_I2c_Write_tworeg(0x3A,0xFF80);
	SL_SGM41573_I2c_Read(0x3A, 2, &SL_Read_Reg);	

	while(1)
	{	
	SL_SGM41573_Delay_Ms(500);			
	sgm41573_get_vbus_adc();			
	sgm41573_get_vbat_adc();
	sgm41573_get_ichg_adc();
  sgm41573_get_ChargerStatus();		
	}
	
}

