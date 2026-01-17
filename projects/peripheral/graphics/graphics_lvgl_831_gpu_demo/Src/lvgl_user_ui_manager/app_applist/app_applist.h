#ifndef __UI_APPLIST_H__
#define __UI_APPLIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_win_manager.h"

#define SCREEN_APPLIST_PAGE \
    [SCREEN_ID_APPLIST] = { \
        .attr_id_self           = SCREEN_ID_APPLIST,\
        .attr_level             = SCREEN_LEVEL_APP,\
        .attr_delete            = 1,\
        .attr_cached            = 0,\
        .attr_auto_off          = 1,\
        .attr_palm_off          = 0,\
        .attr_handsdown_off     = 0,\
        .attr_active_time       = 600,\
        .attr_idle_timeout      = 10,\
        .attr_idle_timeout_target = SCREEN_ID_APPLIST,\
        .p_obj_screen           = NULL,\
        .p_cache_addr           = NULL,\
        .screen_init            = ui_applist_page_init,\
        .screen_release         = ui_applist_page_release,\
        .screen_event           = NULL,\
        .screen_delayed_work      = NULL,\
    }\

lv_obj_t *ui_applist_page_init(uint32_t param);
void ui_applist_page_release(void);

#ifdef __cplusplus
}
#endif

#endif //__UI_APPLIST_H__
