/*
 * LCD.c
 *
 *  Created on: Apr 17, 2023
 *      Author: Zack's PC
 */

#include "main.h"
#include "LCD.h"
#include <math.h>
#include "delay.h"
#include "keypad.h"
#include <string.h>

// anding with a ~ sets to 0
// oring sets it to 1

void LCD_config(void) {
	//LCD
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOEEN);
	GPIOE->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3 |
					  GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 |
					  GPIO_MODER_MODE7 | GPIO_MODER_MODE8);
	GPIOE->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE2_0| GPIO_MODER_MODE3_0 |
					 GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0 |
					 GPIO_MODER_MODE7_0 | GPIO_MODER_MODE8_0);
	GPIOE->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3 |
					   GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 |
					   GPIO_OTYPER_OT7 | GPIO_OTYPER_OT8);
	GPIOE->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3 |
					  GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 |
					  GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD8);
	GPIOE->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED0_Pos)| (3 << GPIO_OSPEEDR_OSPEED2_Pos) |
					   (3 << GPIO_OSPEEDR_OSPEED3_Pos)| (3 << GPIO_OSPEEDR_OSPEED4_Pos) |
					   (3 << GPIO_OSPEEDR_OSPEED5_Pos)| (3 << GPIO_OSPEEDR_OSPEED6_Pos) |
					   (3 << GPIO_OSPEEDR_OSPEED7_Pos)| (3 << GPIO_OSPEEDR_OSPEED8_Pos));
	GPIOE->BRR = 0xFF;
}
void LCD_init(void) {   // RCC & GPIO config removed - leverage A1, A2 code

	GPIOE->BRR = (GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5
			| GPIO_PIN_6 | GPIO_PIN_7);
	delay_us(40000);                     // power-up wait 40 ms

	GPIOE->BSRR = (GPIO_PIN_4 | GPIO_PIN_5); //P1 = (0x30) put 0x30 on output

	delay_us(30000);

	LCD_Pulse_Enable();			//wake up

	delay_us(10000);

	LCD_Pulse_Enable();			//wake up again

	delay_us(10000);

	LCD_Pulse_Enable();			//wake up again

	delay_us(10000);

	GPIOE->BRR = (GPIO_PIN_4); //put 0x20 on output

	delay_us(10);

	LCD_Pulse_Enable();		//function set: 4-bit interface

	LCD_Command(0x28);		//function set: 4-bit/2-line

	LCD_Command(0x0F);		//display ON; Blinking cursor

	LCD_Command(0x06);		//entry Mode set

	LCD_Command(0x02);		//return home

}

void LCD_Pulse_Enable(void) {
// ENAble line sends command on falling edge
// set to restore default then clear to trigger
	GPIOE->BSRR = (GPIO_PIN_3);

	delay_us(1000);

	GPIOE->BRR = (GPIO_PIN_3);
}

void LCD_Command(uint8_t Byte) {

	GPIOE->BSRR = (Byte & 0xF0); //keep upper 4 bits (masking) using data ins 4-7 (upper 4 bits)

	LCD_Pulse_Enable();

	GPIOE->BRR = 0xFF; //reset 8 bits

	GPIOE->BSRR = (Byte << 4) & 0xF0; // take data from lower 4 bits mask out everything else

	LCD_Pulse_Enable();

	GPIOE->BRR = 0xFF; //reset 8 bits
	delay_us(10); //small delay just in case
}

void LCD_write_char(uint8_t letter) {

	GPIOE->BSRR = (letter & 0xF0) | LCD_RS; //keep upper 4 bits (masking) using data ins 4-7 (upper 4 bits)

	LCD_Pulse_Enable();

	GPIOE->BRR = 0xF0; //reset 8 bits

	GPIOE->BSRR = (letter << 4) & 0xF0; // take data from lower 4 bits mask out everything else

	LCD_Pulse_Enable();

	GPIOE->BRR = 0xFF; //reset 8 bits
	delay_us(10); //small delay just in case
}

/*
 * Tanner Ehlers, EE 329 F'22 student. (2023-Apr-20).
 * Personal Communication: Tanner provided working C code for the write string function
 */
void LCD_write_string(char *string) {
	if(strlen(string) <=16) {
		for (int character = 0; character < strlen(string); character++) {
			LCD_write_char((uint8_t) string[character]);
		}
	}
}

void Display_Init(void) {
	LCD_Command(0x01);			//clear display
	delay_us(25000);
	LCD_Command(0x80 | 0x00);
	delay_us(400);
	LCD_write_string("SQU 200 Hz  LAST");
	LCD_Command(0xC0);			//next row
	LCD_write_string("50% DUTY     ' '");
}

void LCD_write_duty_cycle(int duty) {
	LCD_Command(0x80 | 0x40);
	if (duty == 0) {
		LCD_write_string("10");
	}
	if (duty == 1) {
		LCD_write_string("20");
	}
	if (duty == 2) {
		LCD_write_string("30");
	}
	if (duty == 3) {
		LCD_write_string("40");
	}
	if (duty == 4) {
		LCD_write_string("50");
	}
	if (duty == 5) {
		LCD_write_string("60");
	}
	if (duty == 6) {
		LCD_write_string("70");
	}
	if (duty == 7) {
		LCD_write_string("80");
	}
	if (duty == 8) {
		LCD_write_string("90");
	}
}

