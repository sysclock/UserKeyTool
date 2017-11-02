#include "Init.h"
#include "Lcd.h"
#include "Key.h"
#include "Switch.h"
#include "AT24XX.h"
#include "Time.h"

static uc8 KeyArray[24] = {VGA,V,A,0,2,1,0,0,CANCER,ENTER,AV,0,0,0,0,0,RECALL,SAVE,MENU,0,4,3,0,0};
//static uc8 KeyArray[24] = {VGA,V,A,5,4,3,2,1,CANCER,ENTER,AV,0,0,0,0,0,RECALL,SAVE,MENU,0,9,8,7,6};
u8 KeyVal;
OperateInfo_t OperateInfo;
CycleInfo_t CycleInfo;
u32 CycleTimingTmp;
u8 CycleNumTmp;
TipInfo_t TipInfo;
GoInfo_T GoInfo;
u32 LcdBkSec;		
u32 CycleDelay;
u8 CycleIndex;

static u8 GetScanVal(u8 Val)
{
  u8 KeyVal = 0x00;
  u8 tempval;
  u8 i,j;

  SetKeyRVal(Val);
  for(j=0,i=0;i<5;i++)      //读5次，3次连续相等，则认为稳定
  {
		ReadPortData(&tempval);
    if(KeyVal == tempval)
    {
      if(++j>2)
        break;
    }
    else
      KeyVal = tempval;
   
    DelayMs(2);
  }
  
  if(KeyVal&0x01)return 1;
  if(KeyVal&0x02)return 2;
  if(KeyVal&0x04)return 3;
  if(KeyVal&0x08)return 4;
  if(KeyVal&0x10)return 5;
  if(KeyVal&0x20)return 6;
  if(KeyVal&0x40)return 7;
  if(KeyVal&0x80)return 8;
	
  return 0;
}

u8 ReadKey(void)
{
  static u8 KeyFlag = FALSE;   
  u8 KeyVal1,KeyVal2;
  u8 i;
	
  KEY_EN;
  BUS_IN;				
  
  KeyVal1 = GetScanVal(0x07);
  if( KeyVal1 == 0x00)    //没有按键按下
  {
      BUS_OUT;    
	  KEY_DIS;
      KeyFlag = FALSE;
      return FALSE;
  }
  else if(KeyFlag == FALSE)
  {
    for(i=0;i<3;i++)
    {
      KeyVal2 = GetScanVal(1<<i);  
      if(KeyVal2 == KeyVal1)  //两次读取的行值相同
      {
        KeyVal2 += (i<<3);
        if((KeyVal2>0)&&(KeyVal2<25))   
        {
          KeyVal = KeyArray[KeyVal2-1];
          KeyFlag = TRUE;
        }
        BUS_OUT;     //把端口设置为输出
				KEY_DIS;
        return TRUE;
      }
    }
  }
  BUS_OUT;     //把端口设置为输出  
	KEY_DIS;
  return FALSE;
}

static void ShowTypeBMP(u8 Mode)
{
	DrawLine(28);
	if(Mode == 1)			//v,vga,rgb
	{
		if(SysInfo.Type&TypeVideo)								//v		
			LoadBMP(2,24,VID_BMP,24,16);
		else if(SysInfo.Type&TypeVGA)      //VGA RGB 矩阵
			LoadBMP(2,24,VGA_BMP,24,16);	  
        else if(SysInfo.Type&TypeRGB)
            LoadBMP(2,24,RGB_BMP,24,16);	  
	}
	else if(Mode == 2)		//a
		LoadBMP(2,24,AUD_BMP,24,16);	  
	else
	{
		LoadBMP(2,32,AUD_BMP,24,16);
		if(SysInfo.Type&TypeVideo)								//v		
			LoadBMP(2,16,VID_BMP,24,16);
		else if(SysInfo.Type&TypeVGA)//VGA RGB 矩阵
			LoadBMP(2,16,VGA_BMP,24,16);	  	
        else if(SysInfo.Type&TypeRGB)
            LoadBMP(2,16,RGB_BMP,24,16);	  	
	}
}

static void UpdataMode()
{
	LoadBMP(2,16,NULL_BMP,24,16);
	LoadBMP(2,32,NULL_BMP,24,16);

	ShowTypeBMP(OperateInfo.Mode);
}

static void UpdataVal(Mode_t ID)
{
    char Val[8];

	switch(ID)
	{
	case Input:
	  sprintf(Val,"%02d",OperateInfo.Input);		 
	  LcdDisStr(0,4,Val,2);
	  break;
	case Output:	  
	  sprintf(Val,"%02d",OperateInfo.Output);		 	  
	   LcdDisStr(0,13,Val,2);//DrawStr(109,24,Val);	  
	  break;
	case Save:
	  sprintf(Val,"%02d",OperateInfo.Input);		 	  
	  LcdDisStr(0,11,Val,2);//DrawStr(88,25,Val);	  
	  break;
	case Load:
	  sprintf(Val,"%02d",OperateInfo.Output);		 	  
	  LcdDisStr(0,12,Val,2);//DrawStr(100,25,Val);	  
	  break;
	case Addr:
	  sprintf(Val,"%03d",OperateInfo.Input);
	  LcdDisStr(1,11,Val,3);//DrawStr(94,32,Val);
	  break;
        case CycleTim:////discard in 1U
          sprintf(Val,"%04d",CycleTimingTmp);
	  DrawStr(82,48,Val);
	  break;/*
          sprintf(Val,"%04d",CycleInfo.CycleTiming);
	  DrawStr(78,48,Val);
          break;*/
        case CycleNumber:////discard in 1U
          sprintf(Val,"%02d",CycleNumTmp);
	  DrawStr(82,48,Val);
	  break;
	default:
	  break;
	}
}

