/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_demo_user.c
 * 
 * @brief   gh3x2x driver lib demo code that user defined
 * 
 * @author  
 * 
 */

/* includes */
#include "stdint.h"
#include "string.h"
#include "gh_drv.h"
#include "gh_demo_config.h"
#include "gh_demo_inner.h"
#include "gh_demo.h"
#include "gh_demo_io_config.h"

#include "app_error.h"
#define APP_LOG_TAG "hrbo"
#include "app_log.h"
#include "osal_task.h"
#include "app_timer.h"
#include "SEGGER_RTT.h"
#include "WIFI.h"

#define GH_INT_IO_TYPE APP_IO_TYPE_AON
#define GH_INT_PIN     APP_IO_PIN_4
#define GH_INT_MODE    APP_IO_MODE_IT_RISING
#define GH_INT_PULL    APP_IO_PULLDOWN


#define GH_RST_IO_TYPE  APP_IO_TYPE_GPIOA
#define GH_RST_PIN      APP_IO_PIN_13
#define GH_RST_MUX      APP_IO_MUX_7
#define GH_RST_PULL     APP_IO_PULLUP

#define MASTER_DEV_ADDR	(0x38)
#define GH_I2C_ID   APP_I2C_ID_5

#define GH_SCL_PIN  APP_IO_PIN_14
#define GH_SCL_PORT APP_IO_TYPE_GPIOA
#define GH_SCL_MUX  APP_IO_MUX_1

#define GH_SDA_PIN  APP_IO_PIN_15
#define GH_SDA_PORT APP_IO_TYPE_GPIOA
#define GH_SDA_MUX  APP_IO_MUX_1
#define GH_DEFAULT_I2C_IO_CONFIG               { { GH_SCL_PORT, GH_SCL_MUX, GH_SCL_PIN, APP_IO_PULLUP }, \
                                                 { GH_SDA_PORT, GH_SDA_MUX, GH_SDA_PIN, APP_IO_PULLUP } }
#define GH_DEFAULT_I2C_MODE_CONFIG             { DMA0,DMA0, DMA_Channel2, DMA_Channel3 }
#define GH_DEFAULT_I2C_CONFIG                  { I2C_SPEED_1000K, MASTER_DEV_ADDR, I2C_ADDRESSINGMODE_7BIT, I2C_GENERALCALL_ENABLE}
#define GH_DEFAULT_I2C_PARAM_CONFIG            { GH_I2C_ID, APP_I2C_ROLE_MASTER, GH_DEFAULT_I2C_IO_CONFIG, GH_DEFAULT_I2C_MODE_CONFIG, GH_DEFAULT_I2C_CONFIG}


#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
#include "gh3x2x_demo_algo_call.h"
#endif

#if (__GH3X2X_MP_MODE__)
#include "gh3x2x_mp_common.h"
#endif

app_timer_id_t gh_multi_sensor_timer_id;
app_timer_id_t gh_serial_timer_id;
static GU32 snMultiSensorTimerPeriodMs = 0;
static GU32 snSerialSendTimerPeriodMs = 0;


extern uint8_t INT3220;
void gh3x2x_int_callback(app_io_evt_t *p_evt);
#if ( __GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__ )
app_io_init_t io_init = APP_IO_DEFAULT_CONFIG;
app_i2c_params_t gh_params_t = GH_DEFAULT_I2C_PARAM_CONFIG;
app_gpiote_param_t gh3x2x_int_param = {
    .io_evt_cb = gh3x2x_int_callback,
    .pin       = GH_INT_PIN,
    .mode      = GH_INT_MODE,
    .type      = GH_INT_IO_TYPE,
    .pull      = GH_INT_PULL
};

/* i2c interface */
/**
 * @fn     void hal_gh3x2x_i2c_init(void)
 * 
 * @brief  hal i2c init for gh3x2x
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gh3x2x_i2c_init(void)
{

    /* code implement by user */
	if (app_i2c_init(&gh_params_t, NULL) != 0)
	{
		GH3X2X_ERROR_LOG("i2c initial failed! Please check the input paraments.\r\n");
		return ;
    }
    GH3X2X_INFO_LOG("i2c initial successfully!\r\n");
}

