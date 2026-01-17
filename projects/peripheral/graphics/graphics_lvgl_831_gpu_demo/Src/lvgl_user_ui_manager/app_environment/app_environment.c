#include "app_environment.h"
#include "app_service_simulate.h"
#include "ui_win_message.h"
// Cookie is ok

typedef struct _gui_environment_item_t
{
    lv_obj_t*   item;
    const void* image;
    const char* unit;

} gui_environment_item;

gui_environment_item environment_list[3] =
{
    {NULL, &wd_img_app_env_press,      "气压(kPa)"},
    {NULL, &wd_img_app_env_temp,       "温度(°)"},
    {NULL, &wd_img_app_env_brightness, "亮度(级)"},
};

static void environment_return(lv_event_t* e);

static void show_update_cb(lv_event_t* e)
{
    if (e->code == LV_EVENT_USER) {
        ser_event_msg_t* data = e->param;

        app_environment_t environment_data = {0};
        app_environment_get_data(&environment_data);

        switch (data->msg_type)
        {
            case EVENT_ENVIRONMENT_CHANGE:
                cookie_environment_item_set_data(environment_list[0].item, environment_data, 0);
                cookie_environment_item_set_data(environment_list[1].item, environment_data, 1);
                cookie_environment_item_set_data(environment_list[2].item, environment_data, 2);
                break;

            default:
                break;
        }
    }

}

lv_obj_t* environment_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, show_update_cb, LV_EVENT_USER, NULL);
    lv_obj_add_event_cb(screen, environment_return, LV_EVENT_GESTURE, NULL);

    lv_obj_t* bg_img = cookie_img_create(screen, &wd_img_app_env_bg);
    lv_obj_center(bg_img);

    app_environment_t environment_data = {0};
    app_environment_get_data(&environment_data);

    for(uint8_t i = 0; i < 3; i++)
    {
        environment_list[i].item = cookie_environment_item_create(screen, environment_list[i].image, environment_list[i].unit, environment_data, i);

        switch (i)
        {
        case 0:
            lv_obj_align(environment_list[i].item, LV_ALIGN_TOP_MID, 0, 40);
            break;

        case 1:
            lv_obj_align(environment_list[i].item, LV_ALIGN_TOP_LEFT, 46, 178);
            break;

        case 2:
            lv_obj_align(environment_list[i].item, LV_ALIGN_TOP_RIGHT, -46, 178);
            break;

        default:
            break;
        }
    }

    return screen;
}

void environment_release(void)
{
    for(uint8_t i = 0; i < 3; i++)
    {
        environment_list[i].item = NULL;
    }
}

static void environment_return(lv_event_t* e)
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
