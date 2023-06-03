/*
 * LCD.h
 *
 *  Created on: Apr 17, 2023
 *      Author: Zack's PC
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_
#include "main.h"

#define LCD_RS        0b0000000000000001;

void LCD_config(void);
void LCD_init(void);
void LCD_Pulse_Enable(void);
void LCD_4b_Command(uint8_t bits);
void LCD_Command(uint8_t Byte);
void LCD_write_char(uint8_t letter);
void LCD_write_string(char *string);
void Display_Init(void);
void LCD_write_duty_cycle(int duty);
void LCD_write_freq(int freq);
//void LCD_Countdown_1s(uint16_t *count_array);
//void LCD_Countdown_10s(uint16_t *count_array);
//void LCD_Countdown_1m(uint16_t *count_array);
//void LCD_Countdown_10m(uint16_t *count_array);

#endif /* INC_LCD_H_ */
