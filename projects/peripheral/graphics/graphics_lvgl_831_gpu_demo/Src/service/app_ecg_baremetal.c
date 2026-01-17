/**
 * @file    app_ecg_baremetal.c
 * @brief   GH3220 ECG模块裸机版实现
 * @note    该模块不依赖FreeRTOS和LVGL，可在裸机环境下运行
 */
#include "app_3220_task.h"
#include "gh_demo_config.h"
#include "gh_demo.h"
#include "gr55xx_hal.h"
#include "gr55xx_sys.h"
#include "app_ecg_baremetal.h"
#include "SEGGER_RTT.h"
#include <string.h>

/*
 * 宏定义
 */
#define LEAD_ON_OFF_FUNC    1   /* 导联检测功能开关 */

/*
 * 事件类型定义 (替代消息队列)
 */
#define ECG_EVT_NONE    0
#define ECG_EVT_START   1
#define ECG_EVT_STOP    2

/*
 * 静态变量
 */

/* 事件标志 (替代osal_queue) */
static volatile uint8_t s_ecg_pending_event = ECG_EVT_NONE;

/* 定时器变量 (替代FreeRTOS Timer) */
static uint32_t s_ecg_print_en_start_tick = 0;
static bool s_ecg_print_timer_active = false;
static bool s_ecg_print_en_flag = false;

/* 回调函数指针 */
static ecg_data_callback_t s_ecg_data_cb = NULL;
static ecg_hr_callback_t s_ecg_hr_cb = NULL;

/* 状态变量 */
static bool s_ecg_running = false;
static uint8_t s_ecg_hr = 0xff;

/* 外部中断标志 (由GH3220中断处理设置) */
extern uint8_t INT3220;

/* ECG波形数据缓冲区 */
int16_t g_ecg_data_buffer[ECG_MEASURE_POINTS_NUM];

uint32_t hal_get_tick(void)
{
	return  SysTick->VAL;

}

/*
 * 内部函数
 */

/**
 * @brief 检查ECG打印使能定时器
 */
static void check_ecg_print_timer(void)
{
    if (s_ecg_print_timer_active) {
        uint32_t current_tick = hal_get_tick();
        if ((current_tick - s_ecg_print_en_start_tick) >= ECG_PRINT_EN_TIMEOUT_MS) {
            s_ecg_print_en_flag = true;
            s_ecg_print_timer_active = false;
        }
    }
}

/**
 * @brief 启动ECG打印使能定时器
 */
static void start_ecg_print_timer(void)
{
    s_ecg_print_en_start_tick = hal_get_tick();
    s_ecg_print_timer_active = true;
    s_ecg_print_en_flag = false;
}

/**
 * @brief 处理ECG启动事件
 */
static void handle_ecg_start(void)
{
#if LEAD_ON_OFF_FUNC
    Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SPO2 | GH3X2X_FUNCTION_HR | GH3X2X_FUNCTION_ADT);
    Gh3x2xDemoStartSamplingWithCfgSwitch(GH3X2X_FUNCTION_LEAD_DET, 1);
#else
    Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_SPO2 | GH3X2X_FUNCTION_HR | GH3X2X_FUNCTION_ADT);
    Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_ECG);
    GH3X2X_FifoWatermarkThrConfig(150);
    start_ecg_print_timer();
#endif
    s_ecg_running = true;
}

/**
 * @brief 处理ECG停止事件
 */
static void handle_ecg_stop(void)
{
#if LEAD_ON_OFF_FUNC
    Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_ECG | GH3X2X_FUNCTION_LEAD_DET);
#else
    Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_ECG);
#endif
    Gh3x2xDemoStartSamplingWithCfgSwitch(GH3X2X_FUNCTION_ADT, 0);
    s_ecg_print_en_flag = false;
    s_ecg_print_timer_active = false;
    s_ecg_hr = 0xff;
    s_ecg_running = false;
}

/**
 * @brief 处理待处理事件
 */
static void process_pending_event(void)
{
    if (s_ecg_pending_event != ECG_EVT_NONE) {
        uint8_t evt = s_ecg_pending_event;
        s_ecg_pending_event = ECG_EVT_NONE;
        
        switch (evt) {
            case ECG_EVT_START:
                handle_ecg_start();
                break;
            case ECG_EVT_STOP:
                handle_ecg_stop();
                break;
            default:
                break;
        }
    }
}

/*
 * 公共接口实现
 */

void app_ecg_init(void)
{
    /* 初始化状态变量 */
    s_ecg_pending_event = ECG_EVT_NONE;
    s_ecg_print_timer_active = false;
    s_ecg_print_en_flag = false;
    s_ecg_running = false;
    s_ecg_hr = 0xff;
    s_ecg_data_cb = NULL;
    s_ecg_hr_cb = NULL;
    
    /* 调用GH3220驱动初始化 */
    Gh3x2xDemoInit();
    
    /* 启动ADT检测 */
    Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_ADT);
    
    /* 等待初始化完成 */
    delay_ms(1000);
}

void app_ecg_poll(void)
{
    /* 1. 处理GH3220中断 */
    if (INT3220) {
				SEGGER_RTT_printf(0,"GH3220 intrrupt\r\n");
        INT3220 = 0;
        Gh3x2xDemoInterruptProcess();
    }
    
    /* 2. 检查打印使能定时器 */
    check_ecg_print_timer();
    
    /* 3. 处理待处理事件 */
    process_pending_event();
}

void app_ecg_start(void)
{
    s_ecg_pending_event = ECG_EVT_START;
}

void app_ecg_stop(void)
{
    s_ecg_pending_event = ECG_EVT_STOP;
}

void app_ecg_register_data_callback(ecg_data_callback_t cb)
{
    s_ecg_data_cb = cb;
}

void app_ecg_register_hr_callback(ecg_hr_callback_t cb)
{
    s_ecg_hr_cb = cb;
}

uint8_t app_ecg_get_hr(void)
{
    return s_ecg_hr;
}

bool app_ecg_is_running(void)
{
    return s_ecg_running;
}

bool app_ecg_print_enabled(void)
{
    return s_ecg_print_en_flag;
}

/*
 * 供驱动层调用的数据接收接口
 */

void app_ecg_data_received(int16_t *data, uint32_t len)
{
    /* 复制数据到本地缓冲区 */
    if (len > ECG_MEASURE_POINTS_NUM) {
        len = ECG_MEASURE_POINTS_NUM;
    }
    memcpy(g_ecg_data_buffer, data, len * sizeof(int16_t));
    
    /* 调用用户回调 */
    if (s_ecg_data_cb != NULL) {
        s_ecg_data_cb(data, len);
    }
}

void app_ecg_hr_received(uint8_t hr)
{
    /* 心率值有效性过滤 */
    if ((hr >= 60) && (hr <= 100)) {
        if (s_ecg_hr != 0xff) {
            /* 平滑处理 */
            s_ecg_hr = (s_ecg_hr + hr) / 2;
        } else {
            s_ecg_hr = hr;
        }
    }
    
    /* 调用用户回调 */
    if (s_ecg_hr_cb != NULL) {
        s_ecg_hr_cb(s_ecg_hr);
    }
}


