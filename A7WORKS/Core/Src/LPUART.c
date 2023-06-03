/*
 * LPUART.c
 *
 *  Created on: May 25, 2023
 *      Author: james
 *
 *      NOTE: MODIFIED update_position to not have sprintf
 */

#include "main.h"
#include "LPUART.h"
#include "delay.h"

//Not needed for A7
//cursor position center
//int row = 20;
//int col = 40;

void LPUART_init(void) {
	PWR->CR2 |= (PWR_CR2_IOSV);             // power avail on PG[15:2] (LPUART1)
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);   // enable GPIOG clock
	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN; // enable LPUART clock bridge

	//Initialize Pins for USART
	// Pin 7 = tx, pin 8 = rx,
	GPIOC->MODER &= ~(GPIO_MODER_MODE10 | GPIO_MODER_MODE11);	//clear bits
	GPIOC->MODER |= (GPIO_MODER_MODE10_1 | GPIO_MODER_MODE11_1);//set to AFR mode
	GPIOC->AFR[1] |= (8 << GPIO_AFRL_AFSEL7_Pos);
	GPIOC->AFR[1] |= (8 << GPIO_AFRH_AFSEL8_Pos); //set to AF8 (USART)

	LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); // 8-bit data
	LPUART1->CR1 |= USART_CR1_UE;                   // enable LPUART1
	LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);  // enable xmit & recv
//	LPUART1->CR1 |= USART_CR1_RXNEIE;        // enable LPUART1 recv interrupt
//	LPUART1->ISR &= ~(USART_ISR_RXNE);       // clear Recv-Not-Empty flag

	LPUART1->BRR = 53333;			//sets baud rate~VT100 11500

//	NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));   // enable LPUART1 ISR
//	__enable_irq();                          // enable global interrupts
}

void LPUART_Print(const char *message) {
	uint16_t iStrIdx = 0;
	while (message[iStrIdx] != 0) {
		while (!(LPUART1->ISR & USART_ISR_TXE))
			; // wait for empty xmit buffer
		LPUART1->TDR = message[iStrIdx];       // send this character
		iStrIdx++;                             // index to next char
		delay_us(200);
	}
	delay_us(1000);
}

void LPUART_ESC_Print(const char *message) {
	uint16_t iStrIdx = 0;
	while (!(LPUART1->ISR & USART_ISR_TXE))
		;
	LPUART1->TDR = 0x1b;	//initial escape code
	while (message[iStrIdx] != 0) {
		while (!(LPUART1->ISR & USART_ISR_TXE))
			; // wait for empty xmit buffer
		LPUART1->TDR = message[iStrIdx];       // send this character
		iStrIdx++;
		delay_us(200);       // advance index to next char
	}
	delay_us(1000);
}

//works only for row and col values less than 100
void update_position(uint8_t row, uint8_t col) {
	char str[8];
	int i = 0; //pointer to increment if row and col values are greater than 10
	int rone, rten, cone, cten;
	rone = row % 10;
	rten = row / 10;
	cone = col % 10;
	cten = col / 10;

	//converts row and col int values to char
	char row_one = rone + '0';
	char row_ten = rten + '0';
	char col_one = cone + '0';
	char col_ten = cten + '0';

	//Writes position value
	str[0] = '[';
	if (row > 9){
		i++;
		str[1] = row_ten;
	}
	str[1+i] = row_one;
	str[2+i] = ';';
	if (col > 9){
		str[3+i] = col_ten;
		i++;
	}
	str[3+i] = col_one;
	str[4+i] = 'H';
	LPUART_ESC_Print(str);
}

//Below function not in scope of A7
//80 Column, 40 Rows
//void splash_screen(void) {
//
//	__disable_irq();			//disables input
//
//	LPUART_ESC_Print("[11;30H"); //center
//	//LPUART_ESC_Print("[5m");	//blinking mode (sometimes it blinks)
//	LPUART_Print("Penvenne's Plaything");		//Blink Penvene's plaything
//
//	//Heart
//	LPUART_ESC_Print("[18;40H");	//line:column
//	LPUART_Print("*");
//	LPUART_ESC_Print("[17;39H");
//	LPUART_Print("* *");
//	LPUART_ESC_Print("[16;38H");
//	LPUART_Print("*   *");
//	LPUART_ESC_Print("[15;37H");
//	LPUART_Print("*     *");
//	LPUART_ESC_Print("[14;36H");
//	LPUART_Print("*   *   *");
//	LPUART_ESC_Print("[13;36H");
//	LPUART_Print("* *   * *");
//
//	explodeFirework(20, 10);
//	explodeFirework(60, 10);
//
//	__enable_irq();                          // enable global interrupts
//}

//int randomInt(int min, int max) {
//    return min + rand() % (max - min + 1);
//}

//// inspiration taken from ChatGPT but heavily modified
//void explodeFirework(int x, int y) {
//	int i, j, color;
//	char str_pos[8], str_color[5];
//	for(i=12; i>0; i--){
//		LPUART_ESC_Print("[33m"); // yellow
//		sprintf(str_pos, "[%d;%dH", y + i, x);
//		LPUART_ESC_Print(str_pos);
//
//		LPUART_Print("*");
//
//		delay_us(150000);
//	}
//
//    for (i = 0; i < 11; i++) {
//        for (j = 0; j <= i; j++) {
//            double angle = 2 * M_PI * j / i;
//            int dx = round(i * cos(angle));
//            int dy = round(i * sin(angle));
//
//            color = randomInt(0, 7);    // pick random color
//            sprintf(str_pos, "[%d;%dH", y + dy/2, x + dx); // randomish positon
//            LPUART_ESC_Print(str_pos);
//
//            sprintf(str_color, "[3%dm", color);
//            LPUART_ESC_Print(str_color);  // Set random color
//
//            LPUART_Print("*");
//
//
//            delay_us(40000);  // Sleep for 80 milliseconds
//        }
//    }
//}

void LPUART1_IRQHandler(void) {
	uint8_t charRecv;
	if (LPUART1->ISR & USART_ISR_RXNE) {
		charRecv = LPUART1->RDR;
		switch (charRecv) {
		case 'R':
			LPUART_ESC_Print("[31m"); //change cursor red
			break;
		case 'G':
			LPUART_ESC_Print("[32m"); //change cursor red
			break;
		case 'B':
			LPUART_ESC_Print("[34m"); //change cursor red
			break;
		case 'W':
			LPUART_ESC_Print("[37m"); //change cursor red
			break;

		case 'w':	//up
			row -= 1;
			if (row < 1) row = 40;
			break;
		case 's'://down
			row += 1;
			if (row > 40) row = 1;
			break;
		case 'a'://left
			col -= 1;
			if (col < 1) col = 80;
			break;
		case 'd'://right
			col += 1;
			if (col > 80) col = 1;
			break;
		default:
			while (!(LPUART1->ISR & USART_ISR_TXE))
				;    // wait for empty TX buffer
			LPUART1->TDR = charRecv;  // echo char to terminal
		}  // end switch
	}
	//update_position();
}
