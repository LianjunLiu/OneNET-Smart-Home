#ifndef __LED_H
#define __LED_H

#define		LED0_ON()						GPIO_ResetBits(GPIOB , GPIO_Pin_5); 			//PB5 ����͵�ƽ��ʹ�䷢��
#define		LED0_OFF()					GPIO_SetBits(GPIOB , GPIO_Pin_5); 			  //PB5 ����ߵ�ƽ��ʹ����

#define		LED1_ON()						GPIO_ResetBits(GPIOE , GPIO_Pin_5); 			//PE5 ����͵�ƽ��ʹ�䷢��
#define		LED1_OFF()					GPIO_SetBits(GPIOE , GPIO_Pin_5); 			  //PE5 ����ߵ�ƽ��ʹ����

void LED_Init(void);

#endif
