#include "app_body_change.h"
#include "app_container.h"
#include "app_service_simulate.h"
#include "ui_win_message.h"

static void body_change_return(lv_event_t* e);
static void draw_chart_cb(lv_event_t* e);
static body_chart cur_chart = EMOTIONAL;

static lv_obj_t* status_img = NULL;
static lv_obj_t* status_indicator = NULL;
static lv_obj_t* status_text = NULL;

static void show_update_cb(lv_event_t* e)
{
    if (e->code == LV_EVENT_USER) {
        ser_event_msg_t* data = e->param;
        app_body_chart_t body_chart_data = {0};

        switch (data->msg_type)
        {
            case EVENT_BODY_CHANGE:
                switch (cur_chart)
                {
                    case EMOTIONAL:
                        app_body_get_emotion(&body_chart_data);
                        break;

                    case VISUAL:
                        app_body_get_vision(&body_chart_data);
                        break;

                    case MENTAL:
                        app_body_get_mental(&body_chart_data);
                        break;

                    case ENERGY:
                        app_body_get_energy(&body_chart_data);
                        break;

                    case PSYCHOLOGICAL:
                        app_body_get_psychology(&body_chart_data);
                        break;

                    default:
                        break;
                }

                lv_obj_set_style_text_font(status_text, &font_36px, LV_PART_MAIN);
                lv_obj_set_style_text_color(status_text, LV_COLOR_RED, LV_PART_MAIN);
                lv_obj_align(status_text, LV_ALIGN_TOP_MID, 0, 70);

                switch (body_chart_data.level)
                {
                    case 1:
                        lv_label_set_text_fmt(status_text, "%s", "低");
                        if(data->msg_type == ENERGY) {
                            lv_obj_set_style_text_color(status_text, LV_COLOR_RED, LV_PART_MAIN);
                            lv_obj_align_to(status_indicator, status_img, LV_ALIGN_OUT_TOP_RIGHT, -38, -4);
                        } else {
                            lv_obj_set_style_text_color(status_text, LV_COLOR_GREEN, LV_PART_MAIN);
                            lv_obj_align_to(status_indicator, status_img, LV_ALIGN_OUT_TOP_LEFT, 38, -4);
                        }
                        break;

                    case 2:
                        lv_label_set_text_fmt(status_text, "%s", "中");
                        lv_obj_set_style_text_color(status_text, LV_COLOR_YELLOW, LV_PART_MAIN);
                        lv_obj_align_to(status_indicator, status_img, LV_ALIGN_OUT_TOP_MID, 0, -4);
                        break;

                    case 3:
                        lv_label_set_text_fmt(status_text, "%s", "高");
                        if(data->msg_type == ENERGY) {
                            lv_obj_set_style_text_color(status_text, LV_COLOR_GREEN, LV_PART_MAIN);
                            lv_obj_align_to(status_indicator, status_img, LV_ALIGN_OUT_TOP_LEFT, 38, -4);
                        } else {
                            lv_obj_set_style_text_color(status_text, LV_COLOR_RED, LV_PART_MAIN);
                            lv_obj_align_to(status_indicator, status_img, LV_ALIGN_OUT_TOP_RIGHT, -38, -4);
                        }
                        break;

                    default:
                        lv_obj_set_style_text_font(status_text, &font_20px, LV_PART_MAIN);
                        lv_obj_set_style_text_color(status_text, LV_COLOR_GRAY_WHITE, LV_PART_MAIN);
                        lv_label_set_text_fmt(status_text, "%s", "请佩戴手环");
                        lv_obj_align(status_text, LV_ALIGN_TOP_MID, 0, 82);
                        break;
                }
                break;

            default:
                break;
        }
    }

}

