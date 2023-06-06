/*
 ******************************************************************************
 * file      : lcd.c
 * project   : EE 329 S'23 A3
 * details   : LCD module
 * authors   : Wilson Szeto (student) - wiszeto@calpoly.edu
 * version   : 0.1
 * date      : April 23, 2023
 * compiler  : STM32CubeIDE Version: 1.10.1 (2022)
 * target    : NUCLEO-L4A6ZG
 * citations : Szeto, Wilson. (2023-Apr-23). "reference code from here". Retrieved from https://github.com/wiszeto/EE329
 ******************************************************************************
 * wiring    :
 * PC0 - Single LED
 * PA1  - RS
 * PA2  - RW
 * PA3  - E
 * PA4  - D4
 * PA5  - D5
 * PA6  - D6
 * PA7  - D7
 ******************************************************************************
 */

//includes
#include "delay.h"
#include "lcd.h"

//prototypes
//void LCD_init(void);
//void Nybble();
//void command(uint8_t command);
//void write(char letter);
//void lcd_set_cursor_position(uint8_t row, uint8_t col);
//int LCD_convert_ascii_to_time ( uint8_t asctime );
//void clear_LCD(void);

//data bus array
uint16_t GPIO_Pin[] = {D4, D5, D6, D7};

void LCD_init(void) {
  delay_us(100000);
  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOEEN); // enable GPIOA clock on bus

  // Making control pins output mode bits(01)
  control_pins->MODER &=
      ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE10 | GPIO_MODER_MODE11 |
        GPIO_MODER_MODE12 | GPIO_MODER_MODE13 | GPIO_MODER_MODE14 |
        GPIO_MODER_MODE15); // clear bits for PA
  control_pins->MODER |=
      (GPIO_MODER_MODE9_0 | GPIO_MODER_MODE10_0 | GPIO_MODER_MODE11_0 |
       GPIO_MODER_MODE12_0 | GPIO_MODER_MODE13_0 | GPIO_MODER_MODE14_0 |
       GPIO_MODER_MODE15_0); // set up DB4 to DB7, RS, RW, E as outputs
  control_pins->OTYPER &=
      ~(GPIO_OTYPER_OT9 | GPIO_OTYPER_OT10 | GPIO_OTYPER_OT11 | GPIO_OTYPER_OT12 |
        GPIO_OTYPER_OT13 | GPIO_OTYPER_OT14 | GPIO_OTYPER_OT15);
  control_pins->OSPEEDR |=
      ((3 << GPIO_OSPEEDR_OSPEED9_Pos) | (3 << GPIO_OSPEEDR_OSPEED10_Pos) |
       (3 << GPIO_OSPEEDR_OSPEED11_Pos) | (3 << GPIO_OSPEEDR_OSPEED12_Pos) |
       (3 << GPIO_OSPEEDR_OSPEED13_Pos) | (3 << GPIO_OSPEEDR_OSPEED14_Pos) |
       (3 << GPIO_OSPEEDR_OSPEED15_Pos));
  control_pins->BRR =
      (D4 | D5 | D6 | D7 | EN | RW | RS); // Initializes data to all 0

  //LCD init commands based on the LCD datasheet
  delay_us(100000);
  command(0x30);
  delay_us(100);
  command(0x30);
  delay_us(100);
  command(0x30);
  delay_us(100);
  command(0x20);
  delay_us(100);
  command(0x02);
  delay_us(100);
}

void Nybble() {
  control_pins->BSRR = EN;  //turn on EN for pulse
  delay_us(50); // wait
  control_pins->BRR = EN;   //turn off EN for pulse
  delay_us(50);                                // wait
  control_pins->ODR &= ~((D4 | D5 | D6 | D7)); // clear data registers
}

void command(uint8_t command) {
  uint8_t command2 = command >> 4; //shift higher nibbles to lower nibbles
  control_pins->BRR = (RS | RW); // set RS and RW to zero

  for (int i = 0; i < 4; i++) {  //loop through data bus and turn on or off
    if ((command2 >> i) & 0x01) {
      control_pins->BSRR = GPIO_Pin[i];
    } else {
      control_pins->BRR = GPIO_Pin[i];
    }
  }

  delay_us(1000);
  Nybble(); // Send pulse

  for (int i = 0; i < 4; i++) {
    if ((command >> i) & 0x01) { //loop through data bus and turn on or off
      control_pins->BSRR = GPIO_Pin[i];
    } else {
      control_pins->BRR = GPIO_Pin[i];
    }
  }
  delay_us(1000);
  Nybble();                                    // Send pulse
  control_pins->ODR &= ~((D4 | D5 | D6 | D7)); // Clears registers
}

