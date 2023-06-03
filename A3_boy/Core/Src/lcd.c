#include "main.h"
#include "lcd.h"
#include "delay.h"
#include "stm32l4a6xx.h"
#include "stm32l4xx_hal.h"

void LCD_init(void);
void Nybble();
void command(uint8_t command);
void write(char letter);
void lcd_set_cursor_position(uint8_t row, uint8_t col);

uint16_t GPIO_Pin[] = { D4, D5, D6, D7 };

void LCD_init(void) {
	delay_us(100000); // power-up wait 40 ms
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN); // enable GPIOA clock on bus

//Making control pins output mode bits(01)
	control_pins->MODER &= ~(GPIO_MODER_MODE1 | GPIO_MODER_MODE2
			| GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5
			| GPIO_MODER_MODE6 | GPIO_MODER_MODE7); // clear bits for PA
	control_pins->MODER |= (GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0
			| GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0
			| GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0); // set up DB4 to DB7, RS, RW, E as outputs
	control_pins->OTYPER &= ~(GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2
			| GPIO_OTYPER_OT3 | GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5
			| GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);
	control_pins->OSPEEDR |=
			((3 << GPIO_OSPEEDR_OSPEED1_Pos) | (3 << GPIO_OSPEEDR_OSPEED2_Pos)
					| (3 << GPIO_OSPEEDR_OSPEED3_Pos)
					| (3 << GPIO_OSPEEDR_OSPEED4_Pos)
					| (3 << GPIO_OSPEEDR_OSPEED5_Pos)
					| (3 << GPIO_OSPEEDR_OSPEED6_Pos)
					| (3 << GPIO_OSPEEDR_OSPEED7_Pos));
	control_pins->BRR = ( D4 | D5 | D6 | D7 | EN | RW | RS); //Initializes data to all 0

	delay_us(100000); // power-up wait 40 ms

	command(0x30);
	delay_us(100); // power-up wait .04 ms
	command(0x30);
	delay_us(100); // power-up wait .04 ms
	command(0x30);
	delay_us(100); // power-up wait .04 ms
	command(0x20);
	delay_us(100); // power-up wait .04 ms
	command(0x02);
	delay_us(100); // power-up wait .04 ms
}

void Nybble() {
	control_pins->BSRR = EN;
	delay_us(50); // wait
	control_pins->BRR = EN;
	delay_us(50); // wait
	control_pins->ODR &= ~((D4 | D5 | D6 | D7)); // Clears registers
}

void command(uint8_t command) {
	uint8_t command2 = command >> 4;
	control_pins->BRR = (RS | RW); // set RS and RW to zero

	for (int i = 0; i < 4; i++) {
		if ((command >> i) & 0x01) {
			control_pins->BSRR = GPIO_Pin[i];
		} else {
			control_pins->BRR = GPIO_Pin[i];
		}
	}

	delay_us(1000);
	Nybble(); //Send pulse

	for (int i = 0; i < 4; i++) {
		if ((command2 >> i) & 0x01) {
			control_pins->BSRR = GPIO_Pin[i];
		} else {
			control_pins->BRR = GPIO_Pin[i];
		}
	}
	delay_us(1000);
	Nybble(); //Send pulse
	control_pins->ODR &= ~((D4 | D5 | D6 | D7)); // Clears registers
}

void write(char letter) {
	uint8_t c = letter;
	uint8_t c2 = c >> 4;
	for (int i = 0; i < 4; i++) {
		if ((c2 >> i) & 0x01) {
			control_pins->BSRR = GPIO_Pin[i];
		} else {
			control_pins->BRR = GPIO_Pin[i];
		}
	}

	control_pins->BSRR = (RS); // set RS and RW to zero
	control_pins->BRR = (RW); // set RS and RW to zero
	delay_us(1000);
	Nybble();
	//c = c >> 4;

	for (int i = 0; i < 4; i++) {
		if ((c >> i) & 0x01) {
			control_pins->BSRR = GPIO_Pin[i];
		} else {
			control_pins->BRR = GPIO_Pin[i];
		}
	}
	delay_us(1000);
	Nybble();
	control_pins->BRR = (RS); // set RS and RW to zero
	control_pins->ODR &= ~((D4 | D5 | D6 | D7)); // Clears registers
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

