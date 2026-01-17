#ifndef __UI_WIN_MANAGER_H__
#define __UI_WIN_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include "lvgl.h"
#include "ui_win_def.h"
#include "app_assert.h"
#include "lv_img_dsc_list.h"
typedef enum _event_type_e
{
    EVENT_TYPE_WINDOW = 0,      ///< event from window
    EVENT_TYPE_SYSTEM,          ///< event from system
    EVENT_TYPE_USER,            ///< event from user

    EVENT_TYPE_NUMBER,
} event_type_e;

typedef enum _win_event_e
{
    WIN_EVT_ENTER = 0,          ///< event notify when screen enter
    WIN_EVT_EXIT,               ///< event notify when screen exit
    WIN_EVT_PAUSE,              ///< event notify to active screen when lcd off
    WIN_EVT_RESUME,             ///< event notify to active screen when lcd on
    WIN_EVT_REPAINT,            ///< event notify to prev screen when resume to prev screen and repaint

    WIN_EVT_EDGE_SWIPE_EXIT,    ///< event notify edge scroll exit

    WIN_EVT_QUERY_SCROLL_DESC,  ///< Query scroll desc info
    WIN_EVT_QUERY_SWIPE_EXIT,   ///< Query if allow to exit when swipe right
    WIN_EVT_QUERY_KEY_HANDLE,   ///< Query current screen, if key event handler from 'WM' by default
    WIN_EVT_QUERY_ANIM_CLICK,   ///< Query the anim type, when click power key and change to watch screen
} win_event_e;

typedef struct _wm_event_t
{
    event_type_e evt_type;      ///< event type

    unsigned long sub_type;     ///< sub type, corresponding to win_event_e/sys_event_e/user_event_e
    unsigned long parameter;    ///< parameter data or pointer depending on the sub_type
} wm_event_t;

/* screen trans type about call stack */
typedef enum
{
    WIN_CALL_TYPE_START = 0,    ///<起始窗体 -- 同窗体切换，但用于启动启动后使用，切换前无当前有效窗体
    WIN_CALL_TYPE_CHANGE,
    WIN_CALL_TYPE_CALL,
    WIN_CALL_TYPE_RETURN,
    WIN_CALL_TYPE_REPLACE,
    WIN_CALL_TYPE_POPUP,
    WIN_CALL_TYPE_DELETE,
    WIN_CALL_TYPE_DELETE_EX,
    WIN_CALL_TYPE_CALL_EX,
} win_call_type_e;

typedef struct _screen_desc_t
{
    const screen_id_e           attr_id_self;
    uint32_t                    attr_level  :4;
    uint32_t                    attr_delete :1;
    uint32_t                    attr_cached :1;

    uint32_t                    attr_auto_off : 1;
    uint32_t                    attr_palm_off : 1;
    uint32_t                    attr_handsdown_off : 1;

    uint32_t                    attr_delete_flag : 1;
    uint32_t                    attr_init_flag : 1;
    uint32_t                    attr_exit_flag : 1;

    uint32_t                    attr_active_time;
    uint32_t                    attr_idle_timeout;
    uint32_t                    attr_idle_timeout_target;

    lv_obj_t                    *p_obj_screen;
    lv_obj_t                    *p_canvas;
    lv_obj_t                    *p_cache_addr;

    lv_obj_t *(*screen_init)(uint32_t param);
    void (*screen_delayed_work)(uint32_t param);
    uint32_t (*screen_event)(wm_event_t event);
    void (*screen_tick)(void);
    void (*screen_release)(void);
} screen_desc_t;

extern screen_desc_t g_screen_array[SCREEN_ID_NUMBER];

typedef struct _win_manager_t
{
    win_call_type_e call_type;
    screen_id_e     id_screen_prev;
    screen_id_e     id_screen_new;
    screen_desc_t   *p_active;

    lv_ll_t call_stack_ll;
} win_manager_t;

extern void wm_goto_start_screen(screen_id_e id,uint32_t param);
extern bool wm_screen_transition_extern(win_call_type_e call_type, screen_id_e new_id, uint32_t param);
extern bool wm_screen_transition_now(win_call_type_e call_type,screen_id_e new_id,uint32_t param);
extern void wm_screen_transition_ll(win_call_type_e call_type,screen_id_e new_it, uint32_t param,uint8_t level_min,uint8_t level_max,bool update);
extern screen_id_e wm_get_active_screen_id(void);
extern lv_obj_t *wm_get_active_screen(void);

#define SCREEN_START(id,param)                  wm_screen_transition_now(WIN_CALL_TYPE_START, id, param);
#define SCREEN_CHANGE(id,param,anim_type)       wm_screen_transition_extern(WIN_CALL_TYPE_CHANGE,id,param);
#define SCREEN_CALL(id,param,anim_type)         wm_screen_transition_extern(WIN_CALL_TYPE_CALL,id,param);
#define SCREEN_RETURN(param,anim_type)          wm_screen_transition_extern(WIN_CALL_TYPE_RETURN,SCREEN_ID_INVALID,param);
#define SCREEN_REPLACE(id,param,anim_type)      wm_screen_transition_extern(WIN_CALL_TYPE_REPLACE,id,param);
#define SCREEN_POPUP(id,param,anim_type)        wm_screen_transition_extern(WIN_CALL_TYPE_POPUP,id,param);

#ifdef __cplusplus
}
#endif

#endif
