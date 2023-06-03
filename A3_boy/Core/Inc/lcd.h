/*
 * LCD.h
 *
 *  Created on: Apr 18, 2023
 *      Author: Wilso
 */

#ifndef _LCD_H_
#define _LCD_H_

#include "main.h"
#include "stm32l4xx_hal.h"


// Macro definitions for row and column ports
#define control_pins GPIOA // Port for rows
#define data_pins GPIOA // Port for columns

// Macro definitions for row pins
#define RS GPIO_PIN_1 // Row 1 pin
#define RW GPIO_PIN_2 // Row 2 pin
#define EN GPIO_PIN_3 // Row 3 pin

// Macro definitions for row pins
#define D4 GPIO_PIN_4 // Row 1 pin
#define D5 GPIO_PIN_5 // Row 2 pin
#define D6 GPIO_PIN_6 // Row 3 pin
#define D7 GPIO_PIN_7 // Row 4 pin



#endif /* INC_LCD_H_ */
