#include "QMI8658A.h"

/****************** driver接口定义 *******************/

/****************** 接口初始化 *******************/
unsigned char qmi8658a_port_init(void)
{
	unsigned char ret=Qmi_Success;
	
#if QMI8658A_SPI_I2C_SEL	
	
	//ret=qmi8658a_I2c_Init();
	
#else
	
	ret=qmi8658a_Spi_Init();
	
#endif
	
	return ret;
}

/****************** 读寄存器（单/多字节） *******************/
unsigned char qmi8658a_read_reg(qmi8658a_Reg reg_addr, unsigned char* buf, unsigned char length)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg=reg_addr;

#if QMI8658A_SPI_I2C_SEL	
/****************** I2c模式 *******************/
	if(length>1)reg=reg|0x80;	//I2C模式下连续读写时寄存器地址最高位需置1
	
	ret=qmi8658a_I2c_Read(QMI8658A_SLAVE_ADDR,reg_addr,buf,length);

#else
/****************** Spi模式 *******************/
	
		ret=qmi8658a_Spi_Read(reg, buf, length);
	
#endif
	
	return ret;
}

/****************** 写寄存器（单/多字节） *******************/
unsigned char qmi8658a_write_reg(qmi8658a_Reg reg_addr, unsigned char* buf,unsigned char length)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg=reg_addr;

#if QMI8658A_SPI_I2C_SEL	
/****************** I2c模式 *******************/
	if(length>1)reg=reg|0x80;	//I2C模式下连续读写时寄存器地址最高位需置1
	ret=qmi8658a_I2c_Write(QMI8658A_SLAVE_ADDR,reg,buf,length);

#else
/****************** Spi模式 *******************/		
		ret=qmi8658a_Spi_Write(reg, buf,length);
	
#endif
	
	return ret;
}

/****************** 检查WhoAmI寄存器 *******************/
unsigned char qmi8658a_check_WhoAmI(void)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_WhoAmI, &reg_value, 1);
	
	if((ret!=Qmi_Success) || (reg_value != WhoAmI_Value))
	return Qmi_Error;
	
	return Qmi_Success;
}

/****************** 检查Revison寄存器 *******************/
unsigned char qmi8658a_check_Revison(void)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Revision, &reg_value, 1);
	
	if((ret!=Qmi_Success) || (reg_value != Revison_Vlaue))
	return Qmi_Error;
	
	return Qmi_Success;
}

/****************** 配置串行接口参数 *******************/
unsigned char qmi8658a_serial_interface_config(qmi8658a_SIM spi_mode,qmi8658a_ADDR_AI addr_ai,qmi8658a_BE endian)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl1, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	if(spi_mode==Enable_3_Wire_Spi)
	{
		reg_value|=(0x01<<CTRL1_SIM);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL1_SIM);
	}
	
	if(addr_ai==Addr_Auto_Increment)
	{
		reg_value|=(0x01<<CTRL1_ADDR_AI);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL1_ADDR_AI);
	}
	
	if(endian==Big_Endian)
	{
		reg_value|=(0x01<<CTRL1_BE);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL1_BE);
	}
	
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl1, &reg_value,1);
	
	return ret;
}

/****************** 配置中断引脚1使能 *******************/
unsigned char qmi8658a_int1_config(qmi8658a_INT1_En int1_en)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl1, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	if(int1_en==INT1_Enable)
	{
		reg_value|=(0x01<<CTRL1_INT1_EN);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL1_INT1_EN);
	}	
	
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl1, &reg_value,1);
	
	return ret;
}

/****************** 配置中断引脚2使能 *******************/
unsigned char qmi8658a_int2_config(qmi8658a_INT2_En int2_en)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl1, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	if(int2_en==INT2_Enable)
	{
		reg_value|=(0x01<<CTRL1_INT2_EN);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL1_INT2_EN);
	}	
	
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl1, &reg_value,1);
	
	return ret;
}

