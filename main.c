#include "tm4c123gh6pm.h"
#include <stdint.h> 
#include "bsp.h"
void blink_red(void);
void blink_blue(void);
static uint32_t volatile l_tickCti;
uint32_t start ;
uint32_t ticks=1;

void blink_red()
	{
    while (1) {
        GPIO_PORTF_DATA_R ^= LED_RED;
        __asm("CPSID I");
			 start = l_tickCti;
			__asm("CPSIE I");
			while((l_tickCti-start)<5){};
    }
}
void blink_blue() {
    while (1) {
        GPIO_PORTF_DATA_R ^= LED_BLUE;
        __asm("CPSID I");
			start = l_tickCti;
			__asm("CPSIE I");
			while((l_tickCti-start)<10){};
		}
}
int main() {
    SYSCTL_RCGCGPIO_R = 0x20U;
    GPIO_PORTF_DIR_R = 0x0EU;
    GPIO_PORTF_DEN_R = 0x0EU;
    NVIC_ST_RELOAD_R = 1600000-1;   /* reload reg. with max value */
    NVIC_ST_CTRL_R = 7;         /* enable it, enable interrupt, use system clock */
    // Enable interrupts to the processor.
    __asm("CPSIE  I");
	
    blink_blue();
		blink_red();
        while (1) {
    }
}    
void SysTick_Handler(void) {
    ++l_tickCti;
}
