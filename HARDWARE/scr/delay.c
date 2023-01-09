#include "delay.h"

static u8  fac_us = 0;						//us��ʱ������			   
static u16 fac_ms = 0;						//ms��ʱ������

//����������ʱ��ʼ��
void Delay_Init()
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	
	fac_us = SystemCoreClock/8000000;				 
	fac_ms = (u16)fac_us*1000;					
}	 

//��������us��ʱ
void DelayUs(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD = nus*fac_us; 			         //ʱ�����	  		 
	SysTick->VAL = 0x00;        			         //��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ; //��ʼ����	  
	
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01) && !(temp&(1<<16)));	   //�ȴ�ʱ�䵽�� 
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; //�رռ�����
	SysTick->VAL = 0X00;      				         //��ռ�����	 
}

//��������ms��ʱ
void DelayMs(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD = (u32)nms*fac_ms;		       	//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL = 0x00;						            //��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;	//��ʼ����  
	
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01) && !(temp&(1<<16)));		  //�ȴ�ʱ�䵽�� 
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
	SysTick->VAL = 0X00;       				        	//��ռ�����	  	    
}