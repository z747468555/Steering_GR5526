#include "ui_win_manager.h"
#include "ui_win_def.h"

#include "lvgl.h"
#define APP_LOG_TAG	"ui_win"
#include "app_log.h"

/****************************
 *          DEFINES
 ****************************/

/****************************
 *          TYPEDEFS
 ****************************/
typedef struct _win_call_list_t
{
    screen_id_e screen_id;
    uint32_t screen_param;

    win_call_type_e call_type;
} win_call_list_t;


typedef enum _ll_changed_reason_e
{
    LL_CHANGED_THOWOUT = 0,
    LL_CHANGED_LATER,
    LL_CHANGED_NOW,

} ll_changed_reason_e;

/****************************
 *     GLOBAL VARIABLES
 ****************************/

/****************************
 *     MACRO
 ****************************/

/****************************
 *     STATIC PROTOTYPES
 ****************************/
static void _debug_win_call_stack(ll_changed_reason_e reason);

static void _wm_screen_start(screen_id_e id, uint32_t param);
static bool _wm_screen_trans(win_call_type_e call_type, screen_id_e new_id, uint32_t param);

static void  _wm_delete_prev_screen(void);
static void _wm_load_new_screen(void);

static screen_id_e _wm_get_prev_screenid(void);
static win_call_list_t *_wm_call_stack_ins_head(void);
static win_call_list_t *_wm_call_stack_ins_tail(void);
static win_call_list_t *_wm_call_stack_ins_before(win_call_list_t *p_next);
static void _wm_call_stack_rm_tail(void);
static win_call_list_t *_wm_popup_dealed(screen_id_e new_id, bool *popup_allowed);
static win_call_list_t *_wm_popup_search_ll(screen_id_e new_id);
static void _wm_remove_by_id_ll(screen_id_e new_id);

static bool _wm_call_stack_is_root(screen_id_e id);
static screen_id_e  _wm_call_stack_get_parent_by_id(screen_id_e id);


/****************************
 *     STATIC VARIABLES
 ****************************/

win_manager_t _g_win_manager =
{
        .p_active = NULL,
};

/****************************
 *     GLOBAL FUNCTIONS
 ****************************/
void wm_refresh_whole_screen(void)
{
    if (_g_win_manager.p_active)
    {
        lv_obj_invalidate(g_screen_array[_g_win_manager.p_active->attr_id_self].p_obj_screen);
    }

    return;
}

screen_id_e wm_get_prev_screen_id(void)
{
    return _g_win_manager.id_screen_prev;
}

screen_id_e wm_get_new_screen_id(void)
{
    return _g_win_manager.id_screen_new;
}

screen_id_e wm_get_active_screen_id(void)
{
    if (_g_win_manager.p_active)
        return _g_win_manager.p_active->attr_id_self;

    return SCREEN_ID_INVALID;
}

lv_obj_t *wm_get_active_screen(void)
{
    return _g_win_manager.p_active->p_obj_screen;
}

screen_id_e wm_get_list_screen_id(void)
{
    win_call_list_t *p_callstack_item = (win_call_list_t *)_lv_ll_get_tail(&_g_win_manager.call_stack_ll);
    APP_ASSERT_CHECK(p_callstack_item);

    if (p_callstack_item)
        return p_callstack_item->screen_id;
    else
        return SCREEN_ID_INVALID;
}

bool wm_screen_has_exists(screen_id_e screen_id)
{
    win_call_list_t *p_item = NULL;

    _LV_LL_READ_BACK(&_g_win_manager.call_stack_ll, p_item)
    {
        if (p_item->screen_id == screen_id)
        {
            return true;
        }
    }

    return false;
}


/**
 * delete all screen which has not deleted
 *
 * @param none
 * @return none
 *
 */
void wm_delete_screen_all(void)
{
    for (screen_id_e id = SCREEN_ID_WATCHFACE; id < SCREEN_ID_NUMBER; id++)
    {
        if (_g_win_manager.p_active->attr_id_self == id)
            continue;

        if (g_screen_array[id].p_obj_screen)
        {
            if (g_screen_array[id].screen_release)
                g_screen_array[id].screen_release();
            lv_obj_del(g_screen_array[id].p_obj_screen);
            g_screen_array[id].p_obj_screen = NULL;
            g_screen_array[id].p_canvas = NULL;
            g_screen_array[id].attr_init_flag = 0;
        }
    }

    return;
}

