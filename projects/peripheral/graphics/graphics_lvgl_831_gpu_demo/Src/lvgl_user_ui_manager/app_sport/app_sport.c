#include "app_sport.h"
#include "app_service_simulate.h"
#include "ui_win_message.h"
// Cookie is ok

typedef struct _gui_sport_item_t
{
    lv_obj_t* item;
    const void* image;
    const char* name;
    const char* unit;
    uint32_t data;

} gui_sport_item;

static gui_sport_item sport_list[3] =
{
    {NULL, &wd_img_app_sport_steps,     "步数", "步", 0},
    {NULL, &wd_img_app_sport_distances, "距离", "米", 0},
    {NULL, &wd_img_app_sport_consume,   "消耗", "卡", 0},
};

static void sport_return(lv_event_t* e);
static void show_update_cb(lv_event_t* e);

static void show_update_cb(lv_event_t* e)
{
    if (e->code == LV_EVENT_USER) {
        ser_event_msg_t* data = e->param;

        app_sport_t sport_data = {0};
        app_sport_get_data(&sport_data);

        switch (data->msg_type)
        {
            case EVENT_SPORT_CHANGE:
                cookie_sport_item_set_data(sport_list[0].item, sport_data.step);
                cookie_sport_item_set_data(sport_list[1].item, sport_data.distance);
                cookie_sport_item_set_data(sport_list[2].item, sport_data.calorie);
                break;

            default:
                break;
        }
    }

}

lv_obj_t* sport_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, sport_return, LV_EVENT_GESTURE, NULL);
    lv_obj_add_event_cb(screen, show_update_cb, LV_EVENT_USER, NULL);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_top(screen, 30, LV_PART_MAIN);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, LV_HOR_RES);
    lv_label_set_text_fmt(title_text, "%s", "运动监测");

    app_sport_t sport_data = {0};
    app_sport_get_data(&sport_data);

    sport_list[0].data = sport_data.step;
    sport_list[1].data = sport_data.distance;
    sport_list[2].data = sport_data.calorie;

    for(uint8_t i = 0; i < 3; i++)
    {
        sport_list[i].item = cookie_sport_item_create(screen, sport_list[i].image, sport_list[i].name, sport_list[i].unit, sport_list[i].data);
    }

    return screen;
}

void sport_release(void)
{
    for(uint8_t i = 0; i < 3; i++)
    {
        sport_list[i].item = NULL;
        sport_list[i].data = 0;
    }
}

static void sport_return(lv_event_t* e)
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
