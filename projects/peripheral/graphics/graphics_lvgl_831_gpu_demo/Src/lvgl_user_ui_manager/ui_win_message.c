#include "ui_win_message.h"
#include "lvgl.h"
#include "osal_queue.h"
#include "osal_error.h"

#define APP_LOG_TAG "ui"
#include "app_log.h"

#include "app_service_simulate.h"
#include "FreeRTOS.h"
#include "Timers.h"

uint32_t g_user_event;

osal_queue_handle_t g_mq_high_priority;      //message queue used by model timer, with high priority
osal_queue_handle_t g_mq_normal_priority;    //message queue used by model timer, with normal priority
osal_queue_handle_t g_mq_idle_update;        //message queue used when lcd off
osal_queue_handle_t g_mq_system;             //message queue for system event

//uint32_t ui_get_active_keep_time(void){};

//message queue of lvgl
void ui_win_message_creat(void)
{
    int ret = OSAL_SUCCESS;
    ret = osal_queue_create(&g_mq_system,MSG_QUEUE_NUMBER_SYS,sizeof(ser_event_msg_t));
    LV_ASSERT(ret == OSAL_SUCCESS);

    ret = osal_queue_create(&g_mq_high_priority,MSG_QUEUE_NUMBER_HIGH,sizeof(ser_event_msg_t));
    LV_ASSERT(ret == OSAL_SUCCESS);

    ret = osal_queue_create(&g_mq_normal_priority,MSG_QUEUE_NUMBER_NORMAL,sizeof(ser_event_msg_t));
    LV_ASSERT(ret == OSAL_SUCCESS);

    ret = osal_queue_create(&g_mq_idle_update,MSG_QUEUE_NUMBER_IDLE_UPDATE,sizeof(ser_event_msg_t));
    LV_ASSERT(ret == OSAL_SUCCESS);
}

void ui_win_message_monitor_start(void)
{
    static uint8_t _init = 0;

    if (_init)
        return;

    g_user_event = lv_event_register_id();

    // "model" timer
    lv_timer_create(model_event_handler,CONFIG_MODEL_TIME_INTERVAL,NULL);
    _init = 1;
}

