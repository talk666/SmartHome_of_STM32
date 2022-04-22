#include "stm32f10x.h"                  // Device header
#include "Relay.h"
#include "Delay.h"

void Relay_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//ʹ��APB2
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;//pc4
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	//GPIO_SetBits(GPIOC, GPIO_Pin_0);//���øߵ�ƽ
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);//���õ͵�ƽ  //GPIOA PA(GPIO_Pin_0--GPIO_Pin_15)   GPIOB PB(GPIO_Pin_0--GPIO_Pin_15).... 
}

void Relay_ON(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
}

void Relay_OFF(void)
{

	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}

void Relay_Turn(void)
{
	if(0 == GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_4))//����˵�����ǵ͵�ƽ ���ڳ��ص�״̬���ߵ�ƽ����ģʽ��
	{
		Relay_ON();
	}
	else
	{
		Relay_OFF();
	}
}

