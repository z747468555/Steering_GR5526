#include "app_continuity.h"
#include "app_service_simulate.h"
#include "app_container.h"
// Cookie is ok

typedef struct _gui_continuity_screen_t
{
    bool status ;
    lv_obj_t* image;
    lv_obj_t* label;
    lv_obj_t* button;
    lv_obj_t* button_label;

} gui_continuity_screen;

static gui_continuity_screen* p_continuity;

static void continuity_return(lv_event_t* e);
static void continuity_button(lv_event_t* e);

lv_obj_t* continuity_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, continuity_return, LV_EVENT_GESTURE, NULL);

    if(!p_continuity)
    {
        p_continuity = lv_mem_alloc(sizeof(gui_continuity_screen));
        LV_ASSERT_MALLOC(p_continuity);
        lv_memset_00(p_continuity, sizeof(gui_continuity_screen));
    }

    p_continuity->status = app_continuity_get_status();

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "连续监测");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    p_continuity->image = cookie_img_create(screen, &wd_img_app_continue_opened);
    lv_obj_align(p_continuity->image, LV_ALIGN_TOP_MID, 0, 110);

    p_continuity->label = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
    lv_obj_set_width(p_continuity->label, 122);
    lv_label_set_text_fmt(p_continuity->label, "%s", "已开启");
    lv_obj_align(p_continuity->label, LV_ALIGN_TOP_MID, 0, 190);

    lv_obj_t* status_tips = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
    lv_obj_set_width(status_tips, 200);
    lv_label_set_text_fmt(status_tips, "%s", "监测");
    lv_obj_align_to(status_tips, p_continuity->label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t* status_dot = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
    lv_obj_set_width(status_dot, 200);
    lv_label_set_text_fmt(status_dot, "%s", "......");
    lv_obj_align_to(status_dot, status_tips, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    p_continuity->button = cookie_obj_create(screen, 122, 48, LV_DIR_NONE);
    lv_obj_set_style_radius(p_continuity->button, 24, LV_PART_MAIN);
    lv_obj_add_flag(p_continuity->button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(p_continuity->button, 20);
    lv_obj_add_event_cb(p_continuity->button, continuity_button, LV_EVENT_CLICKED, NULL);
    lv_obj_align(p_continuity->button, LV_ALIGN_BOTTOM_MID, 0, -18);

    p_continuity->button_label = cookie_label_create(p_continuity->button, &font_20px, LV_COLOR_WHITE);
    lv_obj_set_width(p_continuity->button_label, 122);

    lv_obj_center(p_continuity->button_label);

    if(p_continuity->status) {
        lv_img_set_src(p_continuity->image, &wd_img_app_continue_opened);
        lv_obj_set_style_bg_color(p_continuity->button, LV_COLOR_GRAY,LV_PART_MAIN);
        lv_label_set_text_fmt(p_continuity->label, "%s", "已开启");
        lv_label_set_text_fmt(p_continuity->button_label, "%s", "关闭");
    } else {
        lv_img_set_src(p_continuity->image, &wd_img_app_continue_closed);
        lv_obj_set_style_bg_color(p_continuity->button, LV_COLOR_BLUE,LV_PART_MAIN);
        lv_label_set_text_fmt(p_continuity->label, "%s", "已关闭");
        lv_label_set_text_fmt(p_continuity->button_label, "%s", "开启");
    }

    return screen;
}

void continuity_release(void)
{
    lv_mem_free(p_continuity);
    p_continuity = NULL;
}

static void continuity_button(lv_event_t* e)
{
    p_continuity->status = !p_continuity->status;

    if(p_continuity->status) {
        lv_img_set_src(p_continuity->image, &wd_img_app_continue_opened);
        lv_obj_set_style_bg_color(p_continuity->button, LV_COLOR_GRAY,LV_PART_MAIN);
        lv_label_set_text_fmt(p_continuity->label, "%s", "已开启");
        lv_label_set_text_fmt(p_continuity->button_label, "%s", "关闭");
    } else {
        lv_img_set_src(p_continuity->image, &wd_img_app_continue_closed);
        lv_obj_set_style_bg_color(p_continuity->button, LV_COLOR_BLUE,LV_PART_MAIN);
        lv_label_set_text_fmt(p_continuity->label, "%s", "已关闭");
        lv_label_set_text_fmt(p_continuity->button_label, "%s", "开启");
    }

    app_continuity_set_status(p_continuity->status);
}

static void continuity_return(lv_event_t* e)
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