static void InitPage(Page_t Page)
{
  	char temp[32];
	u8 Len;
	
  	lcd_clr();//LcdClear();
	switch(Page)
	{
	case DispPage:
	  break;
	case SwitchPage:
	  	LcdDisStr(0,1,"IN:",3);
	  	LcdDisStr(0,9,"OUT:",4);//DrawStr(85,24,"OUT:");
	  	LcdDisStr(0,7,"*",1);//LoadBMP(64,24,ARROW0_BMP,18,16);
	  break;
	case RecallPage:
	  	LcdDisStr(0,2,"LOAD FROM:__",12);
        //DrawLine(35);
		//LoadBMP(0,17,LOAD_BMP,32,32);
	  break;
	case SavePage:
	  	LcdDisStr(0,3,"SAVE TO:__",10);//DrawStr(40,25,"SAVE TO:__");
        //DrawLine(35);
		//LoadBMP(0,17,SAVE_BMP,32,32);	  
	  break;
	case MenuPage:
	  //LcdDisStr(0,3,"1.SET BAUD",10);//(40,6,"1.SET BAUD");
	  LcdDisStr(0,3,"1.SET ADDR",10);//(40,22,"2.SET ADDR");	 
	  LcdDisStr(1,3,"2.SET BEEP",10);// DrawStr(40,38,"3.SET BEEP");	 
          //DrawStr(40,54,"4.SET CYCLE");	   
      //DrawLine(35);
	//  LoadBMP(0,16,SET_BMP,32,32);
	  break;
	case AddrPage:
	  LcdDisStr(0,4,"(1 - 254)",9);// DrawStr(40,16,"(1 - 254)");
	  sprintf(temp,"SET ADDR:%03d",SysInfo.Addr);
	  LcdDisStr(1,2,temp,12);//DrawStr(40,32,temp);	  
      //DrawLine(35);
	  //LoadBMP(0,16,SET_BMP,32,32);	  
	  break;
	case BaudPage://discard in 1U
	  DrawStr(8,16,"1.4800");
	  DrawStr(69,16,"2.9600");
	  DrawStr(8,32,"3.38400");
	  DrawStr(69,32,"4.115200");	
	  if(SysInfo.Baud == Baud_4800)
		DrawStr(0,16,">");
	  else if(SysInfo.Baud == Baud_9600)
		DrawStr(61,16,">");
	  else if(SysInfo.Baud == Baud_38400)
		DrawStr(0,32,">");
	  else if(SysInfo.Baud == Baud_115200)
		DrawStr(61,32,">");
	  break;
        case CyclePage1:     //set timing  discard in 1U
          DrawStr(40,16,"(1 - 9999)");
	  //sprintf(temp,"SET TIMING(Second):%04d",5);
	  DrawStr(40,32,"SET TIMING");	
          sprintf(temp,"SECOND:%04d",CycleInfo.CycleTiming);
          DrawStr(40,48,temp);	
          DrawLine(35);
	  LoadBMP(0,16,SET_BMP,32,32);	  
          break;
        case CyclePage2:     //set num   discard in 1U
          DrawStr(40,16,"(1 - 16)");
	  //sprintf(temp,"SET TIMING(Second):%04d",5);
	  DrawStr(40,32,"SET SCENE");	
          sprintf(temp,"NUMBER:%02d",CycleInfo.CycleNum);
          DrawStr(40,48,temp);	
          DrawLine(35);
	  LoadBMP(0,16,SET_BMP,32,32);	  
          break;
        case CyclePage3:      //discard in 1U
          DrawStr(32,16,"1.CYCLE ON");
	  DrawStr(32,32,"2.CYCLE OFF");	
	  if(SysInfo.CycleEn)
		  DrawStr(24,16,">");
	  else
 		  DrawStr(24,32,">");		
	  break;
    case SystemPage://discard in 1U
	  {
		  time_t Time = 0; 
		  AT24xxRead(FIX_TIME,(u8*)&Time,4);
		  struct tm * CurTime = localtime(&Time);
		  sprintf(temp,"DATE:%02d-%02d-%02d",(CurTime->tm_year+1900)%100,CurTime->tm_mon+1,CurTime->tm_mday);
		  Len = 64-strlen(temp)*3;
		  DrawStr(Len,8,temp);
	  }
	  sprintf(temp,"V:%d.%d.%d",HARD_VERSION,PROT_VERSION,SOFT_VERSION);
	  Len = 64-strlen(temp)*3;
      DrawStr(Len,24,temp);
	  
	  DrawStr(31,40,"SWITCH TEST");
  	  OperateInfo.Input = 1;
	  OperateInfo.Output = 1;
	  if(!(SysInfo.Type & 0xF0))		//没有音频，则直接进行视频测试
		  OperateInfo.Mode = 1;
      break;
	case LangPage:
	  break;
	case BeepPage:
	  LcdDisStr(0,3,"1.BEEP ON",9);//DrawStr(32,16,"1.BEEP ON");
	  LcdDisStr(1,3,"2.BEEP OFF",10);//DrawStr(32,32,"2.BEEP OFF");	
	  if(SysInfo.BeepEn)
		  LcdDisStr(0,2,">",1);//DrawStr(24,16,">");
	  else
 		  LcdDisStr(1,2,">",1);//DrawStr(24,32,">");		
	  break;
	default:
	  break;
	}
}

void DrawLogo(void)
{
	u8 Len;

    Len = strlen((char const *)SysInfo.Logo);
    //Len = (128-Len*12)>>1;
	//DrawStr32(Len,2,(char*)SysInfo.Logo);		//显示LOGO
    LcdDisStr(0,(16-Len)/2,(char*)SysInfo.Logo,Len);
	Len = strlen((char const *)SysInfo.Info);
    //Len = (128-Len*6)>>1; 
        LcdDisStr(1,(16-Len)/2,(char*)SysInfo.Info,Len);

	//DrawStr(Len,34,(char*)SysInfo.Info);
}

void ChangeDisp(void)
{
	u8 TotalPage;
	u8 Mix;
	u8 i,Output;
	u8 VorA;
	u8 yStartAddr = 0;
	char temp[8];
	
	if((SysInfo.Type&0xF0)&&(SysInfo.Type&0x0F))	
	{
		Mix = 1;
		TotalPage = SysInfo.OutputNum>>1;
        if(TotalPage < 2)   
          TotalPage = 2;
	}
	else
    {
	  TotalPage = SysInfo.OutputNum>>2;  
      if(TotalPage == 0)
        TotalPage = 1;
    }
	GoInfo.CurPage++;
	if(GoInfo.CurPage == TotalPage+1)
	{
	  //GoInfo.CurPage = 1;
		GoInfo.Enable = FALSE;
		lcd_clr();//LcdClear();		
		DrawLogo();
		return;
	}
	
	/*if(SysInfo.OutputNum < 3)		
		yStartAddr = 24;
	else if(SysInfo.OutputNum < 5)	
		yStartAddr = 16;*/
	
	lcd_clr();//LcdClear();
				
	if(Mix == 1)
	{
		if(GoInfo.CurPage > (TotalPage>>1))		
		{  
		  	Output = (GoInfo.CurPage-(TotalPage>>1)-1)*4;
			VorA = 2;
		}
		else					
		{
		  	Output = (GoInfo.CurPage-1)*4;
			VorA = 1;
		}
	}
	else
	{
	  	Output = (GoInfo.CurPage-1)*4;  
		if(SysInfo.Type & TypeAudio)	
			VorA = 2;
		else							
			VorA = 1;		  
	}
	
	if(VorA == 1)		
	{
			//ShowTypeBMP(1);			  
			for(i=0;i<4;i++)
			{
                if(i+Output < SysInfo.OutputNum)
                {         
                
                  sprintf(temp,"%02d",(InOut_V[i+Output]+1)&0x7F);
                  LcdDisStr(i/2,(i%2)*8,temp,2);
                  //DrawStr((i%2)*45+32,(i>>1)*16+yStartAddr,temp);
                  if((InOut_V[i+Output]+1)&0x80)
                      LcdDisStr(i/2,(i%2)*8+3,"V",1);//LoadBMP((i%2)*45+44,(i>>1)*16+yStartAddr,ARROW1_BMP,18,16); 
                  else
                    LcdDisStr(i/2,(i%2)*8+3,"v",1);//LoadBMP((i%2)*45+44,(i>>1)*16+yStartAddr,ARROW0_BMP,18,16);
                  sprintf(temp,"%02d",i+Output+1);
                  LcdDisStr(i/2,(i%2)*8+5,temp,2);//DrawStr((i%2)*45+62,(i>>1)*16+yStartAddr,temp);
                }
			}				
	}
	else if(VorA == 2)
	{
			//ShowTypeBMP(2);			  
			for(i=0;i<4;i++)
			{
                if(i+Output < SysInfo.OutputNum)
                {              
                  sprintf(temp,"%02d",(InOut_A[i+Output]+1)&0x7F);
                  LcdDisStr(i/2,(i%2)*8,temp,2);//DrawStr((i%2)*45+32,(i>>1)*16+yStartAddr,temp);
                  if((InOut_A[i+Output]+1)&0x80)
                      LcdDisStr(i/2,(i%2)*8+3,"A",1);//LoadBMP((i%2)*45+44,(i>>1)*16+yStartAddr,ARROW1_BMP,18,16); 
                  else
                    LcdDisStr(i/2,(i%2)*8+3,"a",1);//LoadBMP((i%2)*45+44,(i>>1)*16+yStartAddr,ARROW0_BMP,18,16);
                  sprintf(temp,"%02d",i+Output+1);
                  LcdDisStr(i/2,(i%2)*8+5,temp,2);//DrawStr((i%2)*45+62,(i>>1)*16+yStartAddr,temp);
                }
			}	
	}
}

