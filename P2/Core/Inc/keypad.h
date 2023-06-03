/*
 * keypad.h
 *
 *  Created on: Apr 17, 2023
 *      Author: Zack's PC
 */

#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_
#include "stm32l4xx_hal.h"

void Keypad_init(void);
unsigned int Read_Keypad(void);
#define star_Button (10)
#define valid_Num (9)
#define pound_Button (11)

#endif /* INC_KEYPAD_H_ */
