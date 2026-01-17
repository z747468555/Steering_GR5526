#include "IIC.h"
#define APP_LOG_TAG	"iic"
#include "app_log.h"
#include "FreeRTOS.h" 
#include "semphr.h"

#define ENABLE_SYNC_I2C 1

app_i2c_params_t I2C1_Struct = {
    .id = APP_I2C_ID_1,
    .role = APP_I2C_ROLE_MASTER,
    .pin_cfg = {
        .scl = {
            .type = IIC_SCL_PIN_TYPE,
            .mux  = IIC_SCL_PIN_MUX,
            .pin  = IIC_SCL_PIN_NUM,
            .pull = APP_IO_PULLUP,
        },
        .sda = {
            .type = IIC_SDA_PIN_TYPE,
            .mux  = IIC_SDA_PIN_MUX,
            .pin  = IIC_SDA_PIN_NUM,
            .pull = APP_IO_PULLUP,
        },
    },
    .dma_cfg = {
        .tx_dma_instance = DMA1,                                                                                                                            
        .rx_dma_instance = DMA1,
        .tx_dma_channel  = DMA_Channel2,
        .rx_dma_channel  = DMA_Channel3,
    },
    .init = {
        .speed = I2C_SPEED_100K,
        .own_address = 0x00,
        .addressing_mode = I2C_ADDRESSINGMODE_7BIT,
        .general_call_mode = I2C_GENERALCALL_DISABLE,
    },
};

app_i2c_params_t I2C3_Struct = {
    .id = APP_I2C_ID_3,
    .role = APP_I2C_ROLE_MASTER,
    .pin_cfg = {
        .scl = {
            .type = IIC3_SCL_PIN_TYPE,
            .mux  = IIC3_SCL_PIN_MUX,
            .pin  = IIC3_SCL_PIN_NUM,
            .pull = APP_IO_PULLUP,
        },
        .sda = {
            .type = IIC3_SDA_PIN_TYPE,
            .mux  = IIC3_SDA_PIN_MUX,
            .pin  = IIC3_SDA_PIN_NUM,
            .pull = APP_IO_PULLUP,
        },
    },
    .dma_cfg = {
        .tx_dma_instance = DMA0,                                                                                                                            
        .rx_dma_instance = DMA0,
        .tx_dma_channel  = DMA_Channel4,
        .rx_dma_channel  = DMA_Channel5,
    },
    .init = {
        .speed = I2C_SPEED_100K,
        .own_address = 0x00,
        .addressing_mode = I2C_ADDRESSINGMODE_7BIT,
        .general_call_mode = I2C_GENERALCALL_DISABLE,
    },
};
app_i2c_params_t I2C5_Struct = {
    .id = APP_I2C_ID_5,
    .role = APP_I2C_ROLE_MASTER,
    .pin_cfg = {
        .scl = {
            .type = IIC5_SCL_PIN_TYPE,
            .mux  = IIC5_SCL_PIN_MUX,
            .pin  = IIC5_SCL_PIN_NUM,
            .pull = APP_IO_PULLUP,
        },
        .sda = {
            .type = IIC5_SDA_PIN_TYPE,
            .mux  = IIC5_SDA_PIN_MUX,
            .pin  = IIC5_SDA_PIN_NUM,
            .pull = APP_IO_PULLUP,
        },
    },
    .dma_cfg = {
        .tx_dma_instance = DMA0,                                                                                                                            
        .rx_dma_instance = DMA0,
        .tx_dma_channel  = DMA_Channel5,
        .rx_dma_channel  = DMA_Channel4,
    },
    .init = {
        .speed = I2C_SPEED_100K,
        .own_address = 0x00,
        .addressing_mode = I2C_ADDRESSINGMODE_7BIT,
        .general_call_mode = I2C_GENERALCALL_DISABLE,
    },
};
/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
volatile uint8_t I2C1_tdone = 0;
volatile uint8_t I2C1_rdone = 0;
volatile uint8_t I2C3_tdone = 0;
volatile uint8_t I2C3_rdone = 0;
volatile uint8_t I2C5_tdone = 0;
volatile uint8_t I2C5_rdone = 0;

SemaphoreHandle_t iic3_mutex;


void iic_mutex_create()
{
	iic3_mutex = xSemaphoreCreateMutex();
}



uint16_t IIC_Write_sync(uint8_t ID, uint16_t add, uint8_t *pdata, uint16_t size)
{
	uint16_t ret = APP_DRV_SUCCESS;
	xSemaphoreTake(iic3_mutex,portMAX_DELAY);
    ret = app_i2c_transmit_sync(ID, add, pdata, size, 1);
	xSemaphoreGive(iic3_mutex);
    if (ret != APP_DRV_SUCCESS)
    {
//        APP_LOG_ERROR("I2C master send failed, ERR_Code:%x, IIC_ID:%d.", ret, ID,add,size);
        return ret;
    }
		return ret;
}

uint16_t IIC_Read_sync(uint8_t ID, uint16_t add, uint8_t *pdata, uint16_t size)
{
	uint16_t ret = APP_DRV_SUCCESS;
	xSemaphoreTake(iic3_mutex,portMAX_DELAY);
	ret = app_i2c_receive_sync(ID, add, pdata, size, 1);
	xSemaphoreGive(iic3_mutex);
    if (ret != APP_DRV_SUCCESS) 
    {
        APP_LOG_ERROR( "I2C master receive failed, ERR_Code:%x, IIC_ID:%d.", ret, ID);
        return ret;
    }
		return ret;
}

