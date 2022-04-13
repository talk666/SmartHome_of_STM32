#ifndef __TOUCH_H_
#define __TOUCH_H_

#include "stm32f10x.h"                 // Device header
#include "stdio.h"

void Touch_Init(void);
uint8_t Touch_GetStatus(void);
void LED2_Turn(void);

#endif
