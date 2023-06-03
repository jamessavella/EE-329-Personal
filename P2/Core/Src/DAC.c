/*
***************************************************************************************
 * file      : DAC.c
 * project   : EE 329 S'23 A5
 * details   : display some sample inputs to an LCD display
 * authors   : Zachary Numa - znuma@calpoly.edu
 * 			   ames Savella - jsavella@calpoly.edu
 *
 * Credit	 : Wilson Szeto	EE 329 S'23 student (2023-May-4) Personal Communication: helped debug the DAC_write function
 *
 * version   : 1
 * date      : 5/5/2023
 * compiler  : STM32CubeIDE Version: 1.12.0
 * target    : NUCLEO-L4A6ZG
 ***************************************************************************************
* wiring    : LCD
 * LCD 1   VDD - GND
 * LCD 2   VSS - +3.3V
 * LCD 3   NC
 *
 * GPIO Pin Connections:
 * LCD 4  RS  - PE9
 * LCD 5  RW  - PE8
 * LCD 6  E   - PE10
 * LCD 7  D7  - PE7
 * LCD 8  D6  - PE6
 * LCD 9  D5  - PE5
 * LCD 10 D4  - PE4
 *
 * Anode (A) and Cathode (K) Connections:
 * LCD 15 A   - +3.3V
 * LCD 16 K   - GND
 * -------------------------------------------------------------------------------------
 * wiring    : 4x3 hard keypad	(4-bit mode)
 * PD0 - ROW 0 - ROW 0 INPUT - keypad pin 1
 * PD1 - ROW 1 - ROW 1 INPUT - keypad pin 2
 * PD2 - ROW 2 - ROW 2 INPUT - keypad pin 3
 * PD3 - ROW 3 - ROW 3 INPUT - keypad pin 4
 * PD4 - COL 0 - COL 0 OUTPUT - keypad pin 5
 * PD5 - COL 1 - COL 1 OUTPUT - keypad pin 6
 * PD6 - COL 2 - COL 2 OUTPUT - keypad pin 7
 * 1 on the keypad represents row 0 column 0
 * Keypad pin order:
 * ROW 0 - 3, COL 0 - 3
 * -------------------------------------------------------------------------------------
 * wiring    :  DAC
 * VDD - +3.3V
 * VSS - GND
 * SHDN - GND
 * LDAC - GND
 *
 * GPIO Pin Connections:
 * CS - PA4
 * SCK - PA5
 * SDI - PA7
 *
 ***************************************************************************************
 */


#include <DAC.h>
#include "main.h"

void DAC_init(void) {
	//Enabling DAC PINS
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;	   // enable SPI Clock
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN); // enable GPIOA clock on bus

	//AFR
	DAC_PIN->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE7); // Clear Bits
	DAC_PIN->MODER |=(GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1 | GPIO_MODER_MODE7_1);
	DAC_PIN->AFR[0] |= ((0x0005 << GPIO_AFRL_AFSEL4_Pos) | 		// all set to AF5
			(0x0005 << GPIO_AFRL_AFSEL5_Pos) | (0x0005 << GPIO_AFRL_AFSEL7_Pos));

	//Resistor and Speed Stuff
	DAC_PIN->OTYPER &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT7); //Resistor stuff
	DAC_PIN->OSPEEDR |=
			((3 << GPIO_OSPEEDR_OSPEED4_Pos) | (3 << GPIO_OSPEEDR_OSPEED5_Pos)
					| (3 << GPIO_OSPEEDR_OSPEED7_Pos)); 		//Speed Stuff

	SPI_init();

}

void SPI_init(void) {
	// SPI config as specified @ STM32L4 RM0351 rev.9 p.1459
	// called by or with DAC_init()
	// build control registers CR1 & CR2 for SPI control of peripheral DAC
	// assumes no active SPI xmits & no recv data in process (BSY=0)
	// CR1 (reset value = 0x0000)
	SPI1->CR1 &= ~( SPI_CR1_SPE);             	// disable SPI for config
	SPI1->CR1 &= ~( SPI_CR1_RXONLY);          	// recv-only OFF
	SPI1->CR1 &= ~( SPI_CR1_LSBFIRST);        	// data bit order MSb:LSb
	SPI1->CR1 &= ~( SPI_CR1_CPOL | SPI_CR1_CPHA); // SCLK polarity:phase = 0:0
	SPI1->CR1 |= SPI_CR1_MSTR;              	// MCU is SPI controller
	// CR2 (reset value = 0x0700 : 8b data)
	SPI1->CR2 &= ~( SPI_CR2_TXEIE | SPI_CR2_RXNEIE); // disable FIFO intrpts
	SPI1->CR2 &= ~( SPI_CR2_FRF);              	// Moto frame format
	SPI1->CR2 |= SPI_CR2_NSSP;              	// auto-generate NSS pulse
	SPI1->CR2 |= SPI_CR2_DS;                	// 16-bit data
	SPI1->CR2 |= SPI_CR2_SSOE;              	// enable SS output
	SPI1->CR1 |= SPI_CR1_SSM;					// enable SSM
	SPI1->CR1 |= SPI_CR1_SSI;					// enable SSI
	SPI1->CR1 |= SPI_CR1_SPE;               	// re-enable SPI for ops

}


int DAC_volt_conv(int convert) {
	//convert = (convert * 4095* 0.805) / 3300;
	//the Calibration is better for us if we dont do anything to it
	return convert;
}


//Write the data
void DAC_write(int word) {
	SPI1->CR1 &= ~SPI_CR1_SSI; //clear SSI
	while (!(SPI1->SR & SPI_SR_TXE));	//Polling to ensure buffer is empty
		word = word + 0x1000;	//GA = 0 2 times multiplier
		SPI1->DR = word;		//writing to DAC data register
	while(!(SPI1->SR & SPI_SR_BSY));	//Polling to ensure transmit is over
	SPI1->CR1 |= SPI_CR1_SSI; //enable SSI
}

