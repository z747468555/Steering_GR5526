/**
 ****************************************************************************************
 *
 * @file board_SK.h
 *
 * @brief Start Kit Board Macro.
 *
 ****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */
#ifndef __BOARD_SK_H__
#define __BOARD_SK_H__

#include "app_key.h"
#include "app_uart.h"


/*******KEY DRIVER IO CONFIG********************/
#define APP_KEY_1_IO_TYPE                   APP_IO_TYPE_AON
#define APP_KEY_1_PIN                       APP_IO_PIN_1
#define APP_KEY_1_MUX                       APP_IO_MUX_7
#define APP_KEY_1_TRIGGER_MODE              APP_IO_MODE_IT_RISING
#define APP_KEY_1_PULL_MODE                 APP_IO_PULLDOWN
#define APP_KEY_1_ID                        0x01

#define APP_KEY_2_IO_TYPE                   APP_IO_TYPE_AON
#define APP_KEY_2_PIN                       APP_IO_PIN_0
#define APP_KEY_2_MUX                       APP_IO_MUX_7
#define APP_KEY_2_TRIGGER_MODE              APP_IO_MODE_IT_RISING
#define APP_KEY_2_PULL_MODE                 APP_IO_PULLDOWN
#define APP_KEY_2_ID                        0x02

#define APP_KEY_3_IO_TYPE                   APP_IO_TYPE_AON
#define APP_KEY_3_PIN                       APP_IO_PIN_2
#define APP_KEY_3_MUX                       APP_IO_MUX_7
#define APP_KEY_3_TRIGGER_MODE              APP_IO_MODE_IT_FALLING
#define APP_KEY_3_PULL_MODE                 APP_IO_PULLUP
#define APP_KEY_3_ID                        0x03

/*******UART DRIVER IO CONFIG*******************/
#define APP_UART_ID                         APP_UART_ID_3
#define APP_UART_BAUDRATE                   115200
#define APP_UART_TX_IO_TYPE                 APP_IO_TYPE_GPIOA
#define APP_UART_RX_IO_TYPE                 APP_IO_TYPE_GPIOA
#define APP_UART_TX_PIN                     APP_IO_PIN_4
#define APP_UART_RX_PIN                     APP_IO_PIN_5
#define APP_UART_TX_PINMUX                  APP_IO_MUX_3
#define APP_UART_RX_PINMUX                  APP_IO_MUX_3
#define APP_UART_TX_PULL                    APP_IO_NOPULL
#define APP_UART_RX_PULL                    APP_IO_PULLUP

/*******LED IO CONFIG FOR SK*********************/
#define APP_LED_BAT_IO                      APP_IO_PIN_3
#define APP_LED_CHG_IO                      APP_IO_PIN_5

/*******I2C 1 CONFIG***************************/
#define APP_I2C1_MASTER_ID                  APP_I2C_ID_1
#define APP_I2C1_MASTER_SCL_PIN             APP_IO_PIN_14  //GPIO30
#define APP_I2C1_MASTER_SDA_PIN             APP_IO_PIN_15  //GPIO31
#define APP_I2C1_MASTER_SCL_IO_TYPE         APP_IO_TYPE_GPIOB
#define APP_I2C1_MASTER_SDA_IO_TYPE         APP_IO_TYPE_GPIOB
#define APP_I2C1_MASTER_SCL_PINMUX          APP_IO_MUX_3
#define APP_I2C1_MASTER_SDA_PINMUX          APP_IO_MUX_3

#define APP_6AXIS_INT1_PIN                  APP_IO_PIN_6
#define APP_6AXIS_INT2_PIN                  APP_IO_PIN_7
#define APP_6AXIS_INT_IO_TYPE               APP_IO_TYPE_AON
/*******I2C CHARGE CONFIG *********************/

#define APP_I2C0_MASTER_ID                  APP_I2C_ID_0
#define APP_I2C0_MASTER_SCL_PIN             APP_IO_PIN_4   //AON_GPIO4
#define APP_I2C0_MASTER_SDA_PIN             APP_IO_PIN_12  //GPIO28
#define APP_I2C0_MASTER_SCL_IO_TYPE         APP_IO_TYPE_AON
#define APP_I2C0_MASTER_SDA_IO_TYPE         APP_IO_TYPE_GPIOB
#define APP_I2C0_MASTER_SCL_PINMUX          APP_IO_MUX_2
#define APP_I2C0_MASTER_SDA_PINMUX          APP_IO_MUX_5

#define APP_CHARGE_INT_PIN                  APP_IO_PIN_3
#define APP_CHARGE_INT_IO_TYPE              APP_IO_TYPE_AON

