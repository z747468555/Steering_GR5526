#ifndef __UI_WIN_DEF_H__
#define __UI_WIN_DEF_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "gh_demo_config.h"
#include "app_log.h"

#define COOKIE_LOG(...) //APP_LOG_INFO(_VAR_ARGS_)

#define LV_COLOR_WHITE          _LV_COLOR_MAKE_TYPE_HELPER LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)
#define LV_COLOR_BLUE           _LV_COLOR_MAKE_TYPE_HELPER LV_COLOR_MAKE(0x35, 0xD3, 0xFF)
#define LV_COLOR_GRAY           _LV_COLOR_MAKE_TYPE_HELPER LV_COLOR_MAKE(0x26, 0x26, 0x26)
#define LV_COLOR_BLACK          _LV_COLOR_MAKE_TYPE_HELPER LV_COLOR_MAKE(0x00, 0x00, 0x00)
#define LV_COLOR_RED            _LV_COLOR_MAKE_TYPE_HELPER LV_COLOR_MAKE(0xD4, 0x30, 0x30)
#define LV_COLOR_GREEN          _LV_COLOR_MAKE_TYPE_HELPER LV_COLOR_MAKE(0x38, 0xC9, 0x62)
#define LV_COLOR_GRAY_WHITE     _LV_COLOR_MAKE_TYPE_HELPER LV_COLOR_MAKE(0x99, 0x99, 0x99)
#define LV_COLOR_YELLOW         _LV_COLOR_MAKE_TYPE_HELPER LV_COLOR_MAKE(0xEE, 0xA8, 0x2F)

LV_FONT_DECLARE(font_12px);
LV_FONT_DECLARE(font_16px);
LV_FONT_DECLARE(font_18px);
LV_FONT_DECLARE(font_20px);
LV_FONT_DECLARE(font_24px);
LV_FONT_DECLARE(font_26px);
LV_FONT_DECLARE(font_30px);
LV_FONT_DECLARE(font_36px);
LV_FONT_DECLARE(font_48px);
LV_FONT_DECLARE(font_62px);
LV_FONT_DECLARE(font_72px);
LV_FONT_DECLARE(dync_20px_tt);

typedef enum
{
    SCREEN_ID_INVALID = -1, /// 默认无效窗体
    SCREEN_ID_TEMPLATE,     // test

    SCREEN_ID_LOGO,
    SCREEN_ID_WATCHFACE,
    SCREEN_ID_APPLIST,
    SCREEN_ID_MSG,          // 通知页面
    SCREEN_ID_BODY_CHANGE,  // 身体图表：1：情绪 2：视觉 3：脑力 4：精力 5：心理
    SCREEN_ID_HR_BO,        // 心率血氧
    SCREEN_ID_SOS,          // 报警
    SCREEN_ID_REMINDER,     // 提醒中心
    SCREEN_ID_CARDIOGRAM,   // 心电图
    SCREEN_ID_SPORT,        // 运动数据
    SCREEN_ID_ENVIRONMENT,  // 环境监测
    SCREEN_ID_SYSTEM_SET,   // 系统设置
    SCREEN_ID_CONTINUITY,   // 连续监测
    SCREEN_ID_BRIGHTNESS,   // 屏幕亮度
    SCREEN_ID_CLOCK_SET,    // 时钟设置
    SCREEN_ID_ALARM,        // 闹钟设置
    SCREEN_ID_TIME,         // 时间校准
    SCREEN_ID_STOPWATCH,    // 秒表设置
    SCREEN_ID_RESTING,      // 息屏时间
    SCREEN_ID_ID_CODE,      // 身份码
    SCREEN_ID_VERSION,      // 版本信息
    SCREEN_ID_SHUTDOWN,     // 关机
    SCREEN_ID_OTA,          // ota升级

    SCREEN_ID_DEBUG,        // debug

    SCREEN_ID_NUMBER
} screen_id_e;

typedef enum
{
    EMOTIONAL,
    VISUAL,
    MENTAL,
    ENERGY,
    PSYCHOLOGICAL,
} body_chart;

typedef enum
{
    SCREEN_LEVEL_WATCH_AROUND = 0,  // level 10
    SCREEN_LEVEL_APPLIST,           // level 9
    SCREEN_LEVEL_APP,               // level 8
    SCREEN_LEVEL_RESERVE1,          // level 7

    SCREEN_LEVEL_NOTICE_P3,         // level 6
    SCREEN_LEVEL_NOTICE_P2,         // level 5
    SCREEN_LEVEL_NOTICE_P1,         // level 4
    SCREEN_LEVEL_NOTICE_P0,         // level 3

    SCREEN_LEVEL_RESERVE2,      // level 2
    SCREEN_LEVEL_WATER,         // level 1
} creen_level_e;

#ifdef __cplusplus
}
#endif
#endif //__UI_WIN_DEF_H__
