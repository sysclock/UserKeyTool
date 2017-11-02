#include "Init.h"
#include "Key.h"

typedef struct _HDMIPointType
{
	u8 Data0;
	u8 Data1;
	u8 Data2;
	u8 Clock;
}HDMIPoint;
/*
HDMIPoint HDMIInput[]=
{
	{  2,    0,   1,    3},	    //01  
	{ 29,   27,  24,   25},	    //02  
	{ 18,   14,  12,   19},	    //03
	{ 11,    9,  10,    8},	    //04 
        
        {  7,    4,   6,    5},	    //05
        { 31,   30,  28,   26},	    //06 
        { 23,   21,  22,   20},	    //07 
        { 17,   16,  15,   13},	    //08 
};*/
//change to offset address
/*
HDMIPoint HDMIInput[]=
{
	{  5,    1,   3,    7},	    //A-01
	{ 59,   55,  49,   51},	    //B-02  ---ERR
	{ 37,   29,  25,   39},	    //C-03 ---OK
	{ 23,   19,  21,   17},	    //D-04 
        
        { 15,    9,  13,   11},	    //E-05
        { 63,   61,  57,   53},	    //F-06 ---OK
        { 47,   43,  45,   41},	    //G-07  --ERR
        { 35,   33,  31,   27},	    //H-08 ``
};
HDMIPoint HDMIOutput[]=
{
	{ 25,    23,   19,   27},	    //A-01  
	{ 11,    7,    3,    15},	    //B-02 
	{ 26,    28,   29,   20},	    //C-03    ---OK
	{ 4,     10,   14,   2 },	    //D-04  ----OK
        
        { 31,    21,   17,   18},	    //E-05  
	{  9,     5,    1,   13},	    //F-06  
	{ 22,    30,   24,   16},	    //G-07 ----ERR
	{  0,     6,   12,   8 },	    //H-08  ----OK
 	  
};*/
//------ DVI offset address remap--------
HDMIPoint HDMIInput[]=
{        
        { 63,   61,  57,   53},	    //2------F
	{ 59,   55,  49,   51},	    //1------B
        { 47,   43,  45,   41},	    //4------G        
        { 37,   29,  25,   39},	    //3------C        
        { 35,   33,  31,   27},	    //6------H
        { 23,   19,  21,   17},	    //5------D        
        { 15,    9,  13,   11},	    //8------E
        {  5,    1,   3,    7},	    //7------A
        
        /*
        {  5,    1,   3,    7},	    //A-01
	{ 59,   55,  49,   51},	    //B-02  ---ERR
	{ 37,   29,  25,   39},	    //C-03 ---OK
	{ 23,   19,  21,   17},	    //D-04 
        
        { 15,    9,  13,   11},	    //E-05
        { 63,   61,  57,   53},	    //F-06 ---OK
        { 47,   43,  45,   41},	    //G-07  --ERR
        { 35,   33,  31,   27},	    //H-08 */
};
HDMIPoint HDMIOutput[]=
{
        {  4,    10,   14,   2 },	    //1-----D
        {  0,     6,   12,   8 },	    //2-----H
        { 26,    28,   29,   20},	    //3-----C
        { 22,    30,   24,   16},	    //4-----G
        { 25,    23,   19,   27},	    //5-----A
        { 31,    21,   17,   18},	    //6-----E
        { 11,    7,    3,    15},	    //7-----B
        {  9,     5,    1,   13},	    //8-----F
        /*
	{ 25,    23,   19,   27},	    //A-01  
	{ 11,    7,    3,    15},	    //B-02 
	{ 26,    28,   29,   20},	    //C-03    ---OK
	{ 4,     10,   14,   2 },	    //D-04  ----OK
        
        { 31,    21,   17,   18},	    //E-05  
	{  9,     5,    1,   13},	    //F-06  
	{ 22,    30,   24,   16},	    //G-07 ----ERR
	{  0,     6,   12,   8 },	    //H-08  ----OK*/
 	  
};

u8* InOut_V;
u8* InOut_A;

