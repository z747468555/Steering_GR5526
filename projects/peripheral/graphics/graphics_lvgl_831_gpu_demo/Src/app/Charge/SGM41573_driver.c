#include "SGM41573.h"




uint8_t SL_SGM41573_deviceID_Test(void)
{
    uint8_t SL_Read_Reg=0xff;
	  SL_SGM41573_I2c_Read(SGM41573_REG_DEVICE_ADDRESS, 1, &SL_Read_Reg);
    if(SL_Read_Reg==SGM41573_DEVICE_ID)   return  1;
    else                                    return -1;
}


/***************重载内部寄存器值*********************/
signed char SL_SGM41573_RESET(void)
{
    unsigned char SL_Read_Reg=0xff;
	  SL_SGM41573_I2c_Read(SGM41573_REG_CHARGE_OPTION_3H, 1, &SL_Read_Reg);
	  SL_Read_Reg=SL_SGM41573_RESET_ENABLE|SL_Read_Reg;
	  SL_SGM41573_I2c_Write_Onereg(SGM41573_REG_CHARGE_OPTION_3H,SL_Read_Reg);
    return  1;
}

//获取输入端vbus电压:mv
 int sgm41573_get_vbus_adc(void)
{		
	int vbus_adc;	
	unsigned char SL_Read_Reg=0xff;
	SL_SGM41573_I2c_Read(0x27, 1, &SL_Read_Reg);

	vbus_adc = SL_Read_Reg * 64 + 3200;
	printf("输入端vbus电压:%d mV\r\n",vbus_adc);
	return vbus_adc ;
}

//获取电池端VBAT电压：mv
 int sgm41573_get_vbat_adc(void)
{
	int vbat;
		
	unsigned char SL_Read_Reg=0xff;
	SL_SGM41573_I2c_Read(0x2C, 1, &SL_Read_Reg);
	
	vbat = SL_Read_Reg * 64 + 2880;
	printf("电池端VBAT电压:%d mV\r\n",vbat);
	return vbat;
}


//获取电池充电电流:mA
 int sgm41573_get_ichg_adc(void)
{	
	int ichg;
	unsigned char SL_Read_Reg=0xff;
	SL_SGM41573_I2c_Read(0x29, 1, &SL_Read_Reg);	
	
	ichg = SL_Read_Reg * 64 ; 
	printf("电池充电电流:%d mV\r\n",ichg);
	return ichg;	
}

//DEBUG：获取状态寄存器信息
 int sgm41573_get_ChargerStatus(void)
{	
	int ChargerStatus;	
  unsigned char Status1_Reg=0xff;
	unsigned char Status2_Reg=0xff;
	
	SL_SGM41573_I2c_Read(0x20, 1, &Status1_Reg);
	SL_SGM41573_I2c_Read(0x21, 1, &Status2_Reg);		
	printf("0x20寄存器:0x%x ,0x21寄存器:0x%x \r\n",Status1_Reg,Status2_Reg);	
	
	return ChargerStatus;
}

