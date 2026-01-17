#include "QMI8658A.h"
#include "app_gpiote.h"
#include "osal.h"
#include "app_log.h"

uint8_t Step_flag = 0, WoM_On = 0;
void QMI_INT1_Handle(app_io_evt_t *p_evt);
const app_gpiote_param_t qmi_gpiote_param = {
    .type = INT1_PIN_TYPE,
    .pin  = INT1_PIN,
    .mode = INT1_MODE,        //注意，这里可能与实际不符，需要修改!!!!!!!!!!!!!!
    .pull = INT1_PIN_PULL,              //上拉还是下拉？不确定
    .io_evt_cb = QMI_INT1_Handle
};

/****************** I2c/Spi 通信测试 *******************/
void qmi8658a_communicate_test(void)
{
	unsigned char ret=0;
	unsigned char reg_read=0;
	unsigned char buff[2]={0};
	
	//1.Spi/I2c接口初始化
	ret=qmi8658a_port_init();
	if(ret!=Qmi_Success)
	{
		APP_LOG_INFO( "qmi8658a_port_init error\r\n");
		return;
	}
	else
	{
		APP_LOG_INFO( "qmi8658a_port_init success\r\n");
	}
	
	//2.芯片上电和复位后都需等待1.75s再操作寄存器
	qmi8658a_delay_ms(2000);
	
	//3.读取whoAmI寄存器 读取成功则返回0x05
	qmi8658a_read_reg(qmi8658aRegister_WhoAmI, &reg_read, 1);
	if(reg_read!=0x05)
	{
		APP_LOG_INFO( "qmi8658a_read register error\r\n");
		return;
	}
	else
	{
		APP_LOG_INFO( "qmi8658a_read register success\r\n");
	}
	
	//4.写ctl1寄存器，打开寄存器自增功能，写成功则ctl1寄存器bit6置1
	qmi8658a_read_reg(qmi8658aRegister_Ctrl1, &reg_read, 1);	
	reg_read|=0x40;
	qmi8658a_write_reg(qmi8658aRegister_Ctrl1, &reg_read, 1);
	
	qmi8658a_read_reg(qmi8658aRegister_Ctrl1, &reg_read, 1);
	if((reg_read&0x40) != 0x40)
	{
		APP_LOG_INFO( "qmi8658a write register error\r\n");
		return;
	}
	else
	{
		APP_LOG_INFO( "qmi8658a write register success\r\n");
	}
	                                 
	//5.连续读取whoAmI寄存器和revision寄存器，读成功应返回0x05和0x7c
	qmi8658a_read_reg(qmi8658aRegister_WhoAmI, buff, 2);
	if(buff[0]!=WhoAmI_Value || buff[1]!=Revison_Vlaue)
	{
		APP_LOG_INFO( "qmi8658a read multiple register error 0x%x 0x%x\r\n", buff[0], buff[1]);
		return;
	}

	APP_LOG_INFO( "qmi8658a communicate test success!\r\n");
}

float abs_f(float num)
{
	if(num > 0)
		return num;
	else
		return num * -1;
}

signed short abs_i(signed short num)
{
	if(num > 0)
		return num;
	else
		return num * -1;
}

/****************** 轮询方式读取加速度计数据 *******************/
long qmi8658a_read_acc_by_polling_status_register(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString)
{		
	signed short 	acc_xyz_rawdata[3]={0}, print_xyz[6] = {0};
	float acc_xyz_mg[3]={0};
	unsigned char status0=0;
	
	//1.通信接口初始化
	qmi8658a_port_init();
	
	//2.芯片上电和复位后都需等待1.75s再操作寄存器
	qmi8658a_delay_ms(2000);
	qmi8658a_soft_reset();
	qmi8658a_delay_ms(2000);
	
	//3.使能寄存器地址自增
	qmi8658a_serial_interface_config(Enable_4_Wire_Spi,Addr_Auto_Increment,Big_Endian);
	
	//4.配置加速度计量程和Odr
	qmi8658a_acc_setting(qmi8658aAccRange_4g, qmi8658aAccOdr_125Hz,Self_Test_Disable);
	
	//5.配置加速度计低通滤波器
	qmi8658a_acc_lpf_config(A_LSP_MODE_2,ACC_LPF_Enable);
	
	//6.使能加速度计
	qmi8658a_acc_config(ACC_Enable);
	
	while(1)
	{
		//7.轮询状态寄存器，有更新则读取加速度数据
		qmi8658a_readStatus0(&status0);
		if((status0&STATUS_aDA)==STATUS_aDA)
		{
			qmi8658a_read_acc_rawdata(acc_xyz_rawdata);		
			qmi8658a_acc_rawdata_and_mg_convert(qmi8658aAccRange_4g,acc_xyz_rawdata,acc_xyz_mg);
			print_xyz[0] = (signed short)acc_xyz_mg[0];
			print_xyz[1] = (signed short)((abs_f(acc_xyz_mg[0]) - abs_i(print_xyz[0])) * 100);
			print_xyz[2] = (signed short)acc_xyz_mg[1];
			print_xyz[3] = (signed short)((abs_f(acc_xyz_mg[1]) - abs_i(print_xyz[2])) * 100);
			print_xyz[4] = (signed short)acc_xyz_mg[2];
			print_xyz[5] = (signed short)((abs_f(acc_xyz_mg[2]) - abs_i(print_xyz[4])) * 100);
			APP_LOG_INFO( "x=%d.%dmg,y=%d.%dmg,z=%d.%dmg\r\n",print_xyz[0], print_xyz[1], print_xyz[2], print_xyz[3], print_xyz[4], print_xyz[5]);	
		}
		qmi8658a_delay_ms(1000);
	}
}

