#ifndef __UI_WATCHFACE_H__
#define __UI_WATCHFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_win_manager.h"

typedef enum {
    MSG_CONT_STATUS_NONE,
    MSG_CONT_STATUS_IN_PROCESS,
    MSG_CONT_STATUS_IN_SHOWING,
} _e_msg_status;

typedef struct _gui_watchface_screen_t
{
    lv_obj_t* screen;
    lv_obj_t* flex_screen;
    lv_obj_t* watchface;
    lv_obj_t* applist;

    lv_obj_t* month_day;
    lv_obj_t* week[7];
    lv_obj_t* time_show;

    lv_obj_t* eletricity;
    lv_obj_t* charger;
    lv_obj_t* hr;
    lv_obj_t* step;
    lv_img_t *msg_img;

    uint8_t msg_cont_show;
    _e_msg_status msg_cont_status;

} gui_watchface_screen;

#define SCREEN_WATCHFACE_PAGE \
    [SCREEN_ID_WATCHFACE] = { \
        .attr_id_self           = SCREEN_ID_WATCHFACE,\
        .attr_level             = SCREEN_LEVEL_APP,\
        .attr_delete            = 1,\
        .attr_cached            = 0,\
        .attr_auto_off          = 1,\
        .attr_palm_off          = 0,\
        .attr_handsdown_off     = 0,\
        .attr_active_time       = 600,\
        .attr_idle_timeout      = 10,\
        .attr_idle_timeout_target = SCREEN_ID_WATCHFACE,\
        .p_obj_screen           = NULL,\
        .p_cache_addr           = NULL,\
        .screen_init            = ui_watchface_page_init,\
        .screen_release         = ui_watchface_page_release,\
        .screen_event           = NULL,\
        .screen_delayed_work      = NULL,\
    }\

lv_obj_t *ui_watchface_page_init(uint32_t param);
void ui_watchface_page_release(void);

#ifdef __cplusplus
}
#endif

#endif //__UI_WATCHFACE_H__
