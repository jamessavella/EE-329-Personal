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

int main(void) {
	//Initialization
	HAL_Init();
	SystemClock_Config();
	SysTick_Init();

	LPUART_init();

	//LPUART Live Terminal Display
//	LPUART_ESC_Print("[2J");	//clear all
	update_position(1, 1);
//	LPUART_Print("ADC counts volts");
//	update_position(2, 1);
//	LPUART_Print("MIN  0000  0.000 V");
//	update_position(3, 1);
//	LPUART_Print("MAX  0000  0.000 V");
//	update_position(4, 1);
//	LPUART_Print("AVG  0000  0.000 V");
//	LPUART_ESC_Print("[?25l"); //turns off cursor

	ADC_init(); //ADC initialization

	while (1) {
		if (done) {
			__disable_irq(); //disable interrupt
			LPUART_Print("PEPEE"); //for debugging to see if it interrupt even gets triggered
			ADC_updateArray();
			ADC_getAverage();
			ADC_getMin();
			ADC_getMax();
			ADC1->CR |= ADC_CR_ADSTART;  // start conversion
			done = 0;	//resets flag
			__enable_irq();	//re enables interrupt
		}
		//ADC_print();
	}
}

//updates terminal with new stuff
void ADC_print() {
	__disable_irq();
	//COL COUNT = 6			COL VOLTS = 12
	//Prints MIN VALUES : COUNT and VOLTS
	update_position(2, 6);	//count cursor
	ADC_countConversion(ADC_min);
	update_position(2, 12);	//volt cursor
	ADC_voltsConversion(count_to_volts(ADC_min));

	//Prints MAX Values : COUNT and VOLTS
	update_position(3, 6);	//count cursor
	ADC_countConversion(ADC_max);
	update_position(3, 12);	//volt cursor
	ADC_voltsConversion(count_to_volts(ADC_max));

	//Prints AVG Values : COUNT and VOLTS
	update_position(4, 6);	//count cursor
	ADC_countConversion(ADC_avg);
	update_position(4, 12);	//volt cursor
	ADC_voltsConversion(count_to_volts(ADC_avg));
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
	ADC_sample[SAMPLE] = ADC_input;	//loads value to the rightmost part in the array
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
	ADC1->SMPR1 |= (1 << ADC_SMPR1_SMP5_Pos);  // ch 5 sample time = 6.5 clocks
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

void ADC_IRQHandler(void) {
	if (ADC_ISR_EOC) {
		ADC_input = ADC1->DR; //copies input to variable
		done = 1;
		ADC1->ISR &= ~(ADC_ISR_EOC); //clear
	}
}

//Has this been adjusted to 8 MHz?
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
