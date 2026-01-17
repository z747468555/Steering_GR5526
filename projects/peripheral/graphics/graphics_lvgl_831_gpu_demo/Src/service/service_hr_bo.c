#include "service_hr_bo.h"
#include "app_3220_task.h"
#include "app_rtc.h"
#include "ui_win_message.h"
#include "ui_win_manager.h"
#include "service_reform.h"
#include "app_3220_task.h"
#include "app_log.h"
#include "service_user_data.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "gus.h"


#define HR_TEMP_CNT						60
#define HR_SAMPLE_TICK_CNT				60
		
#define HR_HISTORY_CNT					20				//心率最值记录个数

#define HR_LIMIT_MIN					49				//心率血氧最大最小值
#define HR_LIMIT_MAX					200				
#define BO_LIMIT_MIN					70
#define BO_LIMIT_MAX					100		

#define CADIOGRAM_DATA_VALID_CNT		1000

app_cardiogram_t app_cardiogram;				//心电报告

uint8_t hr_data[APP_HR_BO_MAX_LEN];				//心率记录
uint8_t hr_current = 0xff;						//当前心率
uint8_t hr_bo_data_index = 0;					//心率血氧当前的采集坐标
uint8_t hr_min = 0xff;							//心率最大值
uint8_t hr_max = 0xff;							//心率最低值
static uint8_t hr_min_history[HR_HISTORY_CNT];	//心率最小值记录
static uint8_t hr_max_history[HR_HISTORY_CNT];	//心率最大值记录
static uint32_t hr_history_index = 0;			//心率最值记录序号


uint8_t bo_data[APP_HR_BO_MAX_LEN];				//血氧记录
uint8_t bo_current = 0xff;						//血氧当前值
uint8_t sample_cnt = 0;							//已经采了多少个点（不超过APP_HR_MAX_LEN）
app_time_t hr_end_time;							//最后一次采点时间

int16_t cardiogram_data[MEASURE_POINTS_NUM];	//心电数据
static uint32_t report_hr_sum = 0;
static uint32_t report_hr_cnt = 0;

//hrv 数据
uint16_t hrv_rri[APP_HRV_MAX_CNT] = {0};
uint16_t hrv_index = 0;

//
#define HR_DATA_RANDOM_LEN	7
#define BO_DATA_RANDOM_LEN	2
static bool hr_bo_valid = false;
const uint8_t hr_data_random[] = { 69, 70, 71, 72, 73, 74, 75};
const uint8_t bo_data_random[] = { 98, 99};

bool cardiogram_lean_on = false;

TimerHandle_t cardiogram_lean_on_state_remain_timer_handle = NULL;
TimerHandle_t hr_bo_valid_timer_handle = NULL;

uint32_t cardiogram_valid_data_cnt = 0;

void app_hr_bo_lean_on_set(bool sta)
{
	cardiogram_lean_on = sta;
	
}

bool app_hr_bo_lean_on_get(void)
{
	return cardiogram_lean_on;
}

void cardiogram_report_record(uint8_t hr)
{
	if(hr == 0xff)
		return;
	if(app_hr_bo_lean_on_get())
	{
		report_hr_sum+= hr;
		report_hr_cnt ++;
	}
	else
	{
		report_hr_sum = 0;
		report_hr_cnt = 0;
	}
}

void app_hr_bo_valid_timer_cb(TimerHandle_t pxTimer)
{
	hr_bo_valid = true;
}

bool app_hr_bo_valid_get()
{
	return hr_bo_valid;
}

void app_hr_bo_valid_set()
{
	xTimerStart( hr_bo_valid_timer_handle, portMAX_DELAY);
}

void app_hr_bo_valid_clear()
{
	xTimerStop( hr_bo_valid_timer_handle, portMAX_DELAY);
	hr_bo_valid = false;
}

