#ifndef __QMI8568A_H__
#define __QMI8568A_H__

#include <stdio.h>
#include <string.h>
#include "app_io.h"
/**
 ****************************************************************************************
 * @brief  port接口定义
 ****************************************************************************************
 */
 
#define QMI8658A_SPI_I2C_SEL	1		
/**CS输入低电平 选择  Spi:		0**/
/**CS输入高电平 选择  I2c:		1**/

#define QMI8658A_SLAVE_ADDR			0x6A
/**I2c模式下 SA0输入高电平 I2c 7bit地址:	0x6a**/
/**I2c模式下 SA0输入低电平 I2c 7bit地址:	0x6b**/

/**I2c模式下 addr bit7置1使能寄存器地址累加功能**/
#define QMI_IIC_ID							APP_I2C_ID_3
#define QMI_IIC_SCL_PIN_TYPE				APP_IO_TYPE_GPIOB
#define QMI_IIC_SDA_PIN_TYPE				APP_IO_TYPE_GPIOB
#define QMI_IIC_SCL_PIN_NUM					APP_IO_PIN_14				//GPIO30
#define QMI_IIC_SDA_PIN_NUM					APP_IO_PIN_15				//GPIO31
#define QMI_IIC_SCL_PIN_MUX					APP_IO_MUX_3
#define QMI_IIC_SDA_PIN_MUX					APP_IO_MUX_3


/****************** 用户需要实现的接口函数 *******************/
#if QMI8658A_SPI_I2C_SEL
unsigned char qmi8658a_I2c_Init(void);
unsigned char qmi8658a_I2c_Write(unsigned char slave_addr,unsigned char reg, unsigned char* data,unsigned char length);
unsigned char qmi8658a_I2c_Read(unsigned char slave_addr,unsigned char reg, unsigned char* data, unsigned char length);
#else
unsigned char qmi8658a_Spi_Init(void);
unsigned char qmi8658a_Spi_Write(unsigned char reg, unsigned char* data,unsigned char length);
unsigned char qmi8658a_Spi_Read(unsigned char reg, unsigned char* data, unsigned char length);
#endif
void	qmi8658a_delay_ms(unsigned long int ms);

void qmi8658a_enter_low_power(void);

/**
 ****************************************************************************************
 * @brief  driver接口定义
 ****************************************************************************************
 */
 
/****************** macro define *******************/
#define WhoAmI_Value  0x05
#define Revison_Vlaue 0x7c
#define Reset_Vlaue 	0xb2

typedef enum qmi8658a_Status
{
	Qmi_Disable = 0,
	Qmi_Enable
}qmi_Status;

typedef enum qmi8658a_Error_Code
{
	Qmi_Success = 0,
	Qmi_Error
}qmi_ErrCode;

typedef enum qmi8658aRegister
{
	/**< General Purpose Registers */
	qmi8658aRegister_WhoAmI = 0,
	qmi8658aRegister_Revision,
	
	/**< Setup and Control Registers */
	qmi8658aRegister_Ctrl1,
	qmi8658aRegister_Ctrl2,
	qmi8658aRegister_Ctrl3,
	qmi8658aRegister_Ctrl4,
	qmi8658aRegister_Ctrl5,
	qmi8658aRegister_Ctrl6,
	qmi8658aRegister_Ctrl7,
	qmi8658aRegister_Ctrl8,
	qmi8658aRegister_Ctrl9,
	
	/**< Host Controlled Calibration Registers */
	qmi8658aRegister_Cal1_L = 11,
	qmi8658aRegister_Cal1_H,
	qmi8658aRegister_Cal2_L,
	qmi8658aRegister_Cal2_H,
	qmi8658aRegister_Cal3_L,
	qmi8658aRegister_Cal3_H,
	qmi8658aRegister_Cal4_L,
	qmi8658aRegister_Cal4_H,
	
	/**< FIFO Registers */
	qmi8658aRegister_FifoWmkTh = 19, 
	qmi8658aRegister_FifoCtrl = 20,
	qmi8658aRegister_FifoCount = 21,
	qmi8658aRegister_FifoStatus = 22,
	qmi8658aRegister_FifoData = 23,
	
	/**< Status Registers */
	qmi8658aRegister_StatusInt = 45,
	qmi8658aRegister_Status0,
	qmi8658aRegister_Status1,
	qmi8658aRegister_Timestamp_L = 48,
	
	/**< Timestamp Register */
	qmi8658aRegister_Timestamp_M,
	qmi8658aRegister_Timestamp_H,
	qmi8658aRegister_Tempearture_L = 51,
	qmi8658aRegister_Tempearture_H,
	
	/**< Data Output Registers */
	qmi8658aRegister_Ax_L = 53,
	qmi8658aRegister_Ax_H,
	qmi8658aRegister_Ay_L,
	qmi8658aRegister_Ay_H,
	qmi8658aRegister_Az_L,
	qmi8658aRegister_Az_H,
	qmi8658aRegister_Gx_L = 59,
	qmi8658aRegister_Gx_H,
	qmi8658aRegister_Gy_L,
	qmi8658aRegister_Gy_H,
	qmi8658aRegister_Gz_L,
	qmi8658aRegister_Gz_H,
	qmi8658aRegister_Mx_L = 65,
	qmi8658aRegister_Mx_H,
	qmi8658aRegister_My_L,
	qmi8658aRegister_My_H,
	qmi8658aRegister_Mz_L,
	qmi8658aRegister_Mz_H,
	qmi8658aRegister_Q1_L = 73,
	qmi8658aRegister_Q1_H,
	qmi8658aRegister_Q2_L,
	qmi8658aRegister_Q2_H,
	qmi8658aRegister_Q3_L,
	qmi8658aRegister_Q3_H,
	qmi8658aRegister_Q4_L,
	qmi8658aRegister_Q4_H,
	qmi8658aRegister_Dvx_L = 81,
	qmi8658aRegister_Dvx_H,
	qmi8658aRegister_Dvy_L,
	qmi8658aRegister_Dvy_H,
	qmi8658aRegister_Dvz_L,
	qmi8658aRegister_Dvz_H,
	qmi8658aRegister_AeReg1 = 87,
	qmi8658aRegister_AeOverflow,

    Qmi8658Register_Tap_Status = 89,
    Qmi8658Register_Pedo_L = 90,
    Qmi8658Register_Pedo_M = 91,
    Qmi8658Register_Pedo_H = 92,
	/**< Reset Register */
	qmi8658aRegister_Reset = 96
}qmi8658a_Reg;

/****************** CTRL1 Serial Interface and Sensor Enable. Register Address: 2 (0x02) *******************/
#define CTRL1_SIM				7
#define CTRL1_ADDR_AI		6
#define CTRL1_BE				5
#define CTRL1_INT2_EN		4
#define CTRL1_INT1_EN		3
#define CTRL1_Osc_EN		0

typedef enum qmi8658a_Spi_Mode
{
	Enable_4_Wire_Spi,
	Enable_3_Wire_Spi
}qmi8658a_SIM;/**< I2c模式下该参数无意义 */

typedef enum qmi8658a_Address_Auto_Increment
{
	Addr_No_Auto_Increment,
	Addr_Auto_Increment
}qmi8658a_ADDR_AI;

typedef enum qmi8658a_Endian
{
	Little_Endian,
	Big_Endian			/**<default  */
}qmi8658a_BE;

typedef enum qmi8658a_INT1_Enable
{
	INT1_Disable,	/**<default  */
	INT1_Enable			
}qmi8658a_INT1_En;

typedef enum qmi8658a_INT2_Enable
{
	INT2_Disable,	/**<default  */
	INT2_Enable			
}qmi8658a_INT2_En;

typedef enum qmi8658a_Internal_2MHz_oscillator_Enable
{
	Enable_Internal_Osc,
	Disable_Internal_Osc 
}qmi8658a_Sensor_Osc_En;

/****************** CTRL2 Accelerometer Settings: Address: 3 (0x03) *******************/
#define CTRL2_aST		7
#define CTRL2_aFs		4
#define CTRL2_aODR	0

typedef enum qmi8658a_Self_Test_Enable
{
	Self_Test_Disable,
	Self_Test_Enable
}qmi8658a_ST_En;

typedef enum qmi8658a_Accelerometer_Range
{
	qmi8658aAccRange_2g 	= 0x00,
	qmi8658aAccRange_4g 	= 0x01,
	qmi8658aAccRange_8g 	= 0x02,
	qmi8658aAccRange_16g  = 0x03
}qmi8658a_Acc_Range;

