#include "stm32f10x.h"
#include "control.h"
#include "usart1.h"
#include "mqtt.h"

extern char *led0Flag;		//LED0״̬
extern char *led1Flag;		//LED1״̬
extern char *dhtFlag;     //DHT״̬
extern char *servoFlag;   //SERVO״̬

//���ݳ��� length ����
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

//���Ϳ����豸����
void Send_Data_LED0(void)
{		
	char  head1[3];
	char  temp[50];          	  //����һ����ʱ������1,��������ͷ
	char  tempAll[100];       	//����һ����ʱ������2��������������

	int   dataLen;     	  	    //���ĳ���	
	
	memset(temp,       0, 50);  //��ջ�����1
	memset(tempAll,    0, 100); //��ջ�����2                  						 
	memset(head1,      0, 3);   //���MQTTͷ
	
	sprintf(temp, "{\"led0Flag\":\"%s\"}", led0Flag);//��������
	
	head1[0] = 0x03;         		//�̶���ͷ
	head1[1] = 0x00;         		//�̶���ͷ
	head1[2] = strlen(temp); 		//���ݳ���	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;
	
	u1_printf("led0Flag:%s\r\n", led0Flag);
	
	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //������ݣ�������������		
}

void Send_Data_LED1(void)
{		
	char  head1[3];
	char  temp[50];          	  //����һ����ʱ������1,��������ͷ
	char  tempAll[100];       	//����һ����ʱ������2��������������

	int   dataLen;     	  	    //���ĳ���	
	
	memset(temp,       0, 50);  //��ջ�����1
	memset(tempAll,    0, 100); //��ջ�����2                  						 
	memset(head1,      0, 3);   //���MQTTͷ
	
	sprintf(temp, "{\"led1Flag\":\"%s\"}", led1Flag);//��������
	
	head1[0] = 0x03;         		//�̶���ͷ
	head1[1] = 0x00;         		//�̶���ͷ
	head1[2] = strlen(temp); 		//���ݳ���	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;
	
	u1_printf("led1Flag:%s\r\n", led1Flag);
	
	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //������ݣ�������������		
}

void Send_Data_Servo(void)
{		
	char  head1[3];
	char  temp[50];          	  //����һ����ʱ������1,��������ͷ
	char  tempAll[100];       	//����һ����ʱ������2��������������

	int   dataLen;     	  	    //���ĳ���	
	
	memset(temp,       0, 50);  //��ջ�����1
	memset(tempAll,    0, 100); //��ջ�����2                  						 
	memset(head1,      0, 3);   //���MQTTͷ
	
	sprintf(temp, "{\"servoFlag\":\"%s\"}", servoFlag);//��������
	
	head1[0] = 0x03;         		//�̶���ͷ
	head1[1] = 0x00;         		//�̶���ͷ
	head1[2] = strlen(temp); 		//���ݳ���	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;

	u1_printf("servoFlag:%s\r\n", servoFlag);
	
	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //������ݣ�������������		
}

void Send_Data_Beep_ON(void)
{		
	char  head1[3];
	char  temp[50];          	  //����һ����ʱ������1,��������ͷ
	char  tempAll[100];       	//����һ����ʱ������2��������������

	int   dataLen;     	  	    //���ĳ���	
	
	memset(temp,       0, 50);  //��ջ�����1
	memset(tempAll,    0, 100); //��ջ�����2                  						 
	memset(head1,      0, 3);   //���MQTTͷ
	
	sprintf(temp, "{\"beepFlag\":\"BEEPON\"}");//��������
	
	head1[0] = 0x03;         		//�̶���ͷ
	head1[1] = 0x00;         		//�̶���ͷ
	head1[2] = strlen(temp); 		//���ݳ���	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;

	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //������ݣ�������������		
}

void Send_Data_Beep_OFF(void)
{		
	char  head1[3];
	char  temp[50];          	  //����һ����ʱ������1,��������ͷ
	char  tempAll[100];       	//����һ����ʱ������2��������������

	int   dataLen;     	  	    //���ĳ���	
	
	memset(temp,       0, 50);  //��ջ�����1
	memset(tempAll,    0, 100); //��ջ�����2                  						 
	memset(head1,      0, 3);   //���MQTTͷ
	
	sprintf(temp, "{\"beepFlag\":\"BEEPOFF\"}");//��������
	
	head1[0] = 0x03;         		//�̶���ͷ
	head1[1] = 0x00;         		//�̶���ͷ
	head1[2] = strlen(temp); 		//���ݳ���	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;

	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //������ݣ�������������		
}

void Send_Data_DHT(void)
{		
	char  head1[3];
	char  temp[50];          	  //����һ����ʱ������1,��������ͷ
	char  tempAll[100];       	//����һ����ʱ������2��������������

	int   dataLen;     	  	    //���ĳ���	
	
	memset(temp,       0, 50);  //��ջ�����1
	memset(tempAll,    0, 100); //��ջ�����2                  						 
	memset(head1,      0, 3);   //���MQTTͷ
	
	sprintf(temp, "{\"dhtFlag\":\"%s\"}", dhtFlag);//��������
	
	head1[0] = 0x03;         		//�̶���ͷ
	head1[1] = 0x00;         		//�̶���ͷ
	head1[2] = strlen(temp); 		//���ݳ���	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;

	u1_printf("dhtFlag:%s\r\n", dhtFlag);
	
	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //������ݣ�������������		
}
