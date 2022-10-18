#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "relay.h"
void relay_Init()
{
	SYSCTL_RCGCGPIO_R |= 0x2;                                     // initialize clock
	while(((SYSCTL_PRGPIO_R&0x2) == 0)){} 
	GPIO_PORTB_CR_R |= 0x11; 
  GPIO_PORTB_DIR_R |= 0x11;
	GPIO_PORTB_DEN_R |= 0x11;
}
void set_Device(int D , int M)
{
	switch (D)
	{
		case 1:
			if(M == 1){GPIO_PORTB_DATA_R |= 0x1;}
			if(M == 0){GPIO_PORTB_DATA_R &=~ 0x1;}
	  case 2:
			if(M == 1){GPIO_PORTB_DATA_R |= 0x10;}
			if(M == 0){GPIO_PORTB_DATA_R &=~ 0x10;}
	}
}