void write(char letter) {  //write 1 character on the LCD display
  uint8_t c = letter;
  uint8_t c2 = c >> 4;
  control_pins->BSRR = (RS); // set RS on
  control_pins->BRR = (RW);  // set RW off
  for (int i = 0; i < 4; i++) { //loop through data bus and turn on or off
    if ((c2 >> i) & 0x01) {
      control_pins->BSRR = GPIO_Pin[i];
    } else {
      control_pins->BRR = GPIO_Pin[i];
    }
  }

  delay_us(1000);
  Nybble(); //send pulse

  for (int i = 0; i < 4; i++) { //loop through data bus and turn on or off
    if ((c >> i) & 0x01) {
      control_pins->BSRR = GPIO_Pin[i];
    } else {
      control_pins->BRR = GPIO_Pin[i];
    }
  }
  delay_us(1000);
  Nybble(); //send pulse
  control_pins->BRR = (RS);                    // set RS back to zero
  control_pins->ODR &= ~((D4 | D5 | D6 | D7)); // Clears registers
  delay_us(100);
}

void lcd_set_cursor_position(uint8_t row, uint8_t col) {
  uint8_t address;

  // Calculate the address based on row and col values
  if (row == 0) {
    address = 0x80 + col; // For row 0, the address starts from 0x80
  } else {
    address = 0xC0 + col; // For row 1, the address starts from 0xC0
  }

  // Send the command to set the cursor position
  command(address);
}

void str_write(const char *str) { //writes a character array to LCD
  for (int i = 0; str[i] != '\0'; i++) {
    write(str[i]);
    delay_us(100);
  }
	delay_us(100);
}

// James imported functions for delay

char time_asc[4];

////Resets counter display to 00:00
void LCD_reset_count_display() {
  lcd_set_cursor_position(1, 11); // set cursor at MM:SS
  str_write("     ");             // clears with 5 spaces
  lcd_set_cursor_position(1, 11); // set cursor at MM:SS
  str_write("00:00");
  lcd_set_cursor_position(1, 11); // set cursor at MM:SS
  command(0x0D);                  // turns blinking cursor on
}

// This is only for countdown
// Updates LCD with new values
void LCD_update_count_display() {
  lcd_set_cursor_position(1, 11); // set cursor at MM:SS
  write(time_asc[3]);
  write(time_asc[2]);
  write(':');
  write(time_asc[1]);
  write(time_asc[0]);
}

// Converts total time (s) to ASCII equivalent values and loads them onto array
// for uploading time_in_sec FOR PARAMETER
void load_time(int total_seconds) {
  uint8_t u_min, l_min, u_sec, l_sec; // initializes values
  u_min = total_seconds / 60;         // gets all the minutes
  l_min = u_min % 10;                 // gets ones minutes
  u_min = u_min / 10;                 // gets tens minutes

  u_sec = total_seconds % 60; // gets all the secs
  l_sec = u_sec % 10;         // gets ones secs
  u_sec = u_sec / 10;         // gets tens sec

  // Updates array
  time_asc[3] = LCD_convert_time_to_ascii(u_min);
  time_asc[2] = LCD_convert_time_to_ascii(l_min);
  time_asc[1] = LCD_convert_time_to_ascii(u_sec);
  time_asc[0] = LCD_convert_time_to_ascii(l_sec);
}


// LCD_convert_ascii_to_time FOR PARAMETERS
int time_in_sec(int hmin, int lmin, int hsec, int lsec) {
  int total_time = 0;      // to store total time
  hmin = hmin * 10 + lmin; // adds total minutes together
  hsec = hsec * 10 + lsec; // adds total seconds together
  total_time = (hmin * 60 + hsec);
  return total_time; // returns total time
}

int decrement_time(int time) {
  delay_us(1000000); // delay 1 sec
  return (--time);
}

int LCD_convert_ascii_to_time ( uint8_t asctime ){
	return (0x0F & asctime);
}

void clear_LCD(void) {
	lcd_set_cursor_position(0, 0);
	str_write("                ");
	delay_us(100);
	lcd_set_cursor_position(1, 0);
	str_write("                ");
	delay_us(100);
}
