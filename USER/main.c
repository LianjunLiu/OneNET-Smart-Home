/**********************************************************************************************
 
 OneNET Smart Home 智能家居系统 
 
 
 项目简介：本项目基于正点原子的战舰V3开发板及OneNET物联网平台，实现了智能家居系统的搭建。
					 系统主要有以下功能：
							 （1）LED手动模式：通过OneNET平台对LED0、LED1的开关状态进行控制。
							 （2）LED手动模式：单片机按键对LED0、LED1的开关状态进行控制。
							 （3）LED自动模式：通过光敏传感器检测光强及设定的光强阈值自动控制LED0、LED1
																 的开关状态。
							 （4）通过OneNET平台对DHT11监测温湿度功能进行控制。
							 （5）通过单片机按键平台对DHT11监测温湿度功能进行控制。
							 （6）通过OneNET平台对舵机旋转角度进行固定角度控制。
							 （7）通过OneNET平台对舵机旋转角度进行自由角度控制。
							 （8）通过单片机对舵机旋转角度进行固定角度控制。
							 （9）LCD对温度、湿度、光强随时间变化曲线绘制。
							 （10）OneNET平台通过仪表盘对DHT11监测得到的温湿度数据进行显示。
							 （11）OneNET平台对DHT11监测得到的温湿度数据进行曲线绘制。
							 （12）OneNET平台显示LED控制模式状态。
							 （13）单片机温度超高报警。
							 （14）单片机湿度超高报警。
							 （15）OneNET平台显示温湿度报警状态。
 
 
 存在问题：在使用网页端使用旋转按钮对舵机角度进行自定义控制时，可能会出现命令无效，
					 并使LED打开的现象，重启系统即可解决。该现象一般出现在ST-Link烧录后的第一
					 次启动后。
 
 
																															 作者：刘连竣 @YSU
																															 时间：2023年01月06日
																															 邮箱：liulianjun_2022@163.com
**********************************************************************************************/

#include "stm32f10x.h"
#include "delay.h"

//串口
#include "usart1.h"
#include "usart2.h"

//定时器
#include "sg90.h"  //timer1
#include "timer2.h"
#include "timer3.h"
#include "timer4.h"
#include "tpad.h"  //timer5
#include "timer8.h"

//外设
#include "lcd.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "dht11.h"
#include "adc.h"
#include "lsens.h"

//通信
#include "wifi.h"
#include "mqtt.h"
#include "control.h"


///////////////////////////////////////////////////////////////////////////////////////////////

char *cmdLED0_On  = "LED0ON";      //LED0打开
char *cmdLED0_Off = "LED0OFF";     //LED0关闭
char *cmdLED1_On  = "LED1ON";      //LED1打开
char *cmdLED1_Off = "LED1OFF";     //LED1关闭

char *cmdDHT11_On  = "DHTON";    //温湿度数据传输打开
char *cmdDHT11_Off = "DHTOFF";   //温湿度数据传输关闭

char *cmdServo_On1  = "SERVOON1";     //SERVO打开1
char *cmdServo_On2  = "SERVOON2";     //SERVO打开2
char *cmdServo_On3  = "SERVOON3";     //SERVO打开3
char *cmdServo_On4  = "SERVOON4";     //SERVO打开4
char *cmdServo_Off  = "SERVOOFF";     //SERVO关闭

char *cmdNum  = "N";       //舵机角度控制数字

///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////

char *led0Flag  = "LED0OFF";       //LED0状态
char *led1Flag  = "LED1OFF";       //LED1状态

char *dhtFlag   = "DHTON";		     //DHT状态

char *servoFlag = "SERVOOFF";      //SERVO状态

int   Num       = 0;               //舵机输入角度

int   key                    = 0;               //按键状态
int   tpad                   = 0;               //触摸按键状态
int   tpad_mode_control      = 0;               //触摸按键模式控制状态
int   tpad_mode_control_flag = 0;               //触摸按键模式控制状态标志

