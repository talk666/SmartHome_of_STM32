#ifndef _ESP8266_H_
#define _ESP8266_H_


#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

//链接wifi
#if 1
	#define ESP8266_WIFI_INFO		"AT+CWJAP=\"ChinaNet-hvCg\",\"7wjuppeh\"\r\n"
#else
	#define ESP8266_WIFI_INFO		"AT+CWJAP=\"Mi10\",\"zxcvbnm123\"\r\n"
#endif
#define ESP8266_ALIYUN_INFO		"AT+CIPSTART=\"TCP\",\"h0njatWd7OS.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883\r\n"


extern unsigned char esp8266_buf[128];
extern unsigned short esp8266_cnt, esp8266_cntPre;

void ESP8266_Init(void);

void ESP8266_Clear(void);

void ESP8266_SendData(unsigned char *data, unsigned short len);

unsigned char *ESP8266_GetIPD(unsigned short timeOut);


#endif
