/**
 * @file esp8266_api.c
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
 * <tr><td>2025-05-07 <td>1.0     <td>ZixuanHung     <td>ESP8266 API接口实现
 * </table>
 */

#include "esp8266_api.h"
#include "esp8266_wifi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "crypto.h"
#include "WIFI.h"
#include "SEGGER_RTT.h"

// 服务器API基础路径
#define API_BASE_URL "/api"

// 存储登录信息
static char device_sn[64] = "";
static char device_token[130] = "";
static char device_nonce[64] = "";

// 缓冲区
static char request_buffer[1024];
static uint8_t response_buffer[1024];


// 外部声明的函数
extern uint8_t SM3_Hash(const char *input, char *output);
extern uint8_t SM4_Encrypt(const char *input, const char *key, char *output);
extern uint8_t SM4_Decrypt(const char *input, const char *key, char *output);
extern uint32_t GetTimeStamp(void);
extern const char* GetNonce(void);

// 生成随机字符串
static void GenerateRandomString(char *output, uint8_t length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
    for (uint8_t i = 0; i < length; i++) {
        uint8_t index = rand() % (sizeof(charset) - 1);
        output[i] = charset[index];
    }
    output[length] = '\0';
}

/**
 * 生成API签名
 * @param url API请求URL路径（不会被修改）
 * @param timestamp 时间戳
 * @param output 输出缓冲区，至少需要41字节空间
 * @return 1成功，0失败
 */
static int GenerateSignature(const char *url, uint32_t timestamp, char *output) {
    if (!url || !output) return 0;
    
    char signStr[512] = {0};  // 更大的缓冲区以防溢出
    const char *parts[5];     // 使用const避免类型转换
    char timestamp_str[32] = {0};
    const char *nonce = GetNonce();
    size_t total_len = 0;
    
    int ret = snprintf(timestamp_str, sizeof(timestamp_str), "%u", timestamp);
    if (ret < 0 || ret >= sizeof(timestamp_str)) return 0;
    
    // 准备排序的数组
    parts[0] = url;
    parts[1] = device_sn;
    parts[2] = nonce;
    parts[3] = timestamp_str;
    parts[4] = device_token;
    
    // 预计算总长度
    for (int i = 0; i < 5; i++) {
        total_len += strlen(parts[i]);
    }
    
    // 确保缓冲区足够大
    if (total_len >= sizeof(signStr)) return 0;
    
    // 排序数组
    qsort((void*)parts, 5, sizeof(char*), string_compare);
    
    // 安全连接字符串
    for (int i = 0; i < 5; i++) {
        size_t current_len = strlen(signStr);
        size_t part_len = strlen(parts[i]);
        
        if (current_len + part_len < sizeof(signStr) - 1) {
            strncat(signStr, parts[i], sizeof(signStr) - current_len - 1);
        } else {
            return 0; // 缓冲区不足
        }
    }
    
    // 计算哈希
    if (!SHA1_Hash(signStr, strlen(signStr), output)) {
        return 0;
    }
    
    return 1;
}

// 初始化Key
static void InitKey(const char *token, char *output) {
    char key[64];
    
    // 使用SM3对token进行哈希
    SM3_Hash(token, key);
    
    // 取前32位
    strncpy(output, key, 32);
    output[32] = '\0';
}

/**
 * UTF-8字符串安全复制函数
 * 确保不会在多字节字符中间截断
 * @param dest 目标缓冲区
 * @param src 源字符串
 * @param max_bytes 最大复制字节数
 */
static void utf8_safe_strncpy(char *dest, const char *src, size_t max_bytes) {
    size_t i = 0;
    size_t bytes = 0;
    
    // 复制完整的UTF-8字符，确保不会在字符中间截断
    while (src[i] && bytes < max_bytes - 1) {
        unsigned char c = (unsigned char)src[i];
        size_t char_bytes = 1;
        
        // 检测UTF-8多字节序列
        if ((c & 0xE0) == 0xE0) char_bytes = 3;      // 1110xxxx - 3字节字符
        else if ((c & 0xC0) == 0xC0) char_bytes = 2; // 110xxxxx - 2字节字符
        else if ((c & 0x80) == 0x80) {
            // 非法UTF-8序列开始，跳过
            i++;
            continue;
        }
        
        // 检查是否超出缓冲区
        if (bytes + char_bytes > max_bytes - 1) break;
        
        // 复制完整的UTF-8序列
        for (size_t j = 0; j < char_bytes && src[i]; j++, i++, bytes++) {
            dest[bytes] = src[i];
        }
    }
    
    // 确保字符串以NULL结尾
    dest[bytes] = '\0';
}

