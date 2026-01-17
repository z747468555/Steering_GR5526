
#include "qmi8658.h"

#define QMI8658_SLAVE_ADDR_L			0x6a
#define QMI8658_SLAVE_ADDR_H			0x6b
#define qmi8658_printf					printf

//#define QMI8658_UINT_MG_DPS

enum
{
	AXIS_X = 0,
	AXIS_Y = 1,
	AXIS_Z = 2,

	AXIS_TOTAL
};

typedef struct 
{
	short 				sign[AXIS_TOTAL];
	unsigned short 		map[AXIS_TOTAL];
}qst_imu_layout;

static unsigned short acc_lsb_div = 0;
static unsigned short gyro_lsb_div = 0;
static unsigned short ae_q_lsb_div = (1 << 14);
static unsigned short ae_v_lsb_div = (1 << 10);
static unsigned int imu_timestamp = 0;
static struct Qmi8658Config qmi8658_config;
static unsigned char qmi8658_slave_addr = QMI8658_SLAVE_ADDR_L;

void Qmi8658_delay(int ms)
{
	int i,j;
	for(i=0;i<ms;i++)
	{
		for(j=0;j<1000;j++)
		{
			__NOP(); __NOP(); __NOP(); __NOP();
			__NOP(); __NOP(); __NOP(); __NOP();
			__NOP(); __NOP(); __NOP(); __NOP();
			__NOP(); __NOP(); __NOP(); __NOP();

			__NOP(); __NOP(); __NOP(); __NOP();
			__NOP(); __NOP(); __NOP(); __NOP();
			__NOP(); __NOP(); __NOP(); __NOP();
			__NOP(); __NOP(); __NOP(); __NOP();

			__NOP(); __NOP(); __NOP(); __NOP();
			__NOP(); __NOP(); __NOP(); __NOP();
		}
	}
}

unsigned char Qmi8658_write_reg(unsigned char reg, unsigned char value)
{
	unsigned char ret=0;
	unsigned int retry = 0;

	while((!ret) && (retry++ < 5))
	{
#if defined(QST_USE_SPI)
		ret = qst_fisimu_spi_write(reg,value);
#elif defined(QST_USE_SW_I2C)
		ret = qst_sw_writereg(qmi8658_slave_addr<<1, reg, value);
#else
		ret = mx_i2c1_write(qmi8658_slave_addr<<1, reg, value);
#endif
	}
	return ret;
}

unsigned char Qmi8658_write_regs(unsigned char reg, unsigned char *value, unsigned char len)
{
	unsigned char ret=0;	
	unsigned int retry = 0;

	while((!ret) && (retry++ < 5))
	{
#if defined(QST_USE_SPI)
		ret = qst_fisimu_spi_write_bytes(reg, value, len);
#elif defined(QST_USE_SW_I2C)
		ret = qst_sw_writeregs(qmi8658_slave_addr<<1, reg, value, len);
#else
		ret = I2C_BufferRead(qmi8658_slave_addr<<1, reg, value, len);
#endif
	}
	return ret;
}

unsigned char Qmi8658_read_reg(unsigned char reg, unsigned char* buf, unsigned short len)
{
	unsigned char ret=0;
	unsigned int retry = 0;

	while((!ret) && (retry++ < 5))
	{
#if defined(QST_USE_SPI)
		ret = qst_8658_spi_read(reg, buf, len);
#elif defined(QST_USE_SW_I2C)
		ret = qst_sw_readreg(qmi8658_slave_addr<<1, reg, buf, len);
#else
		ret = mx_i2c1_read(qmi8658_slave_addr<<1, reg, buf, len);
#endif
	}
	return ret;
}

#if 0
static qst_imu_layout imu_map;

