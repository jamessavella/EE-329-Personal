/*
 ******************************************************************************
 * file      : uart.c
 * project   : EE 329 S'23 A6
 * details   : UART module
 * authors   : Wilson Szeto (student) - wiszeto@calpoly.edu
 * version   : 0.1
 * date      : May 20, 2023
 * compiler  : STM32CubeIDE Version: 1.10.1 (2022)
 * target    : NUCLEO-L4A6ZG
 * citations : Szeto, Wilson. (2023-May-20). "reference code from here". Retrieved from https://github.com/wiszeto/EE329
 *  *****************************************************************************
 *  Used Pins
 *  Port G
 *  PG5 - LPUART1_ CTS
 *  PG6 - LPUART1_ RTS_DE
 *  PG7 - LPUART1_TX
 *  PG8 - LPUART1_RX
 */

#include "main.h"
#include "uart.h"


void LPUART_init() {
	PWR->CR2 |= (PWR_CR2_IOSV);             // power avail on PG[15:2] (LPUART1)
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);   // enable GPIOG clock
	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN; // enable LPUART clock bridge
	GPIOG->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7
			| GPIO_MODER_MODE8); //clear bits
	GPIOG->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1
			| GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1); //set bits AF mode
	GPIOG->AFR[0] &= ~((0x000F << GPIO_AFRL_AFSEL5_Pos)
			| (0x000F << GPIO_AFRL_AFSEL6_Pos)
			| (0x000F << GPIO_AFRL_AFSEL7_Pos));
	GPIOG->AFR[1] &= ~(0x000F << GPIO_AFRH_AFSEL8_Pos); //clear AF

	GPIOG->AFR[0] |= ((0x0008 << GPIO_AFRL_AFSEL5_Pos)
			| (0x0008 << GPIO_AFRL_AFSEL6_Pos)
			| (0x0008 << GPIO_AFRL_AFSEL7_Pos));	//set bits AF 8

	GPIOG->AFR[1] |= (0x0008 << GPIO_AFRH_AFSEL8_Pos);
	/* USER: configure GPIOG registers MODER/PUPDR/OTYPER/OSPEEDR then
	 select AF mode and specify which function with AFR[0] and AFR[1] */
	LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); // 8-bit data
	LPUART1->CR1 |= USART_CR1_UE;                   // enable LPUART1
	LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);  // enable xmit & recv
	LPUART1->CR1 |= USART_CR1_RXNEIE;        // enable LPUART1 recv interrupt
	LPUART1->ISR &= ~(USART_ISR_RXNE);       // clear Recv-Not-Empty flag
	LPUART1->BRR = 106666;
	NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));   // enable LPUART1 ISR
	__enable_irq();                          // enable global interrupts
}

void USART_init() {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // enable GPIOA clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN; // enable USART2 clock

    GPIOA->MODER &= ~(GPIO_MODER_MODE0 |GPIO_MODER_MODE1 |GPIO_MODER_MODE2 | GPIO_MODER_MODE3); //clear bits
    	GPIOA->MODER |= (GPIO_MODER_MODE0_1 |GPIO_MODER_MODE1_1 |GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);
    	GPIOA->AFR[0] &= ~((0x000F << GPIO_AFRL_AFSEL0_Pos) |(0x000F << GPIO_AFRL_AFSEL1_Pos) |(0x000F << GPIO_AFRL_AFSEL2_Pos) | (0x000F << GPIO_AFRL_AFSEL3_Pos));
    	GPIOA->AFR[0] |= ((0x0007 << GPIO_AFRL_AFSEL0_Pos) |(0x0007 << GPIO_AFRL_AFSEL1_Pos) |(0x0007 << GPIO_AFRL_AFSEL2_Pos) | (0x0007 << GPIO_AFRL_AFSEL3_Pos));


    USART2->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); // 8-bit data
    USART2->CR1 |= USART_CR1_UE;                   // enable USART2
    USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);  // enable xmit & recv
    USART2->CR1 |= USART_CR1_RXNEIE;        // enable USART2 recv interrupt
    USART2->ISR &= ~(USART_ISR_RXNE);       // clear Recv-Not-Empty flag
    USART2->BRR = 34;                   // Set the correct baud rate
    NVIC->ISER[0] = (1 << (USART2_IRQn & 0x1F));   // enable USART2 ISR
    __enable_irq();                          // enable global interrupts
}

void USART_print(char *c){
    uint8_t i;
    for (i = 0; c[i] != '\0'; i++){
        while (!(USART2->ISR & USART_ISR_TXE)){}
        USART2->TDR = c[i];
    }
}


void LPUART_Print_Char(char ch) {
	while (!(LPUART1->ISR & USART_ISR_TXE))
		;   // wait for empty transmit buffer
	LPUART1->TDR = ch;                        // send the character
}

void LPUART_Print(const char *message) {
	uint16_t iStrIdx = 0;
	while (message[iStrIdx] != '\0') {
		LPUART_Print_Char(message[iStrIdx]);
		iStrIdx++;
	}
}

void LPUART_ESC_Print(const char *message) {
	uint16_t iStrIdx = 0;
	while (message[iStrIdx] != 0) {
		while (!(LPUART1->ISR & USART_ISR_TXE))
			;   // wait for empty transmit buffer
		LPUART1->TDR = message[iStrIdx];        // send this character
		iStrIdx++;                      // advance index to next char.
	}
}
