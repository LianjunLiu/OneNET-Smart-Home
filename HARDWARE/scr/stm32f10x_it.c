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

char humidity;				//定义一个变量，保存湿度值
char temperature;			//定义一个变量，保存温度值	
extern u8 adcx;       //存放光敏传感器数值

int Array_Num = 41;    //LCD温湿度曲线图像点数
int T_Aaary[41] = {0}; //温度数组
int H_Aaary[41] = {0}; //湿度数组
int L_Aaary[41] = {0}; //光强数组
int step = 0;          //LCD温湿度曲线绘制步长

int temperature_threshold = 30; //温度报警阈值
int humidity_threshold = 50;    //湿度报警阈值
extern int light_sensor_threshold;   //LED自动控制阈值

extern char *led0Flag;               //LED0状态
extern char *led1Flag;               //LED1状态
extern int   tpad_mode_control;      //触摸按键模式控制状态

extern int   Num;                    //舵机输入角度

//串口2接收中断函数（处理接收数据）
void USART2_IRQHandler(void)   
{                      
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)    //如果USART_IT_RXNE标志置位，表示有数据到了，进入if分支
	{  
		if(connectFlag == 0) 							                     //如果connectFlag等于0，当前还没有连接服务器，处于指令配置状态
		{
			if(USART2->DR)
			{                                     			         //处于指令配置状态时，非零值才保存到缓冲区	
				Usart2_RxBuff[Usart2_RxCounter] = USART2->DR;      //保存到缓冲区	
				Usart2_RxCounter++; 					                   	 //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
			}					
		}
		else
		{		                                           	       //反之connectFlag等于1，连接上服务器了	
			Usart2_RxBuff[Usart2_RxCounter] = USART2->DR;        //把接收到的数据保存到Usart2_RxBuff中				
			if(Usart2_RxCounter == 0)
			{    									    										     	 //如果Usart2_RxCounter等于0，表示是接收的第1个数据，进入if分支				
				TIM_Cmd(TIM4, ENABLE); 
			}
			else																					    	 //else分支，表示果Usart2_RxCounter不等于0，不是接收的第一个数据
			{                        									    
				TIM_SetCounter(TIM4, 0);  
			}
			Usart2_RxCounter++;         				                 //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
		}
	}
} 

//定时器4中断服务函数,处理MQTT数据
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//如果TIM_IT_Update置位，表示TIM4溢出中断，进入if	
	{
		memcpy(&MQTT_RxDataInPtr[2], Usart2_RxBuff, Usart2_RxCounter);  //拷贝数据到接收缓冲区
		MQTT_RxDataInPtr[0] = Usart2_RxCounter/256;                   	//记录数据长度高字节
		MQTT_RxDataInPtr[1] = Usart2_RxCounter%256;					          	//记录数据长度低字节
		MQTT_RxDataInPtr += RBUFF_UNIT;                                	//指针下移
		if(MQTT_RxDataInPtr == MQTT_RxDataEndPtr)                     	//如果指针到缓冲区尾部了
			MQTT_RxDataInPtr = MQTT_RxDataBuf[0];                         //指针归位到缓冲区开头
		Usart2_RxCounter = 0;                                         	//串口2接收数据量变量清零
		TIM_SetCounter(TIM3, 0);                                      	//清零定时器3计数器，重新计时ping包发送时间
		TIM_Cmd(TIM4, DISABLE);                        				        	//关闭TIM4定时器
		TIM_SetCounter(TIM4, 0);                        			        	//清零定时器4计数器
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     			        	//清除TIM4溢出中断标志 	
	}
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //如果TIM_IT_Update置位，表示TIM3溢出中断，进入if	
		{  
		switch(pingFlag) 					                      //判断pingFlag的状态
		{                               
			case 0:							                          //如果pingFlag等于0，表示正常状态，发送Ping报文  
					MQTT_PingREQ(); 		                      //添加Ping报文到发送缓冲区  
					break;
			case 1:							                          //如果pingFlag等于1，说明上一次发送到的ping报文，没有收到服务器回复，所以1没有被清除为0，可能是连接异常，我们要启动快速ping模式
					TIM3_ENABLE_2S(); 	                      //我们将定时器3设置为2s定时,快速发送Ping报文
					MQTT_PingREQ();			                      //添加Ping报文到发送缓冲区  
					break;
			case 2:						                          	//如果pingFlag等于2，说明还没有收到服务器回复
					MQTT_PingREQ();			                      //添加Ping报文到发送缓冲区  
					break;
			case 3:				      										      //如果pingFlag等于3，说明还没有收到服务器回复
					MQTT_PingREQ();			                      //添加Ping报文到发送缓冲区
					break;
			case 4:				     										        //如果pingFlag等于4，说明还没有收到服务器回复	
					MQTT_PingREQ();  													//添加Ping报文到发送缓冲区 
					break;
			case 5:																				//如果pingFlag等于5，说明我们发送了多次ping，均无回复，应该是连接有问题，我们重启连接
					connectFlag = 0;     									    //连接状态置0，表示断开，没连上服务器
					TIM_Cmd(TIM3, DISABLE); 									//关TIM3 				
					break;			
		}
		pingFlag++;           		   										//pingFlag自增1，表示又发送了一次ping，期待服务器的回复
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     //清除TIM3溢出中断标志 	
	}
}

