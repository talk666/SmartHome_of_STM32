
//单片机头文件
#include "stm32f10x.h"
#include "delay.h"
#include "uart.h"
#include  "relay.h"

u8 ubAnalogWatchdogStatus = 0;
//模拟看门狗初始化 （用来检测一路或者多路adc数值，达到设置阈值则进入中断）
void AD_WGDOG(void)
{
	u16 High = 0xFFF;//0x05;
	u16 Low = 0x02;
	
	ADC_AnalogWatchdogThresholdsConfig(ADC1, High, Low);
	ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Channel_13);
	ADC_AnalogWatchdogCmd(ADC1,ADC_AnalogWatchdog_SingleRegEnable);

}

//temt 6000 光照强度范围在0-1000Lux

void ADC1_Init(void)
{
   	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1,ENABLE);//使能APB2
	
	//PC3作为模拟通道输入引脚
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值
	
	ADC_InitTypeDef ADC_InitStructure; 
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12M
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   
	


//是否配置中断
#if 0
	//adc模拟看门狗配置
	AD_WGDOG();
	
	NVIC_InitTypeDef ADC_NVIC_InitStructure;
	ADC_NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	ADC_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQ通道使能
	ADC_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//优先级
	ADC_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&ADC_NVIC_InitStructure);
	//ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);//打开ADC获取完一次数据的eof中断    ADC_IT_EOC为adc采集完毕后寄存器赋值为1
	ADC_ITConfig(ADC1,ADC_IT_AWD,ENABLE);//打开ADC模拟看门狗中断 
#endif
	
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
	
}



//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果  读取dr寄存器后会自动清除EOC标注
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		DelayMs(5);
	}
	return temp_val/times;
} 	 

void ADC1_2_IRQHandler(void)
{
	//UsartPrintf(USART_DEBUG, "ADC1_2_IRQHandler\r\n");
	ubAnalogWatchdogStatus  = 1;
	ADC_ITConfig(ADC1,ADC_IT_AWD,DISABLE);
	if(ADC_GetFlagStatus(ADC1,ADC_FLAG_AWD) == SET)
	{
		ADC_ClearFlag(ADC1,ADC_FLAG_AWD);
		ADC_ClearITPendingBit(ADC1,ADC_IT_AWD);//清除中断标识 否则一直重复进中断
		
	}
	ADC_ITConfig(ADC1,ADC_IT_AWD,ENABLE);
	/*
	
	uint16_t adc_lux;
	int Lx = 0;
	if(ADC_GetITStatus(ADC1,ADC_IT_AWD) == SET)
	{
		adc_lux = ADC_GetConversionValue(ADC1);
		Lx = (int)((adc_lux/4095.00)*1000.00);
		UsartPrintf(USART_DEBUG, "adc_lux=%d,Lx=%dlux\r\n",adc_lux,Lx);
		
		if(Lx <= 1){
			if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_4) == 0)
			{
				Relay_ON();//寄存器控制开灯
			}
		}else if(Lx >= 600){
			if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_4) == 1)
			{
				Relay_OFF();//寄存器控制关灯
			}
		}
	}*/
}
