#include "lsens.h"
#include "delay.h"
 
//硬件连接
//光敏传感器 -> PF8

//初始化光敏传感器
void Lsens_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);//使能PORTF时钟	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;        //PF8 anolog输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	Adc3_Init();
}

//读取Light Sens的值
//0~100:0,最暗;100,最亮 
u8 Lsens_Get_Val(void)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<LSENS_READ_TIMES;t++)
	{
		temp_val+=Get_Adc3(LSENS_ADC_CHX);	//读取ADC值
		DelayMs(5);
	}
	temp_val/=LSENS_READ_TIMES;//得到平均值
	
	if(temp_val>4000)temp_val=4000;
	
	return (u8)(100-(temp_val/40));
}
