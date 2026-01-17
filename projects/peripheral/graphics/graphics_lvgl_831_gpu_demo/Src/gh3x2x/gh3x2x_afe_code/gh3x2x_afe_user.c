#include "gh3x2x_afe_user.h"
#include "gh3x2x_afe_drv.h"
#include "gh3x2x_afe_io_config.h"


#ifdef GOODIX_DEMO_PLANFORM
#include "gh3x2x_demo_goodix_planfrom.h"
#else
#define GOODIX_PLANFROM_SPI_WRITE_ENTITY(write_buffer, length)
#define GOODIX_PLANFROM_SPI_READ_ENTITY()
#define GOODIX_PLANFROM_SPI_CS_CTRL_ENTITY()
#define GOODIX_PLANFROM_SPI_RESET_PIN_ENTITY()
#define GOODIX_PLANFROM_INT_HANDLER_CALL_BACK_ENTITY()
#define GOODIX_PLANFROM_LOG_ENTITY()
#define GOODIX_PLANFROM_DELAY_US_ENTITY() 
#define GOODIX_PLANFROM_DELAY_MS_ENTITY()
#define GOODIX_PLANFROM_DELAY_MS_ENTITY()
#define GOODIX_PLANFROM_INT_INIT_ENTITY()
#define GOODIX_PLANFROM_RESET_PIN_INIT_ENTITY()
#define GOODIX_PLANFROM_SPI_INIT_ENTITY()
#define GOODIX_PLANFROM_I2C_WRITE_ENTITY(device_id, write_buffer,length)
#define GOODIX_PLANFROM_I2C_INIT_ENTITY()
#define GOODIX_PLANFROM_I2C_READ_ENTITY(device_id, write_buffer, write_length, read_buffer, read_length)
#endif

/**
 * @fn     void hal_gh3x2x_afe_DelayUs(GU16 usUsec)
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

void hal_gh3x2x_afe_DelayUs(GU16 usUsec)
{
    GOODIX_PLANFROM_DELAY_US_ENTITY();
    delay_us(usUsec);
}

/**
 * @fn     void hal_gh3x2x_afe_BspDelayMs(GU16 usMsec)
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
void hal_gh3x2x_afe_DelayMs(GU16 usMsec)
{
    GOODIX_PLANFROM_DELAY_MS_ENTITY();
    delay_ms(usMsec);
}






#if ( GH3X2X_AFE_COM_INTERFACE_TYPE == GH3X2X_AFE_COM_INTERFACE_I2C )

/* i2c interface */
/**
 * @fn     void hal_gh3x2x_afe_i2c_init(void)
 * 
 * @brief  hal i2c init for gh3x2x_afe
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gh3x2x_afe_i2c_init(void)
{

    /* code implement by user */
	app_i2c_params_t params_t = DEFAULT_I2C_PARAM_CONFIG;

	params_t.use_mode.type = APP_I2C_TYPE_POLLING;
	if (app_i2c_init(&params_t, NULL) != 0)
	{
		EXAMPLE_LOG("i2c initial failed! Please check the input paraments.\r\n");
		return ;
	}
}

/**
 * @fn     uint8_t hal_gh3x2x_afe_i2c_write(uint8_t device_id, const uint8_t write_buffer[], uint16_t length)
 * 
 * @brief  hal i2c write for gh3x2x_afe
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
GU8 hal_gh3x2x_afe_i2c_write(GU8 device_id, const GU8 write_buffer[], GU16 length)
{
    uint8_t ret = 1;

    /* code implement by user */
    ret = app_i2c_transmit_sync(GH3x2x_I2C_ID, device_id >> 1, (uint8_t *) write_buffer, length, 100);
	if (ret)
	{
		EXAMPLE_LOG("i2c(%X) transmit failed(%d)! \r\n",device_id,ret);
		return 0;
	}
    return ret;
}

/**
 * @fn     uint8_t hal_gh3x2x_afe_i2c_read(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length,
 *                            uint8_t read_buffer[], uint16_t read_length)
 * 
 * @brief  hal i2c read for gh3x2x_afe
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
GU8 hal_gh3x2x_afe_i2c_read(GU8 device_id, const GU8 write_buffer[], GU16 write_length, GU8 read_buffer[], GU16 read_length)
{
    uint8_t ret = 1;

    /* code implement by user */
	uint8_t i2c_addr = device_id >> 1;
    ret = app_i2c_transmit_sync(GH3x2x_I2C_ID, i2c_addr, (uint8_t *) write_buffer, write_length, 100);
	if (ret)
	{
		EXAMPLE_LOG("i2c(%X) transmit failed(%d)! \r\n",device_id,ret);
		return 0;
	}
    ret = app_i2c_receive_sync(GH3x2x_I2C_ID, i2c_addr, read_buffer, read_length, 100);
	if (ret)
	{
		EXAMPLE_LOG("i2c(%X) receive failed(%d)! \r\n",device_id,ret);
		return 0;
	}
    return ret;
}

