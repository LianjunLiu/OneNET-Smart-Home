#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "usart1.h"
#include "usart2.h"
#include "timer3.h"
#include "mqtt.h"
#include "dht11.h"
#include "lcd.h"
#include "lsens.h"
#include "led.h"
#include "beep.h"
#include "control.h"

char humidity;				//����һ������������ʪ��ֵ
char temperature;			//����һ�������������¶�ֵ	
extern u8 adcx;       //��Ź�����������ֵ

int Array_Num = 41;    //LCD��ʪ������ͼ�����
int T_Aaary[41] = {0}; //�¶�����
int H_Aaary[41] = {0}; //ʪ������
int L_Aaary[41] = {0}; //��ǿ����
int step = 0;          //LCD��ʪ�����߻��Ʋ���

int temperature_threshold = 30; //�¶ȱ�����ֵ
int humidity_threshold = 50;    //ʪ�ȱ�����ֵ
extern int light_sensor_threshold;   //LED�Զ�������ֵ

extern char *led0Flag;               //LED0״̬
extern char *led1Flag;               //LED1״̬
extern int   tpad_mode_control;      //��������ģʽ����״̬

extern int   Num;                    //�������Ƕ�

//����2�����жϺ���������������ݣ�
void USART2_IRQHandler(void)   
{                      
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)    //���USART_IT_RXNE��־��λ����ʾ�����ݵ��ˣ�����if��֧
	{  
		if(connectFlag == 0) 							                     //���connectFlag����0����ǰ��û�����ӷ�����������ָ������״̬
		{
			if(USART2->DR)
			{                                     			         //����ָ������״̬ʱ������ֵ�ű��浽������	
				Usart2_RxBuff[Usart2_RxCounter] = USART2->DR;      //���浽������	
				Usart2_RxCounter++; 					                   	 //ÿ����1���ֽڵ����ݣ�Usart2_RxCounter��1����ʾ���յ���������+1 
			}					
		}
		else
		{		                                           	       //��֮connectFlag����1�������Ϸ�������	
			Usart2_RxBuff[Usart2_RxCounter] = USART2->DR;        //�ѽ��յ������ݱ��浽Usart2_RxBuff��				
			if(Usart2_RxCounter == 0)
			{    									    										     	 //���Usart2_RxCounter����0����ʾ�ǽ��յĵ�1�����ݣ�����if��֧				
				TIM_Cmd(TIM4, ENABLE); 
			}
			else																					    	 //else��֧����ʾ��Usart2_RxCounter������0�����ǽ��յĵ�һ������
			{                        									    
				TIM_SetCounter(TIM4, 0);  
			}
			Usart2_RxCounter++;         				                 //ÿ����1���ֽڵ����ݣ�Usart2_RxCounter��1����ʾ���յ���������+1 
		}
	}
} 

//��ʱ��4�жϷ�����,����MQTT����
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//���TIM_IT_Update��λ����ʾTIM4����жϣ�����if	
	{
		memcpy(&MQTT_RxDataInPtr[2], Usart2_RxBuff, Usart2_RxCounter);  //�������ݵ����ջ�����
		MQTT_RxDataInPtr[0] = Usart2_RxCounter/256;                   	//��¼���ݳ��ȸ��ֽ�
		MQTT_RxDataInPtr[1] = Usart2_RxCounter%256;					          	//��¼���ݳ��ȵ��ֽ�
		MQTT_RxDataInPtr += RBUFF_UNIT;                                	//ָ������
		if(MQTT_RxDataInPtr == MQTT_RxDataEndPtr)                     	//���ָ�뵽������β����
			MQTT_RxDataInPtr = MQTT_RxDataBuf[0];                         //ָ���λ����������ͷ
		Usart2_RxCounter = 0;                                         	//����2������������������
		TIM_SetCounter(TIM3, 0);                                      	//���㶨ʱ��3�����������¼�ʱping������ʱ��
		TIM_Cmd(TIM4, DISABLE);                        				        	//�ر�TIM4��ʱ��
		TIM_SetCounter(TIM4, 0);                        			        	//���㶨ʱ��4������
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     			        	//���TIM4����жϱ�־ 	
	}
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���TIM_IT_Update��λ����ʾTIM3����жϣ�����if	
		{  
		switch(pingFlag) 					                      //�ж�pingFlag��״̬
		{                               
			case 0:							                          //���pingFlag����0����ʾ����״̬������Ping����  
					MQTT_PingREQ(); 		                      //���Ping���ĵ����ͻ�����  
					break;
			case 1:							                          //���pingFlag����1��˵����һ�η��͵���ping���ģ�û���յ��������ظ�������1û�б����Ϊ0�������������쳣������Ҫ��������pingģʽ
					TIM3_ENABLE_2S(); 	                      //���ǽ���ʱ��3����Ϊ2s��ʱ,���ٷ���Ping����
					MQTT_PingREQ();			                      //���Ping���ĵ����ͻ�����  
					break;
			case 2:						                          	//���pingFlag����2��˵����û���յ��������ظ�
					MQTT_PingREQ();			                      //���Ping���ĵ����ͻ�����  
					break;
			case 3:				      										      //���pingFlag����3��˵����û���յ��������ظ�
					MQTT_PingREQ();			                      //���Ping���ĵ����ͻ�����
					break;
			case 4:				     										        //���pingFlag����4��˵����û���յ��������ظ�	
					MQTT_PingREQ();  													//���Ping���ĵ����ͻ����� 
					break;
			case 5:																				//���pingFlag����5��˵�����Ƿ����˶��ping�����޻ظ���Ӧ�������������⣬������������
					connectFlag = 0;     									    //����״̬��0����ʾ�Ͽ���û���Ϸ�����
					TIM_Cmd(TIM3, DISABLE); 									//��TIM3 				
					break;			
		}
		pingFlag++;           		   										//pingFlag����1����ʾ�ַ�����һ��ping���ڴ��������Ļظ�
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     //���TIM3����жϱ�־ 	
	}
}

