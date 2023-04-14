#include "keypad.h"


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
