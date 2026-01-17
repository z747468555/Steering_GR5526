#ifndef __APP_SERVICE_SIMULATE_H__
#define __APP_SERVICE_SIMULATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osal.h"
#include <stdint.h>
#include <stdio.h>
#include "service_notice.h"

typedef enum
{
    EVENT_INVALID = -1,            // 无效通知

    EVENT_UPPER_LEFT_KEY_DBLCLICK, // 左上双击 SOS报警
    EVENT_UPPER_RIGHT_KEY_CLICK,   // 右上单击 返回表盘
    EVENT_SOS_ALARM_STOP,   		// 停止报警

    EVENT_ELETRICITY_CHANGE,       // 电量改变
    EVENT_CHARGER_CHANGE,          // 充电器状态改变

    EVENT_WATCHFACE_UPDATA,        // 表盘刷新
    EVENT_SPORT_CHANGE,            // 运动数据改变
    EVENT_HR_CHANGE,               // 心率发生改变
    EVENT_ENVIRONMENT_CHANGE,      // 环境数据改变

    EVENT_TEXT_LATEST,             // 最新文本通知上报
    EVENT_ALARM_CLOCK,             // 闹钟起闹

    EVENT_SYSTEM_SHUT_DOWN,        // 进入关机页面

    EVENT_ELECTRODE_TOUCH,         // 心电电极触摸
    EVENT_ELECTRODE_REMOVED,       // 心电电极非触摸
    EVENT_ELECTRODE_DATA,          // 心电电极信号上报
    EVENT_ELECTRODE_REPORT,        // 心电报告生成
    EVENT_TEXT_LATEST_EXT,         // 下拉通知

    EVENT_BODY_CHANGE,             // 身体数据改变
    EVENT_OTA,                     // OTA请求

} GUI_EVENT;

// 表盘更新时间 eventID：EVENT_WATCHFACE_UPDATA
typedef struct _app_watchface_t
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour; //默认24小时制

    uint8_t week;
    uint8_t day;
    uint8_t month;
    uint8_t year;
	uint8_t reserved;
} app_time_t;

bool app_watchface_get_current(app_time_t* time);

// 最新通知上报 eventID：EVENT_TEXT_LATEST
typedef struct _app_notice_t
{
    bool overdue;           // 通知有效性 1有效 0无效
    char* text;             // \0结尾
    uint16_t id;            // 唯一识别ID
    app_time_t notice_time; // 系统收到通知时间
} app_notice_t;

uint32_t app_notice_get_latest(app_notice_t* time);//返回值为未读通知数量，若为0表示没有
bool app_notice_set_overdue(uint16_t id);

// 提醒中心 获取所有文本通知的列表  单条删除和全部删除
typedef struct _app_notice_list_t
{
    app_notice_t current_notice;
    void *next;
} app_notice_list_t;

app_notice_list_t *app_notice_get_list(void);  //内存共享 返回值为list指针，若为NULL表示没有
bool app_notice_delete_all(void);
bool app_notice_delete(uint16_t id);

// 获取身体图表信息
typedef struct _app_body_chart_t
{
    uint8_t level;       // 当前等级 1 2 3
    uint16_t max;        // 最大值
    uint16_t min;        // 最小值
    uint8_t data[30];   // 一小时内有效点 如果有数据缺失用0补齐个数 Y:192->245
    app_time_t end_time; //图表结束时间
} app_body_chart_t;

bool app_body_get_emotion(app_body_chart_t* emotion);       // 情绪
bool app_body_get_vision(app_body_chart_t* vision);         // 视觉
bool app_body_get_mental(app_body_chart_t* mental);         // 脑力
bool app_body_get_energy(app_body_chart_t* energy);         // 精力
bool app_body_get_psychology(app_body_chart_t* psychology); // 心理

// 获取血氧心率数据
typedef struct _app_hr_bo_t
{
    uint8_t current_hr;  // 当前心率
    uint8_t current_bo;  // 当前血氧
    uint8_t max_hr;     // 一小时内心率最大值
    uint8_t min_hr;     // 一小时内心率最小值
    uint8_t data[30];   // 一小时内心率有效点 如果有数据缺失用0补齐个数 Y:156->224
    app_time_t end_time; //图表结束时间
} app_hr_bo_t;