static void ReturnDispPage(void)
{
    lcd_clr();//LcdClear();
  	if(TipInfo.Enable)
	{
		if(TipInfo.TipType == Switch)
		{
			//ShowTypeBMP(OperateInfo.Mode);
			LcdDisStr(0,3,"SWITCH OK!",10);//DrawStr(42,24,"SWITCH OK!");
		}
		else if(TipInfo.TipType == Addr)
		{
            //DrawLine(35);
			//LoadBMP(0,17,SET_BMP,32,32);
			LcdDisStr(0,0,"ADDRESS SET OK!",15);//DrawStr(46,17,"ADDRESS SET");
			//LcdDisStr(0,3,"SWITCH OK!",10);//DrawStr(70,33,"OK!");
		}
		else if(TipInfo.TipType == Baud)
		{
            //DrawLine(35);          
		//	LoadBMP(0,17,SET_BMP,32,32);
			LcdDisStr(0,0,"BAUDRATE SET OK!",16);//DrawStr(46,17,"BAUDRATE SET");
			//DrawStr(70,33,"OK!");
		}
		else if(TipInfo.TipType == Beep)
		{
            //DrawLine(35);          
		//	LoadBMP(0,17,SET_BMP,32,32);
			LcdDisStr(0,2,"BEEP SET OK!",12);//DrawStr(58,17,"BEEP SET");
			//DrawStr(70,33,"OK!");
		}	
                else if(TipInfo.TipType == CycleOn)//discard in 1U
                {
                  CycleDelay = 0;
                  DrawLine(35);
		  LoadBMP(0,17,SET_BMP,32,32);
		  DrawStr(40,16,"CYCLE ON");
                  DrawStr(40,32,"PRESS ESC");
                  DrawStr(40,48," TO EXIT");  
                  LoadEnv(CycleIndex+1);
                  CycleIndex++;

                  while(1)
                  {
                    if(ReadKey())
                    {
                        BEEP_START;
                        if(KeyVal == CANCER)
                        {
                          TipInfo.TipType = CycleOff;
                          break;
                        }
                    }
                  }
                }//else if(TipInfo.TipType == CycleOn)
                else if(TipInfo.TipType == CycleOff)////discard in 1U
                {
                  DrawLine(35);          
		  LoadBMP(0,17,SET_BMP,32,32);
		  DrawStr(50,17,"CYCLE OFF");
		  //DrawStr(70,33,"OK!");
                }
		else if(TipInfo.TipType == Connect)
		{
			LcdDisStr(0,2,"CONNECT OK!",11);//DrawStr(30,26,"CONNECT OK!");
		}
		else if(TipInfo.TipType == Save)
		{
           // DrawLine(35);          
		//	LoadBMP(0,17,SAVE_BMP,32,32);
			LcdDisStr(0,4,"SAVE OK!",8);////LcdDisStr(0,0,"CONNECT SET OK!",15);//DrawStr(46,25,"SAVE OK!");
		}
		else if(TipInfo.TipType == Load)
		{
            //DrawLine(35);          
			//LoadBMP(0,17,LOAD_BMP,32,32);
			LcdDisStr(0,4,"LOAD OK!",8);/////DrawStr(46,25,"LOAD OK!");
		}
		if(TipInfo.TipType != Null)
		{
			TipInfo.TipType = Null;
		}
		GoInfo.DlyTime = 30;		
	}
	else
	{
		GoInfo.DlyTime = 40;	
	}

	OperateInfo.Mode = 0;
	OperateInfo.Input = 0;
	OperateInfo.InputFlag = 0;
	OperateInfo.Output = 0;
	OperateInfo.OutputFlag = 0; 
	OperateInfo.CurPage = DispPage;

	GoInfo.CurPage = 0;
	GoInfo.Enable = 1;	
}


/*******************************************************************************
【输入参数】无
【输出参数】无
【返 回 值】无
【描    述】保存系统信息到存储器
*******************************************************************************/
void SaveSysInfo(void)
{
	
}

/*******************************************************************************
【输入参数】无
【输出参数】无
【返 回 值】无
【描    述】保存设备信息到存储器
*******************************************************************************/
void SaveFixInfo(void)
{
	
}

void SaveEnv(u16 Index)
{
    int i,j;
    for(i=0;i<SysInfo.OutputNum;)
	{
	    if(SysInfo.OutputNum-i > 16)
		  j = 16;
		else
		  j = SysInfo.OutputNum-i;
        if(SysInfo.Type & 0x0F)
        {
          AT24xxWrite(Index*SysInfo.OutputNum+i,&InOut_V[i],j);	
          DelayMs(5);
        }
        if(SysInfo.Type & 0xF0)
        {
          AT24xxWrite(Index*SysInfo.OutputNum+SysInfo.AStartAddr+i,&InOut_A[i],j);
          DelayMs(5);
        }
		i+=j;
	}
}

void LoadEnv(u16 Index)
{
	u8 Temp[128];
	int i,j,k;

	if(SysInfo.Type&0x0F)
	{
	   for(i=0;i<SysInfo.OutputNum;)
	   {
		    if(SysInfo.OutputNum-i > 16)
			  j = 16;
			else
			  j = SysInfo.OutputNum-i;
			AT24xxRead(Index*SysInfo.OutputNum+i,Temp,j);
			for(k=i;k<i+j;k++)
			{
			    if(Temp[k-i]&0x80)
					SetSwitchV((Temp[k-i]&0x7f)+1,k+1);
				else		
				    SetSwitchV(0,k+1);
			}
			i += j;
	  }
	}
	if(SysInfo.Type & 0xF0)
	{
	    for(i=0;i<SysInfo.OutputNum;)
		{
		    if(SysInfo.OutputNum-i > 16)
			  j = 16;
			else
			  j = SysInfo.OutputNum-i;
			AT24xxRead(Index*SysInfo.OutputNum+SysInfo.AStartAddr+i,Temp,j);
			for(k=i;k<i+j;k++)
			{
			  	if(Temp[k-i]&0x80)
					SetSwitchA((Temp[k-i]&0x7f)+1,k+1);
				else
				  	SetSwitchA(0,k+1);
			}
			i += j;
		}
	}	
}

