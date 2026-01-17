#include "service_notice.h"
#include "app_service_simulate.h"
#include "ui_win_message.h"
#include "ui_win_manager.h"
#include "app_rtc.h"
#include "service_server.h"
#include "service_user_data.h"
#include "FreeRTOS.h"
#include "Timers.h"

#define APP_NOTICE_TEXT_LEN_MAX			64

#define APP_NOTIFY_LEN					10

#define APP_NOTICE_LIST_MAX_LEN			20

#define APP_NOTICE_DELAY_TIME			300

TimerHandle_t app_notice_delay_timer_handle = NULL;

static app_notice_list_t *app_notice_list = NULL;
static uint32_t app_notice_list_len = 0;

static uint16_t app_notice_id_global = 0;


//通知管理
static app_notice_t app_notice_list_latest[APP_NOTIFY_LEN];
static uint32_t app_notice_latest_cnt = 0;

static void app_notify_update()
{
	uint32_t cnt = 0;
	app_notice_list_t *node = app_notice_list;
	
	while(node!=NULL)
	{
		if(node->current_notice.overdue == 1)
		{
			app_notice_list_latest[cnt] = node->current_notice;
			cnt ++;
			if(cnt>=APP_NOTIFY_LEN)
			{
				break;;
			}
		}
		node = node->next;
	}
	app_notice_latest_cnt = cnt;
	
//	//
//	cnt = 0;
//	node = app_notice_list;
//	APP_LOG_INFO("msg chain:");
//	while(node != NULL)
//	{
//		cnt ++;
//		APP_LOG_INFO(
//			"id=%d,time=%d-%d-%d %d:%d:%d %d, od=%d,text=%s",
//			node->current_notice.id,
//			node->current_notice.notice_time.year,
//			node->current_notice.notice_time.month,
//			node->current_notice.notice_time.day ,
//			node->current_notice.notice_time.hour,
//			node->current_notice.notice_time.min,
//			node->current_notice.notice_time.sec,
//			node->current_notice.notice_time.week,
//			node->current_notice.overdue,
//			node->current_notice.text); 
//		node = node->next;
//	}
//	APP_LOG_INFO("totol msg number:%d",cnt);
//	
//	//
//	APP_LOG_INFO("notify chain:");
//	
//	for(uint32_t i=0;i<app_notice_latest_cnt;i++)
//	{
//		APP_LOG_INFO(
//			"id=%d,time=%d-%d-%d %d:%d:%d %d, od=%d,text=%s",
//			app_notice_list_latest[i].id,
//			app_notice_list_latest[i].notice_time.year,
//			app_notice_list_latest[i].notice_time.month,
//			app_notice_list_latest[i].notice_time.day ,
//			app_notice_list_latest[i].notice_time.hour,
//			app_notice_list_latest[i].notice_time.min,
//			app_notice_list_latest[i].notice_time.sec,
//			app_notice_list_latest[i].notice_time.week,
//			app_notice_list_latest[i].overdue,
//			app_notice_list_latest[i].text); 
//	}
//	APP_LOG_INFO("totol notify number:%d",app_notice_latest_cnt);
}

void log_msg()
{
	uint32_t cnt;
	app_notice_list_t *node;
	cnt = 0;
	node = app_notice_list;
	APP_LOG_INFO("msg chain:");
	while(node != NULL)
	{
		cnt ++;
		APP_LOG_INFO(
			"id=%d,time=%d-%d-%d %d:%d:%d %d, od=%d,text=%s",
			node->current_notice.id,
			node->current_notice.notice_time.year,
			node->current_notice.notice_time.month,
			node->current_notice.notice_time.day ,
			node->current_notice.notice_time.hour,
			node->current_notice.notice_time.min,
			node->current_notice.notice_time.sec,
			node->current_notice.notice_time.week,
			node->current_notice.overdue,
			node->current_notice.text); 
		node = node->next;
	}
	APP_LOG_INFO("totol msg number:%d",cnt);
	
	APP_LOG_INFO("notify chain:");
	
	for(uint32_t i=0;i<app_notice_latest_cnt;i++)
	{
		APP_LOG_INFO(
			"id=%d,time=%d-%d-%d %d:%d:%d %d, od=%d,text=%s",
			app_notice_list_latest[i].id,
			app_notice_list_latest[i].notice_time.year,
			app_notice_list_latest[i].notice_time.month,
			app_notice_list_latest[i].notice_time.day ,
			app_notice_list_latest[i].notice_time.hour,
			app_notice_list_latest[i].notice_time.min,
			app_notice_list_latest[i].notice_time.sec,
			app_notice_list_latest[i].notice_time.week,
			app_notice_list_latest[i].overdue,
			app_notice_list_latest[i].text); 
	}
	APP_LOG_INFO("totol notify number:%d",app_notice_latest_cnt);
}

