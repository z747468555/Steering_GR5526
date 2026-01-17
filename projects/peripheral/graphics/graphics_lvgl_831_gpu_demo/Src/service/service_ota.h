#ifndef __SERVICE_OTA_H__
#define __SERVICE_OTA_H__


#include "app_service_simulate.h"


#define SERVICE_OTA_PACK_MAX_LEN	128

void service_ota_unpack(uint8_t *pack);

void service_ota_write(void);

void service_ota_continue(void);

void service_ota_sleep(void);

void service_ota_restart(void);

#endif
