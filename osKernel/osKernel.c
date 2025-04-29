#include "osKernel.h"

tcbType tcbs[MAX_NUM_OF_THREAD];
tcbType *currentPtr;
uint32_t TCB_STACK[MAX_NUM_OF_THREAD][STACK_SIZE];
int current_num_of_thread = 0;

/**
  * @brief  Initializes the stack for a specific thread.
  * @param  i: Index of the thread.
  * @retval None
  */
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

/**
  * @brief  Adds a new thread to the scheduler.
  * @param  task: Pointer to the task function.
  * @retval 1 if the thread was added successfully, 0 otherwise.
  */
uint8_t osKernelAddThread(void (*task)(void)){
	/*Disable global Interupt*/
	if (current_num_of_thread >= MAX_NUM_OF_THREAD)
    return 0;
	__disable_irq();
	
	// Update PC register
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

/**
  * @brief  Initializes the kernel timing configuration.
  * @param  None
  * @retval None
  */
void osKernelInit(void){
	//1ms
	MILLIS_PRESCALER = (BUS_FREQ/1000);
}

/**
  * @brief  Launches the kernel with a given time slice (quanta).
  * @param  quanta: Time slice in milliseconds.
  * @retval None
  */
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

/**
  * @brief  Causes the current thread to yield the CPU voluntarily.
  * @param  None
  * @retval None
  */
void osThreadYield(void){
	SysTick->VAL = 0;
	INTCTRL |= PENDSTSET;
}

/**
  * @brief  Initializes a semaphore with a specific initial value.
  * @param  semaphore: Pointer to the semaphore variable.
  * @param  val: Initial value of the semaphore.
  * @retval None
  */
void osSemaphore_Init(uint32_t *semaphore, uint32_t val){
	*semaphore = val;
}

/**
  * @brief  Releases (gives) a semaphore, incrementing its count.
  * @param  semaphore: Pointer to the semaphore variable.
  * @retval None
  */
void osSemaphore_Give(uint32_t *semaphore){
	__disable_irq();
	*semaphore += 1;
	__enable_irq();
}

/**
  * @brief  Waits cooperatively until a semaphore is available.
  * @param  semaphore: Pointer to the semaphore variable.
  * @retval None
  */
void osCooperative_Wait(uint32_t *semaphore){
	__disable_irq();
	
	// If resource isn't available
	while(*semaphore <= 0){
		__enable_irq();
		osThreadYield();
		__disable_irq();
	}
	// Give the semaphore back
	*semaphore -= 1;
	
	__enable_irq();
}

/**
  * @brief  Handler for SysTick interrupt, triggers a PendSV.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void){
	INTCTRL |= (1 << 28);
}

void (*PeriodicTask)(void);

/**
  * @brief  Adds a periodic thread (software timer-based task).
  * @param  task: Pointer to the periodic task function.
  * @param  period: Period of the task in ticks.
  * @retval 1 if added successfully, 0 otherwise.
  */
uint8_t osKernelAddPeriodicThreads(void(*task)(void), uint32_t period){
	// Make sure no more periodic tasks then the maximum
	if(numberofPeriodicThreads == NUM_OF_PERIODIC_TASKS || period == 0){
		return 0;
	}
	// Initialize the periodic task
	PeriodicTasks[numberofPeriodicThreads].task = task;
	PeriodicTasks[numberofPeriodicThreads].period = period;
	PeriodicTasks[numberofPeriodicThreads].timeLeft = period - 1;
	// Update the number of periodic tasks
	numberofPeriodicThreads++;
	return 1;
}

/**
  * @brief  Advances to the next thread in round-robin scheduling.
  * @param  None
  * @retval None
  */
void osSchedulerPeriodicRR(void){
	currentPtr = currentPtr->nextPtr;
}

/**
  * @brief  Initializes a periodic interrupt-based task using TIM2.
  * @param  task: Pointer to the periodic task function.
  * @param  freq: Frequency of the periodic interrupt in Hz.
  * @param  priority: Interrupt priority level.
  * @retval None
  */
void periodic_events_exe(){
	for(int i = 0; i < numberofPeriodicThreads; i++){
		if(PeriodicTasks[i].timeLeft == 0){
			// Execuse the periodic task
			PeriodicTasks[i].task();
			// Reset the time cycle
			PeriodicTasks[i].timeLeft = PeriodicTasks[i].period - 1;
		}
		// Decrease waiting time
		else PeriodicTasks[i].timeLeft --;
	}
	// Decrease sleep time of a normal thread
	for(int i = 0; i < current_num_of_thread; i++){
		if(tcbs[i].sleepTime > 0){
			tcbs[i].sleepTime--;
		}
	}
}

/**
  * @brief  Initializes a periodic interrupt-based task using TIM2.
  * @param  task: Pointer to the periodic task function.
  * @param  freq: Frequency of the periodic interrupt in Hz.
  * @param  priority: Interrupt priority level.
  * @retval None
  */
void osPeriodicTask_Init(void(*task)(void), uint32_t freq, uint8_t priority){
	__disable_irq();
	PeriodicTask = task;
	// 1us tick
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 72 - 1;
	TIM2->ARR = 1000000/freq - 1;
	TIM2->CR1 = 1;
	TIM2->DIER |= 1;
	NVIC_SetPriority(TIM2_IRQn , priority);
	NVIC_EnableIRQ(TIM2_IRQn);
}

/**
  * @brief  Handler for TIM2 interrupt, executes the periodic task.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(){
	TIM2->SR = 0;
	if (PeriodicTask != NULL) {
		PeriodicTask();
	}
}

/**
  * @brief  Makes the current thread sleep for a specific time.
  * @param  sleeptime: Sleep duration in ticks.
  * @retval None
  */
void osThreadsSleep(uint32_t sleeptime){
	__disable_irq();
	
	currentPtr->sleepTime = sleeptime;
	__enable_irq();
	osThreadYield();
}

/**
  * @brief  Advances to the next ready thread, skipping sleeping or inactive threads.
  * @param  None
  * @retval None
  */
void osSchedulerRRSleep(void){
	// Move to the next task
	currentPtr = currentPtr->nextPtr;
	while (currentPtr->sleepTime > 0 || currentPtr->isActive == 0) {
    currentPtr = currentPtr->nextPtr;
	}
}
