/*
 ***************************************************************************************
 * file      : SysTick.c
 * project   : EE 329 S'23 A6
 * group     : GB
 * details   : Microsecond Delay
 * authors   : Rushabh Shah - rshah49@calpoly.edu
 *      Current Project Partner(s)
 *           : James Savella - jsavella@calpoly.edu
 *      Original project partner(s)
 *           : Jack Shaffer - jdshaffer@calpoly.edu
 *           : Mason McMguire - mmcgui09@calpoly.edu
 *           : Hardit Singh - hsingh51@calpoly.edu
 *           : Zachary Numa - znuma@calpoly.edu
 *           : Steve Tran - stran100@calpoly.edu
 *           : Zach Kwast - zkwast@calpoly.edu
 * version   : 1
 * date      : 5/19/2023
 * compiler  : STM32CubeIDE Version: 1.12.0
 * target    : NUCLEO-L4A6ZG
 ***************************************************************************************
 */
#include "main.h"
#include "SysTick.h"
void SysTick_setup(void) {
    SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |      // enable SysTick Timer
            SysTick_CTRL_CLKSOURCE_Msk);     // select CPU clock
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);      // disable interrupt
            // breaks HAL delay function
}
void delay_u(const uint32_t time_us) {
    SysTick->LOAD = (uint32_t) ((time_us * (SystemCoreClock / 1000000)) - 1);
    SysTick->VAL = 0;                                   // clear the timer count
    SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);      // clear the count flag
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for the flag
}