/****************** 配置传感器使能 *******************/
unsigned char qmi8658a_sensor_osc_config(qmi8658a_Sensor_Osc_En osc_en)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl1, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	if(osc_en==Enable_Internal_Osc)
	{
		reg_value|=(0x01<<CTRL1_Osc_EN);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL1_Osc_EN);
	}
	
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl1, &reg_value,1);
	
	return ret;
}

/****************** 配置加速度计参数 *******************/
unsigned char qmi8658a_acc_setting(qmi8658a_Acc_Range range, qmi8658a_Acc_Odr odr,qmi8658a_ST_En selftest)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl2, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	if(selftest==Self_Test_Enable)
	{
		reg_value|=(0x01<<CTRL2_aST);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL2_aST);
	}
	
	reg_value|=(range<<CTRL2_aFs);
	reg_value|=(odr<<CTRL2_aODR);
	
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl2, &reg_value,1);
	
	return ret;
}

/****************** 配置陀螺仪参数 *******************/
unsigned char qmi8658a_gyr_setting(qmi8658a_Gyr_Range range, qmi8658a_Gyr_Odr odr,qmi8658a_ST_En selftest)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl3, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	if(selftest==Self_Test_Enable)
	{
		reg_value|=(0x01<<CTRL3_gST);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL3_gST);
	}
	
	reg_value|=(range<<CTRL3_gFs);
	reg_value|=(odr<<CTRL3_gODR);
	
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl3, &reg_value,1);
	return ret;
}

/****************** 配置加速度计低通滤波器参数 *******************/
unsigned char qmi8658a_acc_lpf_config(qmi8658a_Acc_Lpf_Mode lpf_mode,qmi8658a_Acc_Lpf_En lpf_en)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl5, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	if(lpf_en==ACC_LPF_Enable)
	{
		reg_value|=(0x01<<CTRL5_aLPF_EN);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL5_aLPF_EN);	
	}
	
	reg_value |= (lpf_mode<<CTRL5_aLPF_MODE);
	
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl5,&reg_value,1);
	
	return ret;
}

/****************** 配置陀螺仪低通滤波器参数 *******************/
unsigned char qmi8658a_gyr_lpf_config(qmi8658a_Gyr_Lpf_Mode lpf_mode,qmi8658a_Gyr_Lpf_En lpf_en)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl5, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	if(lpf_en==GYR_LPF_Enable)
	{
		reg_value|=(0x01<<CTRL5_gLPF_EN);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL5_gLPF_EN);	
	}
	
	reg_value|=(lpf_mode<<CTRL5_gLPF_MODE);

	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl5,&reg_value,1);
	
	return ret;
}

/****************** 加速度计使能配置 *******************/
unsigned char qmi8658a_acc_config(qmi8658a_Acc_En acc_en)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl7, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	if(acc_en==ACC_Enable)
	{
		reg_value|=(0x01<<CTRL7_aEN);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL7_aEN);	
	}	
	
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl7, &reg_value,1);
	
	return ret;
}

/****************** 陀螺仪配置使能 *******************/
unsigned char qmi8658a_gyr_config(qmi8658a_Gyr_SN_En sn_en,qmi8658a_Gyr_En gyr_en)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl7, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	if(sn_en==Snooze_MODE_Enable)
	{
		reg_value|=(0x01<<CTRL7_sEN);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL7_sEN);	
	}	
	
	if(gyr_en==GYR_Enable)
	{
		reg_value|=(0x01<<CTRL7_gEN);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL7_gEN);	
	}	
	
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl7, &reg_value,1);
	
	return ret;
}

/****************** AE 配置 *******************/
unsigned char qmi8658a_ae_setting(qmi8658a_AE_Odr ae_odr)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl6, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
		
	reg_value|=(ae_odr<<CTRL6_sORD);
	
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl6, &reg_value,1);
	return ret;
}

