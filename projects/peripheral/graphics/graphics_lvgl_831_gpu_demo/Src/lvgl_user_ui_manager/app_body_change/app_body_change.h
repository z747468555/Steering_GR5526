#ifndef __APP_BODY_CHANGE_H__
#define __APP_BODY_CHANGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_win_manager.h"


#define SCREEN_BODY_CHANGE_PAGE \
    [SCREEN_ID_BODY_CHANGE] = { \
        .attr_id_self             = SCREEN_ID_BODY_CHANGE,\
        .attr_level               = SCREEN_LEVEL_APP,\
        .attr_delete              = 1,\
        .attr_cached              = 0,\
        .attr_auto_off            = 1,\
        .attr_palm_off            = 0,\
        .attr_handsdown_off       = 0,\
        .attr_active_time         = 600,\
        .attr_idle_timeout        = 10,\
        .attr_idle_timeout_target = SCREEN_ID_BODY_CHANGE,\
        .p_obj_screen             = NULL,\
        .p_cache_addr             = NULL,\
        .screen_init              = body_change_init,\
        .screen_release           = body_change_release,\
        .screen_event             = NULL,\
        .screen_delayed_work      = NULL,\
    }\

lv_obj_t* body_change_init(uint32_t param);
void body_change_release(void);

#ifdef __cplusplus
}
#endif

#endif //__APP_BODY_CHANGE_H__
