#ifndef __APP_SOS_H__
#define __APP_SOS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_win_manager.h"


#define SCREEN_SOS_PAGE \
    [SCREEN_ID_SOS] = { \
        .attr_id_self             = SCREEN_ID_SOS,\
        .attr_level               = SCREEN_LEVEL_APP,\
        .attr_delete              = 1,\
        .attr_cached              = 0,\
        .attr_auto_off            = 1,\
        .attr_palm_off            = 0,\
        .attr_handsdown_off       = 0,\
        .attr_active_time         = 600,\
        .attr_idle_timeout        = 10,\
        .attr_idle_timeout_target = SCREEN_ID_SOS,\
        .p_obj_screen             = NULL,\
        .p_cache_addr             = NULL,\
        .screen_init              = sos_init,\
        .screen_release           = sos_release,\
        .screen_event             = NULL,\
        .screen_delayed_work      = NULL,\
    }\

lv_obj_t* sos_init(uint32_t param);
void sos_release(void);

#ifdef __cplusplus
}
#endif

#endif //__APP_SOS_H__
