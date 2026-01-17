#ifndef __APP_ID_CODE_H__
#define __APP_ID_CODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_win_manager.h"


#define SCREEN_ID_CODE_PAGE \
    [SCREEN_ID_ID_CODE] = { \
        .attr_id_self             = SCREEN_ID_ID_CODE,\
        .attr_level               = SCREEN_LEVEL_APP,\
        .attr_delete              = 1,\
        .attr_cached              = 0,\
        .attr_auto_off            = 1,\
        .attr_palm_off            = 0,\
        .attr_handsdown_off       = 0,\
        .attr_active_time         = 600,\
        .attr_idle_timeout        = 10,\
        .attr_idle_timeout_target = SCREEN_ID_ID_CODE,\
        .p_obj_screen             = NULL,\
        .p_cache_addr             = NULL,\
        .screen_init              = id_code_init,\
        .screen_release           = id_code_release,\
        .screen_event             = NULL,\
        .screen_delayed_work      = NULL,\
    }\

lv_obj_t* id_code_init(uint32_t param);
void id_code_release(void);

#ifdef __cplusplus
}
#endif

#endif //__APP_ID_CODE_H__
