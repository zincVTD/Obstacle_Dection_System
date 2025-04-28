#include "osKernel.h"

tcbType tcbs[MAX_NUM_OF_THREAD];
tcbType *currentPtr;
uint32_t TCB_STACK[MAX_NUM_OF_THREAD][STACK_SIZE];
int current_num_of_thread = 0;

void osKernelStackInit(int i){
	tcbs[i].stackPtr = &TCB_STACK[i][STACK_SIZE - 16];
	
	/*Set bit21 (T-Bit) in PSR to 1 to operate it*/
	/*Thumb Mode*/
	TCB_STACK[i][STACK_SIZE - 1] = 1 << 24;	/*PSR*/
	
	/*Skip the PC*/
	
	/*Dumming Stack Content*/
	TCB_STACK[i][STACK_SIZE - 3] = 0xAAAAAAAA;	/*R14 (LR)*/
	TCB_STACK[i][STACK_SIZE - 4] = 0xAAAAAAAA;	/*R12*/
	TCB_STACK[i][STACK_SIZE - 5] = 0xAAAAAAAA;	/*R3*/
	TCB_STACK[i][STACK_SIZE - 6] = 0xAAAAAAAA;	/*R2*/
	TCB_STACK[i][STACK_SIZE - 7] = 0xAAAAAAAA;	/*R1*/
	TCB_STACK[i][STACK_SIZE - 8] = 0xAAAAAAAA;	/*R0*/
	
	TCB_STACK[i][STACK_SIZE - 9] = 0xAAAAAAAA; 	/*R11*/
	TCB_STACK[i][STACK_SIZE - 10] = 0xAAAAAAAA;	/*R10*/
	TCB_STACK[i][STACK_SIZE - 11] = 0xAAAAAAAA;	/*R9*/
	TCB_STACK[i][STACK_SIZE - 12] = 0xAAAAAAAA;	/*R8*/
	TCB_STACK[i][STACK_SIZE - 13] = 0xAAAAAAAA;	/*R7*/
	TCB_STACK[i][STACK_SIZE - 14] = 0xAAAAAAAA;	/*R6*/
	TCB_STACK[i][STACK_SIZE - 15] = 0xAAAAAAAA;	/*R5*/
	TCB_STACK[i][STACK_SIZE - 16] = 0xAAAAAAAA;	/*R4*/
}

uint8_t osKernelAddThread(void (*task)(void)){
	/*Disable global Interupt*/
	if (current_num_of_thread >= MAX_NUM_OF_THREAD)
    return 0;
	__disable_irq();
	
	osKernelStackInit(current_num_of_thread);
	TCB_STACK[current_num_of_thread][STACK_SIZE - 2] = (uint32_t)(task);
	
	if(!current_num_of_thread){
		tcbs[current_num_of_thread].nextPtr = &tcbs[current_num_of_thread];
	} else {
		tcbs[current_num_of_thread - 1].nextPtr = &tcbs[current_num_of_thread];
		tcbs[current_num_of_thread].nextPtr = &tcbs[0];
	}
	
	tcbs[current_num_of_thread].sleepTime = 0;
	tcbs[current_num_of_thread].isActive = 1;
	currentPtr = &tcbs[0];
	current_num_of_thread++;
	__enable_irq();
	return 1;
}

uint32_t MILLIS_PRESCALER;
void osKernelInit(void){
	//1ms
	MILLIS_PRESCALER = (BUS_FREQ/1000);
}

void osKernelLaunch(uint32_t quanta){
	/*Reset the SysTick*/
	SysTick->CTRL = SYSTICK_RST;
	
	/*Clear Systick current value register*/
	SysTick->VAL = 0;
	
	/*Load the quanta*/
	SysTick->LOAD = (quanta * MILLIS_PRESCALER - 1);
	
	/*Set PendSV interupt to lowest priority*/
	NVIC_SetPriority(SysTick_IRQn, 14);
	NVIC_SetPriority(PendSV_IRQn, 15);
	
	/*Enable Systick, select Internal clock*/
	SysTick->CTRL |= CTRL_CLCKSRC | CTRL_ENABLE;
	
	/*Enable Systick interupt*/
	SysTick->CTRL |= CTRL_TICKINT;
	
	//os launch scheduler
	osSchedulerLaunch();
}

