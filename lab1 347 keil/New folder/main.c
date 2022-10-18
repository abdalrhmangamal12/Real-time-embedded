#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <FreeRTOS.h>
#include "semphr.h"

 SemaphoreHandle_t xMutex;
void Task3(void *pvParameters);

void Task2(void *pvParameters) ;
void Task1(void *pvParameters);
void Task1(void *pvParameters)
{
  while(1)
  {	vTaskDelay(100/portTICK_RATE_MS);
   xSemaphoreTake(xMutex,portMAX_DELAY);

   xSemaphoreGive(xMutex);
  }}
void Task2(void *pvParameters)
{    while(1){
		vTaskDelay(50/portTICK_RATE_MS);
}}
void Task3(void *pvParameters)
{ int i,j ; 
	while(1) {
   		xSemaphoreTake(xMutex,portMAX_DELAY);
    for(i=0 ; i<10000000 ;i++)
		j++;

    xSemaphoreGive(xMutex);
  }
}

int main( void )
{ xMutex = xSemaphoreCreateMutex(); // create mutex with name xMutex
  xTaskCreate(Task1, "Task1",100,NULL,3,NULL); // Create Task1 with priority 1 
	  xTaskCreate(Task2, "Task2",100,NULL,2,NULL); // Create Task1 with priority 1 
    xTaskCreate(Task3, "Task3",100,NULL,1,NULL); // Create Task1 with priority 1 
  vTaskStartScheduler();

 
}