/**
 * goto start screen, must called when system start, call once
 *
 * @param id the id of new screen
 * @param param private paramter used by screen
 * @return none
 */
void wm_goto_start_screen(screen_id_e id, uint32_t param)
{
    SCREEN_START(id, param);
    return;
}

static win_call_list_t *_wm_call_stack_ins_head(void)
{
    win_call_list_t *p_callstack_item = NULL;

    p_callstack_item = _lv_ll_ins_head(&_g_win_manager.call_stack_ll);
    APP_ASSERT_CHECK(p_callstack_item);

    return p_callstack_item;
}

static win_call_list_t *_wm_call_stack_ins_tail(void)
{
    win_call_list_t *p_callstack_item = NULL;

    p_callstack_item = _lv_ll_ins_tail(&_g_win_manager.call_stack_ll);
    APP_ASSERT_CHECK(p_callstack_item);

    return p_callstack_item;
}

static win_call_list_t *_wm_call_stack_ins_before(win_call_list_t *p_next)
{
    win_call_list_t *p_callstack_item = NULL;

    p_callstack_item = (p_next != NULL) ? _lv_ll_ins_prev(&_g_win_manager.call_stack_ll, p_next) : _lv_ll_ins_head(&_g_win_manager.call_stack_ll);
    APP_ASSERT_CHECK(p_callstack_item);

    return p_callstack_item;
}

static void _wm_call_stack_rm_tail(void)
{
    win_call_list_t *p_callstack_item = _lv_ll_get_tail(&_g_win_manager.call_stack_ll);

    if (NULL != p_callstack_item)
    {
        _lv_ll_remove(&_g_win_manager.call_stack_ll, p_callstack_item);
        lv_mem_free(p_callstack_item);
    }

    return;
}

static bool _wm_call_stack_is_root(screen_id_e id)
{
    win_call_list_t *p_item_head = _lv_ll_get_head(&_g_win_manager.call_stack_ll);

    return (p_item_head && p_item_head->screen_id == id) ? true : false;
}

static screen_id_e _wm_call_stack_get_parent_by_id(screen_id_e id)
{
    APP_ASSERT_CHECK(id >= SCREEN_ID_INVALID && id < SCREEN_ID_NUMBER);

    screen_id_e id_search = SCREEN_ID_INVALID;
    win_call_list_t *p_item = NULL;
    win_call_list_t *p_prev = NULL;

    win_call_list_t *p_item_tail = _lv_ll_get_tail(&_g_win_manager.call_stack_ll);
    if (NULL != p_item_tail)
    {
        // default return tail id
        id_search = p_item_tail->screen_id;
    }

    win_call_list_t *p_item_head = _lv_ll_get_head(&_g_win_manager.call_stack_ll);
    p_prev = p_item_head;
    _LV_LL_READ(&_g_win_manager.call_stack_ll, p_item)
    {
        if (p_item->screen_id == id && p_item != p_item_head)
        {
            id_search = p_prev->screen_id;
            break;
        }
        p_prev = p_item;
    }

    // error: root id equals id_search
    APP_ASSERT_CHECK(id != id_search);

    return id_search;
}

static screen_id_e _wm_get_prev_screenid(void)
{
    win_call_list_t *p_item = NULL;
    uint32_t index = 0;
    screen_id_e id_self = _g_win_manager.p_active->attr_id_self;

    if (!_wm_call_stack_is_root(id_self))
    {
        _LV_LL_READ_BACK(&_g_win_manager.call_stack_ll, p_item)
        {
            if (++index == 2)
            {
                return p_item->screen_id;
            }
        }
    }

    return SCREEN_ID_INVALID;
}

/**
 * screen change call from 'APP'
 *
 * @param msg_type the type of screen change
 * @param new_id the id of new screen
 * @param param private paramter used by screen
 * @param anim_type type of screen transfer animate
 * @return bool continue or not
 */
