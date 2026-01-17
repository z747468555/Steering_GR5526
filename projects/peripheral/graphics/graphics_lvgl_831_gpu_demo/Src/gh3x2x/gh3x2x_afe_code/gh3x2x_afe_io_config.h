/*************************************************************************************
 * Description  : 
 * Version      : 1.0
 * Author       : XiaoPb
 * Date         : 2022-06-15 16:28:23
 * LastEditors  : XiaoPb
 * LastEditTime : 2022-06-15 16:48:45
 * FilePath     : gh3x2x_code/demo_kernel_code/inc/gh3x2x_demo_io_config.h
 * Copyright (C) 2022 XiaoPb. All rights reserved.
 *************************************************************************************/

#ifndef __GH3X2X_DEMO_IO_CONFIG_H__
#define __GH3X2X_DEMO_IO_CONFIG_H__

#include "stdint.h"
#include "string.h"
#include "gh3x2x_afe_user.h"
#include "gh3x2x_afe_drv.h"

#include "app_io.h"
#include "app_gpiote.h"



#define USE_GH3X2X_VIO_PIN (0) // power io

#if (GH3X2X_AFE_RESET_TYPE == GH3X2X_AFE_RESET_HARDWARE)
#define GH3X2X_RST_IO_TYPE APP_IO_TYPE_GPIOB
#define GH3X2X_RST_PINMUX  APP_IO_MUX_7
#define GH3X2X_RST_PIN     APP_IO_PIN_12
#endif

#define GH3X2X_INT_IO_TYPE APP_IO_TYPE_AON
#define GH3X2X_INT_PINMUX  APP_IO_MUX_7
#define GH3X2X_INT_PIN     APP_IO_PIN_4



#if USE_GH3X2X_VIO_PIN // power io
#define GH3X2X_VIO_PIN  APP_IO_PIN_7
#define GH3X2X_VIO_PORT APP_IO_TYPE_NORMAL

#define GH3X2X_VIO_OPEN  APP_IO_PIN_SET
#define GH3X2X_VIO_CLOSE APP_IO_PIN_RESET
#endif

#if (GH3X2X_AFE_COM_INTERFACE_TYPE == GH3X2X_AFE_COM_INTERFACE_SPI)
#include "app_spi.h"
#include "app_spi_dma.h"

#define GH3X2X_CS_IO_TYPE APP_IO_TYPE_GPIOA
#if (GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_SOFTWARE_CS)
#define GH3X2X_CS_PINMUX  APP_IO_MUX_7
#define GH3X2X_CS_PIN_ENABLE APP_SPI_PIN_DISABLE
#else
#define GH3X2X_CS_PINMUX  APP_IO_MUX_2
#define GH3X2X_CS_PIN_ENABLE APP_SPI_PIN_ENABLE
#endif
#define GH3X2X_CS_PIN     APP_IO_PIN_10

#define GH3X2X_CLK_IO_TYPE APP_IO_TYPE_GPIOA
#define GH3X2X_CLK_PINMUX  APP_IO_MUX_2
#define GH3X2X_CLK_PIN     APP_IO_PIN_15

#define GH3X2X_MOSI_IO_TYPE APP_IO_TYPE_GPIOA
#define GH3X2X_MOSI_PINMUX  APP_IO_MUX_2
#define GH3X2X_MOSI_PIN     APP_IO_PIN_14

#define GH3X2X_MISO_IO_TYPE APP_IO_TYPE_GPIOA
#define GH3X2X_MISO_PINMUX  APP_IO_MUX_2
#define GH3X2X_MISO_PIN     APP_IO_PIN_13

#define SPI_CLOCK_PRESCALER             8u             /* The SPI CLOCK Freq = Peripheral CLK/SPI_CLOCK_PRESCALER */
#define SPI_SOFT_CS_MODE_ENABLE         1u              /* suggest to enable SOFT CS MODE */
#define SPI_SOFT_CS_MODE_DISABLE        0u              /* suggest to disable SOFT CS MODE */

