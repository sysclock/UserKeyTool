#include "Init.h"
#include "MyLib.h"
#include "Switch.h"
#include "key.h"
#include "stdlib.h"
#include "ctype.h"
#include "AT24XX.h"

void SendBufToUart(u8 Len);
/*******************************************************************************
【输入参数】无
【输出参数】无
【返 回 值】无
【描    述】发送系统配置到上位机
*******************************************************************************/
static void SendConfig(void)
{
    int i=0;
	u8 CheckSum;
	
	Uart.SendBuf[i++] = '$';
	hextostr1(PROT_VERSION,Uart.SendBuf+i);
	i+=2;
	hextostr1(SysInfo.Addr,Uart.SendBuf+i);
	i+=2;
	hextostr1(0x8e,Uart.SendBuf+i);
	i+=2;
	hextostr1(SysInfo.Type,Uart.SendBuf+i);
	i+=2;
	hextostr1(SysInfo.InputNum,Uart.SendBuf+i);
	i+=2;
	hextostr1(SysInfo.OutputNum,Uart.SendBuf+i);
	i+=2;
	memcpy(Uart.SendBuf+i,SysInfo.Logo,16);
	i+=16;
	CheckSum = CalculateCheckSum(Uart.SendBuf+1,i-1);
	hextostr1(CheckSum,Uart.SendBuf+i);
	i+=2;
	Uart.SendBuf[i++] = '#';
	SendBufToUart(i);
}

/*******************************************************************************
【输入参数】无
【输出参数】无
【返 回 值】无
【描    述】发送当前状态到上位机
*******************************************************************************/
static void SendState(void)
{
    int i=0,j;
	int AudioNum,VideoNum;
	u8 CheckSum;
	
	Uart.SendBuf[i++] = '$';
	hextostr1(PROT_VERSION,Uart.SendBuf+i);
	i+=2;
	hextostr1(SysInfo.Addr,Uart.SendBuf+i);
	i+=2;
	hextostr1(0x84,Uart.SendBuf+i);
	i+=2;
	//audio
	if(SysInfo.Type & TypeAudio)	//有音频
	  AudioNum = SysInfo.OutputNum;
	else
	  AudioNum = 0;
	hextostr1(AudioNum,Uart.SendBuf+i);
	i+=2;
	//video
	if(SysInfo.Type & 0x0F)	        //有视频
	  VideoNum = SysInfo.OutputNum;
	else
	  VideoNum = 0;
	hextostr1(VideoNum,Uart.SendBuf+i);
	i+=2;    
    
	for(j=0;j<AudioNum;j++)
	{
		hextostr1(InOut_A[j],Uart.SendBuf+i);
		i+=2;
	}
        
	for(j=0;j<VideoNum;j++)
	{
		hextostr1(InOut_V[j],Uart.SendBuf+i);
		i+=2;
	}
	CheckSum = CalculateCheckSum(Uart.SendBuf+1,i-1);
	hextostr1(CheckSum,Uart.SendBuf+i);
	i+=2;
	Uart.SendBuf[i++] = '#';
	SendBufToUart(i);
}
/*******************************************************************************
【输入参数】无
【输出参数】无
【返 回 值】无
【描    述】发送指定长度的数据到串口
*******************************************************************************/
void SendBufToUart(u8 Len)
{
	int i;
	for(i=0;i<Len;i++)
	{
	   	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
	   	{}
	  	USART_SendData(USART1,Uart.SendBuf[i]);
	}
}
/*******************************************************************************
【输入参数】无
【输出参数】无
【返 回 值】无
【描    述】串口接收中断
*******************************************************************************/
void USART1_RXD_IT()
{
//  static u8 HeadFlag = FALSE;
  u16 Data;
  
  //如果串口还没有处理完，则不再接受新数据，防止缓冲区溢出。
  if(Uart.UartFlag)
    return;
  
  UART_STOP;
  
  Data = USART_ReceiveData(USART1);
  USART2->DR = Data;      //转发到UART2
  
  Uart.RecvBuf[Uart.RecvLen++] = Data;
  
  UART_START;
}

void USART2_RXD_IT(void)
{
  u16 Data;
  
  Data = USART_ReceiveData(USART2);
  USART1->DR = Data;      //转发到UART2
}

