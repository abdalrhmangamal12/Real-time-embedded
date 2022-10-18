#include "UART.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"

void uart0_Init(void){
		
UART0_CTL_R = 0;
UART0_IBRD_R = 104;
UART0_FBRD_R= 11;
UART0_CC_R = 0;
UART0_LCRH_R = 0x60;
UART0_CTL_R = 0x301;
	
GPIO_PORTA_AFSEL_R = 0x03;
GPIO_PORTA_PCTL_R = 0x11;
GPIO_PORTA_DEN_R = 0x03;
}

void UART0_sendChar(char Char)
{
    while( (UART0_FR_R & 0x20) != 0);
    UART0_DR_R = Char;
}

void UART0_sendState(char *string)
{
  while(*string)
  {
      UART0_sendChar(*(string++));
  }
}