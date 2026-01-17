#include "drv_adapter_port.h"
#include "touchpad_driver.h"
#include "string.h"
#include "TAP.h"
#include "IIC.h"
#include "SEGGER_RTT.h"
/***********************************************************************************
 *                 Static Declarations For Nor-Flash
 ***********************************************************************************/
static bool _touchpad_drv_init(touchpad_drv_t * dev);
static bool _touchpad_drv_deinit(touchpad_drv_t * dev);
static bool _touchpad_drv_read_pointer(touchpad_drv_t * dev, int16_t * x, int16_t * y);
static bool _touchpad_drv_sleep(touchpad_drv_t * dev);
static bool _touchpad_drv_wakeup(touchpad_drv_t * dev);

__weak void _touchpad_drv_irq_notify(void);

/***********************************************************************************
 *                 Public Implements
 ***********************************************************************************/

void drv_adapter_touchpad_register(void) {

    touchpad_drv_t _tp_dev = {
        .id                          = 0,
        .touchpad_drv_init          = _touchpad_drv_init,
        .touchpad_drv_deinit        = _touchpad_drv_deinit,
        .touchpad_drv_read_pointer  = _touchpad_drv_read_pointer,
        .touchpad_drv_sleep         = _touchpad_drv_sleep,
        .touchpad_drv_wakeup        = _touchpad_drv_wakeup,
    };

    drv_adapter_touchpad_reg(&_tp_dev);
		TAP_Init(_touchpad_drv_irq_notify);
    return;
}


/***********************************************************************************
 *                 Static Implements for Nor-Flash
 ***********************************************************************************/

static bool _touchpad_drv_init(touchpad_drv_t * dev) {
    dev = dev; //avoid compile warning
    //touchpad_dev_init(TOUCHPAD_SCRN_TYPE_454P, _touchpad_drv_irq_notify);
//		SEGGER_RTT_SetTerminal(1);
//		SEGGER_RTT_printf(0, "Inter the Init\r\n");
//		SEGGER_RTT_SetTerminal(0);
		// TAP_Init(_touchpad_drv_irq_notify);
    return true;
}

static bool _touchpad_drv_deinit(touchpad_drv_t * dev)
{
    dev = dev; //avoid compile warning
    //touchpad_dev_deinit();
//		SEGGER_RTT_SetTerminal(1);
//		SEGGER_RTT_printf(0, "Inter the deinit\r\n");
//		SEGGER_RTT_SetTerminal(0);
		Tap_Deinit();
    return true;
}

static bool _touchpad_drv_read_pointer(touchpad_drv_t * dev, int16_t * x, int16_t * y) {
    dev = dev; //avoid compile warning
//		SEGGER_RTT_SetTerminal(1);
//		SEGGER_RTT_printf(0, "Inter the Read\r\n");
//		SEGGER_RTT_SetTerminal(0);
    return Tap_Read_Pointer(x, y);
}


static bool _touchpad_drv_sleep(touchpad_drv_t * dev) {
    dev = dev; //avoid compile warning
    //return touchpad_dev_sleep();
		//目前的触摸芯片没有睡眠功能
//		SEGGER_RTT_SetTerminal(1);
//		SEGGER_RTT_printf(0, "Inter the sleep\r\n");
//		SEGGER_RTT_SetTerminal(0);
		return true;
}


static bool _touchpad_drv_wakeup(touchpad_drv_t * dev) {
    dev = dev; //avoid compile warning
    //return touchpad_dev_wakeup();
		//目前的触摸芯片没有唤醒功能
//		SEGGER_RTT_SetTerminal(1);
//		SEGGER_RTT_printf(0, "Inter the wakeup\r\n");
//		SEGGER_RTT_SetTerminal(0);
		return true;
}

/* override this function to get irq notify */
__weak void _touchpad_drv_irq_notify(void) {
}
