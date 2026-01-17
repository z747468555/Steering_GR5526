#include "service_ota.h"
#include "service_setting.h"
#include "qspi_norflash_v2.h"
#include "app_log.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "app_graphics_mem.h"
#include "app_service_task.h"
#include "semphr.h"


#define FLASH_ADDR_OTA_INFO		0x00A04000
#define FLASH_ADDR_OTA			0x00A05000
#define OTA_PACK_SIZE			128
#define OTA_SECTER_SIZE			4096
#define OTA_FLASH_INFO_MAGIC	0x55aa7788


enum{
	OTA_CMD_DOWN_VERSION_GET 	= 0x01,		/* 获取版本号 */
	OTA_CMD_UP_VERSION_PUSH  	= 0x81,		/* 上传版本号 */
	OTA_CMD_DOWN_START 			= 0x02,		/* 触发OTA */
	OTA_CMD_UP_START_ACK 		= 0x82,		/* 触发OTA应答 */
	OTA_CMD_DOWN_INFO 			= 0x11,		/* 文件信息下发 */
	OTA_CMD_UP_INFO_ACK 		= 0x91,		/* 文件信息应答 */
	OTA_CMD_DOWN_PACK 			= 0x12,		/* 文件分包下发 */
	OTA_CMD_UP_PACK_ACK 		= 0x92,		/* 文件收包应答 */
	OTA_CMD_UP_CHECK 			= 0x94,		/* 校验结果上传 */
	OTA_CMD_UP_PACK_GET 		= 0x93,		/* 主动获取分包 */
	OTA_CMD_UP_CPML 			= 0xA1,		/* OTA完成上报 */
};

typedef struct{
	uint32_t magic;
	uint16_t size;
	uint16_t cur_index;
	uint8_t main;
	uint8_t sub;
	uint8_t type; 
}ota_flash_info_t;

typedef struct{
	uint8_t type;
	uint16_t size;
	uint8_t main;
	uint8_t sub;
}ota_info_t;

typedef struct{
	uint16_t cur_index;
	uint16_t total_index;
	uint32_t state;
	
}ota_ctrl_t;

void ota_data_send( uint8_t *buf, uint16_t len);
void service_ota_cmpl( uint8_t rslt);
void service_ota_check_result(uint8_t rslt);
void service_ota_pack_get(uint16_t pack_index);
void service_ota_info_save( bool sta, uint16_t cur_index);

TimerHandle_t service_ota_timer_handler = NULL;

uint8_t ota_send_buf[SERVICE_OTA_PACK_MAX_LEN];

ota_info_t service_ota_info;

ota_ctrl_t service_ota_ctrl = {0,0,0};

bool ota_is_start = 0;

uint8_t ota_data_buf_temp[OTA_PACK_SIZE+1];
uint8_t ota_data_buf[OTA_SECTER_SIZE+1];

SemaphoreHandle_t ota_write_mutex;


void service_ota_get_cb(TimerHandle_t pxTimer)
{
	service_ota_pack_get(service_ota_ctrl.cur_index);
}

/* OTA发送数据 */
void service_ota_send_data(uint8_t *buf,uint32_t len)
{
	APP_LOG_INFO("%s.%d",__FUNCTION__,__LINE__);
	ota_data_send( buf, len);
}

/* OTA start */
void service_ota_start( uint16_t size, uint16_t cur_index)
{
	service_ota_ctrl.state = true;
	service_ota_ctrl.cur_index = cur_index;
	service_ota_ctrl.total_index = size;
}

/* OTA stop */
void service_ota_stop(uint8_t *buf,uint32_t len)
{
	service_ota_ctrl.state = false;
}

/* OTA stop */
bool service_ota_state()
{
	return service_ota_ctrl.state;
}

