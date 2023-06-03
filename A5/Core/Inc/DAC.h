

#ifndef INC_DAC_H_
#define INC_DAC_H_

#include "main.h"

#define DAC GPIOA		//DAC Data Bus
#define SPI GPIO_PIN_7  //SPI = MOSI
#define SCK GPIO_PIN_5  //SCK Pin
#define NSS GPIO_PIN_4 //NSS Pin


//function declarations
void SPI_init(void);
void DAC_init(void);
int DAC_volt_conv(int convert);
void DAC_write(int word);


#endif