///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////

u8 adcx;			                     //存放光敏传感器数值
int light_sensor_threshold = 50;   //LED自动控制阈值
extern int temperature_threshold;  //温度报警阈值
extern int humidity_threshold;     //湿度报警阈值

///////////////////////////////////////////////////////////////////////////////////////////////


int main(void) 
{	
	u8 lcd_id[12];			                             //存放LCD ID字符串
	Delay_Init();                                    //延时功能初始化
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	
	Usart1_Init(115200);                             //串口1功能初始化,波特率115200  串口调试
	Usart2_Init(115200);                             //串口2功能初始化,波特率115200	WIFI通信 MQTT通信
	
	servo_init(1999, 719);                           //定时器1（舵机）初始化
	servo_off();                                     //舵机初始位置设置为0°
	TIM4_Init(500,7200);                             //TIM4初始化,定时时间 500*7200*1000/72000000 = 50ms MQTT通信数据接收
	
	LCD_Init();                                      //LCD初始化
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);  //将LCD ID打印到lcd_id数组。
	LCD_Clear(WHITE);                                //LCD清屏
	LED_Init();			                                 //LED初始化
	BEEP_Init();                                     //BEEP初始化
	KEY_Init();			                                 //按键初始化
	TPAD_Init(6);			                               //触摸按键初始化
	DHT11_Init();                                    //DHT11初始化
	Lsens_Init(); 			                             //初始化光敏传感器
	
	WiFi_ResetIO_Init();                             //初始化WiFi的复位IO
	IoT_Parameter_Init();                            //初始化云IoT平台MQTT服务器的参数	
	
	//LCD标题
	POINT_COLOR=BLACK;LCD_ShowString(30,30,240,24,24,"OneNET Smart Home");
