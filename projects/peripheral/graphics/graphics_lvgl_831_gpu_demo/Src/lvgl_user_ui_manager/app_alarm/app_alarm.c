#include "app_alarm.h"
#include "app_service_simulate.h"

static lv_obj_t* screen = NULL;
static lv_obj_t* roller_h = NULL;
static lv_obj_t* roller_m = NULL;
static app_alarm_clock_t alarm_set = {0};

static void alarm_return(lv_event_t* e);
static void alarm_delete_button(lv_event_t* e);
static void alarm_add_button(lv_event_t* e);
static void alarm_set_time_confirm_button(lv_event_t* e);
static void alarm_set_repeat_confirm_button(lv_event_t* e);
static void alarm_show_create();
static void alarm_set_time_create();
static void alarm_set_repeat_create();

lv_obj_t* alarm_init(uint32_t param)
{
    screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, alarm_return, LV_EVENT_GESTURE, NULL);

    alarm_show_create();

    return screen;
}

void alarm_release(void)
{
    screen = NULL;
}

static void alarm_show_create()
{
    lv_obj_clean(screen);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "闹钟");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    app_alarm_clock_list_t clock_list = {0};
    gui_alarm_clock_get_list(&clock_list);

    if(clock_list.number == 0) {
        lv_obj_t* nothing_img = cookie_img_create(screen, &wd_img_app_clock_noalarm);
        lv_obj_center(nothing_img);
    } else {
        for(uint8_t i = 0; i < clock_list.number; i++) {
            lv_obj_t* item = cookie_alarm_item_create(screen, clock_list.list[i].min, clock_list.list[i].hour, clock_list.list[i].repeat);
            lv_obj_align(item, LV_ALIGN_TOP_MID, 0, 94 + i * 56);
        }
    }

    // if(clock_list.number == 0) {
    //     lv_obj_t* add_img = cookie_img_create(page, &wd_img_app_clock_add);
    //     lv_obj_add_event_cb(add_img, alarm_add_button, LV_EVENT_CLICKED, NULL);
    //     lv_obj_add_flag(add_img, LV_OBJ_FLAG_CLICKABLE);
    //     lv_obj_align(add_img, LV_ALIGN_TOP_MID, 0, 228);
    // } else if (clock_list.number == 3) {
    //     lv_obj_t* delete_img = cookie_img_create(page, &wd_img_app_clock_delete);
    //     lv_obj_add_event_cb(delete_img, alarm_delete_button, LV_EVENT_CLICKED, NULL);
    //     lv_obj_add_flag(delete_img, LV_OBJ_FLAG_CLICKABLE);
    //     lv_obj_align(delete_img, LV_ALIGN_TOP_MID, 0, 228);
    // } else {
    //     lv_obj_t* add_img = cookie_img_create(page, &wd_img_app_clock_add);
    //     lv_obj_add_event_cb(add_img, alarm_add_button, LV_EVENT_CLICKED, NULL);
    //     lv_obj_add_flag(add_img, LV_OBJ_FLAG_CLICKABLE);
    //     lv_obj_align(add_img, LV_ALIGN_TOP_MID, 0, 178);

    //     lv_obj_t* delete_img = cookie_img_create(page, &wd_img_app_clock_delete);
    //     lv_obj_add_event_cb(delete_img, alarm_delete_button, LV_EVENT_CLICKED, NULL);
    //     lv_obj_add_flag(delete_img, LV_OBJ_FLAG_CLICKABLE);
    //     lv_obj_align(delete_img, LV_ALIGN_TOP_MID, 0, 228);
    // }

    if(clock_list.number == 3) {

        lv_obj_t* delete_img = cookie_img_create(screen, &wd_img_app_clock_delete);
        lv_obj_add_event_cb(delete_img, alarm_delete_button, LV_EVENT_CLICKED, NULL);
        lv_obj_add_flag(delete_img, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_align(delete_img, LV_ALIGN_TOP_MID, 0, 298);
        lv_obj_set_ext_click_area(delete_img, 20);

    } else {
        lv_obj_t* add_img = cookie_img_create(screen, &wd_img_app_clock_add);
        lv_obj_add_event_cb(add_img, alarm_add_button, LV_EVENT_CLICKED, NULL);
        lv_obj_add_flag(add_img, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_align(add_img, LV_ALIGN_TOP_MID, 0, 298);
        lv_obj_set_ext_click_area(add_img, 20);

    }
}

static void alarm_add_button(lv_event_t* e)
{
    alarm_set_time_create();
}


static void alarm_set_time_create()
{
    lv_obj_clean(screen);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "闹钟设置");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    app_time_t current_time = {0};
    app_watchface_get_current(&current_time);

    roller_h = cookie_roller_create(screen, 174, 0, 23, 1, current_time.hour);
    lv_obj_align(roller_h, LV_ALIGN_TOP_LEFT, 0, 100);

    roller_m = cookie_roller_create(screen, 174, 0, 59, 1, current_time.min);
    lv_obj_align(roller_m, LV_ALIGN_TOP_RIGHT, 0, 100);

    lv_obj_t* dot_1 = cookie_label_create(screen, &font_30px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(dot_1, "%s", ":");
    lv_obj_align(dot_1, LV_ALIGN_TOP_MID, 0, 160);

    lv_obj_t* confirm = cookie_img_create(screen, &wd_img_app_clock_confirm);
    lv_obj_add_event_cb(confirm, alarm_set_time_confirm_button, LV_EVENT_CLICKED, NULL);
    lv_obj_align(confirm, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_flag(confirm, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(confirm, 20);
}

static void alarm_delete_button(lv_event_t* e)
{
    for(uint8_t i = 0; i < 3; i++) {
        gui_alarm_clock_delete(i);
    }

    alarm_show_create();
}

static lv_obj_t* once_confirm = NULL;
static lv_obj_t* repeat_confirm = NULL;
static bool repeat_status = false;

static void item_once_confirm_button(lv_event_t* e)
{
    repeat_status = !repeat_status;
    if(repeat_status) {
        lv_img_set_src(once_confirm, &wd_img_app_clock_unselected);
        lv_img_set_src(repeat_confirm, &wd_img_app_clock_selected);
    } else {
        lv_img_set_src(once_confirm, &wd_img_app_clock_selected);
        lv_img_set_src(repeat_confirm, &wd_img_app_clock_unselected);
    }

}

static void alarm_set_repeat_create()
{
    lv_obj_clean(screen);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "闹钟设置");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    lv_obj_t* item_once = cookie_obj_create(screen, 300, 60, LV_DIR_NONE);
    lv_obj_set_style_bg_color(item_once, LV_COLOR_GRAY, LV_PART_MAIN);
    lv_obj_set_style_radius(item_once, 24, LV_PART_MAIN);
    lv_obj_align(item_once, LV_ALIGN_TOP_MID, 0, 94);
    lv_obj_add_event_cb(item_once, item_once_confirm_button, LV_EVENT_CLICKED, NULL);

    lv_obj_t* once_label = cookie_label_create(item_once, &font_20px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(once_label, "%s", "单次");
    lv_obj_align(once_label, LV_ALIGN_LEFT_MID, 20, 0);

    once_confirm = cookie_img_create(item_once, &wd_img_app_clock_unselected);
    lv_obj_align(once_confirm, LV_ALIGN_RIGHT_MID, -20, 0);

    lv_obj_t* item_repeat = cookie_obj_create(screen, 300, 60, LV_DIR_NONE);
    lv_obj_set_style_bg_color(item_repeat, LV_COLOR_GRAY, LV_PART_MAIN);
    lv_obj_set_style_radius(item_repeat, 24, LV_PART_MAIN);
    lv_obj_align(item_repeat, LV_ALIGN_TOP_MID, 0, 168);
    lv_obj_add_event_cb(item_repeat, item_once_confirm_button, LV_EVENT_CLICKED, NULL);

    lv_obj_t* repeat_label = cookie_label_create(item_repeat, &font_20px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(repeat_label, "%s", "重复");
    lv_obj_align(repeat_label, LV_ALIGN_LEFT_MID, 20, 0);

    repeat_confirm = cookie_img_create(item_repeat, &wd_img_app_clock_unselected);
    lv_obj_align(repeat_confirm, LV_ALIGN_RIGHT_MID, -20, 0);

    lv_obj_t* confirm = cookie_img_create(screen, &wd_img_app_clock_confirm);
    lv_obj_add_event_cb(confirm, alarm_set_repeat_confirm_button, LV_EVENT_CLICKED, NULL);
    lv_obj_align(confirm, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_flag(confirm, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(confirm, 20);

}

static void alarm_set_time_confirm_button(lv_event_t* e)
{
    alarm_set.hour = cookie_get_roller_data(roller_h);
    alarm_set.min = cookie_get_roller_data(roller_m);

    alarm_set_repeat_create();
}

static void alarm_set_repeat_confirm_button(lv_event_t* e)
{
    alarm_set.repeat = repeat_status;
    gui_alarm_clock_add(&alarm_set);
    alarm_show_create();
}

static void alarm_return(lv_event_t* e)
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
