#include "stm32f10x.h"
#include "dht11.h"
#include "delay.h"
#include "usart1.h"

//Ӳ������
//+ -> 5V
//- -> GND
//DATA -> PA6

//��λDHT11
void DHT11_Rst(void)
{                 
  DHT11_IO_OUT(); 	//����IO���ģʽ
  DHT11_OUT(0); 	  //����IO
  DelayMs(30);      //��������18ms����������30
  DHT11_OUT(1); 	  //����IO
  DelayUs(30);      //��������20~40us����������30us
}

//�ȴ�DHT11�Ļ�Ӧ
char DHT11_Check(void)
{   
	char timeout;                            //����һ���������ڳ�ʱ�ж�  
	timeout = 0;                             //��ʱ��������    
	DHT11_IO_IN();                           //IO��������ģʽ
	
  while((DHT11_DQ_IN == 1) && (timeout < 70))		//DHT11������40~50us,���ǵȴ�70us��ʱʱ��	
	{	 
		timeout++;		//��ʱ����+1
		DelayUs(1);		//��ʱ1us
	} 
	if(timeout >= 70)                             //���timeout>=70,˵������Ϊ��ʱ�˳���whileѭ��������1��ʾ����
	{
		return 1;
	}		
	else                                          //��֮��˵������Ϊ�ȵ���DHT11����IO���˳���whileѭ������ȷ������timeout
	{
		timeout = 0;
	}
  while((DHT11_DQ_IN == 0) && (timeout < 70))		//DHT11���ͺ���ٴ�����40~50us,,���ǵȴ�70us��ʱʱ��	
	{ 		
		timeout++;		//��ʱ����+1
		DelayUs(1);		//��ʱ1us
	}
	if(timeout >= 70)                             //���timeout>=70,˵������Ϊ��ʱ�˳���whileѭ��������2��ʾ���� 
	{
		return 1;
	}
	return 0;                                     //��֮��ȷ������0
}

//��ȡһ��λ
char DHT11_Read_Bit(void)
{
 	char timeout;                          	   //����һ���������ڳ�ʱ�ж�  
	timeout = 0;                               //����timeout
	
	while((DHT11_DQ_IN == 1) && (timeout < 40))//ÿһλ���ݿ�ʼ����12~14us�ĸߵ�ƽ�����ǵ�40us
	{   
		timeout++;                               //��ʱ����+1
		DelayUs(1);                              //��ʱ1us
	}
	timeout = 0;                               //����timeout	
	while((DHT11_DQ_IN == 0) && (timeout < 60))//��������DHT11������IO���������͵�ʱ���ж���0��1�����ǵ�60us
	{
		timeout++;                               //��ʱ����+1
		DelayUs(1);                              //��ʱ1us
	}
	DelayUs(35);                               //��ʱ35us
	if(DHT11_DQ_IN)                            //�����ʱ���Ǹߵ�ƽ����ô��λ���յ���1������1
	{
		return 1;
	}
	else          		                         //��֮��ʱ���ǵ͵�ƽ����ô��λ���յ���0������0
	{
		return 0;
	}
}

//��ȡһ���ֽ�
char DHT11_Read_Byte(void)
{        
  char i;                       				//����һ����������forѭ��  
	char data;                            //����һ���������ڱ������� 
	data = 0;	                        		//����������ݵı���
	for(i = 0; i < 8; i++)                //һ���ֽ�8λ��ѭ��8��
  {	
   	data <<= 1;                    		  //����һλ���ڳ���λ    
	  data |= DHT11_Read_Bit();      		  //��ȡһλ����
  }						    
  return data;                   			  //����һ���ֽڵ�����
}

//��ȡһ��������ʪ��
char DHT11_Read_Data(char *temp, char *humi)    
{
 	char buf[5];                                         //һ��������������5���ֽڣ�����һ��������
	char i;                                              //����һ����������forѭ��  
	DHT11_Rst();                                         //��λDHT11	
	if(DHT11_Check() == 0)							                 //�ж�DHT11�ظ�״̬=0�Ļ�����ʾ��ȷ������if
	{
		for(i = 0; i < 5; i++)                             //һ��������������5���ֽڣ�ѭ��5��
		{
			buf[i] = DHT11_Read_Byte();                      //ÿ�ζ�ȡһ���ֽ�
		}
		if((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])  //�ж�����У�飬ǰ4���ֽ����Ӧ�õ��ڵ�5���ֽڣ���ȷ�Ļ�������if	
		{     
			*humi = buf[0]; //ʪ�����ݣ�������humiָ��ָ��ĵ�ַ������
			*temp = buf[2]; //�¶����ݣ�������tempָ��ָ��ĵ�ַ������
		}
		else                                               //��֮������У�����ֱ�ӷ���1
		{
			return 1;
		}
	}
	else                                                 //��֮�����DHT11�ظ�״̬=1�Ļ�����ʾ���󣬽���else��ֱ�ӷ���2
	{
		return 2;
	}
	return 0;	                                           //��ȡ��ȷ����0    
}  

//��ʼ��DHT11
char DHT11_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                  //����һ��IO�˿ڲ����ṹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��PA�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;            //׼������PA6
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   	  //���������ʽ
	GPIO_Init(GPIOA, &GPIO_InitStructure);            	  //����PA6	
	DHT11_Rst();                                          //��λDHT11
	return DHT11_Check();                                 //����DHT11�Ļظ�״̬
}