void Qmi8658_set_layout(short layout)
{
	if(layout == 0)
	{
		imu_map.sign[AXIS_X] = 1;
		imu_map.sign[AXIS_Y] = 1;
		imu_map.sign[AXIS_Z] = 1;
		imu_map.map[AXIS_X] = AXIS_X;
		imu_map.map[AXIS_Y] = AXIS_Y;
		imu_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 1)
	{
		imu_map.sign[AXIS_X] = -1;
		imu_map.sign[AXIS_Y] = 1;
		imu_map.sign[AXIS_Z] = 1;
		imu_map.map[AXIS_X] = AXIS_Y;
		imu_map.map[AXIS_Y] = AXIS_X;
		imu_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 2)
	{
		imu_map.sign[AXIS_X] = -1;
		imu_map.sign[AXIS_Y] = -1;
		imu_map.sign[AXIS_Z] = 1;
		imu_map.map[AXIS_X] = AXIS_X;
		imu_map.map[AXIS_Y] = AXIS_Y;
		imu_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 3)
	{
		imu_map.sign[AXIS_X] = 1;
		imu_map.sign[AXIS_Y] = -1;
		imu_map.sign[AXIS_Z] = 1;
		imu_map.map[AXIS_X] = AXIS_Y;
		imu_map.map[AXIS_Y] = AXIS_X;
		imu_map.map[AXIS_Z] = AXIS_Z;
	}	
	else if(layout == 4)
	{
		imu_map.sign[AXIS_X] = -1;
		imu_map.sign[AXIS_Y] = 1;
		imu_map.sign[AXIS_Z] = -1;
		imu_map.map[AXIS_X] = AXIS_X;
		imu_map.map[AXIS_Y] = AXIS_Y;
		imu_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 5)
	{
		imu_map.sign[AXIS_X] = 1;
		imu_map.sign[AXIS_Y] = 1;
		imu_map.sign[AXIS_Z] = -1;
		imu_map.map[AXIS_X] = AXIS_Y;
		imu_map.map[AXIS_Y] = AXIS_X;
		imu_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 6)
	{
		imu_map.sign[AXIS_X] = 1;
		imu_map.sign[AXIS_Y] = -1;
		imu_map.sign[AXIS_Z] = -1;
		imu_map.map[AXIS_X] = AXIS_X;
		imu_map.map[AXIS_Y] = AXIS_Y;
		imu_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 7)
	{
		imu_map.sign[AXIS_X] = -1;
		imu_map.sign[AXIS_Y] = -1;
		imu_map.sign[AXIS_Z] = -1;
		imu_map.map[AXIS_X] = AXIS_Y;
		imu_map.map[AXIS_Y] = AXIS_X;
		imu_map.map[AXIS_Z] = AXIS_Z;
	}
	else		
	{
		imu_map.sign[AXIS_X] = 1;
		imu_map.sign[AXIS_Y] = 1;
		imu_map.sign[AXIS_Z] = 1;
		imu_map.map[AXIS_X] = AXIS_X;
		imu_map.map[AXIS_Y] = AXIS_Y;
		imu_map.map[AXIS_Z] = AXIS_Z;
	}
}
#endif

void Qmi8658_config_acc(enum Qmi8658_AccRange range, enum Qmi8658_AccOdr odr, enum Qmi8658_LpfConfig lpfEnable, enum Qmi8658_StConfig stEnable)
{
	unsigned char ctl_dada;

	switch(range)
	{
		case Qmi8658AccRange_2g:
			acc_lsb_div = (1<<14);
			break;
		case Qmi8658AccRange_4g:
			acc_lsb_div = (1<<13);
			break;
		case Qmi8658AccRange_8g:
			acc_lsb_div = (1<<12);
			break;
		case Qmi8658AccRange_16g:
			acc_lsb_div = (1<<11);
			break;
		default: 
			range = Qmi8658AccRange_8g;
			acc_lsb_div = (1<<12);
	}
	if(stEnable == Qmi8658St_Enable)
		ctl_dada = (unsigned char)range|(unsigned char)odr|0x80;
	else
		ctl_dada = (unsigned char)range|(unsigned char)odr;
		
	Qmi8658_write_reg(Qmi8658Register_Ctrl2, ctl_dada);
// set LPF & HPF
	Qmi8658_read_reg(Qmi8658Register_Ctrl5, &ctl_dada,1);
	ctl_dada &= 0xf0;
	if(lpfEnable == Qmi8658Lpf_Enable)
	{
		ctl_dada |= A_LSP_MODE_3;
		ctl_dada |= 0x01;
	}
	else
	{
		ctl_dada &= ~0x01;
	}
	ctl_dada = 0x00;
	Qmi8658_write_reg(Qmi8658Register_Ctrl5,ctl_dada);
// set LPF & HPF
}