/**
 * @fn     uint8_t hal_gh3x2x_i2c_write(uint8_t device_id, const uint8_t write_buffer[], uint16_t length)
 * 
 * @brief  hal i2c write for gh3x2x
 *
 * @attention   device_id is 8bits, if platform i2c use 7bits addr, should use (device_id >> 1)
 *
 * @param[in]   device_id       device addr
 * @param[in]   write_buffer    write data buffer
 * @param[in]   length          write data len
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_i2c_write(GU8 device_id, const GU8 write_buffer[], GU16 length)
{
    uint8_t ret = 1;
	
//	GH3X2X_INFO_LOG( "device_id=%d,len=%d,buffer:\r\n", device_id, length);
//	for(uint32_t i=0;i<length;i++)
//	{
//		GH3X2X_INFO_LOG( "%02x\r\n", write_buffer[i]);
//	}
//	Gh3x2x_BspDelayMs(100);

    ret = app_i2c_transmit_sync(GH_I2C_ID, (device_id >> 1), (uint8_t *) write_buffer, length, 100);
	if (ret)
	{
		GH3X2X_ERROR_LOG("i2c(%X) transmit failed(%d)! \r\n",device_id,ret);
		//Gh3x2x_BspDelayMs(100);
		return 0;
	}
    return !ret;
}

/**
 * @fn     uint8_t hal_gh3x2x_i2c_read(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length,
 *                            uint8_t read_buffer[], uint16_t read_length)
 * 
 * @brief  hal i2c read for gh3x2x
 *
 * @attention   device_id is 8bits, if platform i2c use 7bits addr, should use (device_id >> 1)
 *
 * @param[in]   device_id       device addr
 * @param[in]   write_buffer    write data buffer
 * @param[in]   write_length    write data len
 * @param[in]   read_length     read data len
 * @param[out]  read_buffer     pointer to read buffer
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_i2c_read(GU8 device_id, const GU8 write_buffer[], GU16 write_length, GU8 read_buffer[], GU16 read_length)
{
    uint8_t ret = 1;
    uint16_t i2c_addr = device_id >> 1;
    
    ret = app_i2c_transmit_sync(GH_I2C_ID, i2c_addr, (uint8_t *) write_buffer, write_length, 1000);
	if (ret)
	{
		GH3X2X_ERROR_LOG("i2c(%X) transmit failed(%d)! \r\n",device_id,ret);
		return 0;
	}
    ret = app_i2c_receive_sync(GH_I2C_ID, i2c_addr, read_buffer, read_length, 100);
	if (ret)
	{
		GH3X2X_ERROR_LOG("i2c(%X) receive failed(%d)! \r\n",device_id,ret);
		return 0;
	}
    return !ret;
}

#else // __GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_SPI__
app_spi_params_t gh_spi_params = GH_DEFAULT_PARAM_CONFIG;

volatile uint8_t g_master_tdone = 0;
volatile uint8_t g_master_rdone = 0;
        
static void gh3x2x_spi_master_callback(app_spi_evt_t *p_evt)
{
    if (p_evt->type == APP_SPI_EVT_TX_CPLT)
    {
        g_master_tdone = 1;
    }
    if (p_evt->type == APP_SPI_EVT_RX_CPLT)
    {
        g_master_rdone = 1;
    }
    if (p_evt->type == APP_SPI_EVT_TX_RX_CPLT)
    {
        g_master_tdone = 1;
        g_master_rdone = 1;
    }
    if (p_evt->type == APP_SPI_EVT_ERROR)
    {
        g_master_tdone = 1;
        g_master_rdone = 1;
    }
}
/* spi interface */
/**
 * @fn     void hal_gh3x2x_spi_init(void)
 * 
 * @brief  hal spi init for gh3x2x
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */

void hal_gh3x2x_spi_init(void)
{
    app_drv_err_t ret = 0;
    ret = app_spi_init(&gh_spi_params, gh3x2x_spi_master_callback);
    if (ret != 0)
    {
        GH3X2X_ERROR_LOG("SPI master initial failed! Please check the input paraments.\r\n");
        return;
    }

    ret = app_spi_dma_init(&gh_spi_params);
    if (ret != 0)
    {
        GH3X2X_ERROR_LOG("SPI master dma initial failed! Please check the input paraments.\r\n");
        return;
    }


#if (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__)
    
    app_io_init_t io_init = APP_IO_DEFAULT_CONFIG;

    io_init.pull = APP_IO_PULLUP;
    io_init.mode = APP_IO_MODE_OUTPUT;
    io_init.pin  = GH_CS_PIN;
    io_init.mux  = GH_CS_PINMUX;
    app_io_init(GH_CS_IO_TYPE, &io_init);
#endif
    GH3X2X_INFO_LOG("spi initial successfully!\r\n");
}

/**
 * @fn     GU8 hal_gh3x2x_spi_write(GU8 write_buffer[], GU16 length)
 * 
 * @brief  hal spi write for gh3x2x
 *
 * @attention   if __GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__  , user need generate timming: write_buf[0](W) + write_buf[1](W) + ...
 * @attention   if __GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_HARDWARE_CS__  , user need generate timming: CS LOW  + write_buf[0](W) + write_buf[1](W) + ... + CS HIGH
 *
 * @param[in]   write_buffer    write data buffer
 * @param[in]   length          write data len
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_spi_write(GU8 write_buffer[], GU16 length)
{
    GU8 ret = 1;
    g_master_tdone = 0;
    ret = app_spi_dma_transmit_async(APP_SPI_ID_MASTER, write_buffer, length);
    if (ret)
    {
        GH3X2X_ERROR_LOG("spi write error. ret = %d.\r\n", ret);
        return 0;
    }
    while (!g_master_tdone);

    return !ret;
}


#if (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__) 
/**
 * @fn     GU8 hal_gh3x2x_spi_read(GU8 read_buffer[], GU16 length)
 * 
 * @brief  hal spi read for gh3x2x
 *
 * @attention   user need generate timming: read_buf[0](R) + write_buf[1](R) + ... 
 *
 * @param[in]   read_length     read data len
 * @param[out]  read_buffer     pointer to read buffer
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_spi_read(GU8 read_buffer[], GU16 length)
{
    GU8 ret = 1;

    g_master_rdone = 0;
    ret = app_spi_dma_receive_async(APP_SPI_ID_MASTER, read_buffer, length);
    if (ret)
    {
        GH3X2X_ERROR_LOG("spi read error, err = %d \r\n", ret);
        return 0;
    }
    while (!g_master_rdone);

    return !ret;
}
#elif (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_HARDWARE_CS__)
/**
 * @fn     GU8 hal_gh3x2x_spi_write_F1_and_read(GU8 read_buffer[], GU16 length)
 * 
 * @brief  hal spi write F1 and read for gh3x2x
 *
 * @attention    user need generate timming: CS LOW + F1(W) + read_buf[0](R) + read_buf[1](R) + ... + CS HIGH
 *
 * @param[in]   write_buf     write data
 * @param[in]   length     write data len
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_spi_write_F1_and_read(GU8 read_buffer[], GU16 length)
{
    GU8 ret = 1;
    GU8 write_buffer[1] = {0xF1};
    g_master_rdone = 0;
    ret = app_spi_dma_read_eeprom_async(APP_SPI_ID_MASTER, write_buffer, read_buffer, 1, length);
    if (ret)
    {
        GH3X2X_ERROR_LOG("spi read error, err = %d \r\n", ret);
        return 0;
    }
    while (!g_master_rdone);
    return !ret;
}
#endif

/**
 * @fn     void hal_gh3x2x_spi_cs_ctrl(GU8 cs_pin_level)
 * 
 * @brief  hal spi cs pin ctrl for gh3x2x
 *
 * @attention   pin level set 1 [high level] or 0 [low level]
 *
 * @param[in]   cs_pin_level     spi cs pin level
 * @param[out]  None
 *
 * @return  None
 */