/********QSPI FLASH CONFIG***********************/
#define APP_QSPI0_MODULE                    QSPI0
#define APP_QSPI0_GPIO_MUX                  APP_IO_MUX_0
#define APP_QSPI0_CS_IO_TYPE                APP_IO_TYPE_GPIOB
#define APP_QSPI0_CLK_IO_TYPE               APP_IO_TYPE_GPIOB
#define APP_QSPI0_IO0_IO_TYPE               APP_IO_TYPE_GPIOB
#define APP_QSPI0_IO1_IO_TYPE               APP_IO_TYPE_GPIOB
#define APP_QSPI0_IO2_IO_TYPE               APP_IO_TYPE_GPIOB
#define APP_QSPI0_IO3_IO_TYPE               APP_IO_TYPE_GPIOB
#define APP_QSPI0_CS_PIN                    APP_IO_PIN_10
#define APP_QSPI0_CLK_PIN                   APP_IO_PIN_5
#define APP_QSPI0_IO0_PIN                   APP_IO_PIN_6
#define APP_QSPI0_IO1_PIN                   APP_IO_PIN_7
#define APP_QSPI0_IO2_PIN                   APP_IO_PIN_8
#define APP_QSPI0_IO3_PIN                   APP_IO_PIN_9

#define APP_FLASH_GPIO_MUX                  APP_QSPI0_GPIO_MUX
#define APP_FLASH_CS_IO_TYPE                APP_QSPI0_CS_IO_TYPE
#define APP_FLASH_CLK_IO_TYPE               APP_QSPI0_CLK_IO_TYPE
#define APP_FLASH_IO0_IO_TYPE               APP_QSPI0_IO0_IO_TYPE
#define APP_FLASH_IO1_IO_TYPE               APP_QSPI0_IO1_IO_TYPE
#define APP_FLASH_IO2_IO_TYPE               APP_QSPI0_IO2_IO_TYPE
#define APP_FLASH_IO3_IO_TYPE               APP_QSPI0_IO3_IO_TYPE
#define APP_FLASH_CS_PIN                    APP_QSPI0_CS_PIN
#define APP_FLASH_CLK_PIN                   APP_QSPI0_CLK_PIN
#define APP_FLASH_IO0_PIN                   APP_QSPI0_IO0_PIN
#define APP_FLASH_IO1_PIN                   APP_QSPI0_IO1_PIN
#define APP_FLASH_IO2_PIN                   APP_QSPI0_IO2_PIN
#define APP_FLASH_IO3_PIN                   APP_QSPI0_IO3_PIN
#define APP_FLASH_QSPI_ID                   APP_QSPI_ID_0


/**
 * @defgroup BSP_MAROC Defines
 * @{
 */
    #define BSP_KEY_UP_ID       0x00    /**< ID for UP KEY. */
    #define BSP_KEY_OK_ID       0x01    /**< ID for OK KEY. */

    #define UART_TX_BUFF_SIZE   0x400   /**< Size of app uart tx buffer. */
/** @} */

/**
 * @defgroup BSP_ENUM Enumerations
 * @{
 */
typedef enum
{
    BSP_LED_NUM_0,
    BSP_LED_NUM_1,
} bsp_led_num_t;
/** @} */

/**
 * @defgroup BSP_FUNCTION Functions
 * @{
 */
/**
 *****************************************************************************************
 * @brief Initialize boards key.
 *****************************************************************************************
 */
void bsp_key_init(void);

/**
 *****************************************************************************************
 * @brief App key event handler
 *****************************************************************************************
 */
void app_key_evt_handler(uint8_t key_id, app_key_click_type_t key_click_type);

/**
 *****************************************************************************************
 * @brief Initialize app uart.
 *****************************************************************************************
 */
void bsp_uart_init(void);

/**
 *****************************************************************************************
 * @brief Uart data send.
 *****************************************************************************************
 */
void bsp_uart_send(uint8_t *p_data, uint16_t length);

/**
 *****************************************************************************************
 * @brief Uart data flush.
 *****************************************************************************************
 */
void bsp_uart_flush(void);

/**
 *****************************************************************************************
 * @brief App uart event handler.
 *****************************************************************************************
 */
void app_uart_evt_handler(app_uart_evt_t *p_evt);

/**
 *****************************************************************************************
 * @brief Initialize boards led.
 *****************************************************************************************
 */
void bsp_led_init(void);

/**
 *****************************************************************************************
 * @brief Open boards led.
 *
 * @param[in] led_num: Number of led needed open.
 *****************************************************************************************
 */
void bsp_led_open(bsp_led_num_t led_num);

/**
 *****************************************************************************************
 * @brief Close boards led.
 *
 * @param[in] led_num: Number of led needed close.
 *****************************************************************************************
 */
void bsp_led_close(bsp_led_num_t led_num);

/**
 *****************************************************************************************
 * @brief BSP log init.
 *****************************************************************************************
 */
void bsp_log_init(void);

/**
 *****************************************************************************************
 * @brief Board init.
 *****************************************************************************************
 */
void board_init(void);

#endif  /* __BOARD_SK_H__ */
