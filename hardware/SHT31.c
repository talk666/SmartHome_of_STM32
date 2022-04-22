#include "stm32f10x.h"                  // Device header
#include "SHT31.h"
#include "uart.h"
#include "delay.h"
//使用I2C1进行通信
void I2C1_init(void)
{

	GPIO_InitTypeDef gpioInitStruct;
	I2C_InitTypeDef I2C1_InitStruct;
	
	//使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	//gpio
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	gpioInitStruct.GPIO_Pin = SCL | SDA;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInitStruct);
	
	//I2C
	I2C1_InitStruct.I2C_Ack = I2C_Ack_Enable ;
	I2C1_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C1_InitStruct.I2C_ClockSpeed = 400000;
	I2C1_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C1_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C1_InitStruct.I2C_OwnAddress1 = 0x0a;
	/* 初始化I2C1 */
	I2C_Init(I2C1, &I2C1_InitStruct);
 
	/* 使能I2C1 */
	I2C_Cmd(I2C1, ENABLE);
	
	//开启ack应答模式
	I2C_AcknowledgeConfig(I2C1, ENABLE);

}

void GetDataFromSHT31(float *Tmp_true, float *Rh_true)
{
	u16 Reserved_data[6]; //0 1为温度 2为crc校验 34为湿度5为crc
	uint8_t crc_tmp,crc_rh;//crc结果
	
	//I2c发送数据
	I2C_CommandWrite(HIGH_ENABLED_CMD);
	//I2C接收数据
	I2C_Read(Reserved_data, 6);
	
	//按照手册中公式进行温湿度运算
	I2C1_SHT31_Get_RH_And_Tmp(Reserved_data, Tmp_true, Rh_true);
	
	//u8 test[] = {0xBE,0xEF}; crc测试数据 结果为0x92
	//crc校验
	u8 Tmp_data[] = {Reserved_data[0], Reserved_data[1]};
	u8 Rh_data[] = {Reserved_data[3], Reserved_data[4]};
	
	crc_tmp = crc8(Tmp_data, 2);
	crc_rh = crc8(Rh_data, 2);
	if(crc_tmp != Reserved_data[2] || crc_rh != Reserved_data[5])
	{
		UsartPrintf(USART_DEBUG, "crc error!!crc_tmp-verify[%d-%d],crc_rh-verify[%d-%d] \r\n",crc_tmp,Reserved_data[2],crc_rh,Reserved_data[5]);
	}else{
		UsartPrintf(USART_DEBUG, "crc verify OK==>");
	}
	
}

//向SHT31发送一条指令
void SHT31_send_cmd(SHT31_CMD cmd)
{
	u8 cmd1 = cmd;     //获取低8位
	u8 cmd2 = cmd >> 8; //获取高8位
	
	//UsartPrintf(USART_DEBUG, "cmd1-->%d, cmd2-->%d\r\n",cmd1, cmd2);
	
	//发送要写入的数据  先发送命令的高八位 在发低八位
    I2C_SendData(I2C1, cmd2);
    //检测EV8_2事件
    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
	I2C_SendData(I2C1, cmd1);
    //检测EV8_2事件
    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
}

//往I2C EEPROM芯片 中写一个命令
void I2C_CommandWrite(SHT31_CMD cmd)
{
	//产生起始信号
	I2C_GenerateSTART(I2C1, ENABLE);
	//检测EV5事件
    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	
	//发送设备地址
    I2C_Send7bitAddress(I2C1, SHT31_ADDR_WRITE, I2C_Direction_Transmitter);  //接口内将adress 写的时候bit0置位为0了 读1 所以可以直接只传设备地址
	//检测EV6事件
    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	
	//发送指令
	SHT31_send_cmd(cmd);
	
	//产生停止信号
    I2C_GenerateSTOP(I2C1, ENABLE);
}


//读I2C 数据
void I2C_Read(u16* pBuffer, u8 numByteToRead) 
{	
    //产生start信号
    I2C_GenerateSTART(I2C1, ENABLE);
    //检测EV5事件
    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	 
    //发送设备地址
    I2C_Send7bitAddress(I2C1, SHT31_ADDR_READ, I2C_Direction_Receiver);
    //检测EV6事件
    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR);
	
	
    while(numByteToRead) //2 byte Tmp -- 1byte crc -- 2 byte RH -- 1byte crc
    {
        if(numByteToRead == 1)
		{
			//I2C_AcknowledgeConfig(I2C1, DISABLE);
		}
			
		while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR){};
			*pBuffer = I2C_ReceiveData(I2C1);
			//UsartPrintf(USART_DEBUG, "SHT31 reserved pBuffer--%d,index--%d......\r\n",*pBuffer,numByteToRead);
			
			pBuffer++;
					
			numByteToRead--;
	}		
			
    //产生停止信号
    I2C_GenerateSTOP(I2C1, ENABLE);
 
}

void I2C1_SHT31_Get_RH_And_Tmp(u16 *reserved_date, float *Tmp_read, float *Rh_read)
{
	u16 Tmp,Rh;
	float Tmp_true = 0.0,Rh_true = 0.0;
	
	Tmp = *(reserved_date + 1)|((*reserved_date) << 8);
	Tmp_true = (-45.00) + (175.00 * Tmp)/65535.00;
	
	Rh = *(reserved_date + 4)|(*(reserved_date + 3) << 8); 
	Rh_true = (100.00 * Rh)/65535.00;
	
	*Tmp_read = Tmp_true;
	*Rh_read = Rh_true;
	//UsartPrintf(USART_DEBUG, "RH--%d..RH_TRUE--%f....\r\n",Rh,*Rh_read);
	//UsartPrintf(USART_DEBUG, "Tmp--%d..Tmp_true--%f....\r\n",Tmp,*Tmp_read);
		
}

/*
 * crc8校验函数，data为要校验的数据，len为要校验的数据的字节数
 */
uint8_t crc8(const uint8_t *data, int len)
{
	//UsartPrintf(USART_DEBUG, "crc_DATA--%d....\r\n",*data);
	//UsartPrintf(USART_DEBUG, "crc_DATA--%d....\r\n",*(data + 1));
	
     const uint8_t POLYNOMIAL = 0x31; //x8+x5+x4+1-->1 0011 0001 取最高次幂减一位 110001-》31
     uint8_t crc = 0xFF;//初始值crc寄存器
     int i, j;
   
	 for (i=0; i<len; ++i) 
	 {
     	crc ^= *data++; //依次字节异或运算
 
     	for (j=0; j<8; ++j) //bit异或运算  如果MSB为1 左移一位然后异或表达式  如果为0则继续运算
		{
        	crc = ( crc & 0x80 )? (crc << 1) ^ POLYNOMIAL: (crc << 1);
       	}
  	 }
	
	 return crc;
}