bool wm_screen_transition_extern(win_call_type_e call_type, screen_id_e new_id, uint32_t param)
{
    if (WIN_CALL_TYPE_RETURN == call_type)
        new_id = _wm_get_prev_screenid();

    return wm_screen_transition_now(call_type, new_id, param);
}

/**
 * screen change now
 *
 * @param msg_type the type of screen change
 * @param new_id the id of new screen
 * @param param private paramter used by screen
 * @param anim_type type of screen transfer animate
 * @return bool continue or not
 */
bool wm_screen_transition_now(win_call_type_e call_type, screen_id_e new_id, uint32_t param)
{
    bool ret = true;
    bool popup_allowed = false;
    win_call_list_t *p_callstack_item = NULL;

    screen_id_e act_id = _g_win_manager.p_active ? _g_win_manager.p_active->attr_id_self : SCREEN_ID_INVALID;

    switch (call_type)
    {
    case WIN_CALL_TYPE_START:
        _wm_screen_start(new_id, param);
        break;
    case WIN_CALL_TYPE_CHANGE:
        /*current screen call stack:'A -> B'
         * after change to C, C become the root screen
         */
        if (SCREEN_ID_INVALID != new_id)
        {
            _lv_ll_clear(&_g_win_manager.call_stack_ll);
            p_callstack_item = _wm_call_stack_ins_head();
        }
        ret = _wm_screen_trans(call_type, new_id, param);
        break;
    case WIN_CALL_TYPE_CALL:
        /*current screen call stack: 'A -> B'
         * after call C: 'A -> B -> C'
         */
        if (SCREEN_ID_INVALID != new_id) 
        {
            _wm_remove_by_id_ll(new_id);
            p_callstack_item = _wm_call_stack_ins_tail();
        }
        ret = _wm_screen_trans(call_type, new_id, param);
        break;
    case WIN_CALL_TYPE_RETURN:
        /*current screen call stack: 'A -> B -> C'
         * after return: 'A -> B'
         */
        if (_wm_call_stack_is_root(_g_win_manager.p_active->attr_id_self) && new_id == SCREEN_ID_INVALID)
        {
            ret = false;
            break;
        }

        if (new_id == SCREEN_ID_INVALID)
        {
            if (_lv_ll_get_len(&_g_win_manager.call_stack_ll) >= 2)
            {
                _wm_call_stack_rm_tail();
                p_callstack_item = _lv_ll_get_tail(&_g_win_manager.call_stack_ll);
                new_id = p_callstack_item->screen_id;
            }
            else
            {
                printf("[ERROR] It seems to return from the root screen!");
                ret = false;
                break;
            }
        }
        else
        {
            _wm_remove_by_id_ll(act_id);
            _wm_remove_by_id_ll(new_id);
            p_callstack_item = _wm_popup_dealed(new_id, NULL);
        }

        ret = _wm_screen_trans(call_type, new_id, param);
        break;
    case WIN_CALL_TYPE_REPLACE:
        /*current screen call stack: 'A -> B'
         * after replace by C: 'A -> C'
         */
        if (act_id == new_id)
        {
            ret = false;
            break;
        }

        _wm_remove_by_id_ll(act_id);
        _wm_remove_by_id_ll(new_id);

		p_callstack_item = _wm_call_stack_ins_tail();

        ret = _wm_screen_trans(call_type, new_id, param);
        break;
    case WIN_CALL_TYPE_POPUP:
        _wm_remove_by_id_ll(new_id);
        // popup deal
        p_callstack_item = _wm_popup_dealed(new_id, &popup_allowed);

        if (popup_allowed)
            ret = _wm_screen_trans(call_type, new_id, param);

        break;
    default:
        break;
    }
    if (p_callstack_item)
    {
        p_callstack_item->screen_id = new_id;
        p_callstack_item->screen_param = param;
        p_callstack_item->call_type = call_type;
    }

    _debug_win_call_stack(LL_CHANGED_NOW);

    return ret;
}


/****************************
 *     STATIC FUNCTIONS
 ****************************/
/**
 * screen call stack debug, print the call stack info
 *
 * @param none
 * @return none
 *
 */
