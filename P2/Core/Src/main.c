/*
 ******************************************************************************
 * file      : main.c
 * project   : EE 329 S'23 A3
 * details   : A4 Part A
 * 			   restart timer
 * authors   : James Savella - savella@calpoly.edu
 *           : Cameron Buckeye - cbuckeye@calpoly.edu
 * co-author : Wilson Szeto -
 * version   : 0.1
 * date      : 230423
 * compiler  : STM32CubeIDE Version: 1.12.1 (2022)
 * target    : NUCLEO-L4A6ZG
 ******************************************************************************
 * wiring    :
 * LCD Output Pins
 ******************************************************************************
 * origins   :
 * modified from CubeIDE auto-generated main.c  (c) 2022 STMicro.
 ******************************************************************************
 */

#include "main.h"
#include "DAC.h"
#include "LCD.h"
#include "keypad.h"
#include "delay.h"
#include "wavegen.h"

void SystemClock_Config(void);

//Interrupt Function Declarations
void setup_MCO_CLK(void);
void setup_TIM2();
void TIM2_IRQHandler(void);
void Get_User_Input(void); 	//Zach Function Declaration

//ARR tick and max points in array
#define TICKS 90		//ARE THESE VALUES WRONG???
#define MAX 4000

//Variable declarations to output
int point_local = 0;	//what number to write to DAC from wave LUT
int wave_type = 4;		//1 sin | 2 pos ramp | 3 neg ramp | 4 square
int freq = 1;		// frequency (1 - 5) 100 Hz, 200 Hz, 300 Hz, 400 Hz, 500 Hz

//Duty Cycle Infomation
int duty_local = 4;		//Variable to tell give duty cycle
int duty_array[9] = { MAX * 0.1, MAX * 0.2, MAX * 0.3, MAX * 0.4, MAX * 0.5, MAX
		* 0.6, MAX * 0.7, MAX * 0.8, MAX * 0.9 };

//global variables for arrays---------------NEED TO TEST
int SINE_LUT[MAX];
int POS_RAMP_LUT[MAX];
int NEG_RAMP_LUT[MAX];

int main(void) {
	//Peripheral Initialization
	Keypad_init();
	LCD_config();
	LCD_init();
	DAC_init();
	SPI_init();
	Display_Init();

	//Initialization
	HAL_Init();
	SystemClock_Config();

	//Generates and updates GLOBAL LUT Arrays
	for (int wtype = 1; wtype < 4; wtype++) {
		for (int data_gen = 0; data_gen < MAX; data_gen++) {
			switch (wtype) {
			case 1:	//generates values for SIN_LUT
				SINE_LUT[data_gen] = gen_array(MAX, data_gen, wtype);
				break;

			case 2:	//generates values for POS_RAMP_LUT
				POS_RAMP_LUT[data_gen] = gen_array(MAX, data_gen, wtype);
				break;

			case 3:	//generates values for NEG_RAMP_LUT
				NEG_RAMP_LUT[data_gen] = gen_array(MAX, data_gen, wtype);
				break;
			}
		}
	}

	//Interrupt Enable
	setup_TIM2();

	while (1) {
		//-----------------DETECT KEYPAD------------------
		Get_User_Input();

		//-----------------DAC WRITE OUTPUT---------------
		if (point_local < MAX) {	//point_local gets updated per IRQ
			switch (wave_type) {
			case 1:				//Sinusoids Output
				DAC_write(SINE_LUT[point_local]);
				break;

			case 2:				//Positive Ramp Output
				DAC_write(POS_RAMP_LUT[point_local]);
				break;

			case 3:				//Negative Ramp Output
				DAC_write(NEG_RAMP_LUT[point_local]);
				break;

			case 4:				//Square Wave Output
				if (point_local < duty_array[duty_local]) {
					DAC_write(3000); 		//3 V Output
				} else
					DAC_write(0);			//0 V Output
				break;
			}
		} else
			point_local = 0;	//resets data array pointer location
	}
}

