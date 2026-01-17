#define APP_LOG_TAG "service"
#include "app_log.h"
#include "service_setting.h"
#include "app_service_simulate.h"
#include "app_rtc.h"
#include "app_sys_manager.h"
#include "app_service_task.h"
#include "app_service_simulate.h"
#include "ui_win_message.h"
#include "ui_win_manager.h"
#include "service_key.h"
#include "service_server.h"
#include "Motor.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "service_user_data.h"

#define	VERSOIN_NUM_MAIN	1

#define	VERSOIN_NUM_SUB		7

#define APP_BL_LEVEL_MAX			16

#define APP_BL_LEVEL_SET_MAX		12

#define APP_SETTING_CLOCK_ALARM_MAX	3


app_version_t app_version = { VERSOIN_NUM_MAIN, VERSOIN_NUM_SUB};

TimerHandle_t app_resting_timeout_timer_handle = NULL;
TimerHandle_t app_motor_ctrl_timer_handle = NULL;
uint16_t _g_app_resting_config = 10;
bool _g_screen_keep_on = false;
uint16_t app_resting_timeout = 0;

app_id_code_t app_setting_id_code;

static uint8_t _g_backlight = APP_BL_LEVEL_SET_MAX;//屏幕亮度

static bool service_sos_status = false;

static uint16_t device_sn = 123;
static char device_sn_c[8] = "test";
static char device_mac_c[20] = "test";//{0};
static char device_nfc_c[8] = "nfc";//{0};
static char device_tsm_c[8] = "tsm";//{0};

static bool service_continuous_detect_mode = true;	//连续监测模式


uint8_t device_qr_code[QR_CODE_LEN];

uint32_t vPortGetIPSR( void );

void service_set_qr_code( uint8_t *qr_code)
{
	memcpy( device_qr_code, qr_code, QR_CODE_LEN);
}

void service_get_qr_code( uint8_t *qr_code)
{
	memcpy( qr_code, device_qr_code, QR_CODE_LEN);
}

void service_setting_set_sn(uint16_t sn)
{
	device_sn = sn;
}

static uint32_t app_motor_to = 0;

void app_motor_ctrl_timr_callback(TimerHandle_t pxTimer)
{
	app_motor_to --;
	if(app_motor_to == 0)
	{
		Motor_Stop();
		service_sos_status = false;
		send_msg_to_ui_high( EVENT_SOS_ALARM_STOP, EVENT_SOS_ALARM_STOP);
		xTimerStop( app_motor_ctrl_timer_handle, portMAX_DELAY);
	}
}

void app_motor_shake( uint32_t delay, bool isr)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	app_motor_to = delay/100;
	Motor_Start();
	if(isr)
	{
		xTimerStartFromISR( app_motor_ctrl_timer_handle, &xHigherPriorityTaskWoken);
	}
	else
	{
		xTimerStart( app_motor_ctrl_timer_handle, portMAX_DELAY);
	}
}

void app_motor_stop(bool alarm,bool isr)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(alarm)
	{
		if(isr)
		{
			xTimerStopFromISR( app_motor_ctrl_timer_handle, &xHigherPriorityTaskWoken);
		}
		else
		{
			xTimerStop( app_motor_ctrl_timer_handle, portMAX_DELAY);
		}
		service_sos_status = false;
	}
	Motor_Stop();
}

void app_sos_set_status( bool status, bool isr)
{
    if(status)
	{
		send_msg_to_ui_high( EVENT_UPPER_LEFT_KEY_DBLCLICK, EVENT_UPPER_LEFT_KEY_DBLCLICK);
		app_motor_shake( MOTOR_SHAKE_LONG_DELAY, true);
		server_set_alarm();
		service_sos_status = true;
	}
	else
	{
		service_sos_status = false;
		app_motor_stop(true,isr);
	}
}

bool app_sos_get_status(void)
{
    return service_sos_status;
}

// 开启关闭 连续测量
void app_continuity_set_status(bool status)
{
    service_continuous_detect_mode = true;
}

bool app_continuity_get_status(void)
{
    return service_continuous_detect_mode;
}


#define BLIGHT_TYPE     APP_IO_TYPE_GPIOA       //����оƬ���ŷ���
#define BLIGHT_PIN      APP_IO_PIN_8            //����оƬ���ű�ţ�GPIO8��

