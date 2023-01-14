/**********************************************************************************************
 
 OneNET Smart Home ���ܼҾ�ϵͳ 
 
 
 ��Ŀ��飺����Ŀ��������ԭ�ӵ�ս��V3�����弰OneNET������ƽ̨��ʵ�������ܼҾ�ϵͳ�Ĵ��
					 ϵͳ��Ҫ�����¹��ܣ�
							 ��1��LED�ֶ�ģʽ��ͨ��OneNETƽ̨��LED0��LED1�Ŀ���״̬���п��ơ�
							 ��2��LED�ֶ�ģʽ����Ƭ��������LED0��LED1�Ŀ���״̬���п��ơ�
							 ��3��LED�Զ�ģʽ��ͨ����������������ǿ���趨�Ĺ�ǿ��ֵ�Զ�����LED0��LED1
																 �Ŀ���״̬��
							 ��4��ͨ��OneNETƽ̨��DHT11�����ʪ�ȹ��ܽ��п��ơ�
							 ��5��ͨ����Ƭ������ƽ̨��DHT11�����ʪ�ȹ��ܽ��п��ơ�
							 ��6��ͨ��OneNETƽ̨�Զ����ת�ǶȽ��й̶��Ƕȿ��ơ�
							 ��7��ͨ��OneNETƽ̨�Զ����ת�ǶȽ������ɽǶȿ��ơ�
							 ��8��ͨ����Ƭ���Զ����ת�ǶȽ��й̶��Ƕȿ��ơ�
							 ��9��LCD���¶ȡ�ʪ�ȡ���ǿ��ʱ��仯���߻��ơ�
							 ��10��OneNETƽ̨ͨ���Ǳ��̶�DHT11���õ�����ʪ�����ݽ�����ʾ��
							 ��11��OneNETƽ̨��DHT11���õ�����ʪ�����ݽ������߻��ơ�
							 ��12��OneNETƽ̨��ʾLED����ģʽ״̬��
							 ��13����Ƭ���¶ȳ��߱�����
							 ��14����Ƭ��ʪ�ȳ��߱�����
							 ��15��OneNETƽ̨��ʾ��ʪ�ȱ���״̬��
 
 
 �������⣺��ʹ����ҳ��ʹ����ת��ť�Զ���ǶȽ����Զ������ʱ�����ܻ����������Ч��
					 ��ʹLED�򿪵���������ϵͳ���ɽ����������һ�������ST-Link��¼��ĵ�һ
					 ��������
 
 
																															 ���ߣ������� @YSU
																															 ʱ�䣺2023��01��06��
																															 ���䣺liulianjun_2022@163.com
**********************************************************************************************/

#include "stm32f10x.h"
#include "delay.h"

//����
#include "usart1.h"
#include "usart2.h"

//��ʱ��
#include "sg90.h"  //timer1
#include "timer2.h"
#include "timer3.h"
#include "timer4.h"
#include "tpad.h"  //timer5
#include "timer8.h"

//����
#include "lcd.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "dht11.h"
#include "adc.h"
#include "lsens.h"

//ͨ��
#include "wifi.h"
#include "mqtt.h"
#include "control.h"


///////////////////////////////////////////////////////////////////////////////////////////////

char *cmdLED0_On  = "LED0ON";      //LED0��
char *cmdLED0_Off = "LED0OFF";     //LED0�ر�
char *cmdLED1_On  = "LED1ON";      //LED1��
char *cmdLED1_Off = "LED1OFF";     //LED1�ر�

char *cmdDHT11_On  = "DHTON";    //��ʪ�����ݴ����
char *cmdDHT11_Off = "DHTOFF";   //��ʪ�����ݴ���ر�

char *cmdServo_On1  = "SERVOON1";     //SERVO��1
char *cmdServo_On2  = "SERVOON2";     //SERVO��2
char *cmdServo_On3  = "SERVOON3";     //SERVO��3
char *cmdServo_On4  = "SERVOON4";     //SERVO��4
char *cmdServo_Off  = "SERVOOFF";     //SERVO�ر�

char *cmdNum  = "N";       //����Ƕȿ�������

///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////

char *led0Flag  = "LED0OFF";       //LED0״̬
char *led1Flag  = "LED1OFF";       //LED1״̬

char *dhtFlag   = "DHTON";		     //DHT״̬

char *servoFlag = "SERVOOFF";      //SERVO״̬

int   Num       = 0;               //�������Ƕ�

