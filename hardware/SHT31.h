#ifndef __SHT31_H__
#define __SHT31_H__


#define SCL     GPIO_Pin_6
#define SDA     GPIO_Pin_7


#define SHT_ADDR          (0x44)    //0100 0100
#define SHT31_ADDR_WRITE  (SHT_ADDR<<1)    //1000 1000  低七位表示SHT31设备地址  最高位表示读写 0写 1读
#define SHT31_ADDR_READ  ((SHT_ADDR<<1)+1) // 1000 1001  


typedef enum
{
    /* 软件复位命令 */

    SOFT_RESET_CMD = 0x30A2,    
    /*
    单次测量模式
    命名格式：Repeatability_CS_CMD (测试重复性 影响测量时间和性能)
    CS：Clock stretching（是否使能时钟）
	重复性_使能
    */
    HIGH_ENABLED_CMD    = 0x2C06,
    MEDIUM_ENABLED_CMD  = 0x2C0D,
    LOW_ENABLED_CMD     = 0x2C10,
    HIGH_DISABLED_CMD   = 0x2400,
    MEDIUM_DISABLED_CMD = 0x240B,
    LOW_DISABLED_CMD    = 0x2416,

    /*
    周期测量模式命令 （重复性（低中高）_数据采集频率）
    命名格式：Repeatability_MPS_CMD
    MPS：measurement per second
    */
    HIGH_0_5_CMD   = 0x2032, //两秒采集一次
    MEDIUM_0_5_CMD = 0x2024, 
    LOW_0_5_CMD    = 0x202F,
    HIGH_1_CMD     = 0x2130,  //一秒采集一次
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
	
    /* 周期测量模式读取数据命令 */
    READOUT_FOR_PERIODIC_MODE = 0xE000,
	
	/*中断停止周期性测试命令*/
	BREAK          = 0x3093,
	
	/*加热器操作命令*/
	HEATER_ENABLE = 0x306D,
	HEATER_ABLE = 0x3066,
	
	/*读取寄存器状态命令*/
	READ_OUTOF_STATUS_REGISTER = 0xF32D,
	
	/*寄存器部分清零 */
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
