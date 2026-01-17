#include "app_cardiogram.h"
#include "app_container.h"
#include "ui_win_message.h"
#include "app_service_simulate.h"
#include "gh_demo_config.h"

#define MEASURE_TIME 40
#define MEASURE_ANIM_SPEED 20

#define MEASURE_ANIM_POINTS_NUM 175
#define MEASURE_ANIM_WIOTH 350
#define MEASURE_ANIM_HEIGHT 120
#define DOT_NUM 4
#define MEASURE_ANIM_RAW_LEN (DOT_NUM * MEASURE_ANIM_SPEED * MEASURE_TIME)


#define LV_IMG_CF         LV_IMG_CF_GDX_RGB565
#define LV_IMG_CF_SIZE    2

typedef struct
{
    lv_obj_t* canvas;
    uint8_t* canvas_buf;

    uint16_t raw_record;
    uint16_t raw_read_pos;

    lv_coord_t x[MEASURE_ANIM_POINTS_NUM];
    lv_coord_t y[MEASURE_ANIM_POINTS_NUM];

    uint8_t raw_buf[MEASURE_ANIM_RAW_LEN];
} module_anim_t;

static module_anim_t* g_module_anim = NULL;

static lv_obj_t* screen = NULL;
static lv_obj_t* hr_text = NULL;
static lv_obj_t* countdown_text = NULL;
static lv_timer_t* anim_timer = NULL;
static lv_timer_t* countdown_timer = NULL;
static uint8_t hr_current = 255;
static uint8_t countdown = MEASURE_TIME;
static uint8_t cbuf_comm_chart[LV_CANVAS_BUF_SIZE_ALPHA_8BIT(MEASURE_ANIM_WIOTH, MEASURE_ANIM_HEIGHT) * LV_IMG_CF_SIZE];

static void cardiogram_return(lv_event_t* e);
static void anim_create(lv_event_t* e);
static void cardiogram_update_cb(lv_event_t *e);
static void explain_cb(lv_event_t *e);
static void confirm_cb(lv_event_t *e);
static void understand_cb(lv_event_t *e);
static void anim_release(lv_event_t *e);

static void cardiogram_anim();
static void cardiogram_report();
static void cardiogram_explain();

static lv_obj_t* cardiogram_measure_anim_create(lv_obj_t* parent);
static lv_obj_t* measure_canvas_create(lv_obj_t *parent, module_anim_t* anim);

static void countdown_cb(lv_timer_t* t);
static void measure_anim_cb(lv_timer_t* t);