/************* 计步 **************/

void QMI_Enable_Pedometer(unsigned short odr)
{

	uint8_t reg_value = 0, ret;
	float finalRate = (float)(200.0f/odr); //14.285
	unsigned short ped_sample_cnt = (unsigned short)(0x0032 / finalRate);//6;//(unsigned short)(0x0032 / finalRate) ;
	unsigned short ped_fix_peak2peak = 0x00AC;//0x0006;//0x00CC;
	unsigned short ped_fix_peak = 0x00AC;//0x0006;//0x00CC;
	unsigned short ped_time_up = (unsigned short)(200 / finalRate);
	unsigned char ped_time_low = (unsigned char) (20 / finalRate);
	unsigned char ped_time_cnt_entry = 8;
	unsigned char ped_fix_precision = 0;
	unsigned char ped_sig_count = 1;


	reg_value = ped_sample_cnt & 0xFF;
    qmi8658a_write_reg(qmi8658aRegister_Cal1_L, &reg_value, 1);
	reg_value = (ped_sample_cnt >> 8) & 0xFF;
    qmi8658a_write_reg(qmi8658aRegister_Cal1_H, &reg_value, 1);
	reg_value = ped_fix_peak2peak & 0xFF;
    qmi8658a_write_reg(qmi8658aRegister_Cal2_L, &reg_value, 1);
	reg_value = (ped_fix_peak2peak >> 8) & 0xFF;
    qmi8658a_write_reg(qmi8658aRegister_Cal2_H, &reg_value, 1);
	reg_value = ped_fix_peak & 0xFF;
    qmi8658a_write_reg(qmi8658aRegister_Cal3_L, &reg_value, 1);
	reg_value = (ped_fix_peak >> 8) & 0xFF;
    qmi8658a_write_reg(qmi8658aRegister_Cal3_H, &reg_value, 1);
	reg_value = 0x01;
    qmi8658a_write_reg(qmi8658aRegister_Cal4_H, &reg_value, 1);
	reg_value = 0x02;
    qmi8658a_write_reg(qmi8658aRegister_Cal4_L, &reg_value, 1);
    qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Cmd_EnablePedometer);

	reg_value = ped_time_up & 0xFF;
    qmi8658a_write_reg(qmi8658aRegister_Cal1_L, &reg_value, 1);
	reg_value = (ped_time_up >> 8) & 0xFF;
    qmi8658a_write_reg(qmi8658aRegister_Cal1_H, &reg_value, 1);
    qmi8658a_write_reg(qmi8658aRegister_Cal2_L, &ped_time_low, 1);
    qmi8658a_write_reg(qmi8658aRegister_Cal2_H, &ped_time_cnt_entry, 1);
    qmi8658a_write_reg(qmi8658aRegister_Cal3_L, &ped_fix_precision, 1);
    qmi8658a_write_reg(qmi8658aRegister_Cal3_H, &ped_sig_count, 1);
	reg_value = 0x02;
    qmi8658a_write_reg(qmi8658aRegister_Cal4_H, &reg_value, 1);
	reg_value = 0x02;
    qmi8658a_write_reg(qmi8658aRegister_Cal4_L, &reg_value, 1);
    qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Cmd_EnablePedometer);

	qmi8658a_read_reg(qmi8658aRegister_Ctrl8, &reg_value, 1);
	reg_value |= QMI8658_CTRL8_PEDOMETER_EN;
	reg_value &= ~(1 << 6);		//计步选择INT2, 但并不启用, 这么写主要是避免与抬腕撞车

    qmi8658a_write_reg(qmi8658aRegister_Ctrl8, &reg_value, 1);
	qmi8658a_acc_config(ACC_Enable);
	APP_LOG_INFO( "Pedometer Start\r\n");
}

unsigned int QMI_Get_Step()
{
    unsigned char buf[3];
	unsigned int step;
    qmi8658a_read_reg(Qmi8658Register_Pedo_L, buf, 3); // 0x5a
    step = (unsigned int)((buf[2] << 16) | (buf[1] << 8) | (buf[0]));
	return step;
}

void QMI_Clear_Step()
{
	qmi8658a_ctrl9_cmd_send(0x0f);
}

/**************** WOM 抬腕唤醒 **************************/
void QMI_INT1_Handle(app_io_evt_t *p_evt)
{
	APP_LOG_INFO( "Enter the INT\r\n");
	if(WoM_On & (Step_flag == 0)){
		WoM_On = 0;
		Step_flag = 1;
	}
}

