#include "stm32f10x.h"  
#include "usart1.h"     

//Ӳ������
//TX -> PA9
//RX -> PA10

//��ʼ������1���ͽ��չ���
void Usart1_Init(unsigned int bound)
{  	 	
  GPIO_InitTypeDef GPIO_InitStructure;     //����һ������GPIO���ܵı���
	USART_InitTypeDef USART_InitStructure;   //����һ�����ô��ڹ��ܵı���
   
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //ʹ�ܴ���1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIOAʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;              //׼������PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO����50M
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       //����������������ڴ���1�ķ���
  GPIO_Init(GPIOA, &GPIO_InitStructure);                 //����PA9
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;             //׼������PA10 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //�������룬���ڴ���1�Ľ���
  GPIO_Init(GPIOA, &GPIO_InitStructure);                 //����PA10
	
	USART_InitStructure.USART_BaudRate = bound;                                      //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                      //8������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                           //1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                              //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //��Ӳ������������
                                                                                   //�����ʹ�ܽ���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Tx ;	                               //ֻ��ģʽ        
  USART_Init(USART1, &USART_InitStructure);                                        //���ô���1	
	USART_Cmd(USART1, ENABLE);                            						               //ʹ�ܴ���1
}

//printf�ض���
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40) == 0);//ѭ������,ֱ���������   
    USART1->DR = (u8)ch;      
	return ch;
}
#endif 

//����1 printf����
__align(8) char Usart1_TxBuff[USART1_TXBUFF_SIZE];
void u1_printf(char * fmt, ...) 
{  
	unsigned int i, length;
	
	va_list ap;
	va_start(ap, fmt);
	vsprintf(Usart1_TxBuff, fmt, ap);
	va_end(ap);	
	
	length = strlen((const char*)Usart1_TxBuff);		
	while((USART1->SR&0X40) == 0);
	for(i = 0; i < length; i++)
	{			
		USART1->DR = Usart1_TxBuff[i];
		while((USART1->SR&0X40) == 0);	
	}	
}
