#include "service_user_data.h"
#include "qspi_norflash_v2.h"
#include "app_log.h"


void dump_print( uint8_t *buf, uint32_t len, char *title);
service_data_t service_data;

//uint8_t *flash_monitor = NULL;

#define FLASH_ADDR_SERVICE_DATA	0x00E04000

uint8_t check_sum( uint8_t *buf, uint32_t len)
{
	uint32_t i;
	uint8_t sum = 0;
	for(i=0;i<len;i++)
	{
		sum += *(buf+i);
	}
	return sum;
}

void flash_load_data( uint32_t addr, void *buf, uint32_t len)
{
//	memcpy( buf, flash_monitor, len);
	while(false == qspi_norf_read( addr, buf, len))
	{
		APP_LOG_INFO("%s.ret=false",__FUNCTION__);
		osal_task_delay_ms(100);
	}
	//dump_print( buf, len, "flash_load_data");
	
}


void flash_save_data( uint32_t addr, void *buf, uint32_t len)
{
//	if(flash_monitor == NULL)
//	{
//		flash_monitor = osal_heap_malloc(len);
//	}
//	
//	memcpy( flash_monitor, buf, len);
	while(false == qspi_norf_write( addr, buf, len))
	{
		APP_LOG_INFO("%s.ret=false",__FUNCTION__);
		osal_task_delay_ms(100);
	}
	//dump_print( buf, len, "flash_save_data");
}


#define PRINT_LEN	64


void dump_print( uint8_t *buf, uint32_t len, char *title)
{
	uint32_t index = 0;
	APP_LOG_INFO("%s",title);
	while(index<len)
	{
		if(index+PRINT_LEN<=len)
		{
			APP_LOG_HEX_DUMP( (buf+index), PRINT_LEN);
			index += PRINT_LEN;
			osal_task_delay_ms(10);
		}
		else
		{
			APP_LOG_HEX_DUMP( (buf+index), len-index);
			return;
		}
	}
}



void service_print_hr_bo(service_data_t *payload)
{
	APP_LOG_INFO("hr_bo print");
	dump_print( payload->hr_bo.hr_dat, APP_HR_BO_MAX_LEN, "hr_data");
	dump_print( payload->hr_bo.bo_dat, APP_HR_BO_MAX_LEN, "bo_data");
	APP_LOG_INFO("hr_min=%d",payload->hr_bo.hr_min);
	APP_LOG_INFO("hr_max=%d",payload->hr_bo.hr_max);
	dump_print( (uint8_t*)&payload->hr_bo.end_tim,sizeof(app_time_t), "endtime");
	APP_LOG_INFO("hr_bo print end");
}

void service_print_dev_info(service_data_t *payload)
{
	APP_LOG_INFO("dev_info print");
	APP_LOG_INFO("brightness=%d",payload->dev_info.brightness);
	APP_LOG_INFO("mode=%d",payload->dev_info.mode);
	dump_print( payload->dev_info.qr_code, QR_CODE_LEN, "qr_code");
	APP_LOG_INFO("rest time=%d",payload->dev_info.resting_time);
	APP_LOG_INFO("sn=%d",payload->dev_info.sn);
	APP_LOG_INFO("dev_info print end");
}

void service_print_sport(service_data_t *payload)
{
	APP_LOG_INFO("sport print");
	APP_LOG_INFO("step=%d",payload->sport.step);
	APP_LOG_INFO("sport print end");
}