#define Set_BL()      app_io_write_pin(BLIGHT_TYPE, BLIGHT_PIN, APP_IO_PIN_SET)
#define Reset_BL()    app_io_write_pin(BLIGHT_TYPE, BLIGHT_PIN, APP_IO_PIN_RESET)

static void _brightness_just(uint8_t step)
{
    Reset_BL();
    delay_us(50);
    for(int i = 1; i < step; i ++)
    {
        Set_BL();
        delay_us(200);
        Reset_BL();
        delay_us(50);
    }
    Set_BL();
}

// 获取屏幕亮度 1-16
uint8_t app_brightness_get_percent(void)
{
    return _g_backlight;
}


void app_brightness_set_percent(uint8_t num)
{
    uint8_t step;
    if(num > APP_BL_LEVEL_SET_MAX)
        num = APP_BL_LEVEL_SET_MAX;
    else if(num < 1)
        num = 1;
    if(num > _g_backlight)
    {
        step = APP_BL_LEVEL_MAX + _g_backlight - num;
    }
    else if(num < _g_backlight)
    {
        step = _g_backlight - num;
    }
    else
    {
    	return;
    }
    _brightness_just(step);
    APP_LOG_INFO("set backlight %d->%d step:%d\r\n", _g_backlight, num, step);
    _g_backlight = num;
}

void app_brightness_restore(void)
{
    uint8_t step = APP_BL_LEVEL_MAX - _g_backlight;
    if (step == 0)
    {
    	return;
    }
    _brightness_just(step);
}

void app_resting_timeout_timer_cb(TimerHandle_t pxTimer)
{
	APP_LOG_INFO("app_resting_timeout:%d \r\n", app_resting_timeout);
	if (_g_screen_keep_on)
	{
		app_resting_reset();
	}
    app_resting_timeout--;
    if(app_resting_timeout == 0)
    {
        //停止定时器
		xTimerStop( app_resting_timeout_timer_handle, portMAX_DELAY);

        //发送息屏事件
        //enter screen off mode
        sys_state_switch(SYS_STATE_SCREEN_OFF);
    }
}

// 获取息屏剩余时间
uint16_t app_resting_get_timeout(void)
{
    return app_resting_timeout;
}

void app_resting_reset(void)
{
	app_resting_timeout = _g_app_resting_config;
}

void app_screen_keep_on(bool onoff)
{
	_g_screen_keep_on = onoff;
}

void app_resting_start(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	app_resting_reset();
	if(vPortGetIPSR())
	{
		xTimerStopFromISR( app_resting_timeout_timer_handle, &xHigherPriorityTaskWoken);
		xTimerStartFromISR( app_resting_timeout_timer_handle, &xHigherPriorityTaskWoken);
	}
	else
	{
		xTimerStop( app_resting_timeout_timer_handle, portMAX_DELAY);
		xTimerStart( app_resting_timeout_timer_handle, portMAX_DELAY);
	}
}

void app_resting_set_timeout( uint16_t second)
{
	APP_LOG_INFO("app_resting_set_timeout:%d \r\n", app_resting_timeout);
	_g_app_resting_config = second;
	app_resting_timeout = second;
	app_resting_start();
}

app_alarm_clock_list_t *app_alarm_clock_list;

//���ӹ���
bool gui_alarm_clock_get_list(app_alarm_clock_list_t* list)
{
    *list = *app_alarm_clock_list;
    return true;
}