void Qmi8658_config_gyro(enum Qmi8658_GyrRange range, enum Qmi8658_GyrOdr odr, enum Qmi8658_LpfConfig lpfEnable, enum Qmi8658_StConfig stEnable)
{
	// Set the CTRL3 register to configure dynamic range and ODR
	unsigned char ctl_dada; 

	// Store the scale factor for use when processing raw data
	switch (range)
	{
		case Qmi8658GyrRange_16dps:
			gyro_lsb_div = 2048;
			break;			
		case Qmi8658GyrRange_32dps:
			gyro_lsb_div = 1024;
			break;
		case Qmi8658GyrRange_64dps:
			gyro_lsb_div = 512;
			break;
		case Qmi8658GyrRange_128dps:
			gyro_lsb_div = 256;
			break;
		case Qmi8658GyrRange_256dps:
			gyro_lsb_div = 128;
			break;
		case Qmi8658GyrRange_512dps:
			gyro_lsb_div = 64;
			break;
		case Qmi8658GyrRange_1024dps:
			gyro_lsb_div = 32;
			break;
		case Qmi8658GyrRange_2048dps:
			gyro_lsb_div = 16;
			break;
//		case Qmi8658GyrRange_4096dps:
//			gyro_lsb_div = 8;
//			break;
		default: 
			range = Qmi8658GyrRange_512dps;
			gyro_lsb_div = 64;
			break;
	}

	if(stEnable == Qmi8658St_Enable)
		ctl_dada = (unsigned char)range|(unsigned char)odr|0x80;
	else
		ctl_dada = (unsigned char)range | (unsigned char)odr;
	Qmi8658_write_reg(Qmi8658Register_Ctrl3, ctl_dada);

// Conversion from degrees/s to rad/s if necessary
// set LPF & HPF
	Qmi8658_read_reg(Qmi8658Register_Ctrl5, &ctl_dada,1);
	ctl_dada &= 0x0f;
	if(lpfEnable == Qmi8658Lpf_Enable)
	{
		ctl_dada |= G_LSP_MODE_3;
		ctl_dada |= 0x10;
	}
	else
	{
		ctl_dada &= ~0x10;
	}
	ctl_dada = 0x00;
	Qmi8658_write_reg(Qmi8658Register_Ctrl5,ctl_dada);
// set LPF & HPF
}

void Qmi8658_config_mag(enum Qmi8658_MagDev device, enum Qmi8658_MagOdr odr)
{
	Qmi8658_write_reg(Qmi8658Register_Ctrl4, device|odr);	
}

void Qmi8658_config_ae(enum Qmi8658_AeOdr odr)
{
	//Qmi8658_config_acc(Qmi8658AccRange_8g, AccOdr_1000Hz, Lpf_Enable, St_Enable);
	//Qmi8658_config_gyro(Qmi8658GyrRange_2048dps, GyrOdr_1000Hz, Lpf_Enable, St_Enable);
	Qmi8658_config_acc(qmi8658_config.accRange, qmi8658_config.accOdr, Qmi8658Lpf_Enable, Qmi8658St_Disable);
	Qmi8658_config_gyro(qmi8658_config.gyrRange, qmi8658_config.gyrOdr, Qmi8658Lpf_Enable, Qmi8658St_Disable);
	Qmi8658_config_mag(qmi8658_config.magDev, qmi8658_config.magOdr);
	Qmi8658_write_reg(Qmi8658Register_Ctrl6, odr);
}

#if defined(QMI8658_USE_FIFO)
#if 0
void Qmi8658_calc_fifo_status_fss(int enable_num, unsigned short fifoSize)
{
	int total_num = 0;

	if(enable_num == 1)
	{
		total_num = fifoSize*1;
		switch(total_num)
		{
			case 16:
				qmi8658_config.fifo_fss = 2;
				break;
			case 32:
				qmi8658_config.fifo_fss = 4;
				break;
			case 64:
				qmi8658_config.fifo_fss = 8;
				break;
			case 128:
				qmi8658_config.fifo_fss = 16;
				break;
			default:
				qmi8658_config.fifo_fss = 0;
				break;
		}		
	}
	else if(enable_num == 2)
	{
		total_num = fifoSize*2;
		switch(total_num)
		{
			case 32:
				qmi8658_config.fifo_fss = 4;
				break;
			case 64:
				qmi8658_config.fifo_fss = 8;
				break;
			case 128:
				qmi8658_config.fifo_fss = 16;
				break;
			case 256:
				qmi8658_config.fifo_fss = 32;
				break;
			default:
				qmi8658_config.fifo_fss = 0;
				break;
		}
	}
}
#endif

