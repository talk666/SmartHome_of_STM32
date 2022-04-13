#ifndef _IWDG_H_
#define _IWDG_H_

//独立看门狗
void IWDG_Init(u8 prer, u16 rlr);
void Feed_Dog(void);

//窗口看门狗 待实现
#endif