/* OTA data save */
void service_ota_save( uint16_t index, uint8_t *dat)
{
	uint32_t addr;
	uint32_t base_ptr;
	
	APP_LOG_INFO("%s.%d",__FUNCTION__,__LINE__);
	
	base_ptr = (((uint32_t)index*OTA_PACK_SIZE)%OTA_SECTER_SIZE);
	APP_LOG_INFO("base_ptr=%08x,cur_index=%d",base_ptr,service_ota_ctrl.cur_index );
	
	memcpy( ota_data_buf+base_ptr, dat , OTA_PACK_SIZE);

	if((((base_ptr+OTA_PACK_SIZE)%OTA_SECTER_SIZE) == 0)||(service_ota_ctrl.cur_index == service_ota_ctrl.total_index - 1))
	{
		addr = FLASH_ADDR_OTA + OTA_PACK_SIZE*index-base_ptr;
		
		while(false == qspi_norf_write( addr, ota_data_buf, OTA_SECTER_SIZE))
		{
			APP_LOG_INFO("%s.ret=false,index=%d",__FUNCTION__,index);
			osal_task_delay_ms(100);
		}
		
		memset( ota_data_buf, 0xff , OTA_SECTER_SIZE);
		
		//read from flash
		while(false == qspi_norf_read( addr, ota_data_buf, OTA_SECTER_SIZE))
		{
			APP_LOG_INFO("read.%s.ret=false",__FUNCTION__);
			osal_task_delay_ms(100);
		}
		
		//hex dump
		APP_LOG_INFO("ota save buff");
		for(uint32_t i=0;i<32;i++)
		{
			APP_LOG_HEX_DUMP( ota_data_buf+i*OTA_PACK_SIZE, OTA_PACK_SIZE);
			osal_task_delay_ms(100);
		}
		
		//info update
		service_ota_info_save( false, service_ota_ctrl.cur_index);
	}
	
	APP_LOG_INFO("%s.ret=true,index=%d",__FUNCTION__,index);
}

/* OTA info save */
void service_ota_info_save( bool sta, uint16_t cur_index)
{
	ota_flash_info_t info;

	if(sta)
	{
		info.magic = OTA_FLASH_INFO_MAGIC;
	}
	else
	{
		info.magic = 0x00;
	}
	info.size = service_ota_info.size;
	info.cur_index = cur_index;
	info.main = service_ota_info.main;
	info.sub = service_ota_info.sub;
	info.type = service_ota_info.type;
	
	memset( ota_data_buf, 0xff, OTA_SECTER_SIZE);
	memcpy( ota_data_buf, (void*)(&info), sizeof(ota_flash_info_t));

	while(false == qspi_norf_write( FLASH_ADDR_OTA_INFO, ota_data_buf, OTA_SECTER_SIZE))
	{
		APP_LOG_INFO("%s.ret=false",__FUNCTION__);
		osal_task_delay_ms(100);
	}
	APP_LOG_INFO("%s.ret=true",__FUNCTION__);
}

void service_ota_write()
{
/* test */
#if 0 
	for(uint32_t i=0;i<OTA_PACK_SIZE;i++)
	{
		ota_data_buf_temp[i] = i;
	}
	service_ota_save( service_ota_ctrl.cur_index, ota_data_buf_temp);
	service_ota_ctrl.cur_index ++;
#else
	service_ota_save( service_ota_ctrl.cur_index, ota_data_buf_temp);
	xSemaphoreGive(ota_write_mutex);
#endif
}

/* OTA Firmware Integrity Check */
bool service_ota_FIC()
{
	uint8_t temp[OTA_PACK_SIZE];
	uint8_t xor_val = 0;
	uint32_t i;
	uint32_t j;
	uint32_t addr;
	for(i=0;i<service_ota_info.size;i++)
	{
		/* read data from flash */
		addr = FLASH_ADDR_OTA + OTA_PACK_SIZE*i;
		while(false == qspi_norf_read( addr, temp, OTA_PACK_SIZE))
		{
			APP_LOG_INFO("%s.ret=false",__FUNCTION__);
			osal_task_delay_ms(100);
		}
		
		/* XOR */
		for(j=0;j<OTA_PACK_SIZE;j++)
		{
			xor_val ^= temp[j];
		}
	}
	
	/* result */
	if(xor_val)
	{
		return false;
	}
	return true;
}

/* OTA run */
void service_ota_run()
{
	//uint8_t send_data[APP_SERVICE_EVT_QUEUE_DATA_SIZE];
	
	/* check */
	if(service_ota_FIC())
	{
		/* pass */
		service_ota_check_result(true);
	}
	else
	{
		/* error */
		service_ota_check_result(false);
		return;
	}
	
	/* run update */
	service_version_set( service_ota_info.main, service_ota_info.sub);
	
	/* cmpl */
	service_ota_cmpl(true);

	/* firmware writen */
	service_ota_info_save( true, 0);
	
//	/* sys reset */
//	osal_task_delay_ms(5000);
//	send_data[0] = SERVICE_EVT_SYSRESET;
//	osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
}