void app_notice_delay_timer_cb(TimerHandle_t pxTimer)
{
	send_msg_to_ui_high( EVENT_TEXT_LATEST, EVENT_TEXT_LATEST);
}

void app_notice_init()
{
	//
	app_notice_delay_timer_handle = xTimerCreate( "notice_delay", APP_NOTICE_DELAY_TIME, pdFALSE,  0, app_notice_delay_timer_cb);
	
}

void app_notice_delete_earliest(void)
{
	app_notice_list_t *node = app_notice_list;
	app_notice_list_t *node_before = NULL;
	
	if(node == NULL)
		return;
	if(node->next == NULL)
	{
		app_notice_list = NULL;
		//free
		osal_heap_free(node->current_notice.text);
		osal_heap_free(node);
		app_notice_list_len --;
		return;
	}
	
	//search the earliest one
	while(node->next!=NULL)
	{
		node_before = node;
		node = node->next;
	}
	
	//
	node_before->next = NULL;
	//free
	osal_heap_free(node->current_notice.text);
	osal_heap_free(node);
	
	app_notice_list_len --;

	return;
}

void app_notice_add( int8_t *msg, uint32_t len)
{
	APP_LOG_INFO("%s.%d",__FUNCTION__,__LINE__);
	app_rtc_time_t current_time;
	
	if(len>APP_NOTICE_TEXT_LEN_MAX)
	{
		APP_LOG_INFO("msg too long");
		return;
	}
	
	app_notice_list_t *node = osal_heap_malloc(sizeof(app_notice_list_t));
	
	if(node == NULL)
	{
		APP_LOG_INFO("memory is not enough");
		return;
	}
	
	char *ptext = osal_heap_malloc(len+1);
	
	if(ptext == NULL)
	{
		osal_heap_free(node);
		APP_LOG_INFO("memory is not enough");
		return;
	}
	
	if(app_notice_list_len>=APP_NOTICE_LIST_MAX_LEN)
	{
		APP_LOG_INFO("msg list is full,delete the earliest one");
		app_notice_delete_earliest();
	}
	
	//获取时间
	app_rtc_get_time(&current_time);
	node->current_notice.notice_time.day = current_time.date;
	node->current_notice.notice_time.hour = current_time.hour;
	node->current_notice.notice_time.min = current_time.min;
	node->current_notice.notice_time.month = current_time.mon;
	node->current_notice.notice_time.sec = current_time.sec;
	node->current_notice.notice_time.week = current_time.week;
	node->current_notice.notice_time.year = current_time.year;
	
	//
	node->current_notice.overdue = 1;
	
	//id
	node->current_notice.id = app_notice_id_global++;
	
	//msg copy
	memcpy( ptext, msg, len);
	ptext[len] = 0;
	node->current_notice.text = ptext;
	
	//add it into linklist
	if(app_notice_list == NULL)
	{
		node->next = NULL;
		app_notice_list = node;
	}
	else
	{
		node->next = app_notice_list;
		app_notice_list = node;
	}
	
	//消息数加一
	app_notice_list_len++;
	
	//update notify 
	app_notify_update();
	
	//new notice event
	//send_msg_to_ui_high( EVENT_TEXT_LATEST, EVENT_TEXT_LATEST);
	xTimerStart( app_notice_delay_timer_handle, portMAX_DELAY);
}

