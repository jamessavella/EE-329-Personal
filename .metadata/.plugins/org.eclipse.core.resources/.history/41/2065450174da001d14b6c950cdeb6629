#include "main.h"
#include "keypad.h"

void SystemClock_Config(void);

int set_leds(uint32_t);

static const int keys[4][3] = {{1, 2, 3},
                               {4, 5, 6},
                               {7, 8, 9},
                               {0xE, 0, 0xF}};

static    int cols[4] = {Col1, Col2, Col3};
static    int rows[4] = {Row1, Row2, Row3, Row4};

void keypad_init() {
    /*
     * This function, configures row, column and keypad pin
    */

    //Making column pins output mode bits(01)
    COL_PORT->MODER &= ~(GPIO_MODER_MODE13 | GPIO_MODER_MODE14 | GPIO_MODER_MODE15);
    COL_PORT->MODER |= (GPIO_MODER_MODE13_0 | GPIO_MODER_MODE14_0 | GPIO_MODER_MODE15_0);

    //Making row pins input mode bits(00)
    ROW_PORT->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3);

    //enable pull-down resistor bits(10)
    ROW_PORT->PUPDR &= (GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
    ROW_PORT->PUPDR |= (GPIO_PUPDR_PUPD0_1 | GPIO_PUPDR_PUPD1_1 | GPIO_PUPDR_PUPD2_1 | GPIO_PUPDR_PUPD3_1);
}

int keypad_read(int cols, int row){
    COL_PORT -> BSRR = (Col1 |Col2|Col3); //set columns to high value

    //check if key is pressed
    if (ROW_PORT->IDR & (Row1|Row2|Row3|Row4)){

        //turn off columns
        COL_PORT->BRR = (Col1 |Col2|Col3);
        return check_row_col(NUM_OF_COLS, NUM_OF_ROWS);
    }
    GPIOB->BRR = GPIO_PIN_10;
    return -1;
}

//iterate through each column and check all rows
int check_row_col(int col, int row){
    for (int c=0; c < col; c++){
        COL_PORT->BSRR = cols[c]; //turn on the column
        for (int r=0; r < row; r++){ //Check if key pressed is in the row
            if (ROW_PORT->IDR & (rows[r])) { //Check if key pressed is in the row
               return keys[r][c];
            }
        }
    }
    return -1;
}

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  SystemClock_Config();


  /* Enable Clk*/
  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);
  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN);

  /*Configure LED Output*/
  GPIOB-> MODER &= ~(GPIO_MODER_MODE3 |GPIO_MODER_MODE5 | GPIO_MODER_MODE4 | GPIO_MODER_MODE10);
  GPIOB-> MODER |= (GPIO_MODER_MODE3_0 |GPIO_MODER_MODE5_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE10_0);
  keypad_init();
  GPIOB->BRR = GPIO_PIN_10;
  while (1)
  {
      int output = keypad_read( 4, 3); //get key value
      if (output != -1){
      // count++;
          set_leds(output); //set leds to value
      }
      else {set_leds(0);} //if key not pressed set leds to 0
  }
}

int set_leds(uint32_t number) {
	  // Make pin0 output mode
	  GPIOA->MODER &= ~(GPIO_MODER_MODE0);
	  GPIOA->MODER |= (GPIO_MODER_MODE0_0);

	  // Make pin1 output mode
	  GPIOA->MODER &= ~(GPIO_MODER_MODE1);
	  GPIOA->MODER |= (GPIO_MODER_MODE1_0);

	  // Make pin2 output mode
	  GPIOA->MODER &= ~(GPIO_MODER_MODE2);
	  GPIOA->MODER |= (GPIO_MODER_MODE2_0);

	  // Make pin3 output mode
	  GPIOA->MODER &= ~(GPIO_MODER_MODE3);
	  GPIOA->MODER |= (GPIO_MODER_MODE3_0);



	  GPIOA->ODR = number; // changes all 16 port bits to count in binary


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
