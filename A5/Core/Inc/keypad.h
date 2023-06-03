/*
 * Name: Kelvin Villago and Emily Bodoh
 * Date: 4/14/2023
 * Course: EE 329 Section 05
 * Project: Assignment A2
 * Description: Implement keypad functionality and output keypad to LEDs.
 * 	Also implemented software debounce and seperate keypad .c and .h files
 * Wiring: PC0-PC3 are tied to a 560 ohm resistor that is connected to LED.
 * 	PD0-PD3 are connected to keypad row pins 5-8
 * 	PB4-6 are connected to keypad column pins 2-4
 */

#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

#define ROWS 4
#define COLUMNS 3
#define ROW_PINS GPIOD
#define COL_PINS GPIOD  //GPIOB testing with D

#define ROW1 GPIO_PIN_0
#define ROW2 GPIO_PIN_1
#define ROW3 GPIO_PIN_2
#define ROW4 GPIO_PIN_3

#define COL1 GPIO_PIN_4
#define COL2 GPIO_PIN_5
#define COL3 GPIO_PIN_6

int isAnyPressed();
int debounce();
int getKey();
void keypadInitialize();

#endif /* INC_KEYPAD_H_ */
