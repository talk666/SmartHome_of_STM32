#ifndef _ONENET_H_
#define _ONENET_H_

//���Ӱ�������Ԫ��
#define MQTT_CLIENTID		"FESA234FBDS24|securemode=3,signmethod=hmacsha1,timestamp=789|"

#define MQTT_USARNAME		"Transfor_test&h0njatWd7OS"

#define MQTT_PASSWD     	"caf22e3a8f6356631c15b87bf1071def9defe9fb"

//������������
 //������ģ�������豸���ĵ�Topic �����Topic���Կ��ư�����ģ�������豸
#define	MQTT_PUBLISH_TOPIC   "/sys/h0njatWd7OS/Transfor_test/thing/event/property/post"
//�Զ�������΢��С����ͨѶTopic
#define STM32ToALiYunTopic  "/h0njatWd7OS/Transfor_test/user/Cap_Control"



_Bool ALiYun_DevLink(void);

void ALiYun_Subscribe(const char *topics[], unsigned char topic_cnt);

void ALiYun_Publish(const char *topic, const char *msg);

void ALiYun_RevPro(unsigned char *cmd);

#endif
