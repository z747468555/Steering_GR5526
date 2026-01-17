#include "QMI8658A.h"
#include "app_i2c.h"
#include "IIC.h"
#define APP_LOG_TAG	"qmi"
#include "app_log.h"
#include "osal_task.h"
#if QMI8658A_SPI_I2C_SEL	
	

/****************** I2c操作接口 *******************/

/****************** I2c初始化 *******************/
unsigned char qmi8658a_I2c_Init()
{
	IIC_Init();
	return 0;
}

/****************** I2c写数据（多字节） *******************/
unsigned char qmi8658a_I2c_Write(unsigned char slave_addr,unsigned char reg, unsigned char* data,unsigned char length)
{
	uint16_t ret;
	uint8_t wdata[50];
	wdata[0] = reg;
	memcpy(wdata + 1, data, length);

    ret = IIC_Write(QMI_IIC_ID, slave_addr, wdata, length + 1);
    if (ret != APP_DRV_SUCCESS)
    {
//        APP_LOG_INFO(0, "I2C master send failed.\r\n");
        return 1;
    }
	
	//ret = app_i2c_mem_write_sync(QMI_IIC_ID,slave_addr,reg,1,data,length,600);
	return ret;
}

/****************** I2c读数据（多字节） *******************/
unsigned char qmi8658a_I2c_Read(unsigned char slave_addr,unsigned char reg,  unsigned char *data,unsigned char length)
{	
	uint16_t ret;
    ret = IIC_Write(QMI_IIC_ID, slave_addr, &reg, 1);
	  if (ret != APP_DRV_SUCCESS)
    {
        return 1;
    }
    ret = IIC_Read(QMI_IIC_ID, slave_addr, data, length);
		if (ret != APP_DRV_SUCCESS)
    {
        return 1;
    }

	//ret = app_i2c_mem_read_sync(QMI_IIC_ID,slave_addr,reg,1,data,length,600);
	return 0;
}

#else
/****************** SPI操作接口 *******************/

unsigned char qmi8658a_Spi_Init(void)
{
	 //user code
}

unsigned char qmi8658a_Spi_Write(unsigned char reg, unsigned char* data,unsigned char length)
{
	//user code
}
unsigned char qmi8658a_Spi_Read(unsigned char reg, unsigned char* data, unsigned char length)
{
	//user code
}

#endif

/****************** 毫秒级延时 *******************/
void	qmi8658a_delay_ms(unsigned long int ms)
{
	osal_task_delay_ms(ms);
}