bool app_notice_delete(uint16_t id)
{
	APP_LOG_INFO("%s.%d",__FUNCTION__,__LINE__);
	app_notice_list_t *node = app_notice_list;
	app_notice_list_t *node_before = NULL;
	
	while(node!=NULL)
	{
		if(node->current_notice.id == id)
		{
			if(node_before == NULL)
			{
				//delete node
				app_notice_list = node->next;
				//free
				osal_heap_free(node->current_notice.text);
				osal_heap_free(node);
			}
			else
			{
				node_before->next = node->next;
				//free
				osal_heap_free(node->current_notice.text);
				osal_heap_free(node);
			}
			//
			app_notice_list_len --;
			//update notify 
			app_notify_update();
						
			return true;
		}
		node_before = node;
		node = node->next;
	}
	APP_LOG_INFO("no msg id = %d",id);
	return false;
}

bool app_notice_delete_all(void)
{
	APP_LOG_INFO("%s.%d",__FUNCTION__,__LINE__);
	app_notice_list_t *node = app_notice_list;
	app_notice_list_t *node_next;
	if(node == NULL)
	{
		APP_LOG_INFO("msg is already empty");
		return true;
	}
	node_next = node->next;
	
	while(node_next != NULL)
	{
		//free current node
		osal_heap_free(node->current_notice.text);
		osal_heap_free(node);
		
		//switch to next
		node = node_next;
		node_next = node->next;
	}
	
	//free current node
	osal_heap_free(node->current_notice.text);
	osal_heap_free(node);
	
	app_notice_list_len = 0;
	
	app_notice_list = NULL;
	
	app_notify_update();
	
	return true;
}

bool app_notice_set_overdue(uint16_t id)
{
	APP_LOG_INFO("%s.%d",__FUNCTION__,__LINE__);
	app_notice_list_t *node = app_notice_list;
	
	while(node!=NULL)
	{
		if(node->current_notice.id == id)
		{
			node->current_notice.overdue = 0;
			//update notify 
			app_notify_update();
			//消息已读完
			//if(0 == app_notice_latest_cnt)
			{
				server_set_msg_overdue();
			}
			
			return true;
		}
		node = node->next;
	}
	return false;
}

uint32_t app_notice_get_latest(app_notice_t* time)
{
	memcpy(time,&app_notice_list_latest[0],sizeof(app_notice_t));
	APP_LOG_INFO("latest get = %d", app_notice_latest_cnt);
	return app_notice_latest_cnt;
}

app_notice_list_t *app_notice_get_list(void)
{
	return app_notice_list;
}


//存储消息
void service_data_save_msg(service_data_msg_t *payload)
{
	uint32_t cnt = 0;
	app_notice_list_t *node = app_notice_list;
	while((NULL != node)&&(cnt<20))
	{
		payload->notice[cnt] = node->current_notice;
		memcpy( payload->msg[cnt], node->current_notice.text, 64);
		node = node->next;
		cnt ++;
	}
	payload->cnt = cnt;
}

void service_data_load_msg(service_data_msg_t *payload)
{
	uint32_t i = 0;
	app_notice_list_t *node;
	app_notice_list_t *node_cur;
	char *ptext;

	node_cur = app_notice_list;
	for(i=0;i<payload->cnt;i++)
	{
		node = osal_heap_malloc(sizeof(app_notice_list_t));
		if(node == NULL)
		{
			APP_LOG_INFO("memory is not enough");
			return;
		}
		ptext = osal_heap_malloc(64);
		if(ptext == NULL)
		{
			osal_heap_free(node);
			APP_LOG_INFO("memory is not enough");
			return;
		}
		
		if(NULL == node_cur)
		{
			app_notice_list = node;
			node_cur = node;
			node_cur->current_notice = payload->notice[i];
			node_cur->current_notice.id = app_notice_id_global;
			app_notice_id_global ++;
			node_cur->current_notice.text = ptext;
			memcpy( ptext, payload->msg[i], 64);
			node_cur->next = NULL;
		}
		else
		{
			node_cur->next = node;
			node_cur = node;
			node_cur->current_notice = payload->notice[i];
			node_cur->current_notice.id = app_notice_id_global;
			app_notice_id_global ++;
			node_cur->current_notice.text = ptext;
			memcpy( ptext, payload->msg[i], 64);
			node_cur->next = NULL;
		}
		app_notice_list_len ++;
	}
	app_notify_update();
}

