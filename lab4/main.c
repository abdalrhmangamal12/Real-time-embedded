#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "tm4c123gh6pm.h"

void RedTask(void);
void BlueTask(void);
void greenTask(void);
volatile TickType_t  xLastWakeTime1 ;
volatile TickType_t  xLastWakeTime2 ;
volatile TickType_t  xLastWakeTime3 ;
void Portf_init(void);

int main(void){
	
	Portf_init();
		const TickType_t delay1 = 1000/portTICK_PERIOD_MS ;
		const TickType_t delay2 = 2000/portTICK_PERIOD_MS ;
		const TickType_t delay3 = 3000/portTICK_PERIOD_MS ;

   xTaskCreate ( RedTask,"rt",100,delay1  ,  1  , 0);
	 xTaskCreate ( BlueTask,"Bt",100,delay1  ,  1  , 0);
	 xTaskCreate ( greenTask,"Gt",100,delay1  ,1 , 0);
	vTaskStartScheduler();
	
	while(1){
		
	}
	
}

void Portf_init(void){
	SYSCTL_RCGCGPIO_R|=0x20;
  while((SYSCTL_PRGPIO_R&0x20)==0){};
  GPIO_PORTF_LOCK_R=0x4C4F434B;
  GPIO_PORTF_CR_R=0x0F;
  GPIO_PORTF_DIR_R=0x0F;
  GPIO_PORTF_PUR_R=0x01;
  GPIO_PORTF_DEN_R=0x0F;
}

void RedTask(void){
	
 xLastWakeTime1 = xTaskGetTickCount();
	while(1){
		GPIO_PORTF_DATA_R^= 0x02;
	
		vTaskDelayuntil(xLastWakeTime1 ,1000 / portTICK_RATE_MS );
	}
}

void BlueTask(void){
	xLastWakeTime2 = xTaskGetTickCount();

	while(1){
		GPIO_PORTF_DATA_R^= 0x04;
	
		vTaskDelayuntil(xLastWakeTime2 ,2000 / portTICK_RATE_MS );
	}
}

void greenTask( void )
{   
	 xLastWakeTime3 = xTaskGetTickCount();
	while(1){
	GPIO_PORTF_DATA_R^= 0x08;
		
	vTaskDelayuntil( xLastWakeTime3, 3000 / portTICK_RATE_MS );
}
}