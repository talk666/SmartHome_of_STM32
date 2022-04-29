#include "stm32f10x.h"                  // Device header

#include "Delay.h"
#include "IWDG.h"
#include "uart.h"
#include "touch.h"
#include "Led.h"
#include "Beep.h"
#include "relay.h"
#include "TIM.h"
#include "SHT31.h"
#include "LCD.h"
#include "ADC.h"

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
	
	I2C1_init();                                    //I2C1 进行初始化
	
	Usart1_Init(115200);							//串口1，打印信息用
	
	Usart2_Init(115200);							//串口2，驱动ESP8266用
	
	Uart4_Init(115200);                               //串口4
	
	ADC1_Init();                                    //ADC
									
	LED_Init();										//LED2初始化
	
	LCD_Init();                                    //LCD  tft1.8 初始化
	
	Beep_Init();									//蜂鸣器初始化
	
	Touch_Init();                                  //触摸开关初始化
	
	Relay_Init();                                  //继电器初始化
	
	//TIM3_Int_Init(7199,9999);//5Khz的计数频率，计数到5000为1000ms  !!减1
	
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
}

int main(void)
{
#if 0    //测试分支
	u16 adcx;
	Hardware_Init();				//初始化外围硬件 lcd显示
	float Rh_true = 0.0, Tmp_true = 0.0;
	int Lx = (int)((adcx/4095.00)*1000.00);
	while(1){
		adcx=Get_Adc_Average(ADC_Channel_1,10);
		UsartPrintf(USART_DEBUG, "ADC--0x%X\r\n",adcx);
		GetDataFromSHT31(&Tmp_true, &Rh_true);
		TMP_RH_INFO(Tmp_true, Rh_true, Lx);
		UsartPrintf(USART_DEBUG, "温度--%.2f℃, 湿度--%%%.2f\r\n",Tmp_true, Rh_true);
		DelayMs(500);
	}

	
#elif 0
	
	Hardware_Init();			//初始化外围硬件  温度
	UsartPrintf(USART_DEBUG, "ADC--0x%X\r\n");
	ESP8266_Init();					//初始化ESP8266
	

#elif 1
	unsigned char *dataPtr = NULL;

	//阿里云自定义用于和微信小程序进行通讯的Topic
	const char *topics_cap_control[] = {STM32ToALiYunTopic};
	char  pPayLoad_cap_control[100];
	
	unsigned short timeCount = 0;	//发送间隔变量(心跳包)
	unsigned short status;
	unsigned short Led_status;
	
	float Rh_true = 0.0, Tmp_true = 0.0;//温湿度数据
	
	u16 adcx; //adc读取数据
	
	static int Lx = 0;               // temt 6000 光照强度范围在0-1000Lux
	
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
			//LCD显示 放在这里延缓LCD刷新时间
			TMP_RH_INFO(Tmp_true, Rh_true, Lx);

		}
		if(timeCount%150 == 0){
			Led_status = GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_0);//led的电平状态
			GetDataFromSHT31(&Tmp_true, &Rh_true); //温湿度状态
			adcx = Get_Adc_Average(ADC_Channel_13, 10);
			//转换成LX 实际值
			Lx = (int)((adcx/4095.00)*1000.00);
			
			UsartPrintf(USART_DEBUG, "ADC--%d_lx--%d",adcx, Lx);
			UsartPrintf(USART_DEBUG, "温度--%.2f℃, 湿度--%%%.2f\r\n",Tmp_true, Rh_true);
			
			sprintf(pPayLoad_cap_control,"{\"StatusLightSwitch\" : %d,\"TargetTemperature\": %.2f,\"RelativeHumidity\": %.2f}",!Led_status,Tmp_true,Rh_true);
			ALiYun_Publish(topics_cap_control[0], pPayLoad_cap_control);
			ESP8266_Clear();
			
			Feed_Dog();
		}
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
			ALiYun_RevPro(dataPtr);


		if(Beat_fail_num == 3){
			
			UsartPrintf(USART_DEBUG, "id=%d ,starting reconnect\r\n",Beat_fail_num);
			Feed_Dog();	
			ESP8266_Init();	//初始化ESP8266
			Feed_Dog();			
			while(ALiYun_DevLink())			//接入ALiYun
				DelayXms(500);
	
			Beep_Flag(45); //长响代表重新连接成功 并未掉电
			
			ALiYun_Subscribe(topics_cap_control, 1);   //订阅主题
			Beat_fail_num = 0;
		}
		
		status = Touch_GetStatus();
		if(status == 1){
			LED2_Turn();
			Relay_Turn();
		}
#if 0
		if(ubAnalogWatchdogStatus == 1 && led_flag_once)
		{
			//说明模拟看门狗中断执行 有阈值需要进行处理
			//UsartPrintf(USART_DEBUG, "AD_DOG:ADC--%d_lx--%d",adcx, Lx);
			if(Lx <= 2) Relay_ON();
			led_flag_once = 0;
		}
#endif
		DelayXms(5);
	}
	
#endif
	
}

