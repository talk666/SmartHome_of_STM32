
//单片机头文件
#include "stm32f10x.h"
#include "IWDG.h"

/************************************************************
*	函数名称：	IWDG_Init
*
*	函数功能：	独立看门狗初始化
*
*	入口参数：	u8 prer, u16 rlr (预分频值和重装载值)
*
*	返回参数：	无
*
*	说明：	初始化独立看门狗   计数减去一因为从0开始计数
* prer:分频数:0~7(只有低 3 位有效!)
* 分频因子=4*2^prer.但最大值只能是 256!
* rlr:重装载寄存器值:低 11 位有效.
* 时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms).
*IWDG_Init(4,625);//初始化独立看门狗 40kHZ，分频数为64，重装载值为625，溢出时间计算为：64*625/40=1000ms=1s	
************************************************************
*/
void IWDG_Init(u8 prer, u16 rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(prer);//设置IWDG预分频值
	IWDG_SetReload(rlr - 1);//设置IWDG重装栽值
	IWDG_ReloadCounter();//按照IWDG重载寄存器的值重装载IWDG计数器
	IWDG_Enable();//使能
}

/************************************************************
*	函数名称：	Feed_Dog
*
*	函数功能：	喂狗
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Feed_Dog(void)
{
	IWDG_ReloadCounter();
}