static void DisposalPC()
{
	u8 Addr,CheckSum;
	u8 i;
	u8 Input,Output,Mode;
    u8 Logo[17];
	u8 Info[17];
    u32 Val;
    
    CheckSum = 0;
    for(i=1;i<Uart.RecvLen-3;i++)
      CheckSum += Uart.RecvBuf[i];
    if(CheckSum != strtohex8(Uart.RecvBuf+i))
      return;
    if(PROT_VERSION != strtohex8(Uart.RecvBuf+1))
      return;
    Addr = strtohex8(Uart.RecvBuf+3);
    if((Addr != SysInfo.Addr)&&(Addr != 0xFF))
      return;

    switch(strtohex8(Uart.RecvBuf+5))
    {
    case 0x01:		//切换命令
      Mode = strtohex8(Uart.RecvBuf+7);
      Input = strtohex8(Uart.RecvBuf+9);
	  if(Input > SysInfo.InputNum)
		  return;
      i = 11;
      while(i < Uart.RecvLen-3)
      {
        Output = strtohex8(Uart.RecvBuf+i);
		if(Output > SysInfo.OutputNum)
			return;
        if(Mode&SysInfo.Type&TypeAudio)
        {
          SetSwitchA(Input,Output);
		  if(Output != 0)
	          AT24xxWrite(SysInfo.AStartAddr+Output-1,&InOut_A[Output-1],1);
		  else
			  SaveEnv(0);
          DelayMs(5);
        }
        if(Mode&0x0F)
        {
          SetSwitchV(Input,Output);
		  if(Output != 0)
	          AT24xxWrite(Output-1,&InOut_V[Output-1],1);          
		  else
			  SaveEnv(0);
          DelayMs(5);
        }
        i += 2;
      }
      BEEP_START;
      break;
    case 0x02:		//保存命令
      i = strtohex8(Uart.RecvBuf+7);
      if((i > SysInfo.PresetCount)||(i<1))
        return;
      SaveEnv(i);
      BEEP_START;
      break;
    case 0x03:		//读取命令
      i = strtohex8(Uart.RecvBuf+7);
      if((i > SysInfo.PresetCount)||(i<1))
        return;
      LoadEnv(i);	
      SaveEnv(0);
      BEEP_START;	  
      break;
    case 0x04:		//发送状态
      if(SysInfo.InitOK)
        SendState();
      break;
    case 0x0e:		//读取配置
      SendConfig();
      BEEP_START;	    
      break;
    case 0x0f:		//设置配置
      break;
    case 0x10:      //修改类型使能
      SysInfo.TypeModifyEn = TRUE;
      BEEP_START;
      break;
    case 0x11:      //修改类型
      if(SysInfo.TypeModifyEn)
      {
		u8 TypeLen,LogoLen;
        SysInfo.Type = strtohex8(Uart.RecvBuf+7);
        SysInfo.InputNum = strtohex8(Uart.RecvBuf+9);
        SysInfo.OutputNum = strtohex8(Uart.RecvBuf+11);
		LogoLen = strtohex8(Uart.RecvBuf+13);
		TypeLen = strtohex8(Uart.RecvBuf+15);
        memset(Logo,0,17);
        memcpy(Logo,Uart.RecvBuf+17,LogoLen);
        memset(Info,0,17);
        memcpy(Info,Uart.RecvBuf+17+LogoLen,TypeLen);		
		
        SetDefSysInfo(SysInfo.Type,SysInfo.InputNum,SysInfo.OutputNum,Logo,Info);
        SysInfo.TypeModifyEn = FALSE;
        SysInfo.InitOK = TRUE;
      }
      break;
	case 0x14:		//授时
      memset(Logo,0,17);
      memcpy(Logo,Uart.RecvBuf+7,8);
      Val = strtoul((char const*)Logo,NULL,16);
	  SysInfo.TypeModifyEn = TRUE;
      AT24xxWrite(FIX_TIME,(u8*)&Val,4);	
	  SysInfo.TypeModifyEn = FALSE;
	  BEEP_START;	
		break;
    default:
      break;
    }
}

