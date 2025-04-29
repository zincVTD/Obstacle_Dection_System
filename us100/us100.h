#ifndef __HCSR04_H__
#define __HCSR04_H__

#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_exti.h"             // Keil::Device:StdPeriph Drivers:EXTI

#define NUM_OF_HCSR04 			10
#define HCSR04_QUEUE_SIZE 	16

/**
  * @brief  HC-SR04 states.
  */
typedef enum {
	HCSR04_IDLE,
	HCSR04_WAIT_FOR_TRIGGER,
	HCSR04_WAIT_FOR_ECHO_HIGH,
	HCSR04_WAIT_FOR_ECHO_LOW,
} HCSR04_State;

/**
  * @brief  HC-SR04 definition struct.
  */
typedef struct {
	uint8_t id;
	uint16_t echoPin;
	uint16_t triggerPin;
	GPIO_TypeDef *gpioPort;
	uint8_t lastEchoState;
	uint32_t echoStart;
	uint32_t echoStop;
	HCSR04_State state;
	uint32_t timeout;
} HCSR04_TypeDef;

/**
  * @brief  HC-SR04 data frame in queues.
  */
typedef struct {
	HCSR04_TypeDef *HCSR04_Struct;
	uint16_t distance_cm;
} HCSR04_DataFrame;

/**
  * @brief  HC-SR04 queues.
  */
typedef struct {
	HCSR04_DataFrame buffer[HCSR04_QUEUE_SIZE];
	uint32_t head;
	uint32_t tail;
	uint32_t count;
	uint32_t mutex;
} HCSR04_Queue;

void HCSR04_AllInit(void);
void HCSR04_TimeoutTick(void);
uint16_t HCSR04_ComputeDuration(HCSR04_TypeDef *HCSR04_Struct);
void HCSR04_Measure(void);
void HCSR04_PublicDataTask(void);
void OLED_DisplayTask(void);

extern HCSR04_TypeDef HCSR04List[NUM_OF_HCSR04];
extern HCSR04_Queue HCSR04UARTQueue;
extern HCSR04_Queue HCSR04OLEDQueue;

#endif
