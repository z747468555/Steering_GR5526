#include "app_version.h"
#include "app_service_simulate.h"

typedef enum
{
    version_validity,
    eletricity_low,
    ota_confirm,
    ota_progress,
} version_ota_status;

static void version_return(lv_event_t* e);
static void version_upgrade_button(lv_event_t* e);
static void version_change_page(version_ota_status page);

static lv_obj_t* screen = NULL;
static lv_timer_t* timer = NULL;
static lv_obj_t* arc = NULL;
static lv_obj_t* countdown_text = NULL;
static version_ota_status version_mode = version_validity;

lv_obj_t* version_init(uint32_t param)
{
    screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, version_return, LV_EVENT_GESTURE, NULL);


    version_change_page(version_validity);



    return screen;
}

void version_release(void)
{
    screen = NULL;
    arc = NULL;
    countdown_text = NULL;
    version_mode = version_validity;

    if(timer)
    {
        lv_timer_del(timer);
        timer = NULL;
    }
}

static uint8_t Cookie = 0;
static void ota_in_progress(lv_timer_t* timer)
{
    uint8_t data =  gui_upgrade_process_get();

    if(data == 255)
    {
        app_screen_keep_on(false);
        service_sys_reset();
        lv_timer_pause(timer);
    }
    else if(data == 100)
    {
        app_screen_keep_on(false);
        service_sys_reset();
        lv_timer_pause(timer);
    }
    else
    {
        lv_label_set_text_fmt(countdown_text, "%d%%", data);
        lv_arc_set_value(arc, data);
    }
}

static void version_upgrade_button(lv_event_t* e)
{
    if(app_environment_get_power() > 50)
    {
        version_change_page(ota_confirm);
    }
    else
    {
        version_change_page(eletricity_low);
    }

}

static void confirm_cb(lv_event_t *e)
{
    version_change_page(ota_progress);
}

static void version_return(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_GESTURE)
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if(dir == LV_DIR_RIGHT && version_mode != ota_progress)
        {
            SCREEN_RETURN(0, 0);
        }
    }

    lv_indev_wait_release(lv_indev_get_act());
}

static void version_change_page(version_ota_status page)
{
    if(screen)
    {
        lv_obj_clean(screen);
    }

    version_mode = page;

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "版本升级");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    switch (page)
    {
    case version_validity:
        {
            lv_obj_t* tips_img = cookie_img_create(screen, &wd_img_app_upgrade_icon);
            lv_obj_align(tips_img, LV_ALIGN_TOP_MID, 0, 130);

            app_version_t new = {0};
            gui_new_version_get_data(&new);

            app_version_t current = {0};
            gui_current_version_get_data(&current);

            lv_obj_t* tips_version_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
            lv_obj_set_width(tips_version_text, 200);
            lv_label_set_text_fmt(tips_version_text, "当前版本号%d.%d", current.mian, current.sub);
            lv_obj_align(tips_version_text,  LV_ALIGN_TOP_MID, 0, 182);

            if(gui_version_get_validity() == 1){
                lv_obj_t* tips_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
                lv_obj_set_width(tips_text, 200);
                lv_label_set_text_fmt(tips_text, "已是最新版");
                lv_obj_align_to(tips_text, tips_version_text, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
            } else {
                lv_obj_t* tips_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
                lv_obj_set_width(tips_text, 200);
                lv_label_set_text_fmt(tips_text, "已有新版%d.%d", new.mian, new.sub);
                lv_obj_align_to(tips_text, tips_version_text, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

                lv_obj_t* upgrade_img = cookie_img_create(screen, &wd_img_app_upgrade_button);
                lv_obj_align(upgrade_img, LV_ALIGN_TOP_MID, 0, 296);
                lv_obj_add_flag(upgrade_img, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_add_event_cb(upgrade_img, version_upgrade_button, LV_EVENT_CLICKED, NULL);
                lv_obj_set_ext_click_area(upgrade_img, 20);
            }
        }
        break;

    case ota_progress:
        {
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

            lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
            lv_obj_set_width(title_text, 200);
            lv_label_set_text_fmt(title_text, "%s", "版本升级中");
            lv_obj_align_to(title_text, arc, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

            countdown_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
            lv_label_set_text_fmt(countdown_text, "%d%%", 0);
            lv_obj_center(countdown_text);

            timer = lv_timer_create(ota_in_progress, 500, NULL);

	        app_screen_keep_on(true);
            service_upgrade_start();
        }
        break;

    case eletricity_low:
        {
            lv_obj_t* eletricity = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
            lv_label_set_text_fmt(eletricity, "eletricity low");
            lv_obj_center(eletricity);
        }
        break;

    case ota_confirm:
        {
            lv_obj_t* confirm_label = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
            lv_label_set_text_fmt(confirm_label, "ota need time");
            lv_obj_center(confirm_label);

            lv_obj_t* confirm = cookie_img_create(screen, &wd_img_app_ecg_confirm);
            lv_obj_add_event_cb(confirm, confirm_cb, LV_EVENT_CLICKED, NULL);
            lv_obj_align(confirm, LV_ALIGN_BOTTOM_MID, 0, -20);
            lv_obj_add_flag(confirm, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_ext_click_area(confirm, 20);
        }
        break;

    default:
        break;
    }
}