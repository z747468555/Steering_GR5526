#ifndef __SERVICE_SERVER_H__
#define __SERVICE_SERVER_H__
#include "app_log.h"

//输入接口

//手环参数结构体
typedef struct _wristband_parameters_t
{
	uint8_t hr;//心率
	uint8_t bo;//血氧
	uint16_t step;//步数
	uint32_t pres;//气压
	int16_t temp;//温度
	uint8_t light_level;//光照等级
	uint8_t power;//电量	
}wristband_parameters_t;

//手环时间结构体
typedef struct _wristband_time_t
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour; //默认24小时制
	uint8_t week;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} wristband_time_t;


//获取手环参数
void wristband_get_parameter(wristband_parameters_t *para);

//获取当前时间
void wristband_get_time(wristband_time_t *time);


/*
//心理数据结构体
typedef struct _wristband_psychology_t
{
	//具体有哪些，请填写进来	TODO
	//疲劳指数
	//专注指数
	//等
} wristband_psychology_t;

//写入心理数据
bool wristband_set_psychology(wristband_psychology_t *para);
*/

//写入消息(服务器下发)
bool wristband_set_msg( char *msg, uint32_t len);

//设置手环时间
bool wristband_set_time( wristband_time_t *time);

//设置SN号
bool wristband_set_sn( uint16_t sn);

//获取SN号
uint16_t wristband_get_sn(void);

//设置MAC, 13字节
bool wristband_set_mac( char * mac, uint8_t len);

//获取MAC， 13字节
bool wristband_get_mac(char * mac, uint8_t len);

//获取版本号

//30个字节,0-100,曲线能变化就行了
//写入脑力疲劳(服务器下发)
void wristband_set_mental( uint8_t *data, uint8_t len);

//写入精力(服务器下发)
void wristband_set_energy( uint8_t *data, uint8_t len);

//写入情绪(服务器下发)
void wristband_set_emotion( uint8_t *data, uint8_t len);

//写入心理压力(服务器下发)
void wristband_set_press( uint8_t *data, uint8_t len);

//写入视觉疲劳(服务器下发)
void wristband_set_fatigue( uint8_t *data, uint8_t len);

//设置二维码
void wristband_set_qrcode( uint8_t *qr_code, uint8_t len);

//通知服务器看完消息
void server_set_msg_overdue(void);

//向服务器发起报警
void server_set_alarm(void);

void service_server_task_create(void);
#endif