static void Trans_16(u8 Data)
{
	u8 i,k;
	
    if((SysInfo.InputNum <= 8)&&(SysInfo.OutputNum <= 8))   
      i = 3;   
    else
      i = 4;  
    
	if(Data&0x80)	
		SDA_HI;
	else
		SDA_LOW;
	SCK_LOW;
    DelayUs(1);
	SCK_HI;
	for(k=0;k<i;k++)	
	{
		if(Data&(1<<(i-1-k)))
			SDA_HI;
		else
			SDA_LOW;
		SCK_LOW;
        DelayUs(1);
		SCK_HI;
	}
}

static void SetSwitchVGA(u8 Input,u8 Output)
{
	u8 BN;		
	u8 CN;			
	u8 CB;		
	u8 CC;			
	u8 Data;			
	u8 i,k,m,n,p;

    SCK_HI;        
    UPDATE_HI;
    

    if((SysInfo.InputNum <= 8)&&(SysInfo.OutputNum <= 8))
      m = 3;  
    else
      m = 4;  
    p = 1<<m;  
    
    if(SysInfo.OutputNum <= 8)
      n = 3;   
    else
      n = 4;   
    k = 1<<n; 
    
	BN = (SysInfo.InputNum-1)>>m;
	CN = (SysInfo.OutputNum-1)>>n;
	
    BN+=1;
    CN+=1;
    
	if(Output == 0)    
	{
		for(CB=0;CB<BN;CB++)
		{
			if((Input == 0) || (CB != ((Input-1) >> m)))
			{
    		  	SetVAddr(CB);				       
				for(CC=CN;CC>0;CC--)
				{
					for(i=k;i>0;i--)		
					{
                        Data = InOut_V[((CC-1)<<n)+i-1];
						Data &= 0x7F;		
                        Data %= p;                        
						Trans_16(Data);
					}	
				}
                UPDATE_LOW;
                DelayUs(1);
                UPDATE_HI;                
			}
        }
		if(Input)	
        {
            SetVAddr((Input-1) >> m);	
            for(CC=CN;CC>0;CC--)
            {
                for(i=k;i>0;i--)		
                {
                    Data = Input-1;
                    Data %= p;
                    Data |= 0x80;
                    Trans_16(Data);
                }	
            }
			UPDATE_LOW;
			DelayUs(1);
			UPDATE_HI;            
        }
     	SetVAddr(0x0F);
	}
	else
	{
        if((Input == 0) || ((InOut_V[Output-1]&0x80)&&(((InOut_V[Output-1]&0x7f)>>m)!=((Input-1)>>m))))
		{
			CB = (InOut_V[Output-1]&0x7f) >> m;		
			SetVAddr(CB);
	
			for(CC=CN;CC>0;CC--)
			{
				for(i=k;i>0;i--)		
				{
                    Data = InOut_V[((CC-1)<<n)+i-1];
                    if((Data&0x80)&&(((Data&0x7f)>>m)==CB)) 
                    {
                      Data &= 0x7f;
                      Data %= p;
                      Data |= 0x80;
                    }
                    else
                      Data %= p;
                    
					if(((CC-1)<<m)+i == Output)
						Data &= 0x7f;
					Trans_16(Data);
				}			
			}
			UPDATE_LOW;
			DelayUs(1);
			UPDATE_HI;
			SetVAddr(0x0F);			
		}
		if(Input > 0)
		{
			CB = (Input-1) >> m;		
			SetVAddr(CB);
            
			for(CC=CN;CC>0;CC--)
			{
				for(i=k;i>0;i--)		
				{
                    Data = InOut_V[((CC-1)<<n)+i-1];
                    if((Data&0x80)&&(((Data&0x7f)>>m)==CB))
                    {
                      Data &= 0x7f;
                      Data %= p;
                      Data |= 0x80;
                    }
                    else
                      Data %= p;
                    
					if(((CC-1)<<m)+i == Output)
                    {
						Data = Input-1;
                        Data %= p;
                        Data |= 0x80;
                    }
					Trans_16(Data);
				}			
			}
			UPDATE_LOW;
			DelayUs(1);
			UPDATE_HI;
			SetVAddr(0x0F);
		}
	}  
}