unsigned short Qmi8658_config_fifo(enum Qmi8658_FifoWmkLevel watermark, enum Qmi8658_FifoSize size, enum Qmi8658_FifoMode mode)
{
	unsigned short fifoSize = 0;
	unsigned short watermarkLevel = 0;
	unsigned char	status1 = 0x00;
	unsigned short count=0;

	qmi8658_config.fifo_ctrl = (uint8_t)watermark | (uint8_t)size | (uint8_t)mode;
	// ctrl9 fifo reset
	Qmi8658_write_reg(Qmi8658Register_Ctrl9, Qmi8658_Ctrl9_Cmd_Rst_Fifo);
	while(((status1&QMI8658_STATUS1_CMD_DONE)==0)&&(count++<200))
	{
		Qmi8658_delay(2);
		Qmi8658_read_reg(Qmi8658Register_Status1, &status1, sizeof(status1));
	}
	// ctrl9 fifo reset
	Qmi8658_write_reg(Qmi8658Register_FifoCtrl, qmi8658_config.fifo_ctrl);

	switch(size)
	{
		case Qmi8658_Fifo_16:
			fifoSize = 16;
			break;
		case Qmi8658_Fifo_32:
			fifoSize = 32;
			break;
		case Qmi8658_Fifo_64:
			fifoSize = 64;
			break;
		case Qmi8658_Fifo_128:
			fifoSize = 128;
			break;
	}
	switch(watermark)
	{
		case Qmi8658_Fifo_WmkEmpty:
			watermarkLevel = 0;
			break;
		case Qmi8658_Fifo_WmkOneQuarter:
			watermarkLevel = fifoSize / 4;
			break;
		case Qmi8658_Fifo_WmkHalf:
			watermarkLevel = fifoSize / 2;
			break;
		case Qmi8658_Fifo_WmkThreeQuarters:
			watermarkLevel = (fifoSize * 3) / 4;
			break;
	}
	//Qmi8658_calc_fifo_status_fss(2, fifoSize);
	Qmi8658_write_reg(Qmi8658Register_FifoWmkTh, (unsigned char)watermarkLevel/2);

	if(mode == Qmi8658_Fifo_Bypass)
		return 0;
	else
		return watermarkLevel;
}

unsigned short Qmi8658_read_fifo(unsigned char* data)
{
	unsigned char fifo_status[2] = {0,0};
	unsigned char status1 = 0;
	unsigned short fifo_bytes = 0;
	unsigned short fifo_level = 0;
	unsigned short count;
	//unsigned short i;

	status1 = 0;
	Qmi8658_write_reg(Qmi8658Register_Ctrl9, Qmi8658_Ctrl9_Cmd_Req_Fifo);
	while(((status1&QMI8658_STATUS1_CMD_DONE)==0)&&(count++<200))
	{		
		Qmi8658_read_reg(Qmi8658Register_FifoCount, fifo_status, 2);
		fifo_bytes = (unsigned short)(((fifo_status[1]&0x03)<<8)|fifo_status[0]);
		fifo_level = fifo_bytes/12; // two sensor
		Qmi8658_read_reg(Qmi8658Register_Status1, &status1, 1);
		Qmi8658_delay(2);
	}
	qmi8658_printf("fifo status [0x%x 0x%x] bytes:%d level=%d\n", fifo_status[0],fifo_status[1],fifo_bytes,fifo_level);
		//Qmi8658_write_reg(Qmi8658Register_FifoCtrl, qmi8658_config.fifo_ctrl|0x80);		//read fifo
	if(fifo_level > 0)
	{
		#if 0
		for(i=1; i<fifo_level; i++)
		{
			Qmi8658_read_reg(Qmi8658Register_FifoData, data+i*12, 12);
		}
		#else
		Qmi8658_read_reg(Qmi8658Register_FifoData, data, 12*fifo_level);
		#endif
		Qmi8658_write_reg(Qmi8658Register_FifoCtrl, qmi8658_config.fifo_ctrl);
	}
	return fifo_level;
}

#endif

unsigned char Qmi8658_readStatus0(void)
{
	unsigned char status[2];

	Qmi8658_read_reg(Qmi8658Register_Status0, status, sizeof(status));
	//printf("status[0x%x	0x%x]\n",status[0],status[1]);

	return status[0];
}



unsigned char Qmi8658_readStatusInt(void)
{
	unsigned char status[2];

	Qmi8658_read_reg(Qmi8658Register_StatusInt, status, sizeof(status));
	//printf("status[0x%x	0x%x]\n",status[0],status[1]);

	return status[0];
}


/*!
 * \brief Blocking read of data status register 1 (::Qmi8658Register_Status1).
 * \returns Status byte \see STATUS1 for flag definitions.
 */
unsigned char Qmi8658_readStatus1(void)
{
	unsigned char status;
	
	Qmi8658_read_reg(Qmi8658Register_Status1, &status, sizeof(status));

	return status;
}

float Qmi8658_readTemp(void)
{
	unsigned char buf[2];
	short temp = 0;
	float temp_f = 0;

	Qmi8658_read_reg(Qmi8658Register_Tempearture_L, buf, 2);
	temp = ((short)buf[1]<<8)|buf[0];
	temp_f = (float)temp/256.0f;

	return temp_f;
}