//��ʱ��2�жϷ�����
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)	
	{
		DHT11_Read_Data(&temperature,&humidity);//��ȡ��ʪ��ֵ
		
		if(temperature<=temperature_threshold)//�ж��¶��Ƿ񳬹���ֵ
		{
			POINT_COLOR=GREEN;LCD_ShowNum(180,270+30,temperature,2,24);	  //LCD��ʾ�¶�
		}
		else
		{
			POINT_COLOR=RED;LCD_ShowNum(180,270+30,temperature,2,24);	    //LCD��ʾ�¶�
		}
		
		if(humidity<=humidity_threshold)//�ж�ʪ���Ƿ񳬹���ֵ
		{
			POINT_COLOR=GREEN;LCD_ShowNum(140,300+30,humidity,2,24);		  //LCD��ʾʪ��
		}
		else
		{
			POINT_COLOR=RED;LCD_ShowNum(140,300+30,humidity,2,24);		    //LCD��ʾʪ��
		}
		
		//��ʪ������ǰ��һλ
		for(int i=0;i<(Array_Num-1);i++)
		{
			T_Aaary[i]=T_Aaary[i+1];
			H_Aaary[i]=H_Aaary[i+1];
			L_Aaary[i]=L_Aaary[i+1];
		}
		
		T_Aaary[Array_Num-1]=temperature; //д���¶��������һλ
		H_Aaary[Array_Num-1]=humidity;    //д��ʪ���������һλ
		L_Aaary[Array_Num-1]=adcx;    //д��ʪ���������һλ
		
		step = 400 / (Array_Num-1);       //�����ͼ����
		
		LCD_Fill(61, 371, 459,669, WHITE);//ɾ�������ڲ�����
		
		//������ʪ������
		for(int i=0;i<(Array_Num-1);i++)
		{
			POINT_COLOR=RED;LCD_DrawLine(60 + step*i, 670-T_Aaary[i]*3, 60 + step*(i+1), 670-T_Aaary[i+1]*3);
			POINT_COLOR=BLUE;LCD_DrawLine(60 + step*i, 670-H_Aaary[i]*3, 60 + step*(i+1), 670-H_Aaary[i+1]*3);
			POINT_COLOR=GREEN;LCD_DrawLine(60 + step*i, 670-L_Aaary[i]*3, 60 + step*(i+1), 670-L_Aaary[i+1]*3);
		}
		
		POINT_COLOR=BLACK;LCD_DrawRectangle(60, 370, 460,670);//���ͼ��߿�
		
		if (temperature > temperature_threshold || humidity > humidity_threshold)//����¶Ȼ�ʪ�ȳ���������ֵ
		{
			if(temperature > temperature_threshold)//����¶ȳ���������ֵ
			{
				POINT_COLOR=RED;LCD_ShowString(30,330+380,420,24,24,"Warnning:Temperature is too high!!!"); //LCD��ʾ������Ϣ
				u1_printf("\r\nWarnning:Temperature is too high!!!\r\n");                                   //�������������Ϣ
			}
			if(humidity > humidity_threshold)//���ʪ�ȳ���������ֵ
			{
				POINT_COLOR=RED;LCD_ShowString(30,360+380,420,24,24,"Warnning:Humidity is too high!!!");    //LCD��ʾ������Ϣ
				u1_printf("\r\nWarnning:Humidity is too high!!!\r\n");                                      //�������������Ϣ
			}
			POINT_COLOR=GREEN;LCD_ShowString(110,180,200,24,24,"On "); //LCD���·�����״̬
			u1_printf("\r\nbeepFlag:BEEPON\r\n");                    //�������������״̬
			Send_Data_Beep_ON();                                     //�����������BEEP״̬
			Warning();                                               //����������
	  }
		
		if(temperature <= temperature_threshold)//����¶ȵ��ڱ�����ֵ
		{
			LCD_ShowString(30,330+380,420,24,24,"                                   ");//LCDɾ���¶ȱ�����Ϣ
		}
		
		if(humidity <= humidity_threshold)//���ʪ�ȵ��ڱ�����ֵ
		{
		  LCD_ShowString(30,360+380,420,24,24,"                                   ");//LCDɾ��ʪ�ȱ�����Ϣ
		}
		
		if(temperature <= temperature_threshold && humidity <= humidity_threshold)//�����ʪ�ȶ�û����������ֵ
		{
			POINT_COLOR=RED;LCD_ShowString(110,180,200,24,24,"Off");  //LCD����BEEP״̬
			Send_Data_Beep_OFF();                                     //�����������BEEP״̬
			//u1_printf("\r\nbeepFlag:BEEPOFF\r\n");
		}
		
		char head1[3];
		char temp[50];				//����һ����ʱ������1,��������ͷ
		char tempAll[100];		//����һ����ʱ������2��������������
		
		int	dataLen = 0;			//���ĳ���
		memset(temp,    0, 50);				    //��ջ�����1
		memset(tempAll, 0, 100);				  //��ջ�����2
		memset(head1,   0, 3);					  //���MQTTͷ
		
		sprintf(temp,"{\"temperature\":\"%d%d\",\"humidity\":\"%d%d\"}", temperature/10, temperature%10, humidity/10, humidity%10);//��������
		
		head1[0] = 0x03; 						      //�̶���ͷ
		head1[1] = 0x00; 					      	//�̶���ͷ
		head1[2] = strlen(temp);  				//ʣ�೤��	
		sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
		
		u1_printf("\r\n DHT: Temperature:%d%d; Humidity:%d%d \r\n", temperature/10, temperature%10, humidity/10, humidity%10);
		
		dataLen = strlen(temp) + 3;
		
		MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen);//������ݣ�������������
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);   	
	}
}

