/*!
 * @file       application_gh3x2x.h
 * @brief  
 * @details 
 * @author     XiaoPb (lzh175603@163.com)
 * @date       2022-12-19
 * @version    v0.0.1
 * @copyright  Copyright (C) 2022 XiaoPb. All rights reserved.
 */

#ifndef __APPLICATION_GH3X2X_H__
#define __APPLICATION_GH3X2X_H__

#include <stdint.h>

#define HR_DATA_SAVE            (0)  ///< 0: disable 1: enable
#define HR_DATA_SIZE            (200)

#define HR_24H_DATA_SAVE        (1)  ///< 0: disable 1: enable
#define HR_24H_TIME_INTERVAL    (10) ///< minute
#define HR_24H_DATA_SIZE        ((24 * 60) / HR_24H_TIME_INTERVAL)

#define SPO2_DATA_SAVE          (0)  ///< 0: disable 1: enable
#define SPO2_DATA_SIZE          (200)

#define SPO2_24H_DATA_SAVE      (0)  ///< 0: disable 1: enable
#define SPO2_24H_TIME_INTERVAL  (10) ///< minute
#define SPO2_24H_DATA_SIZE      ((24 * 60) / SPO2_24H_TIME_INTERVAL)


typedef struct 
{
    uint8_t wear_state:4;
    uint8_t living_state:4;
} wear_state_t;

typedef struct 
{
    int16_t hba_out;
#if HR_DATA_SAVE
    int16_t old_hba_out[HR_DATA_SIZE];
#endif    
#if HR_24H_DATA_SAVE
    int16_t hba_out_24h[HR_24H_DATA_SIZE];
#endif    
} hr_algo_result_t;

typedef struct 
{
    int16_t spo2_out;
#if SPO2_DATA_SAVE
    int16_t old_spo2_out[SPO2_DATA_SIZE];
#endif    
#if SPO2_24H_DATA_SAVE
    int16_t spo2_out_24h[SPO2_24H_DATA_SIZE];
#endif    
} spo2_algo_result_t;

typedef struct 
{
    wear_state_t        wear_state;
    hr_algo_result_t    hr_algo_result;
    spo2_algo_result_t  spo2_algo_result;
}app_state_t;

#endif // !__APPLICATION_GH3X2X_H__