void Qmi8658_read_acc_xyz(float acc_xyz[3])
{
	unsigned char	buf_reg[6];
	short 			raw_acc_xyz[3];

	Qmi8658_read_reg(Qmi8658Register_Ax_L, buf_reg, 6); 	// 0x19, 25
	raw_acc_xyz[0] = (short)((unsigned short)(buf_reg[1]<<8) |( buf_reg[0]));
	raw_acc_xyz[1] = (short)((unsigned short)(buf_reg[3]<<8) |( buf_reg[2]));
	raw_acc_xyz[2] = (short)((unsigned short)(buf_reg[5]<<8) |( buf_reg[4]));

	acc_xyz[0] = (raw_acc_xyz[0]*ONE_G)/acc_lsb_div;
	acc_xyz[1] = (raw_acc_xyz[1]*ONE_G)/acc_lsb_div;
	acc_xyz[2] = (raw_acc_xyz[2]*ONE_G)/acc_lsb_div;
}

void Qmi8658_read_gyro_xyz(float gyro_xyz[3])
{
	unsigned char	buf_reg[6];
	short 			raw_gyro_xyz[3];

	Qmi8658_read_reg(Qmi8658Register_Gx_L, buf_reg, 6);  	// 0x1f, 31
	raw_gyro_xyz[0] = (short)((unsigned short)(buf_reg[1]<<8) |( buf_reg[0]));
	raw_gyro_xyz[1] = (short)((unsigned short)(buf_reg[3]<<8) |( buf_reg[2]));
	raw_gyro_xyz[2] = (short)((unsigned short)(buf_reg[5]<<8) |( buf_reg[4]));	

	gyro_xyz[0] = (raw_gyro_xyz[0]*1.0f)/gyro_lsb_div;
	gyro_xyz[1] = (raw_gyro_xyz[1]*1.0f)/gyro_lsb_div;
	gyro_xyz[2] = (raw_gyro_xyz[2]*1.0f)/gyro_lsb_div;
}

void Qmi8658_read_xyz(float acc[3], float gyro[3], unsigned int *tim_count)
{
	unsigned char	buf_reg[12];
	short 			raw_acc_xyz[3];
	short 			raw_gyro_xyz[3];
//	float acc_t[3];
//	float gyro_t[3];

	if(tim_count)
	{
		unsigned char	buf[3];
		unsigned int timestamp;
		Qmi8658_read_reg(Qmi8658Register_Timestamp_L, buf, 3);	// 0x18	24
		timestamp = (unsigned int)(((unsigned int)buf[2]<<16)|((unsigned int)buf[1]<<8)|buf[0]);
		if(timestamp > imu_timestamp)
			imu_timestamp = timestamp;
		else
			imu_timestamp = (timestamp+0x1000000-imu_timestamp);

		*tim_count = imu_timestamp;		
	}
	
	Qmi8658_read_reg(Qmi8658Register_Ax_L, buf_reg, 12); 	// 0x19, 25
	raw_acc_xyz[0] = (short)((unsigned short)(buf_reg[1]<<8) |( buf_reg[0]));
	raw_acc_xyz[1] = (short)((unsigned short)(buf_reg[3]<<8) |( buf_reg[2]));
	raw_acc_xyz[2] = (short)((unsigned short)(buf_reg[5]<<8) |( buf_reg[4]));

	raw_gyro_xyz[0] = (short)((unsigned short)(buf_reg[7]<<8) |( buf_reg[6]));
	raw_gyro_xyz[1] = (short)((unsigned short)(buf_reg[9]<<8) |( buf_reg[8]));
	raw_gyro_xyz[2] = (short)((unsigned short)(buf_reg[11]<<8) |( buf_reg[10]));
	
	qmi8658_printf("%d %d %d %d %d %d\n", raw_acc_xyz[0], raw_acc_xyz[1], raw_acc_xyz[2], raw_gyro_xyz[0], raw_gyro_xyz[1], raw_gyro_xyz[2]);

#if defined(QMI8658_UINT_MG_DPS)
	// mg
	acc[AXIS_X] = (float)(raw_acc_xyz[AXIS_X]*1000.0f)/acc_lsb_div;
	acc[AXIS_Y] = (float)(raw_acc_xyz[AXIS_Y]*1000.0f)/acc_lsb_div;
	acc[AXIS_Z] = (float)(raw_acc_xyz[AXIS_Z]*1000.0f)/acc_lsb_div;
#else
	// m/s2
	acc[AXIS_X] = (float)(raw_acc_xyz[AXIS_X]*ONE_G)/acc_lsb_div;
	acc[AXIS_Y] = (float)(raw_acc_xyz[AXIS_Y]*ONE_G)/acc_lsb_div;
	acc[AXIS_Z] = (float)(raw_acc_xyz[AXIS_Z]*ONE_G)/acc_lsb_div;
#endif
//	acc[AXIS_X] = imu_map.sign[AXIS_X]*acc_t[imu_map.map[AXIS_X]];
//	acc[AXIS_Y] = imu_map.sign[AXIS_Y]*acc_t[imu_map.map[AXIS_Y]];
//	acc[AXIS_Z] = imu_map.sign[AXIS_Z]*acc_t[imu_map.map[AXIS_Z]];

#if defined(QMI8658_UINT_MG_DPS)
	// dps
	gyro[0] = (float)(raw_gyro_xyz[0]*1.0f)/gyro_lsb_div;
	gyro[1] = (float)(raw_gyro_xyz[1]*1.0f)/gyro_lsb_div;
	gyro[2] = (float)(raw_gyro_xyz[2]*1.0f)/gyro_lsb_div;
#else
	// rad/s
	gyro[AXIS_X] = (float)(raw_gyro_xyz[AXIS_X]*0.01745f)/gyro_lsb_div;		// *pi/180
	gyro[AXIS_Y] = (float)(raw_gyro_xyz[AXIS_Y]*0.01745f)/gyro_lsb_div;
	gyro[AXIS_Z] = (float)(raw_gyro_xyz[AXIS_Z]*0.01745f)/gyro_lsb_div;
#endif	
//	gyro[AXIS_X] = imu_map.sign[AXIS_X]*gyro_t[imu_map.map[AXIS_X]];
//	gyro[AXIS_Y] = imu_map.sign[AXIS_Y]*gyro_t[imu_map.map[AXIS_Y]];
//	gyro[AXIS_Z] = imu_map.sign[AXIS_Z]*gyro_t[imu_map.map[AXIS_Z]];
}


