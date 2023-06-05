#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"

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



void str_write(const char *str);
int LCD_convert_ascii_to_time ( uint8_t asctime );
char LCD_convert_time_to_ascii (uint8_t timeasc);
int time_in_sec( int hmin, int lmin, int hsec, int lsec );
void LCD_reset_count_display();
void LCD_update_count_display();
void load_time( int total_seconds );
int decrement_time (int time);



#endif /* INC_LCD_H_ */