#if (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__) 
void hal_gh3x2x_spi_cs_ctrl(GU8 cs_pin_level)
{

    app_io_write_pin(GH_CS_IO_TYPE, GH_CS_PIN, cs_pin_level?APP_IO_PIN_SET:APP_IO_PIN_RESET);

}
#endif

#endif

#if __SUPPORT_HARD_RESET_CONFIG__

/**
 * @fn     void hal_gh3x2x_int_init(void)
 * 
 * @brief  gh3x2x int init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */



void hal_gh3x2x_reset_pin_init(void)
{
    app_io_init_t io_init = APP_IO_DEFAULT_CONFIG;

    io_init.pull = APP_IO_PULLUP;
    io_init.mode = APP_IO_MODE_OUTPUT;
    io_init.pin  = GH_RST_PIN;
    io_init.mux  = GH_RST_MUX;
    app_io_init(GH_RST_IO_TYPE, &io_init);

    app_io_write_pin(GH_RST_IO_TYPE, GH_RST_PIN, APP_IO_PIN_RESET);
    Gh3x2x_BspDelayMs(10);
    app_io_write_pin(GH_RST_IO_TYPE, GH_RST_PIN, APP_IO_PIN_SET);
}

/**
 * @fn     void hal_gh3x2x_reset_pin_ctrl(GU8 pin_level)
 * 
 * @brief  hal reset pin ctrl for gh3x2x
 *
 * @attention   pin level set 1 [high level] or 0 [low level]
 *
 * @param[in]   pin_level     reset pin level
 * @param[out]  None
 *
 * @return  None
 */

void hal_gh3x2x_reset_pin_ctrl(GU8 pin_level)
{
   app_io_write_pin(GH_RST_IO_TYPE, GH_RST_PIN, pin_level?APP_IO_PIN_SET:APP_IO_PIN_RESET);
}

#endif

void hal_gh3x2x_reset_pin_high(void)
{
	app_io_init_t io_init = APP_IO_DEFAULT_CONFIG;

	io_init.pull = APP_IO_PULLUP;
	io_init.mode = APP_IO_MODE_OUTPUT;
	io_init.pin  = GH_RST_PIN;
	io_init.mux  = GH_RST_MUX;
	app_io_init(GH_RST_IO_TYPE, &io_init);
	app_io_write_pin(GH_RST_IO_TYPE, GH_RST_PIN, APP_IO_PIN_SET);
}

void gh3220_power_off(void)
{
	app_io_write_pin(GH_RST_IO_TYPE, GH_RST_PIN, APP_IO_PIN_RESET);
}

void gh3220_reset(void)
{
	app_io_write_pin(GH_RST_IO_TYPE, GH_RST_PIN, APP_IO_PIN_RESET);
	osal_task_delay(1000);
	app_io_write_pin(GH_RST_IO_TYPE, GH_RST_PIN, APP_IO_PIN_SET);
}

extern void gh3x2x_interupt_request(void);
void gh3x2x_int_callback(app_io_evt_t *p_evt)
{
#if (__GH3X2X_MP_MODE__)
    extern void gh3x2x_mp_int_handler_call_back(void);
    gh3x2x_mp_int_handler_call_back();
#endif
#if (__NORMAL_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__ || __MIX_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__)
        hal_gh3x2x_int_handler_call_back();
        INT3220 = 1;
			app_uart_transmit_sync(WIFI_UART_ID,(uint8_t*)"1\r\n", strlen("1\r\n"),5);
#endif
}