static void SetSysDef(void)
{
  	SysInfo.Addr = 1;
	SysInfo.TypeModifyEn = TRUE;
	AT24xxWrite(FIX_ADD,&SysInfo.Addr,1);		
    DelayMs(10);
	SysInfo.Baud = 1;
	AT24xxWrite(FIX_BAUD,&SysInfo.Baud,1);		
    DelayMs(10);  
    SysInfo.TypeModifyEn = FALSE;

    u8 i;
    for(i=0;i<SysInfo.OutputNum;i++)
      InOut_V[i] = 0x80|i;
    
    if(SysInfo.Type & TypeAudio)
    {
      for(i=0;i<SysInfo.OutputNum;i++)
        InOut_A[i] = 0x80|i;
    }
    
    for(i=0;i<=SysInfo.PresetCount;i++)
	{
      SaveEnv(i);
	  IWDG_ReloadCounter();
	}
}

void DisposeKey()
{
    u16 Val;
	char temp[8];
	
	switch(OperateInfo.CurPage)
	{
	case DispPage:
	  if((KeyVal < 10)&&(KeyVal < SysInfo.InputNum+1))		
	  {
			GoInfo.Enable = FALSE;		
	  		OperateInfo.CurPage = SwitchPage;
			InitPage(OperateInfo.CurPage);					
			OperateInfo.Input = KeyVal;
			OperateInfo.InputFlag = 1;
			UpdataVal(Input);
	  }
	  else if(KeyVal == MENU)
	  {
			GoInfo.Enable = FALSE;		
	  		OperateInfo.CurPage = MenuPage;
			InitPage(OperateInfo.CurPage);					
	  }
	  else if(KeyVal == SAVE)
	  {
		  	GoInfo.Enable = FALSE;		
			OperateInfo.CurPage = SavePage;
			InitPage(OperateInfo.CurPage);					
	  }
	  else if(KeyVal == RECALL)
	  {
		  	GoInfo.Enable = FALSE;		
	  		OperateInfo.CurPage = RecallPage;
			InitPage(OperateInfo.CurPage);						  
	  }
	  else if((KeyVal == ENTER)&&(GoInfo.Enable))
			GoInfo.DlyTime = 40;	
	  else
	  {		  	
		GoInfo.Enable = FALSE;				  
		TipInfo.Enable = FALSE;			
		ReturnDispPage();
	  }
	  break;
	case SwitchPage:
	  GoInfo.Enable = FALSE;		
	  if(KeyVal < 10)		
	  {
			if(OperateInfo.Mode == 0)		
			{
				Val = OperateInfo.Input*10+KeyVal;
				if(Val > SysInfo.InputNum)
					Val = SysInfo.InputNum;
				OperateInfo.Input = Val;
				UpdataVal(Input);
			}
			else 
			{
				Val = OperateInfo.Output*10+KeyVal;
				if(Val > SysInfo.OutputNum)
					Val = SysInfo.OutputNum;
				OperateInfo.Output = Val;
				OperateInfo.OutputFlag = 1;
				UpdataVal(Output);
			}
	  }
	  else if(KeyVal == A)		
	  {
			if(SysInfo.Type & 0xF0)
				OperateInfo.Mode = 2; 
			else
				OperateInfo.Mode = 1;		
			//UpdataMode();			
			LcdDisStr(0,7,"A",1);//LoadBMP(64,24,ARROW1_BMP,18,16);			
	  }
	  else if((KeyVal == V)||(KeyVal == VGA))		//mode	
	  {
			if((SysInfo.Type&0x0f) == TypeVideo)
				OperateInfo.Mode = 1;
			else
			{
				if(SysInfo.Type&0x0f)			
					OperateInfo.Mode = 1;
				else
					OperateInfo.Mode = 2;//不严谨 00态没有进入考虑		
			}
			//UpdataMode();
			LcdDisStr(0,7,"V",1);//LoadBMP(64,24,ARROW1_BMP,18,16);
	  }
	  else if(KeyVal == AV)		
	  {
			if((SysInfo.Type&0xf0)&&(SysInfo.Type&0x0f))
				OperateInfo.Mode = 3;			
			else if(SysInfo.Type&0xf0)
				OperateInfo.Mode = 2;
			else
				OperateInfo.Mode = 1;
			//UpdataMode();
			LcdDisStr(0,7,"B",1);//LoadBMP(64,24,ARROW1_BMP,18,16);
	  }
	  else if(KeyVal == ENTER)
	  {
		if(OperateInfo.OutputFlag == 1)		
		{
			if(OperateInfo.Mode == 1)		//V
			{
				SetSwitchV(OperateInfo.Input,OperateInfo.Output);
                if(OperateInfo.Output == 0)
                  SaveEnv(0);
                else
                  AT24xxWrite(OperateInfo.Output-1,&InOut_V[OperateInfo.Output-1],1);
			}
			else if(OperateInfo.Mode == 2)	//A
			{
				SetSwitchA(OperateInfo.Input,OperateInfo.Output);
                if(OperateInfo.Output == 0)
                  SaveEnv(0);
                else
                  AT24xxWrite(SysInfo.AStartAddr+OperateInfo.Output-1,&InOut_A[OperateInfo.Output-1],1);                
			}
			else if(OperateInfo.Mode == 3)	//AV
			{
				SetSwitchV(OperateInfo.Input,OperateInfo.Output);
				SetSwitchA(OperateInfo.Input,OperateInfo.Output);
                if(OperateInfo.Output == 0)
                  SaveEnv(0);
                else
                {
                   AT24xxWrite(OperateInfo.Output-1,&InOut_V[OperateInfo.Output-1],1);                
                   AT24xxWrite(SysInfo.AStartAddr+OperateInfo.Output-1,&InOut_A[OperateInfo.Output-1],1);                
                }
			}
			TipInfo.TipType = Switch;
            TipInfo.Enable = TRUE;	
      		ReturnDispPage();            
		}
	  }
	  else if(KeyVal == CANCER)
	  {
			TipInfo.Enable = FALSE;			
			ReturnDispPage();
	  }
	  break;
	case RecallPage:
		GoInfo.Enable = FALSE;		
	  if(KeyVal < 10)
	  {
		  Val = OperateInfo.Output*10+KeyVal;
		  if((Val > SysInfo.PresetCount)&&(Val != 0xFF))
			Val = SysInfo.PresetCount;
		  OperateInfo.Output = Val;
		  UpdataVal(Load);
	  }
	  else if(KeyVal == ENTER)
	  {
			if(OperateInfo.Output > 0)
			{
              if(OperateInfo.Output == 0xFF)
                SetSysDef();
              else
				LoadEnv(OperateInfo.Output);
                SaveEnv(0);  
				TipInfo.TipType = Load;
				TipInfo.Enable = TRUE;			
				ReturnDispPage();
			}
	  }
	  else if(KeyVal == CANCER)
	  {
		  TipInfo.Enable = FALSE;			
	  	  ReturnDispPage();
	  }	
	  break;
	case SavePage:
		GoInfo.Enable = FALSE;		
	  if(KeyVal < 10)
	  {
		  Val = OperateInfo.Input*10+KeyVal;
		  if(Val > SysInfo.PresetCount)
			Val = SysInfo.PresetCount;
		  OperateInfo.Input = Val;
		  UpdataVal(Save);	  
	  }
	  else if(KeyVal == ENTER)
	  {
			if(OperateInfo.Input > 0)
			{
				SaveEnv(OperateInfo.Input);
				TipInfo.TipType = Save;
				TipInfo.Enable = TRUE;			
				ReturnDispPage();
			}
	  }
	  else if(KeyVal == CANCER)
	  {
			TipInfo.Enable = FALSE;			
	  	ReturnDispPage();
	  }	  
	  break;
	case MenuPage: 
		GoInfo.Enable = FALSE;		
	  if(KeyVal == 1)
	  {
            OperateInfo.CurPage = AddrPage;
			InitPage(OperateInfo.CurPage);	
	  } 
	  else if(KeyVal == 2)
	  {
			OperateInfo.CurPage = BeepPage;
		InitPage(OperateInfo.CurPage); 
	  }
	 /* else if(KeyVal == 0)     
	  {
            OperateInfo.CurPage = SystemPage;
            InitPage(OperateInfo.CurPage);
      }*/
	  else if(KeyVal == CANCER)
	  {
			TipInfo.Enable = FALSE;			
	  	    ReturnDispPage();
	  }	  
	  break;
    case SystemPage://discard in 1U
		if(KeyVal == CANCER)
		{
			GoInfo.Enable = FALSE;		
			TipInfo.Enable = FALSE;		
			ReturnDispPage();
		}
		else if(KeyVal == ENTER)
		{
			if((OperateInfo.Mode == 0)&&(SysInfo.Type&0xf0))		
			{
				LcdClear();
				ShowTypeBMP(2);
				sprintf(temp,"%02d",OperateInfo.Input&0x7F);
                DrawStr(32,24,temp);
                LoadBMP(44,24,ARROW1_BMP,18,16);				
				sprintf(temp,"%02d",OperateInfo.Output&0x7F);
                DrawStr(62,24,temp);				
				SetSwitchA(OperateInfo.Input,OperateInfo.Output);
				if((OperateInfo.Input < SysInfo.InputNum)&&(!OperateInfo.InputFlag))		
					OperateInfo.Input++;
				else if((OperateInfo.Output < SysInfo.OutputNum)&&(!OperateInfo.OutputFlag))
				{
					OperateInfo.Input = 1;
					OperateInfo.InputFlag = TRUE;
					OperateInfo.Output++;
				}
				else if(OperateInfo.Input+15 < SysInfo.InputNum)		
				{
					OperateInfo.OutputFlag = TRUE;
					OperateInfo.Input += 16;
					OperateInfo.Output = 0;			
				}
				else
				{
					OperateInfo.InputFlag = FALSE;
					OperateInfo.OutputFlag = FALSE;
					OperateInfo.Input = 1;
					OperateInfo.Output = 1;			
					OperateInfo.Mode = 1;
				}
			}
			else if((OperateInfo.Mode == 1)&&(SysInfo.Type&0x0f))		
			{
				LcdClear();
				ShowTypeBMP(1);
				sprintf(temp,"%02d",OperateInfo.Input&0x7F);
                DrawStr(32,24,temp);
                LoadBMP(44,24,ARROW1_BMP,18,16);				
				sprintf(temp,"%02d",OperateInfo.Output&0x7F);
                DrawStr(62,24,temp);					
				SetSwitchV(OperateInfo.Input,OperateInfo.Output);
				if((OperateInfo.Input < SysInfo.InputNum)&&(!OperateInfo.InputFlag))		
					OperateInfo.Input++;
				else if((OperateInfo.Output < SysInfo.OutputNum)&&(!OperateInfo.OutputFlag))
				{
					OperateInfo.Input = 1;
					OperateInfo.InputFlag = TRUE;
					OperateInfo.Output++;
				}
				else
				{
					if((SysInfo.Type&TypeVGA)||(SysInfo.Type&TypeFM))
						Val = 8;		
					else
						Val = 16;
					if(OperateInfo.Input+Val-1 < SysInfo.InputNum)		
					{
						OperateInfo.OutputFlag = TRUE;
						OperateInfo.Input += Val;
						OperateInfo.Output = 0;			
					}
				}
			}
		}
      break;
	case AddrPage:
		GoInfo.Enable = FALSE;		
	  if(KeyVal < 10)		
	  {
			Val = OperateInfo.Input*10+KeyVal;
	  		if(Val > 0)
			{
				if(Val > 254)
					Val = 254;
				OperateInfo.Input = Val;
				UpdataVal(Addr);
			}
	  }
	  else if(KeyVal == ENTER)
	  {
			SysInfo.Addr = OperateInfo.Input;
			SysInfo.TypeModifyEn = TRUE;
			AT24xxWrite(FIX_ADD,&SysInfo.Addr,1);	
			SysInfo.TypeModifyEn = FALSE;
			TipInfo.TipType = Addr;
			TipInfo.Enable = TRUE;			
			ReturnDispPage();
	  }
	  else if(KeyVal == CANCER)
	  {
			TipInfo.Enable = FALSE;			
	  		ReturnDispPage();
	  }
	  break;
        case CyclePage1:
          GoInfo.Enable = FALSE;		
          if(KeyVal < 10)		
	  {
		Val = CycleTimingTmp *10+KeyVal;
	  		if(Val > 0)
			{
				if(Val > 9999)
					Val = 9999;
				CycleInfo.CycleTiming  = Val;
                                CycleTimingTmp = Val;
				UpdataVal(CycleTim);
			}
	  }
	  else if(KeyVal == ENTER)
	  {
                        CycleTimingTmp = 0;
			OperateInfo.CurPage = CyclePage2;
                        InitPage(OperateInfo.CurPage);
	  }
	  else if(KeyVal == CANCER)
	  {
                        CycleTimingTmp = 0;
			TipInfo.Enable = FALSE;			
	  		ReturnDispPage();
	  }
	  break;
        case CyclePage2:
          GoInfo.Enable = FALSE;		
          if(KeyVal < 10)		
	  {
		Val = CycleNumTmp *10+KeyVal;
	  		if(Val > 0)
			{
				if(Val > 16)
					Val = 16;
				CycleInfo.CycleNum  = Val;
                                CycleNumTmp = Val;
				UpdataVal(CycleNumber);
			}
	  }
	  else if(KeyVal == ENTER)
	  {
                        CycleNumTmp = 0;
			OperateInfo.CurPage = CyclePage3;
                        InitPage(OperateInfo.CurPage);
	  }
	  else if(KeyVal == CANCER)
	  {
                        CycleNumTmp = 0;
			TipInfo.Enable = FALSE;			
	  		ReturnDispPage();
	  }
	  break;
        case CyclePage3:
          GoInfo.Enable = FALSE;		
		if(KeyVal == 1)
		{
                  SysInfo.CycleEn = 1;//使能循环
			TipInfo.TipType = CycleOn;
			TipInfo.Enable = TRUE;				
			ReturnDispPage(); 
                }
                else if(KeyVal == 2)
		{
                  SysInfo.CycleEn = 0;//关闭循环
			TipInfo.TipType = CycleOff;
			TipInfo.Enable = TRUE;				
			ReturnDispPage();
                }
                else if((KeyVal == CANCER)||(KeyVal == ENTER))
		{
			TipInfo.Enable = FALSE;			
	  		ReturnDispPage();
		}	
                break;
	case BaudPage:
		GoInfo.Enable = FALSE;		
	  if((KeyVal < 5)&&(KeyVal > 0))
	  {
	  		SysInfo.Baud = (KeyVal - 1);
			SysInfo.TypeModifyEn = TRUE;
			AT24xxWrite(FIX_BAUD,&SysInfo.Baud,1);	
			SysInfo.TypeModifyEn = FALSE;
			Com_Config(BaudArray[SysInfo.Baud]);		
			TipInfo.TipType = Baud;
			TipInfo.Enable = TRUE;			
			ReturnDispPage();
	  }
	  else if(KeyVal == CANCER)
	  {
			TipInfo.Enable = FALSE;			
	  		ReturnDispPage();
	  }
	  break;
	case LangPage:
		GoInfo.Enable = FALSE;		
	  break;
	case BeepPage:
		GoInfo.Enable = FALSE;		
		if(KeyVal == 1)
		{
			SysInfo.BeepEn = 1;
			SysInfo.TypeModifyEn = TRUE;
			AT24xxWrite(FIX_BEEP,&SysInfo.BeepEn,1);
			SysInfo.TypeModifyEn = FALSE;
			TipInfo.TipType = Beep;
			TipInfo.Enable = TRUE;				
			ReturnDispPage();
		}
		else if(KeyVal == 2)
		{
			SysInfo.BeepEn = 0;
			SysInfo.TypeModifyEn = TRUE;
			AT24xxWrite(FIX_BEEP,&SysInfo.BeepEn,1);			
			SysInfo.TypeModifyEn = FALSE;
			TipInfo.TipType = Beep;
			TipInfo.Enable = TRUE;				
			ReturnDispPage();
		}
		else if((KeyVal == CANCER)||(KeyVal == ENTER))
		{
			TipInfo.Enable = FALSE;			
			ReturnDispPage();
		}		
	  break;
	default:
	  break;  	  	  	  	  	  
	}
}

