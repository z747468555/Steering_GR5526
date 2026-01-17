#include "service_sport.h"
#include "app_service_simulate.h"
#include "app_mems_task.h"
#include "ui_win_message.h"
#include "ui_win_manager.h"
#include "service_user_data.h"
#include "app_service_task.h"

static uint32_t step_his = 0;
static uint32_t step_read = 0;
static uint32_t step_remain = 0;


void app_sport_step_init()
{
	step_his = 0;
	step_read = 0;
	step_remain = step_get();
}

//计步0点清0，距离每步0.7米，卡路里100步是4卡路李，25步1卡路里
bool app_sport_get_data(app_sport_t* sport_data)
{
	uint32_t step;
	step = step_read + step_his - step_remain;
	sport_data->step = step;
	sport_data->distance = step*7/10;
	sport_data->calorie = step/25 + ((step&25)?1:0);
	return true;
}

void app_sport_step_update()
{
	step_read = step_get();
	send_msg_to_ui_high( EVENT_SPORT_CHANGE, EVENT_SPORT_CHANGE);
}

void app_sport_step_clear()
{
	step_clear();
	app_sport_step_init();
}


//存储计步信息
void service_data_save_step(service_data_sport_t *payload)
{
	payload->step = step_get()+step_his;
}

void service_data_load_step(service_data_sport_t *payload)
{
	step_his = payload->step;
}