void cardiogram_report_gen(void)
{
	APP_LOG_INFO("%s",__FUNCTION__);
	//生成报告
	if(cardiogram_valid_data_cnt<CADIOGRAM_DATA_VALID_CNT)
	{
		app_cardiogram.avg_hr = 0xff;
		app_cardiogram.cardiogram_status = 4;
	}
	else if(report_hr_cnt > 0)
	{
		app_cardiogram.avg_hr = report_hr_sum/report_hr_cnt;
		if(app_cardiogram.avg_hr<60)
		{
			app_cardiogram.cardiogram_status = 1;
		}
		else if(app_cardiogram.avg_hr>100)
		{
			app_cardiogram.cardiogram_status = 2;
		}
		else
		{
			app_cardiogram.cardiogram_status = 0;
		}
		//send_msg_to_ui_high(EVENT_ELECTRODE_REPORT,EVENT_ELECTRODE_REPORT);
	}
	else
	{
		app_cardiogram.avg_hr = 0xff;
		app_cardiogram.cardiogram_status = 4;
	}
	report_hr_cnt = 0;
	report_hr_sum = 0;
}

void app_hr_bo_update()
{
	static uint32_t tick = 1;
	uint8_t temp;
	
	//采集心率血氧
	temp = heart_rate_get();
	if((temp < HR_LIMIT_MIN)||(temp > HR_LIMIT_MAX))
	{
		if(app_hr_bo_valid_get())
		{
			temp = hr_data_random[tick%HR_DATA_RANDOM_LEN];
		}
		else
		{
			temp = 0xff;
		}
	}
	hr_min_history[hr_history_index] = service_min_8( temp, hr_min_history[hr_history_index]);
	hr_max_history[hr_history_index] = service_max_8_without_ff( temp, hr_max_history[hr_history_index]);
	hr_current = temp;
	
	temp = spo2_get();
	if((temp < BO_LIMIT_MIN)||(temp > BO_LIMIT_MAX))
	{
		if(app_hr_bo_valid_get())
		{
			temp = bo_data_random[tick%BO_DATA_RANDOM_LEN];
		}
		else
		{
			temp = 0xff;
		}
	}
	if(temp==100)
		temp = 99;//血氧不能出现100
	bo_current = temp;
		
	//APP_LOG_INFO("hr=%d,bo=%d",hr_current,bo_current);
	
	//历史记录切换
	if(0 == (tick%(HR_SAMPLE_CYCLE/HR_SAMPLE_DURATION/HR_HISTORY_CNT)))
	{
		hr_history_index = (hr_history_index+1)%HR_HISTORY_CNT;
	}
	
	//更新图表
	if(0 == (tick%(HR_SAMPLE_CYCLE/APP_HR_BO_MAX_LEN/2)))
	//if(0 == (tick%(1)))
	{
		app_rtc_time_t current_time;
		app_rtc_get_time(&current_time);
		hr_data[hr_bo_data_index] = hr_current;
		bo_data[hr_bo_data_index] = bo_current;
		hr_end_time.day = current_time.date;
		hr_end_time.hour = current_time.hour;
		hr_end_time.min = current_time.min;
		hr_end_time.month = current_time.mon;
		hr_end_time.sec = current_time.sec;
		hr_end_time.week = current_time.week;
		hr_end_time.year = current_time.year;
		hr_bo_data_index = (hr_bo_data_index+1)%APP_HR_BO_MAX_LEN;
		
		//采样计数加一
		if(sample_cnt<APP_HR_BO_MAX_LEN)
			sample_cnt ++;	
	}
	
	tick ++;
	
	//刷新UI
	send_msg_to_ui_high( EVENT_HR_CHANGE, EVENT_HR_CHANGE);
}

void app_cardiogram_updated( int16_t *inbuf, uint32_t len)
{
	uint32_t i;
	
	cardiogram_valid_data_cnt += 25;
	
	for(i=0;i<MEASURE_POINTS_NUM;i++)
	{
		cardiogram_data[i] = *(inbuf+i);
	}
	send_msg_to_ui_high( EVENT_ELECTRODE_DATA, EVENT_ELECTRODE_DATA);
}