/****************** AE使能配置 *******************/
unsigned char qmi8658a_ae_config(qmi8658a_AE_En ae_en)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl7, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	if(ae_en==AE_Enable)
	{
		reg_value|=(0x01<<CTRL7_sEN);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL7_sEN);	
	}	
	
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl7, &reg_value,1);
	
	return ret;
}

/****************** 发送Ctrl9指令 *******************/
unsigned char qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Command cmd)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	unsigned char statusInt=0;
	unsigned short count=0;

	reg_value=cmd;
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl9, &reg_value,1);	
	if(ret!=Qmi_Success)return ret;
	
	while((statusInt&STATUS_Ctrl9_CmdDone)!=STATUS_Ctrl9_CmdDone)
	{
		qmi8658a_delay_ms(2);
		ret=qmi8658a_read_reg(qmi8658aRegister_StatusInt, &statusInt, 1);
		if((ret!=Qmi_Success) || (count++>10))return Qmi_Error;
	}

	reg_value=qmi8658a_Ctrl9_Cmd_NOP;
	qmi8658a_write_reg(qmi8658aRegister_Ctrl9, &reg_value,1);	
	if(ret!=Qmi_Success)return ret;

	statusInt=0;
	count=0;
	while(((statusInt&STATUS_Ctrl9_CmdDone)==STATUS_Ctrl9_CmdDone)&&(count++<10))		
	{
		qmi8658a_delay_ms(2);	
		qmi8658a_read_reg(qmi8658aRegister_StatusInt, &statusInt, 1);
		if((ret!=Qmi_Success) || (count++>10))return Qmi_Error;
	}

	return ret;
}

/****************** FIFO模式配置 *******************/
unsigned char qmi8658a_FIFO_mode_config(qmi8658a_Fifo_Mode fifo_mode,qmi8658a_Fifo_Size fifo_size)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_FifoCtrl, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	reg_value|=(fifo_mode<<CTRL_FIFO_MODE);
	reg_value|=(fifo_size<<CTRL_FIFO_SIZE);
	
	ret=qmi8658a_write_reg(qmi8658aRegister_FifoCtrl, &reg_value,1);
	return ret;
}

/****************** FIFO水位线阈值配置 *******************/
unsigned char qmi8658a_FIFO_wtm_threshold_config(unsigned char wtm_threshold)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	
	reg_value=wtm_threshold;
	ret=qmi8658a_write_reg(qmi8658aRegister_FifoWmkTh, &reg_value,1);
	
	return ret;
}

/****************** 获取FIFO采样数 *******************/
unsigned char qmi8658a_FIFO_sample_count_get(unsigned short* count)
{
	unsigned char ret=Qmi_Success;
	unsigned char count_lsb=0;
	unsigned char count_msb=0;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_FifoCount, &count_lsb, 1);
	if(ret!=Qmi_Success)return ret;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_FifoStatus, &count_msb, 1);
	if(ret!=Qmi_Success)return ret;
	
	*count=((unsigned short)(count_msb&0x03)<<8) | count_lsb;
	
	return ret;
}

/****************** 清除FIFO读取模式 *******************/
unsigned char qmi8658a_FIFO_read_mode_clear(void)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value=0;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_FifoCtrl, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	reg_value&=~(0x01<<CTRL_FIFO_RD_MODE);
	
	ret=qmi8658a_write_reg(qmi8658aRegister_FifoCtrl, &reg_value,1);
	return ret;
}


/****************** 读取StatusInt寄存器 *******************/
unsigned char qmi8658a_readStatusInt(unsigned char* status_int)
{
	unsigned char ret=Qmi_Success;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_StatusInt, status_int, 1);

	return ret;
}

/****************** 读取Status0寄存器 *******************/
unsigned char qmi8658a_readStatus0(unsigned char* status0)
{
	unsigned char ret=Qmi_Success;

	ret=qmi8658a_read_reg(qmi8658aRegister_Status0, status0, 1);

	return ret;
}