typedef enum qmi8658a_Accelerometer_Odr
{
	qmi8658aAccOdr_8000Hz 						= 0x00,
	qmi8658aAccOdr_4000Hz 						= 0x01,
	qmi8658aAccOdr_2000Hz 						= 0x02,
	qmi8658aAccOdr_1000Hz 						= 0x03,
	qmi8658aAccOdr_500Hz 							= 0x04,
	qmi8658aAccOdr_250Hz 							= 0x05,
	qmi8658aAccOdr_125Hz 							= 0x06,
	qmi8658aAccOdr_62_5Hz 						= 0x07,
	qmi8658aAccOdr_31_25Hz 						= 0x08,
	qmi8658aAccOdr_LowPower_128Hz 		= 0x0c,
	qmi8658aAccOdr_LowPower_21Hz 			= 0x0d,
	qmi8658aAccOdr_LowPower_11Hz 			= 0x0e,
	qmi8658aAccOdr_LowPower_3Hz 			= 0x0f
}qmi8658a_Acc_Odr;
/**< 同时使能加速度计和陀螺仪时Lowpower模式无效，且ORD以加速度计为准，具体请看手册对应说明 */

/****************** CTRL3 Gyroscope Settings: Address 4 (0x04) *******************/
#define CTRL3_gST		7
#define CTRL3_gFs		4
#define CTRL3_gODR	0

typedef enum qmi8658a_Gyroscope_Range
{	
	qmi8658aGyrRange_16dps 	 = 0,
	qmi8658aGyrRange_32dps 	 = 1,
	qmi8658aGyrRange_64dps 	 = 2,
	qmi8658aGyrRange_128dps  = 3,
	qmi8658aGyrRange_256dps  = 4,
	qmi8658aGyrRange_512dps  = 5,
	qmi8658aGyrRange_1024dps = 6,
	qmi8658aGyrRange_2048dps = 7
}qmi8658a_Gyr_Range;

typedef enum qmi8658a_Gyroscope_Odr
{
	qmi8658aGyrOdr_8000Hz   = 0x00,
	qmi8658aGyrOdr_4000Hz   = 0x01,
	qmi8658aGyrOdr_2000Hz   = 0x02,
	qmi8658aGyrOdr_1000Hz   = 0x03,
	qmi8658aGyrOdr_500Hz		= 0x04,
	qmi8658aGyrOdr_250Hz		= 0x05,
	qmi8658aGyrOdr_125Hz		= 0x06,
	qmi8658aGyrOdr_62_5Hz 	= 0x07,
	qmi8658aGyrOdr_31_25Hz	= 0x08
}qmi8658a_Gyr_Odr;

/****************** CTRL4 Reserved – Special Settings. Register Address: 5 (0x05) *******************/

/****************** CTRL5 Sensor Data Processing Settings. Register Address: 6 (0x06) *******************/
#define CTRL5_gLPF_MODE	5
#define CTRL5_gLPF_EN		4	
#define CTRL5_aLPF_MODE	1
#define CTRL5_aLPF_EN		0	

typedef enum qmi8658a_Gyroscope_Lowpassfilter_Mode
{
	G_LSP_MODE_0 = 0x00,	/**< 2.66% of ODR */
	G_LSP_MODE_1 = 0x01,	/**< 3.63% of ODR */
	G_LSP_MODE_2 = 0x02,	/**< 5.39% of ODR */
	G_LSP_MODE_3 = 0x03		/**< 13.37% of ODR */
}qmi8658a_Gyr_Lpf_Mode;

typedef enum qmi8658a_Gyroscope_Lowpassfilter_Enable
{
	GYR_LPF_Disable,
	GYR_LPF_Enable
}qmi8658a_Gyr_Lpf_En;

typedef enum qmi8658a_Accelerometer_Lowpassfilter_Mode
{
	A_LSP_MODE_0 = 0x00,	/**< 2.66% of ODR */
	A_LSP_MODE_1 = 0x01,	/**< 3.63% of ODR */
	A_LSP_MODE_2 = 0x02,	/**< 5.39% of ODR */
	A_LSP_MODE_3 = 0x03 	/**< 13.37% of ODR */
}qmi8658a_Acc_Lpf_Mode;

