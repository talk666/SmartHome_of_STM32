#ifndef _ADC_H_
#define _ADC_H_

void ADC1_Init(void);
u16 Get_Adc(u8 ch) ;
u16 Get_Adc_Average(u8 ch,u8 times);

#endif