/* 上传版本号 */
void service_ota_upload_version()
{
	uint8_t idx = 0;
	
	APP_LOG_INFO("%s",__FUNCTION__);
	app_version_t cur_version;
	gui_current_version_get_data(&cur_version);
	
	/* cmd */
	ota_send_buf[idx++] = OTA_CMD_UP_VERSION_PUSH;
	/* main */
	ota_send_buf[idx++] = cur_version.mian;
	/* sub */
	ota_send_buf[idx++] = cur_version.sub;
	
	service_ota_send_data( ota_send_buf, idx);
}

/* 触发OTA应答 */
void service_ota_start_ack()
{
	uint8_t idx = 0;
	APP_LOG_INFO("%s",__FUNCTION__);
	
	/* cmd */
	ota_send_buf[idx++] = OTA_CMD_UP_START_ACK;
	
	service_ota_send_data( ota_send_buf, idx);
}


/* 文件信息应答 */	
void service_ota_info_ack(uint8_t ack)
{
	uint8_t idx = 0;
	APP_LOG_INFO("%s.ack=%d",__FUNCTION__,ack);
	
	/* cmd */
	ota_send_buf[idx++] = OTA_CMD_UP_INFO_ACK;
	/* ack : OK*/
	ota_send_buf[idx++] = ack;
	
	service_ota_send_data( ota_send_buf, idx);
}

/* 文件收包应答 */
void service_ota_pack_ack(uint16_t pack_index)
{
	uint8_t idx = 0;
	APP_LOG_INFO("%s.idx=%d",__FUNCTION__,pack_index);
	
	/* cmd */
	ota_send_buf[idx++] = OTA_CMD_UP_PACK_ACK;
	/* index */
	ota_send_buf[idx++] = pack_index>>8;
	ota_send_buf[idx++] = pack_index;
	
	service_ota_send_data( ota_send_buf, idx);
}

/* 校验结果上传 */
void service_ota_check_result(uint8_t rslt)
{
	uint8_t idx = 0;
	APP_LOG_INFO("%s.rslt=%d",__FUNCTION__,rslt);
	
	/* cmd */
	ota_send_buf[idx++] = OTA_CMD_UP_CHECK;
	/* index */
	ota_send_buf[idx++] = rslt;
	
	service_ota_send_data( ota_send_buf, idx);
}

/* 主动获取分包 */
void service_ota_pack_get(uint16_t pack_index)
{
	uint8_t idx = 0;
	APP_LOG_INFO("%s.idx=%d",__FUNCTION__,pack_index);
	/* cmd */
	ota_send_buf[idx++] = OTA_CMD_UP_PACK_GET;
	/* index */
	ota_send_buf[idx++] = pack_index>>8;
	ota_send_buf[idx++] = pack_index;
	
	service_ota_send_data( ota_send_buf, idx);
	
	
	xTimerStart( service_ota_timer_handler, portMAX_DELAY);
}

/* OTA完成上报 */
void service_ota_cmpl( uint8_t rslt)
{
	uint8_t idx = 0;
	app_version_t cur_version;
	APP_LOG_INFO("%s.rslt=%d",__FUNCTION__,rslt);
	
	gui_current_version_get_data(&cur_version);
	
	/* cmd */
	ota_send_buf[idx++] = OTA_CMD_UP_CPML;
	/* result */
	ota_send_buf[idx++] = rslt;
	/* index */
	ota_send_buf[idx++] = cur_version.mian;
	ota_send_buf[idx++] = cur_version.sub;
	
	service_ota_send_data( ota_send_buf, idx);
}