typedef enum qmi8658a_Accelerometer_Lowpassfilter_Enable
{
	ACC_LPF_Disable,
	ACC_LPF_Enable	
}qmi8658a_Acc_Lpf_En;

/****************** CTRL6 Attitude Engine ODR and Motion on Demand: Address: 7 (0x07) *******************/
#define CTRL6_sMod	7
#define CTRL6_sORD	0

typedef enum qmi8658a_Motion_on_Demand_Enable
{
	MoD_Disable,
	MoD_Enable			/**< Requires sEN=1 */
}qmi8658a_MoD_En;

typedef enum qmi8658a_Attitude_Engine_Odr
{
	qmi8658aAeOdr_1Hz 	= 0x00,
	qmi8658aAeOdr_2Hz 	= 0x01,
	qmi8658aAeOdr_4Hz 	= 0x02,
	qmi8658aAeOdr_8Hz 	= 0x03,
	qmi8658aAeOdr_16Hz	= 0x04,
	qmi8658aAeOdr_32Hz	= 0x05,
	qmi8658aAeOdr_64Hz	= 0x06,
}qmi8658a_AE_Odr;

/****************** CTRL7 Enable Sensors and Configure Data Reads. Register Address: 8 (0x08) *******************/
#define CTRL7_syncSmpl	7
#define CTRL7_sys_hs		6
#define CTRL7_gSN				4
#define CTRL7_sEN				3
#define CTRL7_gEN				1
#define CTRL7_aEN				0

typedef enum qmi8658a_SyncSample_Enable
{
	SyncSmpl_Disable,
	SyncSmpl_Enable 
}qmi8658a_SyncSmpl_En;

typedef enum qmi8658a_High_Speed_Enable
{
	SysHS_Disable,	/**< Clock based on ODR */
	SysHS_Enable 		/**< High Speed Internal Clock */
}qmi8658a_SysHS_En;

typedef enum qmi8658a_Gyroscope_Snooze_Enable
{
	Snooze_MODE_Disable,	/**< Gyroscope in Full Mode (Drive and Sense are enabled). */
	Snooze_MODE_Enable			/**< Gyroscope in Snooze Mode (only Drive enabled). */
}qmi8658a_Gyr_SN_En;/**< This bit is effective only when gEN is set to 1. */

typedef enum qmi8658a_Attitude_Engine_Enable
{
	AE_Disable,
	AE_Enable			
}qmi8658a_AE_En;/**< This bit is effective only when gEN is set to 1. */

typedef enum qmi8658a_Gyroscope_Enable
{
	GYR_Disable,
	GYR_Enable	
}qmi8658a_Gyr_En;

typedef enum qmi8658a_Accelerometer_Enable
{
	ACC_Disable,
	ACC_Enable  
}qmi8658a_Acc_En;

/****************** CTRL8 Motion Detection Control. Register Address: 9 (0x09) *******************/
#define CTRL8_Handshaking_Type		7
#define CTRL8_INT_pin_for_MoD			6
#define CTRL8_Pedometer Enable		4
#define CTRL8_Sign_Motion_Enable	3
#define CTRL8_No_Motion_Enable		2
#define CTRL8_Any_Motion_Enable		1
#define CTRL8_Tap_Enable					0

typedef enum qmi8658a_CTRL9_Handshaking_type
{
	Use_INT1,		/**< use INT1 as CTRL9 handshake */
	Use_STATUS	/**< use STATUSINT.bit7 as CTRL9 handshake */
}qmi8658a_Crtl9_Hs_Type;

typedef enum qmi8658a_INT_For_Montion_Detection
{
	Use_INT1_For_MoD,	/**< INT1 is used for motion detection event interrupt */
	Use_INT2_For_MoD 	/**< INT2 is used for motion detection event interrupt */
}qmi8658a_MoD_INT_Type;/**< Note: this bit will influence the any/no/sig-motion, pedometer, tap detection interrupt */

typedef enum qmi8658a_Pedometer_Enable
{
	Pedometer_Disable,	
	Pedoeter_Enable 		
}qmi8658a_Pedo_En;

typedef enum qmi8658a_Significant_Motion_Enable
{
	Significant_Motion_Disable,	
	Significant_Motion_Enable 		
}qmi8658a_Sign_Mot_En;

typedef enum qmi8658a_NO_Motion_Enable
{
	No_Motion_Disable,	
	No_Motion_Enable 		
}qmi8658a_No_Mot_En;

