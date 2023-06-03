/*
 * DAC.h
 *
 *  Created on: May 3, 2023
 *      Author: tazzn
 */

#ifndef INC_DAC_H_
#define INC_DAC_H_

#include "main.h"

#define DAC_PIN GPIOA


void DAC_init( void );
void SPI_init( void );
int DAC_volt_conv(int convert);
void DAC_write(int word);

#endif /* INC_DAC_H_ */

