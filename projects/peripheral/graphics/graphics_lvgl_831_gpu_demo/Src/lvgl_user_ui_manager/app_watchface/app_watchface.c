#include "app_watchface.h"
#include "app_service_simulate.h"
#include "ui_win_message.h"
#include "app_container.h"
#include "app_msg/app_msg.h"

static gui_watchface_screen* p_watch;
app_time_t time = {0};
app_sport_t sport = {0};

static void show_update_cb(lv_event_t *e)
{
    if(!p_watch) {
        return;
    }

    if (e->code == LV_EVENT_USER) {
        ser_event_msg_t* data = e->param;

        switch (data->msg_type)
        {
            case EVENT_WATCHFACE_UPDATA:
                app_watchface_get_current(&time);
                lv_label_set_text_fmt(p_watch->time_show, "%02u:%02u:%02u", time.hour, time.min, time.sec);
                lv_label_set_text_fmt(p_watch->month_day, "%02u-%02u", time.month, time.day);
                for(uint8_t i = 0; i < 7; i++)
                {
                    if(i == time.week) {
                        lv_obj_set_style_text_color(p_watch->week[i], LV_COLOR_BLUE, LV_PART_MAIN);
                    } else {
                        lv_obj_set_style_text_color(p_watch->week[i], LV_COLOR_WHITE, LV_PART_MAIN);
                    }
                }
                break;

            case EVENT_ELETRICITY_CHANGE:
                lv_label_set_text_fmt(p_watch->eletricity, "%u%%", app_environment_get_power());
                if(app_environment_get_power() > 20)
                {
                    lv_obj_set_style_bg_color(p_watch->charger, LV_COLOR_GREEN, LV_PART_INDICATOR);
                }
                else
                {
                    lv_obj_set_style_bg_color(p_watch->charger, LV_COLOR_RED, LV_PART_INDICATOR);
                }
                lv_bar_set_value(p_watch->charger, app_environment_get_power(), LV_ANIM_OFF);
                break;

            case EVENT_CHARGER_CHANGE:
                if(app_environment_get_charge_status()) {
                    lv_obj_add_flag(p_watch->charger, LV_OBJ_FLAG_HIDDEN);
                } else {
                    lv_obj_clear_flag(p_watch->charger, LV_OBJ_FLAG_HIDDEN);
                    if(app_environment_get_power() > 20)
                    {
                        lv_obj_set_style_bg_color(p_watch->charger, LV_COLOR_GREEN, LV_PART_INDICATOR);
                    }
                    else
                    {
                        lv_obj_set_style_bg_color(p_watch->charger, LV_COLOR_RED, LV_PART_INDICATOR);
                    }
                    lv_bar_set_value(p_watch->charger, app_environment_get_power(), LV_ANIM_OFF);
                }
                break;

            case EVENT_HR_CHANGE:
                if(app_heart_rate_get_current() == 0 || app_heart_rate_get_current() == 255) {
                    lv_label_set_text_fmt(p_watch->hr, "--");
                } else {
                    lv_label_set_text_fmt(p_watch->hr, "%u", app_heart_rate_get_current());
                }
                break;

            case EVENT_SPORT_CHANGE:
                app_sport_get_data(&sport);
                lv_label_set_text_fmt(p_watch->step, "%u", sport.step);
                break;

            case EVENT_UPPER_RIGHT_KEY_CLICK:
                lv_obj_scroll_to_view(p_watch->watchface, LV_ANIM_OFF);
                break;

            default:
                break;
        }
    }
}



void ui_watchface_gesture_event_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_GESTURE)
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_event_get_indev(e));
        if (dir == LV_DIR_TOP)
        {
            if (p_watch->msg_cont_show)
            {
                release_msg_snap();
                lv_obj_del(p_watch->msg_img);
                p_watch->msg_img = NULL;
                p_watch->msg_cont_show = 0;
                return;
            }
            SCREEN_CALL(SCREEN_ID_APPLIST,0,0);
        }
        else if (dir == LV_DIR_BOTTOM)
        {

        }
        lv_indev_wait_release(lv_indev_get_act());
    }
}