static void SetSwitchVideoPer(u8 Input,u8 Output)
{
	u8 Data;					
	u8 Board;					
	u8 i;
    
    UPDATE_LOW;
    
    if(SysInfo.InputNum <= 8)
      i = 3;
    else
      i = 4;
    
	if((InOut_V[Output-1]&0x80)&&(((InOut_V[Output-1]&0x7F)+1) != Input))
	{
		Board = (InOut_V[Output-1]&0x7f) >> i;		
		Data = (InOut_V[Output-1]&0x7f)%(1<<i);
		Data <<= 4;
		Data |= Output-1;		
		SetPortData(Data);
		SetVAddr(Board);
		DATE_LOW;		  
		UPDATE_HI;
		DelayUs(10);
		UPDATE_LOW;    
	}
	if(Input > 0)
	{
		Board = (Input-1) >> i;
		Data = (Input-1)%(1<<i);	
		Data <<= 4;
		Data |= Output-1;		
		SetPortData(Data);
		SetVAddr(Board);
		DATE_HI;		
		UPDATE_HI;
		DelayUs(10);
		UPDATE_LOW;	
	}
	SetVAddr(0x0f);		
}

static void SetSwitchVideoSer(u8 Input,u8 Output)
{
	u8 BN;		
	u8 CN;			
	u8 CB;		
	u8 CC;			
	u8 Data;			
	u8 i;

  	SCK_HI;       
    UPDATE_HI;
    
	BN = (SysInfo.InputNum-1)>>4;		
	CN = (SysInfo.OutputNum-1)>>4;		
	
    BN+=1;
    CN+=1;
    
	if(Output == 0)
	{
		for(CB=0;CB<BN;CB++)
		{
			if((Input == 0) || (CB != ((Input-1) >> 4)))	
			{
             	SetVAddr(CB);				          
				for(CC=CN;CC>0;CC--)
				{
					for(i=16;i>0;i--)		
					{
						Data = InOut_V[((CC-1)<<4)+i-1];
						Data &= 0x7F;		
                        Data %= 16;
						Trans_16(Data);
					}	
				}
                UPDATE_LOW;
                DelayUs(1);
                UPDATE_HI;
			}
        }
		if(Input)	
        {
            SetVAddr((Input-1)>>4);		
            for(CC=CN;CC>0;CC--)
            {
                for(i=16;i>0;i--)		
                {
                    Data = Input-1;
                    Data %= 16;
                    Data |= 0x80;
                    Trans_16(Data);
                }	
            }
            UPDATE_LOW;
            DelayUs(1);
            UPDATE_HI;            
        }
		SetVAddr(0x0F);
	}
	else
	{
		if((Input == 0) || ((InOut_V[Output-1]&0x80)&&(((InOut_V[Output-1]&0x7F)>>4) != ((Input-1)>>4))))
		{
			CB = (InOut_V[Output-1]&0x7f) >> 4;		
			if(CB > 7)
				return;
			SetVAddr(CB);
	
			for(CC=CN;CC>0;CC--)
			{
				for(i=16;i>0;i--)		
				{
                    Data = InOut_V[((CC-1)<<4)+i-1];
                    if((Data&0x80)&&(((Data&0x7f)>>4)==CB))
                    {
                      Data &= 0x7f;
                      Data %= 16;
                      Data |= 0x80;
                    }
                    else
                      Data %= 16;
                    
                    if(((CC-1)<<4)+i == Output)
                      Data &= 0x7f;
					Trans_16(Data);
				}			
			}
			UPDATE_LOW;
			DelayUs(1);
			UPDATE_HI;
			SetVAddr(0x0F);			
		}
		if(Input > 0)
		{
			CB = (Input-1) >> 4;		
			if(CB > 7)
				return;
			SetVAddr(CB);
	
			for(CC=CN;CC>0;CC--)
			{
				for(i=16;i>0;i--)		
				{
                    Data = InOut_V[((CC-1)<<4)+i-1];
                    if((Data&0x80)&&(((Data&0x7f)>>4)==CB))
                    {
                      Data &= 0x7f;
                      Data %= 16;
                      Data |= 0x80;
                    }
                    else
                      Data %= 16;
                    
                    if(((CC-1)<<4)+i == Output)
                    {
						Data = Input-1;
                        Data %= 16;
                        Data |= 0x80;
                    }
					Trans_16(Data);
				}			
			}
			UPDATE_LOW;
			DelayUs(1);
			UPDATE_HI;
			SetVAddr(0x0F);
		}
	}
}

