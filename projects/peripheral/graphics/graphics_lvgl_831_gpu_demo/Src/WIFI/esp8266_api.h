/**
 * @file esp8266_api.h
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
 * <tr><td>2025-05-07 <td>1.0     <td>ZixuanHung     <td>ESP8266 API接口头文件
 * </table>
 */

#ifndef __ESP8266_API_H
#define __ESP8266_API_H

#include <stdint.h>

/**
 * 加密和认证状态定义
 */
#define ENCRYPTION_ENABLED   1  // 启用加密
#define ENCRYPTION_DISABLED  0  // 禁用加密
#define AUTH_REQUIRED        1  // 需要认证
#define AUTH_NOT_REQUIRED    0  // 不需要认证

/**
 * 响应类型定义
 */
#define RESPONSE_TYPE_NONE           0  // 无响应数据
#define RESPONSE_TYPE_DEVICE_LOGIN   1  // 设备登录响应
#define RESPONSE_TYPE_COLLECT_DATA   2  // 采集数据响应
#define RESPONSE_TYPE_CALENDAR_DATA  4  // 日历数据响应
#define RESPONSE_TYPE_INSTRUCTIONS   5  // 指令响应
#define RESPONSE_TYPE_SELF_OP        6  // 自主操作权限响应

// API路径定义
#define API_DEVICE_LOGIN         "/login/shebei"
#define API_UPLOAD_COLLECT_DATA  "/wristband/collectData"
#define API_UPDATE_SCENE_MODE    "/wristband/sbUpdQjms"
#define API_GET_CALENDAR_DATA    "/wristband/calendarData"
#define API_GET_INSTRUCTIONS     "/wristband/getInstructions"
#define API_GET_SELF_OP          "/wristband/getZzcz"
#define API_UPLOAD_LOCATION      "/wristband/locationInfo"
#define API_UPLOAD_DEVICE_INFO   "/wristband/identityInfo"
#define API_DEVICE_LOGOUT        "/wristband/logout"

/**
 * API接口返回状态
 */
typedef enum {
    API_SUCCESS = 0,        // 成功
    API_ERROR_CONNECT = 1,  // 连接失败
    API_ERROR_SEND = 2,     // 发送失败
    API_ERROR_TIMEOUT = 3,  // 超时
    API_ERROR_FORMAT = 4,   // 数据格式错误
    API_ERROR_PARAM = 5     // 参数错误
} API_Status;



/**
 * 情景模式
 */
typedef enum {
    SCENE_MODE_DAILY = 1,   // 日常模式
    SCENE_MODE_WORK = 2     // 作训模式
} Scene_Mode;

/**
 * 设备联网状态
 */
typedef enum {
    DEVICE_NETWORK_CONNECTED = 1,  // 已连接
    DEVICE_NETWORK_OFFLINE = 2     // 离线
} Device_Network_Status;

/**
 * 设备状态
 */
typedef enum {
    DEVICE_STATE_ON = 1,    // 开机
    DEVICE_STATE_OFF = 2,   // 关机
    DEVICE_STATE_ERROR = 3  // 异常
} Device_State;

/**
 * 自主操作权限
 */
typedef enum {
    SELF_OP_ENABLED = 1,    // 开启
    SELF_OP_DISABLED = 2    // 关闭
} Self_Operation_Status;

/**
 * 设备登录响应数据
 */
typedef struct {
    char sn[64];           // 设备在服务器的唯一标识
    char token[130];        // 登录凭证
} Device_Login_Response;

/**
 * 采集数据响应
 */
typedef struct {
    uint8_t stzk;          // 身体状态
    uint8_t zxqd;          // 作训强度
} Collect_Data_Response;

/**
 * 指令类型
 */
typedef enum {
    INSTRUCTION_CALL = 1,          // 呼叫
    INSTRUCTION_SCENE_CHANGE = 2,  // 情景切换 (参数1=日常，2=作训)
    INSTRUCTION_SHUTDOWN = 3,      // 关机
    INSTRUCTION_FACTORY_RESET = 4, // 恢复出厂设置
    INSTRUCTION_ALARM = 5,         // 警报提醒
    INSTRUCTION_GET_LOCATION = 6,  // 获取经纬度
    INSTRUCTION_GET_IDENTITY = 7   // 设备身份获取
} Instruction_Type;

/**
 * 指令结构体
 */
typedef struct {
    Instruction_Type type;  // 指令类型
    uint8_t param;          // 参数（可选）
} Instruction;

/**
 * 获取指令响应
 */
typedef struct {
    Instruction instructions[10];  // 指令数组
    uint8_t count;                 // 指令数量
} Get_Instructions_Response;

/**
 * 获取自主操作权限响应
 */
typedef struct {
    Self_Operation_Status zzcz;  // 自主操作权限状态
} Get_Self_Op_Response;

/**
 * 1. 设备登录接口
 * @param sn 设备序列号
 * @param mac 设备MAC地址
 * @param response 响应数据(输出)
 * @return API_Status
 */
API_Status API_DeviceLogin(const char *sn, const char *mac, Device_Login_Response *response);

/**
 * 2. 上传采集数据接口
 * @param heart_rate 心率
 * @param blood_oxygen 血氧
 * @param response 响应数据(输出)
 * @return API_Status
 */
API_Status API_UploadCollectData(uint8_t heart_rate, uint8_t blood_oxygen, Collect_Data_Response *response);

/**
 * 3. 更新设备情景模式接口
 * @param scene_mode 情景模式
 * @return API_Status
 */
API_Status API_UpdateSceneMode(Scene_Mode scene_mode);

/**
 * 4. 获取日历数据接口
 * @param time_stamp 输出参数，返回的时间戳
 * @return API_Status
 */
API_Status API_GetCalendarData(uint32_t *time_stamp);

/**
 * 5. 获取指令接口
 * @param response 响应数据(输出)
 * @return API_Status
 */
API_Status API_GetInstructions(Get_Instructions_Response *response);

/**
 * 6. 获取自主操作权限接口
 * @param response 响应数据(输出)
 * @return API_Status
 */
API_Status API_GetSelfOperationStatus(Get_Self_Op_Response *response);

/**
 * 7. 上传设备位置信息接口
 * @param longitude 经度
 * @param latitude 纬度
 * @return API_Status
 */
API_Status API_UploadLocationInfo(float longitude, float latitude);

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
                                const char *standing_info);

/**
 * 9. 设备登出接口
 * @return API_Status
 */
API_Status API_DeviceLogout(void);

/**
 * 初始化API模块
 * @return 1成功，0失败
 */
uint8_t API_Init(void);

/**
 * 设置设备登录信息(由API_DeviceLogin成功后调用)
 * @param device_sn 设备SN
 * @param token 登录凭证
 */
void API_SetLoginInfo(const char *device_sn, const char *token);

#endif /* __ESP8266_API_H */ 
