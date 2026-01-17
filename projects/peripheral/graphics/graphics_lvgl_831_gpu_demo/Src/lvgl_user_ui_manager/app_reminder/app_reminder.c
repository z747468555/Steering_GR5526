#include "app_reminder.h"
#include "app_service_simulate.h"

static app_notice_list_t* notice_list = NULL;
static lv_obj_t* screen = NULL;
static void reminder_return(lv_event_t* e);
static void reminder_delete_all(lv_event_t* e);
static void reminder_list(bool mode); // 1:list 0:one

lv_obj_t* reminder_init(uint32_t param)
{
    screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, reminder_return, LV_EVENT_GESTURE, NULL);

    notice_list = app_notice_get_list();

    if(notice_list == NULL) {
        lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_top(screen, 30, LV_PART_MAIN);
        lv_obj_set_style_pad_row(screen, 78, LV_PART_MAIN);

        lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
        lv_obj_set_width(title_text, 200);
        lv_label_set_text_fmt(title_text, "%s", "提醒中心");

        lv_obj_t* nothing_img = cookie_img_create(screen, &wd_img_app_notice_noMsg);
    } else {
        lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_top(screen, 30, LV_PART_MAIN);
        lv_obj_set_style_pad_bottom(screen, 30, LV_PART_MAIN);
        lv_obj_set_style_pad_row(screen, 10, LV_PART_MAIN);

        reminder_list(1);
    }

    return screen;
}

static uint16_t id = 0;
static void reminder_show_one(lv_event_t* e)
{
    id = (uint16_t)lv_event_get_user_data(e);
    reminder_list(0);
}

static void reminder_overdue_change(lv_event_t* e)
{
    app_notice_set_overdue(id);
    reminder_list(1);
}

static void reminder_delete_one(lv_event_t* e)
{
    app_notice_delete(id);
    reminder_list(1);
}

static void reminder_list(bool mode)
{
    lv_obj_clean(screen);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "提醒中心");

    notice_list = app_notice_get_list();
    if(notice_list == NULL) {
        lv_obj_set_style_pad_top(screen, 30, LV_PART_MAIN);
        lv_obj_set_style_pad_row(screen, 78, LV_PART_MAIN);
        lv_obj_t* nothing_img = cookie_img_create(screen, &wd_img_app_notice_noMsg);
        return;
    }

    if(mode)
    {
        for(uint8_t i = 0; i < 20; i++) {
            cookie_reminder_item_create(screen, notice_list->current_notice.text, notice_list->current_notice.overdue, reminder_show_one, notice_list->current_notice.id);
            if(notice_list->next) {
                notice_list = notice_list->next;
            } else {
                break;
            }
        }

        lv_obj_t* delete_img = cookie_img_create(screen, &wd_img_app_clock_delete);
        lv_obj_add_flag(delete_img, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(delete_img, reminder_delete_all, LV_EVENT_CLICKED, NULL);
        lv_obj_set_ext_click_area(delete_img, 20);
    }
    else
    {
         for(uint8_t i = 0; i < 20; i++) {
            if(notice_list->current_notice.id == id)
            {
                lv_obj_t* page = cookie_obj_create(screen, 280, 185, LV_DIR_VER);
                lv_obj_t* reminder_label = cookie_label_create(page, &dync_20px_tt, LV_COLOR_WHITE);
                lv_obj_set_style_text_align(reminder_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
                lv_obj_set_size(reminder_label, 280, LV_SIZE_CONTENT);
                lv_label_set_long_mode(reminder_label, LV_LABEL_LONG_WRAP);
                lv_label_set_text(reminder_label, notice_list->current_notice.text);
                lv_obj_center(reminder_label);

                if(notice_list->current_notice.overdue)
                {
                    lv_obj_t* received = cookie_img_create(screen, &wd_img_app_notice_received);
                    lv_obj_add_flag(received, LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_add_event_cb(received, reminder_overdue_change, LV_EVENT_CLICKED, NULL);
                    lv_obj_set_ext_click_area(received, 20);
                }
                else
                {
                    lv_obj_t* delete_img = cookie_img_create(screen, &wd_img_app_clock_delete);
                    lv_obj_add_flag(delete_img, LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_add_event_cb(delete_img, reminder_delete_one, LV_EVENT_CLICKED, NULL);
                    lv_obj_set_ext_click_area(delete_img, 20);
                }
                break;
            }
            else
            {
                if(notice_list->next) {
                notice_list = notice_list->next;
                } else {
                    break;
                }
            }
        }
    }
}

static void reminder_delete_all(lv_event_t* e)
{
    app_notice_delete_all();
    SCREEN_RETURN(0, 0);
}

void reminder_release(void)
{
    notice_list = NULL;
    screen = NULL;
}

static void reminder_return(lv_event_t* e)
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
