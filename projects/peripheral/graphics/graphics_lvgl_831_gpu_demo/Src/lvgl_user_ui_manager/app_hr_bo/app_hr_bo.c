#include "app_hr_bo.h"
#include "app_service_simulate.h"
#include "ui_win_message.h"

static lv_obj_t* page = NULL;
static lv_obj_t* hr_data = NULL;
static lv_obj_t* hr_unit = NULL;
static lv_obj_t* hr_max_data = NULL;
static lv_obj_t* hr_min_data = NULL;
static lv_obj_t* bo_data = NULL;

static void hr_bo_return(lv_event_t* e);
static void hr_bo_chart_cb(lv_event_t* e);
static void draw_chart_cb(lv_event_t* e);
static void show_update_cb(lv_event_t* e);

static void show_update_cb(lv_event_t* e)
{
    if(hr_data && bo_data) {
        if (e->code == LV_EVENT_USER) {
            ser_event_msg_t* data = e->param;

            app_hr_bo_t hr_bo_data = {0};
            app_hr_bo_get_chart(&hr_bo_data);

            switch (data->msg_type)
            {
                case EVENT_HR_CHANGE:
                    if(hr_bo_data.current_hr == 0 || hr_bo_data.current_hr == 255) {
                        lv_label_set_text_fmt(hr_data, "--");
                    } else {
                        lv_label_set_text_fmt(hr_data, "%d", hr_bo_data.current_hr);
                    }
                    lv_obj_align(hr_data, LV_ALIGN_TOP_MID, 0, 15);
                    lv_obj_align_to(hr_unit, hr_data, LV_ALIGN_OUT_RIGHT_MID, 6, 12);

                    if(hr_bo_data.max_hr == 0 || hr_bo_data.max_hr == 255) {
                        lv_label_set_text_fmt(hr_max_data, "--");
                    } else {
                        lv_label_set_text_fmt(hr_max_data, "%d", hr_bo_data.max_hr);
                    }

                    if(hr_bo_data.min_hr == 0 || hr_bo_data.min_hr == 255) {
                        lv_label_set_text_fmt(hr_min_data, "--");
                    } else {
                        lv_label_set_text_fmt(hr_min_data, "%d", hr_bo_data.min_hr);
                    }

                    if(hr_bo_data.current_bo == 0 || hr_bo_data.current_bo == 255) {
                        lv_label_set_text_fmt(bo_data, "血氧--%%");
                    } else {
                        lv_label_set_text_fmt(bo_data, "血氧%d%%", hr_bo_data.current_bo);
                    }
                    break;

                default:
                    break;
            }
        }
    }
}