#else 

app_spi_params_t gh3x2x_spi_params = GH3X2X_DEFAULT_PARAM_CONFIG;

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
 * @fn     void hal_gh3x2x_afe_spi_init(void)
 * 
 * @brief  hal spi init for gh3x2x_afe
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */

void hal_gh3x2x_afe_spi_init(void)
{
    app_drv_err_t ret = 0;
    ret = app_spi_init(&gh3x2x_spi_params, gh3x2x_spi_master_callback);
    if (ret != 0)
    {
        EXAMPLE_LOG("SPI master initial failed! Please check the input paraments.\r\n");
    }

    ret = app_spi_dma_init(&gh3x2x_spi_params);
    if (ret != 0)
    {
        EXAMPLE_LOG("SPI master dma initial failed! Please check the input paraments.\r\n");
    }


#if (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__)
    
    app_io_init_t io_init = APP_IO_DEFAULT_CONFIG;

    io_init.pull = APP_IO_PULLUP;
    io_init.mode = APP_IO_MODE_OUTPUT;
    io_init.pin  = GH3X2X_CS_PIN;
    io_init.mux  = GH3X2X_CS_PINMUX;
    app_io_init(GH3X2X_CS_IO_TYPE, &io_init);
#endif

}

/**
 * @fn     GU8 hal_gh3x2x_afe_spi_write(GU8 write_buffer[], GU16 length)
 * 
 * @brief  hal spi write for gh3x2x_afe
 *
 * @attention   if __gh3x2x_afe_SPI_TYPE__ == __gh3x2x_afe_SPI_TYPE_SOFTWARE_CS__  , user need generate timming: write_buf[0](W) + write_buf[1](W) + ...
 * @attention   if __gh3x2x_afe_SPI_TYPE__ == __gh3x2x_afe_SPI_TYPE_HARDWARE_CS__  , user need generate timming: CS LOW  + write_buf[0](W) + write_buf[1](W) + ... + CS HIGH
 *
 * @param[in]   write_buffer    write data buffer
 * @param[in]   length          write data len
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_afe_spi_write(GU8 write_buffer[], GU16 length)
{
    g_master_tdone = 0;
    GU16 err = app_spi_transmit_async(APP_SPI_ID_MASTER, write_buffer, length);
    if (err)
    {
        EXAMPLE_LOG("spi write error. ret = %d.\r\n", err);
        return 0;
    }
    while (!g_master_tdone);
    return 1;
}


#if (GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_SOFTWARE_CS) 
/**
 * @fn     GU8 hal_gh3x2x_afe_spi_read(GU8 read_buffer[], GU16 length)
 * 
 * @brief  hal spi read for gh3x2x_afe
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
GU8 hal_gh3x2x_afe_spi_read(GU8 read_buffer[], GU16 length)
{
    g_master_rdone = 0;
    GU16 err = app_spi_receive_async(APP_SPI_ID_MASTER, read_buffer, length);
    if (err)
    {
        EXAMPLE_LOG("spi read error, err = %d \r\n", err);
        return 0;
    }
    while (!g_master_rdone);
    return 1;
}
#elif (GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_HARDWEAR_CS)
/**
 * @fn     GU8 hal_gh3x2x_afe_spi_write_F1_and_read(GU8 read_buffer[], GU16 length)
 * 
 * @brief  hal spi write F1 and read for gh3x2x_afe
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
GU8 hal_gh3x2x_afe_spi_write_F1_and_read(GU8 read_buffer[], GU16 length)
{
    GU8 write_buffer[1] = {0xF1};
    g_master_rdone = 0;
    GU16 err = app_spi_dma_read_eeprom_async(APP_SPI_ID_MASTER, write_buffer, read_buffer, 1, length);
    if (err){
        EXAMPLE_LOG("spi read error, err = %d \r\n", err);
        return 0;
    }
    while (!g_master_rdone);
    return 1;
}
#endif

/**
 * @fn     void hal_gh3x2x_afe_spi_cs_ctrl(GU8 cs_pin_level)
 * 
 * @brief  hal spi cs pin ctrl for gh3x2x_afe
 *
 * @attention   pin level set 1 [high level] or 0 [low level]
 *
 * @param[in]   cs_pin_level     spi cs pin level
 * @param[out]  None
 *
 * @return  None
 */



#if (GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_SOFTWARE_CS) 
void hal_gh3x2x_afe_spi_cs_ctrl(GU8 cs_pin_level)
{
    app_io_write_pin(GH3X2X_CS_IO_TYPE, GH3X2X_CS_PIN, cs_pin_level?APP_IO_PIN_SET:APP_IO_PIN_RESET);
}
#endif

