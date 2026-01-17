#ifndef __UI_WIN_MESSAGE_H__
#define __UI_WIN_MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"
#include "ui_win_def.h"
#include "ui_win_manager.h"
#include "osal.h"

extern uint32_t g_user_event; ///< user define event
extern uint32_t widget_user_event;

extern osal_queue_handle_t g_mq_high_priority;      //message queue used by model timer, with high priority
extern osal_queue_handle_t g_mq_normal_priority;    //message queue used by model timer, with normal priority
extern osal_queue_handle_t g_mq_idle_update;        //message queue used when lcd off

#define MSG_QUEUE_NUMBER_GUI_CTRL           1
#define MSG_QUEUE_NUMBER_SYS                5
#define MSG_QUEUE_NUMBER_HIGH               5
#define MSG_QUEUE_NUMBER_NORMAL             5
#define MSG_QUEUE_NUMBER_IDLE_UPDATE        4

#define CONFIG_MODEL_TIME_INTERVAL          10


//普通等级队列
typedef enum
{
    EVT_UI_NORMAL_START = 0,            ///< PLACE HOLDER
    EVT_UI_NORMAL_TIME_UPDATED,         ///< gui event: time update,trigger by rtc wakeup event or another
    EVT_UI_NORMAL_SPORT_UPDATE,         ///< gui event: data of sport data update
    EVT_UI_NORMAL_SPORT_SCREEN_ON,
    EVT_UI_NORMAL_SPORT_HR_UPDATE,      ///< gui event：data of sport hr updated
    EVT_UI_NORMAL_ALARM_UPDATED,        ///< gui event: list of alarm data changed
    EVT_UI_NORMAL_STOPWATCH_UPDATED,    ///< gui event: data of stopwatch updated
    EVT_UI_NORMAL_COUNTDOWN_UPDATED,    ///< gui event: data of countdown updated
    EVT_UI_NORMAL_DAILYSPORT_UPDATED,   ///< gui event: data of dailyactivity updated
    EVT_UI_NORMAL_WATCHFACE_SEC_UPDATED,///< gui event: data of watchface updated
    //NOTE:需要新增UI消息上报，在此添加
    EVT_UI_NORMAL_PROJECT_START = 0x00FF,        ///< gui event: vendor event start
    //NOTE:需要新增vendor自定义消息上报，在此添加
    EVT_UI_NORMAL_END = 0x01FF,
} EVT_UI_NORMAL_E;

// 发送给UI的高优先级事件，此事件不可被丢弃，且会唤醒GUI线程
typedef enum
{
    EVT_UI_HIGH_START = EVT_UI_NORMAL_END,      ///< PLACE HOLDER
    EVT_UI_HIGH_LCD_ON,
    EVT_UI_HIGH_LCD_OFF,
    EVT_UI_HIGH_APP_FINDWATCH,                 ///找手表
    EVT_UI_HIGH_CHANGER_STATUS_UPDATED,        ///电量上报，防止亮屏第一帧和第二帧电量百分比发生跳变
    EVT_UI_HIGH_NOTICE_UPDATED,                ///有消息上来了，更新indicator
    EVT_UI_HIGH_PROJECT_START = 0x02FF,        ///<和上面一样，vendor需要新增的事件添加到此
    EVT_UI_HIGH_END = 0x03FF,
} EVT_UI_HIGH_E;

typedef enum
{
    EVT_SYSTEM_START = EVT_UI_HIGH_END,         ///< PLACE HOLDER
    EVT_SYSTEM_KEY_DOWN,                        //////////////////////////////
    EVT_SYSTEM_KEY_UP,                          //
    EVT_SYSTEM_KEY_LONG_PRESSED,                //   system event: key event
    EVT_SYSTEM_KEY_LONG_PRESSED_UP,             //
    EVT_SYSTEM_KEY_LONGLONG_PRESSED,            //   all evt param should be key code
    EVT_SYSTEM_KEY_LONGLONG_PRESSED_UP,         //
    EVT_SYSTEM_KEY_SINGLE_CLOCKED,              //
    EVT_SYSTEM_KEY_DOUBLE_CLICKED,              //
    EVT_SYSTEM_KEY_KEY_FIFTH_CLICKED,           //////////////////////////////
    EVT_SYSTEM_CHARGER_STATUS_CHANGED,          //充电状态发生变化 eg:充电->不充电
    EVT_SYSTEM_END = 0x04FF,
}EVT_SYSTEM_E;

typedef enum
{
    EVT_UI_IDLE_UPDATE_START = EVT_SYSTEM_END,  ///< PLACE HOLDER
    EVT_UI_IDLE_TRIGGER_ONLY,                   ///< 灭屏时唤醒一次gui线程， 类似当前sdk中的refr_now
    EVT_UI_IDLE_UPDATE_DATA,                    ///< 灭屏时，当前屏幕的内容有更新
    EVT_UI_IDLE_UPDATE_TP_OFF,                  ///< TP 休眠
    EVT_UI_IDLE_UPDATE_TP_ON,                   ///< 当有抬腕亮屏功能时，抬腕时接收到tp已经使能了的消息
    EVT_UI_IDLE_UPDATE_NOTICE,                  ///< 当有notication到来时，需要通知ui线程
    EVT_UI_IDLE_UPDATE_END,                     ///< END
} EVT_UI_IDLE_UPDATE_E;

typedef union
{
    uint32_t data;          ///< data,simple use
    struct
    {
        uint32_t id;        ///< data id,used for alone module
        uint32_t *pdata;    ///< data pointer,used for alone module
    }model;
} ser_event_data_t;

typedef struct event_msg
{
    uint16_t msg_type;      ///< evnet type of msg
    ser_event_data_t data;  ///< event data of msg
} ser_event_msg_t;


//亮屏后此次自动息屏时间  ---- handle by sdk 不一定会用到
//uint32_t ui_get_active_keep_time(void);

//message queue of lvgl creat
void ui_win_message_creat(void);

//monitor the message queue creat by ui_win_message_creat
void ui_win_message_monitor_start(void);

//timer handler,simulator the 'Model' of MVP.
void model_event_handler(lv_timer_t *t);

void send_msg_to_ui_normal(uint16_t id,uint32_t data);
void send_msg_to_ui_high(uint16_t id,uint32_t data);

#ifdef _cplusplus
}
#endif

#endif //__UI_WIN_MESSAGE_H__