//定时器2中断服务函数
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)	
	{
		DHT11_Read_Data(&temperature,&humidity);//读取温湿度值
		
		if(temperature<=temperature_threshold)//判断温度是否超过阈值
		{
			POINT_COLOR=GREEN;LCD_ShowNum(180,270+30,temperature,2,24);	  //LCD显示温度
		}
		else
		{
			POINT_COLOR=RED;LCD_ShowNum(180,270+30,temperature,2,24);	    //LCD显示温度
		}
		
		if(humidity<=humidity_threshold)//判断湿度是否超过阈值
		{
			POINT_COLOR=GREEN;LCD_ShowNum(140,300+30,humidity,2,24);		  //LCD显示湿度
		}
		else
		{
			POINT_COLOR=RED;LCD_ShowNum(140,300+30,humidity,2,24);		    //LCD显示湿度
		}
		
		//温湿度数组前移一位
		for(int i=0;i<(Array_Num-1);i++)
		{
			T_Aaary[i]=T_Aaary[i+1];
			H_Aaary[i]=H_Aaary[i+1];
			L_Aaary[i]=L_Aaary[i+1];
		}
		
		T_Aaary[Array_Num-1]=temperature; //写入温度数组最后一位
		H_Aaary[Array_Num-1]=humidity;    //写入湿度数组最后一位
		L_Aaary[Array_Num-1]=adcx;    //写入湿度数组最后一位
		
		step = 400 / (Array_Num-1);       //计算绘图步长
		
		LCD_Fill(61, 371, 459,669, WHITE);//删除矩形内部曲线
		
		//绘制温湿度曲线
		for(int i=0;i<(Array_Num-1);i++)
		{
			POINT_COLOR=RED;LCD_DrawLine(60 + step*i, 670-T_Aaary[i]*3, 60 + step*(i+1), 670-T_Aaary[i+1]*3);
			POINT_COLOR=BLUE;LCD_DrawLine(60 + step*i, 670-H_Aaary[i]*3, 60 + step*(i+1), 670-H_Aaary[i+1]*3);
			POINT_COLOR=GREEN;LCD_DrawLine(60 + step*i, 670-L_Aaary[i]*3, 60 + step*(i+1), 670-L_Aaary[i+1]*3);
		}
		
		POINT_COLOR=BLACK;LCD_DrawRectangle(60, 370, 460,670);//填充图像边框
		
		if (temperature > temperature_threshold || humidity > humidity_threshold)//如果温度或湿度超过报警阈值
		{
			if(temperature > temperature_threshold)//如果温度超过报警阈值
			{
				POINT_COLOR=RED;LCD_ShowString(30,330+380,420,24,24,"Warnning:Temperature is too high!!!"); //LCD提示报警信息
				u1_printf("\r\nWarnning:Temperature is too high!!!\r\n");                                   //串口输出报警信息
			}
			if(humidity > humidity_threshold)//如果湿度超过报警阈值
			{
				POINT_COLOR=RED;LCD_ShowString(30,360+380,420,24,24,"Warnning:Humidity is too high!!!");    //LCD提示报警信息
				u1_printf("\r\nWarnning:Humidity is too high!!!\r\n");                                      //串口输出报警信息
			}
			POINT_COLOR=GREEN;LCD_ShowString(110,180,200,24,24,"On "); //LCD更新蜂鸣器状态
			u1_printf("\r\nbeepFlag:BEEPON\r\n");                    //串口输出蜂鸣器状态
			Send_Data_Beep_ON();                                     //向服务器更新BEEP状态
			Warning();                                               //蜂鸣器报警
	  }
		
		if(temperature <= temperature_threshold)//如果温度低于报警阈值
		{
			LCD_ShowString(30,330+380,420,24,24,"                                   ");//LCD删除温度报警信息
		}
		
		if(humidity <= humidity_threshold)//如果湿度低于报警阈值
		{
		  LCD_ShowString(30,360+380,420,24,24,"                                   ");//LCD删除湿度报警信息
		}
		
		if(temperature <= temperature_threshold && humidity <= humidity_threshold)//如果温湿度都没超过报警阈值
		{
			POINT_COLOR=RED;LCD_ShowString(110,180,200,24,24,"Off");  //LCD更新BEEP状态
			Send_Data_Beep_OFF();                                     //向服务器更新BEEP状态
			//u1_printf("\r\nbeepFlag:BEEPOFF\r\n");
		}
		
		char head1[3];
		char temp[50];				//定义一个临时缓冲区1,不包括报头
		char tempAll[100];		//定义一个临时缓冲区2，包括所有数据
		
		int	dataLen = 0;			//报文长度
		memset(temp,    0, 50);				    //清空缓冲区1
		memset(tempAll, 0, 100);				  //清空缓冲区2
		memset(head1,   0, 3);					  //清空MQTT头
		
		sprintf(temp,"{\"temperature\":\"%d%d\",\"humidity\":\"%d%d\"}", temperature/10, temperature%10, humidity/10, humidity%10);//构建报文
		
		head1[0] = 0x03; 						      //固定报头
		head1[1] = 0x00; 					      	//固定报头
		head1[2] = strlen(temp);  				//剩余长度	
		sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
		
		u1_printf("\r\n DHT: Temperature:%d%d; Humidity:%d%d \r\n", temperature/10, temperature%10, humidity/10, humidity%10);
		
		dataLen = strlen(temp) + 3;
		
		MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen);//添加数据，发布给服务器
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);   	
	}
}