void log_msg(void);
void service_print_msg(service_data_t *payload)
{
	uint32_t i;
	APP_LOG_INFO("msg print");
	APP_LOG_INFO("cnt=%d",payload->msg.cnt);
	for(i=0;i<payload->msg.cnt;i++)
	{
		APP_LOG_INFO("id=%d,time=%d-%d-%d %d:%d:%d %d, od=%d,text=%s",
			payload->msg.notice[i].id,
			payload->msg.notice[i].notice_time.year,
			payload->msg.notice[i].notice_time.month,
			payload->msg.notice[i].notice_time.day,
			payload->msg.notice[i].notice_time.hour,
			payload->msg.notice[i].notice_time.min,
			payload->msg.notice[i].notice_time.sec,
			payload->msg.notice[i].notice_time.week,
			payload->msg.notice[i].overdue,
			payload->msg.msg[i]);
	}
	
	
//	dump_print( (uint8_t*)(payload->msg.msg[0]), 20*64, "msg_payload");
//	dump_print( (uint8_t*)(payload->msg.notice), sizeof(app_notice_t)*20, "notice");
	APP_LOG_INFO("msg print end");
	
	//log_msg();
}

void service_print_alarm(service_data_t *payload)
{
	APP_LOG_INFO("alarm print");
	dump_print( (uint8_t*)(&payload->alarm_clock.alarm_clock), sizeof(app_alarm_clock_list_t), "alarm");
	APP_LOG_INFO("alarm print end");
}


//save all data
void service_data_save(void)
{
	uint32_t len;
	
	//�洢����Ѫ��
	service_data_save_hr_bo(&service_data.hr_bo);

	//�洢�豸��Ϣ
	service_data_save_dev_info(&service_data.dev_info);

	//�洢�Ʋ���Ϣ
	service_data_save_step(&service_data.sport);

	//�洢��Ϣ
	service_data_save_msg(&service_data.msg);

	//�洢����
	service_data_save_alarm_clock(&service_data.alarm_clock);
	
	//check sum
	len = sizeof(service_data_t) - 1;
	service_data.flash_mng.cks8 = check_sum(  (uint8_t*)&service_data, len);
	
	flash_save_data( FLASH_ADDR_SERVICE_DATA, &service_data, sizeof(service_data_t));
}

//load all data
void service_data_load(void)
{
	uint32_t len;
	flash_load_data( FLASH_ADDR_SERVICE_DATA, &service_data, sizeof(service_data_t));
	
	//check sum
	len = sizeof(service_data_t) - 1;
	if(service_data.flash_mng.cks8 == check_sum( (uint8_t*)&service_data, len))
	{
		//load data
		//�洢����Ѫ��
		service_data_load_hr_bo(&service_data.hr_bo);

		//�洢�豸��Ϣ
		service_data_load_dev_info(&service_data.dev_info);

		//�洢�Ʋ���Ϣ
		service_data_load_step(&service_data.sport);

		//�洢��Ϣ
		service_data_load_msg(&service_data.msg);

		//�洢����
		service_data_load_alarm_clock(&service_data.alarm_clock);
		
		//init data
		service_hr_bo_data_init(false);
		app_setting_data_init(false);
		
		APP_LOG_INFO("Service_data_load success!");
	}
	else
	{
		service_hr_bo_data_init(true);
		app_setting_data_init(true);
	}
}



//�����Բ�
void service_data_self_test()
{
	APP_LOG_INFO("%s.%d",__FUNCTION__,__LINE__);
	static uint32_t test_flag = 0;
	
	if(test_flag)
		return;
	test_flag ++;
	
	//save
	service_data_save();
	
	//print
	//dump_print( (uint8_t*)(&service_data), sizeof(service_data_t), "write buf");
	//hrbo
	//service_print_hr_bo(&service_data);
	//service_print_dev_info(&service_data);
	//service_print_sport(&service_data);
	//service_print_msg(&service_data);
	//service_print_alarm(&service_data);
	
	//read
	service_data_load();
	
	//print bak
	//dump_print( (uint8_t*)(&service_data_bak), sizeof(service_data_t), "read buf");
	//hr bo
	//service_print_hr_bo(&service_data_bak);
	//service_print_dev_info(&service_data_bak);
	//service_print_sport(&service_data_bak);
	//service_print_msg(&service_data_bak);
	//service_print_alarm(&service_data_bak);
	
	//log_msg();
}

//���ؾ���	TODO