//	POINT_COLOR=BLUE ;LCD_ShowString(380,30,210,24,24,"Y S U");
//	POINT_COLOR=BLACK;LCD_ShowString(30,70,240,24,24,"Yanshan University");
	POINT_COLOR=BLACK;LCD_ShowString(30,70,200,16,16,"Liu Lianjun 202011030312");
  POINT_COLOR=BLACK;LCD_ShowString(30,90,200,16,16,"Gu  Jinxuan 202011030342");
	
	//设备状态
	POINT_COLOR=BLUE; LCD_ShowString(30,120,200,24,24,"WiFi State:");   //WiFi状态
	POINT_COLOR=BLUE; LCD_ShowString(170,120,200,24,24,"Connecting");
	POINT_COLOR=BLACK;LCD_ShowString(30,150,200,24,24,"LED Control Mode:");//LED控制状态
	POINT_COLOR=RED;  LCD_ShowString(240,150,200,24,24,"Hand");
  POINT_COLOR=BLACK;LCD_ShowString(30+200+40,180,200,24,24,"Servo:");   //Servo状态
	POINT_COLOR=RED;  LCD_ShowString(110+200+40,180,200,24,24,"Off ");
	POINT_COLOR=BLACK;LCD_ShowString(30,180,200,24,24,"Beep :");       //Beep状态
	POINT_COLOR=RED;  LCD_ShowString(110,180,200,24,24,"Off");
	POINT_COLOR=BLACK;LCD_ShowString(30,210,200,24,24,"LED0 :");       //LED0状态
	POINT_COLOR=RED;  LCD_ShowString(110,210,200,24,24,"Off");
	POINT_COLOR=BLACK;LCD_ShowString(30+200+40,210,200,24,24,"LED1 :");   //LED1状态
	POINT_COLOR=RED;  LCD_ShowString(110+200+40,210,200,24,24,"Off");
	POINT_COLOR=BLACK;LCD_ShowString(30,240,200,24,24,"Lsens:");       //DHT状态
	POINT_COLOR=GREEN;LCD_ShowString(110,240,200,24,24,"On ");
	POINT_COLOR=BLACK;LCD_ShowString(30+200+40,240,200,24,24,"DHT  :");   //DHT状态
	POINT_COLOR=GREEN;LCD_ShowString(110+200+40,240,200,24,24,"On ");
	
	//光敏传感器数值显示
	POINT_COLOR=BLACK;LCD_ShowString(30,270,200,24,24,"LsensVal:");
	POINT_COLOR=BLACK;LCD_ShowString(240,275,200,16,16,"Light Sensor Threshold:");
	POINT_COLOR=RED;  LCD_ShowNum(430,270,light_sensor_threshold,2,24);
	
	//温湿度显示
	POINT_COLOR=BLACK;LCD_ShowString(30,270+30,200,24,24,"Temperature:");   //温度显示
	POINT_COLOR=BLACK;LCD_ShowString(210,270+30,200,24,24,"*");
	POINT_COLOR=BLACK;LCD_ShowString(240,270+35,200,16,16,"Temperature Threshold:");
	POINT_COLOR=RED;  LCD_ShowNum(420,270+30,temperature_threshold,2,24);
	POINT_COLOR=BLACK;LCD_ShowString(450,270+35,20,16,16,"*");
	
	POINT_COLOR=BLACK;LCD_ShowString(30,300+30,200,24,24,"Humidity:");      //湿度显示
	POINT_COLOR=BLACK;LCD_ShowString(170,300+30,200,24,24,"%");
	POINT_COLOR=BLACK;LCD_ShowString(240,300+35,200,16,16,"Humidity Threshold:");
	POINT_COLOR=RED;  LCD_ShowNum(395,300+30,humidity_threshold,2,24);
	POINT_COLOR=BLACK;LCD_ShowString(425,300+35,20,16,16,"%");
	
	//温湿度绘图  (60,370),(460,670) 400*300
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
	POINT_COLOR=RED;   LCD_ShowString(50+20, 685,100,16,16,"--Temperature"); //图例 Temperature
	POINT_COLOR=BLUE;  LCD_ShowString(200+30,685,100,16,16,"--Humidity");    //图例 Humidity
	POINT_COLOR=GREEN; LCD_ShowString(350+20,685,100,16,16,"--LightVal");    //图例 LightVal
	while(1)
	{	
		if(connectFlag == 1)// connectFlag=1同服务器建立了连接
		{
			POINT_COLOR=BLUE;LCD_ShowString(170,120,200,24,24,"On        "); //WiFi状态更新
			
			key = KEY_Scan(0);//获取按键状态
			tpad = TPAD_Scan(0);//获取触摸按键状态
			
			if(tpad)//更改LED控制模式
			{
				tpad_mode_control_flag = 1;//悬挂控制状态更改标志位
				tpad_mode_control = 1 - tpad_mode_control;
			}
			
			if(tpad_mode_control == 1 && tpad_mode_control_flag ==1)//LCD显示LED控制模式
			{
				POINT_COLOR=GREEN;  LCD_ShowString(240,150,200,24,24,"Auto");//LED自动控制模式
				tpad_mode_control_flag = 0;//解除控制状态更改标志位
			}
			else if(tpad_mode_control == 0 && tpad_mode_control_flag ==1)
			{
				POINT_COLOR=RED;    LCD_ShowString(240,150,200,24,24,"Hand");//LED手动控制模式
				tpad_mode_control_flag = 0;//解除控制状态更改标志位
			}
			
			//处理发送缓冲区数据
			if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr)
			{                
				//MQTT通讯三个流程
				//1：0x10 连接报文
				//2：0x82 订阅报文,且connectPackFlag置位,表示连接报文成功
				//3：subcribePackFlag置位,说明连接和订阅均成功,其他报文可发
				if((MQTT_TxDataOutPtr[2] == 0x10)||((MQTT_TxDataOutPtr[2] == 0x82)&&(connectPackFlag == 1))||(subcribePackFlag == 1)) 
				{    
					u1_printf("\r\n发送数据:0x%x\r\n", MQTT_TxDataOutPtr[2]); //串口输出信息
					MQTT_TxData(MQTT_TxDataOutPtr);										        //发送数据
					MQTT_TxDataOutPtr += TBUFF_UNIT;									        //指针下移
					if(MQTT_TxDataOutPtr == MQTT_TxDataEndPtr)				        //如果指针到缓冲区尾部了
					{ 
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];		    	        //指针归位到缓冲区开头
					}	
				}				
			}
			
			
			//处理接收缓冲区数据
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr)
			{
				u1_printf("\r\n接收数据:");
				
				//处理CONNACK报文
				if(MQTT_RxDataOutPtr[2] == 0x20)//if判断,如果第一个字节是0x20,表示收到的是CONNACK报文
				{             			
					switch(MQTT_RxDataOutPtr[5])//接判断第4个字节,看CONNECT报文是否成功
					{					   
						case 0x00 : u1_printf("CONNECT报文成功\r\n");	                        //串口输出信息	
										connectPackFlag = 1;//CONNECT报文成功,订阅报文可发
									break;
						case 0x01 : u1_printf("连接已拒绝,不支持的协议版本,准备重启\r\n");    //串口输出信息
									connectFlag = 0;//connectFlag置零,重启连接
									break;
						case 0x02 : u1_printf("连接已拒绝,不合格的客户端标识符,准备重启\r\n");//串口输出信息
									connectFlag = 0;//connectFlag置零,重启连接
									break;
						case 0x03 : u1_printf("连接已拒绝,服务端不可用,准备重启\r\n");	      //串口输出信息
									connectFlag = 0;//connectFlag置零,重启连接
									break;
						case 0x04 : u1_printf("连接已拒绝,无效的用户名或密码,准备重启\r\n");	//串口输出信息
									connectFlag = 0;//connectFlag置零,重启连接						
									break;
						case 0x05 : u1_printf("连接已拒绝,未授权,准备重启\r\n");			      	//串口输出信息
									connectFlag = 0;//connectFlag置零,重启连接						
									break;
						default   : u1_printf("连接已拒绝,未知状态,准备重启\r\n");		        //串口输出信息 
									connectFlag = 0;//connectFlag置零,重启连接					
									break;
					}
			  }
				
				//处理SUBACK报文
				else if(MQTT_RxDataOutPtr[2] == 0x90)//if判断,第一个字节是0x90,表示收到的是SUBACK报文
				{ 
					switch(MQTT_RxDataOutPtr[6])//接着我们要判断订阅回复,看看是不是成功
					{					
						case 0x00 :
						case 0x01 : 
									u1_printf("订阅成功\r\n");			//串口输出信息
									subcribePackFlag = 1;				    //subcribePackFlag置1,表示订阅报文成功,其他报文可发送
									pingFlag = 0;						        //pingFlag清零
									TIM3_ENABLE_30S();					    //启动30s的PING定时器	
									u1_printf("\r\n");              //串口输出设备初始信息
									u1_printf("************************************\r\n");
									Send_Data_LED0();               //向服务器发送LED0初始信息
									Send_Data_LED1();               //向服务器发送LED1初始信息
									Send_Data_Servo();              //向服务器发送SERVO初始信息
									Send_Data_DHT();                //向服务器发送DHT初始信息
									u1_printf("************************************\r\n");
									TIM2_ENABLE();                  //默认开启DHT
									TIM8_ENABLE();                  //默认开启Lsens
									break;
						default: 
									u1_printf("订阅失败,准备重启\r\n");//串口输出信息 
									connectFlag = 0;//connectFlag置零,重启连接
									break;
						}					
			    }
					
					//处理PINGRESP报文
					else if(MQTT_RxDataOutPtr[2] == 0xD0)//if判断,第一个字节是0xD0,表示收到的是PINGRESP报文
					{ 
						u1_printf("PING报文回复\r\n");  //串口输出信息 
						if(pingFlag == 1)    //如果pingFlag=1,表示第一次发送
						{                   						        
							 pingFlag = 0;     //清除pingFlag标志
						}
						else if(pingFlag > 1)//如果pingFlag>1,表示是多次发送了,而且是2s间隔的快速发送
						{ 				 									
							pingFlag = 0;      //清除pingFlag标志
							TIM3_ENABLE_30S(); //PING定时器重回30s的时间
						}				
					}	
					
					//处理服务器推送数据报文
					else if((MQTT_RxDataOutPtr[2] == 0x30))//if判断,如果第一个字节是0x30,表示收到的是服务器发来的推送数据
					{ 
						u1_printf("服务器等级0推送\r\n"); 		   	  //串口输出信息 
						MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);   //处理等级0推送数据,提取控制命令
					}									
					MQTT_RxDataOutPtr += RBUFF_UNIT;              //指针下移
					if(MQTT_RxDataOutPtr == MQTT_RxDataEndPtr)    //如果指针到缓冲区尾部了
					{
						MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];      //指针归位到缓冲区开头              
					}
				}
			
			
				//处理命令缓冲区数据 接收缓冲区接收到数据或有按键按下触发
				if((MQTT_CMDOutPtr != MQTT_CMDInPtr) || key == 1 || key == 2 || key == 3 || key == 4)
				{                             		       
					u1_printf("\r\n命令:");  //串口输出信息
					
					int state = 0; //命令类型变量
					
					//判断从服务器接收到的命令类型
					if((!memcmp(&MQTT_CMDOutPtr[2],cmdLED0_On,strlen(cmdLED0_On))) && tpad_mode_control == 0)//手动控制模式有效
					{
						state = 1;//LED0打开命令
					}
					else if((!memcmp(&MQTT_CMDOutPtr[2],cmdLED0_Off,strlen(cmdLED0_Off))) && tpad_mode_control == 0)//手动控制模式有效
					{
						state = 2;//LED0关闭命令
					}
					else if((!memcmp(&MQTT_CMDOutPtr[2],cmdLED1_On,strlen(cmdLED1_On))) && tpad_mode_control == 0)//手动控制模式有效
					{
						state = 3;//LED1打开命令
					}
					else if((!memcmp(&MQTT_CMDOutPtr[2],cmdLED1_Off,strlen(cmdLED1_Off))) && tpad_mode_control == 0)//手动控制模式有效
					{ 
						state = 4;//LED1关闭命令
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdDHT11_On,strlen(cmdDHT11_On)))
					{ 
						state = 5;//DHT打开命令
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdDHT11_Off,strlen(cmdDHT11_Off)))
					{ 
						state = 6;//DHT关闭命令
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdServo_On1,strlen(cmdServo_On1)))
					{      
						state = 7;//SERVO置45°命令
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdServo_On2,strlen(cmdServo_On2)))
					{      
						state = 8;//SERVO置90°命令
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdServo_On3,strlen(cmdServo_On3)))
					{
						state = 9;//SERVO置135°命令
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdServo_On4,strlen(cmdServo_On4)))
					{      
						state = 10;//SERVO置180°命令
					}
					else if(!memcmp(&MQTT_CMDOutPtr[2],cmdServo_Off,strlen(cmdServo_Off)))
					{
						state = 11;//SERVO置0°命令
					}
