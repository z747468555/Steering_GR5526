#include "app_ota.h"
#include "app_service_task.h"

#define OTA_TIME 60000

void ota_start_cb(void *obj, int32_t value)
{
	lv_label_set_text_fmt(obj, "%d%%", value);
	lv_obj_center(obj);
}

void ota_end_cb(struct _lv_anim_t *anim)
{
	lv_anim_del(anim,ota_start_cb);
	app_screen_keep_on(false);
    service_sys_reset();
}

static void set_angle(void * obj, int32_t v)
{
    lv_arc_set_value(obj, v);
}

lv_obj_t* ota_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);

    lv_obj_t * arc = lv_arc_create(screen);
    lv_obj_set_style_arc_color(arc, LV_COLOR_GRAY_WHITE, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 6, LV_PART_INDICATOR);
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(arc, 360, 360);
    lv_obj_center(arc);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc);
    lv_anim_set_exec_cb(&a, set_angle);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&a, 1000);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_start(&a);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "版本升级中");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    lv_obj_t* countdown_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
    lv_label_set_text_fmt(countdown_text, "%d%%", 0);
    lv_obj_center(countdown_text);

    lv_anim_t ota_anim;
	lv_anim_init(&ota_anim);
	lv_anim_set_var(&ota_anim, countdown_text);
	lv_anim_set_values(&ota_anim,0 , 101);
	lv_anim_set_exec_cb(&ota_anim, ota_start_cb);
	lv_anim_set_ready_cb(&ota_anim, ota_end_cb);
	lv_anim_set_time(&ota_anim, OTA_TIME);
	lv_anim_set_repeat_count(&ota_anim, 0);
	lv_anim_start(&ota_anim);

	app_screen_keep_on(true);

    return screen;
}

void ota_release(void)
{

}


