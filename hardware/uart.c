#include "uart.h"
//C��
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
/*
************************************************************
*	�������ƣ�	Usart1_Init
*
*	�������ܣ�	����1��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PA9		RX-PA10
************************************************************
*/
void Usart1_Init(unsigned int baud)
{
	
	GPIO_InitTypeDef GpioInitStructure; //��ʼ��GPIO�ṹ������
	USART_InitTypeDef UsartInitStructure;//��ʼ��USART�ṹ������
	NVIC_InitTypeDef NVIC_InitStructure;//��ʼ��NVIC
	
	//2. ����ʱ�ӣ�GPIO�ڵ�ʱ�ӣ����Ÿ��õ�ʱ�ӣ����ڵ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//3.������tx�������io��pa9��
	GpioInitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;//�����������
	GpioInitStructure.GPIO_Pin   = GPIO_Pin_9;
	GpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GpioInitStructure);
	
	//4.������rx�������io��pa10��
	GpioInitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;//��������
	GpioInitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GpioInitStructure);

	//�Ƿ������ж�
#if 1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQͨ��ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStructure);
#endif
	//5.���ڽṹ�������
	UsartInitStructure.USART_BaudRate = baud;        //������
	UsartInitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //Ӳ����
	UsartInitStructure.USART_Mode = USART_Mode_Rx| USART_Mode_Tx;					//ģʽ
	UsartInitStructure.USART_Parity = USART_Parity_No;						//У��λ
	UsartInitStructure.USART_StopBits =	USART_StopBits_1;					//ֹͣλ
	UsartInitStructure.USART_WordLength =	USART_WordLength_8b;				//�ֽڳ���
	
	USART_Init(USART1, &UsartInitStructure);
	
	//�жϿ���
#if 1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#endif
	//7.�򿪴���1
	USART_Cmd(USART1, ENABLE);//�򿪴��� ������GPIO����һ��
	
}

