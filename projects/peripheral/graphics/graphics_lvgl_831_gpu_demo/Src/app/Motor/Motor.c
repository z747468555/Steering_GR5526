#include "Motor.h"
#include "app_io.h"

void Motor_Init()
{
    app_io_init_t io_init;
    io_init.pin  = APP_IO_PIN_10;
    io_init.mode = APP_IO_MODE_OUTPUT;
    io_init.pull = APP_IO_PULLDOWN;
    io_init.mux  = APP_IO_MUX;
    app_io_init(APP_IO_TYPE_GPIOA, &io_init);
}

long Motor_Test(char *pcWriteBuffer, unsigned int xWriteBufferLen, const char *pcCommandString)
{
	Motor_Start();
	delay_ms(1000);
	Motor_Stop();
	return 0;
}