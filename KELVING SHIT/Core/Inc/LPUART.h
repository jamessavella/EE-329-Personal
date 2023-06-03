/*
 * LPUART.h
 *
 *  Created on: May 25, 2023
 *      Author: james
 */

#ifndef INC_LPUART_H_
#define INC_LPUART_H_

void LPUART_init(void);
void LPUART_print(const char *message);
void LPUART1_IRQHandler(void);
void LPUART_ESC_Print(const char *message);
void splash_screen(void);
void update_position(uint8_t row, uint8_t col);
int randomInt(int min, int max);
void explodeFirework(int x, int y);

#endif /* INC_LPUART_H_ */
