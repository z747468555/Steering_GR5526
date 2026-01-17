#ifndef __APP_MSG_H__
#define __APP_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_win_manager.h"


#define SCREEN_MSG_PAGE \
    [SCREEN_ID_MSG] = { \
        .attr_id_self             = SCREEN_ID_MSG,\
        .attr_level               = SCREEN_LEVEL_APP,\
        .attr_delete              = 1,\
        .attr_cached              = 0,\
        .attr_auto_off            = 1,\
        .attr_palm_off            = 0,\
        .attr_handsdown_off       = 0,\
        .attr_active_time         = 600,\
        .attr_idle_timeout        = 10,\
        .attr_idle_timeout_target = SCREEN_ID_MSG,\
        .p_obj_screen             = NULL,\
        .p_cache_addr             = NULL,\
        .screen_init              = app_msg_init,\
        .screen_release           = app_msg_release,\
        .screen_event             = NULL,\
        .screen_delayed_work      = NULL,\
    }\

lv_obj_t* app_msg_init(uint32_t param);
void app_msg_release(void);
void update_msg_snap(void);
void release_msg_snap(void);
lv_img_dsc_t *get_msg_snap_dsc(void);

#ifdef __cplusplus
}
#endif

#endif //__APP_INDI_H__

