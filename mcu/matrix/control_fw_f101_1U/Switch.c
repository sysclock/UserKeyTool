#include "Init.h"
#include "Key.h"

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





void SetSwitchV(u8 Input,u8 Output)
{
	u8 i;
	
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
	}	
    else if(SysInfo.Type & TypeFM)	  
    {
        if(Output == 0)			
        {
          for(i=0;i<SysInfo.OutputNum;i++)
          {
              SetSwitchFM(Input,i+1);
              if(Input > 0)
                  InOut_V[i] = (Input-1)|0x80;
              else
                  InOut_V[i] &= 0x7f;
          }
        }
        else			
        {
          SetSwitchFM(Input,Output);				
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
