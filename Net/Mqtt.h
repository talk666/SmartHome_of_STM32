#ifndef _ONENET_H_
#define _ONENET_H_

//连接阿里云三元组
#define MQTT_CLIENTID		"FESA234FBDS24|securemode=3,signmethod=hmacsha1,timestamp=789|"

#define MQTT_USARNAME		"Transfor_test&h0njatWd7OS"

#define MQTT_PASSWD     	"caf22e3a8f6356631c15b87bf1071def9defe9fb"

//发布订阅主题
 //阿里云模拟物理设备订阅的Topic 用这个Topic可以控制阿里云模拟物理设备
#define	MQTT_PUBLISH_TOPIC   "/sys/h0njatWd7OS/Transfor_test/thing/event/property/post"
//自定义用于微信小程序通讯Topic
#define STM32ToALiYunTopic  "/h0njatWd7OS/Transfor_test/user/Cap_Control"



_Bool ALiYun_DevLink(void);

void ALiYun_Subscribe(const char *topics[], unsigned char topic_cnt);

void ALiYun_Publish(const char *topic, const char *msg);

void ALiYun_RevPro(unsigned char *cmd);

#endif