lv_obj_t* cardiogram_init(uint32_t param)
{
    screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, cardiogram_return, LV_EVENT_GESTURE, NULL);
    lv_obj_add_event_cb(screen, cardiogram_update_cb, LV_EVENT_USER, NULL);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "心电监测");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    lv_obj_t* tips_img = cookie_img_create(screen, &wd_img_app_ecg_gesture);
    lv_obj_add_event_cb(tips_img, anim_create, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(tips_img, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_center(tips_img);

    lv_obj_t* tips_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
    lv_obj_set_width(tips_text, 200);
    lv_label_set_text_fmt(tips_text, "%s", "请轻触侧边电极并保持");
    lv_obj_align(tips_text,  LV_ALIGN_BOTTOM_MID, 0, -20);

    return screen;
}

static void anim_create(lv_event_t* e)
{
    cardiogram_anim();
}

static void cardiogram_anim()
{
    lv_obj_clean(screen);

    cardiogram_set(true);
    app_screen_keep_on(true);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "心电监测");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    lv_obj_t* hr_img = cookie_img_create(screen, &wd_img_app_ecg_heart_30);
    lv_obj_align(hr_img, LV_ALIGN_TOP_LEFT, 117, 84);

    health_measure_anim_t raw_data = {0};
    app_cardiogram_get_current(&raw_data);
    hr_current = raw_data.current_hr;

    hr_text = cookie_label_create(screen, &font_36px, LV_COLOR_WHITE);
    if(hr_current == 0 || hr_current == 255) {
        lv_label_set_text_fmt(hr_text, "--");
    } else {
        lv_label_set_text_fmt(hr_text, "%d", hr_current);
    }
    lv_obj_align_to(hr_text, hr_img, LV_ALIGN_OUT_RIGHT_MID, 8, -2);

    lv_obj_t* hr_unit = cookie_label_create(screen, &font_18px, LV_COLOR_GRAY_WHITE);
    lv_label_set_text_fmt(hr_unit, "%s", "次/分");
    lv_obj_align_to(hr_unit, hr_img, LV_ALIGN_OUT_RIGHT_MID, 62, 2);

    cardiogram_measure_anim_create(screen);

    lv_obj_t* keep_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
    lv_obj_set_width(keep_text, 200);
    lv_label_set_text_fmt(keep_text, "%s", "请保持静止");
    lv_obj_align(keep_text, LV_ALIGN_BOTTOM_MID, 0, -60);

    countdown = MEASURE_TIME;
    countdown_text = cookie_label_create(screen, &font_24px, LV_COLOR_WHITE);
    lv_obj_set_width(countdown_text, 200);
    lv_label_set_text_fmt(countdown_text, "%d秒", countdown);
    lv_obj_align(countdown_text, LV_ALIGN_BOTTOM_MID, 0, -26);

    countdown_timer = lv_timer_create(countdown_cb, 1000, NULL);
    lv_timer_set_repeat_count(countdown_timer, -1);
    lv_timer_ready(countdown_timer);
}