int   key                    = 0;               //����״̬
int   tpad                   = 0;               //��������״̬
int   tpad_mode_control      = 0;               //��������ģʽ����״̬
int   tpad_mode_control_flag = 0;               //��������ģʽ����״̬��־

///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////

u8 adcx;			                     //��Ź�����������ֵ
int light_sensor_threshold = 50;   //LED�Զ�������ֵ
extern int temperature_threshold;  //�¶ȱ�����ֵ
extern int humidity_threshold;     //ʪ�ȱ�����ֵ

///////////////////////////////////////////////////////////////////////////////////////////////


int main(void) 
{	
	u8 lcd_id[12];			                             //���LCD ID�ַ���
	Delay_Init();                                    //��ʱ���ܳ�ʼ��
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	Usart1_Init(115200);                             //����1���ܳ�ʼ��,������115200  ���ڵ���
	Usart2_Init(115200);                             //����2���ܳ�ʼ��,������115200	WIFIͨ�� MQTTͨ��
	
	servo_init(1999, 719);                           //��ʱ��1���������ʼ��
	servo_off();                                     //�����ʼλ������Ϊ0��
	TIM4_Init(500,7200);                             //TIM4��ʼ��,��ʱʱ�� 500*7200*1000/72000000 = 50ms MQTTͨ�����ݽ���
	
	LCD_Init();                                      //LCD��ʼ��
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);  //��LCD ID��ӡ��lcd_id���顣
	LCD_Clear(WHITE);                                //LCD����
	LED_Init();			                                 //LED��ʼ��
	BEEP_Init();                                     //BEEP��ʼ��
	KEY_Init();			                                 //������ʼ��
	TPAD_Init(6);			                               //����������ʼ��
	DHT11_Init();                                    //DHT11��ʼ��
	Lsens_Init(); 			                             //��ʼ������������
	
	WiFi_ResetIO_Init();                             //��ʼ��WiFi�ĸ�λIO
	IoT_Parameter_Init();                            //��ʼ����IoTƽ̨MQTT�������Ĳ���	
	
	//LCD����
	POINT_COLOR=BLACK;LCD_ShowString(30,30,240,24,24,"OneNET Smart Home");
