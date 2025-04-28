#include "us100.h"
#include "uart.h"
#include "ssd1306.h"
#include "osKernel.h"

HCSR04_TypeDef HCSR04List[NUM_OF_HCSR04] = {
	{0, GPIO_Pin_14, GPIO_Pin_13, GPIOB},
	{1, GPIO_Pin_8, GPIO_Pin_11, GPIOA},
	{2, GPIO_Pin_4, GPIO_Pin_3, GPIOB},
	{3, GPIO_Pin_9, GPIO_Pin_8, GPIOB},
	{4, GPIO_Pin_0, GPIO_Pin_1, GPIOA},
	{5, GPIO_Pin_3, GPIO_Pin_2, GPIOA},
	{6, GPIO_Pin_5, GPIO_Pin_4, GPIOA},
	{7, GPIO_Pin_7, GPIO_Pin_6, GPIOA},
	{8, GPIO_Pin_1, GPIO_Pin_0, GPIOB},
	{9, GPIO_Pin_14, GPIO_Pin_15, GPIOC}
};

HCSR04_Queue HCSR04UARTQueue;
HCSR04_Queue HCSR04OLEDQueue;

void HCSR04_QueueInit(HCSR04_Queue *queue){
	queue->head = 0;
	queue->tail = 0;
	queue->count = 0;
	osSemaphore_Init(&(queue->mutex), 1);
}

void HCSR04_QueuePut(HCSR04_Queue *queue, HCSR04_DataFrame data){
	osCooperative_Wait(&(queue->mutex));

	if (queue->count < HCSR04_QUEUE_SIZE) {
		queue->buffer[queue->tail] = data;
		queue->tail = (queue->tail + 1) % HCSR04_QUEUE_SIZE;
		queue->count++;
	}

	osSemaphore_Give(&(queue->mutex));
}

uint8_t HCSR04_QueueGet(HCSR04_Queue *queue, HCSR04_DataFrame *data){
	while(1){
		osCooperative_Wait(&(queue->mutex));

		if (queue->count > 0) {
			*data = queue->buffer[queue->head];
			queue->head = (queue->head + 1) % HCSR04_QUEUE_SIZE;
			queue->count--;
			osSemaphore_Give(&(queue->mutex));
			return 1;
		}

		osSemaphore_Give(&(queue->mutex));
		osThreadsSleep(1);
	}
}


void HCSR04_AllInit(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;

	for (int i = 0; i < NUM_OF_HCSR04; i++) {
		// Trigger - Output
		GPIO_InitStruct.GPIO_Pin = HCSR04List[i].triggerPin;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(HCSR04List[i].gpioPort, &GPIO_InitStruct);

		// Echo - Input
		GPIO_InitStruct.GPIO_Pin = HCSR04List[i].echoPin;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(HCSR04List[i].gpioPort, &GPIO_InitStruct);
		
		HCSR04List[i].lastEchoState = 0;
		HCSR04List[i].echoStart = 0;
		HCSR04List[i].echoStop = 0;
		HCSR04List[i].state = HCSR04_IDLE;
		HCSR04List[i].timeout = 0;
	}
	
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStruct.TIM_Prescaler = 72 - 1;
	TIM_InitStruct.TIM_Period = 0xFFFF;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_InitStruct);
	TIM_Cmd(TIM3, ENABLE);
	
	HCSR04_QueueInit(&HCSR04UARTQueue);
	HCSR04_QueueInit(&HCSR04OLEDQueue);
}

uint16_t HCSR04_ComputeDuration(HCSR04_TypeDef *HCSR04_Struct){
	if(HCSR04_Struct->echoStop >= HCSR04_Struct->echoStart)
		return HCSR04_Struct->echoStop - HCSR04_Struct->echoStart;
	return (0xFFFF - HCSR04_Struct->echoStart) + HCSR04_Struct->echoStop + 1;
}

void HCSR04_TimeoutTick(void){
	for(int i = 0; i < NUM_OF_HCSR04; i++){
		if(HCSR04List[i].timeout > 0) HCSR04List[i].timeout--;
	}
}

