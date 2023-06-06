#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"

// Macro definitions for row and column ports
#define control_pins GPIOE // Port for rows
#define data_pins GPIOE // Port for columns

// Macro definitions for row pins
#define RS GPIO_PIN_9 // Row 1 pin
#define RW GPIO_PIN_10 // Row 2 pin
#define EN GPIO_PIN_11 // Row 3 pin

// Macro definitions for row pins
#define D4 GPIO_PIN_12 // Row 1 pin
#define D5 GPIO_PIN_13 // Row 2 pin
#define D6 GPIO_PIN_14 // Row 3 pin
#define D7 GPIO_PIN_15 // Row 4 pin


void Nybble();
void command(uint8_t command);
void write(char letter);
void str_write(const char *str);
int LCD_convert_ascii_to_time ( uint8_t asctime );
char LCD_convert_time_to_ascii (uint8_t timeasc);
int time_in_sec( int hmin, int lmin, int hsec, int lsec );
void LCD_reset_count_display();
void LCD_update_count_display();
void load_time( int total_seconds );
int decrement_time (int time);
void lcd_set_cursor_position(uint8_t row, uint8_t col);
void LCD_init(void);
void clear_LCD(void);


#endif /* INC_LCD_H_ */