//	POINT_COLOR=BLUE ;LCD_ShowString(380,30,210,24,24,"Y S U");
//	POINT_COLOR=BLACK;LCD_ShowString(30,70,240,24,24,"Yanshan University");
	POINT_COLOR=BLACK;LCD_ShowString(30,70,200,16,16,"Liu Lianjun 202011030312");
  POINT_COLOR=BLACK;LCD_ShowString(30,90,200,16,16,"Gu  Jinxuan 202011030342");
	
	//�豸״̬
	POINT_COLOR=BLUE; LCD_ShowString(30,120,200,24,24,"WiFi State:");   //WiFi״̬
	POINT_COLOR=BLUE; LCD_ShowString(170,120,200,24,24,"Connecting");
	POINT_COLOR=BLACK;LCD_ShowString(30,150,200,24,24,"LED Control Mode:");//LED����״̬
	POINT_COLOR=RED;  LCD_ShowString(240,150,200,24,24,"Hand");
  POINT_COLOR=BLACK;LCD_ShowString(30+200+40,180,200,24,24,"Servo:");   //Servo״̬
	POINT_COLOR=RED;  LCD_ShowString(110+200+40,180,200,24,24,"Off ");
	POINT_COLOR=BLACK;LCD_ShowString(30,180,200,24,24,"Beep :");       //Beep״̬
	POINT_COLOR=RED;  LCD_ShowString(110,180,200,24,24,"Off");
	POINT_COLOR=BLACK;LCD_ShowString(30,210,200,24,24,"LED0 :");       //LED0״̬
	POINT_COLOR=RED;  LCD_ShowString(110,210,200,24,24,"Off");
	POINT_COLOR=BLACK;LCD_ShowString(30+200+40,210,200,24,24,"LED1 :");   //LED1״̬
	POINT_COLOR=RED;  LCD_ShowString(110+200+40,210,200,24,24,"Off");
	POINT_COLOR=BLACK;LCD_ShowString(30,240,200,24,24,"Lsens:");       //DHT״̬
	POINT_COLOR=GREEN;LCD_ShowString(110,240,200,24,24,"On ");
	POINT_COLOR=BLACK;LCD_ShowString(30+200+40,240,200,24,24,"DHT  :");   //DHT״̬
	POINT_COLOR=GREEN;LCD_ShowString(110+200+40,240,200,24,24,"On ");
	
	//������������ֵ��ʾ
	POINT_COLOR=BLACK;LCD_ShowString(30,270,200,24,24,"LsensVal:");
	POINT_COLOR=BLACK;LCD_ShowString(240,275,200,16,16,"Light Sensor Threshold:");
	POINT_COLOR=RED;  LCD_ShowNum(430,270,light_sensor_threshold,2,24);
	
	//��ʪ����ʾ
	POINT_COLOR=BLACK;LCD_ShowString(30,270+30,200,24,24,"Temperature:");   //�¶���ʾ
	POINT_COLOR=BLACK;LCD_ShowString(210,270+30,200,24,24,"*");
	POINT_COLOR=BLACK;LCD_ShowString(240,270+35,200,16,16,"Temperature Threshold:");
	POINT_COLOR=RED;  LCD_ShowNum(420,270+30,temperature_threshold,2,24);
	POINT_COLOR=BLACK;LCD_ShowString(450,270+35,20,16,16,"*");
	
	POINT_COLOR=BLACK;LCD_ShowString(30,300+30,200,24,24,"Humidity:");      //ʪ����ʾ
	POINT_COLOR=BLACK;LCD_ShowString(170,300+30,200,24,24,"%");
	POINT_COLOR=BLACK;LCD_ShowString(240,300+35,200,16,16,"Humidity Threshold:");
	POINT_COLOR=RED;  LCD_ShowNum(395,300+30,humidity_threshold,2,24);
	POINT_COLOR=BLACK;LCD_ShowString(425,300+35,20,16,16,"%");
	
	//��ʪ�Ȼ�ͼ  (60,370),(460,670) 400*300
	POINT_COLOR=BLACK;LCD_DrawRectangle(60, 370, 460,670);
	POINT_COLOR=BLACK;LCD_ShowString(30,365,30,16,16,"100");
	POINT_COLOR=BLACK;LCD_ShowString(30,395,30,16,16," 90");
	POINT_COLOR=BLACK;LCD_ShowString(30,425,30,16,16," 80");
	POINT_COLOR=BLACK;LCD_ShowString(30,455,30,16,16," 70");
	POINT_COLOR=BLACK;LCD_ShowString(30,485,30,16,16," 60");
	POINT_COLOR=BLACK;LCD_ShowString(30,515,30,16,16," 50");
	POINT_COLOR=BLACK;LCD_ShowString(30,545,30,16,16," 40");
	POINT_COLOR=BLACK;LCD_ShowString(30,575,30,16,16," 30");
	POINT_COLOR=BLACK;LCD_ShowString(30,605,30,16,16," 20");
	POINT_COLOR=BLACK;LCD_ShowString(30,635,30,16,16," 10");
	POINT_COLOR=BLACK;LCD_ShowString(30,665,30,16,16,"  0");
	POINT_COLOR=RED;   LCD_ShowString(50+20, 685,100,16,16,"--Temperature"); //ͼ�� Temperature
	POINT_COLOR=BLUE;  LCD_ShowString(200+30,685,100,16,16,"--Humidity");    //ͼ�� Humidity
	POINT_COLOR=GREEN; LCD_ShowString(350+20,685,100,16,16,"--LightVal");    //ͼ�� LightVal
	while(1)
	{	
		if(connectFlag == 1)// connectFlag=1ͬ����������������
		{
			POINT_COLOR=BLUE;LCD_ShowString(170,120,200,24,24,"On        "); //WiFi״̬����
			
			key = KEY_Scan(0);//��ȡ����״̬
			tpad = TPAD_Scan(0);//��ȡ��������״̬
			
			if(tpad)//����LED����ģʽ
			{
				tpad_mode_control_flag = 1;//���ҿ���״̬���ı�־λ
				tpad_mode_control = 1 - tpad_mode_control;
			}
			
			if(tpad_mode_control == 1 && tpad_mode_control_flag ==1)//LCD��ʾLED����ģʽ
			{
				POINT_COLOR=GREEN;  LCD_ShowString(240,150,200,24,24,"Auto");//LED�Զ�����ģʽ
				tpad_mode_control_flag = 0;//�������״̬���ı�־λ
			}
			else if(tpad_mode_control == 0 && tpad_mode_control_flag ==1)
			{
				POINT_COLOR=RED;    LCD_ShowString(240,150,200,24,24,"Hand");//LED�ֶ�����ģʽ
				tpad_mode_control_flag = 0;//�������״̬���ı�־λ
			}
			
			//�����ͻ���������
			if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr)
			{                
				//MQTTͨѶ��������
				//1��0x10 ���ӱ���
				//2��0x82 ���ı���,��connectPackFlag��λ,��ʾ���ӱ��ĳɹ�
				//3��subcribePackFlag��λ,˵�����ӺͶ��ľ��ɹ�,�������Ŀɷ�
				if((MQTT_TxDataOutPtr[2] == 0x10)||((MQTT_TxDataOutPtr[2] == 0x82)&&(connectPackFlag == 1))||(subcribePackFlag == 1)) 
				{    
					u1_printf("\r\n��������:0x%x\r\n", MQTT_TxDataOutPtr[2]); //���������Ϣ
					MQTT_TxData(MQTT_TxDataOutPtr);										        //��������
					MQTT_TxDataOutPtr += TBUFF_UNIT;									        //ָ������
					if(MQTT_TxDataOutPtr == MQTT_TxDataEndPtr)				        //���ָ�뵽������β����
					{ 
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];		    	        //ָ���λ����������ͷ
					}	
				}				
			}
			
			
			//������ջ���������
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr)
			{
				u1_printf("\r\n��������:");
				
				//����CONNACK����
				if(MQTT_RxDataOutPtr[2] == 0x20)//if�ж�,�����һ���ֽ���0x20,��ʾ�յ�����CONNACK����
				{             			
					switch(MQTT_RxDataOutPtr[5])//���жϵ�4���ֽ�,��CONNECT�����Ƿ�ɹ�
					{					   
						case 0x00 : u1_printf("CONNECT���ĳɹ�\r\n");	                        //���������Ϣ	
										connectPackFlag = 1;//CONNECT���ĳɹ�,���ı��Ŀɷ�
									break;
						case 0x01 : u1_printf("�����Ѿܾ�,��֧�ֵ�Э��汾,׼������\r\n");    //���������Ϣ
									connectFlag = 0;//connectFlag����,��������
									break;
						case 0x02 : u1_printf("�����Ѿܾ�,���ϸ�Ŀͻ��˱�ʶ��,׼������\r\n");//���������Ϣ
									connectFlag = 0;//connectFlag����,��������
									break;
						case 0x03 : u1_printf("�����Ѿܾ�,����˲�����,׼������\r\n");	      //���������Ϣ
									connectFlag = 0;//connectFlag����,��������
									break;
						case 0x04 : u1_printf("�����Ѿܾ�,��Ч���û���������,׼������\r\n");	//���������Ϣ
									connectFlag = 0;//connectFlag����,��������						
									break;
						case 0x05 : u1_printf("�����Ѿܾ�,δ��Ȩ,׼������\r\n");			      	//���������Ϣ
									connectFlag = 0;//connectFlag����,��������						
									break;
						default   : u1_printf("�����Ѿܾ�,δ֪״̬,׼������\r\n");		        //���������Ϣ 
									connectFlag = 0;//connectFlag����,��������					
									break;
					}
			  }
				
				//����SUBACK����
				else if(MQTT_RxDataOutPtr[2] == 0x90)//if�ж�,��һ���ֽ���0x90,��ʾ�յ�����SUBACK����
				{ 
					switch(MQTT_RxDataOutPtr[6])//��������Ҫ�ж϶��Ļظ�,�����ǲ��ǳɹ�
					{					
						case 0x00 :
						case 0x01 : 
									u1_printf("���ĳɹ�\r\n");			//���������Ϣ
									subcribePackFlag = 1;				    //subcribePackFlag��1,��ʾ���ı��ĳɹ�,�������Ŀɷ���
									pingFlag = 0;						        //pingFlag����
									TIM3_ENABLE_30S();					    //����30s��PING��ʱ��	
									u1_printf("\r\n");              //��������豸��ʼ��Ϣ
									u1_printf("************************************\r\n");
									Send_Data_LED0();               //�����������LED0��ʼ��Ϣ
									Send_Data_LED1();               //�����������LED1��ʼ��Ϣ
									Send_Data_Servo();              //�����������SERVO��ʼ��Ϣ
									Send_Data_DHT();                //�����������DHT��ʼ��Ϣ
									u1_printf("************************************\r\n");
									TIM2_ENABLE();                  //Ĭ�Ͽ���DHT
									TIM8_ENABLE();                  //Ĭ�Ͽ���Lsens
									break;
						default: 
									u1_printf("����ʧ��,׼������\r\n");//���������Ϣ 
									connectFlag = 0;//connectFlag����,��������
									break;
						}					
			    }
					
					//����PINGRESP����
					else if(MQTT_RxDataOutPtr[2] == 0xD0)//if�ж�,��һ���ֽ���0xD0,��ʾ�յ�����PINGRESP����
					{ 
						u1_printf("PING���Ļظ�\r\n");  //���������Ϣ 
						if(pingFlag == 1)    //���pingFlag=1,��ʾ��һ�η���
						{                   						        
							 pingFlag = 0;     //���pingFlag��־
						}
						else if(pingFlag > 1)//���pingFlag>1,��ʾ�Ƕ�η�����,������2s����Ŀ��ٷ���
						{ 				 									
							pingFlag = 0;      //���pingFlag��־
							TIM3_ENABLE_30S(); //PING��ʱ���ػ�30s��ʱ��
						}				
					}	
					
					//����������������ݱ���
					else if((MQTT_RxDataOutPtr[2] == 0x30))//if�ж�,�����һ���ֽ���0x30,��ʾ�յ����Ƿ�������������������
					{ 
						u1_printf("�������ȼ�0����\r\n"); 		   	  //���������Ϣ 
						MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);   //����ȼ�0��������,��ȡ��������
					}									
					MQTT_RxDataOutPtr += RBUFF_UNIT;              //ָ������
					if(MQTT_RxDataOutPtr == MQTT_RxDataEndPtr)    //���ָ�뵽������β����
					{
						MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];      //ָ���λ����������ͷ              
					}
				}
			
			
				//��������������� ���ջ��������յ����ݻ��а������´���
				if((MQTT_CMDOutPtr != MQTT_CMDInPtr) || key == 1 || key == 2 || key == 3 || key == 4)
				{                             		       
					u1_printf("\r\n����:");  //���������Ϣ
					
					int state = 0; //�������ͱ���
					
					//�жϴӷ��������յ�����������
					if((!memcmp(&MQTT_CMDOutPtr[2],cmdLED0_On,strlen(cmdLED0_On))) && tpad_mode_control == 0)//�ֶ�����ģʽ��Ч
					{
						state = 1;//LED0������
					}
					else if((!memcmp(&MQTT_CMDOutPtr[2],cmdLED0_Off,strlen(cmdLED0_Off))) && tpad_mode_control == 0)//�ֶ�����ģʽ��Ч
					{
						state = 2;//LED0�ر�����
					}
					else if((!memcmp(&MQTT_CMDOutPtr[2],cmdLED1_On,strlen(cmdLED1_On))) && tpad_mode_control == 0)//�ֶ�����ģʽ��Ч
					{
						state = 3;//LED1������
					}
					else if((!memcmp(&MQTT_CMDOutPtr[2],cmdLED1_Off,strlen(cmdLED1_Off))) && tpad_mode_control == 0)//�ֶ�����ģʽ��Ч
					{ 
						state = 4;//LED1�ر�����
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdDHT11_On,strlen(cmdDHT11_On)))
					{ 
						state = 5;//DHT������
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdDHT11_Off,strlen(cmdDHT11_Off)))
					{ 
						state = 6;//DHT�ر�����
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdServo_On1,strlen(cmdServo_On1)))
					{      
						state = 7;//SERVO��45������
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdServo_On2,strlen(cmdServo_On2)))
					{      
						state = 8;//SERVO��90������
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdServo_On3,strlen(cmdServo_On3)))
					{
						state = 9;//SERVO��135������
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdServo_On4,strlen(cmdServo_On4)))
					{      
						state = 10;//SERVO��180������
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdServo_Off,strlen(cmdServo_Off)))
					{
						state = 11;//SERVO��0������
					}
