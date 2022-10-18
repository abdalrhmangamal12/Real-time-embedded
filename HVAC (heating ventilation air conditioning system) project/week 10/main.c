#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <stdbool.h>
#include <stdint.h>
#include "gpio.h"
#include "hw_gpio.h"
#include "sysctl.h"
#include "interrupt.h"
#include "hw_memmap.h"
#include "uart.h"
#include "hw_uart.h"
#include "hw_types.h"
#include "semphr.h"
#include "LCD.h"
#include "ADC.h"
#include "tm4c123gh6pm.h"
#include "Keypad.h"
#include "relay.h"
// global variable
int slack = 1;
int templimit=26;
xQueueHandle xQueue;
////////////////////////
// PROTOTYPES
void INIT_TASK(void *pvParameters);
void uart_Task(void *pvParameters);
void ADC_Task(void *pvParameters);
void ChangeThreshold( void *pvParameters );
////////////////////////////////////////

void INIT()
{
	ADCInt();
	relay_Init();
}
void LCD_Task()
{
	LCD lcd = LCD_create();
	LCD_init(&lcd);

	portBASE_TYPE xStatus;
	int temp;
	while(1)
	{
		xStatus = xQueueReceive(xQueue,&temp,0);
		LCD_clear(&lcd);
		LCD_setPosition(&lcd,1,0);
		LCD_sendNum(&lcd,temp);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
	
}
void ADC_Task()
{
	portBASE_TYPE xStatus;  
	//(ox10) portBpin4 = fan 0 = on 1 = off
	//(0x1) portBpin0 = heater 0 on  1 = off
	while(1)
	{
		int temp = ADCreadTemp();
		if(temp < (templimit-slack))
		{
			// heat on fan off
			GPIO_PORTB_DATA_R &=~ 0x1;
			GPIO_PORTB_DATA_R |= 0x10;
			//vTaskDelay(30/portTICK_PERIOD_MS);
		}
		if(temp > templimit)
		{
			// heat off fan on
			GPIO_PORTB_DATA_R  |= 0x1;
			GPIO_PORTB_DATA_R &=~ 0x10;
			//vTaskDelay(30/portTICK_PERIOD_MS);
		}
		xStatus = xQueueSendToBack(xQueue,&temp,portMAX_DELAY);
	}
	
}
int main() 
{
	INIT();
	xQueue = xQueueCreate (1,sizeof(unsigned int));
	if(xQueue!=NULL)
		{
			xTaskCreate(ADC_Task,"ADC_Task",420,NULL,1,NULL);
			xTaskCreate(LCD_Task,"LCD_Task",420,NULL,1,NULL);
			vTaskStartScheduler();
		}
  else
	{
	}
	
}


	
