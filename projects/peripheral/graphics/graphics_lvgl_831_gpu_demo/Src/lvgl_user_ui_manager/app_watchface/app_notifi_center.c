#include "app_watchface.h"
#include "app_notifi_center.h"
#include "app_service_simulate.h"

static lv_obj_t *p_notifi_center;

static uint32_t evt_cnt = 0;
static lv_coord_t start_x;
static lv_coord_t start_y;
static lv_coord_t origin_y;

lv_obj_t *root_screen;

#define CONFIG_NOTIFICATION_HAND_SLIDE 0

static void screen_slider_anim_cb(void *obj, uint32_t val)
{
    lv_obj_set_y(obj, val);
}

static void screen_slider_ready_cb(struct _lv_anim_t *anim)
{
    lv_obj_set_scroll_dir(root_screen, LV_DIR_VER);
}

static void screen_slider_anim_init(lv_coord_t y, bool back, lv_coord_t ori_y)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_time(&a, 500);
    lv_anim_set_var(&a, p_notifi_center);
    lv_anim_set_exec_cb(&a, screen_slider_anim_cb);
    lv_anim_set_ready_cb(&a, screen_slider_ready_cb);
    if (back)
    {
        lv_anim_set_values(&a, lv_obj_get_y(p_notifi_center), ori_y);
    }
    else
    {
        lv_anim_set_values(&a, lv_obj_get_y(p_notifi_center),
                           (-y > (LV_VER_RES / 2)) ? -LV_VER_RES : 0);
    }

    lv_anim_start(&a);
    app_time_t time = {0};
    app_watchface_get_current(&time);
    lv_label_t *ts_label = cookie_label_create(p_notifi_center, &font_18px, lv_color_make(204, 204, 204));
    lv_label_set_text_fmt(ts_label, "%02u:%02u", time.hour, time.min);
    lv_obj_set_width(ts_label, 200);
    lv_obj_align(ts_label, LV_ALIGN_TOP_MID, 0, 12);
}

static void watchface_event_cb(lv_event_t *e)
{
    lv_point_t data;
    static lv_coord_t y;
    lv_dir_t dir = 0;
    app_notice_t msg;

    if (e->user_data != NULL)
    {
        gui_watchface_screen *p_watch = e->user_data;
        if (lv_obj_get_scroll_y(p_watch->flex_screen))
        {
            return;
        }
    }

    lv_indev_get_point(lv_event_get_indev(e), &data);

    switch (e->code)
    {
    case LV_EVENT_PRESSED:
    {
        if (e->user_data)
        {
            app_time_t time = {0};
            app_watchface_get_current(&time);
            if (!app_notice_get_latest(&msg))
            {
                lv_obj_t *noMsgIcon = cookie_img_create(p_notifi_center, &wd_img_app_notice_noMsg);
                lv_obj_center(noMsgIcon);
                lv_label_t *ts_label = cookie_label_create(p_notifi_center, &font_18px, lv_color_make(204, 204, 204));
                lv_label_set_text_fmt(ts_label, "%02u:%02u", time.hour, time.min);
            }
            else
            {
                lv_label_t *notice_label1 = cookie_label_create(p_notifi_center, &dync_20px_tt, LV_COLOR_WHITE);
                lv_label_set_text(notice_label1, msg.text);
                lv_obj_set_width(notice_label1, 200);
                lv_obj_center(notice_label1);
                lv_label_t *ts_label = cookie_label_create(p_notifi_center, &font_18px, lv_color_make(204, 204, 204));
                lv_label_set_text_fmt(ts_label, "%02u:%02u", msg.notice_time.hour, msg.notice_time.min);
            }
        }
        evt_cnt = 0;
        origin_y = lv_obj_get_y(p_notifi_center);
        start_y = data.y;
        break;
    }
    case LV_EVENT_RELEASED:
    {
        screen_slider_anim_init(y, abs(y) < 100, origin_y);
        evt_cnt = 0;
        break;
    }
    case LV_EVENT_PRESSING:
    {
        evt_cnt++;
        y = data.y - start_y;
        if (y > 0) // 下拉
        {
            lv_obj_set_scroll_dir(root_screen, LV_DIR_NONE);
        }
        else // 上拉
        {
            if (e->user_data)
            {
                return;
            }
        }

        lv_coord_t move_y = e->user_data ? -LV_VER_RES + y : y;
        if (move_y > 0)
            move_y = 0;
        lv_obj_set_y(p_notifi_center, move_y);
        break;
    }
    default:
        break;
    }
}

lv_obj_t *cookie_create_notifi_center(lv_obj_t *root_page, gui_watchface_screen *p_watch, lv_obj_t *part_screen)
{
    root_screen = part_screen;

    p_notifi_center = lv_obj_create(root_page);
    lv_obj_set_style_bg_opa(p_notifi_center, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(p_notifi_center, lv_color_black(), NULL);
    lv_obj_set_size(p_notifi_center, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_scroll_dir(p_notifi_center, LV_DIR_NONE);
    lv_obj_set_y(p_notifi_center, -LV_VER_RES);

    lv_obj_add_event_cb(p_watch->watchface, watchface_event_cb, LV_EVENT_PRESSED, p_watch);
    lv_obj_add_event_cb(p_watch->watchface, watchface_event_cb, LV_EVENT_PRESSING, p_watch);
    lv_obj_add_event_cb(p_watch->watchface, watchface_event_cb, LV_EVENT_RELEASED, p_watch);
    lv_obj_add_event_cb(p_notifi_center, watchface_event_cb, LV_EVENT_PRESSED, 0);
    lv_obj_add_event_cb(p_notifi_center, watchface_event_cb, LV_EVENT_PRESSING, 0);
    lv_obj_add_event_cb(p_notifi_center, watchface_event_cb, LV_EVENT_RELEASED, 0);
}