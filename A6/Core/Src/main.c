#include "main.h"
#include "delay.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void SystemClock_Config(void);
void LPUAR_init(void);
void LPUART_Print(const char *message);
void LPUART1_IRQHandler(void);
void LPUART_ESC_Print(const char *message);
void splash_screen(void);
void update_position(void);
int randomInt(int min, int max);
void explodeFirework(int x, int y);
//void rand();

//cursor position center
int row = 20;
int col = 40;

int main(void) {

	HAL_Init();
	SystemClock_Config();
	SysTick_Init();
	LPUAR_init();
	LPUART_ESC_Print("[2J");	//clear all
	LPUART_ESC_Print("[37m");	//makes text white

	LPUART_ESC_Print("[3B");	//3 down
	LPUART_ESC_Print("[5C");	//5 right
	LPUART_Print("All good students read the");
	LPUART_ESC_Print("[1B");	// 1 down
	LPUART_ESC_Print("[21D");	//21 left
	LPUART_ESC_Print("[5m");	//blinking mode
	LPUART_Print("Reference Manual");
	LPUART_ESC_Print("[1;0H");	//put cursor top left
	LPUART_ESC_Print("[0m");	//remove character attributes
	LPUART_Print("Input:");

//	LPUART_ESC_Print("[2J");
//	//splash_screen();
//	delay_us(2000000);
//	LPUART_ESC_Print("[0m");    // remove attributes
//	update_position();
	while (1) {
	}
}

void LPUAR_init(void) {
	PWR->CR2 |= (PWR_CR2_IOSV);             // power avail on PG[15:2] (LPUART1)
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);   // enable GPIOG clock
	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN; // enable LPUART clock bridge

	//Initialize Pins for USART
	// Pin 7 = tx, pin 8 = rx,
	GPIOG->MODER &= ~(GPIO_MODER_MODE7 | GPIO_MODER_MODE8);	//clear bits
	GPIOG->MODER |= (GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1);//set to AFR mode
	GPIOG->AFR[0] |= (8 << GPIO_AFRL_AFSEL7_Pos);
	GPIOG->AFR[1] |= (8 << GPIO_AFRH_AFSEL8_Pos); //set to AF8 (USART)

	LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); // 8-bit data
	LPUART1->CR1 |= USART_CR1_UE;                   // enable LPUART1
	LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);  // enable xmit & recv
	LPUART1->CR1 |= USART_CR1_RXNEIE;        // enable LPUART1 recv interrupt
	LPUART1->ISR &= ~(USART_ISR_RXNE);       // clear Recv-Not-Empty flag

	LPUART1->BRR = 106666;			//sets baud rate

	NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));   // enable LPUART1 ISR
	__enable_irq();                          // enable global interrupts
}

void LPUART_Print(const char *message) {
	uint16_t iStrIdx = 0;
	while (message[iStrIdx] != 0) {
		while (!(LPUART1->ISR & USART_ISR_TXE)); // wait for empty xmit buffer
		LPUART1->TDR = message[iStrIdx];       // send this character
		iStrIdx++;                             // advaasZSAWEQnce index to next char
		delay_us(200);
	}
	delay_us(1000);
}

void LPUART_ESC_Print(const char *message) {
	uint16_t iStrIdx = 0;
	while (!(LPUART1->ISR & USART_ISR_TXE));
	LPUART1->TDR = 0x1b;
	while (message[iStrIdx] != 0) {
		while (!(LPUART1->ISR & USART_ISR_TXE)); // wait for empty xmit buffer
		LPUART1->TDR = message[iStrIdx];       // send this character
		iStrIdx++;
		delay_us(200);       // advance index to next char
	}
	delay_us(1000);
}

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

int randomInt(int min, int max) {
    return min + rand() % (max - min + 1);
}

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


//NEED TO UPDATE
//void update_position(){
//	LPUART_ESC_Print("[2J");	//clear all
//	LPUART_ESC_Print("[25l");	//supposed to turn off cursor but doesn't
//
//	char str[8];
//	sprintf(str, "[%d;%dH",  row, col);
//	LPUART_ESC_Print(str);
//	LPUART_Print("X");
//
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

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

void Error_Handler(void) {

	__disable_irq();
	while (1) {
	}

}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif





