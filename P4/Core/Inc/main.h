
#ifndef __MAIN_H
#define __MAIN_H



#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"

#define ERROR_BUFFER_SIZE 28  //initialize length of buffer to largest possible acknowledge packet
extern uint8_t ConfirmationCode;
extern uint8_t errorbuffer[ERROR_BUFFER_SIZE];
extern int flag;
extern int FPIDrdy;

void Error_Handler(void);



#ifdef __cplusplus
}
#endif

#endif
