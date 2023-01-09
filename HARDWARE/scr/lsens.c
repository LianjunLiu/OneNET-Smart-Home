#include "lsens.h"
#include "delay.h"
 
//Ӳ������
//���������� -> PF8

//��ʼ������������
void Lsens_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);//ʹ��PORTFʱ��	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;        //PF8 anolog����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	Adc3_Init();
}

//��ȡLight Sens��ֵ
//0~100:0,�;100,���� 
u8 Lsens_Get_Val(void)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<LSENS_READ_TIMES;t++)
	{
		temp_val+=Get_Adc3(LSENS_ADC_CHX);	//��ȡADCֵ
		DelayMs(5);
	}
	temp_val/=LSENS_READ_TIMES;//�õ�ƽ��ֵ
	
	if(temp_val>4000)temp_val=4000;
	
	return (u8)(100-(temp_val/40));
}