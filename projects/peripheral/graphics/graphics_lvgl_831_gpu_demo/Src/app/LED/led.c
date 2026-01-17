#include "led.h"
#include "app_io.h"

void LED_Init(void)
{
    app_io_init_t io_init;
	//µç³Ø×´Ì¬led
    io_init.pin  = APP_LED_BAT_IO;
    io_init.mode = APP_IO_MODE_OUTPUT;
    io_init.pull = APP_IO_NOPULL;
    io_init.mux  = APP_IO_MUX;
    app_io_init(APP_IO_TYPE_GPIOA, &io_init);
	//³äµç×´Ì¬LED
    io_init.pin  = APP_LED_CHG_IO;
    io_init.mode = APP_IO_MODE_OUTPUT;
    io_init.pull = APP_IO_NOPULL;
    io_init.mux  = APP_IO_MUX;
    app_io_init(APP_IO_TYPE_GPIOA, &io_init);
	LED_Close(LED_BAT);
	LED_Close(LED_CHG);
}

void LED_Open(bsp_led_num_t led_num)
{
    switch (led_num)
    {
        case LED_BAT:
            app_io_write_pin(APP_IO_TYPE_GPIOA, APP_LED_BAT_IO, APP_IO_PIN_RESET);
            break;

        case LED_CHG:
            app_io_write_pin(APP_IO_TYPE_GPIOA, APP_LED_CHG_IO, APP_IO_PIN_RESET);
            break;

        default:
            break;
    }
}

void LED_Close(bsp_led_num_t led_num)
{
    switch (led_num)
    {
        case LED_BAT:
            app_io_write_pin(APP_IO_TYPE_GPIOA, APP_LED_BAT_IO, APP_IO_PIN_SET);
            break;

        case LED_CHG:
            app_io_write_pin(APP_IO_TYPE_GPIOA, APP_LED_CHG_IO, APP_IO_PIN_SET);
            break;

        default:
            break;
    }
}

long LED_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString)
{
	printf("LED on\r\n");
	LED_Open(LED_BAT);
	LED_Open(LED_CHG);
	delay_ms(1000);
	LED_Close(LED_BAT);
	LED_Close(LED_CHG);
	printf("LED off\r\n");
	return 0;
}

