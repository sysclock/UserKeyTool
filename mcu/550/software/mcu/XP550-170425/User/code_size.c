//===2017,Jun 01 ; 9:39:45  ;BY YANG@BYCOX.COM===
Program Size: Code=75864 RO-data=5464 RW-data=8460 ZI-data=1412  
Program Size: Code=75880 RO-data=5464 RW-data=8460 ZI-data=1412  
Program Size: Code=75904 RO-data=5464 RW-data=8460 ZI-data=,m n

//===2017,Jun 01 ; 9:57:11  ;BY YANG@BYCOX.COM===
1,外部串口速率修改为115200,增加打印内容,在380代码上也不会影响遥控器的响应;
2,在380的中断函数里面增加打印函数，操作遥控器的时候无响应；
3,system-stm32f10x文件在两个工程中是相同的
4,


//===2017,Jun 01 ; 18:11:37  ;BY YANG@BYCOX.COM===
Remote_Scan is got: 0;  ircord[2]:0  
Remote_Scan is got: 0;  ircord[2]:0  
Remote_Scan is got: 0EXTI15_10_IRQHandler  i=0;  irtime=20439 ,RmtCnt= 1 
;  ircord[2]:0  
Remote_Scan is got: 0;  ircord[2]:0  
Remote_Scan is got: 0;  ircord[2]:0  
EXTI15_10_IRQHandler  i=1;  irtime=10 ,RmtCnt= 1 
Remote_Scan is goEXTI15_10_IRQHandler  i=2;  irtime=10 ,RmtCnt= 1 
t: 0;  ircord[2]:0  
Remote_ScEXTI15_10_IRQHandler  i=3;  irtime=21 ,RmtCnt= 1 
an is got: 0;  ircord[2]:0  
Remote_Scan EXTI15_10_IRQHandler  i=4;  irtime=21 ,RmtCnt= 1 
is got: 0;  ircord[2EXTI15_10_IRQHandler  i=5;  irtime=21 ,RmtCnt= 1 
EXTI15_10_IRQHandler  i=6;  irtime=11 ,RmtCnt= 1 
EXTI15_10_IRQHandler  i=7;  irtime=10 ,RmtCnt= 1 
EXTI15_10_IRQHandler  i=8;  irtime=21 ,RmtCnt= 1 
]:0  
Remote_Scan is got: 0;  ircord[2]:0  
Remote_Scan is got: 0;  ircord[2]:0  
Remote_Scan is got: 0;  ircord[2]:0  
Remote_Scan is got: 0;  ircord[2]:0  
Remote_Scan is got: 0;  ircord[2]:0  
Remote_Scan is got: 0;  ircord[2]:0  
Remote_Scan is got: 0;  ircord[2]:0  
Remote_Scan is got: 0;  ircord[2]:0  
Remote_Scan iEXTI15_10_IRQHandler  i=0;  irtime=103 ,RmtCnt= 1 


//===2017,Jun 01 ; 18:26:8  ;BY YANG@BYCOX.COM===
//push down key of IR 550 code
IRcord[0]=:0x9b 
IRcord[1]=:0xda 
IRcord[2]=:0x60 
IRcord[3]=:0x7b 
IRcord[0]=:0xb4 
IRcord[1]=:0xde 
IRcord[2]=:0xd3 
IRcord[3]=:0x43 
IRcord[0]=:0x3d 
IRcord[1]=:0xa6 
IRcord[2]=:0xf6 
IRcord[3]=:0x9a 	//每次接收到的码元都不一样

//===2017,Jun 01 ; 18:39:25  ;BY YANG@BYCOX.COM===
//push down key of IR 380 code
IRcord[0]=:0x0 
IRcord[1]=:0xff 
IRcord[2]=:0x78 
IRcord[3]=:0x87 
IRcord[0]=:0x0 
IRcord[1]=:0xff 
IRcord[2]=:0x78 
IRcord[3]=:0x87 
IRcord[0]=:0x0 
IRcord[1]=:0xff 
IRcord[2]=:0x78 
IRcord[3]=:0x87//每次接收到的相同，而且前后两个互逆 

//right key
IRcord[0]=:0x0 
IRcord[1]=:0xff 
IRcord[2]=:0xd8 
IRcord[3]=:0x27 

//left key
IRcord[0]=:0x0 
IRcord[1]=:0xff 
IRcord[2]=:0xf8 
IRcord[3]=:0x7 

//0 key
IRcord[0]=:0x0 
IRcord[1]=:0xff 
IRcord[2]=:0x50 
IRcord[3]=:0xaf 

//===2017,Jun 02 ; 15:52:15  ;BY YANG@BYCOX.COM===
550 Remote_Init prior is =0 cannot print ircord when push ir-panel