void Qmi8658_read_xyz_raw(short raw_acc_xyz[3], short raw_gyro_xyz[3], unsigned int *tim_count)
{
	unsigned char	buf_reg[12];

	if(tim_count)
	{
		unsigned char	buf[3];
		unsigned int timestamp;
		Qmi8658_read_reg(Qmi8658Register_Timestamp_L, buf, 3);	// 0x18	24
		timestamp = (unsigned int)(((unsigned int)buf[2]<<16)|((unsigned int)buf[1]<<8)|buf[0]);
		if(timestamp > imu_timestamp)
			imu_timestamp = timestamp;
		else
			imu_timestamp = (timestamp+0x1000000-imu_timestamp);

		*tim_count = imu_timestamp;	
	}
	if(raw_acc_xyz && raw_gyro_xyz)
	{
		Qmi8658_read_reg(Qmi8658Register_Ax_L, buf_reg, 12); 	// 0x19, 25
		raw_acc_xyz[0] = (short)((unsigned short)(buf_reg[1]<<8) |( buf_reg[0]));
		raw_acc_xyz[1] = (short)((unsigned short)(buf_reg[3]<<8) |( buf_reg[2]));
		raw_acc_xyz[2] = (short)((unsigned short)(buf_reg[5]<<8) |( buf_reg[4]));
		raw_gyro_xyz[0] = (short)((unsigned short)(buf_reg[7]<<8) |( buf_reg[6]));
		raw_gyro_xyz[1] = (short)((unsigned short)(buf_reg[9]<<8) |( buf_reg[8]));
		raw_gyro_xyz[2] = (short)((unsigned short)(buf_reg[11]<<8) |( buf_reg[10]));
	}
	else if(raw_acc_xyz)
	{
		Qmi8658_read_reg(Qmi8658Register_Ax_L, buf_reg, 6); 	// 0x19, 25
		raw_acc_xyz[0] = (short)((unsigned short)(buf_reg[1]<<8) |( buf_reg[0]));
		raw_acc_xyz[1] = (short)((unsigned short)(buf_reg[3]<<8) |( buf_reg[2]));
		raw_acc_xyz[2] = (short)((unsigned short)(buf_reg[5]<<8) |( buf_reg[4]));
	}
	else if(raw_gyro_xyz)
	{
		Qmi8658_read_reg(Qmi8658Register_Gx_L, buf_reg, 6); 	// 0x19, 25
		raw_gyro_xyz[0] = (short)((unsigned short)(buf_reg[1]<<8) |( buf_reg[0]));
		raw_gyro_xyz[1] = (short)((unsigned short)(buf_reg[3]<<8) |( buf_reg[2]));
		raw_gyro_xyz[2] = (short)((unsigned short)(buf_reg[5]<<8) |( buf_reg[4]));
	}
}