//					else if(!memcmp(&MQTT_CMDOutPtr[2], cmdNum, strlen(cmdNum)))
//					{
//						state = 12;//SERVO��0������
//					}
					else if(MQTT_CMDOutPtr[2] == 'N' && MQTT_CMDOutPtr[3] == ':')
					{
						state = 12;//SERVO��0������
					}
					else
					{
						state = 0;//������
					}
					
					//�жϰ���״̬
					switch(key)
					{
						case 1:
						{
							if((!memcmp(led0Flag,cmdLED0_Off,strlen(cmdLED0_Off))) && tpad_mode_control == 0)//LED0״̬Ϊ�ر��Ұ���1����,��Ϊ�ֶ�����ģʽ
							{
								state = 1;//LED0������
							}
							else if((!memcmp(led0Flag,cmdLED0_On,strlen(cmdLED0_On))) && tpad_mode_control == 0)//LED0״̬Ϊ���Ұ���1����,��Ϊ�ֶ�����ģʽ
							{
								state = 2;//LED0�ر�����
							}
							else
							{
								state = 0;//������
							}
							break;
						}
						case 2:
						{
							if((!memcmp(led1Flag,cmdLED1_Off,strlen(cmdLED1_Off))) && tpad_mode_control == 0)//LED1״̬Ϊ�ر��Ұ���2����,���ֶ�������ģʽ
							{
								state = 3;//LED1������
							}
							else if((!memcmp(led1Flag,cmdLED1_On,strlen(cmdLED1_On))) && tpad_mode_control == 0)//LED1״̬Ϊ���Ұ���1����,���ֶ�������ģʽ
							{
								state = 4;//LED1�ر�����
							}
							else
							{
								state = 0;//������
							}
							break;
						}
						case 3:
						{
							if(!memcmp(dhtFlag,cmdDHT11_Off,strlen(cmdDHT11_Off)))//DHT״̬Ϊ�ر��Ұ���3����
							{
								state = 5;//DHT������
							}
							else
							{
								state = 6;//DHT�ر�����
							}
							break;
						}
						case 4:
						{
							if(!memcmp(servoFlag,cmdServo_Off,strlen(cmdServo_Off)))//SERVO״̬Ϊ�ر��Ұ���4����
							{
								state = 7;//SERVO��45������
							}
							else if(!memcmp(servoFlag,cmdServo_On1,strlen(cmdServo_On1)))
							{
								state = 8;//SERVO��90������
							}
							else if(!memcmp(servoFlag,cmdServo_On2,strlen(cmdServo_On2)))
							{
								state = 9;//SERVO��135������
							}
							else if(!memcmp(servoFlag,cmdServo_On3,strlen(cmdServo_On3)))
							{
								state = 10;//SERVO��180������
							}
							else
							{
								state = 11;//SERVO��0������
							}
							break;
						}
						default:
						{
							break;
						}
					}
					
					switch(state)
					{
						case 1:
						{
						led0Flag = "LED0ON"; 			//����LED0״̬
						u1_printf("LED0ON\r\n");  //���������Ϣ
						POINT_COLOR=GREEN;LCD_ShowString(110,210,200,24,24,"On "); //����LCD��LED0״̬
						LED0_ON();                //��LED0
						Send_Data_LED0();         //�����������LED0״̬
						break;
						}
						case 2:
						{
						led0Flag = "LED0OFF"; 		//����LED0״̬
						u1_printf("LED0OFF\r\n"); //���������Ϣ
						POINT_COLOR=RED;LCD_ShowString(110,210,200,24,24,"Off"); //����LCD��LED0״̬
						LED0_OFF();               //�ر�LED0
						Send_Data_LED0();         //�����������LED0״̬
						break;
						}
						case 3:
						{
						led1Flag = "LED1ON"; 		  //����LED1״̬
						u1_printf("LED1ON\r\n");  //���������Ϣ
						POINT_COLOR=GREEN;LCD_ShowString(110+200+40,210,200,24,24,"On "); //����LCD��LED1״̬
						LED1_ON();                //��LED1
						Send_Data_LED1();         //�����������LED1״̬
						break;
						}
						case 4:
						{
						led1Flag = "LED1OFF"; 		//����LED1״̬
						u1_printf("LED1OFF\r\n"); //���������Ϣ
						POINT_COLOR=RED;LCD_ShowString(110+200+40,210,200,24,24,"Off"); //����LCD��LED1״̬
						LED1_OFF();               //�ر�LED1
						Send_Data_LED1();         //�����������LED1״̬
						break;
						}
						case 5:
						{
						dhtFlag = "DHTON"; 			  //����DHT״̬
						u1_printf("DHTON\r\n");   //���������Ϣ
						POINT_COLOR=GREEN;LCD_ShowString(110+200+40,240,200,24,24,"On "); //����LCD��DHT״̬
						TIM2_ENABLE();            //��DHT
						Send_Data_DHT();          //�����������DHT״̬
						break;
						}
						case 6:
						{
						dhtFlag = "DHTOFF";			  //����DHT״̬
						u1_printf("DHTOFF\r\n");  //���������Ϣ
						POINT_COLOR=RED;LCD_ShowString(110+200+40,240,200,24,24,"Off"); //����LCD��DHT״̬
						TIM_Cmd(TIM2,DISABLE);    //�ر�DHT
						Send_Data_DHT();          //�����������DHT״̬
						break;
						}
						case 7:
						{
						servoFlag = "SERVOON1";		//����SERVO״̬
						u1_printf("SERVOON1\r\n");//���������Ϣ
						POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"45* "); //����LCD��SERVO״̬
						Send_Data_Servo();        //�����������SERVO״̬
						servo_on_45();            //SERVO��45��
						Num=45;
						break;
						}
						case 8:
						{
						servoFlag = "SERVOON2";		//����SERVO״̬
						u1_printf("SERVOON2\r\n");//���������Ϣ
						POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"90* "); //����LCD��SERVO״̬
						Send_Data_Servo();        //�����������SERVO״̬
						servo_on_90();            //SERVO��90��
						Num=90;
						break;
						}
						case 9:
						{
						servoFlag = "SERVOON3";		//����SERVO״̬
						u1_printf("SERVOON3\r\n");//���������Ϣ
						POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"135*"); //����LCD��SERVO״̬
						Send_Data_Servo();        //�����������SERVO״̬
						servo_on_135();           //SERVO��135��
						Num=135;
						break;
						}
						case 10:
						{
						servoFlag = "SERVOON4";		//����SERVO״̬
						u1_printf("SERVOON4\r\n");//���������Ϣ
						POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"180*"); //����LCD��SERVO״̬
						Send_Data_Servo();        //�����������SERVO״̬
						servo_on_180();           //SERVO��180��
						Num=180;
						break;
						}
						case 11:
						{
						servoFlag = "SERVOOFF";		//����SERVO״̬
						u1_printf("SERVOOFF\r\n");//���������Ϣ
						POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"Off "); //����LCD��SERVO״̬
						Send_Data_Servo();        //�����������SERVO״̬
						servo_off();              //SERVO��0��
						Num=0;
						break;
						}
						case 12:
						{
						//�������������͵ĺ���������
						int first_num = 0;
						int second_num = 0;
						int third_num = 0;
						int second_num_flag = 0;//��λ����ʶ
						int third_num_flag = 0; //��λ����ʶ
							
						switch(MQTT_CMDOutPtr[4])//������һλ
						{
							case '0':first_num = 0;break;
							case '1':first_num = 1;break;
							case '2':first_num = 2;break;
							case '3':first_num = 3;break;
							case '4':first_num = 4;break;
							case '5':first_num = 5;break;
							case '6':first_num = 6;break;
							case '7':first_num = 7;break;
							case '8':first_num = 8;break;
							case '9':first_num = 9;break;
							default:break;
						}
						
						switch(MQTT_CMDOutPtr[5])//�����ڶ�λ
						{
							case '0':second_num = 0;second_num_flag = 1;break;
							case '1':second_num = 1;second_num_flag = 1;break;
							case '2':second_num = 2;second_num_flag = 1;break;
							case '3':second_num = 3;second_num_flag = 1;break;
							case '4':second_num = 4;second_num_flag = 1;break;
							case '5':second_num = 5;second_num_flag = 1;break;
							case '6':second_num = 6;second_num_flag = 1;break;
							case '7':second_num = 7;second_num_flag = 1;break;
							case '8':second_num = 8;second_num_flag = 1;break;
							case '9':second_num = 9;second_num_flag = 1;break;
							default:break;
						}
						
						switch(MQTT_CMDOutPtr[6])//��������λ
						{
							case '0':third_num = 0;third_num_flag = 1;break;
							case '1':third_num = 1;third_num_flag = 1;break;
							case '2':third_num = 2;third_num_flag = 1;break;
							case '3':third_num = 3;third_num_flag = 1;break;
							case '4':third_num = 4;third_num_flag = 1;break;
							case '5':third_num = 5;third_num_flag = 1;break;
							case '6':third_num = 6;third_num_flag = 1;break;
							case '7':third_num = 7;third_num_flag = 1;break;
							case '8':third_num = 8;third_num_flag = 1;break;
							case '9':third_num = 9;third_num_flag = 1;break;
							default:break;
						}
						
						//������������͵�����
						if(second_num_flag == 0 && third_num_flag == 0)
						{
							Num = first_num;
							POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"    "); //����LCD��SERVO״̬
							POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24," *");   //����LCD��SERVO״̬
							POINT_COLOR=RED;LCD_ShowNum(110+200+40,180,Num,1,24);            //����LCD��SERVO״̬
						}
						else if(second_num_flag == 1 && third_num_flag == 0)
						{
							Num = first_num*10 + second_num;
							POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"    "); //����LCD��SERVO״̬
							POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"  *");  //����LCD��SERVO״̬
							POINT_COLOR=RED;LCD_ShowNum(110+200+40,180,Num,2,24);            //����LCD��SERVO״̬
						}
						else
						{
							Num = first_num*100 + second_num*10 + third_num;
							if(Num>180){Num = 180;}//��ֹ�������ݹ���
							POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"    "); //����LCD��SERVO״̬
							POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"   *"); //����LCD��SERVO״̬
							POINT_COLOR=RED;LCD_ShowNum(110+200+40,180,Num,3,24);            //����LCD��SERVO״̬
						}
						
						u1_printf("Num:%d\r\n",Num); //���������Ϣ
						
						int Set_Num = 0;
						Set_Num = 1950 - (int)(200 * (float)((float)Num / 180.0));//��������Ҫ���õ���ֵ
						
						//����������¶��״̬
						char head1[3];
						char temp[50];				//����һ����ʱ������1,��������ͷ
						char tempAll[100];		//����һ����ʱ������2��������������
						
						int	dataLen = 0;			//���ĳ���
						
						memset(temp,    0, 50);				    //��ջ�����1
						memset(tempAll, 0, 100);				  //��ջ�����2
						memset(head1,   0, 3);					  //���MQTTͷ
						
						//���¹̶����ư�ť״̬
						sprintf(temp,"{\"servoFlag\":\"SERVOOFF\"}");//��������
						
						head1[0] = 0x03; 						      //�̶���ͷ
						head1[1] = 0x00; 					      	//�̶���ͷ
						head1[2] = strlen(temp);  				//ʣ�೤��	
						sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
						
						//u1_printf("servoFlag:SERVOOFF\r\n");
						
						dataLen = strlen(temp) + 3;
						
						MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen);//������ݣ�������������
						
						memset(temp,    0, 50);				    //��ջ�����1
						memset(tempAll, 0, 100);				  //��ջ�����2
						memset(head1,   0, 3);					  //���MQTTͷ
						
						//�������ɿ�����ť״̬
						sprintf(temp,"{\"Num\":\"%d\"}", Num);//��������
						
						head1[0] = 0x03; 						      //�̶���ͷ
						head1[1] = 0x00; 					      	//�̶���ͷ
						head1[2] = strlen(temp);  				//ʣ�೤��	
						sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
						
						//u1_printf("\r\n HouseKeeper: Num:%d \r\n", Num);
						
						dataLen = strlen(temp) + 3;
						
						MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen);//������ݣ�������������
						
						TIM_SetCompare1(TIM1,Set_Num);//�������Ƕ���ת
						DelayMs(1000);//��ʱ1s
						break;
						}
						default:
						{
						u1_printf("**δָ֪��**%s\r\n", &MQTT_CMDOutPtr[2]); //���������Ϣ
						break;
						}
					}
					if(MQTT_CMDOutPtr != MQTT_CMDInPtr)
					{
						MQTT_CMDOutPtr += CBUFF_UNIT;				      //ָ������
						if(MQTT_CMDOutPtr == MQTT_CMDEndPtr)	    //���ָ�뵽������β����
							MQTT_CMDOutPtr = MQTT_CMDBuf[0];			  //ָ���λ����������ͷ	
					}
				}
			}
			
			else//connectFlag=0,ͬ�������Ͽ�������,�������ӷ�����
			{ 
				POINT_COLOR=BLUE;LCD_ShowString(170,120,200,24,24,"Off       ");
				u1_printf("\r\n\r\n------------------------------------\r\n");
				u1_printf("�豸δ����,���ӷ�����\r\n");        //���������Ϣ
				TIM_Cmd(TIM3, DISABLE);                        //�ر�TIM3  ����������ʧЧ
				TIM_Cmd(TIM4, DISABLE);                        //�ر�TIM4  MQTT����ʧЧ
				WiFi_RxCounter = 0;                            //WiFi������������������                        
				memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);      //���WiFi���ջ����� 
				if(WiFi_Connect_IoTServer() == 0)			         //���WiFi�����Ʒ�������������0,��ʾ��ȷ,����if
				{   			     
					u1_printf("����TCP���ӳɹ�\r\n");            //���������Ϣ
					u1_printf("------------------------------------\r\n");
					connectFlag = 1;                             //connectFlag��1,��ʾ���ӳɹ�	
					WiFi_RxCounter = 0;                          //WiFi������������������                        
					memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);    //���WiFi���ջ����� 
					MQTT_Buff_Init();                            //��ʼ�����ͻ�����                    
				}
			}
	}
}