void osThreadYield(void){
	SysTick->VAL = 0;
	INTCTRL |= PENDSTSET;
}

void osSemaphore_Init(uint32_t *semaphore, uint32_t val){
	*semaphore = val;
}

void osSemaphore_Give(uint32_t *semaphore){
	__disable_irq();
	*semaphore += 1;
	__enable_irq();
}

void osCooperative_Wait(uint32_t *semaphore){
	__disable_irq();
	
	while(*semaphore <= 0){
		__enable_irq();
		osThreadYield();
		__disable_irq();
	}
	*semaphore -= 1;
	
	__enable_irq();
}

void SysTick_Handler(void){
	INTCTRL |= (1 << 28);
}

void (*PeriodicTask)(void);
uint8_t osKernelAddPeriodicThreads(void(*task)(void), uint32_t period){
	if(numberofPeriodicThreads == NUM_OF_PERIODIC_TASKS || period == 0){
		return 0;
	}
	PeriodicTasks[numberofPeriodicThreads].task = task;
	PeriodicTasks[numberofPeriodicThreads].period = period;
	PeriodicTasks[numberofPeriodicThreads].timeLeft = period - 1;
	
	numberofPeriodicThreads++;
	return 1;
}

void osSchedulerPeriodicRR(void){
	currentPtr = currentPtr->nextPtr;
}

void periodic_events_exe(){
	for(int i = 0; i < numberofPeriodicThreads; i++){
		if(PeriodicTasks[i].timeLeft == 0){
			PeriodicTasks[i].task();
			PeriodicTasks[i].timeLeft = PeriodicTasks[i].period - 1;
		}
		else PeriodicTasks[i].timeLeft --;
	}
	for(int i = 0; i < current_num_of_thread; i++){
		if(tcbs[i].sleepTime > 0){
			tcbs[i].sleepTime--;
		}
	}
}

void osPeriodicTask_Init(void(*task)(void), uint32_t freq, uint8_t priority){
	__disable_irq();
	PeriodicTask = task;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 72 - 1;
	TIM2->ARR = 1000000/freq - 1;
	TIM2->CR1 = 1;
	TIM2->DIER |= 1;
	NVIC_SetPriority(TIM2_IRQn , priority);
	NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(){
	TIM2->SR = 0;
	if (PeriodicTask != NULL) {
		PeriodicTask();
	}
}

void osThreadsSleep(uint32_t sleeptime){
	__disable_irq();
	
	currentPtr->sleepTime = sleeptime;
	__enable_irq();
	osThreadYield();
}

void osSchedulerRRSleep(void){
	currentPtr = currentPtr->nextPtr;
	while (currentPtr->sleepTime > 0 || currentPtr->isActive == 0) {
    currentPtr = currentPtr->nextPtr;
	}
}

void osMessageQueue_Init(osMessageQueueType *queue) {
	queue->head = 0;
	queue->tail = 0;
	queue->count = 0;
	osSemaphore_Init(&(queue->mutex), 1);
}

void osMessageQueue_Put(osMessageQueueType *queue, uint32_t message) {
	osCooperative_Wait(&(queue->mutex));

	if (queue->count < QUEUE_SIZE) {
		queue->buffer[queue->tail] = message;
		queue->tail = (queue->tail + 1) % QUEUE_SIZE;
		queue->count++;
	}

	osSemaphore_Give(&(queue->mutex));
}

uint32_t osMessageQueue_Get(osMessageQueueType *queue) {
	uint32_t message = 0;
	
	osCooperative_Wait(&(queue->mutex));

	while (queue->count == 0) {  
		osThreadYield();
	}

	message = queue->buffer[queue->head];
	queue->head = (queue->head + 1) % QUEUE_SIZE;
	queue->count--;

	osSemaphore_Give(&(queue->mutex));
	return message;
}