/**
 * @fn     void hal_gh3x2x_int_init(void)
 * 
 * @brief  gh3x2x int init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gh3x2x_int_init(void)
{
    app_gpiote_init(&gh3x2x_int_param, 1);
}

#if (__NORMAL_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__ || __MIX_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__)
/**
 * @fn     void hal_gh3x2x_int_handler_call_back(void)
 * 
 * @brief  call back of gh3x2x int handler
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gh3x2x_int_handler_call_back(void)
{
    if (Gh3x2xGetInterruptMode() == __NORMAL_INT_PROCESS_MODE__)
    {
        if(GH3X2X_DEMO_WORK_MODE_MPT != g_uchDemoWorkMode)
        {
            g_uchGh3x2xIntCallBackIsCalled = 1;
        }
#if (__GH3X2X_MP_MODE__)
        GH3X2X_MP_SET_INT_FLAG();  //gh3x2x mp test must call it
#endif
        GOODIX_PLATFORM_INT_HANDLER_CALL_BACK_ENTITY();
    }
}
#endif

/**
 * @fn     void hal_gsensor_start_cache_data(void)
 * 
 * @brief  Start cache gsensor data for gh3x2x
 *
 * @attention   This function will be called when start gh3x2x sampling.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gsensor_start_cache_data(void)
{
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    GH3X2X_TimestampSyncAccInit();
#endif
#ifdef SL_SC7A20E
    if (SL_SC7A20E_ERROR != SC7A20E_Init_Status)
    {
        SL_SC7A20E_Odr_Config(SL_SC7A20E_ODR_25HZ,SL_SC7A20E_NormalPower);
        SL_SC7A20E_Read_XYZ_Data_FIFO(sc7a20e_gsensor_buffer);
        EXAMPLE_LOG("hal_gsensor_start_cache_data\r\n");
    }
#endif
}

/**
 * @fn     void hal_gsensor_stop_cache_data(void)
 * 
 * @brief  Stop cache gsensor data for gh3x2x
 *
 * @attention   This function will be called when stop gh3x2x sampling.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gsensor_stop_cache_data(void)
{
#ifdef SL_SC7A20E
    if (SL_SC7A20E_ERROR != SC7A20E_Init_Status)
    {
        SL_SC7A20E_Odr_Config(SL_SC7A20E_ODR_25HZ,SL_SC7A20E_LowPower);
        EXAMPLE_LOG("hal_gsensor_stop_cache_data\r\n");
    }
#endif
}

void hal_cap_start_cache_data(void)
{
    GOODIX_PLATFORM_INT_CAP_START_CACHE_ENTITY();
}

void hal_cap_stop_cache_data(void)
{
    GOODIX_PLATFORM_INT_CAP_STOP_CACHE_ENTITY();
}

void hal_temp_start_cache_data(void)
{
    GOODIX_PLATFORM_INT_TEMP_START_CACHE_ENTITY();
}

void hal_temp_stop_cache_data(void)
{
    GOODIX_PLATFORM_INT_TEMP_STOP_CACHE_ENTITY();
}

void hal_gh3x2x_write_cap_to_flash(GS32 WearCap1,GS32 UnwearCap1,GS32 WearCap2,GS32 UnwearCap2)
{
    GOODIX_PLATFORM_WRITE_CAP_TO_FLASH_ENTITY();
}
void hal_gh3x2x_read_cap_from_flash(GS32* WearCap1,GS32* UnwearCap1,GS32* WearCap2,GS32* UnwearCap2)
{
    GOODIX_PLATFORM_READ_CAP_FROM_FLASH_ENTITY();
}

/**
 * @fn     void hal_gsensor_drv_get_fifo_data(STGsensorRawdata gsensor_buffer[], GU16 *gsensor_buffer_index)
 * 
 * @brief  get gsensor fifo data
 *
 * @attention   When read fifo data of GH3x2x, will call this function to get corresponding cached gsensor data.
 *
 * @param[in]   None
 * @param[out]  gsensor_buffer          pointer to gsensor data buffer
 * @param[out]  gsensor_buffer_index    pointer to number of gsensor data(every gsensor data include x,y,z axis data)
 *
 * @return  None
 */