static void SetSwitchAudioPer(u8 Input,u8 Output)
{
	u8 Data;					
	u8 Board;					
	u8 i;
    
    UPDATE_LOW;
    
    if(SysInfo.InputNum <= 8)
      i = 3;
    else
      i = 4;
	
	if((InOut_A[Output-1]&0x80)&&(((InOut_A[Output-1]&0x7F)+1) != Input))
	{
		Board = (InOut_A[Output-1]&0x7f) >> i;		
		Data = (InOut_A[Output-1]&0x7f)%(1<<i);	
		Data <<= 4;
		Data |= Output-1;		
		SetPortData(Data);
		SetAAddr(Board);
		DATE_LOW;		  
		UPDATE_HI;
		DelayUs(10);
		UPDATE_LOW;    
	}
	if(Input > 0)
	{
		Board = (Input-1) >> i;
		Data = (Input-1)%(1<<i);	
		Data <<= 4;
		Data |= Output-1;		
		SetPortData(Data);
		SetAAddr(Board);
		DATE_HI;		
		UPDATE_HI;
		DelayUs(10);
		UPDATE_LOW;	
	}
	SetAAddr(0x0f);		
}

static void SetSwitchAudioSer(u8 Input,u8 Output)
{
	u8 BN;		
	u8 CN;			
	u8 CB;		
	u8 CC;			
	u8 Data;			
	u8 i;

  	SCK_HI;         
    UPDATE_HI;
    
	BN = (SysInfo.InputNum-1)>>4;		
	CN = (SysInfo.OutputNum-1)>>4;		
	
    BN+=1;
    CN+=1;
    
	if(Output == 0)
	{
		for(CB=0;CB<BN;CB++)
		{
			if((Input == 0) || (CB != ((Input-1) >> 4)))	
			{
    		  	SetAAddr(CB);				         
				for(CC=CN;CC>0;CC--)
				{
					for(i=16;i>0;i--)		
					{
						Data = InOut_A[((CC-1)<<4)+i-1];
						Data &= 0x7F;		
                        Data %= 16;
						Trans_16(Data);
					}	
				}
                UPDATE_LOW;
                DelayUs(1);
                UPDATE_HI;                            
			}
        }
        if(Input)  
        {
            SetAAddr((Input-1) >> 4);				
            for(CC=CN;CC>0;CC--)
            {
                for(i=16;i>0;i--)		
                {
                    Data = Input-1;
                    Data %= 16;
                    Data |= 0x80;
                    Trans_16(Data);
                }	
            }
          UPDATE_LOW;
          DelayUs(1);
          UPDATE_HI;              
        }
        SetAAddr(0x0F);
	}
	else
	{
		if((Input == 0) || ((InOut_A[Output-1]&0x80)&&(((InOut_A[Output-1]&0x7F)>>4) != ((Input-1)>>4))))
		{
			CB = (InOut_A[Output-1]&0x7f) >> 4;			
			SetAAddr(CB);
	
			for(CC=CN;CC>0;CC--)
			{
				for(i=16;i>0;i--)		
				{
                    Data = InOut_A[((CC-1)<<4)+i-1];
                    if((Data&0x80)&&(((Data&0x7f)>>4)==CB))
                    {
                      Data &= 0x7f;
                      Data %= 16;
                      Data |= 0x80;
                    }
                    else
                      Data %= 16;
                    
                    if(((CC-1)<<4)+i == Output)
                      Data &= 0x7f;
					Trans_16(Data);
				}			
			}
			UPDATE_LOW;
			DelayUs(1);
			UPDATE_HI;
			SetAAddr(0x0F);			
		}
		if(Input > 0)
		{
			CB = (Input-1) >> 4;		
			SetAAddr(CB);
	
			for(CC=CN;CC>0;CC--)
			{
				for(i=16;i>0;i--)		
				{
                    Data = InOut_A[((CC-1)<<4)+i-1];
                    if((Data&0x80)&&(((Data&0x7f)>>4)==CB))
                    {
                      Data &= 0x7f;
                      Data %= 16;
                      Data |= 0x80;
                    }
                    else
                      Data %= 16;
                    
                    if(((CC-1)<<4)+i == Output)
                    {
						Data = Input-1;
                        Data %= 16;
                        Data |= 0x80;
                    }
					Trans_16(Data);
				}			
			}
			UPDATE_LOW;
			DelayUs(1);
			UPDATE_HI;
			SetAAddr(0x0F);
		}
	}  
}

