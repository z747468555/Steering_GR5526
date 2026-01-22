/**
 * @file wifi.c
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
 * <tr><td>2025-05-07 <td>1.0     <td>ZixuanHung     <td>WIFI测试模块
 * </table>
 */
#include "WIFI.h"
#include "app_uart.h"
#include "app_io.h"
#include "SEGGER_RTT.h"
//#include "log_uart.h"
#include "string.h"
//#include "esp8266_wifi.h"
//#include "esp8266_api.h"
//#include "crypto.h"

// WiFi连接配置
//static char wifi_ssid[32] = "";
//static char wifi_password[64] = "";
// 设备信息变量
//char device_mac[20] = "11:11:11:11:11:15"; // 默认MAC地址，将被动态获取的地址替换

//#define DEVICE_SN "1214"
//#define DEVICE_MAC "11:11:11:11:11:15"

uint8_t wifi_uart_tx_buffer[UART_DATA_LENTH] = {0};
uint8_t wifi_uart_rx_buffer[UART_DATA_LENTH] = {0};
uint8_t wifi_uart_ring_buffer[UART_DATA_LENTH] = {0};

volatile uint16_t rec_len = 0;
volatile uint8_t wifi_uart_tdone = 0;
volatile uint8_t wifi_uart_rdone = 0;

app_uart_params_t wifi_uart_params = {
    .id      = WIFI_UART_ID,
    .pin_cfg = {
           .tx = {
              .type = WIFI_UART_TX_IO_TYPE,
              .mux  = WIFI_UART_TX_PINMUX,
              .pin  = WIFI_UART_TX_PIN,
              .pull = WIFI_UART_TX_PULL,
           },
           .rx = {
              .type = WIFI_UART_RX_IO_TYPE,
              .mux  = WIFI_UART_RX_PINMUX,
              .pin  = WIFI_UART_RX_PIN,
              .pull = WIFI_UART_RX_PULL,
           },
    },

    .init = {
        .baud_rate = WIFI_UART_BAUDRATE,
        .data_bits = UART_DATABITS_8,
        .stop_bits = UART_STOPBITS_1,
        .parity    = UART_PARITY_NONE,
        .hw_flow_ctrl    = UART_HWCONTROL_NONE,
        .rx_timeout_mode = UART_RECEIVER_TIMEOUT_ENABLE,
    },
};
uint8_t uart_rx_len = 0;
void wifi_uart_callback(app_uart_evt_t *p_evt)
{
    if (p_evt->type == APP_UART_EVT_TX_CPLT)
    {
        wifi_uart_tdone = 1;
				SEGGER_RTT_printf(0,"tdone\r\n");
    }
    if (p_evt->type == APP_UART_EVT_RX_DATA)
    {
        wifi_uart_rdone = 1;
//        rec_len = p_evt->data.size;
//			    // 存储接收到的数据
//				if (uart_rx_len + rec_len < sizeof(wifi_uart_rx_buffer)) {
//						uart_rx_len += rec_len;
//						wifi_uart_rx_buffer[uart_rx_len] = '\0'; // 确保字符串结尾
//				}
				SEGGER_RTT_printf(0,"rdone\r\n");
				// 传递给ESP8266模块处理
//				ESP8266_RxCallback((const char *)wifi_uart_rx_buffer, uart_rx_len);
//			  SEGGER_RTT_printf(0, "wifi_uart_rdone\r\n", wifi_uart_rx_buffer);
    }
    if (p_evt->type == APP_UART_EVT_ERROR)
    {
        wifi_uart_tdone = 1;
        wifi_uart_rdone = 1;
    }
}

/**
 * @brief Uart模块初始化
 */
void UART_Init()
{
    app_io_init_t io_init;
    app_uart_tx_buf_t uart_buffer = {0};
    uint16_t ret;

    uart_buffer.tx_buf = wifi_uart_ring_buffer;
    uart_buffer.tx_buf_size = sizeof(wifi_uart_ring_buffer);
    ret = app_uart_init(&wifi_uart_params, wifi_uart_callback, &uart_buffer);
    if(ret != APP_DRV_SUCCESS){
        SEGGER_RTT_printf(0, "[error] error_code:0x%X, %s, %s", ret, __FILE__, __LINE__);
    }
		SEGGER_RTT_printf(0,"Uart init\r\n");
		app_uart_transmit_sync(WIFI_UART_ID,(uint8_t*)"Uart Init\r\n", strlen("Uart Init\r\n"),100);
		 // 启动接收
    app_uart_receive_async(WIFI_UART_ID, wifi_uart_rx_buffer, sizeof(wifi_uart_rx_buffer));
	
}

void ecg_uart_send_raw_data(int16_t *data, uint32_t len)
{
    if (data == NULL || len == 0)
    {
        return;
    }
    
    // 等待上次发送完成
//    while (!wifi_uart_tdone);
    
    // 格式化数据：帧头 + 数据 + 帧尾
    // 格式: $ECG,数据1,数据2,...,数据N\r\n
    int offset = 0;
    offset += sprintf((char*)wifi_uart_tx_buffer, "$ECG");
    
    for (uint32_t i = 0; i < len && offset < sizeof(wifi_uart_tx_buffer) - 10; i++)
    {
        offset += sprintf((char*)wifi_uart_tx_buffer + offset, ",%d", data[i]);
    }
    
    offset += sprintf((char*)wifi_uart_tx_buffer + offset, "\r\n");
    
    // 发送数据
    wifi_uart_tdone = false;
    app_uart_transmit_async(WIFI_UART_ID, wifi_uart_tx_buffer, offset);
}

