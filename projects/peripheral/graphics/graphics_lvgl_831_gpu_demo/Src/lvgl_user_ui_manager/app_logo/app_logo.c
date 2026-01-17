#include "app_logo.h"
#include "ui_win_manager.h"
#include "ui_win_message.h"
#define APP_LOG_TAG "logo"
#include "app_log.h"

static lv_img_dsc_t  wd_img_logo_psram = {.data = NULL,};

void logo_anim_exec_angle_cb(void *obj, int32_t value)
{
	lv_img_set_angle(obj,value*100);
	lv_obj_center(obj);
	lv_obj_update_layout(obj);
}

void logo_anim_ready_cb(struct _lv_anim_t *anim)
{
	lv_anim_del(anim,logo_anim_exec_angle_cb);
	app_screen_keep_on(false);
	wm_goto_start_screen(SCREEN_ID_WATCHFACE,0);
}

lv_obj_t* app_logo_init(uint32_t param)
{
	lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);

	memcpy(&wd_img_logo_psram, &wd_img_logo,   sizeof(lv_img_dsc_t));
	if(wd_img_logo_psram.data != NULL) wd_img_logo_psram.data = app_graphics_mem_malloc(wd_img_logo_psram.data_size);
	lv_port_res_mode_set(2);
	memcpy((void*)wd_img_logo_psram.data, (void*)wd_img_logo.data, wd_img_logo_psram.data_size);

	lv_obj_t* logo_prev_cont = lv_img_create(screen);
	lv_img_set_size_mode(logo_prev_cont, LV_IMG_SIZE_MODE_REAL);
	lv_obj_set_size(logo_prev_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_img_set_src(logo_prev_cont, &wd_img_logo_psram);
	lv_img_set_pivot(logo_prev_cont,90,90);
	lv_obj_center(logo_prev_cont);

	lv_anim_t logo_anim;
	lv_anim_init(&logo_anim);
	lv_anim_set_var(&logo_anim,logo_prev_cont);
	lv_anim_set_values(&logo_anim,0,36);
	lv_anim_set_exec_cb(&logo_anim,logo_anim_exec_angle_cb);
	lv_anim_set_ready_cb(&logo_anim,logo_anim_ready_cb);
	lv_anim_set_time(&logo_anim,5000);
	lv_anim_set_repeat_count(&logo_anim,0);
	lv_anim_start(&logo_anim);

	app_screen_keep_on(true);

    return screen;
}

void app_logo_release(void)
{
	if(wd_img_logo_psram.data != NULL) {
        app_graphics_mem_free((void*)wd_img_logo_psram.data);
        wd_img_logo_psram.data = NULL;
    }
}