void ui_watchface_throw_event_cb(lv_event_t *e)
{
    lv_point_t data;
    lv_dir_t dir = 0;
    static lv_coord_t start_y = 0;

    lv_indev_get_point(lv_event_get_indev(e), &data);

    if (e->code == LV_EVENT_PRESSED)
    {
        start_y = data.y;

        if ((start_y < 40) && (p_watch->msg_cont_status == MSG_CONT_STATUS_NONE))
        {
            update_msg_snap();
            p_watch->msg_cont_status = MSG_CONT_STATUS_IN_PROCESS;
        }

        if (!p_watch->msg_img && (p_watch->msg_cont_status == MSG_CONT_STATUS_IN_PROCESS))
        {
            p_watch->msg_img = cookie_img_create(p_watch->screen,get_msg_snap_dsc());
            lv_obj_set_style_bg_color(p_watch->msg_img,lv_color_black(),LV_PART_MAIN);
            lv_obj_set_style_bg_opa(p_watch->msg_img, LV_OPA_COVER, LV_PART_MAIN);
            lv_obj_set_pos(p_watch->msg_img,0,-LV_VER_RES);
            p_watch->msg_cont_show = 1;
        }
    }
    else if (e->code == LV_EVENT_PRESSING)
    {
        if (p_watch->msg_cont_status)
        {
            lv_obj_set_y(p_watch->msg_img,data.y-LV_VER_RES);
            return;
        }
        else if (start_y > 180 && ((start_y - data.y) > 100))
        {
            SCREEN_CALL(SCREEN_ID_APPLIST,0,0);
            p_watch->msg_cont_status = MSG_CONT_STATUS_NONE;
            lv_indev_wait_release(lv_indev_get_act());
        }
    }
    else if (e->code == LV_EVENT_RELEASED)
    {
        if (p_watch->msg_cont_status)
        {
            if (data.y < 180)
            {
                lv_obj_set_pos(p_watch->msg_img,0,-LV_VER_RES);
                lv_obj_del(p_watch->msg_img);
                p_watch->msg_img = NULL;
                p_watch->msg_cont_show = 0;
                lv_obj_invalidate(p_watch->screen);
                release_msg_snap();
                p_watch->msg_cont_status = MSG_CONT_STATUS_NONE;
            }
            else
            {
                lv_obj_set_pos(p_watch->msg_img,0,0);
                p_watch->msg_cont_status = MSG_CONT_STATUS_IN_SHOWING;
                p_watch->msg_img = NULL;
                p_watch->msg_cont_show = 0;;
                release_msg_snap();
                SCREEN_CALL(SCREEN_ID_MSG, EVENT_TEXT_LATEST_EXT, 0);
            }
        }
        start_y = 0;
    }
}