void QMI_Disable_WoM(void)
{
	uint8_t reg_value;
	reg_value=0x00;   
	qmi8658a_write_reg(qmi8658aRegister_Ctrl7, &reg_value,1);
	qmi8658a_write_reg(qmi8658aRegister_Cal1_L, &reg_value,1);
	qmi8658a_write_reg(qmi8658aRegister_Cal1_H, &reg_value,1);
	
	qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Cmd_WoM_Setting);
	app_gpiote_deinit(INT1_PIN_TYPE, INT1_PIN);
	APP_LOG_INFO( "Wake on Motion exit\r\n");
}

void QMI_Enable_WoM(void)
{
	uint8_t reg_value, blankingTime, ret;
		//4.Entering Wake on Motion Mode
	//a.Disable sensors. (Write 0x00 to CTRL7)
	qmi8658a_int1_config(INT1_Enable);
    app_gpiote_config(&qmi_gpiote_param);
	reg_value=0x00;
	qmi8658a_write_reg(qmi8658aRegister_Ctrl7, &reg_value,1);
	//the acc_odr should only to be set the lowpower mode
	
	//c.Set Wake on Motion (WoM) Threshold in CAL1_L;
	reg_value=(unsigned char)Qmi8658WomThreshold_high;
	qmi8658a_write_reg(qmi8658aRegister_Cal1_L, &reg_value,1);
	
	//d.select interrupt, polarity and blanking time in CAL1_H
	blankingTime=100;
	reg_value=(unsigned char)qmi8658a_Int1_High | ( blankingTime & 0x3F);
	qmi8658a_write_reg(qmi8658aRegister_Cal1_H, &reg_value,1);
	//INT2，初始电平高，持续时间blankingTime 100
	
	//e.Execute CTRL9 command to configure WoM mode
	qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Cmd_WoM_Setting);
	qmi8658a_delay_ms(5);		//等一下防止进入中断的时候WoM_On为1;
	qmi8658a_acc_config(ACC_Enable);
	WoM_On = 1;
	APP_LOG_INFO( "Wake on Motion enter\r\n");
}


long QMI_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString)
{
	short 	acc_xyz_rawdata[3]={0}, print_xyz[6] = {0};
	float acc_xyz_mg[3]={0};
	qmi8658a_read_acc_rawdata(acc_xyz_rawdata);		
	qmi8658a_acc_rawdata_and_mg_convert(qmi8658aAccRange_4g,acc_xyz_rawdata,acc_xyz_mg);
	print_xyz[0] = (signed short)acc_xyz_mg[0];
	print_xyz[1] = (signed short)((abs_f(acc_xyz_mg[0]) - abs_i(print_xyz[0])) * 100);
	print_xyz[2] = (signed short)acc_xyz_mg[1];
	print_xyz[3] = (signed short)((abs_f(acc_xyz_mg[1]) - abs_i(print_xyz[2])) * 100);
	print_xyz[4] = (signed short)acc_xyz_mg[2];
	print_xyz[5] = (signed short)((abs_f(acc_xyz_mg[2]) - abs_i(print_xyz[4])) * 100);
	APP_LOG_INFO( "acc:\r\nx=%d.%dmg,y=%d.%dmg,z=%d.%dmg\r\n",print_xyz[0], print_xyz[1], print_xyz[2], print_xyz[3], print_xyz[4], print_xyz[5]);	
	
//	qmi8658a_read_gyr_rawdata(acc_xyz_rawdata);
//	qmi8658a_gyr_rawdata_and_dps_convert(qmi8658aGyrRange_512dps, acc_xyz_rawdata, acc_xyz_mg);
//	print_xyz[0] = (signed short)acc_xyz_mg[0];
//	print_xyz[1] = (signed short)((abs_f(acc_xyz_mg[0]) - abs_i(print_xyz[0])) * 100);
//	print_xyz[2] = (signed short)acc_xyz_mg[1];
//	print_xyz[3] = (signed short)((abs_f(acc_xyz_mg[1]) - abs_i(print_xyz[2])) * 100);
//	print_xyz[4] = (signed short)acc_xyz_mg[2];
//	print_xyz[5] = (signed short)((abs_f(acc_xyz_mg[2]) - abs_i(print_xyz[4])) * 100);
//	APP_LOG_INFO( "gry:\r\nx=%d.%ddps,y=%d.%ddps,z=%d.%ddps\r\n",print_xyz[0], print_xyz[1], print_xyz[2], print_xyz[3], print_xyz[4], print_xyz[5]);	

}

static float acc_xyz_mg[3] = {0};

void QMI_update_acc()
{
	unsigned char status0=0;
	signed short 	acc_xyz_rawdata[3]={0};
	
	qmi8658a_readStatus0(&status0);
	
	if((status0&STATUS_aDA)==STATUS_aDA)
	{
		qmi8658a_read_acc_rawdata(acc_xyz_rawdata);		
		qmi8658a_acc_rawdata_and_mg_convert(qmi8658aAccRange_4g,acc_xyz_rawdata,acc_xyz_mg);
		//APP_LOG_INFO( "x=%f,y=%f,z=%f", acc_xyz_mg[0], acc_xyz_mg[1], acc_xyz_mg[2]);
	}
}

