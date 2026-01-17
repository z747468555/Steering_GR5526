#include "service_reform.h"
#include "app_log.h"
#include "gus.h"


uint8_t service_min_8( uint8_t a, uint8_t b)
{
	if(a<b)
		return a;
	return b;
}

uint8_t service_max_8_without_ff( uint8_t a, uint8_t b)
{
	if(a==0xff)
		return b;
	else if(b == 0xff)
		return a;
	return ((a>b)?a:b);
	
}

uint8_t service_get_max_8( uint8_t *dat, uint32_t len)
{
	uint32_t i;
	uint8_t max = 0;
	for(i=0;i<len;i++)
	{
		if(dat[i]>max)
		{
			max = dat[i];
		}
	}
	return max;
}

uint8_t service_get_min_8( uint8_t *dat, uint32_t len)
{
	uint32_t i;
	uint8_t min = 255;
	for(i=0;i<len;i++)
	{
		if(dat[i]<min)
		{
			min = dat[i];
		}
	}
	return min;
}

uint16_t service_get_max_16( uint16_t *dat, uint32_t len)
{
	uint32_t i;
	uint16_t max = 0;
	for(i=0;i<len;i++)
	{
		if(dat[i]>max)
		{
			max = dat[i];
		}
	}
	return max;
}

uint16_t service_get_min_16_without_zero( uint16_t *dat, uint32_t len)
{
	uint32_t i;
	uint16_t min = 0xffff;
	for(i=0;i<len;i++)
	{
		if((dat[i]<min)&&(dat[i]!=0))
		{
			min = dat[i];
		}
	}
	if(min == 0xffff)
	{
		return 0;
	}
	return min;
}

//192-245(身体)
void service_reform1( uint8_t *inbuf, uint8_t *outbuf, uint32_t  len)
{
	const uint8_t low = 192;
	const uint8_t high = 245;
	uint32_t i;
	uint8_t max;
	uint8_t min;
	uint8_t delta = high-low;
	uint8_t sub;
	max = service_get_max_8( inbuf, len);
	min = service_get_min_8( inbuf, len);
	sub = max-min;
	
	for( i=0; i<len; i++)
	{
		*(outbuf+i) = high - (*(inbuf+i)-min)*delta/sub;
	}
}

//156-224(心率)
void service_reform3( uint8_t *inbuf, uint8_t *outbuf, uint32_t  len)
{
	//49-200
	const uint8_t low = 156;
	const uint8_t high = 224;
	uint32_t i;
	uint8_t max;
	uint8_t min;
	uint8_t delta = high-low;
	uint8_t sub;
	max = 200;
	min = 49;
	sub = max-min;
	
	for( i=0; i<len; i++)
	{
		if(*(inbuf+i) == 0xff)
		{
			*(outbuf+i) = 0xff;
		}
		else
		{
			*(outbuf+i) = high - (*(inbuf+i)-min)*delta/sub;
		}
	}
}

//0-120(心电)
void service_reform2( int16_t *inbuf, uint8_t *outbuf, uint32_t  len)
{
	#define VOLT_REF	40
	uint32_t i;
	uint16_t max = 0;
	static int16_t max_record = VOLT_REF;
	
	for(i=0;i<len;i++)
	{
		if(*(inbuf+i)>max_record)
		{
			*(outbuf+i) = 0;
		}
		else if(*(inbuf+i)<-max_record)
		{
			*(outbuf+i) = 120;
		}
		else
		{
			*(outbuf+i) = 60 - ((*(inbuf+i)*60))/max_record;
		}
	}
}

uint8_t avrg8_without_ff( uint8_t *dat, uint32_t len)
{
	uint32_t i;
	uint32_t sum=0;
	uint32_t valid_len=0;
	for(i=0;i<len;i++)
	{
		if(*(dat+i)!=0xff)
		{
			sum += *(dat+i);
			valid_len ++;
		}
	}
	if(valid_len == 0)
	{
		return 0xff;
	}
	return (sum/valid_len);
}

uint8_t service_get_max_8_without_ff( uint8_t *dat, uint32_t len)
{
	uint32_t i;
	uint8_t max = 0;
	for(i=0;i<len;i++)
	{
		if(dat[i] != 0xff)
		{
			if(dat[i]>max)
			{
				max = dat[i];
			}
		}
	}
	if(max == 0)
		return 0xff;
	return max;
}





