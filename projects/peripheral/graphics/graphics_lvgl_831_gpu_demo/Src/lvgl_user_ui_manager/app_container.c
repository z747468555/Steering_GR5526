#include "app_container.h"

// cookie create
lv_obj_t* cookie_obj_create(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_dir_t dir)
{
    lv_obj_t* obj = lv_obj_create(parent);

    lv_obj_set_size(obj, width, height);
    lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(obj, LV_COLOR_BLACK, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(obj, dir);

    return obj;
}

lv_obj_t* cookie_img_create(lv_obj_t* parent, const void* image)
{
    lv_obj_t* img = lv_img_create(parent);
    lv_img_set_size_mode(img, LV_IMG_SIZE_MODE_VIRTUAL);
    lv_img_set_src(img, image);

    return img;
}

lv_obj_t* cookie_label_create(lv_obj_t* parent, const lv_font_t* font, lv_color_t color)
{
    lv_obj_t* label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    return label;
}

lv_obj_t* cookie_label_adaption_create(lv_obj_t* parent, lv_coord_t width, const lv_font_t* font, lv_label_long_mode_t mode, lv_color_t color)
{
    lv_obj_t* label = lv_label_create(parent);

    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
    lv_obj_set_width(label, width);

    lv_label_set_long_mode(label, mode);

    return label;
}

lv_obj_t* cookie_page_create(lv_obj_t* parent)
{
    lv_obj_t* page = lv_obj_create(parent);

    lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_pad_all(page, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(page, LV_COLOR_BLACK, LV_PART_MAIN);
    lv_obj_set_style_border_width(page, 0, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(page, LV_DIR_VER);

    lv_obj_add_flag(page, LV_OBJ_FLAG_SCROLL_ONE);

    return page;
}

lv_obj_t* cookie_tileview_create(lv_obj_t* parent)
{
    lv_obj_t* tileview = lv_tileview_create(parent);
    lv_obj_set_size(tileview, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(tileview, 0, 0);
    lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_side(tileview, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(tileview, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(tileview, 0, LV_PART_MAIN);
    lv_obj_set_scroll_dir(tileview, LV_DIR_VER);

    lv_obj_add_flag(tileview, LV_OBJ_FLAG_EVENT_BUBBLE);

    return tileview;
}

typedef struct {
    int32_t min;
    int32_t max;
} roller_extremum;

lv_obj_t* cookie_roller_create(lv_obj_t* parent, lv_coord_t w, int32_t min, int32_t max, int32_t step, int32_t value)
{
    if(!parent) {

        return NULL;
    }

    lv_obj_t* roller = lv_roller_create(parent);
    lv_obj_set_size(roller, w, 164);
    lv_obj_set_style_bg_opa(roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(roller, LV_COLOR_BLACK, LV_PART_MAIN);
    lv_obj_set_style_text_font(roller, &font_36px, LV_PART_MAIN);
    lv_obj_set_style_text_color(roller, LV_COLOR_GRAY_WHITE, LV_PART_MAIN);
    lv_obj_set_style_text_align(roller, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    // lv_obj_set_style_anim_time(roller, 1000, LV_PART_MAIN);
    lv_obj_set_style_text_color(roller, LV_COLOR_WHITE, LV_PART_SELECTED);
    lv_roller_set_visible_row_count(roller, 3);
    lv_obj_clear_flag(roller, LV_OBJ_FLAG_EVENT_BUBBLE);

    roller_extremum* extremum = (roller_extremum*)lv_mem_alloc(sizeof(roller_extremum));

    if(extremum) {
        extremum->min = min;
        extremum->max = max;
    }

    roller->user_data =(void*)extremum;

    cookie_roller_calc(roller, min, max, step, value);
    lv_obj_add_event_cb(roller, cookie_roller_event_cb, LV_EVENT_VALUE_CHANGED, (void*)step);
    lv_obj_add_event_cb(roller, roller_delete_cb, LV_EVENT_DELETE, NULL);

    return roller;
}

void roller_delete_cb(lv_event_t* e)
{
    lv_obj_t* roller = lv_event_get_target(e);
    roller_extremum* extremum = lv_obj_get_user_data(roller);

    if(extremum) {
        lv_mem_free(extremum);
    }
}

void cookie_roller_event_cb(lv_event_t* e)
{
    if(!e) {
        return;
    }

    if(LV_EVENT_VALUE_CHANGED == lv_event_get_code(e)) {
        lv_obj_t* roller = lv_event_get_target(e);
        roller_extremum* extremum =(roller_extremum*)roller->user_data;
        int32_t step =(int32_t)lv_event_get_user_data(e);
        char buf[10]="\0";
        lv_roller_get_selected_str(roller, buf, sizeof(buf));
        int32_t value = atoi(buf);
        cookie_roller_calc(roller, extremum->min, extremum->max, step, value);
    }
}

void cookie_roller_calc(lv_obj_t* roller, int32_t min, int32_t max, int32_t step, int32_t value)
{
    if(!roller) {
        return;
    }

    char buf[100]= "\0";
    uint16_t index = 5;

    char* format_free_11 ="%02d\n%02d\n%02d\n%02d\n%02d\n%02d\n%02d\n%02d\n%02d\n%02d\n%02d\0";

    int32_t left1 = value - 1 * step;
    int32_t left2 = value - 2 * step;
    int32_t left3 = value - 3 * step;
    int32_t left4 = value - 4 * step;
    int32_t left5 = value - 5 * step;

    int32_t right1 = value + 1 * step;
    int32_t right2 = value + 2 * step;
    int32_t right3 = value + 3 * step;
    int32_t right4 = value + 4 * step;
    int32_t right5 = value + 5 * step;

    int32_t range = max - min + step;

    if(left1 < min) {
        left1 += range;
    }

    if(left2 < min) {
        left2 += range;
    }

    if(left3 < min) {
        left3 += range;
    }

    if(left4 < min) {
        left4 += range;
    }

    if(left5 < min) {
        left5 += range;
    }

    if(right1 > max) {
        right1 -= range;
    }

    if(right2 > max) {
        right2 -= range;
    }

    if(right3 > max) {
        right3 -= range;
    }

    if(right4 > max) {
        right4 -= range;
    }

    if(right5 > max) {
        right5 -= range;
    }

    snprintf(buf, 100, format_free_11, left5, left4, left3, left2, left1, value, right1, right2, right3, right4, right5);

    lv_roller_set_options(roller, buf, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(roller, index, LV_ANIM_OFF);
}

uint8_t cookie_get_roller_data(lv_obj_t* roller)
{
    char buf[100] = {0}; //Cookie
    lv_roller_get_selected_str(roller, buf, sizeof(buf));

    return (uint8_t)atoi(buf);
}

lv_obj_t* cookie_list_item_create(lv_obj_t* parent, const void* image, const char* text, lv_event_cb_t event_cb, screen_id_e id)
{
    lv_obj_t* item = cookie_obj_create(parent, LV_HOR_RES, 94, LV_DIR_NONE);

    if (image)
    {
        lv_obj_t* img = cookie_img_create(item, image);
        lv_obj_align(img, LV_ALIGN_LEFT_MID, 64, 0);
    }

    lv_obj_t* label = cookie_label_create(item, &font_24px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(label, "%s", text);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 160, 0);

    lv_obj_add_event_cb(item, event_cb, LV_EVENT_SHORT_CLICKED, (void*)id);

    return item;
}

lv_obj_t* cookie_sport_item_create(lv_obj_t* parent, const void* image, const char* name_text, const char* unit_text, uint32_t data)
{
    lv_obj_t* item = cookie_obj_create(parent, LV_HOR_RES, 84, LV_DIR_NONE);

    lv_obj_t* img = cookie_img_create(item, image);
    lv_obj_align(img, LV_ALIGN_LEFT_MID, 60, 0);

    lv_obj_t* name_label = cookie_label_create(item, &font_24px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(name_label, "%s", name_text);
    lv_obj_align(name_label, LV_ALIGN_LEFT_MID, 142, 0);

    lv_obj_t* data_label = cookie_label_create(item, &font_36px, LV_COLOR_GREEN);
    lv_label_set_text_fmt(data_label, "%u", data);
    lv_obj_align(data_label, LV_ALIGN_LEFT_MID, 208, 0);

    lv_obj_t* unit_label = cookie_label_create(item, &font_20px, LV_COLOR_GREEN);
    lv_label_set_text_fmt(unit_label, "%s", unit_text);
    lv_obj_align_to(unit_label, data_label, LV_ALIGN_OUT_RIGHT_MID, 4, 6);

    return item;
}

bool cookie_sport_item_set_data(lv_obj_t* item, uint64_t data)
{
    if(item) {
        lv_obj_t* data_label = lv_obj_get_child(item, 2);
        lv_label_set_text_fmt(data_label, "%u", data);

        lv_obj_t* unit_label = lv_obj_get_child(item, 3);
        lv_obj_align_to(unit_label, data_label, LV_ALIGN_OUT_RIGHT_MID, 5, 2);

        return true;
    }

    return false;
}

lv_obj_t* cookie_alarm_item_create(lv_obj_t* parent, uint8_t min, uint8_t hour, bool repeat)
{
    lv_obj_t* item = cookie_obj_create(parent, 280, 46, LV_DIR_NONE);
    lv_obj_set_style_bg_color(item, LV_COLOR_GRAY, LV_PART_MAIN);
    lv_obj_set_style_radius(item, 24, LV_PART_MAIN);

    lv_obj_t* time_label = cookie_label_create(item, &font_24px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(time_label, "%02d:%02d", hour, min);
    lv_obj_align(time_label, LV_ALIGN_LEFT_MID, 16, -2);

    lv_obj_t* repeat_label = cookie_label_create(item, &font_20px, LV_COLOR_WHITE);
    if(repeat) {
        lv_label_set_text_fmt(repeat_label, "%s", "重复");
    } else {
        lv_label_set_text_fmt(repeat_label, "%s", "单次");
    }
    lv_obj_align(repeat_label, LV_ALIGN_RIGHT_MID, -16, 2);

    return item;
}

lv_obj_t* cookie_environment_item_create(lv_obj_t* parent, const void* image, const char* unit_text, app_environment_t data, uint8_t type)
{
    lv_obj_t* item = cookie_obj_create(parent, 120, 126, LV_DIR_NONE);

    lv_obj_t* img = cookie_img_create(item, image);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* data_label = cookie_label_create(item, &font_48px, LV_COLOR_WHITE);
    switch (type)
    {
    case 0:
        lv_label_set_text_fmt(data_label, "%u", data.pressure);
        break;

    case 1:
        lv_label_set_text_fmt(data_label, "%.1f", data.temperature);
        break;

    case 2:
        lv_label_set_text_fmt(data_label, "%u", data.visibility);
        break;

    default:
        break;
    }
    lv_obj_align_to(data_label, img, LV_ALIGN_OUT_BOTTOM_MID, 0, -8);

    lv_obj_t* unit_label = cookie_label_create(item, &font_16px, lv_color_make(0xCC, 0xCC, 0xCC));
    lv_label_set_text_fmt(unit_label, "%s", unit_text);
    lv_obj_align_to(unit_label, data_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    return item;
}

bool cookie_environment_item_set_data(lv_obj_t* item, app_environment_t data, uint8_t type)
{
    if(item) {
        lv_obj_t* data_label = lv_obj_get_child(item, 1);
        switch (type)
        {
        case 0:
            lv_label_set_text_fmt(data_label, "%u", data.pressure);
            break;

        case 1:
            lv_label_set_text_fmt(data_label, "%.1f", data.temperature);
            break;

        case 2:
            lv_label_set_text_fmt(data_label, "%u", data.visibility);
            break;

        default:
            break;
    }

        return true;
    }

    return false;
}

lv_obj_t* cookie_reminder_item_create(lv_obj_t* parent, const char* text, bool overdue, lv_event_cb_t cb, uint16_t id)
{
    lv_obj_t* item = cookie_obj_create(parent, 280, 54, LV_DIR_NONE);
    lv_obj_set_style_bg_color(item, LV_COLOR_GRAY, LV_PART_MAIN);
    lv_obj_set_style_radius(item, 24, LV_PART_MAIN);
    lv_obj_add_event_cb(item, cb, LV_EVENT_CLICKED, (void*)id);

    lv_obj_t* reminder_label = cookie_label_create(item, &dync_20px_tt, LV_COLOR_WHITE);
    lv_obj_set_style_text_align(reminder_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_set_size(reminder_label, 210, 34);
    lv_label_set_long_mode(reminder_label, LV_LABEL_LONG_DOT);
    lv_label_set_text(reminder_label, text);
    lv_obj_align(reminder_label, LV_ALIGN_TOP_LEFT, 20, 10);

    if(overdue)
    {
        lv_obj_t* status = cookie_obj_create(item, 10, 10, LV_DIR_NONE);
        lv_obj_set_style_bg_color(status, LV_COLOR_RED, LV_PART_MAIN);
        lv_obj_set_style_radius(status, 5, LV_PART_MAIN);
        lv_obj_align(status, LV_ALIGN_TOP_RIGHT, -10, 10);
    }

    return item;
}

// cookie init
void cookie_tileview_page_init(lv_obj_t* page)
{
    lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_scroll_dir(page, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);

    lv_obj_add_flag(page, LV_OBJ_FLAG_EVENT_BUBBLE);
}