static void cardiogram_report()
{
    lv_obj_clean(screen);

    app_cardiogram_t cardiogram_data = {0};
    app_cardiogram_get_result(&cardiogram_data);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "心电监测报告");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    lv_obj_t* hr_box = cookie_obj_create(screen, 280, 80, LV_DIR_NONE);
    lv_obj_add_event_cb(hr_box, explain_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(hr_box, LV_ALIGN_TOP_MID, 0, 96);
    lv_obj_add_flag(hr_box, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(hr_box, 24, LV_PART_MAIN);
    lv_obj_set_style_bg_color(hr_box, LV_COLOR_GRAY, LV_PART_MAIN);

    lv_obj_t* box_title_text = cookie_label_create(hr_box, &font_24px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(box_title_text, "%s", "窦性心律");
    lv_obj_align(box_title_text, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_flag(box_title_text, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t* hr_img = cookie_img_create(hr_box, &wd_img_app_ecg_heart_24);
    lv_obj_align(hr_img, LV_ALIGN_TOP_LEFT, 10, 48);
    lv_obj_add_flag(hr_img, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t* avg_hr = cookie_label_create(hr_box, &font_20px, LV_COLOR_GRAY_WHITE);

    if(cardiogram_data.avg_hr == 0 || cardiogram_data.avg_hr == 255) {
        lv_label_set_text_fmt(avg_hr, " 平均--次/分", cardiogram_data.avg_hr);
    } else {
        lv_label_set_text_fmt(avg_hr, " 平均%d次/分", cardiogram_data.avg_hr);
    }
    lv_obj_align_to(avg_hr, hr_img, LV_ALIGN_OUT_RIGHT_MID, -4, 2);
    lv_obj_add_flag(avg_hr, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t* explain_img = cookie_img_create(hr_box, &wd_img_app_ecg_exclamation);
    lv_obj_align(explain_img, LV_ALIGN_RIGHT_MID, -10, 0);

    lv_obj_t* conclusion_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);

    switch (cardiogram_data.cardiogram_status)
    {
    case 0:
        lv_label_set_text_fmt(conclusion_text, "%s", "此心电图未出现房颤现象");
        break;

    case 1:
        lv_label_set_text_fmt(conclusion_text, "%s", "此心电图心动过缓");
        break;

    case 2:
        lv_label_set_text_fmt(conclusion_text, "%s", "此心电图心动过速");
        break;

    default:
        lv_label_set_text_fmt(conclusion_text, "%s", "无心电图");
        break;
    }
    lv_obj_align(conclusion_text, LV_ALIGN_TOP_LEFT, 40, 192);

    lv_obj_t* confirm = cookie_img_create(screen, &wd_img_app_ecg_confirm);
    lv_obj_add_event_cb(confirm, confirm_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(confirm, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_flag(confirm, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(confirm, 20);

}

static void cardiogram_explain()
{
    lv_obj_clean(screen);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(title_text, "%s", "了解窦性心率");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    lv_obj_t* explain = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
    lv_obj_set_width(explain, 280);
    lv_label_set_text_fmt(explain, "%s", "窦性心率意味着心脏以规律的模式跳动。当心脏的上下腔室同步跳动时，就会出现这个情况。窦性心率的结果仅适用于该特定记录，并不意味着你的心率一直处于规律模式。");
    lv_obj_align(explain, LV_ALIGN_TOP_MID, 0, 86);

    lv_obj_t* understand = cookie_img_create(screen, &wd_img_app_ecg_understand);
    lv_obj_add_event_cb(understand, understand_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(understand, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_flag(understand, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(understand, 20);
}

static void explain_cb(lv_event_t *e)
{
    cardiogram_explain();
}

static void confirm_cb(lv_event_t *e)
{
    SCREEN_RETURN(0, 0);
}

static void understand_cb(lv_event_t *e)
{
    cardiogram_report();
}

static void cardiogram_update_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_USER) {
        ser_event_msg_t* data = e->param;

        COOKIE_LOG("Cookie EVENT is %d \n", data->msg_type);
        switch (data->msg_type)
        {
            case EVENT_ELECTRODE_TOUCH:
                //cardiogram_anim();
                break;

            case EVENT_ELECTRODE_REMOVED:
                // cardiogram_report();
                break;

            case EVENT_ELECTRODE_DATA:
                if(g_module_anim != NULL) {
                    health_measure_anim_t raw_data = {0};
                    app_cardiogram_get_current(&raw_data);

                    hr_current = raw_data.current_hr;
                    if((g_module_anim->raw_record + raw_data.length) >= MEASURE_ANIM_RAW_LEN) {
                        COOKIE_LOG("Cookie points exceeded\n");
                        return;
                    }
                    memcpy(&g_module_anim->raw_buf[g_module_anim->raw_record], raw_data.raw, raw_data.length * sizeof(uint16_t));
                    g_module_anim->raw_record += raw_data.length;
                    COOKIE_LOG("Cookie raw_record is %d \n", g_module_anim->raw_record);
                }
                break;

            default:
                break;
        }
    }
}

void cardiogram_release(void)
{
    screen = NULL;
    hr_text = NULL;
    countdown_text = NULL;
    hr_current = 255;
    countdown = MEASURE_TIME;

    if(g_module_anim) {
        lv_mem_free(g_module_anim);
        g_module_anim = NULL;
    }

    if(countdown_timer) {
        lv_timer_del(countdown_timer);
        countdown_timer = NULL;
    }


    if(anim_timer) {
        lv_timer_del(anim_timer);
        anim_timer = NULL;
    }

    app_screen_keep_on(false);
    cardiogram_set(false);
}

static void cardiogram_return(lv_event_t* e)
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

lv_obj_t* cardiogram_measure_anim_create(lv_obj_t* parent)
{
    if(g_module_anim == NULL) {
        module_anim_t* anim = lv_mem_alloc(sizeof(module_anim_t));
        if(anim == NULL) {
            return;
        }
        lv_memset_00(anim, sizeof(module_anim_t));
        g_module_anim = anim;
    }

    lv_obj_t* canvas = measure_canvas_create(parent, g_module_anim);

    anim_timer = lv_timer_create(measure_anim_cb, 1000 / MEASURE_ANIM_SPEED, NULL);
    lv_timer_set_repeat_count(anim_timer, -1);
    lv_timer_ready(anim_timer);

    lv_obj_add_event_cb(parent, anim_release, LV_EVENT_DELETE, NULL);

    return canvas;
}

static void anim_release(lv_event_t* e)
{
    if(g_module_anim) {
        lv_mem_free(g_module_anim);
        g_module_anim = NULL;
    }
}

static void measure_anim_cb(lv_timer_t* t)
{
    if(g_module_anim == NULL) {
        return;
    }

    if(g_module_anim->raw_read_pos >= MEASURE_ANIM_RAW_LEN || g_module_anim->raw_read_pos >= g_module_anim->raw_record) {
        COOKIE_LOG("Cookie no points to draw\n");
        return;
    }
#if 0
    uint16_t step = g_module_anim->raw_buf[g_module_anim->raw_read_pos];
    g_module_anim->raw_read_pos ++;
    COOKIE_LOG("Cookie raw_read_pos is %d \n", g_module_anim->raw_read_pos);
    g_module_anim->y[MEASURE_ANIM_POINTS_NUM - 2] = g_module_anim->y[MEASURE_ANIM_POINTS_NUM - 1];
    g_module_anim->y[MEASURE_ANIM_POINTS_NUM - 1] = step;
#endif
    g_module_anim->y[MEASURE_ANIM_POINTS_NUM - 4] = g_module_anim->y[MEASURE_ANIM_POINTS_NUM - 1];
    g_module_anim->y[MEASURE_ANIM_POINTS_NUM - 3] = g_module_anim->raw_buf[g_module_anim->raw_read_pos - 2];
    g_module_anim->y[MEASURE_ANIM_POINTS_NUM - 2] = g_module_anim->raw_buf[g_module_anim->raw_read_pos - 1];
    g_module_anim->y[MEASURE_ANIM_POINTS_NUM - 1] = g_module_anim->raw_buf[g_module_anim->raw_read_pos];

    g_module_anim->raw_read_pos ++;
    g_module_anim->raw_read_pos ++;
    g_module_anim->raw_read_pos ++;
    COOKIE_LOG("Cookie raw_read_pos is %d \n", g_module_anim->raw_read_pos);

    uint8_t* pcbuf = g_module_anim->canvas_buf;

    const uint32_t ratio = (uint8_t)((float)MEASURE_ANIM_WIOTH / MEASURE_ANIM_POINTS_NUM) * (DOT_NUM - 1);
    const uint32_t h = MEASURE_ANIM_HEIGHT;
    const uint32_t w = MEASURE_ANIM_WIOTH;

    for(uint32_t i = 0; i < h; i++) {
        memmove(&pcbuf[i * w * LV_IMG_CF_SIZE], &pcbuf[i * w * LV_IMG_CF_SIZE + ratio * LV_IMG_CF_SIZE], (w - ratio) * LV_IMG_CF_SIZE);
        lv_memset_00(&pcbuf[(i * w * LV_IMG_CF_SIZE) + (w - ratio) * LV_IMG_CF_SIZE], ratio * LV_IMG_CF_SIZE);
    }

    lv_draw_line_dsc_t dash_line_dsc;
    lv_draw_line_dsc_init(&dash_line_dsc);
    dash_line_dsc.width = 1;
    dash_line_dsc.dash_gap = 7;
    dash_line_dsc.dash_width = 1;
    dash_line_dsc.opa = LV_OPA_COVER;
    dash_line_dsc.color = LV_COLOR_RED;

    lv_coord_t dash_line_x[2] = {349, 349};
    lv_coord_t dash_line_y[2] = {0,   120};
    lv_canvas_draw_line2(g_module_anim->canvas, dash_line_x, dash_line_y, 2, &dash_line_dsc);

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.width = 2;
    line_dsc.opa = LV_OPA_COVER;
    line_dsc.color = LV_COLOR_RED;
    line_dsc.round_start = 1;
    line_dsc.round_end = 1;

    lv_canvas_draw_line2(g_module_anim->canvas, &g_module_anim->x[MEASURE_ANIM_POINTS_NUM - DOT_NUM], &g_module_anim->y[MEASURE_ANIM_POINTS_NUM - DOT_NUM], DOT_NUM, &line_dsc);
}

static lv_obj_t* measure_canvas_create(lv_obj_t *parent, module_anim_t* anim)
{
    if(anim == NULL) {
        return NULL;
    }

    anim->raw_read_pos = 0;
    anim->raw_record = 0;
    memset(anim->raw_buf, MEASURE_ANIM_HEIGHT / 2, MEASURE_ANIM_RAW_LEN);

    for(uint8_t i = 0; i < MEASURE_ANIM_POINTS_NUM; i++) {
        anim->y[i] = MEASURE_ANIM_HEIGHT / 2;
        anim->x[i] = (lv_coord_t)((float)MEASURE_ANIM_WIOTH / MEASURE_ANIM_POINTS_NUM * i);
    }
    lv_memset_00(cbuf_comm_chart, LV_CANVAS_BUF_SIZE_ALPHA_8BIT(MEASURE_ANIM_WIOTH, MEASURE_ANIM_HEIGHT) * LV_IMG_CF_SIZE);
    anim->canvas_buf = cbuf_comm_chart;

    lv_obj_t* canvas = lv_canvas_create(parent);
    lv_canvas_set_buffer(canvas, cbuf_comm_chart, MEASURE_ANIM_WIOTH, MEASURE_ANIM_HEIGHT, LV_IMG_CF);
    lv_canvas_fill_bg(canvas, LV_COLOR_BLACK, LV_OPA_COVER);
    lv_obj_center(canvas);

    lv_draw_line_dsc_t dash_line_dsc;
    lv_draw_line_dsc_init(&dash_line_dsc);
    dash_line_dsc.width = 1;
    dash_line_dsc.dash_gap = 7;
    dash_line_dsc.dash_width = 1;
    dash_line_dsc.opa = LV_OPA_COVER;
    dash_line_dsc.color = LV_COLOR_RED;

    lv_coord_t dash_line_x[2] = {0};
    lv_coord_t dash_line_y[2] = {0};

    for(uint8_t i = 0; i < 50; i++)
    {
        dash_line_x[0] = i * (MEASURE_ANIM_WIOTH / 50);
        dash_line_y[0] = 0;

        dash_line_x[1] = i * (MEASURE_ANIM_WIOTH / 50);
        dash_line_y[1] = MEASURE_ANIM_HEIGHT;

      lv_canvas_draw_line2(canvas, dash_line_x, dash_line_y, 2, &dash_line_dsc);
    }

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.width = 2;
    line_dsc.opa = LV_OPA_COVER;
    line_dsc.color = LV_COLOR_RED;
    line_dsc.round_start = 1;
    line_dsc.round_end = 1;
    lv_canvas_draw_line2(canvas, anim->x, anim->y, MEASURE_ANIM_POINTS_NUM, &line_dsc);

    anim->canvas = canvas;
    return canvas;
}

static void countdown_cb(lv_timer_t* t)
{
    if(countdown_text && hr_text) {
        if(hr_current == 0 || hr_current == 255) {
            lv_label_set_text_fmt(hr_text, "--");
        } else {
            lv_label_set_text_fmt(hr_text, "%d", hr_current);
        }
        lv_obj_invalidate(hr_text);

        countdown = countdown - 1;
        COOKIE_LOG("Cookie countdown is %d\n", countdown);
        lv_label_set_text_fmt(countdown_text, "%d秒",countdown);
        lv_obj_invalidate(countdown_text);

        if(countdown == 0) {
            lv_timer_pause(countdown_timer);
            lv_timer_pause(anim_timer);
            app_screen_keep_on(false);
            cardiogram_set(false);
            countdown = MEASURE_TIME;
            cardiogram_report();
        }
    }
}
