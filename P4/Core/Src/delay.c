
// Header guards
#ifndef DELAY
#define DELAY

//includes
#include "main.h"
#include "delay.h"


// configure SysTick timer for use with delay_us(). This will break
// warning: breaks HAL_delay() by disabling interrupts for shorter delay timing.
void SysTick_Init(void){
    SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |	       // enable SysTick Timer
                      SysTick_CTRL_CLKSOURCE_Msk);     // select CPU clock
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);      // disable interrupt
}

// delay in microseconds using SysTick timer to count CPU clock cycles
// do not call with 0 : error, maximum delay.
// careful calling with small nums : result in longer delays than specified:
//    e.g. @4MHz, delay_us(1) = 10=15 us delay.
void delay_us(const uint32_t time_us) {  // time_us = target delay, microseconds
    SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);
    SysTick->VAL = 0;                                      // clear the timer count
    SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);        // clear the count flag
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for the flag
}

#endif // End of header guards
