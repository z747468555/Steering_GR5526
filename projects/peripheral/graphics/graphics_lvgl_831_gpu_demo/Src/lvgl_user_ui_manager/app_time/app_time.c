#include "app_time.h"
#include "app_service_simulate.h"

static void time_return(lv_event_t* e);
static void resting_confirm_button(lv_event_t* e);

static lv_obj_t* roller_h = NULL;
static lv_obj_t* roller_m = NULL;
static lv_obj_t* roller_s = NULL;

lv_obj_t* time_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, time_return, LV_EVENT_GESTURE, NULL);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "时间校准");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    app_time_t current_time = {0};
    app_watchface_get_current(&current_time);

    roller_h = cookie_roller_create(screen, 100, 0, 23, 1, current_time.hour);
    lv_obj_align(roller_h, LV_ALIGN_TOP_LEFT, 20, 100);

    roller_m = cookie_roller_create(screen, 100, 0, 59, 1, current_time.min);
    lv_obj_align(roller_m, LV_ALIGN_TOP_MID, 0, 100);

    roller_s = cookie_roller_create(screen, 100, 0, 59, 1, current_time.sec);
    lv_obj_align(roller_s, LV_ALIGN_TOP_RIGHT, -20, 100);

    lv_obj_t* dot_1 = cookie_label_create(screen, &font_30px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(dot_1, "%s", ":");
    lv_obj_align(dot_1, LV_ALIGN_TOP_LEFT, 120, 160);

    lv_obj_t* dot_2 = cookie_label_create(screen, &font_30px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(dot_2, "%s", ":");
    lv_obj_align(dot_2, LV_ALIGN_TOP_LEFT, 232, 160);

    lv_obj_t* confirm = cookie_img_create(screen, &wd_img_app_clock_confirm);
    lv_obj_add_event_cb(confirm, resting_confirm_button, LV_EVENT_CLICKED, NULL);
    lv_obj_align(confirm, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_flag(confirm, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(confirm, 20);

    return screen;
}

void time_release(void)
{
    roller_h = NULL;
    roller_m = NULL;
    roller_s = NULL;
}

static void resting_confirm_button(lv_event_t* e)
{
    uint8_t h = cookie_get_roller_data(roller_h);
    uint8_t m = cookie_get_roller_data(roller_m);
    uint8_t s = cookie_get_roller_data(roller_s);

    gui_calibration_set_time(h, m, s);
    SCREEN_RETURN(0, 0);
}

static void time_return(lv_event_t* e)
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
