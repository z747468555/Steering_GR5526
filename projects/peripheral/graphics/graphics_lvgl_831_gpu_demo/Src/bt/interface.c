#include <stdbool.h>
#include <string.h>
#include "gr_soc.h"
#include "string.h"
#include "app_pwr_mgmt.h"
#include "app_drv_error.h"
#include "app_io.h"
#include "app_graphics_dc.h"
#include "stdarg.h"

#include "grx_hal.h"
#include "hal_gdc_mipi.h"
#include "hal_gdc_regs.h"
#include "hal_gdc.h"
#ifdef ENV_USE_FREERTOS
#ifdef USE_OSAL
#include "osal.h"
#else // USE_OSAL
#include "app_rtos_cfg.h"
#include "FreeRTOS.h"
#include "task.h"
#endif // USE_OSAL
#elif 1==CONFIG_ZEPHYR_OS
#include <zephyr/kernel.h>
#endif

#include "service_server.h"
#include "app_service_simulate.h"
#include "service_hr_bo.h"
#include "service_sport.h"
#include "service_environment.h"
#include "app_rtc.h"
#include "service_setting.h"
#include "service_body.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "time.h"
#include "app_log.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "app_sys_manager.h"
#include "service_ota.h"
#include "app_service_task.h"
//#include "mainProcess.h"
//#include "mcuQueue.h"

#define BT_INT_PIN                          APP_IO_PIN_5
#define BT_INT_TYPE                         APP_IO_TYPE_AON
#define BT_INT_MODE                         APP_IO_MODE_IT_RISING
#define BT_INT_PULL                         APP_IO_PULLDOWN

#define TSM_SEND_EN_PIN                     APP_IO_PIN_3
#define TSM_SEND_EN_TYPE                    APP_IO_TYPE_AON
#define TSM_SEND_EN_PULL                    APP_IO_PULLUP
#define TSM_SEND_EN_MODE                    APP_IO_MODE_OUTPUT
#define TSM_SEND_EN_MUX                     APP_IO_MUX_7

#define BT_EN_PIN                           APP_IO_PIN_12
#define BT_EN_TYPE                          APP_IO_TYPE_GPIOB
#define BT_EN_PULL                          APP_IO_PULLUP
#define BT_EN_MODE                          APP_IO_MODE_OUTPUT
#define BT_EN_MUX                           APP_IO_MUX_7
#define BT_EN_Set()                         app_io_write_pin(BT_EN_TYPE, BT_EN_PIN, APP_IO_PIN_SET)
#define BT_EN_Reset()                       app_io_write_pin(BT_EN_TYPE, BT_EN_PIN, APP_IO_PIN_RESET)

#define TSM_WUP_PIN                         APP_IO_PIN_1
#define TSM_WUP_TYPE                        APP_IO_TYPE_GPIOC
#define TSM_WUP_PULL                        APP_IO_PULLUP
#define TSM_WUP_MODE                        APP_IO_MODE_INPUT
#define TSM_WUP_MUX                         APP_IO_MUX_7
#define Read_TSM_WUP()                      app_io_read_pin(TSM_WUP_TYPE, TSM_WUP_PIN)    

#define UART_ID                             APP_UART_ID_3
#define UART_PIN_TYPE                       APP_IO_TYPE_MSIO
#define UART_TX_PIN                         APP_IO_PIN_1
#define UART_RX_PIN                         APP_IO_PIN_0
#define UART_PIN_MUX                        APP_IO_MUX_2


//服务器提供的接口
void resp_msg(void);
void sendCall(void);

bool uart_reset_flag = false;

bool uart_is_init = false;


void get_para(void)
{
	wristband_parameters_t para;
	
	wristband_get_parameter(&para);
	
}

void msg_set(uint8_t buf[], uint16_t len)
{
	wristband_set_msg((char*)buf, len);
}

void qr_set(uint8_t buf[], uint16_t len)
{
	wristband_set_qrcode(buf, len);
}

void sn_set(uint16_t sn)
{
	wristband_set_sn(sn);
}


//通知服务器看完消息
void server_set_msg_overdue(void)
{
	if(app_continuity_get_status())
	{
		resp_msg();
	}
}

