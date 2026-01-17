#include "app_stopwatch.h"

static void stopwatch_return(lv_event_t* e);
static void seconds_running(lv_timer_t* timer);
static void millisecond_running(lv_timer_t* timer);
static void stopwatch_control_button(lv_event_t* e);
static void stopwatch_reset_button(lv_event_t* e);

lv_obj_t* stopwatch_m = NULL;
lv_obj_t* stopwatch_s = NULL;
lv_obj_t* stopwatch_ms = NULL;
lv_obj_t* stopwatch_control = NULL;
lv_timer_t* s_timer = NULL;
lv_timer_t* ms_timer = NULL;
static uint64_t seconds_elapsed = 0;
static uint64_t millisecond_elapsed = 0;
static bool status = false;

lv_obj_t* stopwatch_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, stopwatch_return, LV_EVENT_GESTURE, NULL);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "秒表");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    lv_obj_t* dot_1 = cookie_label_create(screen, &font_30px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(dot_1, "%s", ":");
    lv_obj_align(dot_1, LV_ALIGN_TOP_LEFT, 120, 160);

    lv_obj_t* dot_2 = cookie_label_create(screen, &font_30px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(dot_2, "%s", ":");
    lv_obj_align(dot_2, LV_ALIGN_TOP_LEFT, 232, 160);

    stopwatch_m = cookie_label_create(screen, &font_48px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(stopwatch_m, "%02u", 0);
    lv_obj_align(stopwatch_m, LV_ALIGN_TOP_LEFT, 68, 147);

    stopwatch_s = cookie_label_create(screen, &font_48px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(stopwatch_s, "%02u", 0);
    lv_obj_align(stopwatch_s, LV_ALIGN_TOP_LEFT, 160, 147);

    stopwatch_ms = cookie_label_create(screen, &font_48px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(stopwatch_ms, "%02u", 0);
    lv_obj_align(stopwatch_ms, LV_ALIGN_TOP_LEFT, 252, 147);

    lv_obj_t* stopwatch_reset = cookie_img_create(screen, &wd_img_app_clock_reset);
    lv_obj_add_event_cb(stopwatch_reset, stopwatch_reset_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(stopwatch_reset, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(stopwatch_reset, LV_ALIGN_TOP_LEFT, 90, 264);
    lv_obj_set_ext_click_area(stopwatch_reset, 20);

    stopwatch_control = cookie_img_create(screen, &wd_img_app_clock_play);
    lv_obj_add_event_cb(stopwatch_control, stopwatch_control_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(stopwatch_control, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(stopwatch_control, LV_ALIGN_TOP_LEFT, 210, 264);
    lv_obj_set_ext_click_area(stopwatch_control, 20);

    seconds_elapsed = 0;
    millisecond_elapsed = 0;
    status = false;

    s_timer = lv_timer_create(seconds_running, 1000, NULL);
    lv_timer_set_repeat_count(s_timer, -1);
    lv_timer_ready(s_timer);
    lv_timer_pause(s_timer);

    ms_timer = lv_timer_create(millisecond_running, 1, NULL);
    lv_timer_set_repeat_count(ms_timer, -1);
    lv_timer_ready(ms_timer);
    lv_timer_pause(ms_timer);

    app_screen_keep_on(true);

    return screen;
}

void stopwatch_release(void)
{
    stopwatch_m = NULL;
    stopwatch_s = NULL;
    stopwatch_ms = NULL;
    stopwatch_control = NULL;

    if(s_timer) {
        lv_timer_del(s_timer);
        s_timer = NULL;
    }

    if(ms_timer) {
        lv_timer_del(ms_timer);
        ms_timer = NULL;
    }

    seconds_elapsed = 0;
    millisecond_elapsed = 0;
    status = false;

    app_screen_keep_on(false);
}

static void seconds_running(lv_timer_t* timer)
{
    seconds_elapsed ++;

    uint32_t m = seconds_elapsed / 60;
    uint32_t s = (seconds_elapsed % 3600) % 60;

    lv_label_set_text_fmt(stopwatch_m, "%02u", m);
    lv_label_set_text_fmt(stopwatch_s, "%02u", s);
}

static void millisecond_running(lv_timer_t* timer)
{
    millisecond_elapsed ++;
    lv_label_set_text_fmt(stopwatch_ms, "%02u", millisecond_elapsed % 100);
}

static void stopwatch_control_button(lv_event_t* e)
{
    status = !status;

    if(status) {
        lv_img_set_src(stopwatch_control, &wd_img_app_clock_pause);
        lv_timer_resume(s_timer);
        lv_timer_resume(ms_timer);
    } else {
        lv_img_set_src(stopwatch_control, &wd_img_app_clock_play);
        lv_timer_pause(s_timer);
        lv_timer_pause(ms_timer);
    }
}

static void stopwatch_reset_button(lv_event_t* e)
{
    lv_timer_pause(s_timer);
    lv_timer_pause(ms_timer);

    seconds_elapsed = 0;
    millisecond_elapsed = 0;

    status = false;
    if(status) {
        lv_img_set_src(stopwatch_control, &wd_img_app_clock_pause);
    } else {
        lv_img_set_src(stopwatch_control, &wd_img_app_clock_play);
    }

    lv_label_set_text_fmt(stopwatch_m, "%02u", 0);
    lv_label_set_text_fmt(stopwatch_s, "%02u", 0);
    lv_label_set_text_fmt(stopwatch_ms, "%02u", 0);
}

static void stopwatch_return(lv_event_t* e)
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
