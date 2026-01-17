#include "tap.h"
#include "IIC.h"
#include "SEGGER_RTT.h"
void TAP_INT_Handler(app_io_evt_t *p_evt);
static void touchpad_dev_irq_handler(app_io_evt_t *p_evt);
const app_gpiote_param_t gpiote_param = {
    .type = TAP_INT_IO_TYPE,
    .pin  = TAP_INT_IO_PIN,
    .mode = APP_IO_MODE_IT_FALLING,       
    .pull = APP_IO_PULLUP,              
    .io_evt_cb = touchpad_dev_irq_handler
};

static void(*p_touchpad_irq_notify)(void) = NULL;

void TAP_INT_Handler(app_io_evt_t *p_evt)
{
    uint16_t x, y;
    uint8_t wdata, rdata[4];
    wdata = POSITION_ADD;
    IIC_Write_sync_tp(TAP_IIC, TAP_ADD, &wdata, 1);
    IIC_Read_sync_tp(TAP_IIC, TAP_ADD, rdata, 4);
    x = ((rdata[0] & 0x0F) << 8) + rdata[1];
    y = ((rdata[2] & 0x0F) << 8) + rdata[3];
    SEGGER_RTT_printf(0, "x:%d, y:%d\r\n", x, y);
}

static void touchpad_dev_irq_handler(app_io_evt_t *p_evt)
{
//    SEGGER_RTT_SetTerminal(1);
//    SEGGER_RTT_printf(0, "Enter the INT\r\n");
//    SEGGER_RTT_SetTerminal(0);
    if(p_evt->pin == TAP_INT_IO_PIN)
    {
        if(p_touchpad_irq_notify) {
            p_touchpad_irq_notify();
        }
    }
}

//TAP��ʼ��������Ŀǰֻ��ʼ�����ж����š�
void TAP_Init(void(* _irq_notify)(void))
{    
    app_io_init_t io_init;
	
		p_touchpad_irq_notify = _irq_notify;
		// IIC_Init();
	
    io_init.pin  = TAP_RST_IO_PIN;
    io_init.mode = APP_IO_MODE_OUTPUT;
    io_init.pull = APP_IO_NOPULL;
    io_init.mux  = APP_IO_MUX;
    app_io_init(TAP_RST_IO_TYPE, &io_init);
    app_gpiote_config(&gpiote_param);
	
    app_io_write_pin(TAP_RST_IO_TYPE, TAP_RST_IO_PIN, APP_IO_PIN_SET);
}

bool Tap_Read_Pointer(int16_t  * x, int16_t * y)
{
    uint8_t read_data[5];
    uint8_t wdata;
		//��ȡ����״̬�Ĵ����Լ�����
		wdata = STATUS_ADD;
		IIC_Write_sync_tp(TAP_IIC, TAP_ADD, &wdata, 1);
		IIC_Read_sync_tp(TAP_IIC, TAP_ADD, read_data, 5);
    if (read_data[0] == 0)
    {
        /**
         * the finger leave the touchpad
         */
        return false;
    }
    else
    {
        /**
         * when interrupt generates, set the mcu not to sleep until finger leave the touchpad
         * or the mcu may lost the interrupts
         */

				*x = ((read_data[1] & 0x0F) << 8) + read_data[2];
				*y = ((read_data[3] & 0x0F) << 8) + read_data[4];
                if(*x>359) *x = 359;
                if(*y>359) *y = 359;
//        SEGGER_RTT_SetTerminal(1);
//        SEGGER_RTT_printf(0, "x=%d y=%d", *x, *y);
//        SEGGER_RTT_SetTerminal(0);
        return true;
    }
}

void Tap_Deinit(void)
{
    app_io_deinit(TAP_RST_IO_TYPE, TAP_RST_IO_PIN);
    app_io_deinit(TAP_INT_IO_TYPE, TAP_INT_IO_PIN);
    app_i2c_deinit(TAP_IIC);
    return;
}

//�˺���ֻ���ڲ��ԣ��޾��幦��
void TAP_Test()
{
    uint8_t tdata[3], rdata = 0;
    tdata[0] = 0x1F;
		tdata[1] = 0x05;
    IIC_Write_sync_tp(TAP_IIC, TAP_ADD, tdata, 2);
		delay_us(5);
		IIC_Write_sync_tp(TAP_IIC, TAP_ADD, tdata, 1);
    IIC_Read_sync_tp(TAP_IIC, TAP_ADD, &rdata, 1);
    SEGGER_RTT_printf(0, "Get data:0x%x\r\n", rdata);
}

