#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "D:\TivaWare\inc/hw_types.h"
#include "D:\TivaWare\driverlib/gpio.h"
#include "D:\TivaWare\driverlib/pin_map.h"
#include "D:\TivaWare\inc/hw_memmap.h"
#include "tm4c123gh6pm.h"


xQueueHandle xQueue;
SemaphoreHandle_t xCountingSemaphore;
static void UART_TASK(void *pvParameters);

unsigned int counter = 0;
char BUFFER[2];

void Delay(int count)
{
	int i,j;
  
  for(i = 0; i<count; i++)
		for(j = 0; j<3180; j++);
}
void printchar(char c)
{
	
	while ((UART0_FR_R & (1 << 5)) != 0)
		;
	UART0_DR_R =c;
}

void print(char *string)
{
	
	while (*string)
	{
		printchar(*(string++));
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void init()
{
	 	SYSCTL->RCGCUART |= 0x1; 
    SYSCTL->RCGCGPIO |= 0x1; 
    while((	SYSCTL->RCGCUART & 0x1)==0){};
	  while((SYSCTL->RCGCGPIO & 0x1)==0){};
    GPIOA->AFSEL = 0x3; 
    GPIOA->PCTL = 0x11; 
		GPIOA->DEN = 0x3; 
   UART0->CTL &= ~0x0001;
	UART0->CC=0X0;
	UART0->IBRD=27;
	UART0->FBRD=8;
	UART0->LCRH=(0x3<<5); 
	UART0->CTL=0x301;
	
	  SYSCTL->RCGCGPIO |= 0x20;   
	  while((SYSCTL->RCGCGPIO & 0x20)==0){};
    GPIOF->LOCK = 0x4C4F434B;   
    GPIOF->CR = 0x01;           
    GPIOF->PUR |= 0x11;        
    GPIOF->DIR |= 0x0;         
    GPIOF->DEN |= 0x11;

}

static void BTN_CHK_TASK(void *pvParameters)
{

	portBASE_TYPE xStatus;

	const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

	for (;;)
	{
		
	int BTN1 = GPIOF->DATA & 0x01;
			if(BTN1 == 0)
			{
						vTaskDelay( 200/ portTICK_RATE_MS );
				counter++;
					vTaskDelay( 200/ portTICK_RATE_MS );
			}

			xStatus = xQueueSendToBack(xQueue, &counter, xTicksToWait);
			xSemaphoreGive(xCountingSemaphore);

	

	
			taskYIELD()
		}
	}



static void UART_TASK(void *pvParameters)
{
	portBASE_TYPE xStatus;
	xSemaphoreTake(xCountingSemaphore, 0);
	unsigned int recivedValue = 0;
	for (;;)
	{


		xStatus = xQueueReceive(xQueue, &recivedValue, 0);
		xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);
		sprintf(BUFFER,"%d",recivedValue);
		
		print(BUFFER);
		print("\n\r");
	}
}

int main()
{
	init();

	xQueue = xQueueCreate(3, sizeof(unsigned int));
	xCountingSemaphore = xSemaphoreCreateCounting(1, 1);

	if (xCountingSemaphore != NULL)
	{
		xTaskCreate(BTN_CHK_TASK, "BUTTON", 130, NULL, 1, NULL);
		xTaskCreate(UART_TASK, "UART", 100, NULL,2, NULL);
		vTaskStartScheduler();
	}

	for (;;)
		;
}

