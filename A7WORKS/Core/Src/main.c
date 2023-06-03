#include "main.h"
#include "delay.h"
#include "LPUART.h"

//adjustable sample size
#define SAMPLE 20

//global variables
//ADC values are in counts needs to be converted to volts
int16_t ADC_input; //generated value
uint32_t ADC_avg;
uint32_t ADC_min;
uint32_t ADC_max;
uint32_t ADC_sample[SAMPLE];	//creates array of generated values
uint8_t done = 0; //main flag that disables interrupt when set high
uint8_t tenth_of_sec = 20; //value * 10 = seconds
uint8_t check = 0;		   //check variable to tell when to update screen

//ADC Function Declarations
void SystemClock_Config(void);
void ADC_init();
void ADC_getAverage();
void ADC_getMin();
void ADC_getMax();
void ADC_updateArray();
void ADC_update_test();
void ADC_countConversion(int value);
int count_to_volts(int value);
void ADC_voltsConversion(int value);
void ADC_print();
void ADC1_2_IRQHandler(void);

int main(void) {
	//Initialization
	HAL_Init();
	SystemClock_Config();
	SysTick_Init();
	LPUART_init();
	ADC_init(); //ADC initialization

	LPUART_ESC_Print("[2J");	//clear all
	update_position(1, 1);
	LPUART_Print("ADC counts volts");
	update_position(2, 1);
	LPUART_Print("MIN              V");
	update_position(3, 1);
	LPUART_Print("MAX              V");
	update_position(4, 1);
	LPUART_Print("AVG              V");

	while (1) {
		check = 0;	//resets check variable
		while (check < tenth_of_sec) {
			check++;			//increments count
			delay_us(10000);	//hundreth of a second delay
			if (done) {
				__disable_irq(); //disable interrupt
				ADC_updateArray();
				ADC_getAverage();
				ADC_getMin();
				ADC_getMax();
				done = 0;	//resets flag
				__enable_irq();	//re enables interrupt
				ADC1->CR |= ADC_CR_ADSTART;  // start conversion
			}
		}
		ADC_print();
	}
}

//updates terminal with new stuff
void ADC_print() {
	__disable_irq();
	//COL COUNT = 6			COL VOLTS = 12
	//Prints MIN VALUES : COUNT and VOLTS
	LPUART_ESC_Print("[2;6H");	//count cursor
	ADC_countConversion(ADC_min);
	LPUART_ESC_Print("[2;12H");	//volt cursor
	ADC_voltsConversion(count_to_volts(ADC_min));

	//Prints MAX Values : COUNT and VOLTS
	LPUART_ESC_Print("[3;6H");	//count cursor
	ADC_countConversion(ADC_max);
	LPUART_ESC_Print("[3;12H");	//volt cursor
	ADC_voltsConversion(count_to_volts(ADC_max));

	//Prints AVG Values : COUNT and VOLTS
	LPUART_ESC_Print("[4;6H");	//count cursor
	ADC_countConversion(ADC_avg);
	LPUART_ESC_Print("[4;12H");	//volt cursor
	ADC_voltsConversion(count_to_volts(ADC_avg));

	LPUART_ESC_Print("[?25l"); //turns off cursor
	__enable_irq();
}

//Converts count and writes string
void ADC_countConversion(int value) {
	char str[4] = { '0', '0', '0', '0' };
	int thousand, hundred, ten, one;
	thousand = value / 1000;
	hundred = (value % 1000) / 100;
	ten = (value % 100) / 10;
	one = (value % 10);
	str[3] = one + '0';
	if (value > 9)
		str[2] = ten + '0';
	if (value > 99)
		str[1] = hundred + '0';
	if (value > 999)
		str[0] = thousand + '0';
	LPUART_Print(str);
}

//Calibration
int count_to_volts(int value) {
	value = value * 0.8084 + 40;
	return value;
}