void service_ota_unpack(uint8_t *pack)
{
	uint8_t cmd;
	cmd = *pack;
	
	if(ota_is_start == 0)
		return ;
	
	switch(cmd)
	{
		/* 获取版本号 */
		case OTA_CMD_DOWN_VERSION_GET:
			APP_LOG_INFO("OTA_CMD_DOWN_VERSION_GET");
			service_ota_upload_version();
			break;
		/* 触发OTA */
		case OTA_CMD_DOWN_START:
			APP_LOG_INFO("OTA_CMD_DOWN_START");
			if(pdTRUE == xSemaphoreTake( ota_write_mutex, 10000))
			{
				APP_LOG_INFO("%s.ota_write_mutex=true",__FUNCTION__);
			}
			else
			{
				APP_LOG_INFO("%s.ota_write_mutex=false",__FUNCTION__);
			}
			service_ota_info_save( false, 0);
			service_ota_start_ack();
			break;	
		/* 文件信息下发 */
		case OTA_CMD_DOWN_INFO:
			
			APP_LOG_HEX_DUMP( pack+1, 16);
			/* type */
			service_ota_info.type = *(pack+1);
			/* size */
			service_ota_info.size = ((uint16_t)*(pack+2)<<8) + *(pack+3);
			/* main */
			service_ota_info.main = *(pack+4);
			/* sub */
			service_ota_info.sub = *(pack+5);
			APP_LOG_INFO("OTA_CMD_DOWN_INFO.size=%d",service_ota_info.size);
			
			service_ota_start( service_ota_info.size, 0);
			/* ack */
			service_ota_info_ack(true);
		
			/* ask for pack */
			service_ota_pack_get(service_ota_ctrl.cur_index);
			break;	
		/* 文件分包下发 */
		case OTA_CMD_DOWN_PACK:
			APP_LOG_INFO("OTA_CMD_DOWN_PACK");
			if(service_ota_state())
			{
				uint16_t index;
				uint8_t send_data[APP_SERVICE_EVT_QUEUE_DATA_SIZE];

				xTimerStop( service_ota_timer_handler, portMAX_DELAY);
				index = ((uint16_t)*(pack+1)<<8) + *(pack+2);
				APP_LOG_INFO("index=%d",index);
				if(index == service_ota_ctrl.cur_index)
				{
					//OTA save
					//service_ota_save( index, pack+3);
					memcpy( ota_data_buf_temp, pack+3, OTA_PACK_SIZE);
					APP_LOG_HEX_DUMP( ota_data_buf_temp, OTA_PACK_SIZE);
					
					send_data[0] = SERVICE_EVT_OTA_WRITE;
					osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
					
					//wait semaphor
					if(pdTRUE == xSemaphoreTake( ota_write_mutex, 10000))
					{
						APP_LOG_INFO("%s.ota_write_mutex=true",__FUNCTION__);
						//read&check
						
						
						//
						if(service_ota_ctrl.cur_index == service_ota_ctrl.total_index - 1)
						{
							/* 传输完成 */
							service_ota_run();
						}
						else
						{
							service_ota_ctrl.cur_index ++;
							service_ota_pack_get(service_ota_ctrl.cur_index);
						}
					}
					else
					{
						APP_LOG_INFO("%s.ota_write_mutex=false",__FUNCTION__);
						service_ota_pack_get(service_ota_ctrl.cur_index);
					}
				}
				else
				{
					service_ota_pack_get(service_ota_ctrl.cur_index);
				}
			}
			break;
		default:
			break;
	}
}


void service_ota_continue()
{
	ota_flash_info_t *info;
	APP_LOG_INFO("%s.%d",__FUNCTION__,__LINE__);
	
	//read info from flash
	while(false == qspi_norf_read( FLASH_ADDR_OTA_INFO, ota_data_buf, OTA_SECTER_SIZE))
	{
		APP_LOG_INFO("read.%s.ret=false",__FUNCTION__);
		osal_task_delay_ms(100);
	}
	info = (void*)ota_data_buf;
	APP_LOG_INFO("magic=%08x",info->magic);
	APP_LOG_INFO("index=%d",info->cur_index);
	
	if((info->magic != OTA_FLASH_INFO_MAGIC)&& ((info->cur_index+1)%32 == 0))
	{
		/* type */
		service_ota_info.type = info->type;
		/* size */
		service_ota_info.size = info->size;
		/* main */
		service_ota_info.main = info->main;
		/* sub */
		service_ota_info.sub = info->sub;
		
		APP_LOG_INFO("OTA_CONTINUE.size=%d",service_ota_info.size);
		
		service_ota_start( service_ota_info.size, info->cur_index + 1);
		
		xSemaphoreTake( ota_write_mutex, 3000);
	
		/* ask for pack */
		service_ota_pack_get(service_ota_ctrl.cur_index);
	}
}


void service_ota_init()
{
	ota_write_mutex = xSemaphoreCreateMutex();
	service_ota_timer_handler = xTimerCreate( "ota_to", 9000, pdFALSE,  0, service_ota_get_cb);
	memset( ota_data_buf, 0xff , OTA_SECTER_SIZE);
	ota_is_start = 1;
}

void service_ota_sleep()
{
	xTimerStop( service_ota_timer_handler, portMAX_DELAY);
}

void service_ota_restart()
{
	if(service_ota_ctrl.state)
	{
		xTimerStart( service_ota_timer_handler, portMAX_DELAY);
	}
}