#define FM_ADDR 0x06

static void IICStart(void)
{
	FM_SDA_HI;
	FM_SCK_HI;
	DelayUs(5);
	FM_SDA_LOW;
	DelayUs(5);
	FM_SCK_LOW;
	DelayUs(10);
}

static void IICStop(void)
{
	FM_SDA_LOW;
	FM_SCK_HI;
	DelayUs(5);
	FM_SDA_HI;
	DelayUs(5);
	FM_SCK_LOW;
	DelayUs(10);
}

static u8 IICGetAck(void)
{
  	u8 Val;
	
	FM_SDA_INPUT;
	FM_SDA_HI;
	FM_SCK_HI;
	DelayUs(5);
	Val = FM_SDA_IS_HI;
	DelayUs(5);
	FM_SCK_LOW;
	DelayUs(10);
	FM_SDA_OUTPUT;
	return Val;
}

static void IICWriteByte(char c)
{
	int i;
	for(i=0;i<8;i++)
	{
		if(c&0x80)
		  FM_SDA_HI;
		else
		  FM_SDA_LOW;
		
		FM_SCK_HI;
		DelayUs(10);
		FM_SCK_LOW;
		DelayUs(10);
		c <<= 1;
	}
}

static void FMCtrl(u8 Input,u8 Output)
{
	IICStart(); 
	IICWriteByte(FM_ADDR);  
    IICGetAck();
	IICWriteByte(Output);
    IICGetAck();
	if(Input != 0)
		IICWriteByte(0x80|Input);
	else
		IICWriteByte(Input);
    IICGetAck();
	IICStop();
}

static void SetSwitchFM(u8 Input,u8 Output)
{
	u8 Data;					
	u8 Board;					

	if((InOut_V[Output-1]&0x80)&&(((InOut_V[Output-1]&0x7F)+1) != Input))
	{
		Board = (InOut_V[Output-1]&0x7f) >> 3;	
        SetVAddr(Board);
        FMCtrl(0,Output);
        SetVAddr(0x0F);
	}

	if(Input > 0)
	{
		Board = (Input-1) >> 3;
		Data = (Input-1)%8;	
        SetVAddr(Board);
        FMCtrl(Data+1,Output);
	    SetVAddr(0x0f);		
	}
}

void InitFm()
{
	u8 BN,i;
	
	BN = (SysInfo.InputNum-1)>>3;
	for(i=0;i<BN+1;i++)
	{
		SetVAddr(i);
		FMCtrl(0xFF,0x1d);
		FMCtrl(0xFF,0x1e);
		SetVAddr(0x0f);		
	}
}


void PowerUp211x1(void)
{
	u8 cTick=20;
	HDMI_xRST_LW;// = 0;
	HDMI_xCS_HI;// = 1;
	while(cTick--)
	{
		HDMI_SCLK_HI;// = 1;
		DelayUs(1);
		HDMI_SCLK_LW;// = 0;
		DelayUs(1);
	}
	HDMI_xRST_HI;// = 1;
	cTick = 20;
	while(cTick--)
	{
		HDMI_SCLK_HI;// = 1;
		DelayUs(1);
		HDMI_SCLK_LW;// = 0;
		DelayUs(1);
	}
	HDMI_xINDIS_HI;// 	= 1;
	HDMI_xOUTDIS_HI;// 	= 1;
	
}

