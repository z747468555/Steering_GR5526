#include "app_sos.h"
#include "app_service_simulate.h"
#include "app_container.h"
#include "ui_win_message.h"
// Cookie is ok

static lv_obj_t* button = NULL;
static lv_obj_t* button_label = NULL;

static void sos_return(lv_event_t* e);
static void sos_button(lv_event_t* e);

static void show_update_cb(lv_event_t *e)
{
    if(button == NULL || button_label == NULL){
        return;
    }

    if (e->code == LV_EVENT_USER) {
        ser_event_msg_t* data = e->param;
        switch (data->msg_type)
        {
            case EVENT_UPPER_LEFT_KEY_DBLCLICK:
                if(!app_sos_get_status()) {
                    lv_obj_set_style_bg_color(button, LV_COLOR_GRAY,LV_PART_MAIN);
                    lv_label_set_text_fmt(button_label, "%s", "关闭报警");
                    app_sos_set_status(true,false);
                }
                break;

            case EVENT_SOS_ALARM_STOP:
                if(app_sos_get_status()) {
                    lv_obj_set_style_bg_color(button, LV_COLOR_GRAY,LV_PART_MAIN);
                    lv_label_set_text_fmt(button_label, "%s", "关闭报警");
                } else {
                    lv_obj_set_style_bg_color(button, LV_COLOR_RED,LV_PART_MAIN);
                    lv_label_set_text_fmt(button_label, "%s", "开启报警");
                }
                break;

            default:
                break;
        }
    }
}

lv_obj_t* sos_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, sos_return, LV_EVENT_GESTURE, NULL);
    lv_obj_add_event_cb(screen, show_update_cb, LV_EVENT_USER, NULL);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "SOS报警");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    lv_obj_t* tips_img = cookie_img_create(screen, &wd_img_app_alarm_SOS_icon);
    lv_obj_center(tips_img);

    button = cookie_obj_create(screen, 122, 48, LV_DIR_NONE);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(button, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_style_radius(button, 24, LV_PART_MAIN);
    lv_obj_align(button, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(button, sos_button, LV_EVENT_CLICKED, NULL);
    lv_obj_set_ext_click_area(button, 18);

    button_label = cookie_label_create(button, &font_20px, LV_COLOR_WHITE);
    lv_obj_add_flag(button_label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(button_label, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_width(button_label, 122);

    lv_obj_center(button_label);

    bool status = app_sos_get_status();

    if(status) {
        lv_obj_set_style_bg_color(button, LV_COLOR_GRAY,LV_PART_MAIN);
        lv_label_set_text_fmt(button_label, "%s", "关闭报警");
    } else {
        lv_obj_set_style_bg_color(button, LV_COLOR_RED,LV_PART_MAIN);
        lv_label_set_text_fmt(button_label, "%s", "开启报警");
    }

    return screen;
}

void sos_release(void)
{
    button = NULL;
    button_label = NULL;
}

static void sos_button(lv_event_t* e)
{
    bool status = app_sos_get_status();
    status = !status;

    if(status) {
        lv_obj_set_style_bg_color(button, LV_COLOR_GRAY,LV_PART_MAIN);
        lv_label_set_text_fmt(button_label, "%s", "关闭报警");
    } else {
        lv_obj_set_style_bg_color(button, LV_COLOR_RED,LV_PART_MAIN);
        lv_label_set_text_fmt(button_label, "%s", "开启报警");
    }

    app_sos_set_status(status,false);
}

static void sos_return(lv_event_t* e)
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