void DisposalTick(void)
{
	if(GoInfo.Enable)
	{
		GoInfo.DlyTime++;
		if(GoInfo.DlyTime > 40) 
		{
			GoInfo.DlyTime = 0;
			ChangeDisp();
		}
	}
    
	if(++LcdBkSec == 300)		//30s
    {
		LCD_BK_OFF;
    }
    else if(LcdBkSec == 100)
    {
      if((OperateInfo.CurPage != DispPage)&&(OperateInfo.CurPage!=SystemPage))
      {
        lcd_clr();////LcdClear();
        DrawLogo();
        OperateInfo.CurPage = DispPage;        
      }    
    }
}

void InitSwitch(void)
{
	int i;

    if(SysInfo.Type&0xf0) //音频矩阵
    {
      for(i=0;i<SysInfo.OutputNum;i++)
        InOut_A[i]=0x80;
	}
    if(SysInfo.Type&0x0F) //视频矩阵
    {
        for(i=0;i<SysInfo.OutputNum;i++)
           InOut_V[i]=0x80;
    }
    if(SysInfo.OutputNum > 16)   //输出端口大于16
    {
      if(SysInfo.Type & 0x0F)		
          SetSwitchV(0,0);           //取消所有的切换
      if(SysInfo.Type & 0xF0)		
          SetSwitchA(0,0);          //取消所有的切换
    }
    else//输出端口小于16		
	{
		if((SysInfo.Type & TypeVideo)||(SysInfo.Type&TypeFM))	//视频	&FM VGA
    	{
      		for(i=0;i<SysInfo.InputNum;i++)
      		{
        		SetSwitchV(i+1,0);//N通道指向全部通道
        		SetSwitchV(0,0);  //取消所有的切换
      		}
    	}
    	if(SysInfo.Type&TypeAudio)//音频					
    	{
      		for(i=0;i<SysInfo.InputNum;i++)
      		{
        		SetSwitchA(i+1,0);
        		SetSwitchA(0,0);
      		}    
    	}
	if((SysInfo.Type&TypeVGA)||(SysInfo.Type&TypeRGB)) //	VGA&RGB
          SetSwitchV(0,0);	 //取消所有的切换	
	} //if(SysInfo.OutputNum > 16)  

    if(SysInfo.Type & TypeFM)
      InitFm();
	
    LoadEnv(0);
}