lv_obj_t* body_change_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, body_change_return, LV_EVENT_GESTURE, NULL);
    lv_obj_add_event_cb(screen, show_update_cb, LV_EVENT_USER, NULL);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    cur_chart = (body_chart)param;
    app_body_chart_t body_chart_data = {0};

    switch (cur_chart)
    {
        case EMOTIONAL:
            app_body_get_emotion(&body_chart_data);
            lv_label_set_text_fmt(title_text, "%s", "情绪波动");
            break;

        case VISUAL:
            app_body_get_vision(&body_chart_data);
            lv_label_set_text_fmt(title_text, "%s", "视觉疲劳");
            break;

        case MENTAL:
            app_body_get_mental(&body_chart_data);
            lv_label_set_text_fmt(title_text, "%s", "脑力疲劳");
            break;

        case ENERGY:
            app_body_get_energy(&body_chart_data);
            lv_label_set_text_fmt(title_text, "%s", "精力专注");
            break;

        case PSYCHOLOGICAL:
            app_body_get_psychology(&body_chart_data);
            lv_label_set_text_fmt(title_text, "%s", "心理压力");
            break;

        default:
            break;
    }

    status_img = cookie_img_create(screen, &wd_img_chart_low_normal_high_lines);
    lv_obj_set_size(status_img, 279, 4);
    lv_obj_align(status_img, LV_ALIGN_TOP_MID, 0, 140);

    status_indicator = cookie_img_create(screen, &wd_img_downward);

    status_text = cookie_label_create(screen, &font_36px, LV_COLOR_RED);
    lv_obj_set_width(status_text, 200);
    lv_obj_align(status_text, LV_ALIGN_TOP_MID, 0, 70);

    switch (body_chart_data.level)
    {
        case 1:
            lv_label_set_text_fmt(status_text, "%s", "低");
            if(cur_chart == ENERGY) {
                lv_obj_set_style_text_color(status_text, LV_COLOR_RED, LV_PART_MAIN);
                lv_obj_align_to(status_indicator, status_img, LV_ALIGN_OUT_TOP_RIGHT, -38, -4);
            } else {
                lv_obj_set_style_text_color(status_text, LV_COLOR_GREEN, LV_PART_MAIN);
                lv_obj_align_to(status_indicator, status_img, LV_ALIGN_OUT_TOP_LEFT, 38, -4);
            }
            break;

        case 2:
            lv_label_set_text_fmt(status_text, "%s", "中");
            lv_obj_set_style_text_color(status_text, LV_COLOR_YELLOW, LV_PART_MAIN);
            lv_obj_align_to(status_indicator, status_img, LV_ALIGN_OUT_TOP_MID, 0, -4);
            break;

        case 3:
            lv_label_set_text_fmt(status_text, "%s", "高");
            if(cur_chart == ENERGY) {
                lv_obj_set_style_text_color(status_text, LV_COLOR_GREEN, LV_PART_MAIN);
                lv_obj_align_to(status_indicator, status_img, LV_ALIGN_OUT_TOP_LEFT, 38, -4);
            } else {
                lv_obj_set_style_text_color(status_text, LV_COLOR_RED, LV_PART_MAIN);
                lv_obj_align_to(status_indicator, status_img, LV_ALIGN_OUT_TOP_RIGHT, -38, -4);
            }
            break;

        default:
            lv_obj_set_style_text_font(status_text, &font_20px, LV_PART_MAIN);
            lv_obj_set_style_text_color(status_text, LV_COLOR_GRAY_WHITE, LV_PART_MAIN);
            lv_label_set_text_fmt(status_text, "%s", "请佩戴手环");
            lv_obj_align(status_text, LV_ALIGN_TOP_MID, 0, 82);
            break;
    }

    lv_obj_t* high_text = cookie_label_create(screen, &font_18px, LV_COLOR_GRAY_WHITE);
    lv_label_set_text_fmt(high_text, "%s", "高");
    if(cur_chart == ENERGY) {
        lv_obj_align_to(high_text, status_img, LV_ALIGN_OUT_BOTTOM_LEFT, 38, 4);
    } else {
        lv_obj_align_to(high_text, status_img, LV_ALIGN_OUT_BOTTOM_RIGHT, -38, 4);
    }


    lv_obj_t* normal_text = cookie_label_create(screen, &font_18px, LV_COLOR_GRAY_WHITE);
    lv_label_set_text_fmt(normal_text, "%s", "中");
    lv_obj_align_to(normal_text, status_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);

    lv_obj_t* low_text = cookie_label_create(screen, &font_18px, LV_COLOR_GRAY_WHITE);
    lv_label_set_text_fmt(low_text, "%s", "低");
    if(cur_chart == ENERGY) {
        lv_obj_align_to(low_text, status_img, LV_ALIGN_OUT_BOTTOM_RIGHT, -38, 4);
    } else {
        lv_obj_align_to(low_text, status_img, LV_ALIGN_OUT_BOTTOM_LEFT, 38, 4);
    }

    lv_obj_t* chart_bg = cookie_img_create(screen, &wd_img_chart_curves_lines);
    lv_obj_align(chart_bg, LV_ALIGN_TOP_MID, 0, 192);

    lv_obj_t* time_bg = cookie_img_create(screen, &wd_img_chart_time_lines);
    lv_obj_align(time_bg, LV_ALIGN_TOP_MID, 0, 278);

    // lv_obj_t* time = cookie_label_create(screen, &font_12px, LV_COLOR_GRAY_WHITE);
    // lv_label_set_text_fmt(time, "%02d:%02d", body_chart_data.end_time.hour, body_chart_data.end_time.min);
    // lv_obj_align_to(time, time_bg, LV_ALIGN_OUT_BOTTOM_RIGHT, -10, 0);

    lv_obj_add_event_cb(screen, draw_chart_cb, LV_EVENT_DRAW_MAIN_END, NULL);

    return screen;
}

static void draw_chart_cb(lv_event_t* e)
{
    lv_draw_ctx_t* draw_ctx = lv_event_get_draw_ctx(e);

    app_body_chart_t body_chart_data = {0};

    switch (cur_chart)
    {
        case EMOTIONAL:
            app_body_get_emotion(&body_chart_data);
            break;

        case VISUAL:
            app_body_get_vision(&body_chart_data);
            break;

        case MENTAL:
            app_body_get_mental(&body_chart_data);
            break;

        case ENERGY:
            app_body_get_energy(&body_chart_data);
            break;

        case PSYCHOLOGICAL:
            app_body_get_psychology(&body_chart_data);
            break;

        default:
            break;
    }

    // X:42->318
    // Y:192->245
    uint8_t step = (uint8_t)((float)(318-42)/29);
    lv_point_t line_points[30] = {0};

    for(uint8_t i = 0; i < 30; i++) {
        if(i == 29) {
            line_points[i].x = 318;
        } else {
            line_points[i].x = 42 + step * i;
        }

        if(191 < body_chart_data.data[i] && body_chart_data.data[i] < 246) {
            line_points[i].y = body_chart_data.data[i];
        } else {
            line_points[i].y = 218;
        }
    }

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.width = 2;
    line_dsc.opa = LV_OPA_COVER;
    line_dsc.color = LV_COLOR_BLUE;
    line_dsc.round_start = 1;
    line_dsc.round_end = 1;

    for(uint8_t i = 0; i < 29; i ++) {
        lv_draw_line(draw_ctx, &line_dsc, &line_points[i], &line_points[i+1]);
    }
}

void body_change_release(void)
{
    status_img = NULL;
    status_indicator = NULL;
    status_text = NULL;
}

static void body_change_return(lv_event_t* e)
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