void QMI_Init(void)
{
	signed short 	acc_xyz_rawdata[3]={0}, print_xyz[6] = {0};
	float acc_xyz_mg[3]={0};
	unsigned char status0=0;
	unsigned char ret;
	//qmi8658a_delay_ms(2000);
	qmi8658a_soft_reset();
	qmi8658a_delay_ms(20);	
	APP_LOG_INFO( "QMI reset done");

	//使能寄存器地址自增
	ret = qmi8658a_serial_interface_config(Enable_4_Wire_Spi,Addr_Auto_Increment,Big_Endian);
	if(ret){
		APP_LOG_INFO( "QMI Init failed");
		return;
	}
	//配置加速度计量程和Odr
	ret = qmi8658a_acc_setting(qmi8658aAccRange_8g, qmi8658aAccOdr_62_5Hz,Self_Test_Disable);
	if(ret){
		APP_LOG_INFO( "QMI Init failed");
		return;
	}
	//配置加速度计低通滤波器
	ret = qmi8658a_acc_lpf_config(A_LSP_MODE_2,ACC_LPF_Disable);
	if(ret){
		APP_LOG_INFO( "QMI Init failed");
		return;
	}
//	//使能加速度计
//	ret = qmi8658a_acc_config(ACC_Enable);
//	if(ret){
//		APP_LOG_INFO( "QMI:Init failed\r\n");
//		return;
//	}
	// //配置陀螺仪计量程和Odr
	// ret = qmi8658a_gyr_setting(qmi8658aGyrRange_512dps, qmi8658aGyrOdr_125Hz,Self_Test_Disable);
	// if(ret){
	// 	APP_LOG_INFO( "QMI:Init failed\r\n");
	// 	return;
	// }
	// //配置陀螺仪计低通滤波器
	// ret = qmi8658a_gyr_lpf_config(G_LSP_MODE_2,GYR_LPF_Enable);
	// if(ret){
	// 	APP_LOG_INFO( "QMI:Init failed\r\n");
	// 	return;
	// }
	// //使能陀螺仪
	// ret = qmi8658a_gyr_config(Snooze_MODE_Disable,GYR_Enable);
	// if(ret){
	// 	APP_LOG_INFO( "QMI:Init failed\r\n");
	// 	return;
	// }
	APP_LOG_INFO( "QMI Init successed");
}

/****************** 中断方式读取陀螺仪数据 *******************/
unsigned char data_ready_flag=0;//数据就绪标志

void qmi8658a_read_gyr_by_data_ready_interrupt(void)
{		
	signed short 	gyr_xyz_rawdata[3]={0};
	float gyr_xyz_dps[3]={0};
		
	//1.通信接口初始化
	qmi8658a_port_init();
	
	//2.芯片上电和复位后都需等待1.75s再操作寄存器
	//qmi8658a_delay_ms(2000);
	qmi8658a_soft_reset();
	qmi8658a_delay_ms(20);

	//3.使能寄存器地址自增
	qmi8658a_serial_interface_config(Enable_4_Wire_Spi,Addr_Auto_Increment,Big_Endian);
	
	//4.配置陀螺仪计量程和Odr
	qmi8658a_gyr_setting(qmi8658aGyrRange_512dps, qmi8658aGyrOdr_125Hz,Self_Test_Disable);
	
	//5.配置陀螺仪计低通滤波器
	qmi8658a_gyr_lpf_config(G_LSP_MODE_2,GYR_LPF_Enable);
	
	//6.使能INT2
	qmi8658a_int2_config(INT2_Enable);

	//7.使能陀螺仪
	qmi8658a_gyr_config(Snooze_MODE_Disable,GYR_Enable);
	
	while(1)
	{
		//8.产生数据更新中断后，读取陀螺仪数据
		if(data_ready_flag)
		{
			data_ready_flag=0;
			qmi8658a_read_gyr_rawdata(gyr_xyz_rawdata);		
			qmi8658a_gyr_rawdata_and_dps_convert(qmi8658aGyrRange_512dps,gyr_xyz_rawdata,gyr_xyz_dps);
			APP_LOG_INFO( "x=%fdps,y=%fdps,z=%fdps\r\n",gyr_xyz_dps[0],gyr_xyz_dps[1],gyr_xyz_dps[2]);	
		}
	}
}

//在其中断服务中调用该函数
void data_ready_interrupt_callback()
{
	data_ready_flag=1;
}

/****************** FIFO full中断方式读取加速度计数据 *******************/
unsigned char FIFO_full_flag=0;//FIFO就绪标志

