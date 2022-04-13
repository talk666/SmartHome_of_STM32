#include "uart.h"
//C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
/*
************************************************************
*	函数名称：	Usart1_Init
*
*	函数功能：	串口1初始化
*
*	入口参数：	baud：设定的波特率
*
*	返回参数：	无
*
*	说明：		TX-PA9		RX-PA10
************************************************************
*/
void Usart1_Init(unsigned int baud)
{
	
	GPIO_InitTypeDef GpioInitStructure; //初始化GPIO结构体命名
	USART_InitTypeDef UsartInitStructure;//初始化USART结构体命名
	NVIC_InitTypeDef NVIC_InitStructure;//初始化NVIC
	
	//2. 配置时钟：GPIO口的时钟，引脚复用的时钟，串口的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//3.先配置tx输出引脚io（pa9）
	GpioInitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;//复用推挽输出
	GpioInitStructure.GPIO_Pin   = GPIO_Pin_9;
	GpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GpioInitStructure);
	
	//4.再配置rx输出引脚io（pa10）
	GpioInitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;//浮空输入
	GpioInitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GpioInitStructure);

	//是否配置中断
#if 1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQ通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStructure);
#endif
	//5.串口结构体的配置
	UsartInitStructure.USART_BaudRate = baud;        //波特率
	UsartInitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //硬件流
	UsartInitStructure.USART_Mode = USART_Mode_Rx| USART_Mode_Tx;					//模式
	UsartInitStructure.USART_Parity = USART_Parity_No;						//校验位
	UsartInitStructure.USART_StopBits =	USART_StopBits_1;					//停止位
	UsartInitStructure.USART_WordLength =	USART_WordLength_8b;				//字节长度
	
	USART_Init(USART1, &UsartInitStructure);
	
	//中断开启
#if 1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#endif
	//7.打开串口1
	USART_Cmd(USART1, ENABLE);//打开串口 比配置GPIO多这一步
	
}

/*
************************************************************
*	函数名称：	Usart2_Init
*
*	函数功能：	串口2初始化
*
*	入口参数：	baud：设定的波特率
*
*	返回参数：	无
*
*	说明：		TX-PA2		RX-PA3
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
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//接收和发送
	usartInitStruct.USART_Parity = USART_Parity_No;									//无校验
	usartInitStruct.USART_StopBits = USART_StopBits_1;								//1位停止位
	usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8位数据位
	USART_Init(USART2, &usartInitStruct);
	
	USART_Cmd(USART2, ENABLE);														//使能串口
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//使能接收中断
	
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInitStruct);

}
/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//发送数据
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
	}

}

/*
************************************************************
*	函数名称：	UsartPrintf
*
*	函数功能：	格式化打印
*
*	入口参数：	USARTx：串口组
*				fmt：不定长参
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}

}

/*
************************************************************
*	函数名称：	USART1_IRQHandler
*
*	函数功能：	串口1收发中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void USART1_IRQHandler(void)
{

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断
	{
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
	}

}

//uart4暂时配置失败
void Usart4_Init(void)
{
	
	GPIO_InitTypeDef GpioInitStructure; //初始化GPIO结构体命名
	USART_InitTypeDef UsartInitStructure;//初始化USART结构体命名
	NVIC_InitTypeDef NVIC_InitStructure;//初始化NVIC
	
	//2. 配置时钟：GPIO口的时钟，引脚复用的时钟，串口的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	//3.先配置Rx输出引脚io（pc10）
	GpioInitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;//复用推挽输出
	GpioInitStructure.GPIO_Pin   = GPIO_Pin_10;
	GpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC,&GpioInitStructure);
	
	//4.再配置Tx输出引脚io（pc11）
	GpioInitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;//浮空输入
	GpioInitStructure.GPIO_Pin   = GPIO_Pin_11;
	GPIO_Init(GPIOC,&GpioInitStructure);
	//是否配置中断
#if 1
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQ通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	
	NVIC_Init(&NVIC_InitStructure);
#endif
	//5.串口结构体的配置
	UsartInitStructure.USART_BaudRate =   115200;        //波特率
	UsartInitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //硬件流
	UsartInitStructure.USART_Mode = USART_Mode_Rx| USART_Mode_Tx;					//模式
	UsartInitStructure.USART_Parity = USART_Parity_No;						//校验位
	UsartInitStructure.USART_StopBits =	USART_StopBits_1;					//停止位
	UsartInitStructure.USART_WordLength =	USART_WordLength_8b;				//字节长度
	
	USART_Init(UART4, &UsartInitStructure);
//中断开启
#if 1
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
#endif
	//7.打开串口1
	USART_Cmd(UART4, ENABLE);//打开串口 比配置GPIO多这一步
	
}

//发送字符函数
void USARTSendByte(USART_TypeDef* USARTx, uint16_t Data)
{
		USART_SendData( USARTx,  Data);
		while( USART_GetFlagStatus( USARTx,  USART_FLAG_TXE) == RESET);//USART_GetFlagStatus是判断标志位 USART_FLAG_TXE 去usart.h  FLAG找
}
//发送字符串函数
void USARTSendString( USART_TypeDef* USARTx, char *str)
{
		uint16_t i = 0;
		do{
			USARTSendByte(USART1,*(str+i));
			i++;
		}while(*(str+i) != '\0');
		
		while( USART_GetFlagStatus( USARTx,  USART_FLAG_TC) == RESET);//USART_GetFlagStatus是判断标志位 USART_FLAG_TC（这是判断字符串） 去usart.h  FLAG找

}
//重写Printf函数
int fputc(int ch, FILE *f)
{
		USARTSendByte( USART1,  (uint8_t)ch);
		while( USART_GetFlagStatus( USART1,  USART_FLAG_TXE) == RESET);
		return (ch);
}
//重定向scanf函数
int fgetc(FILE *f)
{
		/*等待串口1输入数据*/
		while( USART_GetFlagStatus( USART1,  USART_FLAG_RXNE) == RESET);
		
		return (int) USART_ReceiveData(USART1);
//接受用rxne，输入用txe
}
#if 0
#define QUEUE_MSG_LEN 16
u8 usart_rx_buf[QUEUE_MSG_LEN];//接收数据最大值
s16 usart_rx_len = 0;

u8 uart1 = 0;
//串口1接收中断函数
void USART1_IRQHandler(void)
{
	//printf("USART1_IRQHandler\n");
	char re_data=0;//为了接收字符，还是定义为字符类型吧
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==1)//确认下是不是串口1接收中断
	{
		re_data=USART_ReceiveData(USART1);	//接收数据
		//USART_SendData(USART1,re_data);			//发送数据
		USART_SendData(UART4,re_data);			//发送数据
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); //等待数据发送完 成
	}	 
	//这个中断是不需要手动清除标志位的，因为读取数据后接收标志位会自动清零
}
//串口4接收中断函数
void UART4_IRQHandler(void)
{	
	printf("USART4_IRQHandler\r\n");
	char re_data=0;//为了接收字符，还是定义为字符类型吧
	if(USART_GetFlagStatus(UART4,USART_FLAG_RXNE)==1)//确认下是不是串口1接收中断
	{
		re_data=USART_ReceiveData(UART4);	//接收数据
		USART_SendData(USART1,re_data);		//发送数据
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET); //等待数据发送完 成
	}	 
	//这个中断是不需要手动清除标志位的，因为读取数据后接收标志位会自动清零
}
#endif

