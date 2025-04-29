#ifndef OSKERNEL_H
#define OSKERNEL_H

#include "stm32f10x.h"                  // Device header

#define MAX_NUM_OF_THREAD	6
#define NUM_OF_PERIODIC_TASKS	4
#define STACK_SIZE 		128
#define BUS_FREQ 			72000000
#define NULL (void*)0

//Init some value for SysTick Reg
#define CTRL_ENABLE 		(1<<0)
#define CTRL_TICKINT 		(1<<1)
#define CTRL_CLCKSRC 		(1<<2)
#define CTRL_COUNTFLAG 	(1<<16)
#define SYSTICK_RST			0
#define INTCTRL 				(*((volatile uint32_t *)0XE000ED04))
#define PENDSTSET   		(1 << 26)

//Set value for SysTick (to calculate quanta time)
extern uint32_t MILLIS_PRESCALER;

/**
  * @brief  Task Control Block (TCB) initialize struct.
  */
struct tcb{
	uint32_t *stackPtr;
	struct tcb* nextPtr;
	uint32_t sleepTime;
	uint8_t isActive;
};

typedef struct tcb tcbType;
// Store the TCBs of all the threads
extern tcbType tcbs[MAX_NUM_OF_THREAD];
// Pointer to the current executing thread
extern tcbType *currentPtr;
// Store the thread status and value
extern uint32_t TCB_STACK[MAX_NUM_OF_THREAD][STACK_SIZE];
extern int current_num_of_thread;

/**
  * @brief  Periodic task initialize struct.
  */
typedef void(*taskT)(void);
typedef struct{
	taskT task;
	uint32_t period;
	uint32_t timeLeft;
} periodicTaskT;

static periodicTaskT PeriodicTasks[NUM_OF_PERIODIC_TASKS];
static uint32_t numberofPeriodicThreads = 0;

extern void (*PeriodicTask)(void);

void osKernelStackInit(int i);
uint8_t osKernelAddThread(void (*task)(void));
void osKernelInit(void);
void osKernelLaunch(uint32_t quanta);
void osSchedulerLaunch(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void osThreadYield(void);
void osSemaphore_Init(uint32_t *semaphore, uint32_t val);
void osSemaphore_Give(uint32_t *semaphore);
void osCooperative_Wait(uint32_t *semaphore);
uint8_t osKernelAddPeriodicThreads(void(*task)(void), uint32_t period);
void osSchedulerPeriodicRR(void);
void periodic_events_exe(void);
void osPeriodicTask_Init(void(*task)(void), uint32_t freq, uint8_t priority);
void osThreadsSleep(uint32_t sleeptime);
void osSchedulerRRSleep(void);

#endif