bool app_hr_bo_get_chart(app_hr_bo_t* hr_bo)
{
	//
	uint8_t temp[APP_HR_BO_MAX_LEN];
	uint32_t i;
	
	//按照时间顺序排列
	for(i=0;i<APP_HR_BO_MAX_LEN;i++)
	{
		temp[APP_HR_BO_MAX_LEN-i-1] = hr_data[(hr_bo_data_index+i)%APP_HR_BO_MAX_LEN];
	}
	
	//坐标变换
	service_reform3( temp, hr_bo->data, APP_HR_BO_MAX_LEN);
	
	//最值
	hr_bo->min_hr = service_get_min_8( hr_min_history, HR_HISTORY_CNT);
	hr_bo->max_hr = service_get_max_8_without_ff( hr_max_history, HR_HISTORY_CNT);
	
	//时间
	hr_bo->end_time = hr_end_time;
	hr_bo->current_bo = bo_current;
	hr_bo->current_hr = hr_current;
	
	return true;
}


uint8_t app_heart_rate_get_current(void)
{
	return hr_current;
}

uint8_t app_bo_get_current(void)
{
	return bo_current;
}

void app_hrv_update( uint8_t num, uint32_t *rri)
{
	uint32_t i;
	for(i=0;i<num;i++)
	{
		if(hrv_index<APP_HRV_MAX_CNT)
		{
			hrv_rri[hrv_index] = *(rri+i);
			hrv_index ++;
		}
	}
}

void app_hrv_get( uint8_t *rri)
{	
	uint32_t i;
	//memcpy( rri, hrv_rri, hrv_index*2);
	for(i=0;i<hrv_index;i++)
	{
		rri[2*i] = hrv_rri[i]>>8;
		rri[2*i+1] = hrv_rri[i];
	}
	memset( hrv_rri, 0, sizeof(hrv_rri));
	hrv_index = 0;
}


bool app_cardiogram_get_result(app_cardiogram_t* result)
{
	*result = app_cardiogram;
	return true;
}

bool app_cardiogram_get_current(health_measure_anim_t* dat)
{
	service_reform2( cardiogram_data, dat->raw, MEASURE_POINTS_NUM);
	dat->length = MEASURE_POINTS_NUM;
	dat->current_hr = ecg_hr_get();
	return true;
}

void cardiogram_set(bool sta)
{
	if(sta)
	{
		cardiogram_valid_data_cnt = 0;
		ECG_Start();
		app_hr_bo_lean_on_set(true);
	}
	else
	{
		ECG_Stop();
		app_hr_bo_lean_on_set(false);
		
		//生成报告
		cardiogram_report_gen();
	}
}

void service_hr_bo_data_init(bool sta)
{
	if(sta)
	{
		memset( hr_data, 0xff, APP_HR_BO_MAX_LEN);
		memset( bo_data, 0xff, APP_HR_BO_MAX_LEN);	
	}
	memset( hr_min_history, 0xff, HR_HISTORY_CNT);
	memset( hr_max_history, 0xff, HR_HISTORY_CNT);
	//cardiogram_lean_on_state_remain_timer_handle = xTimerCreate( "cardiogram", 30000, pdFALSE,  0, cardiogram_lean_on_state_remain_timer_cb);
	
	hr_bo_valid_timer_handle = xTimerCreate( "hr_bo_valid", 5000, pdFALSE,  0, app_hr_bo_valid_timer_cb);
}


//存储心率血氧
void service_data_save_hr_bo( service_data_hr_bo_t *payload)
{
	memcpy( payload->bo_dat, bo_data, APP_HR_BO_MAX_LEN);
	payload->end_tim = hr_end_time;
	memcpy( payload->hr_dat, hr_data, APP_HR_BO_MAX_LEN);
	payload->hr_min = service_get_min_8( hr_min_history, HR_HISTORY_CNT);
	payload->hr_max = service_get_max_8_without_ff( hr_max_history, HR_HISTORY_CNT);
}

void service_data_load_hr_bo( service_data_hr_bo_t *payload)
{
	memcpy( bo_data, payload->bo_dat, APP_HR_BO_MAX_LEN);
	hr_end_time = payload->end_tim;
	memcpy( hr_data, payload->hr_dat, APP_HR_BO_MAX_LEN);
}
