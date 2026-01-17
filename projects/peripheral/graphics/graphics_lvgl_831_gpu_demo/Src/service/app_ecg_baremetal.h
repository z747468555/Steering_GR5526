/**
 * @file    app_ecg_baremetal.h
 * @brief   GH3220 ECG模块裸机版接口定义
 * @note    该模块不依赖FreeRTOS和LVGL，可在裸机环境下运行
 */

#ifndef __APP_ECG_BAREMETAL_H__
#define __APP_ECG_BAREMETAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*
 * 常量定义
 */
#define ECG_MEASURE_POINTS_NUM      25      /* ECG数据点数 */
#define ECG_PRINT_EN_TIMEOUT_MS     3000    /* ECG打印使能延时(ms) */

/*
 * 回调函数类型定义
 */

/**
 * @brief ECG波形数据回调函数类型
 * @param data ECG波形数据数组
 * @param len  数据长度
 */
typedef void (*ecg_data_callback_t)(int16_t *data, uint32_t len);

/**
 * @brief ECG心率数据回调函数类型
 * @param hr 心率值 (bpm)
 */
typedef void (*ecg_hr_callback_t)(uint8_t hr);

/*
 * 初始化与轮询接口
 */

/**
 * @brief ECG模块初始化
 * @note  在main函数中调用一次，完成GH3220芯片初始化
 */
void app_ecg_init(void);

/**
 * @brief ECG模块轮询处理
 * @note  在主循环中周期性调用，处理中断和事件
 */
void app_ecg_poll(void);

/*
 * ECG控制接口
 */

/**
 * @brief 启动ECG测量
 */
void app_ecg_start(void);

/**
 * @brief 停止ECG测量
 */
void app_ecg_stop(void);

/*
 * 回调注册接口
 */

/**
 * @brief 注册ECG波形数据回调函数
 * @param cb 回调函数指针
 */
void app_ecg_register_data_callback(ecg_data_callback_t cb);

/**
 * @brief 注册ECG心率数据回调函数
 * @param cb 回调函数指针
 */
void app_ecg_register_hr_callback(ecg_hr_callback_t cb);

/*
 * 状态获取接口
 */

/**
 * @brief 获取当前ECG心率值
 * @return 心率值 (bpm), 0xff表示无效
 */
uint8_t app_ecg_get_hr(void);

/**
 * @brief 获取ECG是否正在运行
 * @return true: 正在运行, false: 已停止
 */
bool app_ecg_is_running(void);

/**
 * @brief 获取ECG打印使能状态
 * @return true: 已使能, false: 未使能
 */
bool app_ecg_print_enabled(void);

/*
 * 供驱动层调用的数据接收接口
 */

/**
 * @brief ECG波形数据接收 (由GH3220驱动hook调用)
 * @param data ECG波形数据数组
 * @param len  数据长度
 */
void app_ecg_data_received(int16_t *data, uint32_t len);

/**
 * @brief ECG心率数据接收 (由GH3220驱动hook调用)
 * @param hr 心率值 (bpm)
 */
void app_ecg_hr_received(uint8_t hr);

#ifdef __cplusplus
}
#endif

#endif /* __APP_ECG_BAREMETAL_H__ */