void Write211x1OneByte(u16 Addr, u8 Data)
{
	u32 wBit = 0x20000;
	u32 Addr2 = 0x20000;
	Addr2 = Addr;
	Addr2 <<= 0x08;
	Addr2 |= Data;
	Addr2 |= 0x20000;
	HDMI_SCLK_HI;//HDMI_SCLK = 1;
	HDMI_xCS_LW;//HDMI_xCS = 0;
	DelayUs(1);	
	while(wBit)
	{		
		if(Addr2 & wBit)
			HDMI_SDI_HI;//HDMI_SDI = 1;
		else
			HDMI_SDI_LW;//HDMI_SDI = 0;
		wBit >>=1;
		if(wBit == 0x00)
			HDMI_xCS_HI;//HDMI_xCS = 1;	
		HDMI_SCLK_HI;//HDMI_SCLK= 1;
		DelayUs(1);	
		HDMI_SCLK_LW;//HDMI_SCLK = 0;
		DelayUs(1);	
	}
	HDMI_SCLK_HI;//HDMI_SCLK = 1;
	HDMI_SDI_HI;//HDMI_SDI = 1;
	DelayUs(1);	
	HDMI_SCLK_LW;//HDMI_SCLK = 0;
}

   
u8 Read211x1OneByte(u16 Addr)
{
	u16 wBit = 0x200;
	u16 Addr2 = 0x300;
	u16 Result;
	Addr2 |= Addr;
        HDMI_SDO_INPUT;
	HDMI_SCLK_HI;//HDMI_SCLK = 1;
	HDMI_xCS_LW;//HDMI_xCS = 0;
	DelayUs(1);	
	while(wBit)
	{
		if(Addr2 & wBit)
			HDMI_SDI_HI;//HDMI_SDI = 1;
		else
			HDMI_SDI_LW;//HDMI_SDI = 0;
		wBit >>=1;
		HDMI_SCLK_HI;//HDMI_SCLK = 1;
		DelayUs(1);	
		HDMI_SCLK_LW;//HDMI_SCLK= 0;
		DelayUs(1);	
	}
	wBit = 0x200;
	Result = 0x00;
	HDMI_SDI_HI;//HDMI_SDI = 1;
	
	while(wBit)
	{
		DelayUs(1);	
		HDMI_SCLK_LW;//HDMI_SCLK = 0;
		DelayUs(1);	
		HDMI_SCLK_HI;//HDMI_SCLK = 1;
		if(HDMI_SDO_IS_HI)
		{
			Result |= wBit;
		}
		wBit >>= 1;
		if(wBit == 0x00)
			HDMI_xCS_HI;//HDMI_xCS = 1;	
	}
	
	HDMI_SCLK_HI;//HDMI_SCLK = 1;
	DelayUs(1);	
	HDMI_SCLK_LW;//HDMI_SCLK = 0;
	return (u8)(Result>>1);	 	
}


void ChangePort(u8 Input,u8 Output)
{
	u8 Reg,Value;	
 	if(Input>=sizeof(HDMIInput)/sizeof(HDMIPoint))
		return;
	if(Output>=sizeof(HDMIOutput)/sizeof(HDMIPoint))
		return;
 
		Reg = HDMIOutput[Output].Data0;
		Value = HDMIInput[Input].Data0;
		Write211x1OneByte(Reg,Value);

		Reg = HDMIOutput[Output].Data1;
		Value = HDMIInput[Input].Data1;
		Write211x1OneByte(Reg,Value);

		Reg = HDMIOutput[Output].Data2;
		Value = HDMIInput[Input].Data2;
		Write211x1OneByte(Reg,Value);

		Reg = HDMIOutput[Output].Clock;
		Value = HDMIInput[Input].Clock;
		Write211x1OneByte(Reg,Value);		 
}
 
