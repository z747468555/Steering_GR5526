#ifndef __SERVICE_REFORM_H__
#define __SERVICE_REFORM_H__
#include "app_service_simulate.h"

uint8_t service_get_max_8( uint8_t *dat, uint32_t len);

uint8_t service_get_max_8_without_ff( uint8_t *dat, uint32_t len);

uint8_t service_get_min_8( uint8_t *dat, uint32_t len);

uint16_t service_get_max_16( uint16_t *dat, uint32_t len);

void service_reform1( uint8_t *inbuf, uint8_t *outbuf, uint32_t  len);

void service_reform2( int16_t *inbuf, uint8_t *outbuf, uint32_t  len);

void service_reform3( uint8_t *inbuf, uint8_t *outbuf, uint32_t  len);

uint8_t avrg8_without_ff( uint8_t *dat, uint32_t len);

uint8_t service_min_8( uint8_t a, uint8_t b);

uint8_t service_max_8_without_ff( uint8_t a, uint8_t b);

#endif

