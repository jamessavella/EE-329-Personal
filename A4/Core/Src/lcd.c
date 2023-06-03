/*
 * LCD.c
 *
 *  Created on: Apr 17, 2023
 *      Author: tazzn
 */

#include "LCD.h"
#include "SysTick.h"
#include <string.h>
#include <stdio.h>

void LCD_command( uint8_t command )  { // send command to LCD in 4-bit instruction mode
	GPIOE->ODR &= ~(R_W | RS | LCD_DATA_BITS); // write & command mode, reset data pins
	GPIOE->ODR |= (command & LCD_DATA_BITS);
	delay_us(150);
	LCD_pulse_ENA();
	GPIOE->ODR &= ~(LCD_DATA_BITS);
	GPIOE->ODR |= ((command << 4) & LCD_DATA_BITS);
	delay_us(150);
	LCD_pulse_ENA();
	delay_us(1600);
}

void LCD_write_char( uint8_t data )  {
// calls LCD_command() w/char data; assumes all ctrl bits set LO in LCD_init()
	GPIOE->ODR &= ~(R_W | LCD_DATA_BITS);  // write mode, reset data pins
	GPIOE->ODR |= (RS);             // data mode
	GPIOE->ODR |= (data & LCD_DATA_BITS); // send upper four bits of data
	delay_us(150);
	LCD_pulse_ENA();
	GPIOE->ODR &= ~LCD_DATA_BITS;           // reset data pins
	GPIOE->ODR |= ((data << 4) & LCD_DATA_BITS); // send lower four bits of data
	delay_us(150);
	LCD_pulse_ENA();
	delay_us(200);
}

void LCD_write_str( char *string )  {
// calls LCD_command() w/char data; assumes all ctrl bits set LO in LCD_init()
	if(strlen(string) <= 16) {
		for (int character = 0; character < strlen(string); character ++) {
			LCD_write_char((uint8_t) string[character]);
		}
	}
}



void LCD_pulse_ENA( void )  {
// ENAble line sends command on falling edge
// set to restore default then clear to trigger
   GPIOE->BSRR = En;         	// ENABLE = HI
   delay_us(5);                         // TDDR > 320 ns
   GPIOE->BRR  = En;        // ENABLE = LOW
   delay_us(5);
}

void LCD_init(void){ // initialize LCD
	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOEEN);


	PIN->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6
				| GPIO_MODER_MODE7 |GPIO_MODER_MODE8| GPIO_MODER_MODE9|GPIO_MODER_MODE10); // save all other pin modes and reset PD0 - PD3

	PIN->MODER |= (GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0
				| GPIO_MODER_MODE7_0 |GPIO_MODER_MODE8_0 |GPIO_MODER_MODE9_0|GPIO_MODER_MODE10_0); // set the desired modes for pins PD0 - PD3

	PIN->OTYPER &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6
				| GPIO_OTYPER_OT7| GPIO_OTYPER_OT8| GPIO_OTYPER_OT9| GPIO_OTYPER_OT10); // set PD0 - PD3to push-pull and all other pins to open-drain

	PIN->PUPDR &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6
				| GPIO_PUPDR_PUPD7| GPIO_PUPDR_PUPD8| GPIO_PUPDR_PUPD9| GPIO_PUPDR_PUPD10); // set PD0 - PD3 to No pull-up, pull-down and all other PC to reserved

	PIN->OSPEEDR |= ((1 << GPIO_OSPEEDR_OSPEED4_Pos)
				| (1 << GPIO_OSPEEDR_OSPEED5_Pos) | (1 << GPIO_OSPEEDR_OSPEED6_Pos)
				| (1 << GPIO_OSPEEDR_OSPEED7_Pos) | (1 << GPIO_OSPEEDR_OSPEED8_Pos)
				| (1 << GPIO_OSPEEDR_OSPEED9_Pos)| (1 << GPIO_OSPEEDR_OSPEED10_Pos)) ;

	PIN->BRR = (R_W|RS|En|DB4|DB5|DB6|DB7);

	SysTick_Init();

	delay_us(40000);			// power-up wait 40 ms
	for(int i=0; i<3; i++){
	GPIOE->ODR &= ~LCD_DATA_BITS;// wait 40 ms
	GPIOE->ODR |= (0x30 >> 4);         // 0x30 is for wake up?
	delay_us(200);
	LCD_pulse_ENA();
	}
	GPIOE->ODR &= ~LCD_DATA_BITS;// wait 40 ms
	GPIOE->ODR |= (0x20 >> 4);
	LCD_pulse_ENA();      // 4-bit / 2-line
	LCD_command(func_set);
	LCD_command(shft_cursor_l);  // set cursor
	LCD_command(disp_entire_blnk);  // display ON, blinking cursor
	LCD_command(entry_mode_inc);  // entry mode set

}
