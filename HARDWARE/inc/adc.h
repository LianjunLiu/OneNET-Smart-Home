#ifndef __TSENSOR_H
#define __TSENSOR_H	

#include "stm32f10x.h"

void Adc3_Init(void); 				//ADC3初始化
u16  Get_Adc3(u8 ch); 				//获得ADC3某个通道值  

#endif 
