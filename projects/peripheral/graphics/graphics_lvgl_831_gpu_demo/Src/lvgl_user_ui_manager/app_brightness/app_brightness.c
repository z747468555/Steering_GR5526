#include "app_brightness.h"
#include "app_container.h"
#include "app_service_simulate.h"
// Cookie is ok
static lv_obj_t* level = NULL;

static void brightness_change(lv_event_t* e);
static void brightness_return(lv_event_t* e);

lv_obj_t* brightness_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, brightness_return, LV_EVENT_GESTURE, NULL);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "屏幕调节");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    level = cookie_label_create(screen, &font_16px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(level, "亮度(%d级)", app_brightness_get_percent());
    lv_obj_align(level, LV_ALIGN_TOP_MID, 0, 90);

    lv_obj_t* slider = lv_slider_create(screen);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, LV_COLOR_GRAY, LV_PART_MAIN);
    lv_obj_set_style_radius(slider, 24, LV_PART_MAIN);

    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, LV_COLOR_WHITE, LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, 24, LV_PART_INDICATOR);

    lv_obj_remove_style(slider, NULL, LV_PART_KNOB);

    lv_slider_set_mode(slider, LV_SLIDER_MODE_SYMMETRICAL);
    lv_obj_clear_flag(slider, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_size(slider, 280, 60);

    lv_slider_set_range(slider, 1, 12);
    lv_slider_set_value(slider, app_brightness_get_percent(), LV_ANIM_OFF);
    lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 150);

    lv_obj_add_event_cb(slider, brightness_change, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t* img = cookie_img_create(slider, &wd_img_app_brightness_icon);
    lv_obj_align(img, LV_ALIGN_LEFT_MID, 10, 0);

    return screen;
}

void brightness_release(void)
{
    level = NULL;
}

static void brightness_change(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_RELEASED) {
        lv_indev_wait_release(lv_indev_get_act());
    }

    lv_obj_t* slider = lv_event_get_target(e);
    int32_t brightness = lv_slider_get_value(slider);

    lv_label_set_text_fmt(level, "亮度(%d级)", (uint8_t)brightness);
    app_brightness_set_percent((uint8_t)brightness);
}

static void brightness_return(lv_event_t* e)
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