void gui_alarm_clock_add(app_alarm_clock_t *alarm)
{
    uint32_t i;

    //历史闹钟
    for(i=0;i<app_alarm_clock_list->number;i++)
    {
        if((app_alarm_clock_list->list[i].hour == alarm->hour)&&
            (app_alarm_clock_list->list[i].min == alarm->min))
        {
            //覆盖
            app_alarm_clock_list->list[i] = *alarm;
			//有效性强制为1
			app_alarm_clock_list->list[i].validity = 1;
			APP_LOG_INFO( "clarm cnt=%d", app_alarm_clock_list->number);
			for(uint32_t j=0;i<app_alarm_clock_list->number;j++)
			{
				APP_LOG_INFO( "clarm%d:hour=%d,min=%d,repeat=%d,valid=%d\r\n",
					j, app_alarm_clock_list->list[j].hour,  app_alarm_clock_list->list[j].min,
					app_alarm_clock_list->list[j].repeat, app_alarm_clock_list->list[j].validity);
			}

            return;
        }
    }

    //闹钟超过最大值
    if(app_alarm_clock_list->number >= APP_SETTING_CLOCK_ALARM_MAX)
    {
		APP_LOG_INFO( "clarm cnt=%d", app_alarm_clock_list->number);
		for(uint32_t j=0;i<app_alarm_clock_list->number;j++)
			{
				APP_LOG_INFO( "clarm%d:hour=%d,min=%d,repeat=%d,valid=%d\r\n",
					j, app_alarm_clock_list->list[j].hour,  app_alarm_clock_list->list[j].min,
					app_alarm_clock_list->list[j].repeat, app_alarm_clock_list->list[j].validity);
			}
        return;
    }
    //添加闹钟
    app_alarm_clock_list->list[app_alarm_clock_list->number] = *alarm;
	//有效性强制为1
	app_alarm_clock_list->list[app_alarm_clock_list->number].validity = 1;
    app_alarm_clock_list->number ++;
	APP_LOG_INFO( "clarm cnt=%d", app_alarm_clock_list->number);
	for(uint32_t j=0;j<app_alarm_clock_list->number;j++)
			{
				APP_LOG_INFO( "clarm%d:hour=%d,min=%d,repeat=%d,valid=%d\r\n",
					j, app_alarm_clock_list->list[j].hour,  app_alarm_clock_list->list[j].min,
					app_alarm_clock_list->list[j].repeat, app_alarm_clock_list->list[j].validity);
			}
}


void gui_alarm_clock_delete(uint8_t id)
{
    uint32_t i;

    //闹钟已满
    if(id+1 == app_alarm_clock_list->number)
    {
        //删除最后一个闹钟 TODO
        app_alarm_clock_list->number --;
    }
    else
    {
        //闹钟前移
        for(i=id;i<app_alarm_clock_list->number-1;i++)
        {
            app_alarm_clock_list->list[i] = app_alarm_clock_list->list[i+1];
        }
        app_alarm_clock_list->number --;
    }
}

//clock run
void clock_run_per_sec()
{
	static uint8_t current_min = 0;
    app_rtc_time_t current_time;
    uint32_t i;
    uint32_t interval;

    //秒更新到UI
    send_msg_to_ui_high( EVENT_WATCHFACE_UPDATA, EVENT_WATCHFACE_UPDATA);


    //下一个秒更新事件设置
    app_rtc_get_time(&current_time);
    interval = 1050 - current_time.ms;
	//interval = 60050 - current_time.sec - current_time.ms;
    app_rtc_setup_tick(interval);

	//判断整分钟
	if(current_min == current_time.min)
	{
		return;
	}
	current_min = current_time.min;
	APP_LOG_INFO( "alarm check:h=%d,m=%d",current_time.hour,current_time.min);

	//清步数
	if((current_time.hour == 0)&&(current_time.min == 0))
	{
		uint8_t send_data[APP_SERVICE_EVT_QUEUE_DATA_SIZE];
		send_data[0] = SERVICE_EVT_CLEAR_STEP;
		osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
	}

    //处理闹钟
    for(i=0;i<app_alarm_clock_list->number;i++)
    {
        if(app_alarm_clock_list->list[i].validity)
        {
            //时间匹配
            if((app_alarm_clock_list->list[i].hour == current_time.hour)&&
                (app_alarm_clock_list->list[i].min == current_time.min))
            {
				//发送震动提醒到service
				app_motor_shake( MOTOR_SHAKE_MIDIUM_DELAY, false);
                //发送闹钟消息到UI
                send_msg_to_ui_high( EVENT_ALARM_CLOCK, EVENT_ALARM_CLOCK);
                //删除闹钟
                if(app_alarm_clock_list->list[i].repeat == 0)
                {
                    app_alarm_clock_list->list[i].validity = 0;
                }
            }
        }
    }
}

void clock_init()
{
    app_rtc_time_t current_time;
    uint32_t interval;

	//set time
	current_time.date = 24;
	current_time.hour = 11;
	current_time.min = 1;
	current_time.mon = 5;
	current_time.ms = 0;
	current_time.sec = 0;
	current_time.week = 5;
	current_time.year = 14;

    //��ȡ��ǰʱ��
    app_rtc_get_time(&current_time);

    //interval = 60050 - current_time.sec - current_time.ms;
    interval = 1050 - current_time.ms;

    //
    app_rtc_setup_tick(interval);
}

bool app_watchface_get_current(app_time_t* time)
{
    app_rtc_time_t current_time = {0};
	app_rtc_get_time(&current_time);

    time->sec   = current_time.sec;
    time->min   = current_time.min;
    time->hour  = current_time.hour;
    time->week  = current_time.week;
    time->day   = current_time.date;
    time->month = current_time.mon;
    time->year  = current_time.year;

    return true;
}

void service_setting_renew_time(app_rtc_time_t *newtime)
{
	app_rtc_time_t current_time;
	uint32_t interval;

	app_rtc_init_time(newtime);

	//��ȡ��ǰʱ��
	app_rtc_get_time(&current_time);

	interval = 1050 - current_time.ms;;

	//
	app_rtc_setup_tick(interval);
}


// ʱ��У׼
void gui_calibration_set_time(uint8_t hour, uint8_t min, uint8_t sec)
{
	app_rtc_time_t setting_time;

	//get current time
	app_rtc_get_time(&setting_time);

	//
	setting_time.hour = hour;
	setting_time.min = min;
	setting_time.sec = sec;

	service_setting_renew_time(&setting_time);
}

void service_mac_update(uint8_t *mac)
{
	sprintf( device_mac_c, "%02x:%02x:%02x:%02x:%02x:%02x",
		*(mac+0),
		*(mac+1),
		*(mac+2),
		*(mac+3),
		*(mac+4),
		*(mac+5));
	sprintf( device_tsm_c, "%x%02x", (*(mac+4)&0xf), *(mac+5));
	APP_LOG_INFO("mac:%s,tsm:%s",device_mac_c,device_tsm_c);
}

void service_nfc_update( uint8_t nfc_h, uint8_t nfc_l)
{
	sprintf( device_nfc_c, "%02x%02x", nfc_h, nfc_l);
	APP_LOG_INFO("nfc:%s",device_nfc_c);
}

void gui_id_code_get_data(app_id_code_t* id)
{
	sprintf( device_sn_c, "%d", device_sn);
	app_setting_id_code.sn = device_sn_c;
	app_setting_id_code.mac = device_mac_c;
	app_setting_id_code.nfc = device_nfc_c;
	app_setting_id_code.tsm = device_tsm_c;
    *id = app_setting_id_code;
}

//下载中	1.版本有效	2.版本无效	3.升级中	4.安装失败
//
static uint8_t service_upgrade_state = 2;
static uint8_t service_upgrade_process = 0xff;
TimerHandle_t service_upgrade_timer_handle = NULL;

uint8_t gui_version_get_validity(void)
{
    return service_upgrade_state;
}

uint8_t gui_upgrade_process_get(void)
{
    return service_upgrade_process;
}

void service_upgrade_start()
{
	service_upgrade_process = 0;
	service_upgrade_state = 3;
	xTimerStart( service_upgrade_timer_handle, portMAX_DELAY);
}

void service_upgrade_cb(TimerHandle_t pxTimer)
{
	if(service_upgrade_process<100)
	{
		service_upgrade_process ++;
	}
	else
	{
		xTimerStop( service_upgrade_timer_handle, portMAX_DELAY);
		service_upgrade_state = 1;
	}
}

void service_upgrade_init()
{
	service_upgrade_timer_handle = xTimerCreate( "upgrade", 100, pdTRUE,  0, service_upgrade_cb);
}



bool gui_new_version_get_data(app_version_t* data)
{
	*data = app_version;
    return true;
}

bool gui_current_version_get_data(app_version_t* data)
{
	*data = app_version;
    return true;
}

void service_version_set( uint8_t ver_h, uint8_t ver_l)
{
	app_version.mian = ver_h;
	app_version.sub = ver_l;

	send_msg_to_ui_high( EVENT_OTA, EVENT_OTA);
}

