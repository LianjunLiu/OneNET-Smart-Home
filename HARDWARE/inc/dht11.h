#ifndef __DHT11_H
#define __DHT11_H 
 
#define DHT11_IO_IN()  {GPIOA->CRL &= 0XF0FFFFFF;GPIOA->CRL |= 8<<4*6;}       //PA6控制DHT11，设置为输出
#define DHT11_IO_OUT() {GPIOA->CRL &= 0XF0FFFFFF;GPIOA->CRL |= 3<<4*6;}       //PA6控制DHT11，设置为输入
										   
#define DHT11_OUT(x)    GPIO_WriteBit(GPIOA, GPIO_Pin_6, (BitAction)x)        //PA6控制DHT11
#define DHT11_DQ_IN     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)              //PA6控制DHT11	
   
void DHT11_Rst(void);
char DHT11_Check(void);
char DHT11_Read_Bit(void);
char DHT11_Read_Byte(void);
char DHT11_Read_Data(char *temp, char *humi);
char DHT11_Init(void);

#endif