lv_obj_t* hr_bo_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, hr_bo_return, LV_EVENT_GESTURE, NULL);
    lv_obj_add_event_cb(screen, show_update_cb, LV_EVENT_USER, NULL);

    page = cookie_page_create(screen);

    lv_obj_t* tips_img = cookie_img_create(page, &wd_img_wear_remind);
    lv_obj_add_event_cb(tips_img, hr_bo_chart_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(tips_img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(tips_img);

    lv_obj_t* tips_text = cookie_label_create(page, &font_18px, LV_COLOR_WHITE);
    lv_obj_set_width(tips_text, 200);
    lv_label_set_text_fmt(tips_text, "%s", "请将穿戴设备正面向上佩戴在佩戴在腕骨靠后一指位置");
    lv_obj_align(tips_text,  LV_ALIGN_BOTTOM_MID, 0, -14);

    return screen;
}

static void hr_anim_exec_angle_cb(void *obj, int32_t value)
{
	lv_img_set_zoom(obj,value);
}

static void hr_bo_chart_cb(lv_event_t* e)
{
    lv_obj_clean(page);
    app_hr_bo_t hr_bo_data = {0};
    app_hr_bo_get_chart(&hr_bo_data);

    lv_obj_t* hr_img = cookie_img_create(page, &wd_img_hr_icon);
    lv_obj_align(hr_img, LV_ALIGN_TOP_LEFT, 70, 64);

    lv_anim_t hr_anim;
	lv_anim_init(&hr_anim);
	lv_anim_set_var(&hr_anim,hr_img);
	lv_anim_set_values(&hr_anim,128,300);
	lv_anim_set_exec_cb(&hr_anim,hr_anim_exec_angle_cb);
	lv_anim_set_time(&hr_anim,800);
	lv_anim_set_repeat_count(&hr_anim,-1);
	lv_anim_start(&hr_anim);

    hr_data = cookie_label_create(page, &font_72px, LV_COLOR_RED);
    if(hr_bo_data.current_hr == 0 || hr_bo_data.current_hr == 255) {
        lv_label_set_text_fmt(hr_data, "--");
    } else {
        lv_label_set_text_fmt(hr_data, "%d", hr_bo_data.current_hr);
    }
    lv_obj_align(hr_data, LV_ALIGN_TOP_MID, 0, 15);

    hr_unit = cookie_label_create(page, &font_18px, LV_COLOR_GRAY_WHITE);
    lv_label_set_text_fmt(hr_unit, "%s", "次/分");
    lv_obj_align_to(hr_unit, hr_data, LV_ALIGN_OUT_RIGHT_MID, 6, 12);

    bo_data = cookie_label_create(page, &font_20px, LV_COLOR_WHITE);
    if(hr_bo_data.current_bo == 0 || hr_bo_data.current_bo == 255) {
        lv_label_set_text_fmt(bo_data, "血氧--%%");
    } else {
        lv_label_set_text_fmt(bo_data, "血氧%d%%", hr_bo_data.current_bo);
    }
    lv_obj_align_to(bo_data, hr_data, LV_ALIGN_OUT_BOTTOM_MID, 0, -5);

    lv_obj_t* hr_chart_bg = cookie_img_create(page, &wd_img_hr_lines);
    lv_obj_align(hr_chart_bg, LV_ALIGN_TOP_MID, 0, 156);

    lv_obj_t* hr_max_show = cookie_label_create(page, &font_12px, LV_COLOR_GRAY_WHITE);
    lv_label_set_text_fmt(hr_max_show, "%d", 200);
    lv_obj_align_to(hr_max_show, hr_chart_bg, LV_ALIGN_OUT_RIGHT_TOP, 4, -10);

    lv_obj_t* hr_min_show = cookie_label_create(page, &font_12px, LV_COLOR_GRAY_WHITE);
    lv_label_set_text_fmt(hr_min_show, "%d", 49);
    lv_obj_align_to(hr_min_show, hr_chart_bg, LV_ALIGN_OUT_RIGHT_BOTTOM, 4, 10);

    lv_obj_t* min_00 = cookie_label_create(page, &font_16px, LV_COLOR_GRAY_WHITE);
    lv_label_set_text_fmt(min_00, "%dmin", 00);
    lv_obj_align_to(min_00, hr_chart_bg, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 9);

    lv_obj_t* min_30 = cookie_label_create(page, &font_16px, LV_COLOR_GRAY_WHITE);
    lv_label_set_text_fmt(min_30, "%dmin", 30);
    lv_obj_align_to(min_30, hr_chart_bg, LV_ALIGN_OUT_BOTTOM_MID, 0, 9);

    lv_obj_t* min_60 = cookie_label_create(page, &font_16px, LV_COLOR_GRAY_WHITE);
    lv_label_set_text_fmt(min_60, "%dmin", 60);
    lv_obj_align_to(min_60, hr_chart_bg, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 9);

    lv_obj_t* hr_max = cookie_label_create(page, &font_24px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(hr_max, "%s", "最高心率");
    lv_obj_align(hr_max, LV_ALIGN_TOP_LEFT, 70, 274);

    hr_max_data = cookie_label_create(page, &font_36px, LV_COLOR_RED);
    if(hr_bo_data.max_hr == 0 || hr_bo_data.max_hr == 255) {
        lv_label_set_text_fmt(hr_max_data, "--");
    } else {
        lv_label_set_text_fmt(hr_max_data, "%d", hr_bo_data.max_hr);
    }
    lv_obj_align_to(hr_max_data, hr_max, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, -8);

    lv_obj_t* hr_min = cookie_label_create(page, &font_24px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(hr_min, "%s", "最低心率");
    lv_obj_align(hr_min, LV_ALIGN_TOP_RIGHT, -70, 274);

    hr_min_data = cookie_label_create(page, &font_36px, LV_COLOR_WHITE);
    if(hr_bo_data.min_hr == 0 || hr_bo_data.min_hr == 255) {
        lv_label_set_text_fmt(hr_min_data, "--");
    } else {
        lv_label_set_text_fmt(hr_min_data, "%d", hr_bo_data.min_hr);
    }
    lv_obj_align_to(hr_min_data, hr_min, LV_ALIGN_OUT_BOTTOM_LEFT, 0, -8);

    lv_obj_add_event_cb(page, draw_chart_cb, LV_EVENT_DRAW_MAIN_END, NULL);
}

static void draw_chart_cb(lv_event_t* e)
{
    lv_draw_ctx_t* draw_ctx = lv_event_get_draw_ctx(e);

    app_hr_bo_t hr_chart_data = {0};
    app_hr_bo_get_chart(&hr_chart_data);

    // X:50->304
    // Y:156->224
    uint8_t step = (uint8_t)((float)(304-50)/29);
    lv_point_t line_points[30] = {0};

    for(uint8_t i = 0; i < 30; i++) {
        if(i == 29) {
            line_points[i].x = 304;
        } else {
            line_points[i].x = 50 + step * i;
        }

        if(155 < hr_chart_data.data[i] && hr_chart_data.data[i] < 225) {
            line_points[i].y = hr_chart_data.data[i];
        } else {
            line_points[i].y = 255;
        }
    }

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.width = 2;
    line_dsc.opa = LV_OPA_COVER;
    line_dsc.color = LV_COLOR_RED;
    line_dsc.round_start = 1;
    line_dsc.round_end = 1;

    for(uint8_t i = 0; i < 29; i ++) {
        if(line_points[i].y != 255 && line_points[i + 1].y != 255) {
            lv_draw_line(draw_ctx, &line_dsc, &line_points[i], &line_points[i + 1]);
        }
    }
}
void hr_bo_release(void)
{
    page = NULL;
    hr_data = NULL;
    hr_unit = NULL;
    hr_max_data = NULL;
    hr_min_data = NULL;
    bo_data = NULL;
}

static void hr_bo_return(lv_event_t* e)
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