void PowerUpInputChannel(void)
{
	u8 Data,cTick;
	u16 Address;
	for(cTick = 0; cTick<8;cTick++)
	{
		Address =  HDMIInput[cTick].Data0;
		Data = Read211x1OneByte(Address);
		Write211x1OneByte(Address,Data);
		DelayUs(1);	

		Address =  HDMIInput[cTick].Data1;
		Data = Read211x1OneByte(Address);
		Write211x1OneByte(Address,Data);
		DelayUs(1);	

		Address =   HDMIInput[cTick].Data2;
		Data = Read211x1OneByte(Address);
		Write211x1OneByte(Address,Data);
		DelayUs(1);	

		Address =   HDMIInput[cTick].Clock;
		Data = Read211x1OneByte(Address);
		Write211x1OneByte(Address,Data);
		DelayUs(1);	
		 
	}
	for(cTick = 0; cTick<8;cTick++)
	{
		Address = HDMIOutput[cTick].Data0;
		Data = Read211x1OneByte(Address);
		Write211x1OneByte(Address,Data);
		DelayUs(1);	

		Address = HDMIOutput[cTick].Data1;
		Data = Read211x1OneByte(Address);
		Write211x1OneByte(Address,Data);
		DelayUs(1);	

		Address = HDMIOutput[cTick].Data2;
		Data = Read211x1OneByte(Address);
		Write211x1OneByte(Address,Data);
		DelayUs(1);	

		Address = HDMIOutput[cTick].Clock;
		Data = Read211x1OneByte(Address);
		Write211x1OneByte(Address,Data);
		DelayUs(1);	
	}
}