lv_obj_t *ui_watchface_page_init(uint32_t param)
{
    if(!p_watch)
    {
        p_watch = lv_mem_alloc(sizeof(gui_watchface_screen));
        LV_ASSERT_MALLOC(p_watch);
        lv_memset_00(p_watch, sizeof(gui_watchface_screen));
    }

    p_watch->screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_NONE);
    lv_obj_add_event_cb(p_watch->screen, show_update_cb, LV_EVENT_USER, NULL);//接收message消息必须要以screen作为obj
    lv_obj_add_event_cb(p_watch->screen, ui_watchface_throw_event_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(p_watch->screen, ui_watchface_throw_event_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb(p_watch->screen, ui_watchface_throw_event_cb, LV_EVENT_RELEASED, NULL);

    //lv_obj_add_event_cb(p_watch->screen, ui_watchface_gesture_event_cb, LV_EVENT_GESTURE, NULL);



    lv_obj_t* watchface_map = cookie_img_create(p_watch->screen, &wd_img_app_watchface_bg);
    lv_obj_center(watchface_map);

    p_watch->charger = lv_bar_create(p_watch->screen);
    lv_obj_set_style_bg_opa(p_watch->charger, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(p_watch->charger, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(p_watch->charger, LV_COLOR_WHITE, LV_PART_MAIN);
    if(app_environment_get_power() > 20)
    {
        lv_obj_set_style_bg_color(p_watch->charger, LV_COLOR_GREEN, LV_PART_INDICATOR);
    }
    else
    {
        lv_obj_set_style_bg_color(p_watch->charger, LV_COLOR_RED, LV_PART_INDICATOR);
    }

    lv_obj_set_size(p_watch->charger, 10, 18);
    lv_obj_set_pos(p_watch->charger, 90, 219);
    lv_bar_set_value(p_watch->charger, app_environment_get_power(), LV_ANIM_OFF);

    if(app_environment_get_charge_status()) {
        lv_obj_add_flag(p_watch->charger, LV_OBJ_FLAG_HIDDEN);
    }

    app_watchface_get_current(&time);

    p_watch->time_show = cookie_label_create(p_watch->screen, &font_62px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(p_watch->time_show, "%02u:%02u:%02u", time.hour, time.min, time.sec);
    lv_obj_set_size(p_watch->time_show, 240, 82);
    lv_obj_align(p_watch->time_show, LV_ALIGN_TOP_MID, 0, 116);

    char* week[7] = {"日","一","二","三","四","五","六"};
    for(uint8_t i = 0; i < 7; i++)
    {
        p_watch->week[i] = cookie_label_create(p_watch->screen, &font_16px, LV_COLOR_WHITE);
        lv_label_set_text_fmt(p_watch->week[i], "%s", week[i]);
        lv_obj_align_to(p_watch->week[i],  p_watch->time_show, LV_ALIGN_OUT_TOP_LEFT, 22 + i * 30, 0);

        if(i == time.week) {
            lv_obj_set_style_text_color(p_watch->week[i], LV_COLOR_BLUE, LV_PART_MAIN);
        }
    }

    p_watch->month_day = cookie_label_create(p_watch->screen, &font_24px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(p_watch->month_day, "%02u-%02u", time.month, time.day);
    lv_obj_align_to(p_watch->month_day, p_watch->time_show, LV_ALIGN_OUT_TOP_MID, 0, -50);

    p_watch->eletricity = cookie_label_create(p_watch->screen, &font_16px, LV_COLOR_BLUE);
    lv_label_set_text_fmt(p_watch->eletricity, "%u%%", app_environment_get_power());
    lv_obj_align_to(p_watch->eletricity, p_watch->time_show, LV_ALIGN_OUT_BOTTOM_LEFT, 26, 40);

    p_watch->hr = cookie_label_create(p_watch->screen, &font_16px, LV_COLOR_BLUE);
    if(app_heart_rate_get_current() == 0 || app_heart_rate_get_current() == 255) {
        lv_label_set_text_fmt(p_watch->hr, "--");
    } else {
        lv_label_set_text_fmt(p_watch->hr, "%u", app_heart_rate_get_current());
    }
    lv_obj_align_to(p_watch->hr, p_watch->time_show, LV_ALIGN_OUT_BOTTOM_MID, 6, 12);

    app_sport_get_data(&sport);
    p_watch->step = cookie_label_create(p_watch->screen, &font_16px, LV_COLOR_BLUE);
    lv_label_set_text_fmt(p_watch->step, "%u", sport.step);
    lv_obj_align_to(p_watch->step, p_watch->time_show, LV_ALIGN_OUT_BOTTOM_RIGHT, -40, 40);

    return p_watch->screen;
}

void ui_watchface_page_release(void)
{
    lv_mem_free(p_watch);
    p_watch = NULL;
}


