#include "main.h"
#include "lcd.h"
#include "delay.h"

void SystemClock_Config(void);

int main(void) {
	HAL_Init();
	SystemClock_Config();
	SysTick_Init();
	LCD_init();

	int reset_state = 0;	//To display OG display
	int random_state = 0;  	//To do number
	int react_state = 0;	//To start react
	int display_state = 0;	//To display

	//To display timer stuff
	int time_s;
	int time_up_ms;
	int time_md_ms;
	int time_lo_ms;

	int random_number;		//Initialize Random number
	int start;				//Initialize start time
	int end;				//Initialize stop time

	int dd = 10000000; 		//delay for the thing

	//Wilson Szeto initialization
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;	//GPIOC clock init
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;	//GPIOB clock init
	RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;	//RNG clock init, XX
	RCC->CRRCR |= (RCC_CRRCR_HSI48ON);	//HSI clock init
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;	//TIM2 init

	TIM2->ARR = 0xFFFFFFFF;  // Max auto-reload value for 32-bit timer
	TIM2->CR1 |= TIM_CR1_CEN;  // Start TIM2

	GPIOB->MODER &= ~(GPIO_MODER_MODE7);  //LD2 clear
	GPIOB->MODER |= (GPIO_MODER_MODE7_0); //LD2 output mode on-board LED

	GPIOC->MODER &= ~(GPIO_MODER_MODE13);	//button init on-board button
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13_1);
	GPIOC->PUPDR |= (GPIO_PUPDR_PUPD13_1);

	RNG->CR |= (RNG_CR_RNGEN | RNG_CR_IE); //RNG turn on interrupt

	delay_us(100);
	str_write("EE 329 A4 REACT "); // write EE 329 A3 TIMER to first line
	delay_us(100);
	lcd_set_cursor_position(1, 0); // set cursor to second row, first column
	delay_us(100);
	str_write("PUSH SW TO TRIG "); // write *=SET #=GO --:-- to second line
	while (1) {

		//--------------RESET STATE-----------------
		if ((reset_state | react_state | random_state | display_state) == 0) {
			delay_us(100);
			lcd_set_cursor_position(1, 0); //set cursor to second row, first column
			delay_us(100);
			str_write("PUSH SW TO TRIG "); 	// display
			if (GPIOC->IDR & GPIO_IDR_ID13) {
				reset_state = 1; //detects button press and gets out of reset_state
				delay_us(dd);
			}
		}

		//--------------START GENERATING RANDOM NUMBER TO TURN ON LED------------
		if (reset_state == 1) {
			delay_us(100);
			lcd_set_cursor_position(1, 0); // set cursor to second row, first column
			delay_us(100);
			str_write("TIME = -.--- ms "); 	// Display empty timer
			while (!(RNG->SR & RNG_SR_DRDY))
				; 	//polling to get random number
			random_number = RNG->DR; 	  	//loads random number
			delay_us(random_number & 0xFF);
			react_state = 1;
			reset_state = 0;
			start = TIM2->CNT;					//loads initial time count
		}

		//---------------REACT STATE
		if (react_state == 1) {
			//this line was moved to 81

			GPIOB->BSRR = GPIO_PIN_7;			//turn on led
			if (GPIOC->IDR & GPIO_IDR_ID13) {	//detects button push
				GPIOB->BRR = GPIO_PIN_7;		//turn off led
				end = TIM2->CNT;				//loads end of time count
				end = end - start; 				//gets the time difference
				end = end / 4000;				// covert to mmsec
				react_state = 0;
				display_state = 1;				//displays
				delay_us(dd);
			}
		}

		//------------------Display State
		if (display_state == 1) {				//If user input is too slow
			if (end > 10000) {					//If timer is greater than 10 s
				delay_us(100);
				lcd_set_cursor_position(1, 0); // set cursor to second row, first column
				delay_us(100);
				str_write("TIME = 9.999 ms"); 	// Display empty timer
			} else {								//User input goody
				delay_us(100);
				//Gets N.MMMM ms values
				time_s = end / 1000;
				time_up_ms = end % 1000;
				time_up_ms = time_up_ms / 100;
				time_md_ms = end % 100;
				time_md_ms = time_md_ms / 10;
				time_lo_ms = end % 10;

				//converts values to ASCII to display
				time_s = LCD_convert_time_to_ascii(time_s);
				time_up_ms = LCD_convert_time_to_ascii(time_up_ms);
				time_md_ms = LCD_convert_time_to_ascii(time_md_ms);
				time_lo_ms = LCD_convert_time_to_ascii(time_lo_ms);

				//Displays everything
				lcd_set_cursor_position(1, 7);
				delay_us(100);
				write(time_s);
				delay_us(100);

				lcd_set_cursor_position(1, 8);
				delay_us(100);
				write('.');
				delay_us(100);

				lcd_set_cursor_position(1, 9);
				delay_us(100);
				write(time_up_ms);
				delay_us(100);

				lcd_set_cursor_position(1, 10);
				delay_us(100);
				write(time_md_ms);
				delay_us(100);

				lcd_set_cursor_position(1, 11);
				delay_us(100);
				write(time_lo_ms);
				delay_us(100);

				lcd_set_cursor_position(1, 13);
				delay_us(100);
				str_write("ms ");
			}

			while (1) {
				if (GPIOC->IDR & GPIO_IDR_ID13) {
					reset_state = 0;	//To display OG display
					random_state = 0;  	//To do number
					react_state = 0;	//To start react
					display_state = 0;	//To display
					delay_us(dd);
					break;
				}
			}

		}

	}

}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
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

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