void HCSR04_Measure(void){
	while(1){
		uint8_t echoNow;
		for(int i = 0; i < NUM_OF_HCSR04; i++){
			switch (HCSR04List[i].state) {
				case HCSR04_IDLE:
					if(HCSR04List[i].timeout == 0){
						// Trigger pulse
						GPIO_SetBits(HCSR04List[i].gpioPort, HCSR04List[i].triggerPin);
						HCSR04List[i].timeout = 1;

						HCSR04List[i].state = HCSR04_WAIT_FOR_TRIGGER;
					}
					break;
					
				case HCSR04_WAIT_FOR_TRIGGER:
					if(HCSR04List[i].timeout == 0){
						GPIO_ResetBits(HCSR04List[i].gpioPort, HCSR04List[i].triggerPin);

						HCSR04List[i].state = HCSR04_WAIT_FOR_ECHO_HIGH;
						HCSR04List[i].timeout = 50;
						HCSR04List[i].lastEchoState = 0;
					}
					break;

				case HCSR04_WAIT_FOR_ECHO_HIGH:
					echoNow = GPIO_ReadInputDataBit(HCSR04List[i].gpioPort, HCSR04List[i].echoPin);
				
					if (echoNow == 1 && HCSR04List[i].lastEchoState == 0) {
						HCSR04List[i].echoStart = TIM_GetCounter(TIM3);
						HCSR04List[i].state = HCSR04_WAIT_FOR_ECHO_LOW;
						HCSR04List[i].timeout = 50;
					}
					
					else if (HCSR04List[i].timeout == 0) {
						HCSR04List[i].state = HCSR04_IDLE;
						HCSR04List[i].timeout = 10;
					}
					
					HCSR04List[i].lastEchoState = echoNow;
					break;

				case HCSR04_WAIT_FOR_ECHO_LOW:
					echoNow = GPIO_ReadInputDataBit(HCSR04List[i].gpioPort, HCSR04List[i].echoPin);
					if (echoNow == 0 && HCSR04List[i].lastEchoState == 1) {

						HCSR04List[i].echoStop = TIM_GetCounter(TIM3);
						HCSR04List[i].state = HCSR04_IDLE;
						HCSR04List[i].timeout = 10;

						uint16_t distance = HCSR04_ComputeDuration(&HCSR04List[i]) / 58;

						HCSR04_DataFrame frame = {
								.HCSR04_Struct = &HCSR04List[i],
								.distance_cm = distance
						};

						HCSR04_QueuePut(&HCSR04UARTQueue, frame);
						HCSR04_QueuePut(&HCSR04OLEDQueue, frame);
					}
					else if (HCSR04List[i].timeout == 0) {
						HCSR04List[i].state = HCSR04_IDLE;
						HCSR04List[i].timeout = 10;
					}
					
					HCSR04List[i].lastEchoState = echoNow;
					break;
					
				default:
					HCSR04List[i].state = HCSR04_IDLE;
					break;
			}
		}
	}
}

void OLED_DisplayTask(void) {
	char str[8];
	HCSR04_DataFrame frame;

	SSD1306_Init();
	SSD1306_Clear();
	SSD1306_SetCursor(3, 45);
	SSD1306_PrintString("------");

	while(1){
		if (HCSR04_QueueGet(&HCSR04OLEDQueue, &frame)) {
			UART_ConvertIntToString(frame.distance_cm, str);

			SSD1306_SetCursor(3, 45);
			SSD1306_PrintString("      ");

			SSD1306_SetCursor(3, 45);
			SSD1306_PrintString(str);
		}

		osThreadYield();
	}
}

uint16_t distances[10];
void HCSR04_PublicDataTask(void){
	HCSR04_DataFrame frame;
	char str[16];
	uint16_t result[NUM_OF_HCSR04];
	
	for (int i = 0; i < NUM_OF_HCSR04; i++) {
		result[i] = 0xFFFF;
	}
	
	while(1){
		if(HCSR04_QueueGet(&HCSR04UARTQueue, &frame)){
			if(frame.HCSR04_Struct->id < NUM_OF_HCSR04){
				result[frame.HCSR04_Struct->id] = frame.distance_cm;
			}
			
			for (int i = 0; i < NUM_OF_HCSR04; i++) {
				UART_ConvertIntToString(result[i], str);
				UART_SendString(str);
				if (i < NUM_OF_HCSR04 - 1) UART_SendString(",");
			}
			UART_SendString("\r\n");
			
			// Observing data
			switch(frame.HCSR04_Struct->id){
				case 0:
					distances[0] = frame.distance_cm;
					break;
				case 1:
					distances[1] = frame.distance_cm;
					break;
				case 2:
					distances[2] = frame.distance_cm;
					break;
				case 3:
					distances[3] = frame.distance_cm;
					break;
				case 4:
					distances[4] = frame.distance_cm;
					break;
				case 5:
					distances[5] = frame.distance_cm;
					break;
				case 6:
					distances[6] = frame.distance_cm;
					break;
				case 7:
					distances[7] = frame.distance_cm;
					break;
				case 8:
					distances[8] = frame.distance_cm;
					break;
				case 9:
					distances[9] = frame.distance_cm;
					break;
				default:
					break;
			}
		}
		else{
			osThreadsSleep(5);
		}
		osThreadYield();
	}
}
