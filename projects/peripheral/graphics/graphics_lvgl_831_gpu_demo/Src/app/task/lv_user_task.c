/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "grx_sys.h"
#include "platform_sdk.h"
#include "osal.h"
#include "osal_task.h"
#include "drv_adapter_display.h"
#include "app_sys_manager.h"
#include "app_ui_manager.h"
#include "app_power_manager.h"
#include "lv_port_gr5526.h"
#include "lv_port_indev.h"

#define APP_LOG_TAG "ui"
#include "app_log.h"

#include "app_3220_task.h"

#include "ui_win_manager.h"
#include "ui_win_message.h"
#include "app_service_simulate.h"
#include "app_service_task.h"

/*
 * MACRO DEFINITIONS
 *****************************************************************************************
 */


/*
 * GLOBAL PMU FUNCTION DECLARATIONS
 *****************************************************************************************
 */

/*
 * LOCAL MACRO DEFINITIONS
 *****************************************************************************************
 */

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

static bool lv_env_is_inited = false;
static osal_sema_handle_t s_gui_refresh_sem;
static osal_sema_handle_t s_sleep_mgnt_sem;

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
static void lvgl_env_init(void)
{
    lvgl_port_init();

    lv_env_is_inited = true;

    osal_sema_binary_create(&s_gui_refresh_sem);
    osal_sema_binary_create(&s_sleep_mgnt_sem);
}


/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
#define LCD_TEST	0
#if LCD_TEST
#include "graphics_dc_st77916_qspi_drv.h"

extern uint8_t *psram_buf;

static void lcd_test()
{
	uint32_t i;

	lvgl_env_init();

	osal_task_delay(100);
	for(i=0;i<129600*2;i++)
	{
		psram_buf[i] = 0xc3;
	}

	osal_task_delay(100);
	graphics_dc_st77916_set_show_area( 0, 359, 0, 359);

	osal_task_delay(1000);
	graphics_dc_st77916_flush( psram_buf, 0x05, 360,360);//RGB565

	while(1)
	{
		osal_task_delay(1000);
		//graphics_dc_st77916_flush( psram_buf, 0x05, 360,360);//RGB565
	}
}
#endif

void app_gui_render_task(void *p_arg)
{
	#if LCD_TEST
	lcd_test();
	#else
    uint32_t delayTime = 0;

    lvgl_env_init();

    ui_win_message_creat();

	wm_goto_start_screen(SCREEN_ID_LOGO,0);

	//APP_LOG_INFO( "%s.%d\r\n", __FUNCTION__, __LINE__);

    while(1){
		if(app_power_off_without_rtc_status_get())
		{
			osal_task_delay_ms(10000);
			continue;
		}
        delayTime = lv_task_handler();


        if(SYS_STATE_SLEEP != sys_state_get()) {
            //osal_task_delay(delayTime);
            osal_sema_take(s_gui_refresh_sem, delayTime);
        } else {
            osal_sema_take(s_gui_refresh_sem, OSAL_MAX_DELAY);
        }
        model_event_handler(NULL);
    }
	#endif
}


static void _lv_reload_gui(void* user_data) {
    lv_obj_invalidate(lv_scr_act());
}

void app_indev_read_task(void * args)
{
    bool is_press_evt;
	bool is_sleep = 0;
	static bool wakeup = 0;

    while(1) {
        switch(sys_state_get()) {

            case SYS_STATE_UNSET:
            {
                if(lv_env_is_inited){
                    sys_state_switch(SYS_STATE_ACTIVE);
                } else {
                    osal_task_delay(100);
                }
            }
            break;

            case SYS_STATE_ACTIVE:
            {
				if(wakeup == 1)
				{
					wakeup = 0;
					st77916_poweron();
				}
                is_press_evt = lv_port_indev_poll();
                if(is_press_evt) {
                    osal_sema_give(s_gui_refresh_sem);
                    app_resting_reset();
                }
                osal_sema_take(s_sleep_mgnt_sem, 100);
            }
            break;

            case SYS_STATE_SCREEN_OFF:
            {
                APP_LOG_INFO("GOTO SCREEN OFF, tick: %d \r\n", osal_task_get_tick_count());
                sys_state_switch(SYS_STATE_SLEEP);
                drv_adapter_disp_on(false);
				st77916_lowpower();
				wakeup = 1;
				
            }
            break;

            case SYS_STATE_SLEEP:
            {
				if(OSAL_SUCCESS != osal_sema_take(s_sleep_mgnt_sem, 10000))
				{
					if(is_sleep == 0)
					{
						APP_LOG_INFO("GOTO Sleep!\r\n");
						sys_peripherals_sleep();
						is_sleep = 1;
					}
				}
                else
				{
					is_sleep = 0;
					sys_peripherals_resume();
					lv_async_call(_lv_reload_gui, NULL);
                    osal_sema_give(s_gui_refresh_sem);
					drv_adapter_disp_on(true);
                    app_brightness_restore();
					sys_state_switch(SYS_STATE_ACTIVE);
                    app_resting_start();
                    send_msg_to_ui_high(EVENT_ELETRICITY_CHANGE, EVENT_ELETRICITY_CHANGE);
					APP_LOG_INFO("GOTO SCREEN ON, tick: %d \r\n", osal_task_get_tick_count());
				}
            }
            break;

            default:break;
        }
    }
}

/*
 * Override this function, defined as __weak in file drv_adapter_port_touchpad.c
 */
void _touchpad_drv_irq_notify(void) {
	if(!app_power_off_without_rtc_status_get())
	{
		osal_sema_give(s_sleep_mgnt_sem);
	}
}

void screen_wakeup()
{
	osal_sema_give(s_sleep_mgnt_sem);
}

/**
 *****************************************************************************************
 * @brief To create two task, the one is ble-schedule, another is watcher task
 *****************************************************************************************
 */
void lv_user_task_create(void)
{
    osal_task_create("task_indev", app_indev_read_task, TASK_SIZE_TOUCHPAD_READ, TASK_PRIO_TOUCHPAD_READ, &g_task_handle.indev_handle);
    osal_task_create("task_gui",   app_gui_render_task, TASK_SIZE_GUI_RENDER,    TASK_PRIO_GUI_RENDER,    &g_task_handle.gui_handle);
}
