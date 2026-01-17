#include "app_msg.h"
#include "app_service_simulate.h"

#define APP_LOG_TAG "ui"
#include "app_log.h"

app_notice_t msg;
uint32_t app_notice_get_latest_ret;

lv_obj_t *snap_buf = NULL;
lv_img_dsc_t msg_dsc;
uint32_t snap_buf_size = 0;


static void app_msg_return(lv_event_t* e);

static void alarm_msg_contanier(lv_obj_t *screen)
{
    lv_obj_t* img_alarm = cookie_img_create(screen, &wd_img_app_msg_alarm_icon);
    lv_obj_center(img_alarm);

    lv_obj_t* img_close_btn = cookie_img_create(screen, &wd_img_app_msg_alarm_close);
    lv_obj_align(img_close_btn,LV_ALIGN_BOTTOM_MID,0,-20);
    lv_obj_add_flag(img_close_btn,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(img_close_btn,app_msg_return,LV_EVENT_CLICKED,(void*)EVENT_ALARM_CLOCK);
    lv_obj_set_ext_click_area(img_close_btn, 20);
}

static void text_msg_contanier(lv_obj_t *screen)
{
    lv_obj_t* img_close_btn = cookie_img_create(screen, &wd_img_app_notice_received);
    lv_obj_align(img_close_btn,LV_ALIGN_BOTTOM_MID,0,-20);
    lv_obj_add_flag(img_close_btn,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(img_close_btn,app_msg_return,LV_EVENT_CLICKED,(void*)EVENT_TEXT_LATEST);
    lv_obj_set_ext_click_area(img_close_btn, 20);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "通知");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 38);

	app_notice_get_latest_ret = app_notice_get_latest(&msg);

    if (app_notice_get_latest_ret)
    {
        lv_label_t *notice_label1 = cookie_label_create(screen, &dync_20px_tt, LV_COLOR_WHITE);
        lv_label_set_text(notice_label1, msg.text);
        lv_obj_set_size(notice_label1, 200, LV_SIZE_CONTENT);
        lv_label_set_long_mode(notice_label1, LV_LABEL_LONG_WRAP);
        lv_obj_center(notice_label1);
        lv_label_t *ts_label = cookie_label_create(screen, &font_18px, lv_color_make(204, 204, 204));
        lv_label_set_text_fmt(ts_label, "%02u:%02u", msg.notice_time.hour, msg.notice_time.min);
    }
}

static void  text_msg_contanier_ext(lv_obj_t *screen)
{
    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "提醒中心");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 38);

    app_time_t time = {0};
    app_watchface_get_current(&time);

    app_notice_get_latest_ret = app_notice_get_latest(&msg);

    if (!app_notice_get_latest_ret)
    {
        lv_obj_t *noMsgIcon = cookie_img_create(screen, &wd_img_app_notice_noMsg);
        lv_obj_center(noMsgIcon);
        lv_label_t *ts_label = cookie_label_create(screen, &font_18px, lv_color_make(204, 204, 204));
        lv_label_set_text_fmt(ts_label, "%02u:%02u", time.hour, time.min);
    }
    else
    {
        lv_label_t *notice_label1 = cookie_label_create(screen, &dync_20px_tt, LV_COLOR_WHITE);
        lv_label_set_text(notice_label1, msg.text);
        printf("Receive msg : %s \n",msg.text);
        lv_obj_set_size(notice_label1, 200, LV_SIZE_CONTENT);
        lv_label_set_long_mode(notice_label1, LV_LABEL_LONG_WRAP);
        lv_obj_center(notice_label1);
        lv_label_t *ts_label = cookie_label_create(screen, &font_18px, lv_color_make(204, 204, 204));
        lv_label_set_text_fmt(ts_label, "%02u:%02u", msg.notice_time.hour, msg.notice_time.min);

        lv_obj_t* img_close_btn = cookie_img_create(screen, &wd_img_app_notice_received);
        lv_obj_align(img_close_btn,LV_ALIGN_BOTTOM_MID,0,-20);
        lv_obj_add_flag(img_close_btn,LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(img_close_btn,app_msg_return,LV_EVENT_CLICKED, (void*)EVENT_TEXT_LATEST_EXT);
        lv_obj_set_ext_click_area(img_close_btn, 20);
    }
}

lv_obj_t* app_msg_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, app_msg_return, LV_EVENT_GESTURE, NULL);

    switch (param)
    {
    case EVENT_TEXT_LATEST:
        text_msg_contanier(screen);
        break;
    case EVENT_ALARM_CLOCK:
        alarm_msg_contanier(screen);
        break;
    case EVENT_TEXT_LATEST_EXT:
        text_msg_contanier_ext(screen);
        break;

    default:
        break;
    }

    return screen;
}

void app_msg_release(void)
{

}

static void app_msg_return(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_GESTURE)
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if(dir == LV_DIR_RIGHT)
        {
            SCREEN_RETURN(0, 0);
        }
        else if (dir == LV_DIR_TOP)
        {
            SCREEN_RETURN(0, 0);
        }
    }

    if (event == LV_EVENT_CLICKED)
    {
        GUI_EVENT event_type = (GUI_EVENT)lv_event_get_user_data(e);
        switch (event_type)
        {
        case EVENT_TEXT_LATEST:
        case EVENT_TEXT_LATEST_EXT:
            if (app_notice_get_latest(&msg)) {
                app_notice_set_overdue(msg.id);
            }
            break;

        default:
            break;
        }

        SCREEN_RETURN(0, 0);
    }

    lv_indev_wait_release(lv_indev_get_act());
}

void update_msg_snap(void)
{
    lv_res_t res = LV_RES_INV;
    lv_memset_00(&msg_dsc,sizeof(lv_img_dsc_t));
    lv_obj_t *msg_scr = app_msg_init(EVENT_TEXT_LATEST_EXT);
    snap_buf_size = lv_snapshot_buf_size_needed(msg_scr,LV_IMG_CF_GDX_RGB565);
    snap_buf = app_graphics_mem_malloc(snap_buf_size);
    res = lv_snapshot_take_to_buf(msg_scr,LV_IMG_CF_GDX_RGB565,&msg_dsc,snap_buf,snap_buf_size);
    lv_obj_del(msg_scr);
    printf("update_msg_snap %u\n",res);
}

void release_msg_snap(void)
{
    if (snap_buf)
    {
        lv_memset_00(snap_buf,snap_buf_size);
        app_graphics_mem_free(snap_buf);
        snap_buf = NULL;
        lv_memset_00(&msg_dsc,sizeof(lv_img_dsc_t));
        printf("release_msg_snap \n");
    }
}

lv_img_dsc_t *get_msg_snap_dsc(void)
{
    return &msg_dsc;
}