typedef enum qmi8658a_Any_Motion_Enable
{
	Any_Motion_Disable,	
	Any_Motion_Enable 		
}qmi8658a_Any_Mot_En;

typedef enum qmi8658a_Tap_Enable
{
	Tap_Disable,	
	Tap_Enable 		
}qmi8658a_Tap_En;

/****************** CTRL9 Host Commands. Register Address: 10 (0x0A) *******************/
typedef enum 
{
	qmi8658a_Ctrl9_Cmd_NOP											= 0X00,
	qmi8658a_Ctrl9_Cmd_GyroBias								= 0X01,
	qmi8658a_Ctrl9_Cmd_Rqst_Sdi_Mod						= 0X03,
	qmi8658a_Ctrl9_Cmd_Rst_Fifo								= 0X04,
	qmi8658a_Ctrl9_Cmd_Req_Fifo								= 0X05,
	qmi8658a_Ctrl9_Cmd_I2CM_Write							= 0X06,
	qmi8658a_Ctrl9_Cmd_WoM_Setting							= 0x08,
	qmi8658a_Ctrl9_Cmd_AccelHostDeltaOffset		= 0x09,
	qmi8658a_Ctrl9_Cmd_GyroHostDeltaOffset			= 0x0A,
	qmi8658a_Ctrl9_Cmd_EnableExtReset					= 0x0B,
	qmi8658a_Ctrl9_Cmd_EnableTap								= 0x0C,	
	qmi8658a_Ctrl9_Cmd_EnablePedometer					= 0x0D,
	qmi8658a_Ctrl9_Cmd_Motion									= 0x0E,
	qmi8658a_Ctrl9_Cmd_CopyUsid								= 0x10,
	qmi8658a_Ctrl9_Cmd_SetRpu									= 0x11,
	qmi8658a_Ctrl9_Cmd_AHB_Clock_Gating				= 0x12,
	qmi8658a_Ctrl9_Cmd_On_Demand_Cali					= 0xA2,
	qmi8658a_Ctrl9_Cmd_Dbg_WoM_Data_Enable			= 0xF8
}qmi8658a_Ctrl9_Command;

/****************** FIFO_WTM_TH FIFO Watermark *******************/
//enum qmi8658a_Fifo_Watermark_Level
//{
//	qmi8658a_Fifo_WmkEmpty 		  		= (0 << 4),
//	qmi8658a_Fifo_WmkOneQuarter	 	  =	(1 << 4),
//	qmi8658a_Fifo_WmkHalf 						=	(2 << 4),
//	qmi8658a_Fifo_WmkThreeQuarters 	=	(3 << 4)
//}qmi8658a_WTM_lvl;

/****************** FIFO_CTRL FIFO Control Register Address: 20 (0x14) *******************/
#define CTRL_FIFO_RD_MODE		7
#define CTRL_FIFO_SIZE			2
#define CTRL_FIFO_MODE			0

typedef enum 
{
	qmi8658a_Fifo_Read_Disable 	= 	0,
	qmi8658a_Fifo_Read_Enable 	= 	1,
}qmi8658a_Fifo_rd_mode;	

typedef enum 
{
	qmi8658a_Fifo_16 	= 	0,
	qmi8658a_Fifo_32 	= 	1,
	qmi8658a_Fifo_64 	= 	2,
	qmi8658a_Fifo_128	= 	3
}qmi8658a_Fifo_Size;	

typedef enum 
{
	qmi8658a_Fifo_Bypass 			  = 0,	/**< Bypass (FIFO disable) */
	qmi8658a_Fifo_Fifo 			  	= 1,  /**< FIFO */
	qmi8658a_Fifo_Stream 			  = 2,	/**< Stream */
	qmi8658a_Fifo_StreamToFifo  = 3		/**< Stream to FIFO. In stream to FIFO mode, once motion/gesture interrupt event happens, content of FIFO will be emptied, pointers reset */
}qmi8658a_Fifo_Mode;

/****************** FIFO_SMPL_CNT *******************/
#define FIFO_SAMPLE_CNT_LSB	0xFF