// 解析JSON响应
static uint8_t ParseJsonResponse(const char *json, void *response, uint8_t type) {
    // 这里应该实现JSON解析
    // 由于MCU环境可能受限，这里只做简单示例
    // 实际实现可能需要使用更轻量级的解析方式或自定义解析
    
    // 示例实现
    switch (type) {
        case 1: { // 设备登录
            Device_Login_Response *login_resp = (Device_Login_Response *)response;
            
            char *token_start = strstr(json, "\"token\":\"");
            char *identity_start = strstr(json, "\"identity\":\"");
            
            if (token_start) {
                token_start += 9; // 跳过"token":"
                char *token_end = strchr(token_start, '\"');
                
                if (token_end) {
                    // 使用UTF-8安全的字符串复制
                    utf8_safe_strncpy(login_resp->token, token_start, token_end - token_start < 129 ? (token_end - token_start + 1) : 128);
//										SEGGER_RTT_printf(0, "token len:%d\r\n", token_end - token_start);
//										SEGGER_RTT_printf(0, "token:%s\r\n", login_resp->token);
                    // 如果找到了identity字段，将其作为sn存储
                    if (identity_start) {
                        identity_start += 12; // 跳过"identity":"
                        char *identity_end = strchr(identity_start, '\"');
                        
                        if (identity_end) {
                            // 使用UTF-8安全的字符串复制
                            utf8_safe_strncpy(login_resp->sn, identity_start, identity_end - identity_start < 64 ? (identity_end - identity_start + 1) : 63);
//														SEGGER_RTT_printf(0, "sn:%s\r\n", login_resp->sn);
                            return 1;
                        }
                    } else {
                        // 如果没有identity字段，将sn设置为空字符串
                        login_resp->sn[0] = '\0';
                        return 1;
                    }
                }
            }
            break;
        }
        
        case 2: { // 上传采集数据
            Collect_Data_Response *collect_resp = (Collect_Data_Response *)response;
            
            // 模拟解析
            char *stzk_start = strstr(json, "\"stzk\":");
            char *zxqd_start = strstr(json, "\"zxqd\":");
            
            if (stzk_start && zxqd_start) {
                collect_resp->stzk = atoi(stzk_start + 7);
                collect_resp->zxqd = atoi(zxqd_start + 7);
                return 1;
            }
            break;
        }
        
        case 4: { // 获取日历数据
            uint32_t *time_stamp = (uint32_t *)response;
            
            // 模拟解析
            char *time_start = strstr(json, "\"time\":");
            
            if (time_start) {
                *time_stamp = atol(time_start + 7);
                return 1;
            }
            break;
        }
        
        case 5: { // 获取指令
            Get_Instructions_Response *instr_resp = (Get_Instructions_Response *)response;
            
            // 模拟解析
            char *zl_start = strstr(json, "\"zl\":[");
            
            if (zl_start) {
                char *item_start = zl_start + 6;
                uint8_t count = 0;
                
                // 简单解析指令数组
                while (*item_start && count < 10) {
                    if (*item_start == '\"') {
                        item_start++;
                        char *item_end = strchr(item_start, '\"');
                        
                        if (item_end) {
                            char item[16] = {0};
                            // 使用UTF-8安全的字符串复制
                            utf8_safe_strncpy(item, item_start, item_end - item_start < 15 ? item_end - item_start : 14);
                            
                            // 解析指令
                            if (strchr(item, ':')) {
                                // 带参数指令
                                char *param_part = strchr(item, ':');
                                *param_part = '\0';
                                param_part++;
                                
                                instr_resp->instructions[count].type = (Instruction_Type)atoi(item);
                                instr_resp->instructions[count].param = atoi(param_part);
                            } else {
                                // 无参数指令
                                instr_resp->instructions[count].type = (Instruction_Type)atoi(item);
                                instr_resp->instructions[count].param = 0;
                            }
                            
                            count++;
                            item_start = item_end + 1;
                        } else {
                            break;
                        }
                    } else {
                        item_start++;
                    }
                }
                
                instr_resp->count = count;
                return 1;
            }
            break;
        }
        
        case 6: { // 获取自主操作权限
            Get_Self_Op_Response *op_resp = (Get_Self_Op_Response *)response;
            
            // 模拟解析
            char *zzcz_start = strstr(json, "\"zzcz\":");
            
            if (zzcz_start) {
                op_resp->zzcz = (Self_Operation_Status)atoi(zzcz_start + 7);
                return 1;
            }
            break;
        }
        
        default:
            break;
    }
    
    return 0;
}

