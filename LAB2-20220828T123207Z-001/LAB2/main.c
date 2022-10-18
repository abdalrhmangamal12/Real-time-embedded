#include "tm4c123gh6pm.h"
#include <stdint.h> 
#include "bsp.h"
void main_blinky1(void);
void main_blinky2(void);

uint32_t stack_blinky1[40];
uint32_t *sp_blinky1 = &stack_blinky1[40];
void main_blinky1() {
    while (1) {
        GPIO_PORTF_DATA_R = LED_RED;
        BSP_delay(BSP_TICKS_PER_SEC / 100U);
        GPIO_PORTF_DATA_R &= ~LED_RED;
        BSP_delay(BSP_TICKS_PER_SEC / 100U);
    }
}

uint32_t stack_blinky2[40];
uint32_t *sp_blinky2 = &stack_blinky2[40];
void main_blinky2() {
    while (1) {
        GPIO_PORTF_DATA_R = LED_BLUE;
        BSP_delay(BSP_TICKS_PER_SEC / 100U);
        GPIO_PORTF_DATA_R &= ~LED_BLUE;
        BSP_delay(BSP_TICKS_PER_SEC / 100U);
    }
}
int main() {
    SYSCTL_RCGCGPIO_R = 0x20U;
    GPIO_PORTF_DIR_R = 0x0EU;
    GPIO_PORTF_DEN_R = 0x0EU;
    NVIC_ST_RELOAD_R = 0xFFFFFF;   /* reload reg. with max value */
    NVIC_ST_CTRL_R = 7;         /* enable it, enable interrupt, use system clock */
    // Enable interrupts to the processor.
    __asm("CPSIE  I");
    /* fabricate Cortex-M ISR stack frame for blinky1 */
    *(--sp_blinky1) = (1U << 24);  /* xPSR */
    *(--sp_blinky1) = (uint32_t)&main_blinky1; /* PC */
    *(--sp_blinky1) = 0x0000000EU; /* LR  */
    *(--sp_blinky1) = 0x0000000CU; /* R12 */
    *(--sp_blinky1) = 0x00000003U; /* R3  */
    *(--sp_blinky1) = 0x00000002U; /* R2  */
    *(--sp_blinky1) = 0x00000001U; /* R1  */
    *(--sp_blinky1) = 0x00000000U; /* R0  */
    /* fabricate Cortex-M ISR stack frame for blinky2 */
    *(--sp_blinky2) = (1U << 24);  /* xPSR */
    *(--sp_blinky2) = (uint32_t)&main_blinky2; /* PC */
    *(--sp_blinky2) = 0x0000000EU; /* LR  */
    *(--sp_blinky2) = 0x0000000CU; /* R12 */
    *(--sp_blinky2) = 0x00000033U; /* R3  */
    *(--sp_blinky2) = 0x00000022U; /* R2  */
    *(--sp_blinky2) = 0x00000011U; /* R1  */
    *(--sp_blinky2) = 0x00000000U; /* R0  */
//     uint32_t volatile run = 0U; 
//		if (run) {
//			main_blinky1();
//		}
//		else {
//		}
        while (1) {
    }
}    
