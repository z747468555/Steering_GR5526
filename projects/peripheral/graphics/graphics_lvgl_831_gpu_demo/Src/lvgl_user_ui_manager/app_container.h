#ifndef __APP_CONTAINER_H__
#define __APP_CONTAINER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_win_def.h"
#include "app_service_simulate.h"

// cookie create
lv_obj_t* cookie_obj_create(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_dir_t dir);
lv_obj_t* cookie_img_create(lv_obj_t* parent, const void* image);
lv_obj_t* cookie_label_create(lv_obj_t* parent, const lv_font_t* font, lv_color_t color);
lv_obj_t* cookie_label_adaption_create(lv_obj_t* parent, lv_coord_t width, const lv_font_t* font, lv_label_long_mode_t mode, lv_color_t color);
lv_obj_t* cookie_page_create(lv_obj_t* parent);
lv_obj_t* cookie_tileview_create(lv_obj_t* parent);

lv_obj_t* cookie_roller_create(lv_obj_t* parent, lv_coord_t w, int32_t min, int32_t max, int32_t step, int32_t value);
void roller_delete_cb(lv_event_t* e);
void cookie_roller_event_cb(lv_event_t* e);
void cookie_roller_calc(lv_obj_t* roller, int32_t min, int32_t max, int32_t step, int32_t value);

lv_obj_t* cookie_list_item_create(lv_obj_t* parent, const void* image, const char* text, lv_event_cb_t event_cb, screen_id_e id);
lv_obj_t* cookie_sport_item_create(lv_obj_t* parent, const void* image, const char* name_text, const char* unit_text, uint32_t data);
lv_obj_t* cookie_alarm_item_create(lv_obj_t* parent, uint8_t min, uint8_t hour, bool repeat);
lv_obj_t* cookie_environment_item_create(lv_obj_t* parent, const void* image, const char* unit_text, app_environment_t data, uint8_t type);
lv_obj_t* cookie_reminder_item_create(lv_obj_t* parent, const char* text, bool overdue, lv_event_cb_t cb, uint16_t id);

// cookie init
void cookie_tileview_page_init(lv_obj_t* page);

#ifdef __cplusplus
}
#endif

#endif //__APP_CONTAINER_H__