/**
 * 发送API请求并接收响应
 * @param api_path API路径
 * @param json_data JSON数据
 * @param need_encrypt 是否需要加密
 * @param need_auth 是否需要认证
 * @param response 响应数据
 * @param response_type 响应类型
 * @return API_Status
 */
static API_Status SendAPIRequest(const char *api_path, const char *json_data, 
                                uint8_t need_encrypt, uint8_t need_auth,
                                void *response, uint8_t response_type) {
    char url[128];
    char now_data[512];
    char signature[64];
    char encrypted_data[512];
    char key[33];
    uint32_t now_time;
    
    // 构建完整URL
    snprintf(url, sizeof(url), "%s%s", API_BASE_URL, api_path);
    
//    // 连接服务器
//    if (!ESP8266_ConnectServer()) {
//        return API_ERROR_CONNECT;
//    }
		
		
    
    // 初始化请求数据
    memset(request_buffer, 0, sizeof(request_buffer));
    
    // 是否需要认证
    if (need_auth) {
        now_time = GetTimeStamp();
        
        // 获取当前nonce
        const char* nonce = GetNonce();
        
        // 生成签名
        GenerateSignature(url, now_time, signature);
        
        // 是否需要加密
        if (need_encrypt && json_data) {
            // 初始化加密密钥
            InitKey(device_token, key);
            
            // 加密数据
            SM4_Encrypt(json_data, key, encrypted_data);
            
            // 构建请求
            snprintf(request_buffer, sizeof(request_buffer),
                    "POST %s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "Content-Type: application/x-www-form-urlencoded\r\n"
                    "Content-Length: %d\r\n"
                    "Connection: keep-alive\r\n\r\n"
//										"{\"data\":{\"now_sn=%s&nonce=%s&now_time=%u&signature=%s&data=%s\"}}",
										"now_sn=%s&nonce=%s&now_time=%u&signature=%s&data=%s",
                    url, SERVER_HOST, 
                    (int)(strlen("now_sn=") + strlen(device_sn) + 
                          strlen("&nonce=") + strlen(nonce) + 
                          strlen("&now_time=") + 10 + 
                          strlen("&signature=") + strlen(signature) + 
                          strlen("&data=") + strlen(encrypted_data)),
                    device_sn, nonce, now_time, signature, encrypted_data);
        } else {
            // 不加密或无数据
            snprintf(request_buffer, sizeof(request_buffer),
                    "POST %s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: %d\r\n"
                    "Connection: keep-alive\r\n\r\n"
                    "now_sn=%s&nonce=%s&now_time=%u&signature=%s%s%s",
                    url, SERVER_HOST, 
                    (int)(strlen("now_sn=") + strlen(device_sn) + 
                          strlen("&nonce=") + strlen(nonce) + 
                          strlen("&now_time=") + 10 + 
                          strlen("&signature=") + strlen(signature) + 
                          (json_data ? strlen("&data=") + strlen(json_data) : 0)),
                    device_sn, nonce, now_time, signature,
                    (json_data ? "&data=" : ""), 
                    (json_data ? json_data : ""));
        }
    } else {
        // 无需认证，直接发送数据
        if (json_data) {
            snprintf(request_buffer, sizeof(request_buffer),
                    "POST %s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: %d\r\n"
                    "Connection: keep-alive\r\n\r\n"
                    "%s",
                    url, SERVER_HOST, strlen(json_data), json_data);
        } else {
            snprintf(request_buffer, sizeof(request_buffer),
                    "POST %s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: 0\r\n"
                    "Connection: keep-alive\r\n\r\n",
                    url, SERVER_HOST);
        }
    }
    
    // 发送请求
    if (!ESP8266_SendData(request_buffer, strlen(request_buffer))) {
			SEGGER_RTT_printf(0, "ESP8266_CloseConnection\r\n");
        ESP8266_CloseConnection();
        return API_ERROR_SEND;
    }
    
    // 这里应该有接收响应的代码
    // 实际应用中，应该非阻塞方式接收数据
    // 这里简化处理，假设response_buffer中包含了完整响应
    strcpy((char*)response_buffer,(char*)wifi_uart_rx_buffer);
//		SEGGER_RTT_printf(0, "RxBuffer:%s\r\n,Respons:%s\r\n",wifi_uart_rx_buffer);
//		delay_ms(10000);
//		SEGGER_RTT_printf(0, "RxBuffer:%s\r\n,Respons:%s\r\n",wifi_uart_rx_buffer);
//		delay_ms(10000);
//		SEGGER_RTT_printf(0, "Respons:%s\r\n",response_buffer);
//		delay_ms(5000);
    // 如需解析响应
    if (response && response_type) {
			
        // 查找JSON开始位置
        char *json_start = strstr((char*)response_buffer, "{");
        if (json_start) {
//					SEGGER_RTT_printf(0, "ParseJsonResponse\r\n");
            // 如果是加密响应，需要先解密
            if (need_encrypt) {
                char decrypted_json[512];
                InitKey(device_token, key);
                SM4_Decrypt(json_start, key, decrypted_json);
                SEGGER_RTT_printf(0, "sn:%s,token:%s\r\n",device_sn,device_token);
                if (!ParseJsonResponse(decrypted_json, response, response_type)) {
//                    ESP8266_CloseConnection();
                    return API_ERROR_FORMAT;
                }
            } else {
                if (!ParseJsonResponse(json_start, response, response_type)) {
//                    ESP8266_CloseConnection();
                    return API_ERROR_FORMAT;
                }
            }
        } else {
//            ESP8266_CloseConnection();
            return API_ERROR_FORMAT;
        }
    }
    
    // 关闭连接
//    ESP8266_CloseConnection();
    return API_SUCCESS;
}

