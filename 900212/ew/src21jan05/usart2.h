#ifndef _USART2_H
#define _USART2_H

void USART_Init_High(unsigned int baudrate);
void USART_Init_Low( unsigned int baudrate );
unsigned char USART_ReadUCSRC(void);
unsigned char USART_ReadUBRRH(void);

#endif  
