#include "touch.h"
#include "Delay.h"

void Touch_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;//pc7 ��
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

uint8_t Touch_GetStatus(void)
{
	uint8_t status = 0 ;
	if (1 == GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))//pc7Ĭ�ϵ͵�ƽ
	{
		DelayMs(300);//��ʱ��ֹ�ڶ����ж�
//	while(0 == GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7));
	//	DelayMs(20);
		status = 1;
	}
	return status;
}