void Qmi8658_read_ae(float quat[4], float velocity[3])
{
	unsigned char	buf_reg[14];
	short 			raw_q_xyz[4];
	short 			raw_v_xyz[3];

	Qmi8658_read_reg(Qmi8658Register_Q1_L, buf_reg, 14);
	raw_q_xyz[0] = (short)((unsigned short)(buf_reg[1]<<8) |( buf_reg[0]));
	raw_q_xyz[1] = (short)((unsigned short)(buf_reg[3]<<8) |( buf_reg[2]));
	raw_q_xyz[2] = (short)((unsigned short)(buf_reg[5]<<8) |( buf_reg[4]));
	raw_q_xyz[3] = (short)((unsigned short)(buf_reg[7]<<8) |( buf_reg[6]));

	raw_v_xyz[1] = (short)((unsigned short)(buf_reg[9]<<8) |( buf_reg[8]));
	raw_v_xyz[2] = (short)((unsigned short)(buf_reg[11]<<8) |( buf_reg[10]));
	raw_v_xyz[2] = (short)((unsigned short)(buf_reg[13]<<8) |( buf_reg[12]));

	quat[0] = (float)(raw_q_xyz[0]*1.0f)/ae_q_lsb_div;
	quat[1] = (float)(raw_q_xyz[1]*1.0f)/ae_q_lsb_div;
	quat[2] = (float)(raw_q_xyz[2]*1.0f)/ae_q_lsb_div;
	quat[3] = (float)(raw_q_xyz[3]*1.0f)/ae_q_lsb_div;

	velocity[0] = (float)(raw_v_xyz[0]*1.0f)/ae_v_lsb_div;
	velocity[1] = (float)(raw_v_xyz[1]*1.0f)/ae_v_lsb_div;
	velocity[2] = (float)(raw_v_xyz[2]*1.0f)/ae_v_lsb_div;
}

void Qmi8658_enableWakeOnMotion(enum Qmi8658_Interrupt int_set, enum Qmi8658_WakeOnMotionThreshold threshold, unsigned char blankingTime)
{
	unsigned char cal1_1_reg = (unsigned char)threshold;
	unsigned char cal1_2_reg  = (unsigned char)int_set | (blankingTime & 0x3F);
	unsigned char status1 = 0;
	int count = 0;

	Qmi8658_write_reg(Qmi8658Register_Ctrl7, QMI8658_CTRL7_DISABLE_ALL);
	Qmi8658_config_acc(Qmi8658AccRange_8g, Qmi8658AccOdr_LowPower_21Hz, Qmi8658Lpf_Disable, Qmi8658St_Disable);

	Qmi8658_write_reg(Qmi8658Register_Cal1_L, cal1_1_reg);
	Qmi8658_write_reg(Qmi8658Register_Cal1_H, cal1_2_reg);
	// ctrl9 wom setting
	Qmi8658_write_reg(Qmi8658Register_Ctrl9, Qmi8658_Ctrl9_Cmd_WoM_Setting);
	while(((status1&QMI8658_STATUS1_CMD_DONE)==0)&&(count++<200))
	{
		Qmi8658_delay(2);
		Qmi8658_read_reg(Qmi8658Register_Status1, &status1, sizeof(status1));
	}
	// ctrl9 wom setting
	Qmi8658_write_reg(Qmi8658Register_Ctrl7, QMI8658_CTRL7_ACC_ENABLE);
}

void Qmi8658_disableWakeOnMotion(void)
{
	unsigned char status1 = 0;
	int count = 0;

	Qmi8658_write_reg(Qmi8658Register_Ctrl7, QMI8658_CTRL7_DISABLE_ALL);
	Qmi8658_write_reg(Qmi8658Register_Cal1_L, 0);
	Qmi8658_write_reg(Qmi8658Register_Cal1_H, 0);
	
	Qmi8658_write_reg(Qmi8658Register_Ctrl9, Qmi8658_Ctrl9_Cmd_WoM_Setting);
	while(((status1&QMI8658_STATUS1_CMD_DONE)==0)&&(count++<200))
	{
		Qmi8658_delay(2);
		Qmi8658_read_reg(Qmi8658Register_Status1, &status1, sizeof(status1));
	}
}

void Qmi8658_enableSensors(unsigned char enableFlags)
{
	if(enableFlags & QMI8658_CONFIG_AE_ENABLE)
	{
		enableFlags |= QMI8658_CTRL7_ACC_ENABLE | QMI8658_CTRL7_GYR_ENABLE;
	}

	Qmi8658_write_reg(Qmi8658Register_Ctrl7, enableFlags | 0x80);
}