/****************** FIFO_STATUS FIFO Status. Register Address 22 (0x16) *******************/
#define STATUS_FIFO_FULL_FLAG				(0x01<<7)
#define STATUS_FIFO_WTM_FLAG				(0x01<<6)
#define STATUS_FIFO_OVERFLOW_FLAG 	(0x01<<5)
#define STATUS_FIFO_NOT_EMPYT_FLAG 	(0x01<<4)

#define FIFO_SAMPLE_CNT_MSB					(0x03<<0)

/****************** STATUSINT Sensor Data Available and Lock Register Address: 45 (0x2D)*******************/
#define STATUS_Ctrl9_CmdDone	(0x01<<7)
#define STATUS_Locked					(0x01<<1)
#define STATUS_Avail					(0x01<<0)

/****************** STATUS0 Output Data Status Register Address: 46 (0x2E)*******************/
#define STATUS_sDA	(0x01<<3)
#define STATUS_gDA	(0x01<<1)
#define STATUS_aDA	(0x01<<0)

/****************** STATUS1 Miscellaneous Status. Register Address 47 (0x2F) *******************/
#define STATUS_Sign_Mot		(0x01<<7)
#define STATUS_No_Mot			(0x01<<6)
#define STATUS_Any_Mot		(0x01<<5)
#define STATUS_Pedometer	(0x01<<4)
#define STATUS_WoM				(0x01<<2)
#define STATUS_TAP				(0x01<<1)

/****************** Locking Mechanism *******************/
typedef enum qmi8658a_AHB_Clock_Gating_Enable
{
	qmi8658a_AHB_Disable, 
	qmi8658a_AHB_Enable 	
}qmi8658a_ACG_En;

typedef enum 
{
	qmi8658a_Int1_Low  = (0 << 6),
	qmi8658a_Int2_Low  = (1 << 6),
	qmi8658a_Int1_High = (2 << 6),
	qmi8658a_Int2_High = (3 << 6)
}qmi8658_WoM_Int_Type;

typedef enum 
{
	Qmi8658WomThreshold_off  = 0,
	Qmi8658WomThreshold_low  = 32,
	Qmi8658WomThreshold_mid = 128,
	Qmi8658WomThreshold_high = 255
}qmi8658a_WoM_Threshold;

/****************** fuction declaration *******************/
unsigned char qmi8658a_port_init(void);
unsigned char qmi8658a_read_reg(qmi8658a_Reg reg_addr, unsigned char* buf, unsigned char length);
unsigned char qmi8658a_write_reg(qmi8658a_Reg reg_addr, unsigned char* buf,unsigned char length);

unsigned char qmi8658a_check_WhoAmI(void);
unsigned char qmi8658a_check_Revison(void);
unsigned char qmi8658a_serial_interface_config(qmi8658a_SIM spi_mode,qmi8658a_ADDR_AI addr_ai,qmi8658a_BE endian);
unsigned char qmi8658a_int1_config(qmi8658a_INT1_En int1_en);
unsigned char qmi8658a_int2_config(qmi8658a_INT2_En int2_en);
unsigned char qmi8658a_sensor_osc_config(qmi8658a_Sensor_Osc_En osc_en);

unsigned char qmi8658a_readStatusInt(unsigned char* status_int);
unsigned char qmi8658a_readStatus0(unsigned char* status0);
unsigned char qmi8658a_readStatus1(unsigned char* status1);
unsigned char qmi8658a_readTemp(float* temp_f);
unsigned char qmi8658a_read_timestamp(unsigned int* timestamp);

unsigned char qmi8658a_acc_setting(qmi8658a_Acc_Range range, qmi8658a_Acc_Odr odr,qmi8658a_ST_En selftest);
unsigned char qmi8658a_gyr_setting(qmi8658a_Gyr_Range range, qmi8658a_Gyr_Odr odr,qmi8658a_ST_En selftest);
unsigned char qmi8658a_acc_lpf_config(qmi8658a_Acc_Lpf_Mode lpf_mode,qmi8658a_Acc_Lpf_En lpf_en);
unsigned char qmi8658a_gyr_lpf_config(qmi8658a_Gyr_Lpf_Mode lpf_mode,qmi8658a_Gyr_Lpf_En lpf_en);
unsigned char qmi8658a_acc_config(qmi8658a_Acc_En acc_en);
unsigned char qmi8658a_gyr_config(qmi8658a_Gyr_SN_En sn_en,qmi8658a_Gyr_En gyr_en);

