#ifndef INC_UART_H_
#define INC_UART_H_

//declare prototypes
void LPUART_init();
void LPUART_Print(const char *message);
void LPUART_Print_Char(char ch);
void USART_init(void);
void USART_print(char *c);
#endif /* INC_UART_H_ */