bool app_hr_bo_get_chart(app_hr_bo_t* hr_bo); // 心理

typedef struct _app_cardiogram_t
{
    uint8_t avg_hr;				//0xff:无效
    uint8_t cardiogram_status; // 0: 未出现房颤 1：心动过缓 2：心动过速 4:无效
} app_cardiogram_t;

uint8_t app_heart_rate_get_current(void);
bool app_cardiogram_get_result(app_cardiogram_t* result);

// 开启关闭 SOS报警
void app_sos_set_status( bool status, bool isr); //true 开启 false 关闭
bool app_sos_get_status(void); //true 开启 false 关闭

// 运动数据获取
typedef struct _app_sport_t
{
    uint64_t step;
    uint64_t distance;
    uint64_t calorie;
} app_sport_t;

bool app_sport_get_data(app_sport_t* sport_data);

// 环境数据获取
typedef struct _app_environment_t
{
    uint16_t pressure;	//气压值
    float temperature;	//温度
    uint8_t visibility;	//光照强度
} app_environment_t;

bool app_environment_get_data(app_environment_t* environment_data);

// 开启关闭 连续测量
void app_continuity_set_status(bool status); // true 开启 false 关闭
bool app_continuity_get_status(void);        // true 开启 false 关闭

// 屏幕亮度调节 0-100%
uint8_t app_brightness_get_percent(void);
void app_brightness_set_percent(uint8_t percent);
void app_brightness_restore(void);

// 息屏时间设置 0为不息屏 单位秒
uint16_t app_resting_get_timeout(void);
void app_resting_set_timeout( uint16_t second);
void app_resting_reset(void);
void app_screen_keep_on(bool onoff);

// 闹钟功能 eventID：EVENT_ALARM_CLOCK
typedef struct _app_alarm_clock_t
{
    bool repeat;   // 0:单次 1：重复
    bool validity; // 闹钟生效

    uint8_t min;
    uint8_t hour; //默认24小时制
} app_alarm_clock_t;

typedef struct _app_alarm_clock_list_t
{
    uint8_t number; //当前注册个数
    app_alarm_clock_t list[3];
} app_alarm_clock_list_t;

bool gui_alarm_clock_get_list(app_alarm_clock_list_t* list);
void gui_alarm_clock_add(app_alarm_clock_t *alarm);
void gui_alarm_clock_delete(uint8_t id); // 数组下标 0 1 2

// 时间校准
void gui_calibration_set_time(uint8_t hour, uint8_t min, uint8_t sec);

// 身份码
typedef struct _app_id_code_t
{
    char* mac; // \0结尾
    char* tsm; // \0结尾
    char* nfc; // \0结尾
    char* sn;  // \0结尾
} app_id_code_t;

void gui_id_code_get_data(app_id_code_t* id);//information

typedef struct _app_version_t
{
    uint32_t mian;
    uint32_t sub;

} app_version_t;

uint8_t gui_version_get_validity(void);
bool gui_new_version_get_data(app_version_t* data);
bool gui_current_version_get_data(app_version_t* data);

// 关机
void gui_shutdown_launch(void);

//心电数据
#define MEASURE_POINTS_NUM 25
typedef struct
{
    uint8_t raw[MEASURE_POINTS_NUM];
    uint16_t length;
    uint8_t current_hr;
} health_measure_anim_t;

//获取心电数据
bool app_cardiogram_get_current(health_measure_anim_t* dat);

//获取充电状态	0:不在充电	1：在充电
uint8_t app_environment_get_charge_status(void);

//心电开关函数 true：打开心电,	false：关闭心电
void cardiogram_set(bool sta);

//升级进度获取
uint8_t gui_upgrade_process_get(void);

//开始执行升级
void service_upgrade_start(void);

#ifdef __cplusplus
}
#endif

#endif

