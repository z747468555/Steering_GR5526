#ifndef __UI_NOTIFICATION_CENTER_H__
#define __UI_NOTIFICATION_CENTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_win_manager.h"

typedef enum 
{
    NOTIFI_SCREEN_HIDE,
    NOTIFI_SCREEN_ENT,
    NOTIFI_SCREEN_EXT,
    NOTIFI_SCREEN_SHOW,
} app_notifi_status_e;

lv_obj_t *cookie_create_notifi_center(lv_obj_t *root_page, gui_watchface_screen *p_watch, lv_obj_t *part_screen);

#ifdef __cplusplus
}
#endif

#endif //__UI_NOTIFICATION_CENTER_H__