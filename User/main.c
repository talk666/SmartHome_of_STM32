#include "stm32f10x.h"                  // Device header

#include "Delay.h"
#include "IWDG.h"
#include "uart.h"
#include "touch.h"
#include "Led.h"
#include "Beep.h"
#include "TIM.h"
#include <stdio.h>
//网络设备驱动
#include "esp_8266.h"
//云服务器
#include "Mqtt.h"
#include "MqttKit.h"

//保连续活失败次数
unsigned int Beat_fail_num;

/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置
	
	//IWDG_Init(6, 3906);             //独立看门狗初始化(40kHZ)（分频系数暂时设置为6为256分频 即一秒钟计数156.25次 一次需要0.0064秒->6.4毫秒一次） 3906*0.0064约等于25s
	
	Delay_Init();									//systick初始化
	
	Usart1_Init(115200);							//串口1，打印信息用
	
	Usart2_Init(115200);							//串口2，驱动ESP8266用
									
	LED_Init();										//LED2初始化
	
	Beep_Init();									//蜂鸣器初始化
	
	//TIM3_Int_Init(7199,9999);//5Khz的计数频率，计数到5000为1000ms  !!减1
	
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	//UsartPrintf(USART_ESP8266, " Hardware init OK\r\n");
}

int main(void)
{
	unsigned char *dataPtr = NULL;

	//阿里云自定义用于和微信小程序进行通讯的Topic
	const char *topics_cap_control[] = {STM32ToALiYunTopic};
	char  pPayLoad_cap_control[100];
	
	unsigned short timeCount = 0;	//发送间隔变量(心跳包)
	unsigned short status;
	unsigned short Led_status;
	
	Hardware_Init();				//初始化外围硬件
	ESP8266_Init();					//初始化ESP8266

	
	while(ALiYun_DevLink())			//接入ALiYun
		DelayXms(500);
	
	Beep_Flag(15);
	LED2_Flag(50);                  //循环快速闪烁4s

	ALiYun_Subscribe(topics_cap_control, 1);   //订阅主题


	//防止中途喂狗 在这进行初始化
	IWDG_Init(6, 3906);
	
//--步骤一：组包---------代码优化 不每次发送ping的时候都去重新组包放外面用一个 减少组包删包损耗-----------------------
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	while(MQTT_PacketPing(&mqttPacket)){
		UsartPrintf(USART_DEBUG, "MQTT_PacketPing Faild!\r\n");
		DelayXms(10);
	}

	
	while(1)
	{
		if(++timeCount >= 450)									//发送间隔5s  
		{
			ALiYun_HeartBeat(&mqttPacket);
			timeCount = 0;
			ESP8266_Clear();

		}
		if(timeCount%150 == 0){
			Led_status = GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_0);//led的电平状态
			sprintf(pPayLoad_cap_control,"{\"StatusLightSwitch\" : %d,\"TargetTemperature\": %.2f}",!Led_status,11.12);
			ALiYun_Publish(topics_cap_control[0], pPayLoad_cap_control);
			ESP8266_Clear();
			
			Feed_Dog();
		}
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
			ALiYun_RevPro(dataPtr);


		if(Beat_fail_num == 5){
			
			UsartPrintf(USART_DEBUG, "id=%d ,starting reconnect\r\n",Beat_fail_num);
			Feed_Dog();	
			ESP8266_Init();	//初始化ESP8266
			Feed_Dog();			
			while(ALiYun_DevLink())			//接入ALiYun
				DelayXms(500);
	
			Beep_Flag(15);
			LED2_Flag(50);                  //循环快速闪烁4s

			ALiYun_Subscribe(topics_cap_control, 1);   //订阅主题
			Beat_fail_num = 0;
		}
		
		status = Touch_GetStatus();
		if(status == 1){
			LED2_Turn();
		}
		
		DelayXms(5);
	}
	
}

