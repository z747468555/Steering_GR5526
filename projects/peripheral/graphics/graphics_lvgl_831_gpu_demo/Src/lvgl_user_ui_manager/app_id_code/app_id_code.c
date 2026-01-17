#include "app_id_code.h"
#include "app_service_simulate.h"
#include "lv_qrcode.h"
// Cookie is ok

static void id_code_return(lv_event_t* e);

lv_obj_t* id_code_init(uint32_t param)
{
    lv_obj_t* screen = cookie_obj_create(NULL, LV_HOR_RES, LV_VER_RES, LV_DIR_VER);
    lv_obj_add_event_cb(screen, id_code_return, LV_EVENT_GESTURE, NULL);

    app_id_code_t id_code = {0};
    gui_id_code_get_data(&id_code);

    lv_obj_t* title_text = cookie_label_create(screen, &font_26px, LV_COLOR_WHITE);
    lv_obj_set_width(title_text, 200);
    lv_label_set_text_fmt(title_text, "%s", "身份码");
    lv_obj_align(title_text, LV_ALIGN_TOP_MID, 0, 34);

    lv_obj_t* mac_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
    lv_obj_set_width(mac_text, 250);
    lv_label_set_text_fmt(mac_text, "MAC: %s", id_code.mac);
    lv_obj_align(mac_text, LV_ALIGN_TOP_MID, 0, 78);

    lv_obj_t * qr = lv_qrcode_create(screen, 120, LV_COLOR_WHITE, LV_COLOR_BLACK);

    char qr_data[300] = {0};
    sprintf(qr_data,"MAC: %s\nTSM: %s\nNFC: %s\nDEV_SN: %s", id_code.mac, id_code.tsm, id_code.nfc, id_code.sn);

    lv_qrcode_update(qr, qr_data, strlen(qr_data));
    lv_obj_align(qr, LV_ALIGN_TOP_MID, 0, 124);

    lv_obj_t* tsm_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
    lv_obj_set_width(tsm_text, 250);
    lv_label_set_text_fmt(tsm_text, "TSM: %s", id_code.tsm);
    lv_obj_align(tsm_text, LV_ALIGN_TOP_MID, 0, 258);

    lv_obj_t* nfc_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
    lv_obj_set_width(nfc_text, 250);
    lv_label_set_text_fmt(nfc_text, "NFC: %s", id_code.nfc);
    lv_obj_align(nfc_text, LV_ALIGN_TOP_MID, 0, 286);

    lv_obj_t* sn_text = cookie_label_create(screen, &font_20px, LV_COLOR_WHITE);
    lv_obj_set_width(sn_text, 250);
    lv_label_set_text_fmt(sn_text, "DEV_SN: %s", id_code.sn);
    lv_obj_align(sn_text, LV_ALIGN_TOP_MID, 0, 316);

    return screen;
}

void id_code_release(void)
{

}

static void id_code_return(lv_event_t* e)
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