void model_event_handler(lv_timer_t *t)
{
    ser_event_msg_t data;
    while (osal_queue_receive(g_mq_high_priority,&data,0) == OSAL_SUCCESS)
    {
        uint16_t scr_id = wm_get_active_screen_id();

        switch (data.msg_type)
        {
            case EVENT_UPPER_RIGHT_KEY_CLICK:
                if (wm_get_active_screen() && (scr_id != SCREEN_ID_WATCHFACE) && (scr_id != SCREEN_ID_OTA)) {
                    SCREEN_RETURN(0, 0);
                } else if (wm_get_active_screen() && (scr_id == SCREEN_ID_WATCHFACE)){
                    lv_event_send(wm_get_active_screen(), LV_EVENT_USER, &data);
                }
                break;

            case EVENT_UPPER_LEFT_KEY_DBLCLICK:
                if (wm_get_active_screen() && (scr_id != SCREEN_ID_SOS) && (scr_id != SCREEN_ID_OTA)) {
                    SCREEN_CALL(SCREEN_ID_SOS, 0, 0);
                } else if (wm_get_active_screen() && (scr_id == SCREEN_ID_SOS)) {
                    lv_event_send(wm_get_active_screen(), LV_EVENT_USER, &data);
                }
                break;

            case EVENT_SOS_ALARM_STOP:
                if (wm_get_active_screen() && (scr_id == SCREEN_ID_SOS)) {
                    lv_event_send(wm_get_active_screen(), LV_EVENT_USER, &data);
                }
                break;

            case EVENT_SYSTEM_SHUT_DOWN:
                if (wm_get_active_screen() && (scr_id != SCREEN_ID_SHUTDOWN)) {
                    SCREEN_CALL(SCREEN_ID_SHUTDOWN, 0, 0);
                }
                break;

            case EVENT_WATCHFACE_UPDATA:
            case EVENT_ELETRICITY_CHANGE:
            case EVENT_CHARGER_CHANGE:
                if (wm_get_active_screen() && (scr_id == SCREEN_ID_WATCHFACE)) {
                    lv_event_send(wm_get_active_screen(), LV_EVENT_USER, &data);
                }
                break;

            case EVENT_SPORT_CHANGE:
                if (wm_get_active_screen() && (scr_id == SCREEN_ID_WATCHFACE)) {
                    lv_event_send(wm_get_active_screen(), LV_EVENT_USER, &data);
                } else if (wm_get_active_screen() && (scr_id == SCREEN_ID_SPORT)){
                    lv_event_send(wm_get_active_screen(), LV_EVENT_USER, &data);
                }
                break;

            case EVENT_ENVIRONMENT_CHANGE:
                if (wm_get_active_screen() && (scr_id == SCREEN_ID_ENVIRONMENT)) {
                    lv_event_send(wm_get_active_screen(), LV_EVENT_USER, &data);
                }
                break;

            case EVENT_HR_CHANGE:
                if (wm_get_active_screen() && (scr_id == SCREEN_ID_WATCHFACE)) {
                    lv_event_send(wm_get_active_screen(), LV_EVENT_USER, &data);
                } else if(wm_get_active_screen() && (scr_id == SCREEN_ID_HR_BO)) {
                    lv_event_send(wm_get_active_screen(), LV_EVENT_USER, &data);
                }
                break;

            case EVENT_ELECTRODE_TOUCH:
            case EVENT_ELECTRODE_REMOVED:
            case EVENT_ELECTRODE_DATA:
                if (wm_get_active_screen() && (scr_id == SCREEN_ID_CARDIOGRAM)) {
                    lv_event_send(wm_get_active_screen(), LV_EVENT_USER, &data);
                }
                break;

            case EVENT_ALARM_CLOCK:
                if (wm_get_active_screen() && (scr_id != SCREEN_ID_MSG)) {
                    SCREEN_POPUP(SCREEN_ID_MSG, data.msg_type, NULL);
                }
                break;

            case EVENT_TEXT_LATEST:
                if (wm_get_active_screen() && (scr_id != SCREEN_ID_MSG)) {
                    SCREEN_POPUP(SCREEN_ID_MSG, data.msg_type, NULL);
                }
                break;

            case EVENT_OTA:
                if (wm_get_active_screen() && (scr_id != SCREEN_ID_OTA)) {
                    SCREEN_POPUP(SCREEN_ID_OTA, data.msg_type, NULL);
                }
                break;

            case EVENT_BODY_CHANGE:
                if (wm_get_active_screen() && (scr_id == SCREEN_ID_BODY_CHANGE)) {
                    lv_event_send(wm_get_active_screen(), LV_EVENT_USER, &data);
                }
                break;

            default:
                break;
        }

        COOKIE_LOG("receive high msg = %u to scr_id = %u,tick=%d \n", data.msg_type, scr_id, xTaskGetTickCount());
    }

    //message receive && deal for system mq (wm will handle this message)
    while (osal_queue_receive(g_mq_system,&data,0) == OSAL_SUCCESS)
    {
        if (data.msg_type == EVT_SYSTEM_CHARGER_STATUS_CHANGED)
        {
            //battery_update_charge_status(&data); //是否要弹出充电界面
        }
        else
        {
            //wm_sys_event_handler(&data);
        }
    }

    while (osal_queue_receive(g_mq_normal_priority,&data,0) == OSAL_SUCCESS)
    {
        // do something
    }
}


void send_msg_to_ui_normal(uint16_t id,uint32_t p_data)
{
    APP_LOG_INFO("send_msg_to_ui_normal %u",id);
    ser_event_msg_t data = {0};
    data.msg_type = id;
    data.data.data = p_data;
    osal_queue_send(g_mq_normal_priority,&data,0);

}


void send_msg_to_ui_high(uint16_t id,uint32_t p_data)
{
    ser_event_msg_t data = {0};
    data.msg_type = id;
    data.data.data = p_data;
    osal_queue_send(g_mq_high_priority,&data,0);
    //lv_task_handler();
    //lv_obj_invalidate(lv_scr_act());

}


// reference ui_msg_high_handler.c