//
void  set_nfc(uint8_t nfc_h, uint8_t nfc_l)
{
	service_nfc_update( nfc_h, nfc_l);
}

//设置版本号，显示V2.01, ver_h= 0x02, ver_l = 0x01
void set_ver(uint8_t ver_h, uint8_t ver_l)
{
	service_version_set( ver_h, ver_l);
}

/*
	  DATA [1字节]                       //发送日期
	  DATA [1字节]                       //小时
	  DATA [1字节]                       //分
	  DATA [1字节]                       //心率
	  DATA [1字节]                       //血氧
	  DATA [36字节]                      //18跳的心率间期，低字节在前，高字节在后
	  DATA [2字节]                       //步数，低字节在前，高字节在后
      DATA [2字节]                       //大气压，低字节在前，高字节在后
	  DATA [2字节]                       //手环温度，除以10为实际的温度, 低字节在前，高字节在后
	  DATA [1字节]                       //光照
	  DATA [1字节]                       //电量100,80,50,20,10
	  DATA [1字节]                       //版本高字节0x02
	  DATA [1字节]                       //版本低字节0x01   //显示V2.01
*/

void set_muti_para(uint8_t buf[], uint8_t len)
{
	app_rtc_time_t current_time;
	app_sport_t sport;
	app_environment_t env;
	uint16_t tem;
	uint8_t power;
	app_version_t version;
	
	app_rtc_get_time(&current_time);
//	DATA [1字节]                       //发送日期
	buf[0] = current_time.date;
//	DATA [1字节]                       //小时
	buf[1] = current_time.hour;
//	DATA [1字节]                       //分
	buf[2] = current_time.min;
//	DATA [1字节]                       //心率
	buf[3] = app_heart_rate_get_current();
//	DATA [1字节]                       //血氧
	buf[4] = app_bo_get_current();
//	DATA [36字节]                      //18跳的心率间期，低字节在前，高字节在后
	app_hrv_get(buf+5);
//	DATA [2字节]                       //步数，低字节在前，高字节在后
	app_sport_get_data(&sport);
	buf[41] = sport.step&0xff;
	buf[42] = (sport.step>>8)&0xff;
//	DATA [2字节]                       //大气压，低字节在前，高字节在后
	app_environment_get_data(&env);
	buf[43] = env.pressure&0xff;
	buf[44] = (env.pressure>>8)&0xff;
	buf[45] = 0;
	buf[46] = 0;
//	DATA [2字节]                       //手环温度，除以10为实际的温度, 低字节在前，高字节在后
	tem =  (uint16_t)(env.temperature*10);
	buf[47] = tem&0xff;
	buf[48] = (tem>>8)&0xff;
//	DATA [1字节]                       //光照
	buf[49] = env.visibility;
//	DATA [1字节]                       //电量100,80,50,20,10
	power = app_environment_get_power();
	if(power>95)
	{
		buf[50] = 100;
	}
	else if(power>80)
	{
		buf[50] = 80;
	}
	else if(power>50)
	{
		buf[50] = 50;
	}
	else if(power>20)
	{
		buf[50] = 20;
	}
	else
	{
		buf[50] = 10;
	}
//	DATA [1字节]                       //版本高字节0x02
	gui_current_version_get_data(&version);
	buf[51] = version.mian;
//	DATA [1字节]                       //版本低字节0x01   //显示V2.01
	buf[52] = version.sub;
}

//向服务器发起报警
void server_set_alarm(void)
{
	if(app_continuity_get_status())
	{
		sendCall();
	}
}

//收到信息时弹出信息显示	没有这个函数
void jump_to_msg(void)
{
	//使用 	wristband_set_msg( &msg, len); 但是需要传入信息的内容和长度
}

//收到设置二维码时，发送二维码刷新消息刷新二维码
void qr_refresh(void)
{
	//wristband_set_qrcode( &qr_code, len);
}

//延时ms
void HAL_Delay(uint32_t time)
{
	osal_task_delay(time);
}

//获取系统tick, 返回当前的ms数
uint32_t HAL_GetTick(void)
{
	return xTaskGetTickCount();
}

//获取SN号	已经在service_server.c中实现


//这两个函数再service_server.c中实现了
////设置MAC, 13字节
//bool wristband_set_mac( char * mac, uint8_t len)
//{
//	
//	return true;
//}

////获取MAC， 13字节
//bool wristband_get_mac(char * mac, uint8_t len)
//{
//	return true;
//}

//振动打开关闭
//1打开，0关闭
//使用	void service_ctrl_motor_shake( uint32_t delay)；	启动震动，delay为震动持续时间，单位为ms

//
void bt_int_evt_callback(app_io_evt_t *p_evt)
{
    if (p_evt->pin == BT_INT_PIN)
    {
        //BT int pin interrupr
    }
}

//以下几个gpio初始化
void bt_gpio_init(void)
{
	//bt int
	uint16_t ret;
    app_io_init_t io_init = APP_IO_DEFAULT_CONFIG;

    io_init.pull = APP_IO_PULLUP;
    io_init.mode = BT_INT_MODE;
    io_init.pin  = BT_INT_PIN;
    io_init.mux  = APP_IO_MUX;

    ret = app_io_event_register_cb(BT_INT_TYPE, &io_init, bt_int_evt_callback, "BT pin int");
    if (ret != APP_DRV_SUCCESS)
    {
        printf("Bt int init failed.\r\n");
    }

	
    //tsm_send
	io_init.pull = TSM_SEND_EN_PULL;
    io_init.mode = TSM_SEND_EN_MODE;
    io_init.pin  = TSM_SEND_EN_PIN;
    io_init.mux  = TSM_SEND_EN_MUX;
    ret = app_io_init(TSM_SEND_EN_TYPE, &io_init);
    if (ret != APP_DRV_SUCCESS)
    {
        printf("tsm_send_en init failed.\r\n");
    }
	
	//bt_en
	io_init.pull = BT_EN_PULL;
    io_init.mode = BT_EN_MODE;
    io_init.pin  = BT_EN_PIN;
    io_init.mux  = BT_EN_MUX;
    ret = app_io_init(BT_EN_TYPE, &io_init);
    if (ret != APP_DRV_SUCCESS)
    {
        printf("bt_en init failed.\r\n");
    }
	
	//tsm_wup
	io_init.pull = TSM_WUP_PULL;
    io_init.mode = TSM_WUP_MODE;
    io_init.pin  = TSM_WUP_PIN;
    io_init.mux  = TSM_WUP_MUX;
    ret = app_io_init(TSM_WUP_TYPE, &io_init);
    if (ret != APP_DRV_SUCCESS)
    {
        printf("tsm_wup init failed.\r\n");
    }
}

//TSM_SEND_EN GPIO
//输出
//1- 输出高
//0- 输出低
void bt_send_enable_gpio_output(uint8_t sta)
{
	if(sta)
	{
		app_io_write_pin(TSM_SEND_EN_TYPE, TSM_SEND_EN_PIN, APP_IO_PIN_SET);
	}
	else
	{
		app_io_write_pin(TSM_SEND_EN_TYPE, TSM_SEND_EN_PIN, APP_IO_PIN_RESET);
	}
}

//BT_EN GPIO ,接蓝牙reset
//输出
//1- 输出高
//0- 输出低
void bt_rst_gpio_output(uint8_t sta)
{
	if(sta)
	{
		app_io_write_pin(BT_EN_TYPE, BT_EN_PIN, APP_IO_PIN_SET);
	}
	else
	{
		app_io_write_pin(BT_EN_TYPE, BT_EN_PIN, APP_IO_PIN_RESET);
	}
}


//TSM_WUP   GPIO
//输入
//1-输入为高
//0-输入为低
uint8_t bt_wakeup_read_gpio_input(void)
{
	return app_io_read_pin(TSM_WUP_TYPE, TSM_WUP_PIN); 
}

//BT_INT   GPIO
//输入
uint8_t bt_int_read_gpio_input(void)
{
	return app_io_read_pin(BT_INT_TYPE, BT_INT_PIN); 
}

#define UART_DATA_LEN	200

