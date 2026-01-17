#ifndef __APP_DEBUG_H__
#define __APP_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_win_manager.h"


#define SCREEN_DEBUG_PAGE \
    [SCREEN_ID_DEBUG] = { \
        .attr_id_self             = SCREEN_ID_DEBUG,\
        .attr_level               = SCREEN_LEVEL_APP,\
        .attr_delete              = 1,\
        .attr_cached              = 0,\
        .attr_auto_off            = 1,\
        .attr_palm_off            = 0,\
        .attr_handsdown_off       = 0,\
        .attr_active_time         = 600,\
        .attr_idle_timeout        = 10,\
        .attr_idle_timeout_target = SCREEN_ID_DEBUG,\
        .p_obj_screen             = NULL,\
        .p_cache_addr             = NULL,\
        .screen_init              = debug_init,\
        .screen_release           = debug_release,\
        .screen_event             = NULL,\
        .screen_delayed_work      = NULL,\
    }\

lv_obj_t* debug_init(uint32_t param);
void debug_release(void);
bool debug_monkey_is_start(void);

#ifdef __cplusplus
}
#endif

#endif //__APP_DEBUG_H__

