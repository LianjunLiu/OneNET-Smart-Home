#include "beep.h"
#include "delay.h"

//硬件连接
//BEEP -> PB8

//BEEP初始化函数
void BEEP_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOB, &GPIO_InitStructure);	 
 
 GPIO_ResetBits(GPIOB,GPIO_Pin_8);//输出0，关闭蜂鸣器输出
}

//报警函数
void Warning(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	DelayMs(250);
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	DelayMs(250);
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	DelayMs(250);
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	DelayMs(250);
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	DelayMs(250);
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	DelayMs(250);
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	DelayMs(250);
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	DelayMs(250);
}