//Converts count_to_volts to volts str to write to terminal
void ADC_voltsConversion(int value) {
	char str[5] = { '0', '.', '0', '0', '0' };
	int thousand, hundred, ten, one;
	thousand = value / 1000;
	hundred = (value % 1000) / 100;
	ten = (value % 100) / 10;
	one = (value % 10);
	str[4] = one + '0';
	if (value > 9)
		str[3] = ten + '0';
	if (value > 99)
		str[2] = hundred + '0';
	if (value > 999)
		str[0] = thousand + '0';
	LPUART_Print(str);
}

void ADC_updateArray() {
	for (int i = 0; i < SAMPLE - 1; i++) { //shifts array to the left, deletes leftmost to update
		ADC_sample[i] = ADC_sample[i + 1];
	}
	ADC_sample[SAMPLE - 1] = ADC_input;	//loads value to the rightmost part in the array
}

//calculates the average of an array
void ADC_getAverage() {
	uint32_t sum = 0;
	for (int i = 0; i < SAMPLE; i++) {//loads and adds each value in the sample
		sum += ADC_sample[i];
	}
	ADC_avg = sum / SAMPLE;	//updates and gets average value
}

void ADC_getMin() {
	uint32_t smallest = ADC_sample[0];
	for (int i = 0; i < SAMPLE; i++) {//scans through and checks if input is the smallest
		if (ADC_sample[i] < smallest) {
			smallest = ADC_sample[i];
		}
	}
	ADC_min = smallest; //updates and gets smallest value
}

void ADC_getMax() {
	uint32_t largest = ADC_sample[0];
	for (int i = 0; i < SAMPLE; i++) { //scans through and checks if input is the largest
		if (ADC_sample[i] > largest) {
			largest = ADC_sample[i];
		}
	}
	ADC_max = largest; //updates and gets largest value
}

void ADC_init() {
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;         // turn on clock for ADC
	// power up & calibrate ADC
	ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // clock source = HCLK/1
	ADC1->CR &= ~(ADC_CR_DEEPPWD);             // disable deep-power-down
	ADC1->CR |= (ADC_CR_ADVREGEN);         // enable V regulator - see RM 18.4.6
	delay_us(20);                              // wait 20us for ADC to power up
	ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5);    // PA0=ADC1_IN5, single-ended
	ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF); // disable ADC, single-end calib
	ADC1->CR |= ADC_CR_ADCAL;                  // start calibration
	while (ADC1->CR & ADC_CR_ADCAL) {
		;
	}        // wait for calib to finish
	// enable ADC
	ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
	ADC1->CR |= ADC_CR_ADEN;                   // enable ADC
	while (!(ADC1->ISR & ADC_ISR_ADRDY)) {
		;
	}    // wait for ADC Ready flag
	ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
	// configure ADC sampling & sequencing
	ADC1->SQR1 |= (5 << ADC_SQR1_SQ1_Pos);    // sequence = 1 conv., ch 5
	ADC1->SMPR1 |= (7 << ADC_SMPR1_SMP5_Pos);  // ch 5 sample time = 6.5 clocks
	ADC1->CFGR &= ~( ADC_CFGR_CONT |         // single conversion mode
			ADC_CFGR_EXTEN |         // h/w trig disabled for s/w trig
			ADC_CFGR_RES);        // 12-bit resolution
	// configure & enable ADC interrupt
	ADC1->IER |= ADC_IER_EOCIE;                // enable end-of-conv interrupt
	ADC1->ISR &= ~(ADC_ISR_EOC);               // clear EOC flag
	NVIC->ISER[0] = (1 << (ADC1_2_IRQn & 0x1F)); // enable ADC interrupt service
	__enable_irq();                            // enable global interrupts
	// configure GPIO pin PA0
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);    // connect clock to GPIOA
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL0);      // clear alt. function select
	GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL0_Pos); // choose AF 7 (PA0=ADC1_IN5)
	GPIOA->MODER |= (GPIO_MODER_MODE0);	 // analog mode for PA0 (set MODER last)

	ADC1->CR |= ADC_CR_ADSTART;                // start 1st conversion
}

void ADC1_2_IRQHandler(void) {
	if (ADC_ISR_EOC) {
		ADC_input = ADC1->DR; //copies input to variable
		done = 1;
		ADC1->ISR &= ~(ADC_ISR_EOC); //clear
	}
}
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
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
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_9;
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