static uint8_t g_ring_buffer[UART_DATA_LEN] = {0};
static uint8_t g_rx_buffer[UART_DATA_LEN]   = {0};
static SemaphoreHandle_t rx_sync_sem = NULL;
static SemaphoreHandle_t tx_cmpl_sem = NULL;

app_uart_params_t uart_params = {
    .id      = UART_ID,
    .pin_cfg = {
           .tx = {
              .type = UART_PIN_TYPE,
              .mux  = UART_PIN_MUX,
              .pin  = UART_TX_PIN,
              .pull = APP_IO_PULLUP,
           },
           .rx = {
              .type = UART_PIN_TYPE,
              .mux  = UART_PIN_MUX,
              .pin  = UART_RX_PIN,
              .pull = APP_IO_PULLUP,
           },
    },
    .init = {
        .baud_rate = 115200,
        .data_bits = UART_DATABITS_8,
        .stop_bits = UART_STOPBITS_1,
        .parity    = UART_PARITY_NONE,
        .hw_flow_ctrl    = UART_HWCONTROL_NONE,
        .rx_timeout_mode = UART_RECEIVER_TIMEOUT_ENABLE,
    },
};
	
//串口发送
void uart_send(uint8_t buf[], uint8_t len)
{
	uint32_t ret;
	
	if(!uart_is_init)
	{
		return;
	}
	
	app_uart_transmit_async(UART_ID, buf, len);
	
	ret = xSemaphoreTake(tx_cmpl_sem,  pdMS_TO_TICKS(300));
	if (ret != pdTRUE)
	{
		APP_LOG_INFO("uart send to!");
		return;
	}
}

uint16_t recv_len;