uint16_t IIC_Write(uint8_t ID, uint16_t add, uint8_t *pdata, uint16_t size)
{
#if ENABLE_SYNC_I2C
    return IIC_Write_sync(ID, add, pdata, size);
#else
	uint16_t ret = APP_DRV_SUCCESS;
    ret = app_i2c_transmit_async(ID, add, pdata, size);
    if (ret != APP_DRV_SUCCESS)
    {
        APP_LOG_INFO( "I2C master send failed, ERR_Code:%x, IIC_ID:%d.\r\n", ret, ID);
        return ret;
    }
    if(ID == APP_I2C_ID_1){
        while(I2C1_tdone == 0);
            I2C1_tdone = 0;
    }
    else if(ID == APP_I2C_ID_3){
        while(I2C3_tdone == 0);
            I2C3_tdone = 0;
    }
    else if(ID == APP_I2C_ID_5){
        while(I2C5_tdone == 0);
            I2C5_tdone = 0;
    }
    return ret;
#endif
}

uint16_t IIC_Read(uint8_t ID, uint16_t add, uint8_t *pdata, uint16_t size)
{
#if ENABLE_SYNC_I2C
    return IIC_Read_sync(ID, add, pdata, size);
#else
	uint16_t ret = APP_DRV_SUCCESS;
	ret = app_i2c_receive_async(ID, add, pdata, size);
    if (ret != APP_DRV_SUCCESS) 
    {
        APP_LOG_INFO( "I2C master receive failed, ERR_Code:%x, IIC_ID:%d.\r\n", ret, ID);
        return ret;
    }
    if(ID == APP_I2C_ID_1){
        while(I2C1_rdone == 0);
            I2C1_rdone = 0;
    }
    else if(ID == APP_I2C_ID_3){
        while(I2C3_rdone == 0);
            I2C3_rdone = 0;
    }
    else if(ID == APP_I2C_ID_5){
        while(I2C5_rdone == 0);
            I2C5_rdone = 0;
    }
    // vTaskDelay(5);
    return ret;
#endif
}

uint16_t IIC_Write_sync_tp(uint8_t ID, uint16_t add, uint8_t *pdata, uint16_t size)
{
	uint16_t ret = APP_DRV_SUCCESS;
	xSemaphoreTake(iic3_mutex,portMAX_DELAY);
    ret = app_i2c_transmit_sync(ID, add, pdata, size, 1);
	xSemaphoreGive(iic3_mutex);
    if (ret != APP_DRV_SUCCESS)
    {
//        APP_LOG_ERROR("I2C master send failed, ERR_Code:%x, IIC_ID:%d.", ret, ID,add,size);
        return ret;
    }
		return ret;
}

uint16_t IIC_Read_sync_tp(uint8_t ID, uint16_t add, uint8_t *pdata, uint16_t size)
{
	uint16_t ret = APP_DRV_SUCCESS;
	xSemaphoreTake(iic3_mutex,portMAX_DELAY);
	ret = app_i2c_receive_sync(ID, add, pdata, size, 1);
	xSemaphoreGive(iic3_mutex);
    if (ret != APP_DRV_SUCCESS) 
    {
        APP_LOG_ERROR( "I2C master receive failed, ERR_Code:%x, IIC_ID:%d.", ret, ID);
        return ret;
    }
		return ret;
}

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static void app_i2c_master_evt_handler(app_i2c_evt_t * p_evt)
{
    switch (p_evt->type)
    {
        case APP_I2C_EVT_ERROR:
            I2C1_tdone = 1;
            I2C1_rdone = 1;
            break;

        case APP_I2C_EVT_TX_CPLT:
            I2C1_tdone = 1;
            break;

        case APP_I2C_EVT_RX_DATA:
            I2C1_rdone = 1;
            break;
    }
}
static void app_i2c3_master_evt_handler(app_i2c_evt_t * p_evt)
{
    switch (p_evt->type)
    {
        case APP_I2C_EVT_ERROR:
            I2C3_tdone = 1;
            I2C3_rdone = 1;
            break;

        case APP_I2C_EVT_TX_CPLT:
            I2C3_tdone = 1;
            break;

        case APP_I2C_EVT_RX_DATA:
            I2C3_rdone = 1;
            break;
    }
}

static void app_i2c5_master_evt_handler(app_i2c_evt_t * p_evt)
{
    switch (p_evt->type)
    {
        case APP_I2C_EVT_ERROR:
            I2C5_tdone = 1;
            I2C5_rdone = 1;
            break;

        case APP_I2C_EVT_TX_CPLT:
            I2C5_tdone = 1;
            break;

        case APP_I2C_EVT_RX_DATA:
            I2C5_rdone = 1;
            break;
    }
}

void IIC_Init()
{
    uint16_t ret = 0;

    ret = app_i2c_init(&I2C1_Struct, app_i2c_master_evt_handler);
    if (ret != 0)
    {
        APP_LOG_ERROR( "I2C1 master initial failed! Please check the input parameters.");
        return;
    }

	I2C1_tdone = 0;
	I2C1_rdone = 0;

    ret = app_i2c_init(&I2C3_Struct, app_i2c3_master_evt_handler);
    if (ret != 0)
    {
        APP_LOG_ERROR( "I2C3 master initial failed! Please check the input parameters.");
        return;
    }

	I2C3_tdone = 0;
	I2C3_rdone = 0;

	APP_LOG_INFO( "IIC Init Over");
}