/****************** 读取Status1寄存器 *******************/
unsigned char qmi8658a_readStatus1(unsigned char* status1)
{
	unsigned char ret=Qmi_Success;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Status1, status1, 1);

	return ret;
}

/****************** 读取温度 *******************/
unsigned char qmi8658a_readTemp(float* temp_f)
{
	unsigned char buf[2];
	unsigned char ret=Qmi_Success;
	short temp = 0;

	ret=qmi8658a_read_reg(qmi8658aRegister_Tempearture_L, buf, 2);
	
	temp = ((short)buf[1]<<8)|buf[0];
	*temp_f = (float)temp/256.0f;

	return ret;
}

/****************** 读取时间戳 *******************/
unsigned char qmi8658a_read_timestamp(unsigned int* timestamp)
{
	unsigned char	buf[3];
	unsigned char ret=Qmi_Success;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Timestamp_L, buf, 3);
	*timestamp = (unsigned int)(((unsigned int)buf[2]<<16)|((unsigned int)buf[1]<<8)|buf[0]);
		
	return ret;
}


/****************** 读取加速度计原始数据 *******************/
unsigned char qmi8658a_read_acc_rawdata(signed short* xyz_rawdata)
{
	unsigned char ret=Qmi_Success;
	unsigned char buff[6]={0};
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ax_L, buff, 6);
	
	xyz_rawdata[0] = (short)((unsigned short)(buff[1]<<8) |( buff[0]));	//x
	xyz_rawdata[1] = (short)((unsigned short)(buff[3]<<8) |( buff[2]));	//y
	xyz_rawdata[2] = (short)((unsigned short)(buff[5]<<8) |( buff[4]));	//z
	
	return ret;
}

/****************** 读取陀螺仪原始数据 *******************/
unsigned char qmi8658a_read_gyr_rawdata(signed short* xyz_rawdata)
{
	unsigned char ret=Qmi_Success;
	unsigned char buff[6]={0};
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Gx_L, buff, 6);
	
	xyz_rawdata[0] = (short)((unsigned short)(buff[1]<<8) |( buff[0]));	//x
	xyz_rawdata[1] = (short)((unsigned short)(buff[3]<<8) |( buff[2])); //y
	xyz_rawdata[2] = (short)((unsigned short)(buff[5]<<8) |( buff[4])); //z
	
	return ret;
}

/****************** 读取一帧AE数据 *******************/
unsigned char qmi8658a_read_AE_frame(float* quat, float* velocity)
{
	unsigned char ret=Qmi_Success;
	unsigned char buff[14]={0};
	short	raw_q_xyz[4]={0};
	short raw_v_xyz[3]={0};
	unsigned short ae_q_lsb_div = (1 << 14);
	unsigned short ae_v_lsb_div = (1 << 10);
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Q1_L, buff, 14);
	if(ret!=Qmi_Success)return ret;
	
	raw_q_xyz[0] = (short)((unsigned short)(buff[1]<<8) |( buff[0]));
	raw_q_xyz[1] = (short)((unsigned short)(buff[3]<<8) |( buff[2]));
	raw_q_xyz[2] = (short)((unsigned short)(buff[5]<<8) |( buff[4]));
	raw_q_xyz[3] = (short)((unsigned short)(buff[7]<<8) |( buff[6]));

	raw_v_xyz[0] = (short)((unsigned short)(buff[9]<<8) |( buff[8]));
	raw_v_xyz[1] = (short)((unsigned short)(buff[11]<<8) |( buff[10]));
	raw_v_xyz[2] = (short)((unsigned short)(buff[13]<<8) |( buff[12]));
	
	quat[0] = (float)(raw_q_xyz[0]*1.0f)/ae_q_lsb_div;
	quat[1] = (float)(raw_q_xyz[1]*1.0f)/ae_q_lsb_div;
	quat[2] = (float)(raw_q_xyz[2]*1.0f)/ae_q_lsb_div;
	quat[3] = (float)(raw_q_xyz[3]*1.0f)/ae_q_lsb_div;

	velocity[0] = (float)(raw_v_xyz[0]*1.0f)/ae_v_lsb_div;
	velocity[1] = (float)(raw_v_xyz[1]*1.0f)/ae_v_lsb_div;
	velocity[2] = (float)(raw_v_xyz[2]*1.0f)/ae_v_lsb_div;
	
	return ret;
}

