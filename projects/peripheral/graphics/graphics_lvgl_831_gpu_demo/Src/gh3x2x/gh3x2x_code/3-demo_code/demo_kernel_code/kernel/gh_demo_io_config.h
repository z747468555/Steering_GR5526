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

#ifndef __GH_DEMO_IO_CONFIG_H__
#define __GH_DEMO_IO_CONFIG_H__

#include "stdint.h"
#include "string.h"
#include "gh_drv.h"
#include "gh_demo_config.h"

#include "app_io.h"
#include "app_gpiote.h"



#define USE_GH3X2X_VIO_PIN (0) // power io

//#if (__SUPPORT_HARD_RESET_CONFIG__)
//#define GH_RST_IO_TYPE APP_IO_TYPE_GPIOB
//#define GH_RST_PINMUX  APP_IO_MUX_7
//#define GH_RST_PIN     APP_IO_PIN_12
//#endif

//#define GH_INT_IO_TYPE APP_IO_TYPE_AON
//#define GH_INT_PINMUX  APP_IO_MUX_7
//#define GH_INT_PIN     APP_IO_PIN_4



#if USE_GH3X2X_VIO_PIN // power io
#define GH_VIO_PIN  APP_IO_PIN_7
#define GH_VIO_PORT APP_IO_TYPE_NORMAL

#define GH_VIO_OPEN  APP_IO_PIN_SET
#define GH_VIO_CLOSE APP_IO_PIN_RESET
#endif

#if ( __GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_SPI__ )
#include "app_spi.h"
#include "app_spi_dma.h"

#define GH_CS_IO_TYPE APP_IO_TYPE_GPIOA
#if (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__)
#define GH_CS_PINMUX  APP_IO_MUX_7
#define GH_CS_PIN_ENABLE APP_SPI_PIN_DISABLE
#else
#define GH_CS_PINMUX  APP_IO_MUX_2
#define GH_CS_PIN_ENABLE APP_SPI_PIN_ENABLE
#endif
#define GH_CS_PIN     APP_IO_PIN_10

#define GH_CLK_IO_TYPE APP_IO_TYPE_GPIOA
#define GH_CLK_PINMUX  APP_IO_MUX_2
#define GH_CLK_PIN     APP_IO_PIN_15

#define GH_MOSI_IO_TYPE APP_IO_TYPE_GPIOA
#define GH_MOSI_PINMUX  APP_IO_MUX_2
#define GH_MOSI_PIN     APP_IO_PIN_14

#define GH_MISO_IO_TYPE APP_IO_TYPE_GPIOA
#define GH_MISO_PINMUX  APP_IO_MUX_2
#define GH_MISO_PIN     APP_IO_PIN_13

#define SPI_CLOCK_PRESCALER             16u             /* The SPI CLOCK Freq = Peripheral CLK/SPI_CLOCK_PRESCALER */
#define SPI_SOFT_CS_MODE_ENABLE         1u              /* suggest to enable SOFT CS MODE */
#define SPI_SOFT_CS_MODE_DISABLE        0u              /* suggest to disable SOFT CS MODE */

#else // (GH3X2X_AFE_COM_INTERFACE_TYPE == GH_AFE_COM_INTERFACE_I2C)

#include "app_i2c.h"
#include "app_i2c_dma.h"
//#define MASTER_DEV_ADDR	(0x38)
//#define GH_I2C_ID   APP_I2C_ID_5

//#define GH_SCL_PIN  APP_IO_PIN_14
//#define GH_SCL_PORT APP_IO_TYPE_GPIOA
//#define GH_SCL_MUX  APP_IO_MUX_1

//#define GH_SDA_PIN  APP_IO_PIN_15
//#define GH_SDA_PORT APP_IO_TYPE_GPIOA
//#define GH_SDA_MUX  APP_IO_MUX_1
#endif



#if ( __GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_SPI__ )
/*
 * cs
 * clk
 * si
 * so
 */
#define GH_DEFAULT_IO_CONFIG   \
        { \
            {GH_CS_IO_TYPE,   GH_CS_PINMUX,   GH_CS_PIN,   APP_IO_MODE_MUX, APP_IO_PULLUP, GH_CS_PIN_ENABLE}, \
            {GH_CLK_IO_TYPE,  GH_CLK_PINMUX,  GH_CLK_PIN,  APP_IO_MODE_MUX, APP_IO_PULLUP, APP_SPI_PIN_ENABLE}, \
            {GH_MOSI_IO_TYPE, GH_MOSI_PINMUX, GH_MOSI_PIN, APP_IO_MODE_MUX, APP_IO_PULLUP, APP_SPI_PIN_ENABLE}, \
            {GH_MISO_IO_TYPE, GH_MISO_PINMUX, GH_MISO_PIN, APP_IO_MODE_MUX, APP_IO_PULLUP, APP_SPI_PIN_ENABLE}, \
        }

#define GH_DEFAULT_DMA_CONFIG   \
        {DMA0, DMA0, DMA_Channel0, DMA_Channel1}
        
#define GH_DEFAULT_INIT_CONFIG   \
        {SPI_DATASIZE_8BIT, SPI_POLARITY_LOW, SPI_PHASE_1EDGE, SPI_CLOCK_PRESCALER, SPI_TIMODE_DISABLE, SPI_SLAVE_SELECT_0}

#define GH_DEFAULT_PARAM_CONFIG   \
        {APP_SPI_ID_MASTER, GH_DEFAULT_IO_CONFIG, GH_DEFAULT_DMA_CONFIG, GH_DEFAULT_INIT_CONFIG, SPI_SOFT_CS_MODE_DISABLE}
     

#else // (__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__)

//#define GH_DEFAULT_I2C_IO_CONFIG               { { GH_SCL_PORT, GH_SCL_MUX, GH_SCL_PIN, APP_IO_PULLUP }, \
//                                                 { GH_SDA_PORT, GH_SDA_MUX, GH_SDA_PIN, APP_IO_PULLUP } }
//#define GH_DEFAULT_I2C_MODE_CONFIG             { DMA0,DMA0, DMA_Channel2, DMA_Channel3 }
//#define GH_DEFAULT_I2C_CONFIG                  { I2C_SPEED_100K, MASTER_DEV_ADDR, I2C_ADDRESSINGMODE_7BIT, I2C_GENERALCALL_ENABLE}
//#define GH_DEFAULT_I2C_PARAM_CONFIG            { GH_I2C_ID, APP_I2C_ROLE_MASTER, GH_DEFAULT_I2C_IO_CONFIG, GH_DEFAULT_I2C_MODE_CONFIG, GH_DEFAULT_I2C_CONFIG}
#endif

#endif // !__GH_DEMO_IO_CONFIG_H__
