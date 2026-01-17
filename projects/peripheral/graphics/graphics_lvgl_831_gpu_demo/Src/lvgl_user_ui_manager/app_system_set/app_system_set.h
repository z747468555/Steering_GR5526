#ifndef __APP_SYSTEM_SET_H__
#define __APP_SYSTEM_SET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_win_manager.h"


#define SCREEN_SYSTEM_SET_PAGE \
    [SCREEN_ID_SYSTEM_SET] = { \
        .attr_id_self             = SCREEN_ID_SYSTEM_SET,\
        .attr_level               = SCREEN_LEVEL_APP,\
        .attr_delete              = 1,\
        .attr_cached              = 0,\
        .attr_auto_off            = 1,\
        .attr_palm_off            = 0,\
        .attr_handsdown_off       = 0,\
        .attr_active_time         = 600,\
        .attr_idle_timeout        = 10,\
        .attr_idle_timeout_target = SCREEN_ID_SYSTEM_SET,\
        .p_obj_screen             = NULL,\
        .p_cache_addr             = NULL,\
        .screen_init              = system_set_init,\
        .screen_release           = system_set_release,\
        .screen_event             = NULL,\
        .screen_delayed_work      = NULL,\
    }\

lv_obj_t* system_set_init(uint32_t param);
void system_set_release(void);

#ifdef __cplusplus
}
#endif

#endif //__APP_SYSTEM_SET_H__
