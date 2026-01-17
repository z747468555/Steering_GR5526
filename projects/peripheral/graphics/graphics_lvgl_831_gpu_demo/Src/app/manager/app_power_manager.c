#include "app_power_manager.h"

#include "drv_adapter_norflash.h"
#include "drv_adapter_touchpad.h"
#include "charge.h"
#include "QMI8658A.h"
#include "app_mems_task.h"
#include "HP5804.h"
#include "app_uart.h"
#include "app_graphics_dc.h"
#include "app_i2c.h"
#include "gh_demo.h"
#include "led.h"
#include "app_graphics_gpu.h"
#include "service_ota.h"


#define APP_IO_LOWPOWER_CONFIG                      \
{                                                  \
    .pin        = APP_IO_PIN_3,                  \
    .mode       = APP_IO_MODE_OUTPUT,               \
    .pull       = APP_IO_PULLUP,                 \
}
static void pin_to_low_power(void)
{
	app_io_init_t io_init = APP_IO_LOWPOWER_CONFIG;
//	//app_i2c_deinit(APP_I2C_ID_1);
//	app_i2c_deinit(APP_I2C_ID_3);
//	app_i2c_deinit(APP_I2C_ID_5);
	app_io_init(APP_IO_TYPE_MSIO,&io_init);
}
extern void app_graphics_ospi_deinit(void);

void sys_peripherals_sleep(void) {
    /* power off the peripherals one by one */
	
//	LED_Close(LED_BAT);
//	
//	service_ota_sleep();
//	
//    drv_adapter_touchpad_sleep();
//    drv_adapter_norflash_sleep();
//	//Enter_Ship_Mode();
//	qmi8658a_enter_low_power();
//	HP_enter_lowper();
//	Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_ADT);
////	app_graphics_ospi_deinit();
////	graphics_gpu_deinit();
//	//gh3220_power_off();
//	//app_uart_deinit(APP_UART_ID_3);
//	//graphics_dc_deinit();
//	app_graphics_dc_set_power_state(GDC_POWER_STATE_SLEEP);
//	pin_to_low_power();
	hal_pwr_mgmt_set_extra_device_state(EXTRA_DEVICE_NUM_DC, IDLE);  
}


void sys_peripherals_resume(void) {
    /* resume the peripherals one by one */

//    drv_adapter_norflash_wakeup();
//    drv_adapter_touchpad_wakeup();
//	Charge_Init();
//	app_mems_init();
//	Gh3x2xDemoInit();
//	Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_ADT);
//	service_ota_restart();
	printf("wakeup from sleep mode!\r\n");
}
