/*
 * LCD.h
 *
 *  Created on: Apr 17, 2023
 *      Author: tazzn
 */



#ifndef LCD_LCD_H_
#define LCD_LCD_H_

#include "stm32l4xx_hal.h"

//Pin defines
#define PIN	GPIOE
#define DB4 GPIO_PIN_4
#define DB5 GPIO_PIN_5
#define DB6 GPIO_PIN_6
#define DB7 GPIO_PIN_7
#define LCD_DATA_BITS (DB4|DB5|DB6|DB7)

#define R_W GPIO_PIN_8
#define RS  GPIO_PIN_9
#define En GPIO_PIN_10

//clear display
#define clear_display	0x01
//return home
#define return_home 	0x02

//Entry_mode
#define entry_mode_on	0x01
#define entry_mode_off	0x00
#define entry_mode_inc	0x06

//Display Control
#define disp_off		0x00
#define disp_entire		0x0E
#define	disp_entire_blnk 	0x0F

//Cursor/Display Shift
#define shft_cursor_l	0x10
#define shft_cursor_r	0x14
//Function Set
#define func_set		0x28

//Set CGRAM

//Set DGRAM

//Read busy Flag and Address

//Write to Address

//Read to Address



void LCD_init(void);
void LCD_command(uint8_t command);
void LCD_write_char(uint8_t data);
void LCD_write_str( char* string );
void LCD_pulse_ENA( void );



#endif /* LCD_LCD_H_ */
