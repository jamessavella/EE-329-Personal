#include "main.h"
#include "LPUART.h"
#include "delay.h"

#define size 20

uint16_t dataOut;
uint16_t data[size];
int convFlag = 0;
char printData[5];

void SystemClock_Config(void);

int getMax()
// function to get value of array, returns count
{
	int max = data[0];
	for(int i = 0; i < size; i++){
		if(data[i] > max){
			max = data[i];
		}
	}
	return (int)((float)(max*1.02)+3.61);
}

int getMin(){
// function to get value of array, returns count
	int min = data[0];
	for(int i = 0; i < size; i++){
		if(data[i] < min){
			min = data[i];
		}
	}
	return (int)((float)(min*1.035)-9.8);
}

int getAvg()
// function to get value of array, returns count
{
	int avg = 0;
	for(int i = 0; i < size; i++){
		avg += data[i];
	}
	avg = (float)avg/size;
	return (int)((float)(avg*1.02)+2.536);
}

char* convertCurrent(int input){
	int voltage = ((float)input/4095)*3300;
	int current = (float)voltage / 10000;

	printData[0] = current/1000 + '0';
	current = current%1000;
	printData[1] = '.';
	printData[2] = current/100 + '0';
	current = current%100;
	printData[3] = current/10 + '0';
	current = current%10;
	printData[4] = current + '0';

	return printData;
}

char* convertVolt(int input){
	// "#.###" returns char array
	int voltage = ((float)input/4095)*3300;
	printData[0] = voltage/1000 + '0';
	voltage = voltage%1000;
	printData[1] = '.';
	printData[2] = voltage/100 + '0';
	voltage = voltage%100;
	printData[3] = voltage/10 + '0';
	voltage = voltage%10;
	printData[4] = voltage + '0';
	return printData;
}

char* convertCount(int count){
	//returns char array
	printData[0] = ' ';
	printData[1] = count/1000 + '0';
	count = count%1000;
	printData[2] = count/100 + '0';
	count = count%100;
	printData[3] = count/10 + '0';
	count = count%10;
	printData[4] = count + '0';
	return printData;
}

void printAll(){
	__disable_irq();
	LPUART_ESC_Print("[2J");
	LPUART_ESC_Print("[0;0H");
	LPUART_print("ADC counts volts");

	LPUART_ESC_Print("[1;0H");
	LPUART_print("MIN ");
	LPUART_print(convertCount(getMin()));
	LPUART_print("  ");
	LPUART_print(convertVolt(getMin()));
	LPUART_print(" V");

	LPUART_ESC_Print("[2;0H");
	LPUART_print("MAX ");
	LPUART_print(convertCount(getMax()));
	LPUART_print("  ");
	LPUART_print(convertVolt(getMax()));
	LPUART_print(" V");

	LPUART_ESC_Print("[3;0H");
	LPUART_print("AVG ");
	LPUART_print(convertCount(getAvg()));
	LPUART_print("  ");
	LPUART_print(convertVolt(getAvg()));
	LPUART_print(" V");

	LPUART_ESC_Print("[4;0H");
	LPUART_print("coil current = ");
	LPUART_print(convertCurrent(getAvg()));
	LPUART_print(" A");

	delay_us(100000);
	__enable_irq();
}

void ADC_init(){
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;         // turn on clock for ADC
	// power up & calibrate ADC
	ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // clock source = HCLK/1
	ADC1->CR &= ~(ADC_CR_DEEPPWD);             // disable deep-power-down
	ADC1->CR |= (ADC_CR_ADVREGEN);             // enable V regulator - see RM 18.4.6
	delay_us(20);                              // wait 20us for ADC to power up
	ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5);    // PA0=ADC1_IN5, single-ended
	ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF); // disable ADC, single-end calib
	ADC1->CR |= ADC_CR_ADCAL;                  // start calibration
	while (ADC1->CR & ADC_CR_ADCAL) {;}        // wait for calib to finish
	// enable ADC
	ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
	ADC1->CR |= ADC_CR_ADEN;                   // enable ADC
	while(!(ADC1->ISR & ADC_ISR_ADRDY)) {;}    // wait for ADC Ready flag
	ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
	// configure ADC sampling & sequencing
	ADC1->SQR1  |= (5 << ADC_SQR1_SQ1_Pos);    // sequence = 1 conv., ch 5
	ADC1->SMPR1 |= (1 << ADC_SMPR1_SMP5_Pos);  // ch 5 sample time = 6.5 clocks
	// change shift of 1 to 4 for 47.5 and 7 for 640.5
	ADC1->CFGR  &= ~( ADC_CFGR_CONT  |         // single conversion mode
	                  ADC_CFGR_EXTEN |         // h/w trig disabled for s/w trig
	                  ADC_CFGR_RES   );        // 12-bit resolution
	// configure & enable ADC interrupt
	ADC1->IER |= ADC_IER_EOCIE;                // enable end-of-conv interrupt
	ADC1->ISR &= ~(ADC_ISR_EOC);               // clear EOC flag
	NVIC->ISER[0] = (1<<(ADC1_2_IRQn & 0x1F)); // enable ADC interrupt service
	__enable_irq();                            // enable global interrupts
	// configure GPIO pin PA0
	RCC->AHB2ENR  |= (RCC_AHB2ENR_GPIOAEN);    // connect clock to GPIOA
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL0);      // clear alt. function select
	GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL0_Pos); // choose AF 7 (PA0=ADC1_IN5)
	GPIOA->MODER  |= (GPIO_MODER_MODE0);	       // analog mode for PA0 (set MODER last)

	ADC1->CR |= ADC_CR_ADSTART;                // start 1st conversion
}

void ADC1_2_IRQHandler(){
	if(ADC_ISR_EOC){	// check for ISR to have value
		dataOut = ADC1->DR;	// store data from DR
		convFlag = 1;		// global flag for main
		ADC1->ISR &= ~(ADC_ISR_EOC);	// clear flag
	}
}

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
  SysTick_Init();

  RCC->AHB2ENR   |= (RCC_AHB2ENR_GPIOCEN);
  GPIOC->MODER   &= ~(GPIO_MODER_MODE11);
  GPIOC->MODER   |=  (GPIO_MODER_MODE11_0);
  GPIOC->OTYPER  &= ~(GPIO_OTYPER_OT11);
  GPIOC->PUPDR   &= ~(GPIO_OTYPER_OT11);

  GPIOC->MODER   &= ~(GPIO_MODER_MODE13);
  GPIOC->PUPDR   &= ~(GPIO_PUPDR_PUPD13_1);
  GPIOC->PUPDR   |= (GPIO_PUPDR_PUPD13_1);

  LPUART_init();

  ADC_init();
  int ind = 0;

  while (1)
  {
	  if(GPIOC->IDR & GPIO_PIN_13){
		  GPIOC->BSRR = GPIO_PIN_11;
	  }
	  else{
		  GPIOC->BRR = GPIO_PIN_11;
	  }
	  if(convFlag){
		  __disable_irq();
		  data[ind] = dataOut;
		  ind+=1;
		  if(ind > 19){
			  ind = 0;
		  }
		  convFlag = 0;
		  ADC1->CR |= ADC_CR_ADSTART;  // start conversion
		  __enable_irq();
	  }
	  printAll();
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
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

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
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
}
#endif
