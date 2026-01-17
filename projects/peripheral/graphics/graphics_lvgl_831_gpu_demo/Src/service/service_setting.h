#ifndef __SERVICE_SETTING_H__
#define __SERVICE_SETTING_H__
#include "app_io.h"
#include "app_rtc.h"

#define QR_CODE_LEN	16

#define MAC_LEN		20
#define NFC_LEN		8
#define TSM_LEN		8

void app_setting_init(void);

void app_setting_alarm_update(void);

void BackLight_Set(uint8_t num);

void clock_run_per_sec(void);

void service_setting_renew_time(app_rtc_time_t *newtime);

void service_setting_set_sn(uint16_t sn);

uint16_t service_setting_get_version(void);

void service_set_qr_code( uint8_t *qr_code);

uint16_t service_setting_get_sn(void);

void app_motor_shake( uint32_t delay, bool alarm);

void app_setting_data_init(bool sta);

void service_mac_update(uint8_t *mac);

void service_version_set( uint8_t ver_h, uint8_t ver_l);

void service_nfc_update( uint8_t nfc_h, uint8_t nfc_l);

void service_sys_reset(void);

#endif

