#include "stm32f10x.h"                  // Device header
#include "Beep.h"
#include "Delay.h"

void Beep_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��APB2
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;//Beep
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	//����Ĭ�ϵ͵�ƽ���Ը�Ϊ��
	GPIO_SetBits(GPIOB, GPIO_Pin_9);//���øߵ�ƽ
	//GPIO_ResetBits(GPIOC, GPIO_Pin_0);//���õ͵�ƽ  //GPIOA PA(GPIO_Pin_0--GPIO_Pin_15)   GPIOB PB(GPIO_Pin_0--GPIO_Pin_15).... 
}

void Beep_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);
}

void Beep_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_9);
}

void Beep_Flag(int conunt)
{
	while(conunt--)
	{
		Beep_ON();
		DelayMs(5);
		Beep_OFF();
		DelayMs(5);
	}
}