void service_sys_reset()
{
	APP_LOG_INFO("System will restart\r\n");
    uint8_t send_data[APP_SERVICE_EVT_QUEUE_DATA_SIZE];
    send_data[0] = SERVICE_EVT_SYSRESET;
    osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
}

//
void gui_shutdown_launch(void)
{
	APP_LOG_INFO("System will shut down\r\n");
    uint8_t send_data[APP_SERVICE_EVT_QUEUE_DATA_SIZE];
    send_data[0] = SERVICE_EVT_SHUT_WITHOUT_RTC;
    osal_queue_send( app_service_evt_queue, send_data, OSAL_MAX_DELAY);
}


void app_setting_init(void)
{
	//升级
	service_upgrade_init();

    //自动息屏定时器
	app_resting_timeout_timer_handle = xTimerCreate( "rest_to", 1000, pdTRUE,  0, app_resting_timeout_timer_cb);
	app_resting_start();

	//电机定时器
	app_motor_ctrl_timer_handle = xTimerCreate( "motor", 100, pdTRUE,  0, app_motor_ctrl_timr_callback);

//    //初始化闹钟
//    app_alarm_clock_list->number = 0;

    //初始化时钟
    clock_init();

	//初始化电机
	Motor_Init();
}

void app_setting_data_init(bool sta)
{
	if(sta)
	{
		//初始化闹钟
		app_alarm_clock_list->number = 0;
	}
}

uint16_t service_setting_get_sn(void)
{
	return device_sn;
}


//设置背光亮度
void BackLight_Set(uint8_t num)
{
    // unsigned char step, i = 0;
    // if(num > 16)
    //     num = 16;
    // if(num < 1)
    //     num = 1;
    // if(num > _g_backlight)
    //     step = 16 + _g_backlight - num;
    // else
    //     step = _g_backlight - num;
    // Reset_BL();
    // delay_us(200);
    // for(i = 1; i < step; i ++){
    //     Set_BL();
    //     delay_us(500);
    //     Reset_BL();
    //     delay_us(200);
    // }
    // Set_BL();
    // delay_ms(4);
    // _g_backlight = num;
}

//存储设备信息
void service_data_save_dev_info(service_data_dev_info_t *payload)
{
	app_rtc_time_t current_time;
	payload->brightness = _g_backlight;
	payload->mode = service_continuous_detect_mode;
	memcpy( payload->qr_code, device_qr_code, QR_CODE_LEN);
	payload->resting_time = _g_app_resting_config;
	payload->sn = device_sn;
	app_rtc_get_time(&current_time);
	payload->sys_time = current_time;
	memcpy( payload->mac, device_mac_c, MAC_LEN);
	memcpy( payload->nfc, device_nfc_c, NFC_LEN);
	memcpy( payload->tsm, device_tsm_c, TSM_LEN);
}

void service_data_load_dev_info(service_data_dev_info_t *payload)
{
	_g_backlight = payload->brightness;
	if(_g_backlight>APP_BL_LEVEL_SET_MAX)
		_g_backlight = APP_BL_LEVEL_SET_MAX;
	else if(_g_backlight<1)
		_g_backlight = 1;
	app_brightness_restore();
	service_continuous_detect_mode = payload->mode;
	memcpy( device_qr_code, payload->qr_code, QR_CODE_LEN);
	_g_app_resting_config = payload->resting_time;
	device_sn = payload->sn;
	//time
	service_setting_renew_time(&payload->sys_time);
	//
	memcpy( device_mac_c, payload->mac, MAC_LEN);
	memcpy( device_nfc_c, payload->nfc, NFC_LEN);
	memcpy( device_tsm_c, payload->tsm, TSM_LEN);
}

//存储闹钟
void service_data_save_alarm_clock(service_data_alarm_clock_t *payload)
{

}

void service_data_load_alarm_clock(service_data_alarm_clock_t *payload)
{
	app_alarm_clock_list = &payload->alarm_clock;
	if(app_alarm_clock_list->number > APP_SETTING_CLOCK_ALARM_MAX)
	{
		app_alarm_clock_list->number = 0;
	}
}