///**
// * 发送AT命令并等待响应
// * @param cmd AT命令
// * @param resp 期望的响应
// * @param timeout 超时时间(ms)
// * @return 1:成功 0:失败
// */
//static uint16_t rx_index = 0;
//uint8_t ESP8266_SendCommand(const char *cmd, const char *resp, uint32_t timeout) {
//    uint32_t time = 0;
//    // 清空接收缓冲区
//    memset(wifi_uart_rx_buffer, 0, sizeof(wifi_uart_rx_buffer));
//		SEGGER_RTT_printf(0, "[TX]:%s\r\n", cmd);
//    rx_index = 0;
//    
//    // 发送AT命令
//    app_uart_transmit_sync(WIFI_UART_ID,(uint8_t*)cmd, strlen(cmd),1000);
//		// 等待响应
//		app_uart_receive_sync(WIFI_UART_ID, wifi_uart_rx_buffer, sizeof(wifi_uart_rx_buffer),timeout);

//		if (strstr((char *)wifi_uart_rx_buffer, resp) != NULL) {
//				SEGGER_RTT_printf(0, "OK\r\n", wifi_uart_rx_buffer);
//				return 1; // 成功
//		}
//	
//    SEGGER_RTT_printf(0, "[RX]:fail\r\n");
//    return 0; // 超时
//}

///**
// * 连接WiFi
// * @return 1:成功 0:失败
// */
//uint8_t ESP8266_ConnectWiFi(const char *ssid, const char *password) {
//    char cmd[128];
//	
//	  strncpy(wifi_ssid, ssid, sizeof(wifi_ssid) - 1);
//    strncpy(wifi_password, password, sizeof(wifi_password) - 1);
//    
//    // 格式化AT命令
//    snprintf(cmd, sizeof(cmd), AT_CWJAP, wifi_ssid, wifi_password);
//    
//    // 发送连接WiFi命令
//    if (!ESP8266_SendCommand(cmd, AT_OK, 5000)) {
//				SEGGER_RTT_printf(0, "[RX]:fail\r\n");
//        return 0;
//    }
//    SEGGER_RTT_printf(0, "[RX]:wifi connect\r\n");
//    return 1;
//}


/**
 * @brief wifi模块测试
 */
//void WIFI_Test()
//{
//    uint8_t cmd0[] = {"AT\r\n"};
//    uint8_t cmd1[] = {"AT+CWMODE=1\r\n"};
//    uint8_t cmd2[] = {"AT+RST\r\n"};
//    uint8_t cmd3[] = {"AT+CWJAP=\"CharlesWong\",\"qqzixuan\"\r\n"};
//		uint8_t cmd4[] = {"AT+CIPSTART=\"TCP\",\"www.yunhantec.com\",6015\r\n"};
//		uint8_t cmd5[] = {"AT+CIPMODE=1\r\n"};
//		uint8_t cmd6[] = {"AT+CIPSEND\r\n"};
//		uint8_t cmd7[] = {"GET /api/test/test HTTP/1.1\r\nHost: www.yunhantec.com\r\nConnection: close\r\n"};
//	SEGGER_RTT_printf(0, "\r\nstart WIFI test\r\n");
//    wifi_uart_tdone = 0;
//	
////    delay_ms(3000);
//		
////		ESP8266_SendCommand(AT_RST, AT_READY,2000);
////    
////    ESP8266_SendCommand(AT_CWMODE, AT_OK,2000);
////		
////		ESP8266_SendCommand(AT_CIPMODE_NOR, AT_OK,2000);
////		
////		ESP8266_ConnectWiFi(WIFI_SSID, WIFI_PASSWORD);
//		
////		ESP8266_ConnectServer();
////		
////		ESP8266_SendCommand(AT_CIPMODE_TSP, AT_OK,1000);
////		
////		ESP8266_SendCommand(AT_CIPSEND, AT_OK,1000);
//		
////		ESP8266_SendCommand(AT_EXIT_TSP, " ",1000);
////		
////		ESP8266_SendCommand(AT_CIPMODE_NOR, AT_OK,2000);
////		
////		ESP8266_SendCommand(AT_CWMODE, AT_OK,2000);
//		ESP8266_SendCommand(AT_ATE0, AT_OK,2000);
//		// 设备登录
//    Device_Login_Response login_resp;
//		
//		API_Status status;
//		
//		// 初始化API模块
//    API_Init();
//		
//		status = API_DeviceLogin(DEVICE_SN, DEVICE_MAC, &login_resp);
//		
//		if(status ==API_SUCCESS)
//		{
//				// 保存登录信息
//			API_SetLoginInfo(DEVICE_SN, login_resp.token);
//	//		SEGGER_RTT_printf(0, "sn:%s,token:%s\r\n",login_resp.sn,login_resp.token);
//			
//			// 更新API签名参数，使用获取到的正式token
//			SetAPISignParams(API_DEVICE_LOGIN, login_resp.sn, login_resp.token);
//			
//			// 上传采集数据
//			Collect_Data_Response collect_resp;
//			uint8_t heart_rate = 75;   // 模拟心率数据
//			uint8_t blood_oxygen = 98; // 模拟血氧数据
//			
//			SEGGER_RTT_printf(0,"\nUploadCollectData...\r\n");
//			SEGGER_RTT_printf(0,"HR: %d, BO: %d\r\n", heart_rate, blood_oxygen);
//			
//			status = API_UploadCollectData(heart_rate, blood_oxygen, &collect_resp);
//			if (status == API_SUCCESS) {
//					printf("SUCCESS\r\n");
//					printf("BodySta: %d, TrainInden: %d\r\n", collect_resp.stzk, collect_resp.zxqd);
//			} else {
//					printf("FailCode: %d\r\n", status);
//			}		
//					
//			ESP8266_CloseConnection();
//		}
//		
//    delay_ms(1000);
//}