unsigned char qmi8658a_read_acc_rawdata(signed short* xyz_rawdata);
unsigned char qmi8658a_read_gyr_rawdata(signed short* xyz_rawdata);
void qmi8658a_acc_rawdata_and_mg_convert(qmi8658a_Acc_Range acc_range,const signed short* rawdata,float* acc_value);
void qmi8658a_gyr_rawdata_and_dps_convert(qmi8658a_Gyr_Range gry_range,const signed short* rawdata,float* acc_value);

unsigned char qmi8658a_ae_setting(qmi8658a_AE_Odr ae_odr);
unsigned char qmi8658a_ae_config(qmi8658a_AE_En ae_en);
unsigned char qmi8658a_read_AE_frame(float* quat, float* velocity);

unsigned char qmi8658a_FIFO_mode_config(qmi8658a_Fifo_Mode fifo_mode,qmi8658a_Fifo_Size fifo_size);
unsigned char qmi8658a_FIFO_wtm_threshold_config(unsigned char wtm_threshold);
unsigned char qmi8658a_FIFO_sample_count_get(unsigned short* count);
unsigned char qmi8658a_FIFO_read_mode_enable(void);
unsigned char qmi8658a_FIFO_read_mode_clear(void);
unsigned char qmi8658a_read_FIFO_frame(signed short* xyz_rawdata);
unsigned char qmi8658a_read_FIFO_frame_two_sensors(signed short* acc_xyz_rawdata,signed short* gyr_xyz_rawdata);
	
unsigned char qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Command cmd);
unsigned char qmi8658a_locking_mechanism_config(qmi8658a_SyncSmpl_En syncSmpl_en,qmi8658a_SysHS_En sysHS_en);
unsigned char qmi8658a_internal_AHB_clock_gating_config(qmi8658a_ACG_En AHB_en);

void qmi8658a_soft_reset(void);
/**
 ****************************************************************************************
 * @brief  example接口定义
 ****************************************************************************************
 */

#define qmi8658a_log	printf
#define INT1_PIN_TYPE		APP_IO_TYPE_AON
#define INT1_PIN			APP_IO_PIN_6
#define INT1_PIN_MUX		APP_IO_MUX_7
#define INT1_MODE			APP_IO_MODE_IT_FALLING
#define INT1_PIN_PULL		APP_IO_PULLUP

#define INT2_PIN_TYPE		APP_IO_TYPE_GPIOA
#define INT2_PIN			APP_IO_PIN_9
#define INT2_PIN_MUX		APP_IO_MUX_7
#define INT2_MODE			APP_IO_MODE_IT_FALLING
#define INT2_PIN_PULL		APP_IO_PULLUP


#define QMI8658_CTRL8_PEDOMETER_EN 0x10
/**< example.1 spi/i2c通信测试例程 */
void qmi8658a_communicate_test(void);
/**< example.2 加速度计轮询读数据例程 */
long qmi8658a_read_acc_by_polling_status_register(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString);
/**< example.3 陀螺仪中断读数据例程 */
void qmi8658a_read_gyr_by_data_ready_interrupt(void);
void data_ready_interrupt_callback(void);
/**< example.4 加速度计FIFO中断读数据例程 */
void qmi8658a_read_acc_by_FIFO_full_interrupt(void);
void FIFO_full_interrupt_callback(void);
/**< example.5 加速度计和陀螺仪FIFO水位线中断读数据例程 */
void qmi8658a_read_acc_gyr_by_FIFO_wtm_interrupt(void);
void FIFO_wtm_interrupt_callback(void);
/**< example.6 姿态引擎例程*/
void qmi8658a_AE_example(void);
/**< example.7 加速度计和陀螺仪校准例程 */
void qmi8658a_acc_gry_calibration_example(void);
/**< example.8 闭锁方式读取加速度计例程 */
void qmi8658a_read_acc_in_locking_mechanical_mode(void);
/**< example.9 运动唤醒例程 */
void qmi8658a_wake_on_motion_example(void);
void FIFO_WoM_interrupt_callback(void);
void QMI_Init(void);
long QMI_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString);
void QMI_Enable_WoM(void);
void QMI_Enable_Pedometer(unsigned short odr);
unsigned int QMI_Get_Step(void);
void QMI_Clear_Step(void);
void QMI_Disable_WoM(void);

#endif

