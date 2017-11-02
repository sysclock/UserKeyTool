
#include "header.h"
#include "stdio.h"

unsigned char g_wifiRevFlag=0;

unsigned int a;

void InitEsp8266()
{

	delay(1);
	printf ("AT+RST\n");//重启模块
	delay(5);
	printf ("AT+CWMODE=3\n");//3-AP兼Station模式  需重启后生效(AT+RST)
	delay(5);
	printf ("AT+CWSAP=\"bycox-");
	UART3_SendByte(ChipUniqueID%10000/1000+48);
	UART3_SendByte(ChipUniqueID%1000/100+48);
	UART3_SendByte(ChipUniqueID%100/10+48);
	UART3_SendByte(ChipUniqueID%10+48);
	printf("\",\"123456123456\",1,3\n");//配置 AP 参数
	delay(5);
	printf ("AT+RST\n");
	delay(5);
	printf ("AT+CIPMUX=1\n");//开启多连接模式
	delay(5);
	printf ("AT+CIPSERVER=1,8080\n");//创建服务器
	delay(5);
	printf ("AT+CIPSTO=2880\n");//服务器超时时间，0~2880，单位为 s
	delay(5);
	g_wifiRevFlag=1;

}















