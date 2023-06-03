/*
 ******************************************************************************
 * file      : delay.c
 * project   : EE 329 S'23 A4
 * details   : Hardware delay
 * authors   : James Savella - savella@calpoly.edu
 *             Donna Nikjou  - dnikou@calpol.edu
 * co-authors: Wilson Szeto  - wiszeto@calpoly.edu
 * 	      	   Cameron Buckeye - cbuckeye@calpoly.edu
 * version   : 0.1 : Imported from A3 project
 * date      : 230423
 * compiler  : STM32CubeIDE Version: 1.12.1 (2022)
 * target    : NUCLEO-L4A6ZG
 ******************************************************************************
 * origins   :
 * modified from CubeIDE auto-generated main.c  (c) 2022 STMicro.
 ******************************************************************************
 */
// Header guards
#ifndef DELAY
#define DELAY

#include "main.h"
#include "delay.h"

// configure SysTick timer for use with delay_us()
// warning: breaks HAL_delay() by disabling interrupts for shorter delay timing.
void SysTick_Init(void) {
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |	     // enable SysTick Timer
			SysTick_CTRL_CLKSOURCE_Msk);     // select CPU clock
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);      // disable interrupt
}

// delay in microseconds using SysTick timer to count CPU clock cycles
// do not call with 0 : error, maximum delay.
// careful calling with small nums : result in longer delays than specified:
//    e.g. @4MHz, delay_us(1) = 10=15 us delay.
void delay_us(const uint32_t time_us) {  // time_us = target delay, microseconds
	SysTick->LOAD = (uint32_t) ((time_us * (SystemCoreClock / 1000000)) - 1);
	SysTick->VAL = 0;                                   // clear the timer count
	SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);      // clear the count flag
	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
		; // wait for the flag
}

#endif // End of header guards