void LCD_write_freq(int freq) {
	LCD_Command(0x80 | 0x04);
	if (freq == 1) {
		LCD_write_string("100");
	}
	if (freq == 2) {
		LCD_write_string("200");
	}
	if (freq == 3) {
		LCD_write_string("300");
	}
	if (freq == 4) {
		LCD_write_string("400");
	}
	if (freq == 5) {
		LCD_write_string("500");
	}
}

//void LCD_Countdown_1s(uint16_t *count_array) {     //Counts down the 1 sec digit
//	int KEYS = 0;
//	while (count_array[3] != 0) {            //Check if the 1 sec digit is not 0
//		KEYS = Read_Keypad();
//		if (KEYS == star_Button) {          //Checks if the reset button has been pressed
//			LCD_Command(0x80 | 0x4B); //If reset, reset the count and go back to main
//			LCD_write_char('0');
//			LCD_write_char('0');
//			LCD_Command(0x14);
//			LCD_write_char('0');
//			LCD_write_char('0');
//			return;
//		}
//		count_array[3] = count_array[3] - 1;         //Decrement the 1 sec digit
//		LCD_Command(0x80 | 0x4F);                   //Move cursor to 1 sec digit
//		LCD_write_char(count_array[3] + 48);            //Update 1 sec digit
//		if (count_array[0] == 0 && count_array[1] == 0 && count_array[2] == 0 && count_array[3] == 0) {
//			while (KEYS != pound_Button) {           //Turn on LED until the GO button pressed
//				KEYS = Read_Keypad();
//				GPIOE->BSRR = (GPIO_PIN_8);
//				delay_us(5000);
//			}
//			GPIOE->BRR = (GPIO_PIN_8);        //Once GO button pressed, turn off LED
//		}
//		delay_us(960000);
//	}
//	LCD_Countdown_10s(count_array); //If 1 sec digit is 0, move to count down 10 sec digit
//}
//
//void LCD_Countdown_10s(uint16_t *count_array) {   //Counts down the 10 sec digit
//	if (count_array[2] != 0) {             //Checks if the 10 sec digit is not 0
//		count_array[2] = count_array[2] - 1;       //Decrements the 10 sec digit
//		count_array[3] = 9;                          //Sets the 1 sec digit to 9
//		LCD_Command(0x80 | 0x4E);                 //Moves cursor to 10 sec digit
//		LCD_write_char(count_array[2] + 48);   //Updates the 10 and 1 sec digits
//		LCD_write_char(count_array[3] + 48);
//		delay_us(965000);
//		LCD_Countdown_1s(count_array);     //Goes back to count down 1 sec digit
//	} else {
//		LCD_Countdown_1m(count_array); //If 10 sec digit is 0, move to count down 1 min digit
//	}
//}
//
//void LCD_Countdown_1m(uint16_t *count_array) {     //Counts down the 1 min digit
//	if (count_array[1] != 0) {              //Checks if the 1 min digit is not 0
//		count_array[1] = count_array[1] - 1;        //Decrements the 1 min digit
//		count_array[2] = 5;                         //Sets the 10 sec digit to 5
//		count_array[3] = 9;                          //Sets the 1 sec digit to 9
//		LCD_Command(0x80 | 0x4C);              //Moves cursor to the 1 min digit
//		LCD_write_char(count_array[1] + 48); //Updates the 1 min, 10 sec, and 1 sec digits
//		LCD_Command(0x14);
//		LCD_write_char(count_array[2] + 48);
//		LCD_write_char(count_array[3] + 48);
//		delay_us(965000);
//		LCD_Countdown_1s(count_array); //Goes back to count down the 1 sec digit
//	} else {
//		LCD_Countdown_10m(count_array); //If 1 min digit is 0, move to count down the 10 min digit
//	}
//}
//void LCD_Countdown_10m(uint16_t *count_array) {   //Counts down the 10 min digit
//	if (count_array[0] != 0) {             //Checks if the 10 min digit is not 0
//		count_array[0] = count_array[0] - 1;       //Decrements the 10 min digit
//		count_array[1] = 9;                          //Sets the 1 min digit to 9
//		count_array[2] = 5;                         //Sets the 10 sec digit to 5
//		count_array[3] = 9;                          //Sets the 1 sec digit to 9
//		LCD_Command(0x80 | 0x4B);               //Moves the cursor to the 10 min
//		LCD_write_char(count_array[0] + 48);            //Updates all digits
//		LCD_write_char(count_array[1] + 48);
//		LCD_Command(0x14);
//		LCD_write_char(count_array[2] + 48);
//		LCD_write_char(count_array[3] + 48);
//		delay_us(965000);
//		LCD_Countdown_1s(count_array); //Goes back to count down the 1 sec digit
//	}
//}