void qmi8658a_read_acc_by_FIFO_full_interrupt(void)
{		
	signed short 	acc_xyz_rawdata[3]={0};
	float acc_xyz_mg[3]={0};
	unsigned short count=0;

	//1.通信接口初始化
	qmi8658a_port_init();
	
	//2.芯片上电和复位后都需等待1.75s再操作寄存器
	qmi8658a_delay_ms(2000);
	qmi8658a_soft_reset();
	qmi8658a_delay_ms(2000);
	
	//3.使能寄存器地址自增
	qmi8658a_serial_interface_config(Enable_4_Wire_Spi,Addr_Auto_Increment,Big_Endian);
	
	//4.配置加速度计量程和Odr
	qmi8658a_acc_setting(qmi8658aAccRange_4g, qmi8658aAccOdr_125Hz,Self_Test_Disable);
	
	//5.配置加速度计低通滤波器
	qmi8658a_acc_lpf_config(A_LSP_MODE_2,ACC_LPF_Enable);
	
	//6.配置FIFO模式
	qmi8658a_FIFO_mode_config(qmi8658a_Fifo_Fifo,qmi8658a_Fifo_128);
	//单传感器情况下每帧数据只包括acc或gyr的xyz三个轴数据，一帧共六个字节，FIFO=128表明存满时FIFO可存128*6 byte共768 byte
	
	//7.配置FIFO水位线阈值
	qmi8658a_FIFO_wtm_threshold_config(128);//设置水位线为FIFO容量，FIFO 满时产生中断
	
	//7.使能INT2
	qmi8658a_int2_config(INT2_Enable);	
	qmi8658a_int1_config(INT1_Enable);
		
	//8.重置FIFO并使能传感器，开始采样
	qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Cmd_Rst_Fifo);
	qmi8658a_acc_config(ACC_Enable);
		
	while(1)
	{
		//9.触发中断后一次性读完FIFO内数据
		if(FIFO_full_flag)
		{
			FIFO_full_flag=0;
			
			//a.先获取FIFO数据数量
			qmi8658a_FIFO_sample_count_get(&count);
			count=count/3;
			//FIFO满时要读取128帧数据，单传感器情况下每帧三个轴共六个字节，count读到的值应该为128(frames)*3(axis)=384
			
			//b.发送ctrl9指令，请求读指令
			qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Cmd_Req_Fifo);
			
			//c.读取fifo数据寄存器，每次读一帧（3axis/6byte）直至读完
			for(;count>0;count--)
			{
				qmi8658a_read_FIFO_frame(acc_xyz_rawdata);
				qmi8658a_acc_rawdata_and_mg_convert(qmi8658aAccRange_4g,acc_xyz_rawdata,acc_xyz_mg);
				APP_LOG_INFO( "x=%fmg,y=%fmg,z=%fmg\r\n",acc_xyz_mg[0],acc_xyz_mg[1],acc_xyz_mg[2]);	
			}	

			//d.读完后重置FIFO并清空标志位
			qmi8658a_FIFO_read_mode_clear();
			qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Cmd_Rst_Fifo);			
		}
	}
}

//请在INT2引脚的中断服务中调用该函数
void FIFO_full_interrupt_callback()
{
	FIFO_full_flag=1;
}

/****************** FIFO水位线中断方式读取加速度计和陀螺仪数据 *******************/
unsigned char FIFO_wtm_flag=0;//FIFO就绪标志

void qmi8658a_read_acc_gyr_by_FIFO_wtm_interrupt(void)
{		
	signed short 	acc_xyz_rawdata[3]={0};
	signed short 	gyr_xyz_rawdata[3]={0};
	float acc_xyz_mg[3]={0};
	float gyr_xyz_dps[3]={0};
	unsigned short count=0;

	//1.通信接口初始化
	qmi8658a_port_init();
	
	//2.芯片上电和复位后都需等待1.75s再操作寄存器
	qmi8658a_delay_ms(2000);
	qmi8658a_soft_reset();
	qmi8658a_delay_ms(2000);
	
	//3.使能寄存器地址自增
	qmi8658a_serial_interface_config(Enable_4_Wire_Spi,Addr_Auto_Increment,Big_Endian);
	
	//4.配置加速度&陀螺仪计量程和Odr
	qmi8658a_acc_setting(qmi8658aAccRange_4g, qmi8658aAccOdr_125Hz,Self_Test_Disable);
	qmi8658a_gyr_setting(qmi8658aGyrRange_512dps, qmi8658aGyrOdr_125Hz,Self_Test_Disable);
	/**< 同时使能双传感器时，ORD以陀螺仪为准 */
	
	//5.配置加速度计&陀螺仪低通滤波器
	qmi8658a_acc_lpf_config(A_LSP_MODE_2,ACC_LPF_Enable);
	qmi8658a_gyr_lpf_config(G_LSP_MODE_2,GYR_LPF_Enable);
	
	//6.配置FIFO模式
	qmi8658a_FIFO_mode_config(qmi8658a_Fifo_Fifo,qmi8658a_Fifo_128);
	//双传感器情况下每帧数据包括acc和gyr的共六个轴数据，一帧共十二个字节，FIFO=128表明存满时FIFO可存128*12 byte共1536byte
	
	//7.配置FIFO水位线阈值
	qmi8658a_FIFO_wtm_threshold_config(64);//设置水位线为FIFO容量的一半，越过水位线时产生中断
	
	//7.使能INT2
	qmi8658a_int2_config(INT2_Enable);	
	qmi8658a_int1_config(INT1_Enable);
		
	//8.重置FIFO并使能传感器，开始采样
	qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Cmd_Rst_Fifo);
	qmi8658a_acc_config(ACC_Enable);
	qmi8658a_gyr_config(Snooze_MODE_Disable,GYR_Enable);
	
	while(1)
	{
		//9.触发中断后一次性读完FIFO内数据
		if(FIFO_wtm_flag)
		{
			FIFO_wtm_flag=0;
			
			//a.先获取FIFO数据数量
			qmi8658a_FIFO_sample_count_get(&count);
			count=count/6;
			//水位线满时要读取64帧数据，双传感器情况下每帧六个轴共十二个字节，count读到的值应该为64(frames)*6(axis)=384
			
			//b.发送ctrl9指令，请求读指令
			qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Cmd_Req_Fifo);
			
			//c.读取fifo数据寄存器，每次读一帧（6axis/12byte）直至读完
			for(;count>0;count--)
			{
				//acc data
				qmi8658a_read_FIFO_frame_two_sensors(acc_xyz_rawdata,gyr_xyz_rawdata);
				
				qmi8658a_acc_rawdata_and_mg_convert(qmi8658aAccRange_4g,acc_xyz_rawdata,acc_xyz_mg);
				qmi8658a_gyr_rawdata_and_dps_convert(qmi8658aGyrRange_512dps,gyr_xyz_rawdata,gyr_xyz_dps);
				APP_LOG_INFO( "xyz\r\n");	
				APP_LOG_INFO( "x=%fmg,y=%fmg,z=%fmg\r\n",acc_xyz_mg[0],acc_xyz_mg[1],acc_xyz_mg[2]);	
				APP_LOG_INFO( "x=%fdps,y=%fdps,z=%fdps\r\n",gyr_xyz_dps[0],gyr_xyz_dps[1],gyr_xyz_dps[2]);	
			}	

			//d.读完后清空标志位，等待下一次FIFO数据再次超过水位线
			qmi8658a_FIFO_read_mode_clear();
		}
	}
}

