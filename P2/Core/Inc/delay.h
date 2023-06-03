/*
 * delay.h
 *
 *  Created on: Apr 19, 2023
 *      Author: Zack's PC
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

void SysTick_Init(void);
void delay_us(const uint32_t time_us);

#endif /* INC_DELAY_H_ */
