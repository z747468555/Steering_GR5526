/**
 * @file esp8266_wifi.h
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
 * <tr><td>2025-05-07 <td>1.0     <td>ZixuanHung     <td>ESP8266 WIFI模块头文件
 * </table>
 */

#ifndef __ESP8266_WIFI_H
#define __ESP8266_WIFI_H

#include <stdint.h>

// 服务器信息
#define SERVER_HOST "www.yunhantec.com"
#define SERVER_PORT 6015
#define API_URL "/api/wristband/collectData"

// 配置WiFi参数
#define WIFI_SSID     "CharlesWong"
#define WIFI_PASSWORD "qqzixuan"

// AT命令集
#define AT_OK "OK\r\n"
#define AT_RST "AT+RST\r\n"
#define AT_CWMODE "AT+CWMODE=1\r\n"  // Station模式
#define AT_CWJAP "AT+CWJAP=\"%s\",\"%s\"\r\n"  // 连接WiFi
#define AT_CIPSTART "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n"  // 建立TCP连接
#define AT_CIPSEND "AT+CIPSEND\r\n"  // 发送数据长度
#define AT_CIPMODE_NOR "AT+CIPMODE=0\r\n"  // 非透传模式
#define AT_CIPMODE_TSP "AT+CIPMODE=1\r\n"  // 透传模式
#define AT_CIPSTATUS "AT+CIPSTATUS\r\n"  // 查询连接状态
#define AT_GMR "AT+GMR\r\n"  // 获取版本信息和SN号
#define AT_CIPSTAMAC "AT+CIPSTAMAC?\r\n"  // 获取MAC地址
#define AT_READY "ready\r\n"
#define AT_EXIT_TSP "+++"
#define AT_CONNECTED "CONNECT"
#define AT_ATE0 "ATE0\r\n"	//关闭回显

// ESP8266状态
typedef enum {
    ESP8266_IDLE,
    ESP8266_BUSY,
    ESP8266_CONNECTED,
    ESP8266_ERROR
} ESP8266_Status;

extern volatile ESP8266_Status esp8266_status;

/**
 * 初始化ESP8266模块
 * @param ssid WiFi名称
 * @param password WiFi密码
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_Init(const char *ssid, const char *password);

/**
 * 发送AT命令并等待响应
 * @param cmd AT命令
 * @param resp 期望的响应
 * @param timeout 超时时间(ms)
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_SendCommand(const char *cmd, const char *resp, uint32_t timeout);

/**
 * 连接WiFi
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_ConnectWiFi(const char *ssid, const char *password);

/**
 * 连接到服务器
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_ConnectServer(void);

/**
 * 发送数据到服务器
 * @param data 数据指针
 * @param len 数据长度
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_SendData(const char *data, uint16_t len);

/**
 * 构建并发送HTTP POST请求
 * @param data_json 要发送的JSON数据
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_SendHTTPPost(const char *data_json);

/**
 * 发送采集数据到服务器
 * @param collect_data 采集的数据字符串
 * @return 1:成功 0:失败
 */
uint8_t ESP8266_SendCollectData(const char *collect_data);

/**
 * 关闭连接
 */
void ESP8266_CloseConnection(void);

/**
 * ESP8266接收数据处理函数
 * 当UART接收到数据时，调用此函数
 * @param data 接收的数据
 * @param len 数据长度
 */
void ESP8266_RxCallback(const char *data, uint16_t len);

/**
 * 获取ESP8266模块的MAC地址
 * @return 1:成功 0:失败
 */
uint8_t get_esp8266_mac_address(void);

/**
 * 获取ESP8266模块的SN号
 * @return 1:成功 0:失败
 */
uint8_t get_esp8266_sn(void);

/**
 * 获取设备信息结构体指针
 * @return 设备信息指针
 */
char* get_device_sn(void);

#endif /* __ESP8266_WIFI_H */ 

