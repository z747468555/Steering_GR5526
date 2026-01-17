#include "app_clock_set.h"
#include "app_container.h"
// Cookie is ok

static void clock_set_return(lv_event_t* e);
static void clock_set_item_clicked_cb(lv_event_t* e);

typedef struct _gui_clock_set_item_t
{
    const void* image;
    const char* text;
    lv_event_cb_t event_cb;
    screen_id_e id;

} gui_clock_set_item;

typedef enum
{
    CLOCK_SET_INVALID = -1,

    CLOCK_SET_ALARM,
    CLOCK_SET_TIME,
    CLOCK_SET_STOPWATCH,

    CLOCK_SET_MAX,

} clock_set_id;

gui_clock_set_item clock_set[CLOCK_SET_MAX] =
{
    [CLOCK_SET_ALARM]      = {&wd_img_app_clock_alarm,     "闹钟设置", clock_set_item_clicked_cb, (void*)SCREEN_ID_ALARM},
    [CLOCK_SET_TIME]       = {&wd_img_app_clock_time_calc, "时间校准", clock_set_item_clicked_cb, (void*)SCREEN_ID_TIME},
    [CLOCK_SET_STOPWATCH]  = {&wd_img_app_clock_stopwatch, "秒表设置", clock_set_item_clicked_cb, (void*)SCREEN_ID_STOPWATCH},
};

lv_obj_t* clock_set_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, clock_set_return, LV_EVENT_GESTURE, NULL);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_top(screen, 30, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(screen, 30, LV_PART_MAIN);

    for(uint8_t item = CLOCK_SET_ALARM; item < CLOCK_SET_MAX; item++)
    {
        cookie_list_item_create(screen, clock_set[item].image, clock_set[item].text, clock_set[item].event_cb, clock_set[item].id);
    }

    return screen;
}

void clock_set_release(void)
{

}

static void clock_set_return(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_GESTURE)
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if(dir == LV_DIR_RIGHT)
        {
            SCREEN_RETURN(0, 0);
        }
    }

    lv_indev_wait_release(lv_indev_get_act());
}

static void clock_set_item_clicked_cb(lv_event_t* e)
{
    screen_id_e jump_id = (screen_id_e)lv_event_get_user_data(e);
    SCREEN_CALL(jump_id, 0, 0);
}