static void _debug_win_call_stack(ll_changed_reason_e reason)
{
    win_call_list_t *p_item = NULL;
    const char *str_reason;

    switch (reason)
    {
    case LL_CHANGED_THOWOUT:
        str_reason = "Throwout";
        break;
    case LL_CHANGED_LATER:
        str_reason = "Later";
        break;
    case LL_CHANGED_NOW:
        str_reason = "Now";
        break;
    default:
        str_reason = "Error";
        break;
    }

    printf("===========WIN CALL STACK[%s]===========\r", str_reason);
    //printf("===========need change[%s]===========\r", _g_win_manager.call_stack_ll_changed ? "yes" : "no");
    _LV_LL_READ_BACK(&_g_win_manager.call_stack_ll, p_item)
    {
       printf("ID [%d] call type:%d, param:%ld",
             p_item->screen_id, p_item->call_type, p_item->screen_param);
    }
    printf("=====================================\r");

    return;
}

/**
 * goto the first screen when system startup
 *
 * @param id the id of first screen
 * @param param private paramter used by screen
 * @return none
 */
static void _wm_screen_start(screen_id_e id, uint32_t param)
{
    // trans info: last screen id, trans type etc.
    _g_win_manager.id_screen_prev = SCREEN_ID_INVALID;
    _g_win_manager.id_screen_new = id;
    _g_win_manager.call_type = WIN_CALL_TYPE_START;

    // new screen init
    if (NULL == g_screen_array[id].p_obj_screen)
        g_screen_array[id].p_obj_screen = g_screen_array[id].screen_init(param);

    // transfer
    lv_disp_load_scr(g_screen_array[id].p_obj_screen);

    // set active screen
    _g_win_manager.p_active = &g_screen_array[id];

    // init call stack
    _lv_ll_init(&_g_win_manager.call_stack_ll, sizeof(win_call_list_t));
    win_call_list_t *p_callstack_item = _wm_call_stack_ins_head();
    p_callstack_item->screen_id = id;
    p_callstack_item->screen_param = param;
    p_callstack_item->call_type = WIN_CALL_TYPE_START;

    if (g_screen_array[id].screen_delayed_work)
        g_screen_array[id].screen_delayed_work(param);

    return;
}

static bool _wm_screen_trans(win_call_type_e call_type, screen_id_e id_new, uint32_t param)
{
    bool ret = true;
    screen_id_e id_pre = _g_win_manager.p_active->attr_id_self;

    //PERF_DECLARE();
    //PERF_START_TRACE();

    // trans info: last screen id, trans type etc.
    _g_win_manager.id_screen_prev = id_pre;
    _g_win_manager.id_screen_new = id_new;
    _g_win_manager.call_type = call_type;

    // new screen init
    if (id_new != SCREEN_ID_INVALID)
    {
        // screen exit event
        if (g_screen_array[id_pre].screen_event)
        {
            wm_event_t event_exit = {
                .evt_type = EVENT_TYPE_WINDOW,
                .sub_type = WIN_EVT_EXIT,
                .parameter = id_new,
            };
            g_screen_array[id_pre].screen_event(event_exit);
        }

        g_screen_array[id_pre].attr_exit_flag = 1;

        // new screen init
        if (g_screen_array[id_new].attr_delete_flag || (NULL == g_screen_array[id_new].p_obj_screen))
        {
            g_screen_array[id_new].p_obj_screen = g_screen_array[id_new].screen_init(param);
            g_screen_array[id_new].attr_init_flag = 1;
            g_screen_array[id_new].attr_delete_flag = 0;
            printf("init new screen id: %d\n", id_new);
        }

        _lv_inv_area(NULL, NULL);
        _wm_load_new_screen();
        printf("load new screen id: %d\n", id_new);

        // old screen deinit && delete, optimize for memory limit
        if (g_screen_array[id_pre].attr_delete && id_pre != id_new &&
            g_screen_array[id_pre].p_obj_screen)
        {
            if (g_screen_array[id_pre].screen_release)
                g_screen_array[id_pre].screen_release();
            lv_obj_clean(g_screen_array[id_pre].p_obj_screen);
            lv_obj_del(g_screen_array[id_pre].p_obj_screen);
            g_screen_array[id_pre].p_obj_screen = NULL;
            g_screen_array[id_pre].attr_init_flag = 0;
            printf("delete screen id: %d\n", id_pre);
        }

        // if (NULL == g_screen_array[id_pre].p_obj_screen)
        //     _g_win_manager.p_active = &g_screen_array[id_new];
    }

    return ret;
}

