#include "main.h"
#include "uart.h"
//#include "lcd.h"
#include "delay.h"
#include "fingerprint.h"


void SystemClock_Config(void);
#define BUFFER_SIZE 256
static char buffer[BUFFER_SIZE];
static int index = 0;
int flag = 0;

int main(void) {

	HAL_Init();
	SystemClock_Config();
	//LPUART_init();
	//LCD_init();
	//command(0x0C);

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;  // GPIOC clock init
	GPIOC->MODER &= ~(GPIO_MODER_MODE13); // button init
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13_1);
	GPIOC->PUPDR |= (GPIO_PUPDR_PUPD13_1);
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;  // GPIOB clock init
	GPIOB->MODER &= ~(GPIO_MODER_MODE7);  // LD2 clear
	GPIOB->MODER |= (GPIO_MODER_MODE7_0); // LD2 output mode

	delay_us(100);
	USART_init();

	//Fingerprint initialization
	handshake();
	delay_us(1000000);
	SetSysPara(4, 6);
	delay_us(1000000);
	SetSysPara(5, 3);
	delay_us(1000000);
	SetSysPara(6, 0);





	while (1) {



		//=--------------WILLIE POGGIE ATTRIBUTION
//		USART_print("Hello");
//		if (flag) {
//			delay_us(10000);
//			flag = 0;
//			str_write("Welcome ");
//			str_write(buffer);  // process the string
//
//			lcd_set_cursor_position(1, 0); // set cursor to second row, first column
//			str_write("Put fngr on sens");
//			lcd_set_cursor_position(0, 0); // set cursor to second row, first column
//		}
//		if (GPIOC->IDR & GPIO_IDR_ID13) { // Check if the button is pressed
//			GPIOB->BSRR = GPIO_PIN_7;
//			delay_us(10000);
//			while (!(LPUART1->ISR & USART_ISR_TXE));
//			LPUART_Print("WILSON\n");
//			delay_us(1000000);
//		}
//		GPIOB->BRR = GPIO_PIN_7;

	}
}

void LPUART1_IRQHandler(void) {
	if (LPUART1->ISR & USART_ISR_RXNE) { // check if there is new data in the UART receiver
		GPIOB->BSRR = GPIO_PIN_7;

		char charRecv = LPUART1->RDR;  // read the received character
		buffer[index] = charRecv;
		index++;

		if (charRecv == '\n') {
			flag = 1;
			index--;
			buffer[index] = '\0';
			index = 0;
		}
		while (!(LPUART1->ISR & USART_ISR_TXE))
			;  // wait for empty TX buffer
		LPUART1->TDR = charRecv; // send received character

		GPIOB->BRR = GPIO_PIN_7;
	}
}

void USART2_IRQHandler(void){

  if (USART2->ISR & USART_ISR_RXNE){
    char character = USART2->RDR;
    switch (character){
        default:
            while (!(USART2->ISR & USART_ISR_TXE)){}
            USART2->TDR = character;
            break;
    }
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {
  }

}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif