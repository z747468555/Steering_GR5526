#include "app_watchface.h"
#include "app_service_simulate.h"
#include "ui_win_message.h"
#include "app_container.h"

lv_obj_t *applist;
static uint16_t scroll_y = 0;

static void applist_item_body_clicked_cb(lv_event_t* e);
static void applist_item_clicked_cb(lv_event_t* e);
static void applist_debug_clicked_cb(lv_event_t* e);
static void applist_return_watchface_cb(lv_event_t* e);


typedef struct _gui_applist_item_t
{
    const void* image;
    const char* text;
    lv_event_cb_t event_cb;
    screen_id_e id;

} gui_applist_item;

typedef enum
{
    APPLIST_INVALID = -1,

    APPLIST_EMOTIONAL,
    APPLIST_VISUAL,
    APPLIST_MENTAL,
    APPLIST_ENERGY,
    APPLIST_PSYCHOLOGICAL,
    APPLIST_HR_BO,
    APPLIST_SOS,
    APPLIST_REMINDER,
    APPLIST_CARDIOGRAM,
    APPLIST_SPORT,
    APPLIST_ENVIRONMENT,
    APPLIST_SYSTEM_SET,

    APPLIST_MAX,

} applist_id;

gui_applist_item app_list[APPLIST_MAX] =
{
    [APPLIST_EMOTIONAL]     = {&wd_img_applist_emotion_icon,         "情绪波动", applist_item_body_clicked_cb, (void*)EMOTIONAL},
    [APPLIST_VISUAL]        = {&wd_img_applist_vision_icon,          "视觉疲劳", applist_item_body_clicked_cb, (void*)VISUAL},
    [APPLIST_MENTAL]        = {&wd_img_applist_mental_icon,          "脑力疲劳", applist_item_body_clicked_cb, (void*)MENTAL},
    [APPLIST_ENERGY]        = {&wd_img_applist_energy_icon,          "精力专注", applist_item_body_clicked_cb, (void*)ENERGY},
    [APPLIST_PSYCHOLOGICAL] = {&wd_img_applist_psychology_icon,      "心理压力", applist_item_body_clicked_cb, (void*)PSYCHOLOGICAL},
    [APPLIST_HR_BO]         = {&wd_img_applist_physiological_icon,   "生理指标", applist_item_clicked_cb,      (void*)SCREEN_ID_HR_BO},
    [APPLIST_SOS]           = {&wd_img_applist_sos_icon,             "SOS报警",  applist_item_clicked_cb,      (void*)SCREEN_ID_SOS},
    [APPLIST_REMINDER]      = {&wd_img_applist_alarm_center_icon,    "提醒中心", applist_item_clicked_cb,      (void*)SCREEN_ID_REMINDER},
    [APPLIST_CARDIOGRAM]    = {&wd_img_applist_ecg_icon,             "心电监测", applist_item_clicked_cb,      (void*)SCREEN_ID_CARDIOGRAM},
    [APPLIST_SPORT]         = {&wd_img_applist_sport_monitor_icon,   "运动监测", applist_item_clicked_cb,      (void*)SCREEN_ID_SPORT},
    [APPLIST_ENVIRONMENT]   = {&wd_img_applist_environment_icon,     "环境监测", applist_item_clicked_cb,      (void*)SCREEN_ID_ENVIRONMENT},
    [APPLIST_SYSTEM_SET]    = {&wd_img_applist_system_settings_icon, "系统设置", applist_item_clicked_cb,      (void*)SCREEN_ID_SYSTEM_SET},
};


lv_obj_t *ui_applist_page_init(uint32_t param)
{

    applist = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);

	lv_obj_set_flex_flow(applist, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_top(applist, 30, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(applist, 30, LV_PART_MAIN);

    for(uint8_t item = APPLIST_EMOTIONAL; item < APPLIST_MAX; item++)
    {
        cookie_list_item_create(applist, app_list[item].image, app_list[item].text, app_list[item].event_cb, app_list[item].id);
    }

    lv_obj_add_event_cb(applist,applist_return_watchface_cb,LV_EVENT_GESTURE,NULL);
#if 0
    lv_obj_t* debug_button = cookie_obj_create(applist, LV_HOR_RES, 90, LV_DIR_NONE);
    lv_obj_add_flag(debug_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(debug_button, 24, LV_PART_MAIN);
    lv_obj_set_style_bg_color(debug_button, LV_COLOR_RED, LV_PART_MAIN);
    lv_obj_add_event_cb(debug_button, applist_debug_clicked_cb, LV_EVENT_CLICKED, (void*)SCREEN_ID_DEBUG);

    lv_obj_t* debug_label = cookie_label_create(debug_button, &font_20px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(debug_label, "%s", "debug");
    lv_obj_center(debug_label);
#endif

    lv_obj_scroll_to_y(applist, scroll_y, LV_ANIM_OFF);

    return applist;
}

void ui_applist_page_release(void)
{

}

static void applist_return_watchface_cb(lv_event_t* e)
{
    if (e->code == LV_EVENT_GESTURE) {
        if (lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT)
        {
            scroll_y = lv_obj_get_scroll_y(applist);
            SCREEN_RETURN(0,0);
            lv_indev_wait_release(lv_indev_get_act());
        }
    }
}

static void applist_item_body_clicked_cb(lv_event_t* e)
{
    scroll_y = lv_obj_get_scroll_y(applist);
    screen_id_e jump_id = (screen_id_e)lv_event_get_user_data(e);
    SCREEN_CALL(SCREEN_ID_BODY_CHANGE, jump_id, 0);
}

static void applist_item_clicked_cb(lv_event_t* e)
{
    scroll_y = lv_obj_get_scroll_y(applist);
    screen_id_e jump_id = (screen_id_e)lv_event_get_user_data(e);
    SCREEN_CALL(jump_id, 0, 0);
}

static void applist_debug_clicked_cb(lv_event_t* e)
{
    if (debug_monkey_is_start())
    {
        return;
    }
    scroll_y = lv_obj_get_scroll_y(applist);
    screen_id_e jump_id = (screen_id_e)lv_event_get_user_data(e);
    SCREEN_CALL(jump_id, 0, 0);
}