static win_call_list_t *_wm_popup_dealed(screen_id_e new_id, bool *popup_allowed)
{
    win_call_list_t *p_callstack_item = NULL;

    // popup deal
    win_call_list_t *p_next_lc = _wm_popup_search_ll(new_id);
    if (p_next_lc) // find insert pos item
    {
        p_callstack_item = _wm_call_stack_ins_before(p_next_lc);
        if (popup_allowed)
            *popup_allowed = false;
    }
    else // not find or list empty
    {
        p_callstack_item = _wm_call_stack_ins_tail();
        if (popup_allowed)
            *popup_allowed = true;
    }

    return p_callstack_item;
}

static win_call_list_t *_wm_popup_search_ll(screen_id_e new_id)
{
    APP_ASSERT_CHECK(new_id >= SCREEN_ID_INVALID && new_id < SCREEN_ID_NUMBER);

    win_call_list_t *p_item = NULL;

    _LV_LL_READ_BACK(&_g_win_manager.call_stack_ll, p_item)
    {
        uint32_t id = p_item->screen_id;

        if (g_screen_array[id].attr_level <= g_screen_array[new_id].attr_level)
        {
            p_item = _lv_ll_get_next(&_g_win_manager.call_stack_ll, p_item);
            break;
        }
    }

    return p_item;
}

static void _wm_remove_by_id_ll(screen_id_e new_id)
{
    APP_ASSERT_CHECK(new_id >= SCREEN_ID_INVALID && new_id < SCREEN_ID_NUMBER);

    win_call_list_t *p_item = NULL;

    _LV_LL_READ(&_g_win_manager.call_stack_ll, p_item)
    {
        if (p_item->screen_id == new_id)
        {
            _lv_ll_remove(&_g_win_manager.call_stack_ll, p_item);
            lv_mem_free(p_item);
            break;
        }
    }

    return;
}

static void  _wm_delete_prev_screen(void)
{
    screen_id_e id_old = _g_win_manager.id_screen_prev;
    screen_id_e id_new = _g_win_manager.id_screen_new;

    if (!g_screen_array[id_old].attr_exit_flag)
    {
        g_screen_array[id_old].attr_exit_flag = 1;
    }

    // old screen deinit && delete
    if (id_old != id_new && g_screen_array[id_old].p_obj_screen)
    {
        g_screen_array[id_old].screen_release();
        lv_obj_del(g_screen_array[id_old].p_obj_screen);
        g_screen_array[id_old].p_obj_screen = NULL;
        g_screen_array[id_old].attr_init_flag = 0;
    }

    return;
}

static void _wm_load_new_screen(void)
{
    //PERF_DECLARE();
    screen_id_e id_new = _g_win_manager.id_screen_new;
    //PERF_START_TRACE();

    // new screen init if needed
    if (NULL == g_screen_array[id_new].p_obj_screen)
    {
        g_screen_array[id_new].p_obj_screen = g_screen_array[id_new].screen_init(0);
        printf("screen[%d] init_top: %d ticks", id_new);
        g_screen_array[id_new].attr_init_flag = 1;
    }

    // load new id immmediately
    lv_disp_load_scr(g_screen_array[id_new].p_obj_screen);

    // screen enter event
    if (g_screen_array[id_new].screen_event)
    {
        wm_event_t event_enter = {
            .evt_type = EVENT_TYPE_WINDOW,
            .sub_type = WIN_EVT_ENTER,
            .parameter = _g_win_manager.id_screen_prev,
        };
        g_screen_array[id_new].screen_event(event_enter);
    }

    // set active screen
    _g_win_manager.p_active = &g_screen_array[id_new];
    g_screen_array[id_new].attr_exit_flag = 0;

    // refr now
    lv_refr_now(NULL);
    printf("screen[%d] refr now: %d ticks", id_new);

    // delayed work
    if (g_screen_array[id_new].screen_delayed_work)
    {
        g_screen_array[id_new].screen_delayed_work(0);
        printf("screen[%d] delay work: %d ticks", id_new);
    }

    return;
}
