#include "beep.h"
#include "delay.h"

//Ӳ������
//BEEP -> PB8

//BEEP��ʼ������
void BEEP_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOB, &GPIO_InitStructure);	 
 
 GPIO_ResetBits(GPIOB,GPIO_Pin_8);//���0���رշ��������
}

//��������
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
