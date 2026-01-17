#include "app_shutdown.h"
// Cookie is ok

static void shutdown_return(lv_event_t* e);
static void shutdown_confirm_button(lv_event_t* e);
static void shutdown_delete_button(lv_event_t* e);
static lv_obj_t* screen = NULL;
static lv_timer_t* timer = NULL;
static lv_obj_t* arc = NULL;
static lv_obj_t* countdown_text = NULL;
static uint16_t shutdown = 0;

lv_obj_t* shutdown_init(uint32_t param)
{
    screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, shutdown_return, LV_EVENT_GESTURE, NULL);

    lv_obj_t* tips_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
    lv_obj_set_width(tips_text, LV_HOR_RES);
    lv_label_set_text_fmt(tips_text, "%s", "您确定要关机吗？");
    lv_obj_align(tips_text,  LV_ALIGN_TOP_MID, 0, 140);

    lv_obj_t* confirm = cookie_img_create(screen, &wd_img_app_clock_confirm);
    lv_obj_add_event_cb(confirm, shutdown_confirm_button, LV_EVENT_CLICKED, NULL);
    lv_obj_align(confirm, LV_ALIGN_TOP_LEFT, 60, 186);
    lv_obj_add_flag(confirm, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t* delete = cookie_img_create(screen, &wd_img_app_shutdown_cancel);
    lv_obj_add_event_cb(delete, shutdown_delete_button, LV_EVENT_CLICKED, NULL);
    lv_obj_align(delete, LV_ALIGN_TOP_RIGHT, -60, 186);
    lv_obj_add_flag(delete, LV_OBJ_FLAG_CLICKABLE);

    return screen;
}

void shutdown_release(void)
{
    screen = NULL;
    timer = NULL;
    arc = NULL;
    countdown_text = NULL;
    shutdown = 0;
}

static void shutdown_progress(lv_timer_t* timer)
{
    shutdown ++;
    lv_label_set_text_fmt(countdown_text, "%d", shutdown / 33);
    lv_arc_set_value(arc, shutdown);

    if(shutdown == 100) {
        app_screen_keep_on(false);
        gui_shutdown_launch();
    }
}

static void shutdown_confirm_button(lv_event_t* e)
{
    lv_obj_clean(screen);

    countdown_text = cookie_label_create(screen, &font_36px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(countdown_text, "%d%%", 0);
    lv_obj_center(countdown_text);

    arc = lv_arc_create(screen);
    lv_obj_set_style_arc_opa(arc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, LV_COLOR_GRAY_WHITE, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 6, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(arc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, LV_COLOR_BLUE, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 6, LV_PART_INDICATOR);
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_arc_set_angles(arc, 0, 360);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(arc, 130, 130);
    lv_obj_center(arc);

    lv_obj_t* tips_text = cookie_label_create(screen, &font_24px, LV_COLOR_WHITE);
    lv_obj_set_width(tips_text, LV_HOR_RES);
    lv_label_set_text_fmt(tips_text, "%s", "关机中");
    lv_obj_align_to(tips_text, arc, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    timer = lv_timer_create(shutdown_progress, 15, NULL);
    app_screen_keep_on(true);
}

static void shutdown_delete_button(lv_event_t* e)
{
    SCREEN_RETURN(0, 0);
}

static void shutdown_return(lv_event_t* e)
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
