#include "stm32f10x.h"                  // Device header
#include "SHT31.h"
#include "uart.h"
#include "delay.h"
//ʹ��I2C1����ͨ��
void I2C1_init(void)
{

	GPIO_InitTypeDef gpioInitStruct;
	I2C_InitTypeDef I2C1_InitStruct;
	
	//ʹ��ʱ��
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
	/* ��ʼ��I2C1 */
	I2C_Init(I2C1, &I2C1_InitStruct);
 
	/* ʹ��I2C1 */
	I2C_Cmd(I2C1, ENABLE);
	
	//����ackӦ��ģʽ
	I2C_AcknowledgeConfig(I2C1, ENABLE);

}

void GetDataFromSHT31(float *Tmp_true, float *Rh_true)
{
	u16 Reserved_data[6]; //0 1Ϊ�¶� 2ΪcrcУ�� 34Ϊʪ��5Ϊcrc
	uint8_t crc_tmp,crc_rh;//crc���
	
	//I2c��������
	I2C_CommandWrite(HIGH_ENABLED_CMD);
	//I2C��������
	I2C_Read(Reserved_data, 6);
	
	//�����ֲ��й�ʽ������ʪ������
	I2C1_SHT31_Get_RH_And_Tmp(Reserved_data, Tmp_true, Rh_true);
	
	//u8 test[] = {0xBE,0xEF}; crc�������� ���Ϊ0x92
	//crcУ��
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

//��SHT31����һ��ָ��
void SHT31_send_cmd(SHT31_CMD cmd)
{
	u8 cmd1 = cmd;     //��ȡ��8λ
	u8 cmd2 = cmd >> 8; //��ȡ��8λ
	
	//UsartPrintf(USART_DEBUG, "cmd1-->%d, cmd2-->%d\r\n",cmd1, cmd2);
	
	//����Ҫд�������  �ȷ�������ĸ߰�λ �ڷ��Ͱ�λ
    I2C_SendData(I2C1, cmd2);
    //���EV8_2�¼�
    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
	I2C_SendData(I2C1, cmd1);
    //���EV8_2�¼�
    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
}

//��I2C EEPROMоƬ ��дһ������
void I2C_CommandWrite(SHT31_CMD cmd)
{
	//������ʼ�ź�
	I2C_GenerateSTART(I2C1, ENABLE);
	//���EV5�¼�
    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	
	//�����豸��ַ
    I2C_Send7bitAddress(I2C1, SHT31_ADDR_WRITE, I2C_Direction_Transmitter);  //�ӿ��ڽ�adress д��ʱ��bit0��λΪ0�� ��1 ���Կ���ֱ��ֻ���豸��ַ
	//���EV6�¼�
    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	
	//����ָ��
	SHT31_send_cmd(cmd);
	
	//����ֹͣ�ź�
    I2C_GenerateSTOP(I2C1, ENABLE);
}


//��I2C ����
void I2C_Read(u16* pBuffer, u8 numByteToRead) 
{	
    //����start�ź�
    I2C_GenerateSTART(I2C1, ENABLE);
    //���EV5�¼�
    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	 
    //�����豸��ַ
    I2C_Send7bitAddress(I2C1, SHT31_ADDR_READ, I2C_Direction_Receiver);
    //���EV6�¼�
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
			
    //����ֹͣ�ź�
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
 * crc8У�麯����dataΪҪУ������ݣ�lenΪҪУ������ݵ��ֽ���
 */
uint8_t crc8(const uint8_t *data, int len)
{
	//UsartPrintf(USART_DEBUG, "crc_DATA--%d....\r\n",*data);
	//UsartPrintf(USART_DEBUG, "crc_DATA--%d....\r\n",*(data + 1));
	
     const uint8_t POLYNOMIAL = 0x31; //x8+x5+x4+1-->1 0011 0001 ȡ��ߴ��ݼ�һλ 110001-��31
     uint8_t crc = 0xFF;//��ʼֵcrc�Ĵ���
     int i, j;
   
	 for (i=0; i<len; ++i) 
	 {
     	crc ^= *data++; //�����ֽ��������
 
     	for (j=0; j<8; ++j) //bit�������  ���MSBΪ1 ����һλȻ�������ʽ  ���Ϊ0���������
		{
        	crc = ( crc & 0x80 )? (crc << 1) ^ POLYNOMIAL: (crc << 1);
       	}
  	 }
	
	 return crc;
}

