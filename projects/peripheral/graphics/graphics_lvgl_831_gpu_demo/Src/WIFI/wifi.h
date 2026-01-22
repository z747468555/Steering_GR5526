#ifndef _WIFI_H_
#define _WIFI_H_

#include "board_SK.h"

#define WIFI_UART_ID                         APP_UART_ID_3
#define WIFI_UART_BAUDRATE                   115200
#define WIFI_UART_TX_IO_TYPE                 APP_IO_TYPE_MSIO
#define WIFI_UART_RX_IO_TYPE                 APP_IO_TYPE_MSIO
#define WIFI_UART_TX_PIN                     APP_IO_PIN_1
#define WIFI_UART_RX_PIN                     APP_IO_PIN_0
#define WIFI_UART_TX_PINMUX                  APP_IO_MUX_2
#define WIFI_UART_RX_PINMUX                  APP_IO_MUX_2 	
#define WIFI_UART_TX_PULL                    APP_IO_PULLUP
#define WIFI_UART_RX_PULL                    APP_IO_PULLUP

//#define WIFI_EN_PIN                          APP_IO_PIN_13
//#define WIFI_EN_TYPE                         APP_IO_TYPE_GPIOA
//#define WIFI_EN_PULL                         APP_IO_PULLDOWN
//#define WIFI_EN_PIN_MUX                      APP_IO_MUX_7

#define UART_DATA_LENTH                       (1024)

extern uint8_t wifi_uart_tx_buffer[UART_DATA_LENTH];
extern uint8_t wifi_uart_rx_buffer[UART_DATA_LENTH];
extern uint8_t wifi_uart_ring_buffer[UART_DATA_LENTH];
void UART_Init(void);
extern void ecg_uart_send_raw_data(int16_t *data, uint32_t len);
/**
 * @brief wifi模块测试
 */
void WIFI_Test();

#endif // !_WIFI_H_