#else // (GH3X2X_AFE_COM_INTERFACE_TYPE == GH3X2X_AFE_COM_INTERFACE_I2C)

#include "app_i2c.h"
#include "app_i2c_dma.h"
#define MASTER_DEV_ADDR	(0x38)
#define GH3X2X_I2C_ID   APP_I2C_ID_1

#define GH3X2X_SCL_PIN  APP_IO_PIN_9
#define GH3X2X_SCL_PORT APP_IO_TYPE_NORMAL
#define GH3X2X_SCL_MUX  APP_IO_MUX_1

#define GH3X2X_SDA_PIN  APP_IO_PIN_8
#define GH3X2X_SDA_PORT APP_IO_TYPE_NORMAL
#define GH3X2X_SDA_MUX  APP_IO_MUX_1
#endif



#if (GH3X2X_AFE_COM_INTERFACE_TYPE == GH3X2X_AFE_COM_INTERFACE_SPI)
/*
 * cs
 * clk
 * si
 * so
 */
#define GH3X2X_DEFAULT_IO_CONFIG   \
        { \
            {GH3X2X_CS_IO_TYPE,   GH3X2X_CS_PINMUX,   GH3X2X_CS_PIN,   APP_IO_MODE_MUX, APP_IO_PULLUP, GH3X2X_CS_PIN_ENABLE}, \
            {GH3X2X_CLK_IO_TYPE,  GH3X2X_CLK_PINMUX,  GH3X2X_CLK_PIN,  APP_IO_MODE_MUX, APP_IO_PULLUP, APP_SPI_PIN_ENABLE}, \
            {GH3X2X_MOSI_IO_TYPE, GH3X2X_MOSI_PINMUX, GH3X2X_MOSI_PIN, APP_IO_MODE_MUX, APP_IO_PULLUP, APP_SPI_PIN_ENABLE}, \
            {GH3X2X_MISO_IO_TYPE, GH3X2X_MISO_PINMUX, GH3X2X_MISO_PIN, APP_IO_MODE_MUX, APP_IO_PULLUP, APP_SPI_PIN_ENABLE}, \
        }

#define GH3X2X_DEFAULT_DMA_CONFIG   \
        {DMA0, DMA0, DMA_Channel0, DMA_Channel1}
        
#define GH3X2X_DEFAULT_INIT_CONFIG   \
        {SPI_DATASIZE_8BIT, SPI_POLARITY_LOW, SPI_PHASE_1EDGE, SPI_CLOCK_PRESCALER, SPI_TIMODE_DISABLE, SPI_SLAVE_SELECT_0}

#define GH3X2X_DEFAULT_PARAM_CONFIG   \
        {APP_SPI_ID_MASTER, GH3X2X_DEFAULT_IO_CONFIG, GH3X2X_DEFAULT_DMA_CONFIG, GH3X2X_DEFAULT_INIT_CONFIG, SPI_SOFT_CS_MODE_DISABLE}
     

#else // (__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__)

#define DEFAULT_I2C_IO_CONFIG               { { GH3X2X_SCL_PORT, GH3X2X_SCL_MUX, GH3X2X_SCL_PIN, APP_IO_PULLUP }, \
                                              { GH3X2X_SDA_PORT, GH3X2X_SDA_MUX, GH3X2X_SDA_PIN, APP_IO_PULLUP } }
#define DEFAULT_I2C_MODE_CONFIG             { APP_I2C_TYPE_DMA, DMA_Channel2, DMA_Channel3 }
#define DEFAULT_I2C_CONFIG                  { I2C_SPEED_100K, MASTER_DEV_ADDR, I2C_ADDRESSINGMODE_7BIT, I2C_GENERALCALL_ENABLE}
#define DEFAULT_I2C_PARAM_CONFIG            { GH3X2X_I2C_ID, APP_I2C_ROLE_MASTER, DEFAULT_I2C_IO_CONFIG, DEFAULT_I2C_MODE_CONFIG, DEFAULT_I2C_CONFIG}
#endif

#endif // !__GH3X2X_DEMO_IO_CONFIG_H__
