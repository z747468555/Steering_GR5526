#ifndef __APP_SYSTEM_MANAGER_H__
#define __APP_SYSTEM_MANAGER_H__

#include "osal.h"
#include "custom_config.h"

/*
 * define the prio for tasks
 */
#define TASK_PRIO_GUI_RENDER            7
#define TASK_PRIO_TOUCHPAD_READ         8
#define TASK_PRIO_EVT_SERVICE         	9
#define TASK_PRIO_RUN_SERVICE         	9
#define TASK_PRIO_OTA                   6


/*
 * define the stack size for tasks
 */
#define TASK_SIZE_GUI_RENDER            10240u
#define TASK_SIZE_TOUCHPAD_READ         512u
#define TASK_SIZE_APP_EVENT             512u
#define TASK_SIZE_OTA                   1024u
#define TASK_SIZE_SERVICE_EVT           1024u
#define TASK_SIZE_SERVICE_RUN           512u


#define SYS_SCREEN_OFF_WAIT_TIME_MS         30000u
#define SYS_SLEEP_WAIT_TIME_MS              30000u


typedef enum {
    SYS_STATE_UNSET = 0,
    SYS_STATE_ACTIVE,           /* 正常工作状态 */
    SYS_STATE_SCREEN_OFF,       /* 屏幕息屏状态 */
    SYS_STATE_SLEEP,            /* 人机设备休眠状态 */
} sys_running_state_e;


typedef struct {
    sys_running_state_e running_state;
    uint32_t            running_state_base_ms;
} sys_param_t;


typedef struct {
    osal_task_handle_t gui_handle;
    osal_task_handle_t indev_handle;
    osal_task_handle_t gui_evt_handle;
    osal_task_handle_t app_service_evt_handle;
    osal_task_handle_t app_service_run_handle;
    osal_task_handle_t app_3220_handle;
    osal_task_handle_t ble_test;
	osal_task_handle_t service_server_task_handle;
    osal_task_handle_t ota_handle;
} sys_task_handle_t;


extern sys_task_handle_t        g_task_handle;

void                    sys_state_reset_base_time(void);
uint32_t                sys_state_get_base_time(void);
uint32_t                sys_state_calc_delta_time(void);
void                    sys_state_switch(sys_running_state_e new_state);
sys_running_state_e     sys_state_get(void);

#endif /* __APP_SYSTEM_MANAGER_H__ */
