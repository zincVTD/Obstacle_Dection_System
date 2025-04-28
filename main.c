#include "osKernel.h"
#include "uart.h"
#include "us100.h"
#include "ssd1306.h"

int main(void) {
	UART_Init();
	HCSR04_AllInit();
	osKernelInit();

	osPeriodicTask_Init(periodic_events_exe, 1000, 5);
	osKernelAddPeriodicThreads(HCSR04_TimeoutTick, 1);
	osKernelAddThread(HCSR04_Measure);
	osKernelAddThread(HCSR04_PublicDataTask);
//	osKernelAddThread(OLED_DisplayTask);
	osKernelLaunch(1);

	while(1);
}