void TIM8_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)	
	{
		adcx = Lsens_Get_Val();//��ȡ������������ֵ
		
		//��ʪ�ȹ�ǿ����ǰ��һλ
		for(int i=0;i<(Array_Num-1);i++)
		{
			T_Aaary[i]=T_Aaary[i+1];
			H_Aaary[i]=H_Aaary[i+1];
			L_Aaary[i]=L_Aaary[i+1];
		}
		
		T_Aaary[Array_Num-1]=temperature; //д���¶��������һλ
		H_Aaary[Array_Num-1]=humidity;    //д��ʪ���������һλ
		L_Aaary[Array_Num-1]=adcx;        //д���ǿ�������һλ
		
		step = 400 / (Array_Num-1);       //�����ͼ����
		
		LCD_Fill(61, 371, 459,669, WHITE);//ɾ�������ڲ�����
		
		//������ʪ������
		for(int i=0;i<(Array_Num-1);i++)
		{
			POINT_COLOR=RED;LCD_DrawLine(60 + step*i, 670-T_Aaary[i]*3, 60 + step*(i+1), 670-T_Aaary[i+1]*3);
			POINT_COLOR=BLUE;LCD_DrawLine(60 + step*i, 670-H_Aaary[i]*3, 60 + step*(i+1), 670-H_Aaary[i+1]*3);
			POINT_COLOR=GREEN;LCD_DrawLine(60 + step*i, 670-L_Aaary[i]*3, 60 + step*(i+1), 670-L_Aaary[i+1]*3);
		}
		
		POINT_COLOR=BLACK;LCD_DrawRectangle(60, 370, 460,670);//���ͼ��߿�
		
		if(adcx<=light_sensor_threshold)//�жϹ�ǿ�Ƿ������ֵ
		{
			POINT_COLOR=RED;LCD_ShowNum(140,270,adcx,2,24);//LCD��ʾ��ǿ
		}
		else
		{
			POINT_COLOR=GREEN;LCD_ShowNum(140,270,adcx,2,24);//LCD��ʾ��ǿ
		}
			
		if(tpad_mode_control)//LED�Զ�����ģʽ
			{
				if(adcx<=light_sensor_threshold)
				{
					led0Flag = "LED0ON"; 			//����LED0״̬
					u1_printf("LED0ON\r\n");  //���������Ϣ
					POINT_COLOR=GREEN;LCD_ShowString(110,210,200,24,24,"On "); //����LCD��LED0״̬
					LED0_ON();                //��LED0
					if(subcribePackFlag == 1){Send_Data_LED0();}         //�����������LED0״̬
					led1Flag = "LED1ON"; 		  //����LED1״̬
					u1_printf("LED1ON\r\n");  //���������Ϣ
					POINT_COLOR=GREEN;LCD_ShowString(110+200+40,210,200,24,24,"On "); //����LCD��LED1״̬
					LED1_ON();                //��LED1
					if(subcribePackFlag == 1){Send_Data_LED1();}         //�����������LED1״̬
				}
				else
				{
					led0Flag = "LED0OFF"; 		//����LED0״̬
					u1_printf("LED0OFF\r\n"); //���������Ϣ
					POINT_COLOR=RED;LCD_ShowString(110,210,200,24,24,"Off"); //����LCD��LED0״̬
					LED0_OFF();               //�ر�LED0
					if(subcribePackFlag == 1){Send_Data_LED0();}         //�����������LED0״̬
					led1Flag = "LED1OFF"; 		//����LED1״̬
					u1_printf("LED1OFF\r\n"); //���������Ϣ
					POINT_COLOR=RED;LCD_ShowString(110+200+40,210,200,24,24,"Off"); //����LCD��LED1״̬
					LED1_OFF();               //�ر�LED1
					if(subcribePackFlag == 1){Send_Data_LED1();}         //�����������LED1״̬
				}
		}
		
		char head1[3];
		char temp[50];				//����һ����ʱ������1,��������ͷ
		char tempAll[100];		//����һ����ʱ������2��������������
		
		int	dataLen = 0;			//���ĳ���
		
		memset(temp,    0, 50);				    //��ջ�����1
		memset(tempAll, 0, 100);				  //��ջ�����2
		memset(head1,   0, 3);					  //���MQTTͷ
		
		sprintf(temp,"{\"LightVal\":\"%d%d\"}", adcx/10, adcx%10);//��������
		
		head1[0] = 0x03; 						      //�̶���ͷ
		head1[1] = 0x00; 					      	//�̶���ͷ
		head1[2] = strlen(temp);  				//ʣ�೤��	
		sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
		
		u1_printf("\r\n Lsens: LightVal:%d%d \r\n", adcx/10, adcx%10);
		
		dataLen = strlen(temp) + 3;
		
		MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen);//������ݣ�������������
				
		
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
		
		
		memset(temp,    0, 50);				    //��ջ�����1
		memset(tempAll, 0, 100);				  //��ջ�����2
		memset(head1,   0, 3);					  //���MQTTͷ
		
		sprintf(temp,"{\"ControlMode\":\"%d\"}", tpad_mode_control);//��������
		
		head1[0] = 0x03; 						      //�̶���ͷ
		head1[1] = 0x00; 					      	//�̶���ͷ
		head1[2] = strlen(temp);  				//ʣ�೤��	
		sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
		
		u1_printf("\r\n HouseKeeper: ControlMode:%d \r\n", tpad_mode_control);
		
		dataLen = strlen(temp) + 3;
		
		MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen);//������ݣ�������������
		
		
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
		
		
		memset(temp,    0, 50);				    //��ջ�����1
		memset(tempAll, 0, 100);				  //��ջ�����2
		memset(head1,   0, 3);					  //���MQTTͷ
		
		sprintf(temp,"{\"Num\":\"%d\"}", Num);//��������
		
		head1[0] = 0x03; 						      //�̶���ͷ
		head1[1] = 0x00; 					      	//�̶���ͷ
		head1[2] = strlen(temp);  				//ʣ�೤��	
		sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
		
		u1_printf("\r\n HouseKeeper: Num:%d \r\n", Num);
		
		dataLen = strlen(temp) + 3;
		
		MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen);//������ݣ�������������
		
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);   	
	}
}
