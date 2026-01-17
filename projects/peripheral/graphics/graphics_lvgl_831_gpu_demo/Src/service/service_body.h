#ifndef __SERVICE_BODY_H__
#define __SERVICE_BODY_H__
#include "app_service_simulate.h"

void app_body_init(void);

void app_body_set_psychology( uint8_t *dat, uint32_t len);

void app_body_set_energy( uint8_t *dat, uint32_t len);

void app_body_set_mental( uint8_t *dat, uint32_t len);

void app_body_set_vision( uint8_t *dat, uint32_t len);

void app_body_set_emotion( uint8_t *dat, uint32_t len);

#endif


