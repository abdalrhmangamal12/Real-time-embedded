#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "uart.h"
#include "TM4C123.h"
#include "hw_memmap.h"
#include "hw_types.h" 
#include "hw_gpio.h"
#include "basic_io.h"
#include "uartstdio.h"




int counter = 0;
xQueueHandle xQueue;
 
void UART5_Transmitter(int data); //thi is int not char


void BTN1_CHK_TASK(void *)
	{
		for(;;){
			int BTN1 = GPIOF->DATA & 0x01;
			if(BTN1 == 0)
			{
						vTaskDelay( 200/ portTICK_RATE_MS );
				counter++;
					vTaskDelay( 200/ portTICK_RATE_MS );
			}
			taskYIELD();
		}
}
void BTN2_CHK_TASK(void *){
		portBASE_TYPE xstatus;
	
		for(;;)
		{
			int BTN2 = GPIOF->DATA & 0x10;
			if(GPIOF->DATA == 0x01)
			{
				vTaskDelay( 200/ portTICK_RATE_MS );
				xstatus = xQueueSendToBack(xQueue, 	&counter, 0);
				if(xstatus == pdPASS)
				{
					counter = 0;
				}
				vTaskDelay( 200/ portTICK_RATE_MS );
			}
				taskYIELD();
		}
		}
void UART_TASK(void *){
		int ReceivedValue;
	  portBASE_TYPE xStatus;
	  unsigned char buffer [2];
    for(;;){
		  xStatus = xQueueReceive(xQueue,&ReceivedValue,portMAX_DELAY);
	  	if (xStatus == pdPASS) {
				  	sprintf (buffer, "%d",ReceivedValue);
				  	for(int i = 0; buffer[i] != '\0'; i++){
				  		UARTCharPut(UART0_BASE, buffer[i]);
				  	}
						UARTCharPut(UART0_BASE, '\r');
				  	UARTCharPut(UART0_BASE, '\n');
				  }
  }
}


void UART_PORTF_Init(void)
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
	UART0->IBRD=104;
	UART0->FBRD=11;
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

int main(){
	UART_PORTF_Init();
		xQueue= xQueueCreate(5,sizeof(int));
		if(xQueue!=NULL)
		{
			
		  xTaskCreate(UART_TASK,"Uart_task",240, NULL, 2, NULL);
			xTaskCreate(BTN1_CHK_TASK,"button_1 task",240, NULL, 1, NULL);
			xTaskCreate(BTN2_CHK_TASK,"button_2 task",240,  NULL, 1, NULL);
			vTaskStartScheduler();
		}
		else{}
		while(1){};
}


