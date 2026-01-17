#ifndef __SGM41573_H__
#define __SGM41573_H__

//add
#include <stdio.h>
#include <string.h>
#include <stdint.h>



#define SGM41573_MANUFACTURER	"sgmicro"
#define SGM41573_NAME		    "sgm41573"
#define SGM41573_DEV_ID		    0x88
/******************* define ********************/


#define SGM41573_DEVICE_ADDRESS_8BIT	    0xD6	// 从机设备地址
#define SGM41573_DEVICE_ID                0x88  //SGM41573设备ID



#define SGM41573_MAX_CHG_CURR_uA             8128000
#define SGM41573_MAX_CHG_CURR_STEP_uV        64000


//#define SGM41573_1CELL_MAX_CHG_VOLT_uV       4200000
//#define SGM41573_2CELL_MAX_CHG_VOLT_uV       8400000
//#define SGM41573_3CELL_MAX_CHG_VOLT_uV       12600000

#define SGM41573_4CELL_MAX_CHG_VOLT_uV       19200000
#define SGM41573_MAX_CHG_VOLT_STEP_uV        8000
#define SGM41573_MAX_CHG_VOLT_OFFSET_uV      1024000

#define SGM41573_MIN_VSYS_MAX_uV             16128000
#define SGM41573_MIN_VSYS_OFFSET_uV          1024000
#define SGM41573_MIN_VSYS_STEP_uV            256000

#define SGM41573_ILIM_MAX_uA             6350000
#define SGM41573_ILIM_OFFSET_uA          50000
#define SGM41573_ILIM_STEP_uA            50000

#define SGM41573_VLIM_MAX_uV             19520000
#define SGM41573_VLIM_OFFSET_uV          3200000
#define SGM41573_VLIM_STEP_uV            64000

#define SGM41573_OTG_VOLT_MAX_HIGH_uV        20560000
#define SGM41573_OTG_VOLT_MAX_LOW_uV         19280000
#define SGM41573_OTG_VOLT_MIN_HIGH_uV        4280000
#define SGM41573_OTG_VOLT_MIN_LOW_uV         3000000
#define SGM41573_OTG_VOLT_OFFSET_uV          1280000
#define SGM41573_OTG_VOLT_STEP_uV            8000

#define SGM41573_OTG_CURR_MAX_uA         6350000
#define SGM41573_OTG_CURR_STEP_uA        50000



#define SL_SGM41573_RESET_ENABLE         0x60


//寄存器部分
//充电选项
#define SGM41573_REG_CHARGE_OPTION_0L				0x00
#define SGM41573_REG_CHARGE_OPTION_0H				0x01

#define SGM41573_REG_CHARGE_OPTION_1L				0x30
#define SGM41573_REG_CHARGE_OPTION_1H				0x31

#define SGM41573_REG_CHARGE_OPTION_2L				0x32
#define SGM41573_REG_CHARGE_OPTION_2H				0x33

#define SGM41573_REG_CHARGE_OPTION_3L				0x34
#define SGM41573_REG_CHARGE_OPTION_3H				0x35
//7位充电电流设置
#define SGM41573_REG_CHARGE_CURRENT_H				    0x03
#define SGM41573_REG_CHARGE_CURRENT_L				    0x02
//12位充电电压设置
#define SGM41573_REG_MAX_CHARGE_VOLTAGE_H			    0x05
#define SGM41573_REG_MAX_CHARGE_VOLTAGE_L			    0x04
//PROCHOT选项
#define SGM41573_REG_PROCHOT_OPTION_0			    0x36
#define SGM41573_REG_PROCHOT_OPTION_1			    0x38
//ADC选项
#define SGM41573_REG_ADC_OPTION					    0x3A
//充电器状态
#define SGM41573_REG_CHARGER_STATUS				    0x20
//Prochot状态
#define SGM41573_REG_PROCHOT_STATUS				    0x22
//7位输入电流限制
#define SGM41573_REG_IIN_DPM						0x24

#define SGM41573_REG_ADC_VBUS_PSYS				    0x26
#define SGM41573_REG_ADC_IBAT					    0x28
#define SGM41573_REG_ADC_CMPIN_IIN				    0x2A
#define SGM41573_REG_ADC_VSYS_VBAT				    0x2C
#define SGM41573_REG_OTG_VOLTAGE					0x06
#define SGM41573_REG_OTG_CURRENT					0x08

#define SGM41573_REG_INPUT_VOLTAGE_H				    0x0B
#define SGM41573_REG_INPUT_VOLTAGE_L				    0x0A

#define SGM41573_REG_MIN_SYSTEM_VOLTAGE_H			    0x0D
#define SGM41573_REG_MIN_SYSTEM_VOLTAGE_L			    0x0C

#define SGM41573_REG_IIN_HOST_H					    0x0F
#define SGM41573_REG_IIN_HOST_L					    0x0E

#define SGM41573_REG_MANUFACTURER_ID				0x2E
#define SGM41573_REG_DEVICE_ADDRESS				    0x2F



/********客户需要进行的IIC/SPI接口封包函数****************/

extern void	SL_SGM41573_Delay_Ms(uint16_t ms);
extern unsigned char SL_SGM41573_I2c_Write_tworeg(uint8_t reg, uint16_t data);
extern unsigned char SL_SGM41573_I2c_Write_Onereg(uint8_t reg, uint8_t data);
extern unsigned char SL_SGM41573_I2c_Read(uint8_t reg, uint8_t len, uint8_t *buf);
extern unsigned char SL_SGM41573_I2c_Init();	
/********驱动库API****************/
uint8_t BQ25713_Init ( void );
uint8_t SL_SGM41573_deviceID_Test(void);
signed char SL_SGM41573_RESET(void);

//获取输入端vbus电压:mv
 int sgm41573_get_vbus_adc(void);
//获取电池端VBAT电压：mv
 int sgm41573_get_vbat_adc(void);
//获取电池充电电流:mA
 int sgm41573_get_ichg_adc(void);

 int sgm41573_get_ChargerStatus(void);


//测试例程
void SGM41573_Battery_charging();

void SGM41573_Battery_chargingtest();
	
#endif
