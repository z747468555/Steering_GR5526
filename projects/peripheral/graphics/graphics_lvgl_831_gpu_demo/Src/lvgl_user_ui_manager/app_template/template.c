#include "template.h"

static uint32_t click_cnt = 0;

static void template_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
 
    if(code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
 
        switch(dir) {
            case LV_DIR_RIGHT:
                SCREEN_RETURN(0,0);
                break;
            default:
                break;
        }
    }
}

lv_obj_t *ui_template_page_init(uint32_t param)
{
    lv_obj_t *page = lv_obj_create(NULL);
    lv_obj_set_style_pad_all(page,0,LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(page,0,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa( page, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(page, lv_color_black(), LV_STATE_DEFAULT);

    lv_btn_t *btn = lv_btn_create(page);
    lv_obj_set_style_bg_opa( page, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(page, lv_color_make(0x90,0,0), LV_STATE_DEFAULT);
    lv_obj_set_size(btn,200,50);
    lv_obj_center(btn);
    lv_label_t *label = lv_label_create(btn);
    lv_label_set_text(label,"[ui_template_page]");
    lv_obj_center(label);

    lv_obj_add_event_cb(page,template_event_cb,LV_EVENT_GESTURE,page);

    return page;
}

void ui_template_page_release(void)
{
    ;
}