static void DisposalCenter()
{
  char *pBuf;// = (char*)Uart.RecvBuf;
  u8 Index,i8;
  u8 Num = 0,Mode = 0;
  u8 Input,Output;
  char tmpBuf[64];
  bool BeepState = FALSE;

  memset(tmpBuf,0,64);
  for(i8=0;i8<Uart.RecvLen;i8++)
    Uart.RecvBuf[i8] = toupper(Uart.RecvBuf[i8]);
  pBuf = (char*)Uart.RecvBuf;
  
  if(strchr(pBuf,'.'))
  {
    //查找','个数
    for(Index=0;Index<Uart.RecvLen;Index++)
      if(pBuf[Index] == ',')
        Num++;

    if((Index = strcspn(pBuf,"#")) != Uart.RecvLen)
      Mode = '#';
    else if((Index = strcspn(pBuf,"$")) != Uart.RecvLen)
      Mode = '$';
    if(Mode != 0)
    {
      for(i8=0;i8<Num;i8++)
      {
        memset(tmpBuf,0,32);
        Index = strcspn(pBuf,",");
        memcpy(tmpBuf,pBuf,Index);
        if(Mode == '#')
        {
          Input = atoi(tmpBuf);
          pBuf = strchr(pBuf,',')+1;
          if((Input != 0)&&(SysInfo.Type&0x0F))
          {
            SetSwitchV(Input,Input);
            AT24xxWrite(Input-1,&InOut_V[Input-1],1);
            DelayMs(5);
            BeepState = TRUE;
          }
          if((Input != 0)&&(SysInfo.Type&0xF0))
          {
            SetSwitchA(Input,Input);  
            AT24xxWrite(SysInfo.AStartAddr+Input-1,&InOut_A[Input-1],1); 
            DelayMs(5);
            BeepState = TRUE;
          }
        }
        else
        {
          Output = atoi(tmpBuf);
          pBuf = strchr(pBuf,',')+1;
          if((Output != 0)&&(SysInfo.Type&0x0F))
          {
            SetSwitchV(0,Output);
            AT24xxWrite(Output-1,&InOut_V[Output-1],1); 
            DelayMs(5);
            BeepState = TRUE;
          }
          if((Output != 0)&&(SysInfo.Type&0xf0))
          {
            SetSwitchA(0,Output);             
            AT24xxWrite(SysInfo.AStartAddr+Output-1,&InOut_A[Output-1],1); 
            DelayMs(5);
            BeepState = TRUE;
          }
        }
      }
      memset(tmpBuf,0,32);
      if(Mode == '#')
      {
        Index = strcspn(pBuf,"#");
        memcpy(tmpBuf,pBuf,Index);
        if(memcmp(tmpBuf,"ALL",3))
        {
          Input = atoi(tmpBuf);
          if((Input != 0)&&(SysInfo.Type&0x0F))
          {
            SetSwitchV(Input,Input);
            AT24xxWrite(Input-1,&InOut_V[Input-1],1); 
            DelayMs(5);
            BeepState = TRUE;
          }
          if((Input != 0)&&(SysInfo.Type&0xF0))
          {
            SetSwitchA(Input,Input);       
            AT24xxWrite(SysInfo.AStartAddr+Input-1,&InOut_A[Input-1],1); 
            DelayMs(5);
            BeepState = TRUE;
          }
        }
        else
        {
          for(i8=0;i8<SysInfo.OutputNum;i8++)
          {
            if(SysInfo.Type&0x0F)
            {
                SetSwitchV(i8+1,i8+1);
                AT24xxWrite(i8,&InOut_V[i8],1); 
                DelayMs(5);
            }
            if(SysInfo.Type&0xF0)
            {
                SetSwitchA(i8+1,i8+1);          
                AT24xxWrite(SysInfo.AStartAddr+i8,&InOut_A[i8],1); 
                DelayMs(5);
            }
          }
          BeepState = TRUE;
        }
      }
      else
      {
        Index = strcspn(pBuf,"$");
        memcpy(tmpBuf,pBuf,Index);
        if(memcmp(tmpBuf,"ALL",3))
        {
          Output = atoi(tmpBuf);
          if((Output != 0)&&(SysInfo.Type&0x0F))
          {
            SetSwitchV(0,Output);
            AT24xxWrite(Output-1,&InOut_V[Output-1],1); 
            DelayMs(5);            
            BeepState = TRUE;
          }
          if((Output != 0)&&(SysInfo.Type&0xF0))
          {
            SetSwitchA(0,Output);   
            AT24xxWrite(SysInfo.AStartAddr+Output-1,&InOut_A[Output-1],1); 
            DelayMs(5);            
            BeepState = TRUE;
          }
        }
        else
        {
          for(i8=0;i8<SysInfo.OutputNum;i8++)
          {
            if(SysInfo.Type&0x0F)
            {
                SetSwitchV(0,i8+1);                        
                AT24xxWrite(i8,&InOut_V[i8],1); 
                DelayMs(5);            
            }
            if(SysInfo.Type&0xF0)
            {
                SetSwitchA(0,i8+1);
                AT24xxWrite(SysInfo.AStartAddr+i8,&InOut_A[i8],1); 
                DelayMs(5);            
            }
          }
          BeepState = TRUE;
        }
      }
    }
    else if(strstr(pBuf,"ALL"))
    {
      if(strcspn(pBuf,"A") != Uart.RecvLen)
        Index = strcspn(pBuf,"A");
      
      memcpy(tmpBuf,pBuf,Index);
      Input = atoi(tmpBuf);
      if(Input != 0)
      {
        if(SysInfo.Type&0x0F)
            SetSwitchV(Input,0);        
        if(SysInfo.Type&0xF0)
            SetSwitchA(Input,0);
        
        SaveEnv(0);
        BeepState = TRUE;
      }
    }
    else
    {
      if((Index = strcspn(pBuf,"V")) != Uart.RecvLen)
        Mode = 'V';
      else if((Index = strcspn(pBuf,"A")) != Uart.RecvLen)
        Mode = 'A';
      else if((Index = strcspn(pBuf,"B")) != Uart.RecvLen)
        Mode = 'B';
  
      if(Mode != 0)
      {
        memcpy(tmpBuf,pBuf,Index);
        Input = atoi(tmpBuf);
        pBuf = strchr(pBuf,Mode)+1;    
  
        for(i8=0;i8<Num;i8++)
        {
          memset(tmpBuf,0,32);
          Index = strcspn(pBuf,",");
          memcpy(tmpBuf,pBuf,Index);
          Output = atoi(tmpBuf);
          pBuf = strchr(pBuf,',')+1;
          if(((Output != 0)&&((Mode == 'V')||(Mode == 'B')))&&(SysInfo.Type&0x0F))
          {
            SetSwitchV(Input,Output);
            AT24xxWrite(Output-1,&InOut_V[Output-1],1); 
            DelayMs(5);  
          }
          if(((Output != 0)&&((Mode == 'A')||(Mode == 'B')))&&(SysInfo.Type&0xF0))
          {
            SetSwitchA(Input,Output);        
            AT24xxWrite(SysInfo.AStartAddr+Output-1,&InOut_A[Output-1],1);
            DelayMs(5);
          }
        }
        memset(tmpBuf,0,32);
        Index = strcspn(pBuf,".");
        memcpy(tmpBuf,pBuf,Index);
        Output = atoi(tmpBuf);
        if(((Output != 0)&&((Mode == 'V')||(Mode == 'B')))&&(SysInfo.Type&0x0F))
        {
            SetSwitchV(Input,Output);
            AT24xxWrite(Output-1,&InOut_V[Output-1],1); 
            DelayMs(5);  
            BeepState = TRUE;         
        }
        if(((Output != 0)&&((Mode == 'A')||(Mode == 'B')))&&(SysInfo.Type&0xF0))
        {
            SetSwitchA(Input,Output); 
            AT24xxWrite(SysInfo.AStartAddr+Output-1,&InOut_A[Output-1],1);
            DelayMs(5);          
            BeepState = TRUE;
        }
      }
    }
  }
  else if(strchr(pBuf,'*'))
  {
    if(strstr(pBuf,"&"))
      Mode = '&';
    else if(strstr(pBuf,"%"))
      Mode = '%';
    else if(strstr(pBuf,"$"))
      Mode = '$';
    else if(strstr(pBuf,"!"))
      Mode = '!';
    
    if(Mode != 0)
    {
      memset(tmpBuf,0,32);
      Index = strcspn(pBuf,"*");
      memcpy(tmpBuf,pBuf,Index);
      pBuf = strchr(pBuf,'*')+1;
      Input = atoi(tmpBuf);
      
      memset(tmpBuf,0,32);
      memcpy(tmpBuf,pBuf,Uart.RecvLen-Index-2);
      Output = atoi(tmpBuf);
      if(Output == 0)
        return;
      if(((Mode == '&')||(Mode == '%')||(Mode == '!'))&&(SysInfo.Type&0x0F))
      {
        SetSwitchV(Input,Output);
        AT24xxWrite(Output-1,&InOut_V[Output-1],1); 
        DelayMs(5);          
        BeepState = TRUE;      
      }
      if(((Mode == '$')||(Mode == '!'))&&(SysInfo.Type&0xF0))
      {
        SetSwitchA(Input,Output);
        AT24xxWrite(SysInfo.AStartAddr+Output-1,&InOut_A[Output-1],1); 
        DelayMs(5);          
        BeepState = TRUE;      
      }
    }
  }
  if(BeepState)
    BEEP_START;
}

/*******************************************************************************
【输入参数】无
【输出参数】无
【返 回 值】无
【描    述】处理串口
*******************************************************************************/
void DisposalUart()
{
    if((Uart.RecvBuf[0]=='$')&&(Uart.RecvBuf[Uart.RecvLen-1]=='#'))
      DisposalPC();     //PC协议
    else
      DisposalCenter(); //中控协议
}