uint8_t test_cnt = 0;
void hal_gsensor_drv_get_fifo_data(STGsensorRawdata gsensor_buffer[], GU16 *gsensor_buffer_index)
{
/**************************** WARNNING  START***************************************************/
/*  (*gsensor_buffer_index) can not be allowed bigger than __GSENSOR_DATA_BUFFER_SIZE__  ****************/
/* Be care for copying data to gsensor_buffer, length of gsensor_buffer is __GSENSOR_DATA_BUFFER_SIZE__ *****/
/**************************** WARNNING END*****************************************************/

#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    for (int i = 0; i < *gsensor_buffer_index; i++)
    {
        GU32 unTimeStamp = 0;
        /* user set time stamp code here */
        GH3X2X_TimestampSyncFillAccSyncBuffer(unTimeStamp, gsensor_buffer[i].sXAxisVal, gsensor_buffer[i].sYAxisVal, gsensor_buffer[i].sZAxisVal);
    }
#endif
#ifdef SL_SC7A20E
    if (SL_SC7A20E_ERROR != SC7A20E_Init_Status)
    {
        *gsensor_buffer_index = SL_SC7A20E_Read_XYZ_Data_FIFO(sc7a20e_gsensor_buffer);
        EXAMPLE_LOG("hal_gsensor_drv_get_fifo_data = %d\r\n", *gsensor_buffer_index);
        for (int i = 0; i < *gsensor_buffer_index; i++)
        {
            gsensor_buffer[i].sXAxisVal = SL_SC7A20E_DATA_CONVERT(sc7a20e_gsensor_buffer[i*3+0], SL_SC7A20E_FS_4G_RATIO);
            gsensor_buffer[i].sYAxisVal = SL_SC7A20E_DATA_CONVERT(sc7a20e_gsensor_buffer[i*3+1], SL_SC7A20E_FS_4G_RATIO);
            gsensor_buffer[i].sZAxisVal = SL_SC7A20E_DATA_CONVERT(sc7a20e_gsensor_buffer[i*3+2], SL_SC7A20E_FS_4G_RATIO);
            #if __GS_GYRO_ENABLE__
            gsensor_buffer[i].sXGyroVal = i;
            gsensor_buffer[i].sYGyroVal = 255-i;
            gsensor_buffer[i].sZGyroVal = ;
            #endif
        }
    }
    else
#endif
    {
//        test_cnt ++;
        if (test_cnt > 150)
        {
            test_cnt = 0;
        }
        *gsensor_buffer_index = __GSENSOR_DATA_BUFFER_SIZE__;
        for (int i = 0; i < *gsensor_buffer_index; i++)
        {
            gsensor_buffer[i].sXAxisVal = 0;
            gsensor_buffer[i].sYAxisVal = 0;
            gsensor_buffer[i].sZAxisVal = 512;
            #if __GS_GYRO_ENABLE__
            gsensor_buffer[i].sXGyroVal = i;
            gsensor_buffer[i].sYGyroVal = 255-i;
            gsensor_buffer[i].sZGyroVal = i;
            #endif
        }
    }
/**************************** WARNNING: DO NOT REMOVE OR MODIFY THIS CODE   ---START***************************************************/
    if((*gsensor_buffer_index) > (__GSENSOR_DATA_BUFFER_SIZE__))
    {
        while(1);   // Fatal error !!!
    }
/**************************** WARNNING: DO NOT REMOVE OR MODIFY THIS CODE   ---END***************************************************/
}

void hal_cap_drv_get_fifo_data(STCapRawdata cap_data_buffer[], GU16 *cap_buffer_index)
{
    GOODIX_PLATFORM_INT_GET_CAP_DATA_ENTITY()  ;
    if((*cap_buffer_index) > (__CAP_DATA_BUFFER_SIZE__))
    {
        while(1);   // Fatal error !!!
    }
}

void hal_temp_drv_get_fifo_data(STTempRawdata temp_data_buffer[], GU16 *temp_buffer_index)
{
    GOODIX_PLATFORM_INT_GET_TEMP_DATA_ENTITY();
    if((*temp_buffer_index) > (__TEMP_DATA_BUFFER_SIZE__))
    {
        while(1);   // Fatal error !!!
    }
}

#if (__GH3X2X_INFO_LOG_TYPE__)
/**
 * @fn     void GH3X2X_PlatformLog(char *log_string)
 * 
 * @brief  for debug version, log
 *
 * @attention   this function must define that use debug version lib
 *
 * @param[in]   log_string      pointer to log string
 * @param[out]  None
 *
 * @return  None
 */