void Qmi8658_Config_apply(struct Qmi8658Config const* config)
{
	unsigned char fisSensors = config->inputSelection;

	if(fisSensors & QMI8658_CONFIG_AE_ENABLE)
	{
		Qmi8658_config_ae(config->aeOdr);
	}
	else
	{
		if (config->inputSelection & QMI8658_CONFIG_ACC_ENABLE)
		{
			Qmi8658_config_acc(config->accRange, config->accOdr, Qmi8658Lpf_Disable, Qmi8658St_Disable);
		}
		if (config->inputSelection & QMI8658_CONFIG_GYR_ENABLE)
		{
			Qmi8658_config_gyro(config->gyrRange, config->gyrOdr, Qmi8658Lpf_Disable, Qmi8658St_Disable);
		}
	}

	if(config->inputSelection & QMI8658_CONFIG_MAG_ENABLE)
	{
		Qmi8658_config_mag(config->magDev, config->magOdr);
	}
#if defined(QMI8658_USE_FIFO)
	//Qmi8658_config_fifo(Qmi8658_Fifo_WmkHalf,Qmi8658_Fifo_64,Qmi8658_Fifo_Stream);
#endif
	Qmi8658_enableSensors(fisSensors);	
}

unsigned char Qmi8658_init(void)
{
	unsigned char qmi8658_chip_id = 0x00;
	unsigned char qmi8658_revision_id = 0x00;
	unsigned char qmi8658_slave[2] = {QMI8658_SLAVE_ADDR_L, QMI8658_SLAVE_ADDR_H};
	unsigned char iCount = 0;

	while((qmi8658_chip_id == 0x00)&&(iCount<2))
	{
		qmi8658_slave_addr = qmi8658_slave[iCount];
		Qmi8658_read_reg(Qmi8658Register_WhoAmI, &qmi8658_chip_id, 1);
		if(qmi8658_chip_id == 0x05)
			break;
		iCount++;
	}
	Qmi8658_read_reg(Qmi8658Register_Revision, &qmi8658_revision_id, 1);
	if(qmi8658_chip_id == 0x05)
	{
		qmi8658_printf("Qmi8658_init slave=0x%x  Qmi8658Register_WhoAmI=0x%x 0x%x\n", qmi8658_slave_addr,qmi8658_chip_id,qmi8658_revision_id);
		Qmi8658_write_reg(Qmi8658Register_Ctrl1, 0x60);
		qmi8658_config.inputSelection = QMI8658_CONFIG_ACCGYR_ENABLE;//QMI8658_CONFIG_ACCGYR_ENABLE;
		qmi8658_config.accRange = Qmi8658AccRange_2g;
		qmi8658_config.accOdr = Qmi8658AccOdr_500Hz;
		qmi8658_config.gyrRange = Qmi8658GyrRange_64dps;		//Qmi8658GyrRange_2048dps   Qmi8658GyrRange_1024dps
		qmi8658_config.gyrOdr = Qmi8658GyrOdr_500Hz;
		qmi8658_config.magOdr = Qmi8658MagOdr_125Hz;
		qmi8658_config.magDev = MagDev_AKM09918;
		qmi8658_config.aeOdr = Qmi8658AeOdr_128Hz;

		Qmi8658_Config_apply(&qmi8658_config);
		
		Qmi8658_write_reg(Qmi8658Register_Ctrl5, 0x55);
		
		if(1)
		{
			unsigned char read_data = 0x00;
			unsigned char firmware[3] = {0x00};
			Qmi8658_read_reg(Qmi8658Register_Ctrl1, &read_data, 1);
			qmi8658_printf("Qmi8658Register_Ctrl1=0x%x \n", read_data);
			Qmi8658_read_reg(Qmi8658Register_Ctrl2, &read_data, 1);
			qmi8658_printf("Qmi8658Register_Ctrl2=0x%x \n", read_data);
			Qmi8658_read_reg(Qmi8658Register_Ctrl3, &read_data, 1);
			qmi8658_printf("Qmi8658Register_Ctrl3=0x%x \n", read_data);
			Qmi8658_read_reg(Qmi8658Register_Ctrl4, &read_data, 1);
			qmi8658_printf("Qmi8658Register_Ctrl4=0x%x \n", read_data);
			Qmi8658_read_reg(Qmi8658Register_Ctrl5, &read_data, 1);
			qmi8658_printf("Qmi8658Register_Ctrl5=0x%x \n", read_data);
			Qmi8658_read_reg(Qmi8658Register_Ctrl6, &read_data, 1);
			qmi8658_printf("Qmi8658Register_Ctrl6=0x%x \n", read_data);
			Qmi8658_read_reg(Qmi8658Register_Ctrl7, &read_data, 1);
			qmi8658_printf("Qmi8658Register_Ctrl7=0x%x \n", read_data);
			
			Qmi8658_read_reg(0x49, firmware, 3);
			qmi8658_printf("Qmi8658_firmware =0x%x 0x%x 0x%x\n", firmware[0], firmware[1], firmware[2]);
		}
//		Qmi8658_set_layout(2);
		return 1;
	}
	else
	{
		qmi8658_printf("Qmi8658_init fail\n");
		qmi8658_chip_id = 0;
		return 0;
	}
	//return qmi8658_chip_id;
}

