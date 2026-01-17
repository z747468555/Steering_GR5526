#ifndef __SERVICE_ENVIRONMENT_H__
#define __SERVICE_ENVIRONMENT_H__

#include "HP5804.h"
#include "nst112c-cwlr_drive.h"
#include "light.h"
#include "Charge.h"

void app_environment_update_pressure(void);

void app_environment_update_temp(void);

void app_environment_update_visibility(void);

void app_environment_update_power(void);

void app_encironment_init(void);

uint8_t app_environment_get_power(void);

#endif

