/*
 ***************************************************************************************
 * file      : main.c
 * project   : EE 329 S'23 A8
 * group     : GB
 * details   : I2C interface with EEPROM
 * authors   : Rushabh Shah - rshah49@calpoly.edu
 *      Current Project Partner(s)
 *           : James Savella - jsavella@calpoly.edu
 *      Original project partner(s)
 *           : Jack Shaffer - jdshaffer@calpoly.edu
 *           : Mason McMguire - mmcgui09@calpoly.edu
 *           : Hardit Singh - hsingh51@calpoly.edu
 *           : Zachary Numa - znuma@calpoly.edu
 *           : Steve Tran - stran100@calpoly.edu
 *           : Zach Kwast - zkwast@calpoly.edu
 * version   : 1
 * date      : 6/2/2023
 * compiler  : STM32CubeIDE Version: 1.12.0
 * target    : NUCLEO-L4A6ZG
 ***************************************************************************************
 */


#include "main.h"
#include "SysTick.h"
#include "I2C.h"


void SystemClock_Config(void);
void blue_LED_setup();


int main(void) {
    HAL_Init();
    SystemClock_Config();


    SysTick_setup();  //setup microsecond delay function
    blue_LED_setup();
    I2C_setup();


    uint8_t data_read = 0;
        I2C_write(0x0020, 0x31);
        delay_u(6000);
        data_read = I2C_read(0x0020);
        if (data_read == 0x31) GPIOB->BSRR = GPIO_PIN_7;


    while (1) {
//      GPIOB->ODR ^= GPIO_PIN_7; // reset LEDs
//      I2C_write(0x0020, 0x31);
//      delay_u(250000);
    }
}


void blue_LED_setup(){
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);
    GPIOB->MODER &= ~(GPIO_MODER_MODE7); // save all other pin modes and reset PC0 - PC3
    GPIOB->MODER |= (GPIO_MODER_MODE7_0);
            // set the desired modes for pins PC0 - PC3
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT7);
            // set PC0 - PC3 to push-pull and all other pins to open-drain
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD7);
            // set PC0 - PC3 to No pull-up, pull-down and all other PC to reserved
    GPIOB->OSPEEDR |= (3 << GPIO_OSPEEDR_OSPEED7_Pos);
            // set PC0 - PC3 to very high speed and leave other PC unchanged
    GPIOB->BRR = GPIO_PIN_7; // reset LEDs
}


//Configurations below are left as their default, auto-generated values
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