//Zach's Code
//void Get_User_Input(void) {
//	int Key_Press = Read_Keypad();
//
//	//---------------DETECTS KEYPRESS TO CHANGE FREQ
//	if ((Key_Press == 1) | (Key_Press == 2) | (Key_Press == 3)
//			| (Key_Press == 4) | (Key_Press == 5)) {
//		LCD_write_freq(Key_Press);
//		freq = Key_Press;						//Freq (100 Hz - 500 Hz)
//		LCD_Command(0x80 | 0x4E);
//		LCD_write_char(Key_Press + 48); //Writes freq to LCD (see function in LCD.c)
//	}
//
//	//---------------DETECTS KEYPRESS TO CHANGE DUTY CYCLE/RAMP POLAIRTY
//	if ((Key_Press == 10) | (Key_Press == 11) | (Key_Press == 0)) {
//		if (Key_Press == 10 && wave_type == 3) {	//Decreases Duty cycle if square waveform
//			duty_local = duty_local - 1;
//			LCD_Command(0x80 | 0x4E);
//			LCD_write_string("*");
//			if (duty_local < 0) {		//Ensures duty cycle never goes past array
//				duty_local = 0;
//			}
//			LCD_write_duty_cycle(duty_local); //Writes duty cycle to LCD (see function in LCD.c)
//		}
//		if ((Key_Press == 11) && (wave_type == 3)) { //Case for increasing duty cycle if square waveform
//			duty_local = duty_local + 1;
//			LCD_Command(0x80 | 0x4E);
//			LCD_write_string("#");
//			if (duty_local > 8) {		//Ensures duty cycle never goes past array
//				duty_local = 8;
//			}
//			LCD_write_duty_cycle(duty_local); //Writes duty cycle to LCD (see function in LCD.c)
//		}
//		if (Key_Press == 0) { //Case for setting 50% duty cycle if waveform is square and toggling polarity if sawtooth
//			if (wave_type == 2) {                //toggling polarity if sawtooth
//				wave_type = 3;					 //sets to negative polarity
//				LCD_Command(0x80 | 0x40);		 //Cursor Location
//				LCD_write_string("NEGATIVE");
//			} else {
//				wave_type = 2;					//sets to positive polarity
//				LCD_Command(0x80 | 0x40);		//Cursor Location
//				LCD_write_string("POSITIVE");
//			}
//		}
//		if (wave_type == 4) {        	//Setting 50% duty cycle if square wave
//			duty_local = 4;
//			LCD_Command(0x80 | 0x40);	//Sets Cursor
//			LCD_write_string("50% DUTY");
//		}
//		LCD_Command(0x80 | 0x4E);
//		LCD_write_string("0");
//	}
//
//	//------------------DETECTS KEYPRESS TO CHANGE WAVE OUTPUT
//	if ((Key_Press == 6) | (Key_Press == 7) | (Key_Press == 8)
//			| (Key_Press == 9)) {
//		LCD_Command(0x80 | 0x00);		//Sets Cursor
//		if (Key_Press == 6) {           //Case for setting a sinusoidal waveform
//			wave_type = 1;
//			LCD_write_string("SIN");
//			LCD_Command(0x80 | 0x40);	//Sets Cursor
//			LCD_write_string("nnnn PTS");
//		}
//		if (Key_Press == 7) {             //Case for setting a sawtooth waveform
//			wave_type = 2;				  //Positive Sawtooth
//			LCD_write_string("SAW");
//			LCD_Command(0x80 | 0x40);	//Sets Cursor
//			LCD_write_string("POSITIVE");
//
//		}
//		if (Key_Press == 8) {               //Case for setting a square waveform
//			wave_type = 4;					//Square Wave
//			LCD_write_string("SQU");
//			LCD_Command(0x80 | 0x40);	//Sets Cursor
//			LCD_write_duty_cycle(duty_local);
//			LCD_write_string("% DUTY");
//		}
//		if (Key_Press == 9) {           //Case for resetting to power-on default
//			wave_type = 4;				//Resets back to square
//			freq = 1;
//			duty_local = 4;
//			LCD_write_string("SQU");
//			LCD_Command(0x80 | 0x40);	//Sets Cursor
//			LCD_write_string("50% DUTY");
//			LCD_Command(0x80 | 0x04);	//Sets Cursor
//			LCD_write_string("100");
//		}
//		LCD_Command(0x80 | 0x4E);	//Sets Cursor
//		LCD_write_char(Key_Press + 48);
//	}
//	while (Key_Press != 12) {
//		Key_Press = Read_Keypad();
//	}
}

//Interrupt Timer Enable
void setup_TIM2() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;           // enable clock for TIM2
	TIM2->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE); // enable event gen, rcv CCR1
	TIM2->ARR = TICKS;                             // ARR = T = counts @4MHz
	TIM2->SR &= ~(TIM_SR_CC1IF | TIM_SR_UIF);      // clr IRQ flag in status reg
	NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F));     // set NVIC interrupt: 0x1F
	__enable_irq();                                 // global IRQ enable
	TIM2->CR1 |= TIM_CR1_CEN;                       // start TIM2 CR1
}

//Code to tweak to write to DAC
void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_UIF) {
		TIM2->SR &= ~(TIM_SR_UIF);		  //Clears Status Register
		point_local = point_local + freq; //update data array pointer location
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
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 18;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
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
