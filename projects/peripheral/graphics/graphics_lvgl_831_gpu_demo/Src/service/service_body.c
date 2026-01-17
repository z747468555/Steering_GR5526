#include "service_body.h"
#include "service_reform.h"
#include "ui_win_message.h"
#include "ui_win_manager.h"


#define APP_BODY_DATA_CNT	30

uint8_t app_body_emotion_data[APP_BODY_DATA_CNT] = {0};
uint8_t app_body_vision_data[APP_BODY_DATA_CNT] = {0};
uint8_t app_body_mental_data[APP_BODY_DATA_CNT] = {0};	//
uint8_t app_body_energy_data[APP_BODY_DATA_CNT] = {0};
uint8_t app_body_psychology_data[APP_BODY_DATA_CNT] = {0};

app_time_t app_body_emotion_end_time;
app_time_t app_body_vision_end_time;
app_time_t app_body_mental_end_time;
app_time_t app_body_energy_end_time;
app_time_t app_body_psychology_end_time;

uint8_t app_body_emotion_level;
uint8_t app_body_vision_level;
uint8_t app_body_mental_level;
uint8_t app_body_energy_level;
uint8_t app_body_psychology_level;


bool app_body_get_emotion(app_body_chart_t* emotion)
{
	service_reform1( app_body_emotion_data, emotion->data, APP_BODY_DATA_CNT);
	emotion->level = app_body_emotion_level;
	emotion->end_time = app_body_emotion_end_time;
	emotion->min = service_get_min_8(app_body_emotion_data,APP_BODY_DATA_CNT);
	emotion->max = service_get_max_8(app_body_emotion_data,APP_BODY_DATA_CNT);
	
	return true;
}


bool app_body_get_vision(app_body_chart_t* vision)
{
	service_reform1( app_body_vision_data, vision->data, APP_BODY_DATA_CNT);
	vision->level = app_body_vision_level;
	vision->end_time = app_body_vision_end_time;
	vision->min = service_get_min_8(app_body_vision_data,APP_BODY_DATA_CNT);
	vision->max = service_get_max_8(app_body_vision_data,APP_BODY_DATA_CNT);
	
	return true;
}


bool app_body_get_mental(app_body_chart_t* mental)
{
	service_reform1( app_body_mental_data, mental->data, APP_BODY_DATA_CNT);
	mental->level = app_body_mental_level;
	mental->end_time = app_body_mental_end_time;
	mental->min = service_get_min_8(app_body_mental_data,APP_BODY_DATA_CNT);
	mental->max = service_get_max_8(app_body_mental_data,APP_BODY_DATA_CNT);
	
	return true;
}


bool app_body_get_energy(app_body_chart_t* energy)
{
	service_reform1( app_body_energy_data, energy->data, APP_BODY_DATA_CNT);
	energy->level = app_body_energy_level;
	energy->end_time = app_body_energy_end_time;
	energy->min = service_get_min_8(app_body_energy_data,APP_BODY_DATA_CNT);
	energy->max = service_get_max_8(app_body_energy_data,APP_BODY_DATA_CNT);
	
	return true;
}


bool app_body_get_psychology(app_body_chart_t* psychology)
{
	service_reform1( app_body_psychology_data, psychology->data, APP_BODY_DATA_CNT);
	psychology->level = app_body_psychology_level;
	psychology->end_time = app_body_psychology_end_time;
	psychology->min = service_get_min_8(app_body_psychology_data,APP_BODY_DATA_CNT);
	psychology->max = service_get_max_8(app_body_psychology_data,APP_BODY_DATA_CNT);
	
	return true;
}

void app_body_set_psychology( uint8_t *dat, uint32_t len)
{
	if(*dat>2)
	{
		app_body_psychology_level = 3;
	}
	else
	{
		app_body_psychology_level = *dat + 1;
	}
	memcpy( app_body_psychology_data, dat+1, len);
	send_msg_to_ui_high( EVENT_BODY_CHANGE, EVENT_BODY_CHANGE);
}

void app_body_set_energy( uint8_t *dat, uint32_t len)
{
	if(*dat>2)
	{
		app_body_energy_level = 3;
	}
	else
	{
		app_body_energy_level = *dat + 1;
	}
	memcpy( app_body_energy_data, dat+1, len);
	send_msg_to_ui_high( EVENT_BODY_CHANGE, EVENT_BODY_CHANGE);
}

void app_body_set_mental( uint8_t *dat, uint32_t len)
{
	if(*dat>2)
	{
		app_body_mental_level = 3;
	}
	else
	{
		app_body_mental_level = *dat + 1;
	}
	memcpy( app_body_mental_data, dat+1, len);
	send_msg_to_ui_high( EVENT_BODY_CHANGE, EVENT_BODY_CHANGE);
}

void app_body_set_vision( uint8_t *dat, uint32_t len)
{
	if(*dat>2)
	{
		app_body_vision_level = 3;
	}
	else
	{
		app_body_vision_level = *dat + 1;
	}
	memcpy( app_body_vision_data, dat+1, len);
	send_msg_to_ui_high( EVENT_BODY_CHANGE, EVENT_BODY_CHANGE);
}

void app_body_set_emotion( uint8_t *dat, uint32_t len)
{
	if(*dat>2)
	{
		app_body_emotion_level = 3;
	}
	else
	{
		app_body_emotion_level = *dat + 1;
	}
	memcpy( app_body_emotion_data, dat+1, len);
	send_msg_to_ui_high( EVENT_BODY_CHANGE, EVENT_BODY_CHANGE);
}

void app_body_init()
{

}