#endif

#if (GH3X2X_AFE_RESET_TYPE == GH3X2X_AFE_RESET_HARDWARE)
/**
 * @fn     void hal_gh3x2x_afe_int_init(void)
 * 
 * @brief  gh3x2x_afe int init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */



void hal_gh3x2x_afe_reset_pin_init(void)
{
    app_io_init_t io_init = APP_IO_DEFAULT_CONFIG;
    app_drv_err_t err_code = APP_DRV_SUCCESS;

    io_init.pull = APP_IO_PULLUP;
    io_init.mode = APP_IO_MODE_OUTPUT;
    io_init.pin  = GH3X2X_RST_PIN;
    io_init.mux  = GH3X2X_RST_PINMUX;
    err_code = app_io_init(GH3X2X_RST_IO_TYPE, &io_init);
//    APP_DRV_ERR_CODE_CHECK(err_code);

    app_io_write_pin(GH3X2X_RST_IO_TYPE, GH3X2X_RST_PIN, APP_IO_PIN_RESET);
    hal_gh3x2x_afe_DelayMs(10);
    app_io_write_pin(GH3X2X_RST_IO_TYPE, GH3X2X_RST_PIN, APP_IO_PIN_SET);
}

/**
 * @fn     void hal_gh3x2x_afe_reset_pin_ctrl(GU8 pin_level)
 * 
 * @brief  hal reset pin ctrl for gh3x2x_afe
 *
 * @attention   pin level set 1 [high level] or 0 [low level]
 *
 * @param[in]   pin_level     reset pin level
 * @param[out]  None
 *
 * @return  None
 */

void hal_gh3x2x_afe_reset_pin_ctrl(GU8 pin_level)
{

    app_io_write_pin(GH3X2X_RST_IO_TYPE, GH3X2X_RST_PIN, pin_level?APP_IO_PIN_SET:APP_IO_PIN_RESET);

}

#endif

void hal_gh3x2x_afe_int_handler_call_back(void);
extern void gh3x2x_interupt_request(void);
void gh3x2x_int_callback(app_io_evt_t *p_evt)
{
//    app_io_pin_state_t pin_level = APP_IO_PIN_RESET;

//    switch (p_evt->type)
//    {
//    case GH3X2X_INT_IO_TYPE:
//        if ((p_evt->pin & APP_IO_PIN_ALL) == GH3X2X_INT_PIN)
//        {
//            //printf("hal_gh3x2x_int_handler_call_back start\r\n");
//            pin_level = app_io_read_pin(GH3X2X_INT_IO_TYPE, GH3X2X_INT_PIN);
//            if (pin_level == APP_IO_PIN_SET)
//            {
                hal_gh3x2x_afe_int_handler_call_back();
//                if(g_semGh3x2x != NULL)
                {
                    gh3x2x_interupt_request();
                }
//                //printf("hal_gh3x2x_int_handler_call_back end\r\n");
//            }
//        }
//        break;

//    default:
////        delay_ms(1);
//        break;
//    }
}

/**
 * @fn     void hal_gh3x2x_afe_int_init(void)
 * 
 * @brief  gh3x2x_afe int init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */


void hal_gh3x2x_afe_int_init(void)
{
    app_gpiote_param_t gh3x2x_int_param = {
        GH3X2X_INT_IO_TYPE, GH3X2X_INT_PIN, APP_IO_MODE_IT_FALLING, APP_IO_PULLUP, gh3x2x_int_callback
    };
    app_gpiote_init(&gh3x2x_int_param, 1);
}

#if (GH3X2X_AFE_PRO_BY_INT == GH3X2X_AFE_PRO_TYPE)
/**
 * @fn     void hal_gh3x2x_afe_int_handler_call_back(void)
 * 
 * @brief  call back of gh3x2x_afe int handler
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gh3x2x_afe_int_handler_call_back(void)
{
/****************** DO NOT MODITY ****************************************************************/
	GH3X2X_AFE_SetNeedWakeUpGh3x2xFlag(0);  //not need wake up gh3x2x_afe when process int event
  g_uchGh3x2xAfeIntCallBackIsCalled = 1;
/****************** DO NOT MODITY ****************************************************************/
    
    
    
    GOODIX_PLANFROM_INT_HANDLER_CALL_BACK_ENTITY();
}

#endif

/**
 * @fn     void GH3X2X_AFE_Log(char *log_string)
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
void GH3X2X_AFE_Log(GCHAR *log_string)
{
    GOODIX_PLANFROM_LOG_ENTITY();
    printf("[gh3x2x afe] %s", log_string);
}