/**
 * 初始化API模块
 * @return 1成功，0失败
 */
uint8_t API_Init(void) {
    // 初始化随机数生成器
    srand((unsigned int)GetTimeStamp());
    
    // 初始化缓冲区
    memset(request_buffer, 0, sizeof(request_buffer));
    memset(response_buffer, 0, sizeof(response_buffer));
    
    // 初始化登录信息
    memset(device_sn, 0, sizeof(device_sn));
    memset(device_token, 0, sizeof(device_token));
    
    return 1;
}

/**
 * 设置设备登录信息
 * @param device_sn 设备SN
 * @param token 登录凭证
 */
void API_SetLoginInfo(const char *sn, const char *token) {
    strncpy(device_sn, sn, sizeof(device_sn) - 1);
    strncpy(device_token, token, sizeof(device_token) - 1);
}

/**
 * 1. 设备登录接口
 * @param sn 设备序列号
 * @param mac 设备MAC地址
 * @param response 响应数据(输出)
 * @return API_Status
 */
API_Status API_DeviceLogin(const char *sn, const char *mac, Device_Login_Response *response) {
    char json_data[256];
    
    if (!sn || !mac || !response) {
        return API_ERROR_PARAM;
    }
    
    // 构建JSON请求 - 更新为嵌套格式，添加data外层
    snprintf(json_data, sizeof(json_data), 
             "{\"data\":{\"sn\":\"%s\",\"mac\":\"%s\"}}", 
             sn, mac);
		
       // 连接服务器 - 增加对连接状态的检查
    if (!ESP8266_ConnectServer()) {
        // 检查是否已经连接了但函数返回失败
        if (esp8266_status == ESP8266_CONNECTED) {
            // 已经连接，可以继续
            SEGGER_RTT_printf(0,"ESP8266_CONNECTED\r\n");
        } else {
            // 真正的连接失败
						SEGGER_RTT_printf(0,"ESP8266_CONNECTED FAIL\r\n");
            return API_ERROR_CONNECT;
        }
		}
    // 发送请求，无需加密和认证
    return SendAPIRequest(API_DEVICE_LOGIN, json_data, ENCRYPTION_DISABLED, AUTH_NOT_REQUIRED, response, RESPONSE_TYPE_DEVICE_LOGIN);
}

/**
 * 2. 上传采集数据接口
 * @param heart_rate 心率
 * @param blood_oxygen 血氧
 * @param response 响应数据(输出)
 * @return API_Status
 */
API_Status API_UploadCollectData(uint8_t heart_rate, uint8_t blood_oxygen, Collect_Data_Response *response) {
    char json_data[256];
    
    if (!response) {
        return API_ERROR_PARAM;
    }
    
    // 构建JSON请求 - 更新为嵌套格式，添加data外层
    snprintf(json_data, sizeof(json_data), 
             "{\"data\":{\"xl\":%d,\"xy\":%d}}", 
             heart_rate, blood_oxygen);
    
    // 发送请求，需要加密和认证
    return SendAPIRequest(API_UPLOAD_COLLECT_DATA, json_data, ENCRYPTION_ENABLED, AUTH_REQUIRED, response, RESPONSE_TYPE_COLLECT_DATA);
}