void TIM8_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)	
	{
		adcx = Lsens_Get_Val();//获取光敏传感器数值
		
		//温湿度光强数组前移一位
		for(int i=0;i<(Array_Num-1);i++)
		{
			T_Aaary[i]=T_Aaary[i+1];
			H_Aaary[i]=H_Aaary[i+1];
			L_Aaary[i]=L_Aaary[i+1];
		}
		
		T_Aaary[Array_Num-1]=temperature; //写入温度数组最后一位
		H_Aaary[Array_Num-1]=humidity;    //写入湿度数组最后一位
		L_Aaary[Array_Num-1]=adcx;        //写入光强数组最后一位
		
		step = 400 / (Array_Num-1);       //计算绘图步长
		
		LCD_Fill(61, 371, 459,669, WHITE);//删除矩形内部曲线
		
		//绘制温湿度曲线
		for(int i=0;i<(Array_Num-1);i++)
		{
			POINT_COLOR=RED;LCD_DrawLine(60 + step*i, 670-T_Aaary[i]*3, 60 + step*(i+1), 670-T_Aaary[i+1]*3);
			POINT_COLOR=BLUE;LCD_DrawLine(60 + step*i, 670-H_Aaary[i]*3, 60 + step*(i+1), 670-H_Aaary[i+1]*3);
			POINT_COLOR=GREEN;LCD_DrawLine(60 + step*i, 670-L_Aaary[i]*3, 60 + step*(i+1), 670-L_Aaary[i+1]*3);
		}
		
		POINT_COLOR=BLACK;LCD_DrawRectangle(60, 370, 460,670);//填充图像边框
		
		if(adcx<=light_sensor_threshold)//判断光强是否低于阈值
		{
			POINT_COLOR=RED;LCD_ShowNum(140,270,adcx,2,24);//LCD显示光强
		}
		else
		{
			POINT_COLOR=GREEN;LCD_ShowNum(140,270,adcx,2,24);//LCD显示光强
		}
			
		if(tpad_mode_control)//LED自动控制模式
			{
				if(adcx<=light_sensor_threshold)
				{
					led0Flag = "LED0ON"; 			//更改LED0状态
					u1_printf("LED0ON\r\n");  //串口输出信息
					POINT_COLOR=GREEN;LCD_ShowString(110,210,200,24,24,"On "); //更新LCD的LED0状态
					LED0_ON();                //打开LED0
					if(subcribePackFlag == 1){Send_Data_LED0();}         //向服务器更新LED0状态
					led1Flag = "LED1ON"; 		  //更改LED1状态
					u1_printf("LED1ON\r\n");  //串口输出信息
					POINT_COLOR=GREEN;LCD_ShowString(110+200+40,210,200,24,24,"On "); //更新LCD的LED1状态
					LED1_ON();                //打开LED1
					if(subcribePackFlag == 1){Send_Data_LED1();}         //向服务器更新LED1状态
				}
				else
				{
					led0Flag = "LED0OFF"; 		//更改LED0状态
					u1_printf("LED0OFF\r\n"); //串口输出信息
					POINT_COLOR=RED;LCD_ShowString(110,210,200,24,24,"Off"); //更新LCD的LED0状态
					LED0_OFF();               //关闭LED0
					if(subcribePackFlag == 1){Send_Data_LED0();}         //向服务器更新LED0状态
					led1Flag = "LED1OFF"; 		//更改LED1状态
					u1_printf("LED1OFF\r\n"); //串口输出信息
					POINT_COLOR=RED;LCD_ShowString(110+200+40,210,200,24,24,"Off"); //更新LCD的LED1状态
					LED1_OFF();               //关闭LED1
					if(subcribePackFlag == 1){Send_Data_LED1();}         //向服务器更新LED1状态
				}
		}
		
		char head1[3];
		char temp[50];				//定义一个临时缓冲区1,不包括报头
		char tempAll[100];		//定义一个临时缓冲区2，包括所有数据
		
		int	dataLen = 0;			//报文长度
		
		memset(temp,    0, 50);				    //清空缓冲区1
		memset(tempAll, 0, 100);				  //清空缓冲区2
		memset(head1,   0, 3);					  //清空MQTT头
		
		sprintf(temp,"{\"LightVal\":\"%d%d\"}", adcx/10, adcx%10);//构建报文
		
		head1[0] = 0x03; 						      //固定报头
		head1[1] = 0x00; 					      	//固定报头
		head1[2] = strlen(temp);  				//剩余长度	
		sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
		
		u1_printf("\r\n Lsens: LightVal:%d%d \r\n", adcx/10, adcx%10);
		
		dataLen = strlen(temp) + 3;
		
		MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen);//添加数据，发布给服务器
				
		
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
		
		
		memset(temp,    0, 50);				    //清空缓冲区1
		memset(tempAll, 0, 100);				  //清空缓冲区2
		memset(head1,   0, 3);					  //清空MQTT头
		
		sprintf(temp,"{\"ControlMode\":\"%d\"}", tpad_mode_control);//构建报文
		
		head1[0] = 0x03; 						      //固定报头
		head1[1] = 0x00; 					      	//固定报头
		head1[2] = strlen(temp);  				//剩余长度	
		sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
		
		u1_printf("\r\n HouseKeeper: ControlMode:%d \r\n", tpad_mode_control);
		
		dataLen = strlen(temp) + 3;
		
		MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen);//添加数据，发布给服务器
		
		
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
		
		
		memset(temp,    0, 50);				    //清空缓冲区1
		memset(tempAll, 0, 100);				  //清空缓冲区2
		memset(head1,   0, 3);					  //清空MQTT头
		
		sprintf(temp,"{\"Num\":\"%d\"}", Num);//构建报文
		
		head1[0] = 0x03; 						      //固定报头
		head1[1] = 0x00; 					      	//固定报头
		head1[2] = strlen(temp);  				//剩余长度	
		sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
		
		u1_printf("\r\n HouseKeeper: Num:%d \r\n", Num);
		
		dataLen = strlen(temp) + 3;
		
		MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen);//添加数据，发布给服务器
		
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);   	
	}
}