void DisposalIR(void)
{
  u8 Data,Temp;
  
  Data = (IRInfo.Data >> 16);
  Temp = (IRInfo.Data >> 24);
  if(Data&Temp)
    return;
  
  switch(Data)
  {
  case 0X45://case 0X58:
    KeyVal = 1;
    break;
  case 0X46://case 0X59:
    KeyVal = 2;
    break;
  case 0X47://case 0X45:
    KeyVal = 3;
    break;
  case 0X44://case 0X54:
    KeyVal = 4;
    break;
  case 0X40://case 0X55:
    KeyVal = 5;
    break;
  case 0X43://case 0X49:
    KeyVal = 6;
    break;
  case 0X07://case 0X50:
    KeyVal = 7;
    break;
  case 0X15://case 0X51:
    KeyVal = 8;
    break;
  case 0X09://case 0X4D:
    KeyVal = 9;
    break;
  case 0X16://case 0X1D:
    KeyVal = 0;
    break;
  case 0X08://case 0X44:
    KeyVal = A;
    break;
  case 0X1C://case 0X48:
    KeyVal = V;
    break;
  case 0X5A://case 0X4C:
    KeyVal = AV;
    break;
  case 0X52://case 0X14:
    KeyVal = CANCER;
    break;
  case 0X4A://case 0X16:
    KeyVal = ENTER;
    break;
  case 0X18://case 0X1C:
    KeyVal = SAVE;
    break;
  case 0X5E://case 0X1E:
    KeyVal = RECALL;
    break;
  case 0X0C://case 0X1F:
    KeyVal = MENU;
    break;
  }
  DisposeKey();
}