//请在INT2引脚的中断服务中调用该函数
void FIFO_wtm_interrupt_callback()
{
	FIFO_wtm_flag=1;
}

/****************** Calibration example *******************/
void qmi8658a_acc_gry_calibration_example()
{
	signed short 	acc_xyz_rawdata[3]={0};
	signed short 	gyr_xyz_rawdata[3]={0};
	float acc_xyz_mg[3]={0};
	float gyr_xyz_dps[3]={0};
	float accel_calibration_sum[3] = {0};
	float gyro_calibration_sum[3] = {0};
	float offset_acc[3] = {0};
	float offset_gyro[3] = {0};
	unsigned char status0=0;
	float local_g=965.5;/**< 用户定义1G加速度，单位mg */
	unsigned char count=0;
	unsigned char i=0;
	
	//1.通信接口初始化
	qmi8658a_port_init();
	
	//2.芯片上电和复位后都需等待1.75s再操作寄存器
	qmi8658a_delay_ms(2000);
	qmi8658a_soft_reset();
	qmi8658a_delay_ms(2000);
	
	//3.使能寄存器地址自增
	qmi8658a_serial_interface_config(Enable_4_Wire_Spi,Addr_Auto_Increment,Big_Endian);
	
	//4.配置加速度计和陀螺仪量程和Odr
	qmi8658a_acc_setting(qmi8658aAccRange_4g, qmi8658aAccOdr_125Hz,Self_Test_Disable);
	qmi8658a_gyr_setting(qmi8658aGyrRange_512dps, qmi8658aGyrOdr_125Hz,Self_Test_Disable);
	
	//5.配置加速度计低通滤波器
	qmi8658a_acc_lpf_config(A_LSP_MODE_2,ACC_LPF_Enable);
	qmi8658a_gyr_lpf_config(G_LSP_MODE_2,GYR_LPF_Enable);
	
	//6.使能加速度计
	qmi8658a_acc_config(ACC_Enable);
	qmi8658a_gyr_config(Snooze_MODE_Disable,GYR_Enable);
	
	//7,开始校准，注意使能传感器后要等一会数据稳定后再采样
	APP_LOG_INFO( "calibration start\r\n");	
	qmi8658a_delay_ms(2000);	
	
	//a.水平放置传感器后，读取100帧数据并累加
	for(count=0;count<100;count++)
	{		
		qmi8658a_read_acc_rawdata(acc_xyz_rawdata);
		qmi8658a_read_gyr_rawdata(gyr_xyz_rawdata);
		qmi8658a_acc_rawdata_and_mg_convert(qmi8658aAccRange_4g,acc_xyz_rawdata,acc_xyz_mg);
		qmi8658a_gyr_rawdata_and_dps_convert(qmi8658aGyrRange_512dps,gyr_xyz_rawdata,gyr_xyz_dps);
			
		for(i=0;i<3;i++)
		{
			if(i==2)
			{
				acc_xyz_mg[i]-=local_g;/**< 水平放置时，Z轴会有一个G的加速度需减去（向上或者向下，具体数值由用户环境决定） */
			}
			accel_calibration_sum[i]+=acc_xyz_mg[i];
			gyro_calibration_sum[i]+=gyr_xyz_dps[i];
		}
		qmi8658a_delay_ms(10);
	}
	
	//b.100帧数据取平均，得到offset值
	for(i=0;i<3;i++)
	{
		offset_acc[i]=accel_calibration_sum[i]/100;
		offset_gyro[i]=gyro_calibration_sum[i]/100;
	}
	
	//c.校准完成后，用户应自行保存好校准值，供后续使用
	//StoreCalibrationToFile();
	APP_LOG_INFO( "calibration end\r\n");
	
	//8.校准完成，输出数据检查是否正常
	while(1)
	{	
		qmi8658a_readStatus0(&status0);
		if((status0&STATUS_aDA)==STATUS_aDA)
		{
			qmi8658a_read_acc_rawdata(acc_xyz_rawdata);		
			qmi8658a_read_gyr_rawdata(gyr_xyz_rawdata);		
			qmi8658a_acc_rawdata_and_mg_convert(qmi8658aAccRange_4g,acc_xyz_rawdata,acc_xyz_mg);
			qmi8658a_gyr_rawdata_and_dps_convert(qmi8658aGyrRange_512dps,gyr_xyz_rawdata,gyr_xyz_dps);
			
				for(i=0;i<3;i++)
			{
				acc_xyz_mg[i]-=offset_acc[i];
				gyr_xyz_dps[i]-=offset_gyro[i];
			}
			
			APP_LOG_INFO( "x=%fmg,y=%fmg,z=%fmg\r\n",acc_xyz_mg[0],acc_xyz_mg[1],acc_xyz_mg[2]);			
			APP_LOG_INFO( "x=%fdps,y=%fdps,z=%fdps\r\n",gyr_xyz_dps[0],gyr_xyz_dps[1],gyr_xyz_dps[2]);	
		}
		qmi8658a_delay_ms(500);
	}
}

