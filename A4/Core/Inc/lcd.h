#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"

// Macro definitions for row and column ports
#define control_pins GPIOA // Port for rows
#define data_pins GPIOA // Port for columns

// Macro definitions for row pins
#define RS GPIO_PIN_1 // RS pin
#define RW GPIO_PIN_2 // RW pin
#define EN GPIO_PIN_3 // EN pin

// Macro definitions for row pins
#define D4 GPIO_PIN_4 // D4 pin
#define D5 GPIO_PIN_5 // D5 pin
#define D6 GPIO_PIN_6 // D6 pin
#define D7 GPIO_PIN_7 // D7 pin

// Function Declarations
void LCD_init(void);
void str_write(const char *str);
int LCD_convert_ascii_to_time(uint8_t asctime);
char LCD_convert_time_to_ascii(uint8_t timeasc);
int time_in_sec(int hmin, int lmin, int hsec, int lsec);
void LCD_reset_count_display();
void LCD_update_count_display();
void load_time(int total_seconds);
int decrement_time(int time);
void lcd_set_cursor_position(uint8_t row, uint8_t col);
void write(char letter);

#endif /* INC_LCD_H_ */

