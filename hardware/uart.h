#ifndef _USART_H_
#define _USART_H_


#include "stm32f10x.h"


#define USART_DEBUG		    USART1		//调试打印所使用的串口组
#define USART_ESP8266		UART4		

void Usart1_Init(unsigned int baud);

void Usart2_Init(unsigned int baud);

void Uart4_Init(unsigned int baud);

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);

#endif
