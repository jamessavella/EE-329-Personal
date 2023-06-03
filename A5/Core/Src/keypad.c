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

#include "main.h"
#include "keypad.h"

static const int keypad[4][3] = //treat * as 10 and # as 11
		{ { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 10, 0, 11 } };

int rows[ROWS] = { ROW1, ROW2, ROW3, ROW4 };
int columns[COLUMNS] = { COL1, COL2, COL3 };

void keypadInitialize() {
	// Idea and framework for this section was from James Savella and Wilson Szeto. We asked them for help and this
	//   worked better. Called in main before while loop

	// configure PD0, PD1, PD2, PD3 for GPIO ROWS Input
	ROW_PINS->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2
			| GPIO_MODER_MODE3);
	ROW_PINS->PUPDR &= (GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2
			| GPIO_PUPDR_PUPD3);
	ROW_PINS->PUPDR |= (GPIO_PUPDR_PUPD0_1 | GPIO_PUPDR_PUPD1_1
			| GPIO_PUPDR_PUPD2_1 | GPIO_PUPDR_PUPD3_1);

	// configure PB4, PB5, PB6 for GPIO COLS Output
	COL_PINS->MODER &=
			~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6);
	COL_PINS->MODER |= (GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0
			| GPIO_MODER_MODE6_0);
}

int isAnyPressed() {
	//Framework for this portion is from the lab report. Changed variable names for our purposes
	COL_PINS->BSRR = (COL1 | COL2 | COL3);         	  // set all columns to high

	for (uint16_t i = 0; i < 10000; i++)
		// let it settle
		;
	if ((ROW_PINS->IDR & (ROW1 | ROW2 | ROW3 | ROW4)) != 0)   // got a keypress!
			{
		COL_PINS->BRR = (COL1 | COL2 | COL3);         	   // set all columns HI
		return (1);
	} else {
		COL_PINS->BRR = (COL1 | COL2 | COL3);         	   // set all columns HI
		return (0);
	}
}

int getKey() {
	int key = -1;                       // determine invalid value for key as -1
	for (int c = 0; c < COLUMNS; c++)     // for loop to go through columns
			{
		COL_PINS->BSRR = columns[c];    // turn on the column
		for (int r = 0; r < ROWS; r++)    // for loop to go through rows
				{
			if (ROW_PINS->IDR & (rows[r])) {
				key = keypad[r][c];
				break;
			}
		}

		if (key != -1)                   // determine if key has a valid value
				{
			break;
		}
	}
	return key;
}

int debounce() {
	int a = getKey();
	for (int i = 0; i < 500; i++) {
		;
	}
	int b = getKey();
	if (a == b) {
		return 1;
	} else {
		return 0;
	}
}