void InitDVI()
{
  u8 cTick;
  PowerUp211x1();
  Write211x1OneByte(0xEF,0x01); DelayUs(1);	//reset
  Write211x1OneByte(0xEF,0x01);DelayUs(1);	//reset
  
  Write211x1OneByte(0xEC,0x03); //de-emphasis applitude: 50%, 600ps, enable
  Write211x1OneByte(0xEB,0x04);	//This register must be set to 04h.
  /*
  cTick = Read211x1OneByte(0xF0);
  USART1->DR = cTick;	  DelayMs(50);
  cTick = Read211x1OneByte(0xEB);
  USART1->DR = cTick;	  DelayMs(50);
   cTick = Read211x1OneByte(0xE7);
  USART1->DR = cTick;	  DelayMs(50);
  
   cTick = Read211x1OneByte(0xEC);
  USART1->DR = cTick;	  DelayMs(50);*/
   
   
  for(cTick = 0x22;cTick<0x44;cTick++)
  {
    Write211x1OneByte(cTick,0x44);  // Input Channel Selection
    DelayUs(1);
  }
  //--------------------
  for(cTick = 0x80;cTick<0xA1;cTick+=2)
  {
	Write211x1OneByte(cTick,0x01);  // Enable OutPut
	DelayUs(1);
  }
  //--------------------
  for(cTick = 0x81;cTick<0xC4;cTick+=2)
  {
    Write211x1OneByte(cTick,0x03);  // Enable InPut
    DelayUs(1);
  }	
  
  /*USART1->DR  = 0x00;DelayMs(10);USART1->DR  = 0x00;DelayMs(10);
  USART1->DR  = 0x00;DelayMs(10);USART1->DR  = 0x00;DelayMs(10);
  for(cTick = 0x81;cTick<0xC4;cTick+=2)
  {
   USART1->DR  =  Read211x1OneByte(cTick);  // Enable InPut
    DelayMs(10);
  }	*/
  //--------------------
  for(cTick = 0;cTick <8;cTick++)
  {
    ChangePort(cTick,cTick);// input  --->  output
  }  
  //读输出寄存器
  
}
void SetSwitchV(u8 Input,u8 Output)
{
	u8 i,cTick;
	
	if((Input > SysInfo.InputNum)||(Output > SysInfo.OutputNum))
		return;
	
	if((SysInfo.Type&0x0F) == TypeVideo)			
	{
		if(SysInfo.OutputNum <= 16)			
		{
			if(Output == 0)		
			{
				for(i=0;i<SysInfo.OutputNum;i++)
				{
					SetSwitchVideoPer(Input,i+1);
					if(Input > 0)
						InOut_V[i] = (Input-1)|0x80;
					else
						InOut_V[i] &= 0x7f;
				}
			}
			else				
			{
				SetSwitchVideoPer(Input,Output);
				if(Input > 0)
					InOut_V[Output-1] = (Input-1)|0x80;
				else
					InOut_V[Output-1] &= 0x7f;		
			}
		}
		else		
		{
			if(Output == 0)			
		    {
				SetSwitchVideoSer(Input,Output);
				for(i=0;i<SysInfo.OutputNum;i++)
				{
					if(Input > 0)
						InOut_V[i] = (Input-1)|0x80;
					else
						InOut_V[i] &= 0x7f;
				}
			}
			else			
			{
				SetSwitchVideoSer(Input,Output);				
				if(Input > 0)
					InOut_V[Output-1] = (Input-1)|0x80;
				else
					InOut_V[Output-1] &= 0x7f;		
			}
		}
	}	//if((SysInfo.Type&0x0F) == TypeVideo)	
    else if(SysInfo.Type & TypeDVI)	  
    {//USART1->DR = 0X80;	  DelayMs(50);
        if(Output == 0)			
        {
          for(i=0;i<SysInfo.OutputNum;i++)
          {
            if((Input > 0))
            {
              ChangePort(Input-1,i);//SetSwitchFM(Input,i+1);
            }
            else
            {
              for(cTick = 0;cTick<34;cTick++)
              {
                Write211x1OneByte(cTick,0x00);  // 关闭所有通道
                DelayUs(2);
              }	
            }
              if(Input > 0)
                  InOut_V[i] = (Input-1)|0x80;
              else
                  InOut_V[i] &= 0x7f;
          }
        }
        else			
        {
          //USART1->DR = Input;	  DelayMs(50);
          //USART1->DR = Output;	  DelayMs(50);
          ChangePort(Input-1,Output-1);//SetSwitchFM(Input,Output);				
          if(Input > 0)
              InOut_V[Output-1] = (Input-1)|0x80;
          else
              InOut_V[Output-1] &= 0x7f;	
        }  
    }//else if(SysInfo.Type & TypeDVI)	
	else	//TypeVGA		
    {
		if(Output == 0)		
		{
		  SetSwitchVGA(Input,Output);
		  for(i=0;i<SysInfo.OutputNum;i++)
		  {
			  if(Input > 0)
				  InOut_V[i] = (Input-1)|0x80;
			  else
				  InOut_V[i] &= 0x7f;
		  }
		}
		else		
		{
		  SetSwitchVGA(Input,Output);				
		  if(Input > 0)
			  InOut_V[Output-1] = (Input-1)|0x80;
		  else
			  InOut_V[Output-1] &= 0x7f;	
		}    
    }
}

void SetSwitchA(u8 Input,u8 Output)
{
	int i;
	if((Input > SysInfo.InputNum)||(Output > SysInfo.OutputNum))
		return;
	
	if(SysInfo.OutputNum <= 16)			
	{
		if(Output == 0)		
		{
			for(i=0;i<SysInfo.OutputNum;i++)
			{
				SetSwitchAudioPer(Input,i+1);
				if(Input > 0)
					InOut_A[i] = (Input-1)|0x80;
				else
					InOut_A[i] &= 0x7f;
			}
		}
		else			
		{
			SetSwitchAudioPer(Input,Output);
			if(Input > 0)
				InOut_A[Output-1] = (Input-1)|0x80;
			else
				InOut_A[Output-1] &= 0x7f;	
		}
	}
	else
    {
        if(Output == 0)		
        {
            SetSwitchAudioSer(Input,Output);
            for(i=0;i<SysInfo.OutputNum;i++)
            {
                if(Input > 0)
                    InOut_A[i] = (Input-1)|0x80;
                else
                    InOut_A[i] &= 0x7f;
            }
        }
        else			
        {
            SetSwitchAudioSer(Input,Output);
            if(Input > 0)
                InOut_A[Output-1] = (Input-1)|0x80;
            else
                InOut_A[Output-1] &= 0x7f;		
        }        
    }
}