#if __GH3X2X_INFO_LOG_TYPE__ == __GH3X2X_INFO_LOG_METHOD_0__
void GH3X2X_PlatformLog(GCHAR *log_string)
{
	printf("%s", log_string);
}
#endif

#if __GH3X2X_INFO_LOG_TYPE__ == __GH3X2X_INFO_LOG_METHOD_1__
void GH3X2X_RegisterPrintf(int (**pPrintfUser)(const char *format, ...))
{
    //(*pPrintfUser) = printf;   //use printf in <stdio.h>  or use equivalent function in your platform
    GOODIX_PLATFORM_PRINTF_ENTITY();
}
#endif




#endif

/**
 * @fn     void Gh3x2x_BspDelayUs(GU16 usUsec)
 * 
 * @brief  Delay in us,user should register this function into driver lib
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */

void Gh3x2x_BspDelayUs(GU16 usUsec)
{
    delay_us(usUsec);
}


/**
 * @fn     void Gh3x2x_BspDelayMs(GU16 usMsec)
 * 
 * @brief  Delay in ms
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2x_BspDelayMs(GU16 usMsec)
{
    delay_ms(usMsec);
}

#if (__GH_MULTI_SENSOR_EVENT_PRO_CONIG__)

void GhMultiSensorTimerHandler(void* p_ctx)
{
    GhMultiSensorTimerHandle();
}

void Gh3x2x_CreateMultiSensorTimer(GU32 unPeriodMs)
{
    snMultiSensorTimerPeriodMs = unPeriodMs;
    app_timer_create(&gh_multi_sensor_timer_id, ATIMER_REPEAT, GhMultiSensorTimerHandler);
}



/**
 * @fn     void Gh3x2x_StartMultiSensorTimer(void)
 * 
 * @brief  Start time of adt confirm to get g sensor
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2x_StartMultiSensorTimer(void)
{
    if(snMultiSensorTimerPeriodMs)
    {
        GH3X2X_DEBUG_LOG("Gh3x2x_StartMultiSensorTimer\r\n");
        app_timer_start(gh_multi_sensor_timer_id, snMultiSensorTimerPeriodMs, NULL);
    }
}


/**
 * @fn     void Gh3x2x_StopMultiSensorTimer(void)
 * 
 * @brief  Stop time of adt confirm
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2x_StopMultiSensorTimer(void)
{
    if(snMultiSensorTimerPeriodMs)
    {
        GH3X2X_DEBUG_LOG("Gh3x2x_StopMultiSensorTimer\r\n");
        app_timer_stop(gh_multi_sensor_timer_id);
    }
}

void Gh3x2x_DeleteMultiSensorTimer(void)
{
    snMultiSensorTimerPeriodMs = 0;
}


const STGhMultiSensorTimerOps g_stGhMultiSensorTimerOps =
{
    .TimerCreate = Gh3x2x_CreateMultiSensorTimer,
    .TimerStart = Gh3x2x_StartMultiSensorTimer,
    .TimerStop = Gh3x2x_StopMultiSensorTimer,
    .TimerDelete = Gh3x2x_DeleteMultiSensorTimer,
};
#endif




/**
 * @fn     void Gh3x2x_UserHandleCurrentInfo(void)
 * 
 * @brief  handle gh3x2x chip current information for user
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2x_UserHandleCurrentInfo(void)
{
    //read or write  slotx current

    //GH3X2X_GetSlotLedCurrent(0,0); //read slot 0  drv 0
    //GH3X2X_GetSlotLedCurrent(1,0); // read  slot 1  drv 0

    //GH3X2X_SlotLedCurrentConfig(0,0,50);  //set slot0 drv0 50 LSB
    //GH3X2X_SlotLedCurrentConfig(1,0,50);  //set slot1 drv0 50 LSB
}

#if (__SUPPORT_PROTOCOL_ANALYZE__)
/**
 * @fn     void Gh3x2x_HalSerialSendData(GU8* uchTxDataBuf, GU16 usBufLen)
 *
 * @brief  Serial send data
 *
 * @attention   None
 *
 * @param[in]   uchTxDataBuf        pointer to data buffer to be transmitted
 * @param[in]   usBufLen            data buffer length
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2x_HalSerialSendData(GU8* uchTxDataBuf, GU16 usBufLen)
{
    sdk_err_t error_code;
    extern sdk_err_t health_tx_data_send(uint8_t conn_idx, uint8_t *p_data, uint16_t length);
    health_tx_data_send(0, uchTxDataBuf, usBufLen);
#if 0
    APP_LOG_INFO("Send Data: ");
    APP_LOG_HEX_DUMP(uchTxDataBuf, usBufLen);
#endif
}

void GhSerialSendTimerHandler(void* p_ctx)
{
    Gh3x2xSerialSendTimerHandle();
}

/**
 * @fn      void Gh3x2xSerialSendTimerInit(GU8 uchPeriodMs)
 *
 * @brief  Gh3x2xSerialSendTimerInit
 *
 * @attention   None
 *
 * @param[in]   uchPeriodMs    timer period (ms)
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xSerialSendTimerInit(GU8 uchPeriodMs)
{
    snSerialSendTimerPeriodMs = uchPeriodMs;
    
    app_timer_create(&gh_serial_timer_id, ATIMER_REPEAT, GhSerialSendTimerHandler);
}


/**
 * @fn     void Gh3x2xSerialSendTimerStop(void)
 *
 * @brief  Serial send timer stop
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xSerialSendTimerStop(void)
{
    if(snSerialSendTimerPeriodMs)
    {
        GH3X2X_DEBUG_LOG("Gh3x2x_StopSerialSendTimer\r\n");
        app_timer_stop(gh_serial_timer_id);
    }
}



/**
 * @fn     void Gh3x2xSerialSendTimerStart(void)
 *
 * @brief  Serial send timer start
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xSerialSendTimerStart(void)
{
    if(snSerialSendTimerPeriodMs)
    {
        GH3X2X_DEBUG_LOG("Gh3x2x_StartSerialSendTimer\r\n");
        app_timer_start(gh_serial_timer_id, snSerialSendTimerPeriodMs, NULL);
    }
}

#endif





void* Gh3x2xMallocUser(GU32 unMemSize)
{
#if (__USER_DYNAMIC_DRV_BUF_EN__)
#ifdef GOODIX_DEMO_PLATFORM
    GOODIX_PLATFORM_MALLOC_USER_ENTITY();
#else
    //return malloc(unMemSize);
    return 0;
#endif
#else
    return 0;
#endif
}

void Gh3x2xFreeUser(void* MemAddr)
{
#if (__USER_DYNAMIC_DRV_BUF_EN__)
    GOODIX_PLATFORM_FREE_USER_ENTITY();
    //free(MemAddr);
#endif
}









#if ( __FUNC_TYPE_BP_ENABLE__ && (__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__))
/**
 * @fn     GS8 gh3x2x_write_pressure_parameters(GS32 *buffer)
 * 
 * @brief  gh3x2x get pressure value
 *
 * @attention   None
 *
 * @param[out]   buffer     buffer[0] = rawdata_a0,
 *                          buffer[1] = rawdata_a1,
 *                          buffer[2] = pressure g value of rawdata_a1,
 *
 * @return  error code
 */
GS8 gh3x2x_write_pressure_parameters(GS32 *buffer)
{
    //return GOODIX_PLATFORM_PRESSURE_PARAS_WRITE_ENTITY();
	return 10;
}

/**
 * @fn     GS8 gh3x2x_read_pressure_parameters(GS32 *buffer)
 * 
 * @brief  gh3x2x get pressure value
 *
 * @attention   None
 *
 * @param[out]   buffer     buffer[0] = area,
 *                          buffer[1] = rawdata_a0,
 *                          buffer[2] = rawdata_a1,
 *                          buffer[3] = pressure g value,
 *
 * @return  error code
 */
GS8 gh3x2x_read_pressure_parameters(GS32 *buffer)
{
    //return GOODIX_PLATFORM_PRESSURE_PARAS_READ_ENTITY();
	  return 10;
}
#endif





/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
