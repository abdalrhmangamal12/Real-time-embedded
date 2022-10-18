#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"

int counter = 0;
SemaphoreHandle_t xBinarySemaphore ;
portBASE_TYPE xMutex;



/****************************************Function Prototypes************************************/
void InitTask (void);
void UART_init  (void);

static void Count_TASK    (void * pvParameters);
static void Periodic_TASK     (void * pvParameters);

void GPIOF_Handler (void);
/***********************************************************************************************/


/*****************************************UART Methods******************************************/
void Print_Character(char x)
{
	while ((UART0_FR_R & (1<<5)) != 0);
	UART0_DR_R = x ;
}

void Print_integer(int x)
{
	char mySize = 0;
	char Str[32];
	
	if (x == 0)
	{
		Print_Character('(');
		Print_Character('0');
		Print_Character(')');

	}
	/* 
	* looping to add each digit in array form least significant to most significant ()
	* if number is 1 2 3 4 5 
	* it will be stored 5 4 3 2 1 
	*/
	else
	{
	while (x != 0)
	{
		Str[mySize]=x%10;
		x=x/10;
		mySize++;
	}
	// to print ( before the Number
		Print_Character('(');
	
	/*
	* Loop to print the assigned numbers in an array and print it reversed (From most to least)
	* if the array has stored values 5 4 3 2 1 
	*	it will print 1 2 3 4 5
	*/
	for (int i=mySize-1;i>=0;i--)
	{
		Print_Character(48+Str[i]);
	}
	
	//to print ) after the number
		Print_Character(')');
		}
	Print_Character('\r');
	Print_Character('\n');
}

void Print_String(char* string)
{
	while (*string)
	{
		Print_Character(*(string++));
	}
	
	Print_Character('\r');
	Print_Character('\n');
}

void UART_init(void)
{
	SYSCTL_RCGCUART_R  |= 1<<0 ;                      //initiallizing uart0
	SYSCTL_RCGCGPIO_R  |= 1<<0 ;                      //initiallizing portA pins 
	GPIO_PORTA_AFSEL_R |= (1<<0 | 1<<1);              //choosing the alternative function for pins 0 & 1 (Tx & Rx)
	GPIO_PORTA_PCTL_R  |= (1<<(0*4) | 1<<(1*4));      //choosing the alternative function to the pin
	GPIO_PORTA_DEN_R   |= (1<<0 | 1<<1);              //openning digital enable for pins 0 & 1 in PortA
	UART0_CTL_R &=~ 1<<0;     
	
	UART0_IBRD_R = 104;
	UART0_FBRD_R = 11;
	UART0_LCRH_R = 0x60;
	UART0_CC_R =0x0;
	
	UART0_CTL_R = 0x301;              
}

static void Count_TASK(void * pvParameters)
{
	Print_String("This is The Counter Task");
	for(;;)
	{
		vTaskDelay(2000/portTICK_RATE_MS); // delay 2s for debugging only
		xSemaphoreTake(xMutex , portMAX_DELAY);
		for (int i=0;i<=10;i++)
		{
			Print_integer(i);
		}
		xSemaphoreGive(xMutex);

	}
}

static void Periodic_TASK(void * pvParameters)
{
	xSemaphoreTake(xBinarySemaphore,0);
	for (;;)
	{
		xSemaphoreTake(xBinarySemaphore,portMAX_DELAY);
		
		xSemaphoreTake(xMutex , portMAX_DELAY);
		Print_String("This is The LedToggler Task");
		GPIO_PORTF_DATA_R ^= (1<<1|1<<2|1<<3); //toggle white
		xSemaphoreGive(xMutex);

		vTaskDelay(500/portTICK_RATE_MS);
	}
}

void GPIOF_Handler (void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(xBinarySemaphore , &xHigherPriorityTaskWoken);
	GPIO_PORTF_ICR_R |= 1<<4;
	portEND_SWITCHING_ISR (xHigherPriorityTaskWoken);
}


void InitTask()
{	
	SYSCTL_RCGCGPIO_R |= 0x00000020;                                     // initialize clock
	while((SYSCTL_PRGPIO_R&0x00000020) == 0){}                           // looping until clock is initiallized
	GPIO_PORTF_LOCK_R = 0x4C4F434B;                                      // Unlocking PortF Pins
	GPIO_PORTF_CR_R |= (1<<0 | 1<<4);                                    // unlocking commit register for switch 1 & switch 2 
  GPIO_PORTF_DIR_R |= (1<<1 | 1<<2 | 1<<3);                            // detrmining the output pins  
	GPIO_PORTF_DIR_R &=~ (1<<0 | 1<<4);                                  // detrmining the input pins 
	GPIO_PORTF_PUR_R |= (1<<0 | 1<<4);                                   // configuring the switches with PullUp resistors
  GPIO_PORTF_DEN_R |= (1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4);              // detrmining the pins direction 
	GPIO_PORTF_DATA_R &=~ (1<<1 | 1<<2 | 1<<3);                          // initially all leds are of 
	
	GPIO_PORTF_IS_R  &=~ 1<<4;
	GPIO_PORTF_IBE_R &=~ 1<<4;
	GPIO_PORTF_IEV_R &=~ 1<<4;
	GPIO_PORTF_ICR_R |= 1<<4;
	GPIO_PORTF_IM_R |= 1<<4;
	NVIC_PRI7_R =6 <<21;
	NVIC_EN0_R |= 1<<30;
	__enable_irq();
}

int main (void)
{	
InitTask();
UART_init();
	
vSemaphoreCreateBinary(xBinarySemaphore); //Create binary semaphore
xMutex = xSemaphoreCreateMutex();	//create mutex
	
if( xBinarySemaphore != NULL && xMutex!= NULL){	//verified that mutex anf semaphore created successfully
xTaskCreate(Count_TASK, (const portCHAR*)"Count Task", configMINIMAL_STACK_SIZE, NULL ,1,NULL);	//create count task
xTaskCreate(Periodic_TASK, (const portCHAR*)"Periodic Task", configMINIMAL_STACK_SIZE, NULL ,3,NULL);	//creat periodic task higher prioority

vTaskStartScheduler();
}
else {
	}
}
