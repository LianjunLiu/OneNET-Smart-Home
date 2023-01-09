#ifndef __LED_H
#define __LED_H

#define		LED0_ON()						GPIO_ResetBits(GPIOB , GPIO_Pin_5); 			//PB5 输出低电平，使其发光
#define		LED0_OFF()					GPIO_SetBits(GPIOB , GPIO_Pin_5); 			  //PB5 输出高电平，使其灭

#define		LED1_ON()						GPIO_ResetBits(GPIOE , GPIO_Pin_5); 			//PE5 输出低电平，使其发光
#define		LED1_OFF()					GPIO_SetBits(GPIOE , GPIO_Pin_5); 			  //PE5 输出高电平，使其灭

void LED_Init(void);

#endif
