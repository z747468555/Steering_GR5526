#include "app_resting.h"
#include "app_service_simulate.h"
// Cookie is ok

static void resting_return(lv_event_t* e);
static void resting_confirm_button(lv_event_t* e);

lv_obj_t* roller = NULL;

lv_obj_t* resting_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, resting_return, LV_EVENT_GESTURE, NULL);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "息屏时间");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    int32_t value = app_resting_get_timeout();
    roller = cookie_roller_create(screen, LV_HOR_RES, 10, 90, 10, value);
    lv_obj_center(roller);

    lv_obj_t* sec = cookie_label_create(screen, &font_24px, LV_COLOR_GRAY_WHITE);
    lv_label_set_text_fmt(sec, "%s", "秒");
    lv_obj_align(sec, LV_ALIGN_TOP_LEFT, 214, 164);

    lv_obj_t* confirm = cookie_img_create(screen, &wd_img_app_clock_confirm);
    lv_obj_add_event_cb(confirm, resting_confirm_button, LV_EVENT_CLICKED, NULL);
    lv_obj_align(confirm, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_flag(confirm, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(confirm, 20);

    return screen;
}

void resting_release(void)
{
    roller = NULL;
}

static void resting_confirm_button(lv_event_t* e)
{
    uint8_t rest_time = cookie_get_roller_data(roller);
    app_resting_set_timeout(rest_time);

    SCREEN_RETURN(0, 0);
}

static void resting_return(lv_event_t* e)
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