/****************** 读取一帧FIFO数据（单传感器） *******************/
unsigned char qmi8658a_read_FIFO_frame(signed short* xyz_rawdata)
{
	unsigned char ret=Qmi_Success;
	unsigned char buff[6]={0};
	
	ret=qmi8658a_read_reg(qmi8658aRegister_FifoData, buff, 6);
	if(ret!=Qmi_Success)return ret;
	
	xyz_rawdata[0] = (short)((unsigned short)(buff[1]<<8) |( buff[0]));	//x
	xyz_rawdata[1] = (short)((unsigned short)(buff[3]<<8) |( buff[2])); //y
	xyz_rawdata[2] = (short)((unsigned short)(buff[5]<<8) |( buff[4])); //z
	
	return ret;
}

/****************** 读取一帧FIFO数据(双传感器) *******************/
unsigned char qmi8658a_read_FIFO_frame_two_sensors(signed short* acc_xyz_rawdata,signed short* gyr_xyz_rawdata)
{
	unsigned char ret=Qmi_Success;
	unsigned char buff[12]={0};
	
	ret=qmi8658a_read_reg(qmi8658aRegister_FifoData, buff, 12);
	if(ret!=Qmi_Success)return ret;
	
	acc_xyz_rawdata[0] = (short)((unsigned short)(buff[1]<<8) |( buff[0]));	//acc_x
	acc_xyz_rawdata[1] = (short)((unsigned short)(buff[3]<<8) |( buff[2])); //acc_y
	acc_xyz_rawdata[2] = (short)((unsigned short)(buff[5]<<8) |( buff[4])); //acc_z
	
	gyr_xyz_rawdata[0] = (short)((unsigned short)(buff[7]<<8) |( buff[6]));	//acc_x
	gyr_xyz_rawdata[1] = (short)((unsigned short)(buff[9]<<8) |( buff[8])); //acc_y
	gyr_xyz_rawdata[2] = (short)((unsigned short)(buff[11]<<8) |( buff[10])); //acc_z
	
	return ret;
}

/****************** 转换加速度计原始数据为mg *******************/
void qmi8658a_acc_rawdata_and_mg_convert(qmi8658a_Acc_Range acc_range,const signed short* rawdata,float* acc_value)
{
	unsigned short convent_factor=0;
	
	switch (acc_range)
  {
  	case qmi8658aAccRange_2g:
			convent_factor=(0x01 << 14);
  		break;
  	case qmi8658aAccRange_4g:
			convent_factor=(0x01 << 13);
  		break;
		case qmi8658aAccRange_8g:
			convent_factor=(0x01 << 12);
  		break;
		case qmi8658aAccRange_16g:
			convent_factor=(0x01 << 11);
  		break;
		
  	default:
  		break;
  }
	
	acc_value[0] = (float)(rawdata[0]*1000.0f)/convent_factor;	//x
	acc_value[1] = (float)(rawdata[1]*1000.0f)/convent_factor;	//y
	acc_value[2] = (float)(rawdata[2]*1000.0f)/convent_factor;	//z
}

