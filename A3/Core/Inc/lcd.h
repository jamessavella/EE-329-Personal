/*
 * LCD.h
 *
 *  Created on: Apr 18, 2023
 *      Author: Wilso
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_



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




//JAMES NEW FUCKING SHITTY ASS VARIABLES FOR SHITTTTTTTTT
char up_min;		//Donna's shitty array shit start
char lo_min;
char up_sec;
char lo_sec;		//Donna's shitt arry shit stop shit

//To load ASCII number to timer
char time_asc[4];  	//time_asc[3] = up_min
					//time_asc[2] = lo_min
					//time_asc[1] = up_sec
					//time_asc[0] = lo_sec
					//reference MM:SS = [0][0]:[0][0] = [up_min][lo_min]:[up_sec][lo_sec]







#endif /* INC_LCD_H_ */