/**
 * 3. 更新设备情景模式接口
 * @param scene_mode 情景模式
 * @return API_Status
 */
API_Status API_UpdateSceneMode(Scene_Mode scene_mode) {
    char json_data[128];
    
    // 构建JSON请求 - 更新为嵌套格式，添加data外层
    snprintf(json_data, sizeof(json_data), 
             "{\"data\":{\"qjms\":%d}}", 
             scene_mode);
    
    // 发送请求，需要加密和认证
    return SendAPIRequest(API_UPDATE_SCENE_MODE, json_data, ENCRYPTION_ENABLED, AUTH_REQUIRED, NULL, RESPONSE_TYPE_NONE);
}

/**
 * 4. 获取日历数据接口
 * @param time_stamp 输出参数，返回的时间戳
 * @return API_Status
 */
API_Status API_GetCalendarData(uint32_t *time_stamp) {
    if (!time_stamp) {
        return API_ERROR_PARAM;
    }
    
    // 发送请求，需要加密和认证
    return SendAPIRequest(API_GET_CALENDAR_DATA, NULL, ENCRYPTION_ENABLED, AUTH_REQUIRED, time_stamp, RESPONSE_TYPE_CALENDAR_DATA);
}

/**
 * 5. 获取指令接口
 * @param response 响应数据(输出)
 * @return API_Status
 */
API_Status API_GetInstructions(Get_Instructions_Response *response) {
    if (!response) {
        return API_ERROR_PARAM;
    }
    
    // 发送请求，需要加密和认证
    return SendAPIRequest(API_GET_INSTRUCTIONS, NULL, ENCRYPTION_ENABLED, AUTH_REQUIRED, response, RESPONSE_TYPE_INSTRUCTIONS);
}

/**
 * 6. 获取自主操作权限接口
 * @param response 响应数据(输出)
 * @return API_Status
 */
API_Status API_GetSelfOperationStatus(Get_Self_Op_Response *response) {
    if (!response) {
        return API_ERROR_PARAM;
    }
    
    // 发送请求，需要加密和认证
    return SendAPIRequest(API_GET_SELF_OP, NULL, ENCRYPTION_ENABLED, AUTH_REQUIRED, response, RESPONSE_TYPE_SELF_OP);
}

/**
 * 7. 上传设备位置信息接口
 * @param longitude 经度
 * @param latitude 纬度
 * @return API_Status
 */
API_Status API_UploadLocationInfo(float longitude, float latitude) {
    char json_data[256];
    
    // 构建JSON请求 - 更新为嵌套格式，添加data外层
    snprintf(json_data, sizeof(json_data), 
             "{\"data\":{\"lng\":%f,\"lat\":%f}}", 
             longitude, latitude);
    
    // 发送请求，需要加密和认证
    return SendAPIRequest(API_UPLOAD_LOCATION, json_data, ENCRYPTION_ENABLED, AUTH_REQUIRED, NULL, RESPONSE_TYPE_NONE);
}

/**
 * 8. 上传设备信息接口
 * @param network_status 设备联网状态
 * @param device_state 设备状态
 * @param battery_percent 剩余电量百分比
 * @param standing_info 其他信息JSON字符串
 * @return API_Status
 */
API_Status API_UploadDeviceInfo(Device_Network_Status network_status, 
                                Device_State device_state, 
                                uint8_t battery_percent, 
                                const char *standing_info) {
    char json_data[384];
    
    // 构建JSON请求 - 更新为嵌套格式，添加data外层
    snprintf(json_data, sizeof(json_data), 
             "{\"data\":{\"sblwzt\":%d,\"sbzt\":%d,\"sydl\":%d,\"standing\":\"%s\"}}", 
             network_status, device_state, battery_percent, 
             standing_info ? standing_info : "{}");
    
    // 发送请求，需要加密和认证
    return SendAPIRequest(API_UPLOAD_DEVICE_INFO, json_data, ENCRYPTION_ENABLED, AUTH_REQUIRED, NULL, RESPONSE_TYPE_NONE);
}

/**
 * 9. 设备登出接口
 * @return API_Status
 */
API_Status API_DeviceLogout(void) {
    // 发送请求，需要加密和认证，无参数，无响应数据
    return SendAPIRequest(API_DEVICE_LOGOUT, NULL, ENCRYPTION_ENABLED, AUTH_REQUIRED, NULL, RESPONSE_TYPE_NONE);
} 