/*
************************************************************
*	�������ƣ�	Usart2_Init
*
*	�������ܣ�	����2��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PA2		RX-PA3
************************************************************
*/
void Usart2_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//PA2	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA3	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
	usartInitStruct.USART_Parity = USART_Parity_No;									//��У��
	usartInitStruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
	USART_Init(USART2, &usartInitStruct);
	
	USART_Cmd(USART2, ENABLE);														//ʹ�ܴ���
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
	
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInitStruct);

}
/*
************************************************************
*	�������ƣ�	Usart_SendString
*
*	�������ܣ�	�������ݷ���
*
*	��ڲ�����	USARTx��������
*				str��Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//��������
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
	}

}

/*
************************************************************
*	�������ƣ�	UsartPrintf
*
*	�������ܣ�	��ʽ����ӡ
*
*	��ڲ�����	USARTx��������
*				fmt����������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//��ʽ��
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}

}

/*
************************************************************
*	�������ƣ�	USART1_IRQHandler
*
*	�������ܣ�	����1�շ��ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void USART1_IRQHandler(void)
{

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //�����ж�
	{
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
	}

}

//uart4��ʱ����ʧ��
void Usart4_Init(void)
{
	
	GPIO_InitTypeDef GpioInitStructure; //��ʼ��GPIO�ṹ������
	USART_InitTypeDef UsartInitStructure;//��ʼ��USART�ṹ������
	NVIC_InitTypeDef NVIC_InitStructure;//��ʼ��NVIC
	
	//2. ����ʱ�ӣ�GPIO�ڵ�ʱ�ӣ����Ÿ��õ�ʱ�ӣ����ڵ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	//3.������Rx�������io��pc10��
	GpioInitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;//�����������
	GpioInitStructure.GPIO_Pin   = GPIO_Pin_10;
	GpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC,&GpioInitStructure);
	
	//4.������Tx�������io��pc11��
	GpioInitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;//��������
	GpioInitStructure.GPIO_Pin   = GPIO_Pin_11;
	GPIO_Init(GPIOC,&GpioInitStructure);
	//�Ƿ������ж�
#if 1
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQͨ��ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	
	NVIC_Init(&NVIC_InitStructure);
#endif
	//5.���ڽṹ�������
	UsartInitStructure.USART_BaudRate =   115200;        //������
	UsartInitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //Ӳ����
	UsartInitStructure.USART_Mode = USART_Mode_Rx| USART_Mode_Tx;					//ģʽ
	UsartInitStructure.USART_Parity = USART_Parity_No;						//У��λ
	UsartInitStructure.USART_StopBits =	USART_StopBits_1;					//ֹͣλ
	UsartInitStructure.USART_WordLength =	USART_WordLength_8b;				//�ֽڳ���
	
	USART_Init(UART4, &UsartInitStructure);
//�жϿ���
#if 1
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
#endif
	//7.�򿪴���1
	USART_Cmd(UART4, ENABLE);//�򿪴��� ������GPIO����һ��
	
}

//�����ַ�����
void USARTSendByte(USART_TypeDef* USARTx, uint16_t Data)
{
		USART_SendData( USARTx,  Data);
		while( USART_GetFlagStatus( USARTx,  USART_FLAG_TXE) == RESET);//USART_GetFlagStatus���жϱ�־λ USART_FLAG_TXE ȥusart.h  FLAG��
}
//�����ַ�������
void USARTSendString( USART_TypeDef* USARTx, char *str)
{
		uint16_t i = 0;
		do{
			USARTSendByte(USART1,*(str+i));
			i++;
		}while(*(str+i) != '\0');
		
		while( USART_GetFlagStatus( USARTx,  USART_FLAG_TC) == RESET);//USART_GetFlagStatus���жϱ�־λ USART_FLAG_TC�������ж��ַ����� ȥusart.h  FLAG��

}
//��дPrintf����
int fputc(int ch, FILE *f)
{
		USARTSendByte( USART1,  (uint8_t)ch);
		while( USART_GetFlagStatus( USART1,  USART_FLAG_TXE) == RESET);
		return (ch);
}
//�ض���scanf����
int fgetc(FILE *f)
{
		/*�ȴ�����1��������*/
		while( USART_GetFlagStatus( USART1,  USART_FLAG_RXNE) == RESET);
		
		return (int) USART_ReceiveData(USART1);
//������rxne��������txe
}
#if 0
#define QUEUE_MSG_LEN 16
u8 usart_rx_buf[QUEUE_MSG_LEN];//�����������ֵ
s16 usart_rx_len = 0;

u8 uart1 = 0;
//����1�����жϺ���
void USART1_IRQHandler(void)
{
	//printf("USART1_IRQHandler\n");
	char re_data=0;//Ϊ�˽����ַ������Ƕ���Ϊ�ַ����Ͱ�
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==1)//ȷ�����ǲ��Ǵ���1�����ж�
	{
		re_data=USART_ReceiveData(USART1);	//��������
		//USART_SendData(USART1,re_data);			//��������
		USART_SendData(UART4,re_data);			//��������
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); //�ȴ����ݷ����� ��
	}	 
	//����ж��ǲ���Ҫ�ֶ������־λ�ģ���Ϊ��ȡ���ݺ���ձ�־λ���Զ�����
}
//����4�����жϺ���
void UART4_IRQHandler(void)
{	
	printf("USART4_IRQHandler\r\n");
	char re_data=0;//Ϊ�˽����ַ������Ƕ���Ϊ�ַ����Ͱ�
	if(USART_GetFlagStatus(UART4,USART_FLAG_RXNE)==1)//ȷ�����ǲ��Ǵ���1�����ж�
	{
		re_data=USART_ReceiveData(UART4);	//��������
		USART_SendData(USART1,re_data);		//��������
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET); //�ȴ����ݷ����� ��
	}	 
	//����ж��ǲ���Ҫ�ֶ������־λ�ģ���Ϊ��ȡ���ݺ���ձ�־λ���Զ�����
}
#endif

