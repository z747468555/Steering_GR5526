#include "ui_win_def.h"
#include "ui_win_manager.h"

#include "app_template/template.h"
#include "app_msg/app_msg.h"
#include "app_watchface/app_watchface.h"
#include "app_body_change/app_body_change.h"
#include "app_brightness/app_brightness.h"
#include "app_cardiogram/app_cardiogram.h"
#include "app_clock_set/app_clock_set.h"
#include "app_alarm/app_alarm.h"
#include "app_time/app_time.h"
#include "app_stopwatch/app_stopwatch.h"
#include "app_continuity/app_continuity.h"
#include "app_environment/app_environment.h"
#include "app_hr_bo/app_hr_bo.h"
#include "app_id_code/app_id_code.h"
#include "app_reminder/app_reminder.h"
#include "app_resting/app_resting.h"
#include "app_shutdown/app_shutdown.h"
#include "app_sos/app_sos.h"
#include "app_sport/app_sport.h"
#include "app_system_set/app_system_set.h"
#include "app_version/app_version.h"
#include "app_debug/app_debug.h"
#include "app_logo/app_logo.h"
#include "app_applist/app_applist.h"
#include "app_ota/app_ota.h"

screen_desc_t g_screen_array[SCREEN_ID_NUMBER] =
{
    SCREEN_LOGO_PAGE,
    SCREEN_WATCHFACE_PAGE,
    SCREEN_APPLIST_PAGE,
	SCREEN_MSG_PAGE,
    SCREEN_TEMPLATE_PAGE,
    SCREEN_BODY_CHANGE_PAGE,
    SCREEN_HR_BO_PAGE,
    SCREEN_SOS_PAGE,
    SCREEN_REMINDER_PAGE,
    SCREEN_CARDIOGRAM_PAGE,
    SCREEN_SPORT_PAGE,
    SCREEN_ENVIRONMENT_PAGE,
    SCREEN_SYSTEM_SET_PAGE,
    SCREEN_CONTINUITY_PAGE,
    SCREEN_BRIGHTNESS_PAGE,
    SCREEN_CLOCK_SET_PAGE,
    SCREEN_ALARM_PAGE,
    SCREEN_TIME_PAGE,
    SCREEN_STOPWATCH_PAGE,
    SCREEN_RESTING_PAGE,
    SCREEN_ID_CODE_PAGE,
    SCREEN_VERSION_PAGE,
    SCREEN_SHUTDOWN_PAGE,
    SCREEN_OTA_PAGE,
    SCREEN_DEBUG_PAGE,
};
