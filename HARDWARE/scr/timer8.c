#include "timer8.h"
#include "stm32f10x_tim.h"

//定时器8使能定时
//10s -> ARR:20000-1; PSC:36000-1
void TIM8_ENABLE(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;             //定义一个设置定时器的变量
	NVIC_InitTypeDef NVIC_InitStructure;                           //定义一个设置中断的变量	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                //设置中断向量分组：第2组 抢先优先级：0 1 2 3 子优先级：0 1 2 3		
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);           //使能TIM8时钟	
	TIM_DeInit(TIM8);                                              //定时器2寄存器恢复默认值	
	TIM_TimeBaseInitStructure.TIM_Period = 5000-1; 	               //设置自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36000-1;             //设置定时器预分频数
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //1分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStructure);            //设置TIM8
	
	TIM_ClearITPendingBit(TIM8, TIM_IT_Update);                    //清除溢出中断标志位
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);                     //使能TIM8溢出中断    
	TIM_Cmd(TIM8, ENABLE);                                         //开TIM8
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;             //设置TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;      //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //中断通道使能
	NVIC_Init(&NVIC_InitStructure);                                //设置中断
}
