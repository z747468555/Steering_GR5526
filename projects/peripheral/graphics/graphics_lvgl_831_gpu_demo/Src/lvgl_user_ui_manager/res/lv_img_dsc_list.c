#include "lvgl.h"
#include "gr55xx_hal.h"
#include "lv_img_dsc_list.h"
const lv_img_dsc_t  wd_img_applist_alarm_center_icon = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APPLIST_ALARM_CENTER_ICON),
    };
const lv_img_dsc_t  wd_img_applist_ecg_icon = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APPLIST_ECG_ICON),
    };
const lv_img_dsc_t  wd_img_applist_emotion_icon = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APPLIST_EMOTION_ICON),
    };
const lv_img_dsc_t  wd_img_applist_energy_icon = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APPLIST_ENERGY_ICON),
    };
const lv_img_dsc_t  wd_img_applist_environment_icon = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APPLIST_ENVIRONMENT_ICON),
    };
const lv_img_dsc_t  wd_img_applist_mental_icon = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APPLIST_MENTAL_ICON),
    };
const lv_img_dsc_t  wd_img_applist_physiological_icon = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APPLIST_PHYSIOLOGICAL_ICON),
    };
const lv_img_dsc_t  wd_img_applist_psychology_icon = {
    .header.always_zero = 0,
    .header.w = 76,
    .header.h = 75,
    .data_size = 22800, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APPLIST_PSYCHOLOGY_ICON),
    };
const lv_img_dsc_t  wd_img_applist_sos_icon = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APPLIST_SOS_ICON),
    };
const lv_img_dsc_t  wd_img_applist_sport_monitor_icon = {
    .header.always_zero = 0,
    .header.w = 76,
    .header.h = 75,
    .data_size = 22800, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APPLIST_SPORT_MONITOR_ICON),
    };
const lv_img_dsc_t  wd_img_applist_system_settings_icon = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APPLIST_SYSTEM_SETTINGS_ICON),
    };
const lv_img_dsc_t  wd_img_applist_vision_icon = {
    .header.always_zero = 0,
    .header.w = 76,
    .header.h = 75,
    .data_size = 22800, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APPLIST_VISION_ICON),
    };
const lv_img_dsc_t  wd_img_app_alarm_close = {
    .header.always_zero = 0,
    .header.w = 122,
    .header.h = 48,
    .data_size = 23424, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ALARM_CLOSE),
    };
const lv_img_dsc_t  wd_img_app_alarm_open = {
    .header.always_zero = 0,
    .header.w = 122,
    .header.h = 48,
    .data_size = 23424, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ALARM_OPEN),
    };
const lv_img_dsc_t  wd_img_app_alarm_SOS_icon = {
    .header.always_zero = 0,
    .header.w = 188,
    .header.h = 94,
    .data_size = 70688, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ALARM_SOS_ICON),
    };
const lv_img_dsc_t  wd_img_app_brightness_icon = {
    .header.always_zero = 0,
    .header.w = 50,
    .header.h = 50,
    .data_size = 10000, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_BRIGHTNESS_ICON),
    };
const lv_img_dsc_t  wd_img_app_clock_add = {
    .header.always_zero = 0,
    .header.w = 110,
    .header.h = 44,
    .data_size = 19360, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_ADD),
    };
const lv_img_dsc_t  wd_img_app_clock_alarm = {
    .header.always_zero = 0,
    .header.w = 70,
    .header.h = 70,
    .data_size = 19600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_ALARM),
    };
const lv_img_dsc_t  wd_img_app_clock_confirm = {
    .header.always_zero = 0,
    .header.w = 110,
    .header.h = 44,
    .data_size = 19360, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_CONFIRM),
    };
const lv_img_dsc_t  wd_img_app_clock_delete = {
    .header.always_zero = 0,
    .header.w = 110,
    .header.h = 44,
    .data_size = 19360, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_DELETE),
    };
