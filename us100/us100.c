#include "us100.h"
#include "uart.h"
#include "ssd1306.h"
#include "osKernel.h"

// List of GPIO pins used for all the sensors
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

/**
  * @brief  Initializes a queue for HCSR04 data frames.
  * @param  queue: Pointer to the HCSR04_Queue structure.
  * @retval None
  */
void HCSR04_QueueInit(HCSR04_Queue *queue){
	queue->head = 0;
	queue->tail = 0;
	queue->count = 0;
	osSemaphore_Init(&(queue->mutex), 1);
}

/**
  * @brief  Adds a data frame to the HCSR04 queue.
  * @param  queue: Pointer to the HCSR04_Queue structure.
  * @param  data: Data frame to be added to the queue.
  * @retval None
  */
void HCSR04_QueuePut(HCSR04_Queue *queue, HCSR04_DataFrame data){
	// Wait for semaphore to queue is available
	osCooperative_Wait(&(queue->mutex));

	if (queue->count < HCSR04_QUEUE_SIZE) {
		queue->buffer[queue->tail] = data;
		queue->tail = (queue->tail + 1) % HCSR04_QUEUE_SIZE;
		queue->count++;
	}

	// Give the semaphore back when queue is full
	osSemaphore_Give(&(queue->mutex));
}

/**
  * @brief	Retrieves a data frame from the HCSR04 queue.
  * @param  queue: Pointer to the HCSR04_Queue structure.
  * @param  data:  Pointer to the data frame to store the retrieved value.
  * @retval 1 if data was retrieved successfully.
  */
uint8_t HCSR04_QueueGet(HCSR04_Queue *queue, HCSR04_DataFrame *data){
	while(1){
		// Wait for semaphore to queue is available
		osCooperative_Wait(&(queue->mutex));

		if (queue->count > 0) {
			*data = queue->buffer[queue->head];
			queue->head = (queue->head + 1) % HCSR04_QUEUE_SIZE;
			queue->count--;
			osSemaphore_Give(&(queue->mutex));
			return 1;
		}

		// Give the semaphore back when no data found in queue
		osSemaphore_Give(&(queue->mutex));
		osThreadsSleep(1);
	}
}

/**
  * @brief  Initializes all HCSR04 sensors and related peripherals.
  * @param  None
  * @retval None
  */
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

/**
  * @brief  Computes the pulse width of the Echo signal.
  * @param  HCSR04_Struct: Pointer to the structure containing echo start and stop timestamps.
  * @retval Pulse width of the Echo signal (in timer ticks).
  */
uint16_t HCSR04_ComputeDuration(HCSR04_TypeDef *HCSR04_Struct){
	if(HCSR04_Struct->echoStop >= HCSR04_Struct->echoStart)
		return HCSR04_Struct->echoStop - HCSR04_Struct->echoStart;
	return (0xFFFF - HCSR04_Struct->echoStart) + HCSR04_Struct->echoStop + 1;
}

/**
  * @brief  Periodic task that decrements the timeout counter for each HCSR04 sensor.
  * @param  None
  * @retval None
  */
void HCSR04_TimeoutTick(void){
	for(int i = 0; i < NUM_OF_HCSR04; i++){
		if(HCSR04List[i].timeout > 0) HCSR04List[i].timeout--;
	}
}

/**
  * @brief  Measures distances from all HCSR04 sensors and sends results to queues.
  * @param  None
  * @retval None
  */
void HCSR04_Measure(void){
	while(1){
		uint8_t echoNow;		// Store the current status of the echo pin
		for(int i = 0; i < NUM_OF_HCSR04; i++){
			switch (HCSR04List[i].state) {
				case HCSR04_IDLE:
					if(HCSR04List[i].timeout == 0){
						// Trigger pulse
						GPIO_SetBits(HCSR04List[i].gpioPort, HCSR04List[i].triggerPin);
						HCSR04List[i].timeout = 1;
						// Next state
						HCSR04List[i].state = HCSR04_WAIT_FOR_TRIGGER;
					}
					break;
					
				case HCSR04_WAIT_FOR_TRIGGER:
					if(HCSR04List[i].timeout == 0){
						// Complete trigger pulse
						GPIO_ResetBits(HCSR04List[i].gpioPort, HCSR04List[i].triggerPin);
						//Next state
						HCSR04List[i].state = HCSR04_WAIT_FOR_ECHO_HIGH;
						HCSR04List[i].timeout = 50;
						HCSR04List[i].lastEchoState = 0;
					}
					break;

				case HCSR04_WAIT_FOR_ECHO_HIGH:
					echoNow = GPIO_ReadInputDataBit(HCSR04List[i].gpioPort, HCSR04List[i].echoPin);
					
					// If detected rising edge
					if (echoNow == 1 && HCSR04List[i].lastEchoState == 0) {
						HCSR04List[i].echoStart = TIM_GetCounter(TIM3);
						// Next stage
						HCSR04List[i].state = HCSR04_WAIT_FOR_ECHO_LOW;
						HCSR04List[i].timeout = 50;
					}
					
					// No rising edge detected and timeout
					else if (HCSR04List[i].timeout == 0) {
						// Reset state
						HCSR04List[i].state = HCSR04_IDLE;
						HCSR04List[i].timeout = 10;
					}
					
					HCSR04List[i].lastEchoState = echoNow;
					break;

				case HCSR04_WAIT_FOR_ECHO_LOW:
					echoNow = GPIO_ReadInputDataBit(HCSR04List[i].gpioPort, HCSR04List[i].echoPin);
					
					// If deteccted falling edge
					if (echoNow == 0 && HCSR04List[i].lastEchoState == 1) {
						HCSR04List[i].echoStop = TIM_GetCounter(TIM3);
						HCSR04List[i].state = HCSR04_IDLE;
						HCSR04List[i].timeout = 10;

						// Use math formulas to calculate to cm
						uint16_t distance = HCSR04_ComputeDuration(&HCSR04List[i]) / 58;

						HCSR04_DataFrame frame = {
								.HCSR04_Struct = &HCSR04List[i],
								.distance_cm = distance
						};

						// Transfer data to UI and OLED
						HCSR04_QueuePut(&HCSR04UARTQueue, frame);
						HCSR04_QueuePut(&HCSR04OLEDQueue, frame);
					}
					
					// No falling edge detected and timeout
					else if (HCSR04List[i].timeout == 0) {
						HCSR04List[i].state = HCSR04_IDLE;
						HCSR04List[i].timeout = 10;
					}
					// Reset state
					HCSR04List[i].lastEchoState = echoNow;
					break;
					
				default:
					HCSR04List[i].state = HCSR04_IDLE;
					break;
			}
		}
	}
}

/**
  * @brief  Displays distance data on the OLED screen using SSD1306.
  * @param  None
  * @retval None
  */
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

uint16_t distances[10];		// For debugging purpose

/**
  * @brief  Retrieves HCSR04 distance data and publishes it via UART.
  * @param  None
  * @retval None
  */
void HCSR04_PublicDataTask(void){
	HCSR04_DataFrame frame;
	char str[16];
	uint16_t result[NUM_OF_HCSR04];
	
	// Initial values before get the data
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
