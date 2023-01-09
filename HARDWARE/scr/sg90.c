#include "sg90.h"
#include "stm32f10x_tim.h"
#include "delay.h"

//Ó²¼þÁ¬½Ó
//+ -> 5V
//- -> GND
//TIM1 CH1 PWM -> PA8

//TIM3 PWM
//arr:2000-1
//psc:7200-1

void servo_init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 |RCC_APB2Periph_GPIOA, ENABLE);
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM1_CH1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler =psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1,ENABLE);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

void servo_on_45(void)
{
	TIM_SetCompare1(TIM1,1900);
	DelayMs(1000);
}

void servo_on_90(void)
{
	TIM_SetCompare1(TIM1,1850);
	DelayMs(1000);
}

void servo_on_135(void)
{
	TIM_SetCompare1(TIM1,1800);
	DelayMs(1000);
}

void servo_on_180(void)
{
	TIM_SetCompare1(TIM1,1750);
	DelayMs(1000);
}

void servo_off(void)
{
	TIM_SetCompare1(TIM1,1950);
	DelayMs(1000);
}