static void service_server_uart_callback(app_uart_evt_t *p_evt)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (p_evt->type == APP_UART_EVT_RX_DATA)
    {
		recv_len = p_evt->data.size;
        xSemaphoreGiveFromISR(rx_sync_sem, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken == pdTRUE)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
	if (p_evt->type == APP_UART_EVT_TX_CPLT)
    {
        xSemaphoreGiveFromISR(tx_cmpl_sem, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken == pdTRUE)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
    if (p_evt->type == APP_UART_EVT_ERROR)
    {
		
    }
}

void uart_init(void)
{
    app_drv_err_t ret = 0;
    app_uart_tx_buf_t uart_buffer = {0};

    uart_buffer.tx_buf = g_ring_buffer;
    uart_buffer.tx_buf_size = sizeof(g_ring_buffer);

    rx_sync_sem = xSemaphoreCreateBinary();
    if (rx_sync_sem == NULL)
    {
        printf("Create uart rx sync semaphore failed.\r\n");
        return;
    }
	
	tx_cmpl_sem = xSemaphoreCreateBinary();
    if (tx_cmpl_sem == NULL)
    {
        printf("Create uart tx cmpl semaphore failed.\r\n");
        return;
    }
	
    ret = app_uart_init(&uart_params, service_server_uart_callback, &uart_buffer);
    if (ret != APP_DRV_SUCCESS)
    {
        printf("UART initialization failed.\r\n");
    }
	uart_is_init = true;
}

void uart_reinit(void)
{
	app_drv_err_t ret = 0;
	app_uart_tx_buf_t uart_buffer = {0};

    uart_buffer.tx_buf = g_ring_buffer;
    uart_buffer.tx_buf_size = sizeof(g_ring_buffer);
	
	ret = app_uart_init(&uart_params, service_server_uart_callback, &uart_buffer);
    if (ret != APP_DRV_SUCCESS)
    {
        printf("UART initialization failed.\r\n");
    }
	uart_is_init = true;
}
	
void uart_deinit(void)
{
	app_uart_deinit(UART_ID);
	uart_is_init = false;
}

void trace(const char * strOutputString, ...)
{

	char strBuffer[1025] = { 0 };
	va_list vlArgs;

	va_start(vlArgs, strOutputString); 	
	vsprintf(strBuffer, strOutputString, vlArgs);
	va_end(vlArgs);

	APP_LOG_INFO(strBuffer);
}

//串口接收函数,放到串口接收中断里面
__weak void uart_receive(uint8_t buf[], uint8_t len)
{

}

__weak void bt_uart_process()
{

}

void uart_recv_process()
{
	static uint32_t uart_recv_no_pack_time = 0;
	uint32_t ret;
	memset(g_rx_buffer, 0, sizeof(g_rx_buffer));
	app_uart_receive_async(UART_ID, g_rx_buffer, UART_DATA_LEN);
	ret = xSemaphoreTake(rx_sync_sem,  pdMS_TO_TICKS(300));
	if (ret != pdTRUE)
	{
		//APP_LOG_INFO("uart recv to!");
		uart_recv_no_pack_time += 300;
		if(uart_recv_no_pack_time>=90000)
		{
			uart_recv_no_pack_time = 0;
			uart_reset_flag = true;
		}
		return;
	}
	//
	uart_recv_no_pack_time = 0;
	//
	uart_receive( g_rx_buffer, recv_len);
}

void service_server_read_mac()
{
#define UART_TO		2000
	//data
	uint32_t ret;
	uint8_t send_buf[] = {0x55,0xaa,0x02,0x03,0x11};
	uint32_t to;
	
	while(1)
	{
		//send_en low
		bt_send_enable_gpio_output(0);
		//wait for wake up low
		to = 0;
		while(to<UART_TO)
		{
			if(bt_wakeup_read_gpio_input() == 0)
				break;
			//APP_LOG_INFO("wait wakeup");
			to += 10;
			vTaskDelay(10);
		}
		if(to<UART_TO)
		{
			//send data
			uart_send( send_buf, 5);
			//send en high
			bt_send_enable_gpio_output(1);
			//recv
			memset(g_rx_buffer, 0, sizeof(g_rx_buffer));
			app_uart_receive_async(UART_ID, g_rx_buffer, UART_DATA_LEN);
			ret = xSemaphoreTake(rx_sync_sem,  pdMS_TO_TICKS(20));
			if (ret != pdTRUE)
			{
				//APP_LOG_INFO("uart recv to!");
			}
			else
			{
				//APP_LOG_HEX_DUMP( g_rx_buffer, recv_len);
				if((g_rx_buffer[4])||(g_rx_buffer[5])||(g_rx_buffer[6])||(g_rx_buffer[7])||(g_rx_buffer[8])||(g_rx_buffer[9]))
				{
					service_mac_update(g_rx_buffer+4);
					return;
				}
			}
		}
		else
		{
			//send en high
			bt_send_enable_gpio_output(1);
		}
		vTaskDelay(10000);
	}
}


//新任务函数入口
void service_server_task(void *arg)
{	
	bt_gpio_init();
	//rst bt
	bt_rst_gpio_output(0);
	vTaskDelay(500);
	bt_rst_gpio_output(1);
	vTaskDelay(10);
	uart_init();
	
	service_server_read_mac();
	
	while(1)
	{
		if(app_power_off_without_rtc_status_get())
		{
			osal_task_delay_ms(10000);
			continue;
		}
		if(uart_reset_flag)
		{
			//rst bt
			bt_rst_gpio_output(0);
			vTaskDelay(500);
			bt_rst_gpio_output(1);
			vTaskDelay(10);
			uart_reset_flag = false;
		}
		if(SYS_STATE_SLEEP != sys_state_get())  //当处于休眠的时候，不再读取uart数据
		{
			if(!uart_is_init)
			{
				uart_reinit();
			}
			uart_recv_process();
			bt_uart_process();
		}
		else
		{
			vTaskDelay(10000);
			if(uart_is_init)
			{
				uart_deinit();
			}
		}
		vTaskDelay(10);
	}
}

void service_server_task_create(void)
{
    osal_task_create("uart_task",   service_server_task, 2048,    configMAX_PRIORITIES - 1, &g_task_handle.service_server_task_handle);
}
	
void service_server_sos_alarm_send()
{
	
}

void ota_data_receive(uint8_t *buf, uint16_t len)
{
	APP_LOG_INFO("%s.%d",__FUNCTION__,__LINE__);
	service_ota_unpack( buf);
}

//void ota_data_request(void)
//{
//	ota_data_send( buf, len);
//}
