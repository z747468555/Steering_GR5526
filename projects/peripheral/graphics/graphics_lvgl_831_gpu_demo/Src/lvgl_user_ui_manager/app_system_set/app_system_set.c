#include "app_system_set.h"
// Cookie is ok

static void system_set_item_clicked_cb(lv_event_t* e);
typedef struct _gui_system_set_item_t
{
    const void* image;
    const char* text;
    lv_event_cb_t event_cb;
    screen_id_e id;

} gui_system_set_item;

static lv_obj_t* page = NULL;
static uint16_t scroll_y = 0;

typedef enum
{
    SYSTEM_SET_INVALID = -1,

    SYSTEM_SET_CONTINUITY,
    SYSTEM_SET_BRIGHTNESS,
    SYSTEM_SET_CLOCK_SET,
    SYSTEM_SET_RESTING,
    SYSTEM_SET_ID_CODE,
    SYSTEM_SET_VERSION,
    SYSTEM_SET_SHUTDOWN,

    SYSTEM_SET_MAX,

} system_set_id;

gui_system_set_item system_set[SYSTEM_SET_MAX] =
{
    [SYSTEM_SET_CONTINUITY] = {&wd_img_app_settings_sport_continuity,    "连续监测", system_set_item_clicked_cb, (void*)SCREEN_ID_CONTINUITY},
    [SYSTEM_SET_BRIGHTNESS] = {&wd_img_app_settings_brightness,          "屏幕调节", system_set_item_clicked_cb, (void*)SCREEN_ID_BRIGHTNESS},
    [SYSTEM_SET_CLOCK_SET]  = {&wd_img_app_settings_alarm,               "时钟设置", system_set_item_clicked_cb, (void*)SCREEN_ID_CLOCK_SET},
    [SYSTEM_SET_RESTING]    = {&wd_img_app_settings_screen_shutdowntime, "息屏时间", system_set_item_clicked_cb, (void*)SCREEN_ID_RESTING},
    [SYSTEM_SET_ID_CODE]    = {&wd_img_app_settings_IDCard,              "身份码",   system_set_item_clicked_cb, (void*)SCREEN_ID_ID_CODE},
    [SYSTEM_SET_VERSION]    = {&wd_img_app_settings_upgrade,             "版本信息", system_set_item_clicked_cb, (void*)SCREEN_ID_VERSION},
    [SYSTEM_SET_SHUTDOWN]   = {&wd_img_app_settings_shutdown,            "关机",     system_set_item_clicked_cb, (void*)SCREEN_ID_SHUTDOWN},
};

static void system_set_return(lv_event_t* e);

lv_obj_t* system_set_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, system_set_return, LV_EVENT_GESTURE, NULL);

    page = cookie_page_create(screen);
    lv_obj_set_flex_flow(page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_top(page, 30, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(page, 30, LV_PART_MAIN);

    for(uint8_t item = SYSTEM_SET_CONTINUITY; item < SYSTEM_SET_MAX; item++)
    {
        cookie_list_item_create(page, system_set[item].image, system_set[item].text, system_set[item].event_cb, system_set[item].id);
    }

    lv_obj_scroll_to_y(page, scroll_y, LV_ANIM_OFF);
    return screen;
}

void system_set_release(void)
{
    page = NULL;
}

static void system_set_return(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_GESTURE)
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if(dir == LV_DIR_RIGHT)
        {
            scroll_y = 0;
            SCREEN_RETURN(0, 0);
        }
    }

    lv_indev_wait_release(lv_indev_get_act());
}

static void system_set_item_clicked_cb(lv_event_t* e)
{
    scroll_y = lv_obj_get_scroll_y(page);
    screen_id_e jump_id = (screen_id_e)lv_event_get_user_data(e);
    SCREEN_CALL(jump_id, 0, 0);
}