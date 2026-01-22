/**
 * @file esp8266_wifi.c
 * @brief 
 * @author ZixuanHung (13246009616@163.com)
 * @version 1.0
 * @date 2025-05-07
 * 
 * @copyright Copyright (c) 2025  ZixuanHung
 * 
 * @par LOG:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2025-05-07 <td>1.0     <td>ZixuanHung     <td>ESP8266 WIFI模块实现
 * </table>
 */

#include "esp8266_wifi.h"
#include <string.h>
#include <stdio.h>
#include "SEGGER_RTT.h"
#include "WIFI.h"
#include "app_uart_dma.h"
#include "esp32_c6.h"
#include "uart.h"

// 设备信息全局变量
static char device_sn[32] = "1214";  // 默认SN号
extern char device_mac[20];  // 在main.c中定义的全局变量

// WiFi连接配置
static char wifi_ssid[32] = "";
static char wifi_password[64] = "";

volatile ESP8266_Status esp8266_status = ESP8266_IDLE;

// 接收缓冲区
static char rx_buffer[1024];
static uint16_t rx_index = 0;

extern void delay_ms(uint32_t ms);



/**
 * 发送AT命令并等待响应
 * @param cmd AT命令
 * @param resp 期望的响应
 * @param timeout 超时时间(ms)
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_SendCommand(const char *cmd, const char *resp, uint32_t timeout) {
    uint32_t time = 0;
    // 清空接收缓冲区
    memset(wifi_uart_rx_buffer, 0, sizeof(wifi_uart_rx_buffer));
		SEGGER_RTT_printf(0, "[TX]:%s\r\n", cmd);
    rx_index = 0;
    
    // 发送AT命令
    app_uart_transmit_sync(WIFI_UART_ID,(uint8_t*)cmd, strlen(cmd),1000);
		// 等待响应
		app_uart_receive_sync(WIFI_UART_ID, wifi_uart_rx_buffer, sizeof(wifi_uart_rx_buffer),timeout);
		SEGGER_RTT_printf(0, "%s\r\n", wifi_uart_rx_buffer);
		if (strstr((char *)wifi_uart_rx_buffer, resp) != NULL) {
				SEGGER_RTT_printf(0, "OK\r\n", wifi_uart_rx_buffer);
				return 1; // 成功
		}
		
    SEGGER_RTT_printf(0, "[RX]:fail\r\n");
    return 0; // 超时
}

/**
 * ESP8266接收数据处理
 * @param data 接收的数据
 * @param len 数据长度
 */
void ESP8266_RxCallback(const char *data, uint16_t len) {
    // 确保不会缓冲区溢出
    if (rx_index + len < sizeof(rx_buffer)) {
        memcpy(&rx_buffer[rx_index], data, len);
        rx_index += len;
        rx_buffer[rx_index] = '\0'; // 确保字符串结尾
    }
}

