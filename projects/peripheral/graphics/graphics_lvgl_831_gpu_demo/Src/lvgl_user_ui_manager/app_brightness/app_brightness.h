#ifndef __APP_BRIGHTNESS_H__
#define __APP_BRIGHTNESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_win_manager.h"


#define SCREEN_BRIGHTNESS_PAGE \
    [SCREEN_ID_BRIGHTNESS] = { \
        .attr_id_self             = SCREEN_ID_BRIGHTNESS,\
        .attr_level               = SCREEN_LEVEL_APP,\
        .attr_delete              = 1,\
        .attr_cached              = 0,\
        .attr_auto_off            = 1,\
        .attr_palm_off            = 0,\
        .attr_handsdown_off       = 0,\
        .attr_active_time         = 600,\
        .attr_idle_timeout        = 10,\
        .attr_idle_timeout_target = SCREEN_ID_BRIGHTNESS,\
        .p_obj_screen             = NULL,\
        .p_cache_addr             = NULL,\
        .screen_init              = brightness_init,\
        .screen_release           = brightness_release,\
        .screen_event             = NULL,\
        .screen_delayed_work      = NULL,\
    }\

lv_obj_t* brightness_init(uint32_t param);
void brightness_release(void);

#ifdef __cplusplus
}
#endif

#endif //__APP_BRIGHTNESS_H__