const lv_img_dsc_t  wd_img_app_clock_noalarm = {
    .header.always_zero = 0,
    .header.w = 80,
    .header.h = 85,
    .data_size = 27200, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_NOALARM),
    };
const lv_img_dsc_t  wd_img_app_clock_pause = {
    .header.always_zero = 0,
    .header.w = 60,
    .header.h = 60,
    .data_size = 14400, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_PAUSE),
    };
const lv_img_dsc_t  wd_img_app_clock_play = {
    .header.always_zero = 0,
    .header.w = 60,
    .header.h = 60,
    .data_size = 14400, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_PLAY),
    };
const lv_img_dsc_t  wd_img_app_clock_plus = {
    .header.always_zero = 0,
    .header.w = 28,
    .header.h = 28,
    .data_size = 3136, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_PLUS),
    };
const lv_img_dsc_t  wd_img_app_clock_reduce = {
    .header.always_zero = 0,
    .header.w = 28,
    .header.h = 28,
    .data_size = 3136, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_REDUCE),
    };
const lv_img_dsc_t  wd_img_app_clock_reset = {
    .header.always_zero = 0,
    .header.w = 60,
    .header.h = 60,
    .data_size = 14400, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_RESET),
    };
const lv_img_dsc_t  wd_img_app_clock_selected = {
    .header.always_zero = 0,
    .header.w = 29,
    .header.h = 29,
    .data_size = 3364, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_SELECTED),
    };
const lv_img_dsc_t  wd_img_app_clock_stopwatch = {
    .header.always_zero = 0,
    .header.w = 70,
    .header.h = 70,
    .data_size = 19600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_STOPWATCH),
    };
const lv_img_dsc_t  wd_img_app_clock_time_calc = {
    .header.always_zero = 0,
    .header.w = 70,
    .header.h = 70,
    .data_size = 19600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_TIME_CALC),
    };
const lv_img_dsc_t  wd_img_app_clock_unselected = {
    .header.always_zero = 0,
    .header.w = 30,
    .header.h = 30,
    .data_size = 3600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CLOCK_UNSELECTED),
    };
const lv_img_dsc_t  wd_img_app_continue_closed = {
    .header.always_zero = 0,
    .header.w = 70,
    .header.h = 70,
    .data_size = 19600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CONTINUE_CLOSED),
    };
const lv_img_dsc_t  wd_img_app_continue_opened = {
    .header.always_zero = 0,
    .header.w = 70,
    .header.h = 70,
    .data_size = 19600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_CONTINUE_OPENED),
    };
const lv_img_dsc_t  wd_img_app_ecg_bg = {
    .header.always_zero = 0,
    .header.w = 360,
    .header.h = 360,
    .data_size = 518400, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ECG_BG),
    };
const lv_img_dsc_t  wd_img_app_ecg_confirm = {
    .header.always_zero = 0,
    .header.w = 110,
    .header.h = 44,
    .data_size = 19360, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ECG_CONFIRM),
    };
const lv_img_dsc_t  wd_img_app_ecg_exclamation = {
    .header.always_zero = 0,
    .header.w = 20,
    .header.h = 20,
    .data_size = 1600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ECG_EXCLAMATION),
    };
const lv_img_dsc_t  wd_img_app_ecg_gesture = {
    .header.always_zero = 0,
    .header.w = 360,
    .header.h = 360,
    .data_size = 518400, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ECG_GESTURE),
    };
const lv_img_dsc_t  wd_img_app_ecg_heart_24 = {
    .header.always_zero = 0,
    .header.w = 24,
    .header.h = 24,
    .data_size = 2304, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ECG_HEART_24),
    };
const lv_img_dsc_t  wd_img_app_ecg_heart_30 = {
    .header.always_zero = 0,
    .header.w = 30,
    .header.h = 30,
    .data_size = 3600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ECG_HEART_30),
    };
const lv_img_dsc_t  wd_img_app_ecg_understand = {
    .header.always_zero = 0,
    .header.w = 110,
    .header.h = 44,
    .data_size = 19360, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ECG_UNDERSTAND),
    };
const lv_img_dsc_t  wd_img_app_env_bg = {
    .header.always_zero = 0,
    .header.w = 360,
    .header.h = 360,
    .data_size = 518400, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ENV_BG),
    };
const lv_img_dsc_t  wd_img_app_env_brightness = {
    .header.always_zero = 0,
    .header.w = 40,
    .header.h = 40,
    .data_size = 6400, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ENV_BRIGHTNESS),
    };
const lv_img_dsc_t  wd_img_app_env_press = {
    .header.always_zero = 0,
    .header.w = 40,
    .header.h = 40,
    .data_size = 6400, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ENV_PRESS),
    };
const lv_img_dsc_t  wd_img_app_env_temp = {
    .header.always_zero = 0,
    .header.w = 40,
    .header.h = 40,
    .data_size = 6400, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ENV_TEMP),
    };
const lv_img_dsc_t  wd_img_app_id_qrcode = {
    .header.always_zero = 0,
    .header.w = 162,
    .header.h = 162,
    .data_size = 104976, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_ID_QRCODE),
    };
const lv_img_dsc_t  wd_img_app_msg_alarm_close = {
    .header.always_zero = 0,
    .header.w = 110,
    .header.h = 44,
    .data_size = 19360, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_MSG_ALARM_CLOSE),
    };
const lv_img_dsc_t  wd_img_app_msg_alarm_icon = {
    .header.always_zero = 0,
    .header.w = 50,
    .header.h = 50,
    .data_size = 10000, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_MSG_ALARM_ICON),
    };
const lv_img_dsc_t  wd_img_app_notice_noMsg = {
    .header.always_zero = 0,
    .header.w = 80,
    .header.h = 85,
    .data_size = 27200, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_NOTICE_NOMSG),
    };
const lv_img_dsc_t  wd_img_app_notice_received = {
    .header.always_zero = 0,
    .header.w = 110,
    .header.h = 44,
    .data_size = 19360, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_NOTICE_RECEIVED),
    };
const lv_img_dsc_t  wd_img_app_settings_alarm = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_SETTINGS_ALARM),
    };
const lv_img_dsc_t  wd_img_app_settings_brightness = {
    .header.always_zero = 0,
    .header.w = 76,
    .header.h = 75,
    .data_size = 22800, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_SETTINGS_BRIGHTNESS),
    };
const lv_img_dsc_t  wd_img_app_settings_IDCard = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_SETTINGS_IDCARD),
    };
const lv_img_dsc_t  wd_img_app_settings_screen_shutdowntime = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_SETTINGS_SCREEN_SHUTDOWNTIME),
    };
const lv_img_dsc_t  wd_img_app_settings_shutdown = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_SETTINGS_SHUTDOWN),
    };
const lv_img_dsc_t  wd_img_app_settings_sport_continuity = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_SETTINGS_SPORT_CONTINUITY),
    };
const lv_img_dsc_t  wd_img_app_settings_upgrade = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 74,
    .data_size = 21904, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_SETTINGS_UPGRADE),
    };
const lv_img_dsc_t  wd_img_app_shutdown_cancel = {
    .header.always_zero = 0,
    .header.w = 110,
    .header.h = 44,
    .data_size = 19360, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_SHUTDOWN_CANCEL),
    };
const lv_img_dsc_t  wd_img_app_sport_consume = {
    .header.always_zero = 0,
    .header.w = 70,
    .header.h = 70,
    .data_size = 19600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_SPORT_CONSUME),
    };
const lv_img_dsc_t  wd_img_app_sport_distances = {
    .header.always_zero = 0,
    .header.w = 70,
    .header.h = 70,
    .data_size = 19600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_SPORT_DISTANCES),
    };
const lv_img_dsc_t  wd_img_app_sport_steps = {
    .header.always_zero = 0,
    .header.w = 70,
    .header.h = 70,
    .data_size = 19600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_SPORT_STEPS),
    };
const lv_img_dsc_t  wd_img_app_upgrade_button = {
    .header.always_zero = 0,
    .header.w = 110,
    .header.h = 44,
    .data_size = 19360, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_UPGRADE_BUTTON),
    };
const lv_img_dsc_t  wd_img_app_upgrade_icon = {
    .header.always_zero = 0,
    .header.w = 48,
    .header.h = 48,
    .data_size = 9216, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_UPGRADE_ICON),
    };
const lv_img_dsc_t  wd_img_app_watchface_bg = {
    .header.always_zero = 0,
    .header.w = 360,
    .header.h = 360,
    .data_size = 518400, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_APP_WATCHFACE_BG),
    };
const lv_img_dsc_t  wd_img_chart_curves_lines = {
    .header.always_zero = 0,
    .header.w = 277,
    .header.h = 54,
    .data_size = 59832, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_CHART_CURVES_LINES),
    };
const lv_img_dsc_t  wd_img_chart_low_normal_high_lines = {
    .header.always_zero = 0,
    .header.w = 279,
    .header.h = 1,
    .data_size = 1116, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_CHART_LOW_NORMAL_HIGH_LINES),
    };
const lv_img_dsc_t  wd_img_chart_time_lines = {
    .header.always_zero = 0,
    .header.w = 277,
    .header.h = 7,
    .data_size = 7756, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_CHART_TIME_LINES),
    };
const lv_img_dsc_t  wd_img_close = {
    .header.always_zero = 0,
    .header.w = 110,
    .header.h = 44,
    .data_size = 19360, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_CLOSE),
    };
const lv_img_dsc_t  wd_img_downward = {
    .header.always_zero = 0,
    .header.w = 26,
    .header.h = 26,
    .data_size = 2704, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_DOWNWARD),
    };
const lv_img_dsc_t  wd_img_hr_icon = {
    .header.always_zero = 0,
    .header.w = 49,
    .header.h = 49,
    .data_size = 9604, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_HR_ICON),
    };
const lv_img_dsc_t  wd_img_hr_lines = {
    .header.always_zero = 0,
    .header.w = 256,
    .header.h = 69,
    .data_size = 70656, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_HR_LINES),
    };
const lv_img_dsc_t  wd_img_leftward = {
    .header.always_zero = 0,
    .header.w = 26,
    .header.h = 26,
    .data_size = 2704, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_LEFTWARD),
    };
const lv_img_dsc_t  wd_img_logo = {
    .header.always_zero = 0,
    .header.w = 180,
    .header.h = 180,
    .data_size = 129600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_LOGO),
    };
const lv_img_dsc_t  wd_img_logo_bayi = {
    .header.always_zero = 0,
    .header.w = 180,
    .header.h = 180,
    .data_size = 129600, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_LOGO_BAYI),
    };
const lv_img_dsc_t  wd_img_logo_star = {
    .header.always_zero = 0,
    .header.w = 254,
    .header.h = 254,
    .data_size = 258064, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_LOGO_STAR),
    };
const lv_img_dsc_t  wd_img_logo_triangle = {
    .header.always_zero = 0,
    .header.w = 13,
    .header.h = 11,
    .data_size = 572, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_LOGO_TRIANGLE),
    };
const lv_img_dsc_t  wd_img_open = {
    .header.always_zero = 0,
    .header.w = 110,
    .header.h = 44,
    .data_size = 19360, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_OPEN),
    };
const lv_img_dsc_t  wd_img_upward = {
    .header.always_zero = 0,
    .header.w = 26,
    .header.h = 26,
    .data_size = 2704, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_UPWARD),
    };
const lv_img_dsc_t  wd_img_wear_remind = {
    .header.always_zero = 0,
    .header.w = 360,
    .header.h = 360,
    .data_size = 518400, 
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = (uint8_t*)(QSPI0_XIP_BASE + ADDR_WEAR_REMIND),
    };