/**
 * 初始化ESP8266模块
 * @param ssid WiFi名称
 * @param password WiFi密码
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_Init(const char *ssid, const char *password) {
    // 保存WiFi配置
    strncpy(wifi_ssid, ssid, sizeof(wifi_ssid) - 1);
    strncpy(wifi_password, password, sizeof(wifi_password) - 1);
    
    // 复位ESP8266
    if (!ESP8266_SendCommand(AT_RST, AT_OK, 5000)) {
        return 0;
    }
    delay_ms(1000);
    
    // 设置Station模式
    if (!ESP8266_SendCommand(AT_CWMODE, AT_OK, 2000)) {
        return 0;
    }
    
    // 设置非透传模式
    if (!ESP8266_SendCommand(AT_CIPMODE_NOR, AT_OK, 2000)) {
        return 0;
    }
    
    return 1;
}

/**
 * 连接WiFi
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_ConnectWiFi(const char *ssid, const char *password) {
    char cmd[128];
    
	  strncpy(wifi_ssid, ssid, sizeof(wifi_ssid) - 1);
    strncpy(wifi_password, password, sizeof(wifi_password) - 1);
    
    // 格式化AT命令
    snprintf(cmd, sizeof(cmd), AT_CWJAP, wifi_ssid, wifi_password);
    
    // 发送连接WiFi命令
    if (!ESP8266_SendCommand(cmd, AT_OK, 5000)) {
				SEGGER_RTT_printf(0, "[RX]:fail\r\n");
        return 0;
    }
    SEGGER_RTT_printf(0, "[RX]:wifi connect\r\n");
    return 1;
    
    return 1;
}

/**
 * 连接到服务器
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_ConnectServer(void) {
    char cmd[128];
    
    // 格式化连接命令
    snprintf(cmd, sizeof(cmd), AT_CIPSTART, SERVER_HOST, SERVER_PORT);
    
    // 发送连接服务器命令
    if ((!ESP8266_SendCommand(cmd, AT_OK, 2000))) {
				SEGGER_RTT_printf(0, "[RX]:fail\r\n");
        return 0;
    }
    SEGGER_RTT_printf(0, "[RX]:Server connect\r\n");
    esp8266_status = ESP8266_CONNECTED;
		ESP8266_SendCommand(AT_CIPMODE_TSP, AT_OK,2000);
    if (!ESP8266_SendCommand(AT_CIPSEND, ">", 5000)) {
        return 0;
    }		
    return 1;
}

/**
 * 发送数据到服务器
 * @param data 数据指针
 * @param len 数据长度
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_SendData(const char *data, uint16_t len) {
    char cmd[32];
    
    if (esp8266_status != ESP8266_CONNECTED) {
        return 0;
    }
//    ESP8266_SendCommand(AT_CIPMODE_TSP, AT_OK,2000);
//    // 设置发送长度
//    snprintf(cmd, sizeof(cmd), AT_CIPSEND);
//    if (!ESP8266_SendCommand(AT_CIPSEND, ">", 5000)) {
//        return 0;
//    }
    SEGGER_RTT_printf(0, "Login....\r\n");
		// 清空接收缓冲区
    memset(wifi_uart_rx_buffer, 0, sizeof(wifi_uart_rx_buffer));
    // 发送数据
		SEGGER_RTT_printf(0, "POST:%s\r\n", data);
		delay_ms(100);
		SEGGER_RTT_printf(0, "POST:%s\r\n", data);
		delay_ms(100);
		app_uart_transmit_sync(WIFI_UART_ID,(uint8_t*)data, len,2000);
    
    // 等待发送完成
		app_uart_receive_sync(WIFI_UART_ID, wifi_uart_rx_buffer, sizeof(wifi_uart_rx_buffer),2000);	
		SEGGER_RTT_printf(0, "RX:%s\r\n", wifi_uart_rx_buffer);
		if (strstr((char *)wifi_uart_rx_buffer, "token") != NULL) {
				SEGGER_RTT_printf(0, "login OK\r\n", wifi_uart_rx_buffer);
				return 1; // 成功
		}
//    if (!ESP8266_SendCommand("", "token", 2000)) {
//        return 0;
//    }
    SEGGER_RTT_printf(0, "login FAIL\r\n", wifi_uart_rx_buffer);
    return 0;
}

/**
 * 构建并发送HTTP POST请求
 * @param data_json 要发送的JSON数据
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_SendHTTPPost(const char *data_json) {
    char http_request[512];
    
    // 构建HTTP POST请求
    snprintf(http_request, sizeof(http_request),
             "POST %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n\r\n"
             "%s",
             API_URL, SERVER_HOST, strlen(data_json), data_json);
    
    // 发送HTTP请求
    return ESP8266_SendData(http_request, strlen(http_request));
}

/**
 * 发送采集数据到服务器
 * @param collect_data 采集的数据字符串
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_SendCollectData(const char *collect_data) {
    char json_data[256];
    
    // 构建JSON数据 - 使用标准JSON对象格式
    snprintf(json_data, sizeof(json_data), 
             "{\"data\":{\"collect\":\"%s\"}}",
             collect_data);
    
    // 发送HTTP POST请求
    return ESP8266_SendHTTPPost(json_data);
}

/**
 * 关闭连接
 */
void ESP8266_CloseConnection(void) {
		ESP8266_SendCommand(AT_EXIT_TSP, "",1000);
    ESP8266_SendCommand("AT+CIPCLOSE\r\n", AT_OK, 2000);
    esp8266_status = ESP8266_IDLE;
}

/**
 * 解析响应中的MAC地址
 * @param response 响应字符串
 * @return 1:成功 0:失败
 */
uint8_t parse_mac_address(const char *response) {
    char *mac_start = strstr(response, "+CIPSTAMAC:\"");
    
    if (mac_start != NULL) {
        mac_start += 12;  // 跳过"+CIPSTAMAC:"
        
        int i = 0;
        while (mac_start[i] != '\"' && i < 17) {
            device_mac[i] = mac_start[i];
            i++;
        }
        device_mac[i] = '\0';
        return 1;
    }
    
    return 0;
}

/**
 * 解析响应中的SN号
 * @param response 响应字符串
 * @return 1:成功 0:失败
 */
uint8_t parse_sn(const char *response) {
    char *sn_start = strstr(response, "ChipID:");
    
    if (sn_start != NULL) {
        sn_start += 7;  // 跳过"ChipID:"
        
        // 去除前导空白
        while (*sn_start == ' ' || *sn_start == '\t') {
            sn_start++;
        }
        
        int i = 0;
        while (sn_start[i] != '\r' && sn_start[i] != '\n' && sn_start[i] != '\0' && i < 31) {
            device_sn[i] = sn_start[i];
            i++;
        }
        device_sn[i] = '\0';
        return 1;
    }
    
    return 0;
}

/**
 * 获取ESP8266模块的MAC地址
 * @return 1:成功 0:失败
 */
uint8_t get_esp8266_mac_address(void) {
    // 清空接收缓冲区
    memset(rx_buffer, 0, sizeof(rx_buffer));
    rx_index = 0;
    
    // 发送获取MAC地址命令
    if (!ESP8266_SendCommand(AT_CIPSTAMAC, AT_OK, 2000)) {
        return 0;
    }
    
    // 解析MAC地址
    return parse_mac_address(rx_buffer);
}

/**
 * 获取ESP8266模块的SN号
 * @return 1:成功 0:失败
 */
uint8_t get_esp8266_sn(void) {
    // 清空接收缓冲区
    memset(rx_buffer, 0, sizeof(rx_buffer));
    rx_index = 0;
    
    // 发送获取版本信息命令，其中包含SN号
    if (!ESP8266_SendCommand(AT_GMR, AT_OK, 2000)) {
        return 0;
    }
    
    // 解析SN号
    return parse_sn(rx_buffer);
}

/**
 * 获取设备SN号
 * @return 设备SN号
 */
char* get_device_sn(void) {
    return device_sn;
} 