/****************** AE example *******************/
void qmi8658a_AE_example()
{
	unsigned char status0=0;
	float quat[4]={0};
	float velocity[3]={0};
	
	//1.通信接口初始化
	qmi8658a_port_init();
	
	//2.芯片上电和复位后都需等待1.75s再操作寄存器
	qmi8658a_delay_ms(2000);
	qmi8658a_soft_reset();
	qmi8658a_delay_ms(2000);
	
	//3.使能寄存器地址自增
	qmi8658a_serial_interface_config(Enable_4_Wire_Spi,Addr_Auto_Increment,Big_Endian);
	
	//4.配置加速度计、陀螺仪、姿态引擎量程和Odr
	qmi8658a_acc_setting(qmi8658aAccRange_4g, qmi8658aAccOdr_500Hz,Self_Test_Disable);
	qmi8658a_gyr_setting(qmi8658aGyrRange_512dps, qmi8658aGyrOdr_500Hz,Self_Test_Disable);
	qmi8658a_ae_setting(qmi8658aAeOdr_16Hz);
	
	//5.配置加速度计低通滤波器
	qmi8658a_acc_lpf_config(A_LSP_MODE_2,ACC_LPF_Enable);
	qmi8658a_gyr_lpf_config(G_LSP_MODE_2,GYR_LPF_Enable);
	
	//6.使能传感器
	qmi8658a_acc_config(ACC_Enable);
	qmi8658a_gyr_config(Snooze_MODE_Disable,GYR_Enable);
	qmi8658a_ae_config(AE_Enable);
	
	while(1)
	{
		qmi8658a_readStatus0(&status0);
		if((status0&STATUS_sDA)==STATUS_sDA)
		{
			qmi8658a_read_AE_frame(quat, velocity);
			APP_LOG_INFO( "quat[0]=%f,quat[1]=%f,quat[2]=%f,quat[3]=%f\r\n",quat[0],quat[1],quat[2],quat[3]);			
			APP_LOG_INFO( "velocity[0]=%f,velocity[1]=%f,velocity[2]=%f\r\n",velocity[0],velocity[1],velocity[2]);	
			
			qmi8658a_delay_ms(500);
		}
	}

}	

/****************** Wake on Motion example*******************/
unsigned char WoM_flag=0;//WoM就绪标志

