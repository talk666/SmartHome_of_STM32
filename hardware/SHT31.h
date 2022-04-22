#ifndef __SHT31_H__
#define __SHT31_H__


#define SCL     GPIO_Pin_6
#define SDA     GPIO_Pin_7


#define SHT_ADDR          (0x44)    //0100 0100
#define SHT31_ADDR_WRITE  (SHT_ADDR<<1)    //1000 1000  ����λ��ʾSHT31�豸��ַ  ���λ��ʾ��д 0д 1��
#define SHT31_ADDR_READ  ((SHT_ADDR<<1)+1) // 1000 1001  


typedef enum
{
    /* �����λ���� */

    SOFT_RESET_CMD = 0x30A2,    
    /*
    ���β���ģʽ
    ������ʽ��Repeatability_CS_CMD (�����ظ��� Ӱ�����ʱ�������)
    CS��Clock stretching���Ƿ�ʹ��ʱ�ӣ�
	�ظ���_ʹ��
    */
    HIGH_ENABLED_CMD    = 0x2C06,
    MEDIUM_ENABLED_CMD  = 0x2C0D,
    LOW_ENABLED_CMD     = 0x2C10,
    HIGH_DISABLED_CMD   = 0x2400,
    MEDIUM_DISABLED_CMD = 0x240B,
    LOW_DISABLED_CMD    = 0x2416,

    /*
    ���ڲ���ģʽ���� ���ظ��ԣ����иߣ�_���ݲɼ�Ƶ�ʣ�
    ������ʽ��Repeatability_MPS_CMD
    MPS��measurement per second
    */
    HIGH_0_5_CMD   = 0x2032, //����ɼ�һ��
    MEDIUM_0_5_CMD = 0x2024, 
    LOW_0_5_CMD    = 0x202F,
    HIGH_1_CMD     = 0x2130,  //һ��ɼ�һ��
    MEDIUM_1_CMD   = 0x2126,
    LOW_1_CMD      = 0x212D,
    HIGH_2_CMD     = 0x2236,
    MEDIUM_2_CMD   = 0x2220,
    LOW_2_CMD      = 0x222B,
    HIGH_4_CMD     = 0x2334,
    MEDIUM_4_CMD   = 0x2322,
    LOW_4_CMD      = 0x2329,
    HIGH_10_CMD    = 0x2737,
    MEDIUM_10_CMD  = 0x2721,
    LOW_10_CMD     = 0x272A,
	
    /* ���ڲ���ģʽ��ȡ�������� */
    READOUT_FOR_PERIODIC_MODE = 0xE000,
	
	/*�ж�ֹͣ�����Բ�������*/
	BREAK          = 0x3093,
	
	/*��������������*/
	HEATER_ENABLE = 0x306D,
	HEATER_ABLE = 0x3066,
	
	/*��ȡ�Ĵ���״̬����*/
	READ_OUTOF_STATUS_REGISTER = 0xF32D,
	
	/*�Ĵ����������� */
	CLEAR_STATUS_REGISTER = 0x3041
} SHT31_CMD;


void I2C1_init(void);
void SHT31_send_cmd(SHT31_CMD cmd);
void I2C_Read(u16* pBuffer, u8 numByteToRead);
void I2C_CommandWrite(SHT31_CMD cmd);
void I2C1_SHT31_Get_RH_And_Tmp(u16 *reserved_date, float *Tmp_read, float *Rh_read);
uint8_t crc8(const uint8_t *data, int len);
void GetDataFromSHT31(float *Tmp_true, float *Rh_true);
#endif