/****************** 转换陀螺仪原始数据为dps *******************/
void qmi8658a_gyr_rawdata_and_dps_convert(qmi8658a_Gyr_Range gry_range,const signed short* rawdata,float* acc_value)
{
	unsigned short convent_factor=0;
	
	switch (gry_range)
  {
  	case qmi8658aGyrRange_16dps:
			convent_factor=2408;
  		break;
  	case qmi8658aGyrRange_32dps:
			convent_factor=1024;
  		break;
		case qmi8658aGyrRange_64dps:
			convent_factor=512;
  		break;
		case qmi8658aGyrRange_128dps:
			convent_factor=128;
  		break;
		case qmi8658aGyrRange_256dps:
			convent_factor=64;
  		break;
		case qmi8658aGyrRange_512dps:
			convent_factor=32;
  		break;
		case qmi8658aGyrRange_1024dps:
			convent_factor=16;
  		break;
		case qmi8658aGyrRange_2048dps:
			convent_factor=8;
  		break;
		
  	default:
  		break;
  }
	
	acc_value[0] = (float)(rawdata[0]*1.0f)/convent_factor;	//x	
	acc_value[1] = (float)(rawdata[1]*1.0f)/convent_factor;	//y
	acc_value[2] = (float)(rawdata[2]*1.0f)/convent_factor; //z
}
	
/****************** 闭锁使能配置 *******************/
unsigned char qmi8658a_locking_mechanism_config(qmi8658a_SyncSmpl_En syncSmpl_en,qmi8658a_SysHS_En sysHS_en)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value;
	
	ret=qmi8658a_read_reg(qmi8658aRegister_Ctrl7, &reg_value, 1);
	if(ret!=Qmi_Success)return ret;
	
	if(syncSmpl_en==SyncSmpl_Enable)
	{
		reg_value|=(0x01<<CTRL7_syncSmpl);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL7_syncSmpl);	
	}	
	
	if(sysHS_en==SysHS_Enable)
	{
		reg_value|=(0x01<<CTRL7_sys_hs);
	}
	else 
	{
		reg_value&=~(0x01<<CTRL7_sys_hs);	
	}	
	
	ret=qmi8658a_write_reg(qmi8658aRegister_Ctrl7, &reg_value,1);
	
	return ret;
}

//When using SPI (accel only mode with ODR less 
//than 500Hz), I2C or I3C interfaces (in all ODRs), The 
//user needs to disable the internal AHB clock gating。
//And after disabling the sensor, enable back the clock gating
/****************** AHB时钟选通配置 *******************/
unsigned char qmi8658a_internal_AHB_clock_gating_config(qmi8658a_ACG_En AHB_en)
{
	unsigned char ret=Qmi_Success;
	unsigned char reg_value;
	
	if(AHB_en==qmi8658a_AHB_Disable)
	{
		reg_value=0x01;
	}
	else
	{
		reg_value=0x00;
	}
	ret=qmi8658a_write_reg(qmi8658aRegister_Cal1_L, &reg_value,1);
	if(ret!=Qmi_Success)return ret;
	
	ret=qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Cmd_AHB_Clock_Gating);
	
	return ret;
}

void qmi8658a_soft_reset(void)
{
	unsigned char reg_value=Reset_Vlaue;
	
	qmi8658a_write_reg(qmi8658aRegister_Reset, &reg_value,1);
}

void qmi8658a_enter_low_power(void)
{
	uint8_t reg_read;
	qmi8658a_read_reg(qmi8658aRegister_Ctrl1, &reg_read, 1);	
	reg_read &= (~0x01);										//关闭ctrl1 的bit0 sensorDisable =0
	qmi8658a_write_reg(qmi8658aRegister_Ctrl1, &reg_read, 1);
	
	reg_read = 0;
	qmi8658a_write_reg(qmi8658aRegister_Ctrl7, &reg_read, 1);  // CTRL7 aEN = 0, gEN = 0,
	
	reg_read = 0x0F;
	qmi8658a_write_reg(qmi8658aRegister_Ctrl7, &reg_read, 1);  // CTRL2 AODR=11xx
	
}
