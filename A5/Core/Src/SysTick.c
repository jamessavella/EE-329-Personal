/*
 ***************************************************************************************
 * file      : SysTick.c
 * project   : EE 329 S'23 A3
 * details   : microsecond delay function
 * authors   : Rushabh Shah - rshah49@calpoly.edu
 * version   : 1
 * date      : 4/14/2023
 * compiler  : STM32CubeIDE Version: 1.12.0
 * target    : NUCLEO-L4A6ZG
 ***************************************************************************************
 * Code given in EE 329 Lab Manual under section A3
 ***************************************************************************************
 */
#include "main.h"
#include "SysTick.h"

void SysTick_Init(void) {
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |	     // enable SysTick Timer
			SysTick_CTRL_CLKSOURCE_Msk);     // select CPU clock
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);      // disable interrupt
			// breaks HAL delay function
}

void delay_us(const uint32_t time_us) {
	SysTick->LOAD = (uint32_t) ((time_us * (SystemCoreClock / 1000000)) - 1);
	SysTick->VAL = 0;                                   // clear the timer count
	SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);      // clear the count flag
	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for the flag
}
