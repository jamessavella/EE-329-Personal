/*
 * delay.c
 *
 *  Created on: Apr 19, 2023
 *      Author: Zack's PC
 */

#include "main.h"
#include "delay.h"
#include <math.h>

void SysTick_Init(void) {
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |     // enable SysTick Timer
                      SysTick_CTRL_CLKSOURCE_Msk); 	// select CPU clock
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);  	// disable interrupt
}

void delay_us(const uint32_t time_us) {
	// set the counts for the specified delay
	SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 9000000)) - 1);
	SysTick->VAL = 0;                                  	 // clear timer count
	SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);    	 // clear count flag
	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for flag
}
