#include "stm32f10x.h"
#include "control.h"
#include "usart1.h"
#include "mqtt.h"

extern char *led0Flag;		//LED0状态
extern char *led1Flag;		//LED1状态
extern char *dhtFlag;     //DHT状态
extern char *servoFlag;   //SERVO状态

//数据长度 length 函数
int length(int data)
{
	int len = 1;
	while(data/10 != 0)
	{
		len++;
		data = data/10;
	}
	return len;
}

//发送控制设备数据
void Send_Data_LED0(void)
{		
	char  head1[3];
	char  temp[50];          	  //定义一个临时缓冲区1,不包括报头
	char  tempAll[100];       	//定义一个临时缓冲区2，包括所有数据

	int   dataLen;     	  	    //报文长度	
	
	memset(temp,       0, 50);  //清空缓冲区1
	memset(tempAll,    0, 100); //清空缓冲区2                  						 
	memset(head1,      0, 3);   //清空MQTT头
	
	sprintf(temp, "{\"led0Flag\":\"%s\"}", led0Flag);//构建报文
	
	head1[0] = 0x03;         		//固定报头
	head1[1] = 0x00;         		//固定报头
	head1[2] = strlen(temp); 		//数据长度	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;
	
	u1_printf("led0Flag:%s\r\n", led0Flag);
	
	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //添加数据，发布给服务器		
}

void Send_Data_LED1(void)
{		
	char  head1[3];
	char  temp[50];          	  //定义一个临时缓冲区1,不包括报头
	char  tempAll[100];       	//定义一个临时缓冲区2，包括所有数据

	int   dataLen;     	  	    //报文长度	
	
	memset(temp,       0, 50);  //清空缓冲区1
	memset(tempAll,    0, 100); //清空缓冲区2                  						 
	memset(head1,      0, 3);   //清空MQTT头
	
	sprintf(temp, "{\"led1Flag\":\"%s\"}", led1Flag);//构建报文
	
	head1[0] = 0x03;         		//固定报头
	head1[1] = 0x00;         		//固定报头
	head1[2] = strlen(temp); 		//数据长度	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;
	
	u1_printf("led1Flag:%s\r\n", led1Flag);
	
	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //添加数据，发布给服务器		
}

void Send_Data_Servo(void)
{		
	char  head1[3];
	char  temp[50];          	  //定义一个临时缓冲区1,不包括报头
	char  tempAll[100];       	//定义一个临时缓冲区2，包括所有数据

	int   dataLen;     	  	    //报文长度	
	
	memset(temp,       0, 50);  //清空缓冲区1
	memset(tempAll,    0, 100); //清空缓冲区2                  						 
	memset(head1,      0, 3);   //清空MQTT头
	
	sprintf(temp, "{\"servoFlag\":\"%s\"}", servoFlag);//构建报文
	
	head1[0] = 0x03;         		//固定报头
	head1[1] = 0x00;         		//固定报头
	head1[2] = strlen(temp); 		//数据长度	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;

	u1_printf("servoFlag:%s\r\n", servoFlag);
	
	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //添加数据，发布给服务器		
}

void Send_Data_Beep_ON(void)
{		
	char  head1[3];
	char  temp[50];          	  //定义一个临时缓冲区1,不包括报头
	char  tempAll[100];       	//定义一个临时缓冲区2，包括所有数据

	int   dataLen;     	  	    //报文长度	
	
	memset(temp,       0, 50);  //清空缓冲区1
	memset(tempAll,    0, 100); //清空缓冲区2                  						 
	memset(head1,      0, 3);   //清空MQTT头
	
	sprintf(temp, "{\"beepFlag\":\"BEEPON\"}");//构建报文
	
	head1[0] = 0x03;         		//固定报头
	head1[1] = 0x00;         		//固定报头
	head1[2] = strlen(temp); 		//数据长度	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;

	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //添加数据，发布给服务器		
}

void Send_Data_Beep_OFF(void)
{		
	char  head1[3];
	char  temp[50];          	  //定义一个临时缓冲区1,不包括报头
	char  tempAll[100];       	//定义一个临时缓冲区2，包括所有数据

	int   dataLen;     	  	    //报文长度	
	
	memset(temp,       0, 50);  //清空缓冲区1
	memset(tempAll,    0, 100); //清空缓冲区2                  						 
	memset(head1,      0, 3);   //清空MQTT头
	
	sprintf(temp, "{\"beepFlag\":\"BEEPOFF\"}");//构建报文
	
	head1[0] = 0x03;         		//固定报头
	head1[1] = 0x00;         		//固定报头
	head1[2] = strlen(temp); 		//数据长度	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;

	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //添加数据，发布给服务器		
}

void Send_Data_DHT(void)
{		
	char  head1[3];
	char  temp[50];          	  //定义一个临时缓冲区1,不包括报头
	char  tempAll[100];       	//定义一个临时缓冲区2，包括所有数据

	int   dataLen;     	  	    //报文长度	
	
	memset(temp,       0, 50);  //清空缓冲区1
	memset(tempAll,    0, 100); //清空缓冲区2                  						 
	memset(head1,      0, 3);   //清空MQTT头
	
	sprintf(temp, "{\"dhtFlag\":\"%s\"}", dhtFlag);//构建报文
	
	head1[0] = 0x03;         		//固定报头
	head1[1] = 0x00;         		//固定报头
	head1[2] = strlen(temp); 		//数据长度	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;

	u1_printf("dhtFlag:%s\r\n", dhtFlag);
	
	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //添加数据，发布给服务器		
}
