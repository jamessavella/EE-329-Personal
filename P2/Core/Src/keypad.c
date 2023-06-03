/*
 * keypad.c
 *
 *  Created on: Apr 17, 2023
 *      Author: Zack's PC
 */

#include "keypad.h"
#include <math.h>
#include "main.h"
#include "delay.h"

void Keypad_init(void) {
	// anding with a ~ sets to 0
	// oring sets it to 1
	// Columns: PD7, PD6, PD5, PD4
	// Rows:    PD0, PD1, PD2, PD3
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIODEN);
	//Columns
	GPIOD->MODER &= ~(GPIO_MODER_MODE7 | GPIO_MODER_MODE6 | GPIO_MODER_MODE5
			| GPIO_MODER_MODE4);
	GPIOD->MODER |= (GPIO_MODER_MODE7_0 | GPIO_MODER_MODE6_0
			| GPIO_MODER_MODE5_0 | GPIO_MODER_MODE4_0);
	GPIOD->OTYPER &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT5
			| GPIO_OTYPER_OT4);
	GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD5
			| GPIO_PUPDR_PUPD4);
	GPIOD->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED7_Pos)
			| (3 << GPIO_OSPEEDR_OSPEED6_Pos) | (3 << GPIO_OSPEEDR_OSPEED5_Pos)
			| (3 << GPIO_OSPEEDR_OSPEED4_Pos));

	//Rows
	GPIOD->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2
			| GPIO_MODER_MODE3);
	GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2
			| GPIO_PUPDR_PUPD3);
	GPIOD->PUPDR |= (2 << GPIO_PUPDR_PUPD0_Pos);
	GPIOD->PUPDR |= (2 << GPIO_PUPDR_PUPD1_Pos);
	GPIOD->PUPDR |= (2 << GPIO_PUPDR_PUPD2_Pos);
	GPIOD->PUPDR |= (2 << GPIO_PUPDR_PUPD3_Pos);
}

unsigned int Read_Keypad(void) {
	int IDR_CONTENTS = 0;
	int ROW_CONTENTS = 0;
	int SUCCESS_COUNT = 0;
	int ROW = 0;
	int COL = 0;
	unsigned int KEY = 0;
	for (int COL_PORT = 0; COL_PORT <= 3; COL_PORT ++) {                         //Starts for loop to set each of the columns high
	    GPIOD->ODR &= (0b1111111100001111);                                      //Masks all ODR bits not in use
	    GPIOD->ODR |= 0b0000 << 4;                                                    //Turns off all columns
	    GPIOD->ODR |= (0b1000 >> COL_PORT) << 4;                                 //Sets high the column corresponding to the variable in the previous for loop
	    delay_us(10000);
	    IDR_CONTENTS = GPIOD->IDR & 0x0F;                                               //Reads the input data register from the rows
	    IDR_CONTENTS &= (0b0000000000001111);
	    if (IDR_CONTENTS != 0) {                                                 //Starts section to code to execute if one of the rows set high
	    	for (int CHECK_COUNT = 0; CHECK_COUNT <= 100; CHECK_COUNT ++) {      //Starts for loop to read the IDR 100 times
	    		for (int DELAY_COUNT = 100; DELAY_COUNT >= 0; DELAY_COUNT --);  //Just a delay
	    		IDR_CONTENTS = GPIOD->IDR & 0x0F;                                       //Reads the IDR from the rows
	     		IDR_CONTENTS &= (0b0000000000001111);
	    		if (IDR_CONTENTS != 0) {                                         //If statement to count the number of times a row was set high
	    			SUCCESS_COUNT ++;
	    			COL = COL_PORT + 1;
	    	        ROW_CONTENTS = IDR_CONTENTS;
	    		}
	    		else {                                                           //If no rows were set to high, reset the success count to zero
	    			SUCCESS_COUNT = 0;
	    		}
	    		if (SUCCESS_COUNT >= 50) {                                       //Once a row has been read as on 50 times, exit the 100-instance for loop
	    			break;
	    		}
	    	}
	    }
	}

	if (ROW_CONTENTS == 0b1000) {                //Case for determining row 4 pressed
		ROW = 0b0100;
	}
	if (ROW_CONTENTS == 0b0100) {                //Case for determining row 3 pressed
	    ROW = 0b0011;
    }
	if (ROW_CONTENTS == 0b0010) {                //Case for determining row 2 pressed
		ROW = 0b0010;
	}
	if (ROW_CONTENTS == 0b0001) {                //Case for determining row 1 pressed
	    ROW = 0b0001;
    }

	if (ROW == 1 && COL == 1) {                  //Case for key 1 pressed
		KEY = 1;
	}
	if (ROW == 1 && COL == 2) {                  //Case for key 2 pressed
	    KEY = 2;
	}
	if (ROW == 1 && COL == 3) {                  //Case for key 3 pressed
	    KEY = 3;
	}
	if (ROW == 1 && COL == 4) {                  //Case for key A pressed
	    KEY = 12;
	}
	if (ROW == 2 && COL == 1) {                  //Case for key 4 pressed
	    KEY = 4;
	}
	if (ROW == 2 && COL == 2) {                  //Case for key 5 pressed
	    KEY = 5;
	}
	if (ROW == 2 && COL == 3) {                  //Case for key 6 pressed
	    KEY = 6;
	}
	if (ROW == 2 && COL == 4) {                  //Case for key B pressed
	    KEY = 12;
	}
	if (ROW == 3 && COL == 1) {                  //Case for key 7 pressed
	    KEY = 7;
	}
	if (ROW == 3 && COL == 2) {                  //Case for key 8 pressed
	    KEY = 8;
	}
	if (ROW == 3 && COL == 3) {                  //Case for key 9 pressed
	    KEY = 9;
	}
	if (ROW == 3 && COL == 4) {                  //Case for key C pressed
	    KEY = 12;
	}
	if (ROW == 4 && COL == 1) {                  //Case for SET key pressed
	    KEY = 10;
	}
	if (ROW == 4 && COL == 2) {                  //Case for key 0 pressed
	    KEY = 0;
	}
	if (ROW == 4 && COL == 3) {                  //Case for GO key pressed
	    KEY = 11;
	}
	if (ROW == 4 && COL == 4) {                  //Case for key D pressed
	    KEY = 12;
	}
	if (ROW == 0 && COL == 0) {                  //Case for no key pressed
		KEY = 12;
	}

    return KEY;                                  //Return whatever key was pressed
}
