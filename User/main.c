#include "stm32f10x.h"                  // Device header

#include "Delay.h"
#include "IWDG.h"
#include "uart.h"
#include "touch.h"
#include "Led.h"
#include "Beep.h"
#include "Relay.h"
#include "TIM.h"
#include "SHT31.h"
#include "LCD.h"

#include <stdio.h>
//�����豸����
#include "esp_8266.h"
//�Ʒ�����
#include "Mqtt.h"
#include "MqttKit.h"

//��������ʧ�ܴ���
unsigned int Beat_fail_num;

/*
************************************************************
*	�������ƣ�	Hardware_Init
*
*	�������ܣ�	Ӳ����ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		��ʼ����Ƭ�������Լ�����豸
************************************************************
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϿ�������������
	
	//IWDG_Init(6, 3906);             //�������Ź���ʼ��(40kHZ)����Ƶϵ����ʱ����Ϊ6Ϊ256��Ƶ ��һ���Ӽ���156.25�� һ����Ҫ0.0064��->6.4����һ�Σ� 3906*0.0064Լ����25s
	
	Delay_Init();									//systick��ʼ��
	
	I2C1_init();                                    //I2C1 ���г�ʼ��
	
	Usart1_Init(115200);							//����1����ӡ��Ϣ��
	
	Usart2_Init(115200);							//����2������ESP8266��
									
	LED_Init();										//LED2��ʼ��
	
	LCD_Init();                                    //LCD  tft1.8 ��ʼ��
	
	Beep_Init();									//��������ʼ��
	
	Touch_Init();                                  //�������س�ʼ��
	
	Relay_Init();                                  //�̵�����ʼ��
	
	//TIM3_Int_Init(7199,9999);//5Khz�ļ���Ƶ�ʣ�������5000Ϊ1000ms  !!��1
	
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	//UsartPrintf(USART_ESP8266, " Hardware init OK\r\n");
}

int main(void)
{
#if 0    //���Է�֧
	Hardware_Init();				//��ʼ����ΧӲ��
	float Rh_true = 0.0, Tmp_true = 0.0;
	
	while(1){
		
		GetDataFromSHT31(&Tmp_true, &Rh_true);
		TMP_RH_INFO(Tmp_true, Rh_true);
		UsartPrintf(USART_DEBUG, "�¶�--%.2f��, ʪ��--%%%.2f\r\n",Tmp_true, Rh_true);
		DelayMs(500);
	}
#else
	unsigned char *dataPtr = NULL;

	//�������Զ������ں�΢��С�������ͨѶ��Topic
	const char *topics_cap_control[] = {STM32ToALiYunTopic};
	char  pPayLoad_cap_control[100];
	
	unsigned short timeCount = 0;	//���ͼ������(������)
	unsigned short status;
	unsigned short Led_status;
	
	float Rh_true = 0.0, Tmp_true = 0.0;//��ʪ������
	
	Hardware_Init();				//��ʼ����ΧӲ��

	ESP8266_Init();					//��ʼ��ESP8266
	
	
	
	while(ALiYun_DevLink())			//����ALiYun
		DelayXms(500);
	
	Beep_Flag(15);
	LED2_Flag(50);                  //ѭ��������˸4s

	ALiYun_Subscribe(topics_cap_control, 1);   //��������


	//��ֹ��;ι�� ������г�ʼ��
	IWDG_Init(6, 3906);
	
//--����һ�����---------�����Ż� ��ÿ�η���ping��ʱ��ȥ���������������һ�� �������ɾ�����-----------------------
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	while(MQTT_PacketPing(&mqttPacket)){
		UsartPrintf(USART_DEBUG, "MQTT_PacketPing Faild!\r\n");
		DelayXms(10);
	}

	
	while(1)
	{
		if(++timeCount >= 450)									//���ͼ��5s  
		{
			ALiYun_HeartBeat(&mqttPacket);			
			timeCount = 0;
			ESP8266_Clear();

		}
		if(timeCount%150 == 0){
			Led_status = GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_0);//led�ĵ�ƽ״̬
			GetDataFromSHT31(&Tmp_true, &Rh_true); //��ʪ��״̬
			TMP_RH_INFO(Tmp_true, Rh_true);//LCD��ʾ
			UsartPrintf(USART_DEBUG, "�¶�--%.2f��, ʪ��--%%%.2f\r\n",Tmp_true, Rh_true);
			
			sprintf(pPayLoad_cap_control,"{\"StatusLightSwitch\" : %d,\"TargetTemperature\": %.2f,\"RelativeHumidity\": %.2f}",!Led_status,Tmp_true,Rh_true);
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
			ESP8266_Init();	//��ʼ��ESP8266
			Feed_Dog();			
			while(ALiYun_DevLink())			//����ALiYun
				DelayXms(500);
	
			Beep_Flag(45); //��������������ӳɹ�
			
			ALiYun_Subscribe(topics_cap_control, 1);   //��������
			Beat_fail_num = 0;
		}
		
		status = Touch_GetStatus();
		if(status == 1){
			LED2_Turn();
			Relay_Turn();
		}
		
		DelayXms(5);
	}
	
#endif
	
}