//					else if(!memcmp(&MQTT_CMDOutPtr[2], cmdNum, strlen(cmdNum)))
//					{
//						state = 12;//SERVO置0°命令
//					}
					else if(MQTT_CMDOutPtr[2] == 'N' && MQTT_CMDOutPtr[3] == ':')
					{
						state = 12;//SERVO置0°命令
					}
					else
					{
						state = 0;//无命令
					}
					
					//判断按键状态
					switch(key)
					{
						case 1:
						{
							if((!memcmp(led0Flag,cmdLED0_Off,strlen(cmdLED0_Off))) && tpad_mode_control == 0)//LED0状态为关闭且按键1按下,且为手动控制模式
							{
								state = 1;//LED0打开命令
							}
							else if((!memcmp(led0Flag,cmdLED0_On,strlen(cmdLED0_On))) && tpad_mode_control == 0)//LED0状态为打开且按键1按下,且为手动控制模式
							{
								state = 2;//LED0关闭命令
							}
							else
							{
								state = 0;//无命令
							}
							break;
						}
						case 2:
						{
							if((!memcmp(led1Flag,cmdLED1_Off,strlen(cmdLED1_Off))) && tpad_mode_control == 0)//LED1状态为关闭且按键2按下,且手动到控制模式
							{
								state = 3;//LED1打开命令
							}
							else if((!memcmp(led1Flag,cmdLED1_On,strlen(cmdLED1_On))) && tpad_mode_control == 0)//LED1状态为打开且按键1按下,且手动到控制模式
							{
								state = 4;//LED1关闭命令
							}
							else
							{
								state = 0;//无命令
							}
							break;
						}
						case 3:
						{
							if(!memcmp(dhtFlag,cmdDHT11_Off,strlen(cmdDHT11_Off)))//DHT状态为关闭且按键3按下
							{
								state = 5;//DHT打开命令
							}
							else
							{
								state = 6;//DHT关闭命令
							}
							break;
						}
						case 4:
						{
							if(!memcmp(servoFlag,cmdServo_Off,strlen(cmdServo_Off)))//SERVO状态为关闭且按键4按下
							{
								state = 7;//SERVO置45°命令
							}
							else if(!memcmp(servoFlag,cmdServo_On1,strlen(cmdServo_On1)))
							{
								state = 8;//SERVO置90°命令
							}
							else if(!memcmp(servoFlag,cmdServo_On2,strlen(cmdServo_On2)))
							{
								state = 9;//SERVO置135°命令
							}
							else if(!memcmp(servoFlag,cmdServo_On3,strlen(cmdServo_On3)))
							{
								state = 10;//SERVO置180°命令
							}
							else
							{
								state = 11;//SERVO置0°命令
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
						led0Flag = "LED0ON"; 			//更改LED0状态
						u1_printf("LED0ON\r\n");  //串口输出信息
						POINT_COLOR=GREEN;LCD_ShowString(110,210,200,24,24,"On "); //更新LCD的LED0状态
						LED0_ON();                //打开LED0
						Send_Data_LED0();         //向服务器更新LED0状态
						break;
						}
						case 2:
						{
						led0Flag = "LED0OFF"; 		//更改LED0状态
						u1_printf("LED0OFF\r\n"); //串口输出信息
						POINT_COLOR=RED;LCD_ShowString(110,210,200,24,24,"Off"); //更新LCD的LED0状态
						LED0_OFF();               //关闭LED0
						Send_Data_LED0();         //向服务器更新LED0状态
						break;
						}
						case 3:
						{
						led1Flag = "LED1ON"; 		  //更改LED1状态
						u1_printf("LED1ON\r\n");  //串口输出信息
						POINT_COLOR=GREEN;LCD_ShowString(110+200+40,210,200,24,24,"On "); //更新LCD的LED1状态
						LED1_ON();                //打开LED1
						Send_Data_LED1();         //向服务器更新LED1状态
						break;
						}
						case 4:
						{
						led1Flag = "LED1OFF"; 		//更改LED1状态
						u1_printf("LED1OFF\r\n"); //串口输出信息
						POINT_COLOR=RED;LCD_ShowString(110+200+40,210,200,24,24,"Off"); //更新LCD的LED1状态
						LED1_OFF();               //关闭LED1
						Send_Data_LED1();         //向服务器更新LED1状态
						break;
						}
						case 5:
						{
						dhtFlag = "DHTON"; 			  //更改DHT状态
						u1_printf("DHTON\r\n");   //串口输出信息
						POINT_COLOR=GREEN;LCD_ShowString(110+200+40,240,200,24,24,"On "); //更新LCD的DHT状态
						TIM2_ENABLE();            //打开DHT
						Send_Data_DHT();          //向服务器更新DHT状态
						break;
						}
						case 6:
						{
						dhtFlag = "DHTOFF";			  //更改DHT状态
						u1_printf("DHTOFF\r\n");  //串口输出信息
						POINT_COLOR=RED;LCD_ShowString(110+200+40,240,200,24,24,"Off"); //更新LCD的DHT状态
						TIM_Cmd(TIM2,DISABLE);    //关闭DHT
						Send_Data_DHT();          //向服务器更新DHT状态
						break;
						}
						case 7:
						{
						servoFlag = "SERVOON1";		//更改SERVO状态
						u1_printf("SERVOON1\r\n");//串口输出信息
						POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"45* "); //更新LCD的SERVO状态
						Send_Data_Servo();        //向服务器更新SERVO状态
						servo_on_45();            //SERVO置45°
						Num=45;
						break;
						}
						case 8:
						{
						servoFlag = "SERVOON2";		//更改SERVO状态
						u1_printf("SERVOON2\r\n");//串口输出信息
						POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"90* "); //更新LCD的SERVO状态
						Send_Data_Servo();        //向服务器更新SERVO状态
						servo_on_90();            //SERVO置90°
						Num=90;
						break;
						}
						case 9:
						{
						servoFlag = "SERVOON3";		//更改SERVO状态
						u1_printf("SERVOON3\r\n");//串口输出信息
						POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"135*"); //更新LCD的SERVO状态
						Send_Data_Servo();        //向服务器更新SERVO状态
						servo_on_135();           //SERVO置135°
						Num=135;
						break;
						}
						case 10:
						{
						servoFlag = "SERVOON4";		//更改SERVO状态
						u1_printf("SERVOON4\r\n");//串口输出信息
						POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"180*"); //更新LCD的SERVO状态
						Send_Data_Servo();        //向服务器更新SERVO状态
						servo_on_180();           //SERVO置180°
						Num=180;
						break;
						}
						case 11:
						{
						servoFlag = "SERVOOFF";		//更改SERVO状态
						u1_printf("SERVOOFF\r\n");//串口输出信息
						POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"Off "); //更新LCD的SERVO状态
						Send_Data_Servo();        //向服务器更新SERVO状态
						servo_off();              //SERVO置0°
						Num=0;
						break;
						}
						case 12:
						{
						//解析服务器发送的含数字命令
						int first_num = 0;
						int second_num = 0;
						int third_num = 0;
						int second_num_flag = 0;//二位数标识
						int third_num_flag = 0; //三位数标识
							
						switch(MQTT_CMDOutPtr[4])//解析第一位
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
						
						switch(MQTT_CMDOutPtr[5])//解析第二位
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
						
						switch(MQTT_CMDOutPtr[6])//解析第三位
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
						
						//计算服务器发送的数字
						if(second_num_flag == 0 && third_num_flag == 0)
						{
							Num = first_num;
							POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"    "); //更新LCD的SERVO状态
							POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24," *");   //更新LCD的SERVO状态
							POINT_COLOR=RED;LCD_ShowNum(110+200+40,180,Num,1,24);            //更新LCD的SERVO状态
						}
						else if(second_num_flag == 1 && third_num_flag == 0)
						{
							Num = first_num*10 + second_num;
							POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"    "); //更新LCD的SERVO状态
							POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"  *");  //更新LCD的SERVO状态
							POINT_COLOR=RED;LCD_ShowNum(110+200+40,180,Num,2,24);            //更新LCD的SERVO状态
						}
						else
						{
							Num = first_num*100 + second_num*10 + third_num;
							if(Num>180){Num = 180;}//防止输入数据过大
							POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"    "); //更新LCD的SERVO状态
							POINT_COLOR=RED;LCD_ShowString(110+200+40,180,200,24,24,"   *"); //更新LCD的SERVO状态
							POINT_COLOR=RED;LCD_ShowNum(110+200+40,180,Num,3,24);            //更新LCD的SERVO状态
						}
						
						u1_printf("Num:%d\r\n",Num); //串口输出信息
						
						int Set_Num = 0;
						Set_Num = 1950 - (int)(200 * (float)((float)Num / 180.0));//计算舵机需要设置的数值
						
						//向服务器更新舵机状态
						char head1[3];
						char temp[50];				//定义一个临时缓冲区1,不包括报头
						char tempAll[100];		//定义一个临时缓冲区2，包括所有数据
						
						int	dataLen = 0;			//报文长度
						
						memset(temp,    0, 50);				    //清空缓冲区1
						memset(tempAll, 0, 100);				  //清空缓冲区2
						memset(head1,   0, 3);					  //清空MQTT头
						
						//更新固定控制按钮状态
						sprintf(temp,"{\"servoFlag\":\"SERVOOFF\"}");//构建报文
						
						head1[0] = 0x03; 						      //固定报头
						head1[1] = 0x00; 					      	//固定报头
						head1[2] = strlen(temp);  				//剩余长度	
						sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
						
						//u1_printf("servoFlag:SERVOOFF\r\n");
						
						dataLen = strlen(temp) + 3;
						
						MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen);//添加数据，发布给服务器
						
						memset(temp,    0, 50);				    //清空缓冲区1
						memset(tempAll, 0, 100);				  //清空缓冲区2
						memset(head1,   0, 3);					  //清空MQTT头
						
						//更新自由控制旋钮状态
						sprintf(temp,"{\"Num\":\"%d\"}", Num);//构建报文
						
						head1[0] = 0x03; 						      //固定报头
						head1[1] = 0x00; 					      	//固定报头
						head1[2] = strlen(temp);  				//剩余长度	
						sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
						
						//u1_printf("\r\n HouseKeeper: Num:%d \r\n", Num);
						
						dataLen = strlen(temp) + 3;
						
						MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen);//添加数据，发布给服务器
						
						TIM_SetCompare1(TIM1,Set_Num);//舵机任意角度旋转
						DelayMs(1000);//延时1s
						break;
						}
						default:
						{
						u1_printf("**未知指令**%s\r\n", &MQTT_CMDOutPtr[2]); //串口输出信息
						break;
						}
					}
					if(MQTT_CMDOutPtr != MQTT_CMDInPtr)
					{
						MQTT_CMDOutPtr += CBUFF_UNIT;				      //指针下移
						if(MQTT_CMDOutPtr == MQTT_CMDEndPtr)	    //如果指针到缓冲区尾部了
							MQTT_CMDOutPtr = MQTT_CMDBuf[0];			  //指针归位到缓冲区开头	
					}
				}
			}
			
			else//connectFlag=0,同服务器断开了连接,重启连接服务器
			{ 
				POINT_COLOR=BLUE;LCD_ShowString(170,120,200,24,24,"Off       ");
				u1_printf("\r\n\r\n------------------------------------\r\n");
				u1_printf("设备未联网,连接服务器\r\n");        //串口输出信息
				TIM_Cmd(TIM3, DISABLE);                        //关闭TIM3  心跳包发送失效
				TIM_Cmd(TIM4, DISABLE);                        //关闭TIM4  MQTT接收失效
				WiFi_RxCounter = 0;                            //WiFi接收数据量变量清零                        
				memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);      //清空WiFi接收缓冲区 
				if(WiFi_Connect_IoTServer() == 0)			         //如果WiFi连接云服务器函数返回0,表示正确,进入if
				{   			     
					u1_printf("建立TCP连接成功\r\n");            //串口输出信息
					u1_printf("------------------------------------\r\n");
					connectFlag = 1;                             //connectFlag置1,表示连接成功	
					WiFi_RxCounter = 0;                          //WiFi接收数据量变量清零                        
					memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);    //清空WiFi接收缓冲区 
					MQTT_Buff_Init();                            //初始化发送缓冲区                    
				}
			}
	}
}
