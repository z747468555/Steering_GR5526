#include "app_debug.h"
#include "app_container.h"
#include "service_setting.h"
// Cookie is ok
static bool _g_monkey_start = false;

static void monkey_cb(lv_event_t* e);
static void debug_return(lv_event_t* e);
static void debug_item_create(lv_obj_t* parent, const char* text, lv_event_cb_t event_cb);

typedef struct _gui_debug_item_t
{
    const char* text;
    lv_event_cb_t event_cb;
} gui_debug_item;

typedef enum
{
    DEBUG_INVALID = -1,

    DEBUG_MONKEY,
    DEBUG_CRASH,

    DEBUG_MAX,

} system_set_id;

static void debug_return(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_GESTURE)
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if(dir == LV_DIR_RIGHT)
        {
            SCREEN_RETURN(0, 0);
        }
    }

    lv_indev_wait_release(lv_indev_get_act());
}

static void monkey_cb(lv_event_t* e)
{
    if (_g_monkey_start)
    {
        return;
    }
    _g_monkey_start = true;

    lv_monkey_config_t config;
    lv_monkey_config_init(&config);  // 初始化默认配置

    config.input_range.min = 0;
    config.input_range.max = DISP_HOR_RES - 1;  // 水平方向的范围
    config.period_range.min = 300;
    config.period_range.max = 500;
    config.type = LV_INDEV_TYPE_POINTER;  // 输入设备类型
    lv_monkey_t * monkey = lv_monkey_create(&config);

    /*Start monkey test*/
    lv_monkey_set_enable(monkey, true);
    SCREEN_RETURN(0, 0);
    app_screen_keep_on(true);
}

static void crash_cb(lv_event_t* e)
{
    uint32_t *p = 0x88888888;
    *p = 0xFFFFFFFF;
}

lv_obj_t* debug_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, debug_return, LV_EVENT_GESTURE, NULL);

    lv_obj_t* page = cookie_page_create(screen);
    lv_obj_set_flex_flow(page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_top(page, 30, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(page, 30, LV_PART_MAIN);

    gui_debug_item debug_list[DEBUG_MAX] =
    {
        [DEBUG_MONKEY] = {"monkey", monkey_cb},
        [DEBUG_CRASH] = {"crash",   crash_cb},
    };

    for(uint8_t item = DEBUG_MONKEY; item < DEBUG_MAX; item++)
    {
        debug_item_create(page, debug_list[item].text, debug_list[item].event_cb);
    }

    return screen;
}

void debug_release(void)
{

}

bool debug_monkey_is_start(void)
{
    return _g_monkey_start;
}

static void debug_item_create(lv_obj_t* parent, const char* text, lv_event_cb_t event_cb)
{
    lv_obj_t* button = cookie_obj_create(parent, LV_HOR_RES, 90, LV_DIR_NONE);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(button, 24, LV_PART_MAIN);
    lv_obj_set_style_bg_color(button, LV_COLOR_RED, LV_PART_MAIN);
    lv_obj_add_event_cb(button, event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* label = cookie_label_create(button, &font_20px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(label, "%s", text);
    lv_obj_center(label);
}
