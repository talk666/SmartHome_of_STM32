#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "Delay.h"

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//ʹ��APB2
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;//led2
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	//����Ĭ�ϵ͵�ƽ���Ը�Ϊ��
	GPIO_SetBits(GPIOC, GPIO_Pin_0);//���øߵ�ƽ
	//GPIO_ResetBits(GPIOC, GPIO_Pin_0);//���õ͵�ƽ  //GPIOA PA(GPIO_Pin_0--GPIO_Pin_15)   GPIOB PB(GPIO_Pin_0--GPIO_Pin_15).... 
}

void LED2_ON(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_0);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_0);
}

void LED2_Turn(void)
{
	if(0 == GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_0))//����˵�����ǵ͵�ƽ ������״̬
	{
		LED2_OFF();
	}
	else
	{
		LED2_ON();
	}
}

void LED2_Flag(int conunt)
{
	while(conunt--)
	{
		LED2_ON();
		DelayMs(5);
		LED2_OFF();
		DelayMs(5);
	}
}