void qmi8658a_wake_on_motion_example()
{
	unsigned char reg_value;
	unsigned char blankingTime=0;
	unsigned count=0;
	
	//1.通信接口初始化
	qmi8658a_port_init();
	
	//2.芯片上电和复位后都需等待1.75s再操作寄存器
	qmi8658a_delay_ms(2000);
	qmi8658a_soft_reset();
	qmi8658a_delay_ms(2000);
	
	//3.使能寄存器地址自增
	qmi8658a_serial_interface_config(Enable_4_Wire_Spi,Addr_Auto_Increment,Big_Endian);
	
	//7.使能INT2
	qmi8658a_int2_config(INT2_Enable);	
	qmi8658a_int1_config(INT1_Enable);
    app_gpiote_config(&qmi_gpiote_param);
	
	//4.Entering Wake on Motion Mode
	//a.Disable sensors. (Write 0x00 to CTRL7)
	reg_value=0x00;
	qmi8658a_write_reg(qmi8658aRegister_Ctrl7, &reg_value,1);
	
	//b.Set Accelerometer sample rate and scale (Write CTRL2)
	qmi8658a_acc_setting(qmi8658aAccRange_8g, qmi8658aAccOdr_LowPower_21Hz,Self_Test_Disable);
	qmi8658a_acc_lpf_config(A_LSP_MODE_2,ACC_LPF_Disable);
	//the acc_odr should only to be set the lowpower mode
	
	//c.Set Wake on Motion (WoM) Threshold in CAL1_L;
	reg_value=(unsigned char)Qmi8658WomThreshold_mid;
	qmi8658a_write_reg(qmi8658aRegister_Cal1_L, &reg_value,1);
	
	//d.select interrupt, polarity and blanking time in CAL1_H
	blankingTime=100;
	reg_value=(unsigned char)qmi8658a_Int1_High | ( blankingTime & 0x3F);
	qmi8658a_write_reg(qmi8658aRegister_Cal1_H, &reg_value,1);
	//INT2，初始电平高，持续时间blankingTime 100
	
	//e.Execute CTRL9 command to configure WoM mode
	qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Cmd_WoM_Setting);

	//f.Set Accelerometer enable bit in CTRL7
	qmi8658a_acc_config(ACC_Enable);
	APP_LOG_INFO( "Wake on Motion enter\r\n");
	
	//5.等待中断发生,直到60秒后自动退出循环
	count=2*60;
	WoM_flag=0;
	while(count--)
	{
		qmi8658a_delay_ms(500);
		if(WoM_flag)
		{
			qmi8658a_readStatus1(&reg_value);
			if((reg_value&STATUS_WoM)==STATUS_WoM)
			{
				//中断发生后立刻结束循环，退出WoM模式
				APP_LOG_INFO( "Wake on Motion Interrup\r\n");
				WoM_flag=0;
				count=0;
			}
		}	
	}
	
	//6.Exiting Wake on Motion Mode
	reg_value=0x00;   
	qmi8658a_write_reg(qmi8658aRegister_Ctrl7, &reg_value,1);
	qmi8658a_write_reg(qmi8658aRegister_Cal1_L, &reg_value,1);
	qmi8658a_write_reg(qmi8658aRegister_Cal1_H, &reg_value,1);
	
	qmi8658a_ctrl9_cmd_send(qmi8658a_Ctrl9_Cmd_WoM_Setting);
	APP_LOG_INFO( "Wake on Motion exit\r\n");
	
	//7.用户设置为其他工作模式
	//user code
}
//请在INT2引脚的中断服务中调用该函数
void FIFO_WoM_interrupt_callback()
{
	WoM_flag=1;
}

/****************** 闭锁方式读取加速度计数据 *******************/
void qmi8658a_read_acc_in_locking_mechanical_mode(void)
{		
	unsigned short count=0x7ff;
	unsigned char status_int=0;;
	signed short 	acc_xyz_rawdata[3]={0};
	float acc_xyz_mg[3]={0};

	//1.通信接口初始化
	qmi8658a_port_init();
	
	//2.芯片上电和复位后都需等待1.75s再操作寄存器
	qmi8658a_delay_ms(2000);
	qmi8658a_soft_reset();
	qmi8658a_delay_ms(2000);
	
	//3.使能寄存器地址自增
	qmi8658a_serial_interface_config(Enable_4_Wire_Spi,Addr_Auto_Increment,Big_Endian);
	
	//4.配置加速度计量程和Odr
	qmi8658a_acc_setting(qmi8658aAccRange_4g, qmi8658aAccOdr_62_5Hz,Self_Test_Disable);
	
	//5.配置加速度计低通滤波器
	qmi8658a_acc_lpf_config(A_LSP_MODE_2,ACC_LPF_Enable);
	
	//6.使能闭锁
	qmi8658a_locking_mechanism_config(SyncSmpl_Enable,SysHS_Disable);
	
	//7.关闭AHB Clock gating(根据模式需求选择)
	qmi8658a_internal_AHB_clock_gating_config(qmi8658a_AHB_Disable);
	
	//8.使能加速度计
	qmi8658a_acc_config(ACC_Enable);
	
	while(count)
	{
		//9.检查StatusINT寄存器是否有数据锁住
		qmi8658a_readStatusInt(&status_int);
		if(status_int==(STATUS_Avail|STATUS_Locked))
		{
			//10.读取加速度数据
			qmi8658a_read_acc_rawdata(acc_xyz_rawdata);		
			qmi8658a_acc_rawdata_and_mg_convert(qmi8658aAccRange_4g,acc_xyz_rawdata,acc_xyz_mg);
			APP_LOG_INFO( "x=%fmg,y=%fmg,z=%fmg\r\n",acc_xyz_mg[0],acc_xyz_mg[1],acc_xyz_mg[2]);	
			count--;
		}
		qmi8658a_delay_ms(10);
	}
	
	//11.采样结束，关闭传感器并重新打开AHB时钟
	qmi8658a_acc_config(ACC_Disable);
	qmi8658a_locking_mechanism_config(SyncSmpl_Disable,SysHS_Disable);
	qmi8658a_internal_AHB_clock_gating_config(qmi8658a_AHB_Enable);
}
