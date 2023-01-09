#include "timer8.h"
#include "stm32f10x_tim.h"

//��ʱ��8ʹ�ܶ�ʱ
//10s -> ARR:20000-1; PSC:36000-1
void TIM8_ENABLE(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;             //����һ�����ö�ʱ���ı���
	NVIC_InitTypeDef NVIC_InitStructure;                           //����һ�������жϵı���	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                //�����ж��������飺��2�� �������ȼ���0 1 2 3 �����ȼ���0 1 2 3		
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);           //ʹ��TIM8ʱ��	
	TIM_DeInit(TIM8);                                              //��ʱ��2�Ĵ����ָ�Ĭ��ֵ	
	TIM_TimeBaseInitStructure.TIM_Period = 5000-1; 	               //�����Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36000-1;             //���ö�ʱ��Ԥ��Ƶ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //1��Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStructure);            //����TIM8
	
	TIM_ClearITPendingBit(TIM8, TIM_IT_Update);                    //�������жϱ�־λ
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);                     //ʹ��TIM8����ж�    
	TIM_Cmd(TIM8, ENABLE);                                         //��TIM8
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;             //����TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;      //��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);                                //�����ж�
}
