///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT626X_drv.c>
//   @author Jau-chih.Tseng@ite.com.tw
//   @date   2012/06/29
//   @fileversion: IT626X_BASE_M1_MYSON_V206
//******************************************/

/////////////////////////////////////////////////////////////////////
// IT626X.C
// Driver code for platform independent
/////////////////////////////////////////////////////////////////////
#include "stm32f10x.h"

#include "hdmitx.h"
#include "IO.h"

//
#define FALLING_EDGE_TRIGGER


#define MSCOUNT 1000
#define LOADING_UPDATE_TIMEOUT (3000/32)    // 3sec

INSTANCE Instance[HDMITX_INSTANCE_MAX] ;

#if 0
extern unsigned char  I2C_ReadBuf_FromDevice(u8 eepaddr,u8 addr ,u8* pBuffer,u16 no);
extern unsigned char  I2C_ByteWrite_ToDevice(u8 eepaddr,u8 addr,u8 dat);
#endif

//////////////////////////////////////////////////////////////////////
// Function Prototype
//////////////////////////////////////////////////////////////////////

// static BOOL IsRxSense();

static void SetInputMode(BYTE InputMode,BYTE bInputSignalType);
static void SetCSCScale(BYTE bInputMode,BYTE bOutputMode);
// static void SetupAFE(BYTE ucFreqInMHz);
static void SetupAFE(VIDEOPCLKLEVEL PCLKLevel);
static void FireAFE(void);


//atic SYS_STATUS SetAudioFormat(BYTE NumChannel,BYTE AudioEnable,BYTE bSampleFreq,BYTE AudSWL,BYTE AudioCatCode);
static SYS_STATUS SetNCTS(/*ULONG PCLK,*/BYTE Fs);

static void AutoAdjustAudio(void);
static SYS_STATUS SetAVIInfoFrame(AVI_InfoFrame *pAVIInfoFrame);
static SYS_STATUS SetAudioInfoFrame(Audio_InfoFrame *pAudioInfoFrame);
static SYS_STATUS SetSPDInfoFrame(SPD_InfoFrame *pSPDInfoFrame);
static SYS_STATUS SetMPEGInfoFrame(MPEG_InfoFrame *pMPGInfoFrame);
static SYS_STATUS ReadEDID(BYTE *pData,BYTE bSegment,BYTE offset,SHORT Count);
static void AbortDDC(void);
static void ClearDDCFIFO(void);
static void ClearDDCFIFO(void);
static void GenerateDDCSCLK(void);
static SYS_STATUS HDCP_EnableEncryption(void);
static void HDCP_ResetAuth(void);
static void HDCP_Auth_Fire(void);
static void HDCP_StartAnCipher(void);
static void HDCP_StopAnCipher(void);
static void HDCP_GenerateAn(void);
static SYS_STATUS HDCP_GetVr(BYTE *pVr);
static SYS_STATUS HDCP_GetBCaps(PBYTE pBCaps ,PUSHORT pBStatus);
static SYS_STATUS HDCP_GetBKSV(BYTE *pBKSV);
static SYS_STATUS HDCP_Authenticate(void);
static SYS_STATUS HDCP_Authenticate_Repeater(void);
static SYS_STATUS HDCP_VerifyIntegration(void);
static SYS_STATUS HDCP_GetKSVList(BYTE *pKSVList,BYTE cDownStream);
static SYS_STATUS HDCP_CheckSHA(BYTE M0[],USHORT BStatus,BYTE KSVList[],int devno,BYTE Vr[]);
static void HDCP_ResumeAuthentication(void);
static void HDCP_Reset(void);
static void HDCP_CancelRepeaterAuthenticate(void);


static void ENABLE_NULL_PKT(void);
static void ENABLE_ACP_PKT(void);
static void ENABLE_ISRC1_PKT(void);
static void ENABLE_ISRC2_PKT(void);
static void ENABLE_AVI_INFOFRM_PKT(void);
static void ENABLE_AUD_INFOFRM_PKT(void);
static void ENABLE_SPD_INFOFRM_PKT(void);
static void ENABLE_MPG_INFOFRM_PKT(void);

static void DISABLE_NULL_PKT(void);
static void DISABLE_ACP_PKT(void);
static void DISABLE_ISRC1_PKT(void);
static void DISABLE_ISRC2_PKT(void);
static void DISABLE_AVI_INFOFRM_PKT(void);
static void DISABLE_AUD_INFOFRM_PKT(void);
static void DISABLE_SPD_INFOFRM_PKT(void);
static void DISABLE_MPG_INFOFRM_PKT(void);
static BYTE countbit(BYTE b);
//_IDATA ULONG CurrCTS;
_IDATA BYTE LastRefaudfreqnum=0;

BOOL BootFlag=TRUE;
extern BOOL ReGenTimingEnable;
extern BOOL bChangeMode;
extern BOOL bForceCTS;
_IDATA BYTE AudioDelayCnt=0;
//////////////////////////////////////////////////////////////////////
// Function Body.
//////////////////////////////////////////////////////////////////////


#ifndef EXTERN_CSC_TABLE
    // Y,C,RGB offset
    // for register 73~75
    _CODE BYTE bCSCOffset_16_235[] =
    {
        0x00,0x80,0x00
    };

    _CODE BYTE bCSCOffset_0_255[] =
    {
        0x10,0x80,0x10
    };


    #ifdef SUPPORT_INPUTRGB
        _CODE BYTE bCSCMtx_RGB2YUV_ITU601_16_235[] =
        {
            0xB2,0x04,0x64,0x02,0xE9,0x00,
            0x93,0x3C,0x18,0x04,0x56,0x3F,
            0x49,0x3D,0x9F,0x3E,0x18,0x04
        } ;

        _CODE BYTE bCSCMtx_RGB2YUV_ITU601_0_255[] =
        {
            0x09,0x04,0x0E,0x02,0xC8,0x00,
            0x0E,0x3D,0x84,0x03,0x6E,0x3F,
            0xAC,0x3D,0xD0,0x3E,0x84,0x03
        } ;

        _CODE BYTE bCSCMtx_RGB2YUV_ITU709_16_235[] =
        {
            0xB8,0x05,0xB4,0x01,0x93,0x00,
            0x49,0x3C,0x18,0x04,0x9F,0x3F,
            0xD9,0x3C,0x10,0x3F,0x18,0x04
        } ;

        _CODE BYTE bCSCMtx_RGB2YUV_ITU709_0_255[] =
        {
            0xE5,0x04,0x78,0x01,0x81,0x00,
            0xCE,0x3C,0x84,0x03,0xAE,0x3F,
            0x49,0x3D,0x33,0x3F,0x84,0x03
        } ;
    #endif

    #ifdef SUPPORT_INPUTYUV

        _CODE BYTE bCSCMtx_YUV2RGB_ITU601_16_235[] =
        {
            0x00,0x08,0x6A,0x3A,0x4F,0x3D,
            0x00,0x08,0xF7,0x0A,0x00,0x00,
            0x00,0x08,0x00,0x00,0xDB,0x0D
        } ;

        _CODE BYTE bCSCMtx_YUV2RGB_ITU601_0_255[] =
        {
            0x4F,0x09,0x81,0x39,0xDF,0x3C,
            0x4F,0x09,0xC2,0x0C,0x00,0x00,
            0x4F,0x09,0x00,0x00,0x1E,0x10
        } ;

        _CODE BYTE bCSCMtx_YUV2RGB_ITU709_16_235[] =
        {
            0x00,0x08,0x53,0x3C,0x89,0x3E,
            0x00,0x08,0x51,0x0C,0x00,0x00,
            0x00,0x08,0x00,0x00,0x87,0x0E
        } ;

        _CODE BYTE bCSCMtx_YUV2RGB_ITU709_0_255[] =
        {
            0x4F,0x09,0xBA,0x3B,0x4B,0x3E,
            0x4F,0x09,0x56,0x0E,0x00,0x00,
            0x4F,0x09,0x00,0x00,0xE7,0x10
        } ;
    #endif
#else
    extern _CODE BYTE bCSCOffset_16_235[] ;
    extern _CODE BYTE bCSCOffset_0_255[] ;
    extern _CODE BYTE bCSCMtx_RGB2YUV_ITU601_16_235[] ;
    extern _CODE BYTE bCSCMtx_RGB2YUV_ITU601_0_255[] ;
    extern _CODE BYTE bCSCMtx_RGB2YUV_ITU709_16_235[] ;
    extern _CODE BYTE bCSCMtx_RGB2YUV_ITU709_0_255[] ;
    extern _CODE BYTE bCSCMtx_YUV2RGB_ITU601_16_235[] ;
    extern _CODE BYTE bCSCMtx_YUV2RGB_ITU601_0_255[] ;
    extern _CODE BYTE bCSCMtx_YUV2RGB_ITU709_16_235[] ;
    extern _CODE BYTE bCSCMtx_YUV2RGB_ITU709_0_255[] ;

#endif // extern table
//////////////////////////////////////////////////////////////////////
// external Interface                                                         //
//////////////////////////////////////////////////////////////////////
BYTE LVDS_ReadI2C_Byte(BYTE RegAddr)
{
	BYTE p_data;

	i2c_read_byte(0x66, RegAddr, 1, &p_data, I2CDEV);
	//I2C_ReadBuf_FromDevice(0x66,RegAddr,  &p_data,1);


	return p_data;
}

SYS_STATUS LVDS_WriteI2C_Byte(BYTE RegAddr, BYTE d)
{
	BOOL flag;

	flag = i2c_write_byte(0x66, RegAddr, 1, &d, I2CDEV);
	//I2C_ByteWrite_ToDevice(0x66,RegAddr,d);
	//return ER_SUCCESS;
	return !flag;
}
void InitLVDS()
{
    Switch_HDMITX_Bank(0);
    HDMITX_WriteI2C_Byte(0x1d,0x66);
    HDMITX_WriteI2C_Byte(0x1E,0x01);
#ifdef SUPPORT_REGEN_TIMING
    EnableDeOnly(FALSE);
#endif
    ResetLVDS();
    delay1ms(10);
    SetLVDSinterface();
    delay1ms(10);
    SetLVDS_AFE();
    delay1ms(500);
}
void ResetLVDS(void)
{
    LVDS_AndREG_TX_Byte(0x3c,0xfe);//LVDS AFE PLL RESET
    delay1ms(1);
    LVDS_OrREG_TX_Byte(0x3c,0x01);

    LVDS_OrREG_TX_Byte(0x05,0x02);//RESET LVDS PCLK
    delay1ms(1);
    LVDS_AndREG_TX_Byte(0x05,0xfd);
#ifdef IT626X_SSC
    LVDS_OrREG_TX_Byte(0x2c,0x40);//SRAM RESET
    delay1ms(1);
    LVDS_AndREG_TX_Byte(0x2c,0xbf);
#endif
}
void SetLVDS_AFE(void)
{
    LVDS_WriteI2C_Byte(0x3e,0xaa);//dont ask me "What is it"
    LVDS_WriteI2C_Byte(0x3f,0x02);//dont ask me "What is it"
    LVDS_WriteI2C_Byte(0x47,0xaa);//dont ask me "What is it"
    LVDS_WriteI2C_Byte(0x48,0x02);//dont ask me "What is it"
    LVDS_WriteI2C_Byte(0x4f,0x11);//dont ask me "What is it"

    LVDS_OrREG_TX_Byte(0x0b,0x01);
#ifdef IT626X_SSC
    LVDS_OrREG_TX_Byte(0x3c,0x07); //[1]PLL POWER DOWN [2]RCLK SELECT
    LVDS_AndREG_TX_Byte(0x2c,(~(1<<6)));

    LVDS_OrREG_TX_Byte(0x2c,0x40); //SRAM RESET
    delay1ms(1);
    LVDS_AndREG_TX_Byte(0x2c,0xbf);
#else
    LVDS_AndREG_TX_Byte(0x3c,(~0x07));
    LVDS_OrREG_TX_Byte(0x2c,(1<<6));
#endif
    if(100000000L<GetInputPclk())
        LVDS_AndREG_TX_Byte(0x39,0x3f);
    else
        LVDS_OrREG_TX_Byte(0x39,0xc0);

#ifdef SUPPORT_MAP3
    LVDS_OrREG_TX_Byte(0x2c,(1<<4));
#else
    LVDS_AndREG_TX_Byte(0x2c,~(1<<4));
#endif

    LVDS_OrREG_TX_Byte(0x39,0x02);
    LVDS_AndREG_TX_Byte(0x39,0xfd);

//    LVDS_WriteI2C_Byte(0x2c,0x51);//dont ask me "What is it"
    delay1ms(20);

}
BOOL SetLVDSinterface(void)
{
    BYTE uc;
    uc=(LVDS_ReadI2C_Byte(0x2c)&0x03);
   // Switch_HDMITX_Bank(0);
   // HDMITX_WriteI2C_Byte(0x1d,0x66);
#ifdef SUPPORT_LVDS_8_BIT
    if(0x01 != uc)
    {
        LVDS_AndREG_TX_Byte(0x2c,0xfc);//[1:0]=("00"-6 bit),("01"-8 bit),("10"-10 bit)
        LVDS_OrREG_TX_Byte(0x2c,0x01);
    }
#endif
#ifdef SUPPORT_LVDS_6_BIT
    if(0x00 != uc)
    {
        LVDS_AndREG_TX_Byte(0x2c,0xfc);//[1:0]=("00"-6 bit),("01"-8 bit),("10"-10 bit)
        LVDS_OrREG_TX_Byte(0x2c,0x00);
    }
#endif
#ifdef SUPPORT_LVDS_10_BIT
    if(0x02 != uc)
    {
        LVDS_AndREG_TX_Byte(0x2c,0xfc);//[1:0]=("00"-6 bit),("01"-8 bit),("10"-10 bit)
        LVDS_OrREG_TX_Byte(0x2c,0x02);
    }
#endif

    uc=(LVDS_ReadI2C_Byte(0x2c)&(1<<7));
#ifdef SUPPORT_DISO
    if(uc != (1<<7))
    {
        LVDS_OrREG_TX_Byte(0x2c,(1<<7));
        LVDS_OrREG_TX_Byte(0x52,(1<<1));
    }
#else
    if(uc != (0<<7))
    {
        LVDS_AndREG_TX_Byte(0x2c,(~(1<<7)));
        LVDS_AndREG_TX_Byte(0x52,(~(1<<1)));
    }
#endif
    return TRUE;
}
WORD GetInputClockCount(void)
{
    BYTE u1,u2;
    WORD temp1;
    u1 = LVDS_ReadI2C_Byte(0x58);
    u2 = LVDS_ReadI2C_Byte(0x57);
    temp1=((WORD)u1<<8);
    temp1+=u2;
    return temp1;
}
ULONG GetInputPclk(void)
{
    BYTE u3,StableCnt=0,LoopCnt=20;
    WORD LastTemp;
    WORD temp1;
    while(LoopCnt--)
    {
        temp1=GetInputClockCount();
        if( (temp1<(LastTemp-(LastTemp>>6)))||
            (temp1>(LastTemp+(LastTemp>>6))))
        {
            StableCnt=0;
        }
        else
        {
            StableCnt++;
            //delay1ms(temp1%5);
        }
        if(StableCnt>5)break;
        LastTemp=temp1;
    }

    u3=LVDS_ReadI2C_Byte(0x2c);
    if(u3&0x80)
        temp1/=2;
    if(0x40&u3)
    {
        return ((49000000L/temp1)<<8);
    }
    else
    {
        return ((27000000L/temp1)<<10);
    }
}

BOOL CheckLVDS(void)
{
    BYTE u1;
    static BYTE LVDS_UnStable_Cnt=0;
    SetLVDSinterface();
    u1=(0x03&LVDS_ReadI2C_Byte(0x30));
    if(0x03!=u1)//0x30[1] LVDS Lock [0] LVDS Stable
    {
        //DisableAudioOutput();

        LVDS_UnStable_Cnt++;
    }
    else
    {
        LVDS_UnStable_Cnt=0;
        return TRUE;
    }
    if(LVDS_UnStable_Cnt>0)
    {
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("Reset LVDS\n"));
#endif
       // InitIT626X();
        InitLVDS();
        LVDS_UnStable_Cnt=0;
    }
    return FALSE;
}
//===================================================================
#ifdef SUPPORT_REGEN_TIMING
struct CRT_TimingSetting {
	BYTE fmt;
    BYTE interlaced;
	UINT  VEC[11];
};
//   VDEE_L,   VDEE_H, VRS2S_L, VRS2S_H, VRS2E_L, VRS2E_H, HalfL_L, HalfL_H, VDE2S_L, VDE2S_H, HVP&Progress
_CODE struct CRT_TimingSetting TimingTable[] =
{
// {    FMT             ,  Int,      HT   ,	 H_DEW,    H_FBH,  H_SyncW,   H_BBH,   VTotal,   V_DEW,  V_FBH,  V_SyncW,  V_BBH,   Sync_pol,   }},
	{ HDMI_480p60_16x9  ,    0,    { 858 ,    720,       16,      62,       60,       525,     480,      9,        6,     30,    0 } },
	{ HDMI_480p60_16x9  ,    0,    { 858 ,    720,       16,      62,       60,       525,     480,      9,        6,     30,    0 } },
	{ HDMI_720p60       ,    0,    {1650 ,   1280,      110,      40,      220,       750,     720,      5,        5,     20,    3 } },
	{ HDMI_1080i60      ,    1,    {2200 ,   1920,       88,      44,      148,      1125,     540,      2,        5,     15,    3 } },
	{ HDMI_480i60_16x9  ,    1,    { 858 ,    720,       19,      62,       57,       525,     240,      4,        3,     15,    0 } },
	{ HDMI_1080p60      ,    0,    {2200 ,   1920,       88,      44,      148,      1125,    1080,      4,        5,     36,    3 } },
	{ HDMI_1080p50      ,    0,    {2640 ,   1920,       528,     44,      148,      1125,    1080,      4,        5,     36,    3 } },
};
#define MaxIndex (sizeof(TimingTable)/sizeof(struct CRT_TimingSetting))
BOOL ReGenTiming(BYTE fmt_assign)
{
    UINT HRS, HRE ;
    UINT HDES, HDEE;
    UINT VRS, VRE;
    UINT VDES, VDEE   ;
    UINT VRS2ndRise, VRS2nd, VRE2nd;
    UINT VDES2nd, VDEE2nd;
    UINT HTotal, VTotal;
    UINT i;
    UINT H_Total, H_FBH, H_SyncW, H_BBH, H_DEW;
    UINT V_Total,  V_FBH,  V_SyncW,  V_BBH, V_DEW;
    UINT SyncPol;
    BYTE fmt_index;
    BYTE interlaced;
    BYTE reg90, reg91, reg92, reg93, reg94, reg95, reg96, reg97, reg98;
    BYTE reg99, reg9A, reg9B, reg9C, reg9D, reg9E, reg9F, regA0, regA1, regA2, regA3;
    BYTE regA4, regA5;
    fmt_index=0;
    for(i=0;i< MaxIndex;i ++)
    {
        if(TimingTable[i].fmt==fmt_assign) fmt_index=i;
    }
    if(i==MaxIndex && fmt_index==0)
    {
        EnableDeOnly(FALSE);
        return FALSE;
    }
    interlaced= TimingTable[fmt_index].interlaced;

    H_Total=TimingTable[fmt_index].VEC[0];
    H_DEW  =TimingTable[fmt_index].VEC[1];
    H_FBH  =TimingTable[fmt_index].VEC[2];
    H_SyncW=TimingTable[fmt_index].VEC[3];
    H_BBH  =TimingTable[fmt_index].VEC[4];
    V_Total=TimingTable[fmt_index].VEC[5];
    V_DEW  =TimingTable[fmt_index].VEC[6];
    V_FBH  =TimingTable[fmt_index].VEC[7];
    V_SyncW=TimingTable[fmt_index].VEC[8];
    V_BBH  =TimingTable[fmt_index].VEC[9];
    SyncPol=TimingTable[fmt_index].VEC[10];

	HTotal= H_Total-1;
	VTotal= V_Total-1;
	HRS=H_FBH-1;
	HRE=HRS+H_SyncW;
	HDES= HRE+H_BBH;
	HDEE= HDES+H_DEW;

	VRS= V_FBH;
	VRE= VRS+V_SyncW;
	VDES= VRE+V_BBH;
	VDEE= (VDES+V_DEW)%V_Total;
	VRS2nd = VDEE+ V_FBH;
	VRE2nd = VRS2nd+ V_SyncW;
	VDES2nd= VRS2nd + V_BBH;
	VDEE2nd= (VDES2nd+V_DEW)%V_Total;
	VRS2ndRise =  H_Total/2 + HRS;
	VDES= VRE+V_BBH-1;
	VDEE= (VDES+V_DEW)%V_Total;
    VDES2nd= VRS2nd + V_SyncW + V_BBH ;
	VDEE2nd= (VDES2nd+V_DEW)%V_Total;

	reg90 = ((HTotal&0xF)<<4) + (SyncPol<<1) ;
	reg91 =   ((HTotal& 0xFF0)>>4);
	reg92 = HDES& 0xFF;
	reg93 = HDEE& 0xFF;
	reg94 = ((HDEE& 0xF00)>>4) + ((HDES&0xF00)>>8);

	reg95 = HRS& 0xFF;
	reg96 = HRE& 0xFF;
	reg97 = ((HRE& 0xF00)>>4) + ((HRS&0xF00)>>8);
    reg98 = VTotal & 0xFF;
	reg99 = (VTotal& 0xF00)>>8;
	reg9A = VDES& 0xFF;
	reg9B = VDEE& 0xFF;
	reg9C = ((VDEE& 0xF00)>>4) + ((VDES&0xF00)>>8);
	reg9D = VDES2nd& 0xFF;
	reg9E = VDEE2nd& 0xFF;
	reg9F = ((VDEE2nd& 0xF00)>>4) + ((VDES2nd&0xF00)>>8);
	regA0 = VRS& 0xFF;
	regA1 = ((VRE &0xF)<<4) + ((VRS&0xF00)>>8);

	regA2 = VRS2nd& 0xFF;
	if(interlaced==1) regA3 = ( (VRE2nd& 0xF)<<4 ) + ( (VRS2nd& 0xF00)>>8);
	else regA3=0xFF;

	regA4 = VRS2ndRise& 0xFF;
	regA5 = 0x20 + ((VRS2ndRise& 0xF00)>>8);
    EnableHVToolDetect(FALSE);

    Switch_HDMITX_Bank(0);
    HDMITX_WriteI2C_Byte(0x90,reg90);
    HDMITX_WriteI2C_Byte(0x91,reg91);
    HDMITX_WriteI2C_Byte(0x92,reg92);
    HDMITX_WriteI2C_Byte(0x93,reg93);
    HDMITX_WriteI2C_Byte(0x94,reg94);
    HDMITX_WriteI2C_Byte(0x95,reg95);
    HDMITX_WriteI2C_Byte(0x96,reg96);
    HDMITX_WriteI2C_Byte(0x97,reg97);
    HDMITX_WriteI2C_Byte(0x98,reg98);
    HDMITX_WriteI2C_Byte(0x99,reg99);
    HDMITX_WriteI2C_Byte(0x9a,reg9A);
    HDMITX_WriteI2C_Byte(0x9b,reg9B);
    HDMITX_WriteI2C_Byte(0x9c,reg9C);
    HDMITX_WriteI2C_Byte(0x9d,reg9D);
    HDMITX_WriteI2C_Byte(0x9e,reg9E);
    HDMITX_WriteI2C_Byte(0x9f,reg9F);
    HDMITX_WriteI2C_Byte(0xa0,regA0);
    HDMITX_WriteI2C_Byte(0xa1,regA1);
    HDMITX_WriteI2C_Byte(0xa2,regA2);
    HDMITX_WriteI2C_Byte(0xa3,regA3);
    HDMITX_WriteI2C_Byte(0xa4,regA4);
    //EnableDeOnly(TRUE);
    HDMITX_WriteI2C_Byte(0xa5,regA5);
    HDMITX_AndREG_Byte(0xa8,~(1<<3));
    return TRUE;
}
#endif
//===================================================================
void
HDMITX_InitInstance(INSTANCE *pInstance)
{
	if(pInstance && 0 < HDMITX_INSTANCE_MAX)
	{
		Instance[0] = *pInstance ;
	}
    Instance[0].bAudFs=AUDFS_OTHER;
}
/*
static void C6613_Check_EMEM_sts()
{

    Switch_HDMITX_Bank(0);
    HDMITX_WriteI2C_Byte(0xF8,0xC3);//unlock register
    HDMITX_WriteI2C_Byte(0xF8,0xA5);//unlock register

    HDMITX_WriteI2C_Byte(0x22,0x00);


    HDMITX_WriteI2C_Byte(0x10,0x03);//

    HDMITX_WriteI2C_Byte(0x11,0xA0);
    HDMITX_WriteI2C_Byte(0x12,0x00);//
    HDMITX_WriteI2C_Byte(0x13,0x08);
    HDMITX_WriteI2C_Byte(0x14,0x00);//
    HDMITX_WriteI2C_Byte(0x15,0x00);//
    delay1ms(200);
    if((0x80 & HDMITX_ReadI2C_Byte(0x1c))){    //if 0x1c[7]==1 EXT_ROM
          Instance[0].TxEMEMStatus=FALSE;
#ifdef _PRINT_HDMI_TX_
          //HDMITX_DEBUG_PRINT(("==Dev %X is ExtROM==\n",I2CDEV));
#endif
    }
    else
    {                                    //if 0x1c[1] !=1 EMEM
          Instance[0].TxEMEMStatus=TRUE;
#ifdef _PRINT_HDMI_TX_
          //HDMITX_DEBUG_PRINT(("==Dev %X is EMEM==\n",I2CDEV));
#endif
    }
    HDMITX_WriteI2C_Byte(0x04,0x3F);     //reset all reg
    HDMITX_WriteI2C_Byte(0xF8,0xFF);//lock register
}
*/
// static void InitIT626X_HDCPROM()
// {
//
//     Switch_HDMITX_Bank(0);
//     HDMITX_WriteI2C_Byte(0xF8,0xC3);//unlock register
//     HDMITX_WriteI2C_Byte(0xF8,0xA5);//unlock register
//     if(Instance[0].TxEMEMStatus==TRUE){
//             // with internal eMem
//             HDMITX_WriteI2C_Byte(REG_TX_ROM_HEADER,0xE0);
//             HDMITX_WriteI2C_Byte(REG_TX_LISTCTRL,0x48);
//     }else{
//         // with external ROM
//         HDMITX_WriteI2C_Byte(REG_TX_ROM_HEADER,0xA0);
//         HDMITX_WriteI2C_Byte(REG_TX_LISTCTRL,0x00);
//     }
//     HDMITX_WriteI2C_Byte(0xF8,0xFF);//lock register
// }
void InitIT626X()
{
    BYTE intclr;
    Switch_HDMITX_Bank(0);
    HDMITX_WriteI2C_Byte(REG_TX_INT_CTRL,Instance[0].bIntType);
    Instance[0].bIntPOL = (Instance[0].bIntType&B_INTPOL_ACTH)?TRUE:FALSE ;
    Instance[0].TxEMEMStatus=TRUE;//C6613_Check_EMEM_sts();

    HDMITX_WriteI2C_Byte(REG_TX_SW_RST,B_REF_RST|B_VID_RST|B_AUD_RST|B_AREF_RST|B_HDCP_RST);
    delay1ms(1);
    HDMITX_WriteI2C_Byte(REG_TX_SW_RST,B_VID_RST|B_AUD_RST|B_AREF_RST|B_HDCP_RST);
    InitLVDS();
    // Avoid power loading in un play status.
    HDMITX_WriteI2C_Byte(REG_TX_AFE_DRV_CTRL,B_AFE_DRV_RST|B_AFE_DRV_PWD);
	//////////////////////////////////////////////////////////////////
	// Setup HDCP ROM
	//////////////////////////////////////////////////////////////////

	//InitIT626X_HDCPROM();
    HDMITX_AndREG_Byte(0xF3,~0x30);
    HDMITX_OrREG_Byte(0xF3,0x20);
    // set interrupt mask,mask value 0 is interrupt available.
    //HDMITX_WriteI2C_Byte(REG_TX_INT_MASK1,0xB2);
	HDMITX_WriteI2C_Byte(REG_TX_INT_MASK1,0x30);
    HDMITX_WriteI2C_Byte(REG_TX_INT_MASK2,0xF8);
    HDMITX_WriteI2C_Byte(REG_TX_INT_MASK3,0x37);
    Switch_HDMITX_Bank(0);
    DISABLE_NULL_PKT();
    DISABLE_ACP_PKT();
    DISABLE_ISRC1_PKT();
    DISABLE_ISRC2_PKT();
    DISABLE_AVI_INFOFRM_PKT();
    DISABLE_AUD_INFOFRM_PKT();
    DISABLE_SPD_INFOFRM_PKT();
    DISABLE_MPG_INFOFRM_PKT();
    Switch_HDMITX_Bank(1);
    HDMITX_AndREG_Byte(REG_TX_AVIINFO_DB1,~(3<<5));
    Switch_HDMITX_Bank(0);
	//////////////////////////////////////////////////////////////////
	// Setup Output Audio format.
	//////////////////////////////////////////////////////////////////
    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL1,Instance[0].bOutputAudioMode); // regE1 bOutputAudioMode should be loaded from ROM image.


	HDMITX_WriteI2C_Byte(REG_TX_INT_CLR0,0xFF);
    HDMITX_WriteI2C_Byte(REG_TX_INT_CLR1,0xFF);
    intclr = (HDMITX_ReadI2C_Byte(REG_TX_SYS_STATUS))|B_CLR_AUD_CTS | B_INTACTDONE ;
    HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,intclr); // clear interrupt.
    intclr &= ~(B_INTACTDONE);
    HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,intclr); // INTACTDONE reset to zero.
// #ifdef EXTERN_HDCPROM
// #pragma message("EXTERN ROM CODED");
// 	HDMITX_WriteI2C_Byte(REG_TX_ROM_HEADER,0xA0);
// #endif
    {
        ULONG n=6144L;
        Switch_HDMITX_Bank(1);
        HDMITX_WriteI2C_Byte(REGPktAudN0,(BYTE)((n)&0xFF));
        HDMITX_WriteI2C_Byte(REGPktAudN1,(BYTE)((n>>8)&0xFF));
        HDMITX_WriteI2C_Byte(REGPktAudN2,(BYTE)((n>>16)&0xF));
        Switch_HDMITX_Bank(0);
        HDMITX_WriteI2C_Byte(0xc4,0xfe);
        HDMITX_OrREG_Byte(REG_TX_PKT_SINGLE_CTRL,(1<<4)|(1<<5));
    }
}

//////////////////////////////////////////////////////////////////////
// export this for dynamic change input signal
//////////////////////////////////////////////////////////////////////
BOOL SetupVideoInputSignal(BYTE inputSignalType)
{
	Instance[0].bInputVideoSignalType = inputSignalType ;
    // SetInputMode(inputColorMode,Instance[0].bInputVideoSignalType);
    return TRUE ;
}

BOOL EnableVideoOutput(/*VIDEOPCLKLEVEL*/BYTE level,BYTE inputColorMode,BYTE outputColorMode,BYTE bHDMI)
{
    // bInputVideoMode,bOutputVideoMode,Instance[0].bInputVideoSignalType,bAudioInputType,should be configured by upper F/W or loaded from EEPROM.
    // should be configured by initsys.c
    // VIDEOPCLKLEVEL level ;

    Instance[0].bHDMIMode = (BYTE)bHDMI ;

    if(Instance[0].bHDMIMode)
    {
        SetAVMute(TRUE);
    }

    SetInputMode(inputColorMode,Instance[0].bInputVideoSignalType);

    SetCSCScale(inputColorMode,outputColorMode);

    if(Instance[0].bHDMIMode)
    {
        HDMITX_WriteI2C_Byte(REG_TX_HDMI_MODE,B_TX_HDMI_MODE);
    }
    else
    {
        HDMITX_WriteI2C_Byte(REG_TX_HDMI_MODE,B_TX_DVI_MODE);
    }

#ifdef INVERT_VID_LATCHEDGE
    uc = HDMITX_ReadI2C_Byte(REG_TX_CLK_CTRL1);
    uc |= B_VDO_LATCH_EDGE ;
    HDMITX_WriteI2C_Byte(REG_TX_CLK_CTRL1, uc);
#endif


    SetupAFE(level); // pass if High Freq request
    FireAFE();

    HDMITX_WriteI2C_Byte(REG_TX_INT_CLR0,0);
    HDMITX_WriteI2C_Byte(REG_TX_INT_CLR1,B_CLR_VIDSTABLE);
    HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,B_INTACTDONE);

	return TRUE ;
}

BOOL GetEDIDData(int EDIDBlockID,BYTE *pEDIDData)
{
	if(!pEDIDData)
	{
		return FALSE ;
	}

    if(ReadEDID(pEDIDData,EDIDBlockID/2,(EDIDBlockID%2)*128,128) == ER_FAIL)
    {
        return FALSE ;
    }

    return TRUE ;
}

#ifdef SUPPORT_HDCP
BOOL
EnableHDCP(BYTE bEnable)
{
    if(bEnable)
    {
        if(ER_FAIL == HDCP_Authenticate())
        {
            HDCP_ResetAuth();
			return FALSE ;
        }
    }
    else
    {
        HDCP_CancelRepeaterAuthenticate();
        HDCP_ResetAuth();
        Instance[0].bAuthenticated=FALSE;
    }
    return TRUE ;
}
#endif

BYTE CheckHDMITX(BYTE *pHPD,BYTE *pHPDChange)
{
	BYTE intdata1,intdata2,intdata3,sysstat;
	BYTE  intclr3 = 0 ;
	BYTE PrevHPD = Instance[0].bHPD ;
	BYTE HPD ;

	sysstat = HDMITX_ReadI2C_Byte(REG_TX_SYS_STATUS);
#ifdef Debug_message
	//HDMITX_DEBUG_PRINTF(("REG_TX_SYS_STATUS = %X \n",(WORD)sysstat));

#endif

	//if((sysstat&(1<<4)))
	//    SetupAudioChannel();

	if((sysstat & (B_HPDETECT/*|B_RXSENDETECT*/)) == (B_HPDETECT/*|B_RXSENDETECT*/))
	{
		HPD = TRUE;
	}
	else
	{
		HPD = FALSE;
	}
	// 2007/06/20 added by jj_tseng@chipadvanced.com

	if(pHPDChange)
	{
		*pHPDChange = FALSE ;
	}
	//~jj_tseng@chipadvanced.com 2007/06/20

	if(HPD==FALSE)
	{
		Instance[0].bAuthenticated = FALSE ;
	}

	if(sysstat & B_INT_ACTIVE)
	{

		intdata1 = HDMITX_ReadI2C_Byte(REG_TX_INT_STAT1);
		if(intdata1 & B_INT_AUD_OVERFLOW)
		{
#ifdef Debug_message
			HDMITX_DEBUG_PRINTF(("B_INT_AUD_OVERFLOW.\n"));
#endif
			HDMITX_OrREG_Byte(0xc5,(1<<4));
			HDMITX_OrREG_Byte(REG_TX_SW_RST,(B_AUD_RST|B_AREF_RST));
			HDMITX_AndREG_Byte(REG_TX_SW_RST,~(B_AUD_RST|B_AREF_RST));
			AudioDelayCnt=AudioOutDelayCnt;
			LastRefaudfreqnum=0;
		}
#ifdef Debug_message
		HDMITX_DEBUG_PRINTF(("INT_Handler: reg%X = %X\n",(WORD)REG_TX_INT_STAT1,(WORD)intdata1));
#endif
		if(intdata1 & B_INT_DDCFIFO_ERR)
		{
#ifdef Debug_message
			HDMITX_DEBUG_PRINTF(("DDC FIFO Error.\n"));
#endif
			ClearDDCFIFO();
		}


		if(intdata1 & B_INT_DDC_BUS_HANG)
		{
#ifdef Debug_message
			HDMITX_DEBUG_PRINTF(("DDC BUS HANG.\n"));
#endif
			AbortDDC();
#ifdef SUPPORT_HDCP
			if(Instance[0].bAuthenticated)
			{
#ifdef Debug_message
				HDMITX_DEBUG_PRINTF(("when DDC hang,and aborted DDC,the HDCP authentication need to restart.\n"));
#endif
				HDCP_ResumeAuthentication();
			}
#endif
		}


		if(intdata1 & (B_INT_HPD_PLUG|B_INT_RX_SENSE))
		{
			if(pHPDChange && (Instance[0].bHPD != HPD))
			{
				*pHPDChange = TRUE;
#ifdef Debug_message
				HDMITX_DEBUG_PRINTF(("2-*pHPDChange = %X\n",(WORD)*pHPDChange));
#endif
			}
			if(HPD == FALSE)
			{
#ifdef Debug_message
				HDMITX_DEBUG_PRINTF(("HPD = %X\n",(WORD)HPD));
#endif
				HDMITX_WriteI2C_Byte(REG_TX_SW_RST,B_AREF_RST|B_VID_RST|B_AUD_RST|B_HDCP_RST);
				delay1ms(1);
				HDMITX_WriteI2C_Byte(REG_TX_AFE_DRV_CTRL,B_AFE_DRV_RST|B_AFE_DRV_PWD);
				//HDMITX_DEBUG_PRINTF(("Unplug,%x %x\n",HDMITX_ReadI2C_Byte(REG_TX_SW_RST),HDMITX_ReadI2C_Byte(REG_TX_AFE_DRV_CTRL)));
				// VState = TXVSTATE_Unplug ;
				LastRefaudfreqnum=0;
			}
		}


		intdata2 = HDMITX_ReadI2C_Byte(REG_TX_INT_STAT2);
#ifdef Debug_message
		HDMITX_DEBUG_PRINTF(("INT_Handler: reg%X = %X\n",(WORD)REG_TX_INT_STAT2,(WORD)intdata2));
		
#endif



#ifdef SUPPORT_HDCP
		if(intdata2 & B_INT_AUTH_DONE)
		{
#ifdef Debug_message
			HDMITX_DEBUG_PRINTF(("interrupt Authenticate Done.\n"));
#endif
			HDMITX_OrREG_Byte(REG_TX_INT_MASK2,(BYTE)B_T_AUTH_DONE_MASK);
			Instance[0].bAuthenticated = TRUE ;
			SetAVMute(FALSE);
		}

		if(intdata2 & B_INT_AUTH_FAIL)
		{
#ifdef Debug_message
			HDMITX_DEBUG_PRINTF(("reg46 = %02x reg16 = %02x reg1A = %02x\n",(WORD)HDMITX_ReadI2C_Byte(0x46),(WORD)HDMITX_ReadI2C_Byte(0x16),(WORD)HDMITX_ReadI2C_Byte(0x1A)));
			HDMITX_DEBUG_PRINTF(("interrupt Authenticate Fail.\n"));
#endif
			SetAVMute(TRUE);
			AbortDDC();   // @emily add
			HDCP_ResumeAuthentication();
		}
#endif // SUPPORT_HDCP

		intdata3 = HDMITX_ReadI2C_Byte(REG_TX_INT_STAT3);
		if(intdata3 & B_INT_VIDSTABLE)
		{
			sysstat = HDMITX_ReadI2C_Byte(REG_TX_SYS_STATUS);
			if(Instance[0].bVideoOut==FALSE ||
			sysstat & B_TXVIDSTABLE )
			{
				//CurrCTS=0;
				//LastRefaudfreqnum=0;
				bChangeMode=TRUE;
				//#ifdef Debug_message
				//                HDMITX_DEBUG_PRINTF(("B_INT_VIDSTABLE Instance[0].bVideoOut==FALSE\n"));
				//#endif
			}
			else
			{
				//DisableAudioOutput();
				Instance[0].bVideoOut=FALSE;
				//#ifdef Debug_message
				//                HDMITX_DEBUG_PRINTF(("B_INT_VIDSTABLE Instance[0].bVideoOut==TRUE\n"));
				//#endif
			}
		}
		HDMITX_WriteI2C_Byte(REG_TX_INT_CLR0,0xFF);
		HDMITX_WriteI2C_Byte(REG_TX_INT_CLR1,0xFF);
		intclr3 = (HDMITX_ReadI2C_Byte(REG_TX_SYS_STATUS))|B_CLR_AUD_CTS | B_INTACTDONE ;
		HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,intclr3); // clear interrupt.
		intclr3 &= ~(B_INTACTDONE);
		HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,intclr3); // INTACTDONE reset to zero.
	}
	else
	{
		if(pHPDChange)
		{
			if(HPD != PrevHPD)
			{
			*pHPDChange = TRUE;
			}
			else
			{
			*pHPDChange = FALSE;
			}
		}
	}
	if(pHPDChange)
	{
		if((*pHPDChange==TRUE) &&(HPD==FALSE))
		{
			HDMITX_WriteI2C_Byte(REG_TX_AFE_DRV_CTRL,B_AFE_DRV_RST|B_AFE_DRV_PWD);
		}
	}

	if(pHPD)
	{
		*pHPD = HPD    ;
	}

	Instance[0].bHPD = HPD ;
	return HPD ;
}




void
DisableIT626X()
{
    HDMITX_WriteI2C_Byte(REG_TX_SW_RST,B_AREF_RST|B_VID_RST|B_AUD_RST|B_HDCP_RST);
    delay1ms(1);
    HDMITX_WriteI2C_Byte(REG_TX_AFE_DRV_CTRL,B_AFE_DRV_RST|B_AFE_DRV_PWD);
}

void
DisableVideoOutput()
{
    BYTE uc = HDMITX_ReadI2C_Byte(REG_TX_SW_RST) | B_VID_RST ;
    HDMITX_WriteI2C_Byte(REG_TX_SW_RST,uc);
    HDMITX_WriteI2C_Byte(REG_TX_AFE_DRV_CTRL,B_AFE_DRV_RST|B_AFE_DRV_PWD);
    Instance[0].bVideoOut=FALSE;
}


void
DisableAudioOutput()
{
    //UINT msec,count;
    HDMITX_OrREG_Byte(REG_TX_PKT_SINGLE_CTRL,(1<<5));
    SetAVMute(TRUE);
    /*
    for(msec=0;msec<32;msec++)
    {
        for(count=0;count<2048;count++)
        {
            _nop_();_nop_();_nop_();_nop_();
            _nop_();_nop_();_nop_();_nop_();
        }
    }
    */
    HDMITX_OrREG_Byte(REG_TX_SW_RST,B_AUD_RST|B_AREF_RST);
    //HDMITX_AndREG_Byte(0x59,~(1<<2)); //for test
    AudioDelayCnt=AudioOutDelayCnt;
    LastRefaudfreqnum=0;
}

static SYS_STATUS
SetVSIInfoFrame(VendorSpecific_InfoFrame *pVSIInfoFrame)
{
    int i ;
    BYTE ucData=0 ;

    if(!pVSIInfoFrame)
    {
        return ER_FAIL ;
    }
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("SetVSIInfo(): "));
    HDMITX_DEBUG_PRINTF(("\n"));
#endif
    Switch_HDMITX_Bank(1);
    // for(i=0x38;i<0x57;i++)
    // {
    //     HDMITX_WriteI2C_Byte(i,0);
    // }
    if(pVSIInfoFrame->pktbyte.VS_HB[2]>27) // HB[2] = Checksum ;
    {
        pVSIInfoFrame->pktbyte.VS_HB[2] = 27 ;
    }

    HDMITX_WriteI2C_Byte(REG_TX_PKT_HB00,pVSIInfoFrame->pktbyte.VS_HB[0]); ucData -= pVSIInfoFrame->pktbyte.VS_HB[0] ;
    HDMITX_WriteI2C_Byte(REG_TX_PKT_HB01,pVSIInfoFrame->pktbyte.VS_HB[1]); ucData -= pVSIInfoFrame->pktbyte.VS_HB[1] ;
    HDMITX_WriteI2C_Byte(REG_TX_PKT_HB02,pVSIInfoFrame->pktbyte.VS_HB[2]); ucData -= pVSIInfoFrame->pktbyte.VS_HB[2] ;

    for( i = 0 ; i < (int)(pVSIInfoFrame->pktbyte.VS_HB[2]) ; i++ )
    {
        HDMITX_WriteI2C_Byte(REG_TX_PKT_PB01+i,pVSIInfoFrame->pktbyte.VS_DB[1+i]) ;
        ucData -= pVSIInfoFrame->pktbyte.VS_DB[1+i] ;
    }

    for( ; i < 27 ;i++ )
    {
        HDMITX_WriteI2C_Byte(REG_TX_PKT_PB01+i,0 ) ;
    }


    // HDMITX_WriteI2C_Byte(REG_TX_PKT_PB01,pVSIInfoFrame->pktbyte.VS_DB[1]);
    // HDMITX_WriteI2C_Byte(REG_TX_PKT_PB02,pVSIInfoFrame->pktbyte.VS_DB[2]);
    // HDMITX_WriteI2C_Byte(REG_TX_PKT_PB03,pVSIInfoFrame->pktbyte.VS_DB[3]);
    // HDMITX_WriteI2C_Byte(REG_TX_PKT_PB04,pVSIInfoFrame->pktbyte.VS_DB[4]);
    // HDMITX_WriteI2C_Byte(REG_TX_PKT_PB05,pVSIInfoFrame->pktbyte.VS_DB[5]);
    // HDMITX_WriteI2C_Byte(REG_TX_PKT_PB06,pVSIInfoFrame->pktbyte.VS_DB[6]);

    // for(i = 1; i < VENDORSPEC_INFOFRAME_LEN+1 ; i++)
    // {
    //     ucData -= pVSIInfoFrame->pktbyte.VS_DB[i] ;
    // }
    //
    // ucData -= 0x80+VENDORSPEC_INFOFRAME_VER+VENDORSPEC_INFOFRAME_TYPE+VENDORSPEC_INFOFRAME_LEN ;
    ucData &= 0xFF ;
    pVSIInfoFrame->pktbyte.VS_DB[0]=ucData;

    HDMITX_WriteI2C_Byte(REG_TX_PKT_PB00,pVSIInfoFrame->pktbyte.VS_DB[0]);
    Switch_HDMITX_Bank(0);
    ENABLE_NULL_PKT();
    return ER_SUCCESS ;
}

BOOL EnableVSInfoFrame(BYTE bEnable,BYTE *pVSInfoFrame)
{
    if(!bEnable)
    {
        DISABLE_NULL_PKT();
        return TRUE ;
    }

    if(SetVSIInfoFrame((VendorSpecific_InfoFrame *)pVSInfoFrame) == ER_SUCCESS)
    {
        return TRUE ;
    }

    return FALSE ;
}

BOOL
EnableAVIInfoFrame(BYTE bEnable,BYTE *pAVIInfoFrame)
{
    if(!bEnable)
    {
        DISABLE_AVI_INFOFRM_PKT();
        Switch_HDMITX_Bank(1);
        HDMITX_AndREG_Byte(REG_TX_AVIINFO_DB1,~(3<<5));
        Switch_HDMITX_Bank(0);
        return TRUE ;
    }

    if(SetAVIInfoFrame((AVI_InfoFrame *)pAVIInfoFrame) == ER_SUCCESS)
    {
        return TRUE ;
    }

    return FALSE ;
}

BOOL
EnableAudioInfoFrame(BYTE bEnable,BYTE *pAudioInfoFrame)
{
    if(!bEnable)
    {
        DISABLE_AVI_INFOFRM_PKT();
        return TRUE ;
    }


    if(SetAudioInfoFrame((Audio_InfoFrame *)pAudioInfoFrame) == ER_SUCCESS)
    {
        return TRUE ;
    }

    return FALSE ;
}

void
SetAVMute(BYTE bEnable)
{
    BYTE uc ;
    Switch_HDMITX_Bank(0);
    uc = HDMITX_ReadI2C_Byte(REG_TX_GCP);
    uc &= ~B_TX_SETAVMUTE ;
    uc |= bEnable?B_TX_SETAVMUTE:0 ;
    HDMITX_WriteI2C_Byte(REG_TX_GCP,uc);
    HDMITX_WriteI2C_Byte(REG_TX_PKT_GENERAL_CTRL,B_ENABLE_PKT|B_REPEAT_PKT);
    if(bEnable)
    {
        //printf("==========Set AV Mute============\n");
    }
    else
    {
        //printf("==========Clear AV Mute============\n");
    }
}
BOOL GetAVMute()
{
    if(B_SET_AVMUTE&HDMITX_ReadI2C_Byte(REG_TX_GCP))
    {    return TRUE;}
    else
    {    return FALSE;}
}
void
SetOutputColorDepthPhase(BYTE ColorDepth,BYTE bPhase)
{
    BYTE uc ;
    BYTE bColorDepth ;
    bPhase=(~bPhase);
    if(ColorDepth == 30)
    {
        bColorDepth = B_CD_30 ;
    }
    else if (ColorDepth == 36)
    {
        bColorDepth = B_CD_36 ;
    }/*
    else if (ColorDepth == 24)
    {
        bColorDepth = B_CD_24 ;
    }*/
    else
    {
        bColorDepth = 0 ; // not indicated
    }

    Switch_HDMITX_Bank(0);
    uc = HDMITX_ReadI2C_Byte(REG_TX_GCP);
    uc &= ~B_COLOR_DEPTH_MASK ;
    uc |= bColorDepth&B_COLOR_DEPTH_MASK;
    HDMITX_WriteI2C_Byte(REG_TX_GCP,uc);
}

#if 0
void
Get6613Reg(BYTE *pReg)
{
    int i ;
    BYTE reg ;
    Switch_HDMITX_Bank(0);
    for(i = 0 ; i < 0x100 ; i++)
    {
        reg = i & 0xFF ;
        pReg[i] = HDMITX_ReadI2C_Byte(reg);
    }
    Switch_HDMITX_Bank(1);
    for(reg = 0x30 ; reg < 0xB0 ; i++,reg++)
    {
        pReg[i] = HDMITX_ReadI2C_Byte(reg);
    }
    Switch_HDMITX_Bank(0);

}
#endif
//////////////////////////////////////////////////////////////////////
// SubProcedure process                                                       //
//////////////////////////////////////////////////////////////////////


//~jj_tseng@chipadvanced.com 2007/01/02


//////////////////////////////////////////////////////////////////////
// Function: SetInputMode
// Parameter: InputMode,bInputSignalType
//      InputMode - use [1:0] to identify the color space for reg70[7:6],
//                  definition:
//                     #define F_MODE_RGB444  0
//                     #define F_MODE_YUV422 1
//                     #define F_MODE_YUV444 2
//                     #define F_MODE_CLRMOD_MASK 3
//      bInputSignalType - defined the CCIR656 D[0],SYNC Embedded D[1],and
//                     DDR input in D[2].
// Return: N/A
// Remark: program Reg70 with the input value.
// Side-Effect: Reg70.
//////////////////////////////////////////////////////////////////////

static void
SetInputMode(BYTE InputMode,BYTE bInputSignalType)
{
    BYTE ucData ;
#ifdef Debug_message
//    HDMITX_DEBUG_PRINTF(("SetInputMode(%02X,%02X)\n",InputMode,bInputSignalType));
#endif
    ucData = HDMITX_ReadI2C_Byte(REG_TX_INPUT_MODE);

    ucData &= ~(M_INCOLMOD|B_2X656CLK|B_SYNCEMB|B_INDDR|B_PCLKDIV2);

    switch(InputMode & F_MODE_CLRMOD_MASK)
    {
    case F_MODE_YUV422:
        ucData |= B_IN_YUV422 ;
        break ;
    case F_MODE_YUV444:
        ucData |= B_IN_YUV444 ;
        break ;
    case F_MODE_RGB444:
    default:
        ucData |= B_IN_RGB ;
        break ;
    }

    if(bInputSignalType & T_MODE_PCLKDIV2)
    {
        ucData |= B_PCLKDIV2 ;
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("PCLK Divided by 2 mode\n"));
#endif
    }
    if(bInputSignalType & T_MODE_CCIR656)
    {
        ucData |= B_2X656CLK ;
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("CCIR656 mode\n"));
#endif
    }

    if(bInputSignalType & T_MODE_SYNCEMB)
    {
        ucData |= B_SYNCEMB ;
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("Sync Embedded mode\n"));
#endif
    }

    if(bInputSignalType & T_MODE_INDDR)
    {
        ucData |= B_INDDR ;
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("Input DDR mode\n"));
#endif
    }

    HDMITX_WriteI2C_Byte(REG_TX_INPUT_MODE,ucData);
}

//////////////////////////////////////////////////////////////////////
// Function: SetCSCScale
// Parameter: bInputMode -
//             D[1:0] - Color Mode
//             D[4] - Colorimetry 0: ITU_BT601 1: ITU_BT709
//             D[5] - Quantization 0: 0_255 1: 16_235
//             D[6] - Up/Dn Filter 'Required'
//                    0: no up/down filter
//                    1: enable up/down filter when csc need.
//             D[7] - Dither Filter 'Required'
//                    0: no dither enabled.
//                    1: enable dither and dither free go "when required".
//            bOutputMode -
//             D[1:0] - Color mode.
// Return: N/A
// Remark: reg72~reg8D will be programmed depended the input with table.
// Side-Effect:
//////////////////////////////////////////////////////////////////////

static void
SetCSCScale(BYTE bInputMode,BYTE bOutputMode)
{
    BYTE ucData,csc ;
    BYTE filter = 0 ; // filter is for Video CTRL DN_FREE_GO,EN_DITHER,and ENUDFILT


    // (1) YUV422 in,RGB/YUV444 output (Output is 8-bit,input is 12-bit)
    // (2) YUV444/422  in,RGB output (CSC enable,and output is not YUV422)
    // (3) RGB in,YUV444 output   (CSC enable,and output is not YUV422)
    //
    // YUV444/RGB24 <-> YUV422 need set up/down filter.

    switch(bInputMode&F_MODE_CLRMOD_MASK)
    {
    #ifdef SUPPORT_INPUTYUV444
    case F_MODE_YUV444:
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("Input mode is YUV444 "));
#endif
        switch(bOutputMode&F_MODE_CLRMOD_MASK)
        {
        case F_MODE_YUV444:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("Output mode is YUV444\n"));
#endif
            csc = B_CSC_BYPASS ;
            break ;

        case F_MODE_YUV422:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("Output mode is YUV422\n"));
#endif
            if(bInputMode & F_MODE_EN_UDFILT) // YUV444 to YUV422 need up/down filter for processing.
            {
                filter |= B_TX_EN_UDFILTER ;
            }
            csc = B_CSC_BYPASS ;
            break ;
        case F_MODE_RGB444:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("Output mode is RGB24\n"));
#endif
            csc = B_CSC_YUV2RGB ;
            if(bInputMode & F_MODE_EN_DITHER) // YUV444 to RGB24 need dither
            {
                filter |= B_TX_EN_DITHER | B_TX_DNFREE_GO ;
            }

            break ;
        }
        break ;
    #endif

    #ifdef SUPPORT_INPUTYUV422
    case F_MODE_YUV422:
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("Input mode is YUV422\n"));
#endif
        switch(bOutputMode&F_MODE_CLRMOD_MASK)
        {
        case F_MODE_YUV444:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("Output mode is YUV444\n"));
#endif
            csc = B_CSC_BYPASS ;
            if(bInputMode & F_MODE_EN_UDFILT) // YUV422 to YUV444 need up filter
            {
                filter |= B_TX_EN_UDFILTER ;
            }

            if(bInputMode & F_MODE_EN_DITHER) // YUV422 to YUV444 need dither
            {
                filter |= B_TX_EN_DITHER | B_TX_DNFREE_GO ;
            }

            break ;
        case F_MODE_YUV422:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("Output mode is YUV422\n"));
#endif
            csc = B_CSC_BYPASS ;

            break ;

        case F_MODE_RGB444:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("Output mode is RGB24\n"));
#endif
            csc = B_CSC_YUV2RGB ;
            if(bInputMode & F_MODE_EN_UDFILT) // YUV422 to RGB24 need up/dn filter.
            {
                filter |= B_TX_EN_UDFILTER ;
            }

            if(bInputMode & F_MODE_EN_DITHER) // YUV422 to RGB24 need dither
            {
                filter |= B_TX_EN_DITHER | B_TX_DNFREE_GO ;
            }

            break ;
        }
        break ;
    #endif

    #ifdef SUPPORT_INPUTRGB
    case F_MODE_RGB444:
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("Input mode is RGB24\n"));
#endif
        switch(bOutputMode&F_MODE_CLRMOD_MASK)
        {
        case F_MODE_YUV444:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("Output mode is YUV444\n"));
#endif
            csc = B_CSC_RGB2YUV ;

            if(bInputMode & F_MODE_EN_DITHER) // RGB24 to YUV444 need dither
            {
                filter |= B_TX_EN_DITHER | B_TX_DNFREE_GO ;
            }
            break ;

        case F_MODE_YUV422:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("Output mode is YUV422\n"));
#endif
            if(bInputMode & F_MODE_EN_UDFILT) // RGB24 to YUV422 need down filter.
            {
                filter |= B_TX_EN_UDFILTER ;
            }

            if(bInputMode & F_MODE_EN_DITHER) // RGB24 to YUV422 need dither
            {
                filter |= B_TX_EN_DITHER | B_TX_DNFREE_GO ;
            }
            csc = B_CSC_RGB2YUV ;
            break ;

        case F_MODE_RGB444:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("Output mode is RGB24\n"));
#endif
            csc = B_CSC_BYPASS ;
            break ;
        }
        break ;
    #endif
    }

    #ifdef SUPPORT_INPUTRGB
    // set the CSC metrix registers by colorimetry and quantization
    if(csc == B_CSC_RGB2YUV)
    {
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("CSC = RGB2YUV %x ",csc));
#endif
        switch(bInputMode&(F_MODE_ITU709|F_MODE_16_235))
        {
        case F_MODE_ITU709|F_MODE_16_235:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("ITU709 16-235 "));
#endif
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_YOFF,bCSCOffset_16_235,SIZEOF_CSCOFFSET);
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_MTX11_L,bCSCMtx_RGB2YUV_ITU709_16_235,SIZEOF_CSCMTX);
            break ;
        case F_MODE_ITU709|F_MODE_0_255:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("ITU709 0-255 "));
#endif
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_YOFF,bCSCOffset_0_255,SIZEOF_CSCOFFSET);
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_MTX11_L,bCSCMtx_RGB2YUV_ITU709_0_255,SIZEOF_CSCMTX);
            break ;
        case F_MODE_ITU601|F_MODE_16_235:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("ITU601 16-235 "));
#endif
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_YOFF,bCSCOffset_16_235,SIZEOF_CSCOFFSET);
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_MTX11_L,bCSCMtx_RGB2YUV_ITU601_16_235,SIZEOF_CSCMTX);
            break ;
        case F_MODE_ITU601|F_MODE_0_255:
        default:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("ITU601 0-255 "));
#endif
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_YOFF,bCSCOffset_0_255,SIZEOF_CSCOFFSET);
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_MTX11_L,bCSCMtx_RGB2YUV_ITU601_0_255,SIZEOF_CSCMTX);
            break ;
        }

    }
    #endif

    #ifdef SUPPORT_INPUTYUV
    if (csc == B_CSC_YUV2RGB)
    {
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("CSC = YUV2RGB %x ",csc));
#endif
        switch(bInputMode&(F_MODE_ITU709|F_MODE_16_235))
        {
        case F_MODE_ITU709|F_MODE_16_235:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("ITU709 16-235 "));
#endif
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_YOFF,bCSCOffset_16_235,SIZEOF_CSCOFFSET);
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_MTX11_L,bCSCMtx_YUV2RGB_ITU709_16_235,SIZEOF_CSCMTX);
            break ;
        case F_MODE_ITU709|F_MODE_0_255:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("ITU709 0-255 "));
#endif
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_YOFF,bCSCOffset_0_255,SIZEOF_CSCOFFSET);
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_MTX11_L,bCSCMtx_YUV2RGB_ITU709_0_255,SIZEOF_CSCMTX);
            break ;
        case F_MODE_ITU601|F_MODE_16_235:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("ITU601 16-235 "));
#endif
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_YOFF,bCSCOffset_16_235,SIZEOF_CSCOFFSET);
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_MTX11_L,bCSCMtx_YUV2RGB_ITU601_16_235,SIZEOF_CSCMTX);
            break ;
        case F_MODE_ITU601|F_MODE_0_255:
        default:
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("ITU601 0-255 "));
#endif
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_YOFF,bCSCOffset_0_255,SIZEOF_CSCOFFSET);
            HDMITX_WriteI2C_ByteN(REG_TX_CSC_MTX11_L,bCSCMtx_YUV2RGB_ITU601_0_255,SIZEOF_CSCMTX);
            break ;
        }
    }
    #endif

    ucData = HDMITX_ReadI2C_Byte(REG_TX_CSC_CTRL) & ~(M_CSC_SEL|B_TX_DNFREE_GO|B_TX_EN_DITHER|B_TX_EN_UDFILTER);
    ucData |= filter|csc ;

    HDMITX_WriteI2C_Byte(REG_TX_CSC_CTRL,ucData);

    // set output Up/Down Filter,Dither control

}


//////////////////////////////////////////////////////////////////////
// Function: SetupAFE
// Parameter: VIDEOPCLKLEVEL level
//            PCLK_LOW - for 13.5MHz (for mode less than 1080p)
//            PCLK MEDIUM - for 25MHz~74MHz
//            PCLK HIGH - PCLK > 80Hz (for 1080p mode or above)
// Return: N/A
// Remark: set reg62~reg65 depended on HighFreqMode
//         reg61 have to be programmed at last and after video stable input.
// Side-Effect:
//////////////////////////////////////////////////////////////////////

static void
// SetupAFE(BYTE ucFreqInMHz)
SetupAFE(VIDEOPCLKLEVEL level)
{
    BYTE uc ;
    // @emily turn off reg61 before SetupAFE parameters.
    HDMITX_WriteI2C_Byte(REG_TX_AFE_DRV_CTRL,B_AFE_DRV_RST);/* 0x10 */
#ifdef Debug_message
    // HDMITX_WriteI2C_Byte(REG_TX_AFE_DRV_CTRL,0x3);
    HDMITX_DEBUG_PRINTF(("SetupAFE()\n"));
#endif
    //TMDS Clock < 80MHz    TMDS Clock > 80MHz
    //Reg61    0x03    0x03

    //Reg62    0x18    0x88
    //Reg63    Default    Default
    //Reg64    0x08    0x80
    //Reg65    Default    Default
    //Reg66    Default    Default
    //Reg67    Default    Default
    uc = HDMITX_ReadI2C_Byte(REG_TX_CLK_CTRL1);

#ifdef FALLING_EDGE_TRIGGER
    uc |= B_VDO_LATCH_EDGE ;
#else
    uc &= ~B_VDO_LATCH_EDGE ;
#endif
    HDMITX_WriteI2C_Byte(REG_TX_CLK_CTRL1, uc);
    switch(level)
    {
    case PCLK_HIGH:
        HDMITX_WriteI2C_Byte(REG_TX_AFE_XP_CTRL,0x88); // reg62
        HDMITX_WriteI2C_Byte(REG_TX_AFE_ISW_CTRL, 0x10); // reg63
        HDMITX_WriteI2C_Byte(REG_TX_AFE_IP_CTRL,0x84); // reg64
        break ;
    default:
        HDMITX_WriteI2C_Byte(REG_TX_AFE_XP_CTRL,0x18); // reg62
        HDMITX_WriteI2C_Byte(REG_TX_AFE_ISW_CTRL, 0x10); // reg63
        HDMITX_WriteI2C_Byte(REG_TX_AFE_IP_CTRL,0x0C); // reg64
        break ;
    }
    // 2009/01/15 modified by Mingchih.Lung@ite.com.tw
    /*
    uc = HDMITX_ReadI2C_Byte(REG_TX_SW_RST);
    uc &= ~(B_REF_RST|B_VID_RST);
    HDMITX_WriteI2C_Byte(REG_TX_SW_RST,uc);
    delay1ms(1);
    */
    // HDMITX_WriteI2C_Byte(REG_TX_SW_RST,uc|B_VID_RST);
    // delay1ms(100);
    // HDMITX_WriteI2C_Byte(REG_TX_SW_RST,uc);
    //~Mingchih.Lung@ite.com.tw


}


//////////////////////////////////////////////////////////////////////
// Function: FireAFE
// Parameter: N/A
// Return: N/A
// Remark: write reg61 with 0x04
//         When program reg61 with 0x04,then audio and video circuit work.
// Side-Effect: N/A
//////////////////////////////////////////////////////////////////////
static void
FireAFE()
{
    BYTE reg;
    SoftWareVideoReset();
    Switch_HDMITX_Bank(0) ;
    HDMITX_WriteI2C_Byte(REG_TX_AFE_DRV_CTRL,0);
#ifdef Debug_message
    for(reg = 0x61 ; reg <= 0x67 ; reg++)
    {
        HDMITX_DEBUG_PRINTF(("Reg[%02X] = %02X\n",(WORD)reg,(WORD)HDMITX_ReadI2C_Byte(reg)));
    }
#endif
    Instance[0].bVideoOut=TRUE;
    //EnableHVToolDetect(FALSE);
    //EnableHVToolDetect(TRUE);
}
//////////////////////////////////////////////////////////////////////
// Audio Output
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Function: SetAudioFormat
// Parameter:
//    NumChannel - number of channel,from 1 to 8
//    AudioEnable - Audio source and type bit field,value of bit field are
//        ENABLE_SPDIF    (1<<4)
//        ENABLE_I2S_SRC3  (1<<3)
//        ENABLE_I2S_SRC2  (1<<2)
//        ENABLE_I2S_SRC1  (1<<1)
//        ENABLE_I2S_SRC0  (1<<0)
//    SampleFreq - the audio sample frequence in Hz
//    AudSWL - Audio sample width,only support 16,18,20,or 24.
//    AudioCatCode - The audio channel catalogy code defined in IEC 60958-3
// Return: ER_SUCCESS if done,ER_FAIL for otherwise.
// Remark: program audio channel control register and audio channel registers
//         to enable audio by input.
// Side-Effect: register bank will keep in bank zero.
//////////////////////////////////////////////////////////////////////
static void
AutoAdjustAudio(void)
{
    unsigned long SampleFreq,cTMDSClock ;
    unsigned long N ;
    ULONG aCTS=0;
    BYTE fs, uc,LoopCnt=10;
    if(bForceCTS)
    {
        Switch_HDMITX_Bank(0);
        HDMITX_WriteI2C_Byte(0xF8, 0xC3) ;
        HDMITX_WriteI2C_Byte(0xF8, 0xA5) ;
        HDMITX_AndREG_Byte(REG_TX_PKT_SINGLE_CTRL,~B_SW_CTS) ; // D[1] = 0, HW auto count CTS
        HDMITX_WriteI2C_Byte(0xF8, 0xFF) ;
    }
    //delay1ms(50);
    Switch_HDMITX_Bank(1);
    N = ((unsigned long)HDMITX_ReadI2C_Byte(REGPktAudN2)&0xF) << 16 ;
    N |= ((unsigned long)HDMITX_ReadI2C_Byte(REGPktAudN1)) <<8 ;
    N |= ((unsigned long)HDMITX_ReadI2C_Byte(REGPktAudN0));

    while(LoopCnt--)
    {   ULONG TempCTS=0;
        aCTS = ((unsigned long)HDMITX_ReadI2C_Byte(REGPktAudCTSCnt2)) << 12 ;
        aCTS |= ((unsigned long)HDMITX_ReadI2C_Byte(REGPktAudCTSCnt1)) <<4 ;
        aCTS |= ((unsigned long)HDMITX_ReadI2C_Byte(REGPktAudCTSCnt0)&0xf0)>>4  ;
        if(aCTS==TempCTS)
        {break;}
        TempCTS=aCTS;
    }
    Switch_HDMITX_Bank(0);
    if( aCTS == 0)
    {
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("aCTS== 0"));
#endif
        return;
    }

    uc = HDMITX_ReadI2C_Byte(0xc1);

    cTMDSClock = GetInputPclk();//Instance[0].TMDSClock ;
    //TMDSClock=GetInputPclk();
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("PCLK = %u0,000\n",(WORD)(cTMDSClock/10000)));
#endif
    switch(uc & 0x70)
    {
    case 0x50:
        cTMDSClock *= 5 ;
        cTMDSClock /= 4 ;
        break ;
    case 0x60:
        cTMDSClock *= 3 ;
        cTMDSClock /= 2 ;
    }
    SampleFreq = cTMDSClock/aCTS ;
    SampleFreq *= N ;
    SampleFreq /= 128 ;
    //SampleFreq=48000;

#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("SampleFreq = %u0\n",(WORD)(SampleFreq/10)));
#endif
    if( SampleFreq>31000L && SampleFreq<=38050L ){fs = AUDFS_32KHz ;}
    else if (SampleFreq < 46550L )  {fs = AUDFS_44p1KHz ;}//46050
    else if (SampleFreq < 68100L )  {fs = AUDFS_48KHz ;}
    else if (SampleFreq < 92100L )  {fs = AUDFS_88p2KHz ;}
    else if (SampleFreq < 136200L ) {fs = AUDFS_96KHz ;}
    else if (SampleFreq < 184200L ) {fs = AUDFS_176p4KHz ;}
    else if (SampleFreq < 240200L ) {fs = AUDFS_192KHz ;}
    else if (SampleFreq < 800000L ) {fs = AUDFS_768KHz ;}
    else
    {
        fs = AUDFS_OTHER;
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("fs = AUDFS_OTHER\n"));
#endif
    }
    if(Instance[0].bAudFs != fs)
    {
        Instance[0].bAudFs=fs;
        SetNCTS(/*Instance[0].TMDSClock, */Instance[0].bAudFs); // set N, CTS by new generated clock.
        //CurrCTS=0;
        return;
    }
    return;
}
BOOL IsAudioChang()
{
    //ULONG pCTS=0;
    BYTE FreDiff=0,Refaudfreqnum;

    //Switch_HDMITX_Bank(1);
    //pCTS = ((unsigned long)HDMITX_ReadI2C_Byte(REGPktAudCTSCnt2)) << 12 ;
    //pCTS |= ((unsigned long)HDMITX_ReadI2C_Byte(REGPktAudCTSCnt1)) <<4 ;
    //pCTS |= ((unsigned long)HDMITX_ReadI2C_Byte(REGPktAudCTSCnt0)&0xf0)>>4  ;
    //Switch_HDMITX_Bank(0);
    Switch_HDMITX_Bank(0) ;
    Refaudfreqnum=HDMITX_ReadI2C_Byte(0x60);
    //HDMITX_DEBUG_PRINTF(("Refaudfreqnum=%X    pCTS= %u",(WORD)Refaudfreqnum,(WORD)(pCTS/10000)));
    //if((pCTS%10000)<1000)HDMITX_DEBUG_PRINTF(("0"));
    //if((pCTS%10000)<100)HDMITX_DEBUG_PRINTF(("0"));
    //if((pCTS%10000)<10)HDMITX_DEBUG_PRINTF(("0"));
    //HDMITX_DEBUG_PRINTF(("%u\n",(WORD)(pCTS%10000)));
    if((1<<4)&HDMITX_ReadI2C_Byte(0x5f))
    {
        //printf("=======XXXXXXXXXXX=========\n");
        return FALSE;
    }

    if(LastRefaudfreqnum>Refaudfreqnum)
        {FreDiff=LastRefaudfreqnum-Refaudfreqnum;}
    else
        {FreDiff=Refaudfreqnum-LastRefaudfreqnum;}
    LastRefaudfreqnum=Refaudfreqnum;
    if(2<FreDiff)
    {
        HDMITX_OrREG_Byte(REG_TX_PKT_SINGLE_CTRL,(1<<5));
        HDMITX_AndREG_Byte(REG_TX_AUDIO_CTRL0,0xF0);
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}
//void RampOn()
//{
    //HDMITX_OrREG_Byte(REG_TX_SW_RST,(B_AUD_RST|B_AREF_RST));
    //HDMITX_OrREG_Byte(REG_TX_PKT_SINGLE_CTRL,(1<<2));
    //HDMITX_AndREG_Byte(REG_TX_PKT_SINGLE_CTRL,(~0x3C));
    //HDMITX_AndREG_Byte(REG_TX_SW_RST,~(B_AUD_RST|B_AREF_RST));
//}
void SetupAudioChannel(BOOL EnableAudio_b)
{
    static BOOL AudioOutStatus=FALSE;
    if(EnableAudio_b)
    {
        if(AudioDelayCnt==0)
        {
            //if(Instance[0].bAuthenticated==FALSE)
            //{EnableHDCP(TRUE);}
#ifdef SUPPORT_AUDIO_MONITOR
            if(IsAudioChang())
            {
                AutoAdjustAudio();
#else
                if(AudioOutStatus==FALSE)
                {
                    SetNCTS(Instance[0].bAudFs);
#endif
                    HDMITX_WriteI2C_Byte(REG_TX_AUD_SRCVALID_FLAT,0);
                    HDMITX_OrREG_Byte(REG_TX_PKT_SINGLE_CTRL,(1<<5));
                    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL0, Instance[0].bAudioChannelEnable);
                    //HDMITX_OrREG_Byte(0x59,(1<<2));  //for test
                    HDMITX_AndREG_Byte(REG_TX_PKT_SINGLE_CTRL,(~0x3C));
                    HDMITX_AndREG_Byte(REG_TX_PKT_SINGLE_CTRL,(~(1<<5)));
                    //printf("Audio Out Enable\n");
#ifndef SUPPORT_AUDIO_MONITOR
                    AudioOutStatus=TRUE;
                }
#endif
            }
        }
        else
        {
            AudioOutStatus=FALSE;
            if(((Instance[0].bAudioChannelEnable & B_AUD_SPDIF)==0)|| // if I2S , ignore the reg5F[5] check.
                (0x20==(HDMITX_ReadI2C_Byte(REG_TX_CLK_STATUS2)&0x30)))
            {
                AudioDelayCnt--;
                //printf("AudioDelayCnt=%u\n",(WORD)AudioDelayCnt);
            }
            else
            {
                AudioDelayCnt=AudioOutDelayCnt;
            }
        }
    }
    else
    {
       // CurrCTS=0;
    }
}
//////////////////////////////////////////////////////////////////////
// Function: SetNCTS
// Parameter: PCLK - video clock in Hz.
//            Fs - Encoded audio sample rate
//                          AUDFS_22p05KHz  4
//                          AUDFS_44p1KHz 0
//                          AUDFS_88p2KHz 8
//                          AUDFS_176p4KHz    12
//
//                          AUDFS_24KHz  6
//                          AUDFS_48KHz  2
//                          AUDFS_96KHz  10
//                          AUDFS_192KHz 14
//
//                          AUDFS_768KHz 9
//
//                          AUDFS_32KHz  3
//                          AUDFS_OTHER    1

// Return: ER_SUCCESS if success
// Remark: set N value,the CTS will be auto generated by HW.
// Side-Effect: register bank will reset to bank 0.
//////////////////////////////////////////////////////////////////////

static SYS_STATUS
SetNCTS(/*ULONG PCLK,*/BYTE Fs)
{
    ULONG n,MCLK,SampleFreq;
    BYTE LoopCnt=255,CTSStableCnt=0;
    ULONG diff;
    ULONG CTS=0,LastCTS=0;
    BOOL HBR_mode;
    BYTE aVIC;
    if(B_HBR & HDMITX_ReadI2C_Byte(REG_TX_AUD_HDAUDIO))
    {
        HBR_mode=TRUE;
    }
    else
    {
        HBR_mode=FALSE;
    }

    Switch_HDMITX_Bank(1);
    aVIC = (HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB4)&0x7f);
    Switch_HDMITX_Bank(0);

    if(aVIC)
    {
        switch(Fs)
        {
        case AUDFS_32KHz: n = 4096; break;
        case AUDFS_44p1KHz: n = 6272; break;
        case AUDFS_48KHz: n = 6144; break;
        case AUDFS_88p2KHz: n = 12544; break;
        case AUDFS_96KHz: n = 12288; break;
        case AUDFS_176p4KHz: n = 25088; break;
        case AUDFS_192KHz: n = 24576; break;
        default: n = 6144;
        }
    }
    else
    {
        switch(Fs)
        {
            case AUDFS_32KHz: SampleFreq = 32000L; break;
            case AUDFS_44p1KHz: SampleFreq = 44100L; break;
            case AUDFS_48KHz: SampleFreq = 48000L; break;
            case AUDFS_88p2KHz: SampleFreq = 88200L; break;
            case AUDFS_96KHz: SampleFreq = 96000L; break;
            case AUDFS_176p4KHz: SampleFreq = 176000L; break;
            case AUDFS_192KHz: SampleFreq = 192000L; break;
            default: SampleFreq = 768000L;
        }
        MCLK = SampleFreq * 256 ; // MCLK = fs * 256 ;
        n = MCLK / 2000;
    }
    // tr_printf((" n = %ld\n",n)) ;
    Switch_HDMITX_Bank(1) ;
    HDMITX_WriteI2C_Byte(REGPktAudN0,(BYTE)((n)&0xFF)) ;
    HDMITX_WriteI2C_Byte(REGPktAudN1,(BYTE)((n>>8)&0xFF)) ;
    HDMITX_WriteI2C_Byte(REGPktAudN2,(BYTE)((n>>16)&0xF)) ;
    //Switch_HDMITX_Bank(0) ;
    //delay1ms(200);
    //Switch_HDMITX_Bank(1) ;
    if(bForceCTS)
    {
        ULONG SumCTS=0;
        while(LoopCnt--)
        {
            delay1ms(30);
            CTS = ((unsigned long)HDMITX_ReadI2C_Byte(REGPktAudCTSCnt2)) << 12 ;
            CTS |= ((unsigned long)HDMITX_ReadI2C_Byte(REGPktAudCTSCnt1)) <<4 ;
            CTS |= ((unsigned long)HDMITX_ReadI2C_Byte(REGPktAudCTSCnt0)&0xf0)>>4  ;
            if( CTS == 0)
            {
                continue;
            }
            else
            {
                if(LastCTS>CTS )
                    {diff=LastCTS-CTS;}
                else
                    {diff=CTS-LastCTS;}
                //HDMITX_DEBUG_PRINTF(("LastCTS= %u%u",(WORD)(LastCTS/10000),(WORD)(LastCTS%10000)));
                //HDMITX_DEBUG_PRINTF(("       CTS= %u%u\n",(WORD)(CTS/10000),(WORD)(CTS%10000)));
                LastCTS=CTS;
                if(5>diff)
                {
                    CTSStableCnt++;
                    SumCTS+=CTS;
                }
                else
                {
                    CTSStableCnt=0;
                    SumCTS=0;
                    continue;
                }
                if(CTSStableCnt>=32)
                {
                    LastCTS=(SumCTS>>5);
                    break;
                }
            }
        }
    }
    HDMITX_WriteI2C_Byte(REGPktAudCTS0,(BYTE)((LastCTS)&0xFF)) ;
    HDMITX_WriteI2C_Byte(REGPktAudCTS1,(BYTE)((LastCTS>>8)&0xFF)) ;
    HDMITX_WriteI2C_Byte(REGPktAudCTS2,(BYTE)((LastCTS>>16)&0xF)) ;
    Switch_HDMITX_Bank(0) ;
#ifdef Force_CTS
    bForceCTS = TRUE;
#endif
    HDMITX_WriteI2C_Byte(0xF8, 0xC3) ;
    HDMITX_WriteI2C_Byte(0xF8, 0xA5) ;
    if(bForceCTS)
    {
        HDMITX_OrREG_Byte(REG_TX_PKT_SINGLE_CTRL,B_SW_CTS) ; // D[1] = 0, HW auto count CTS
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("bForceCTS = TRUE  [LoopCnt=%u]\n Write CTS= %u",(WORD)LoopCnt,(WORD)(LastCTS/10000)));
        HDMITX_DEBUG_PRINTF(("%u\n",(WORD)(LastCTS%10000)));
#endif
    }
    else
    {
        HDMITX_AndREG_Byte(REG_TX_PKT_SINGLE_CTRL,~B_SW_CTS) ; // D[1] = 0, HW auto count CTS
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("bForceCTS = FALSE\n"));
#endif
    }
    HDMITX_WriteI2C_Byte(0xF8, 0xFF) ;

    if(FALSE==HBR_mode) //LPCM
    {
        BYTE uData;
        Switch_HDMITX_Bank(1);
        HDMITX_WriteI2C_Byte(REG_TX_AUDCHST_CA_FS,0x00|Fs);
        Fs = ~Fs ; // OFS is the one's complement of FS
        uData = (0x0f&HDMITX_ReadI2C_Byte(REG_TX_AUDCHST_OFS_WL));
        HDMITX_WriteI2C_Byte(REG_TX_AUDCHST_OFS_WL,(Fs<<4)|uData);
        Switch_HDMITX_Bank(0);
    }
 #ifdef Debug_message
 //   HDMITX_DEBUG_PRINTF(("CurrCTS = %u",(WORD)(LastCTS/10000)));
 //   HDMITX_DEBUG_PRINTF(("%u\n",(WORD)(CurrCTS%10000)));
    HDMITX_DEBUG_PRINTF(("N = %d \n",(WORD)n));
    //HDMITX_DEBUG_PRINTF(("SampleFreq = %d 000\n",(WORD)(SampleFreq/1000)));
    HDMITX_DEBUG_PRINTF(("**************************************\n"));
#endif

    return ER_SUCCESS ;
}

//////////////////////////////////////////////////////////////////////
// DDC Function.
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Function: ClearDDCFIFO
// Parameter: N/A
// Return: N/A
// Remark: clear the DDC FIFO.
// Side-Effect: DDC master will set to be HOST.
//////////////////////////////////////////////////////////////////////

static void
ClearDDCFIFO()
{
    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERDDC|B_MASTERHOST);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_CMD,CMD_FIFO_CLR);
}

static void
GenerateDDCSCLK()
{
    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERDDC|B_MASTERHOST);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_CMD,CMD_GEN_SCLCLK);
}
//////////////////////////////////////////////////////////////////////
// Function: AbortDDC
// Parameter: N/A
// Return: N/A
// Remark: Force abort DDC and reset DDC bus.
// Side-Effect:
//////////////////////////////////////////////////////////////////////

static void
AbortDDC()
{
    BYTE CPDesire,SWReset,DDCMaster ;
    BYTE uc, timeout, i ;
    // save the SW reset,DDC master,and CP Desire setting.
    SWReset = HDMITX_ReadI2C_Byte(REG_TX_SW_RST);
    CPDesire = HDMITX_ReadI2C_Byte(REG_TX_HDCP_DESIRE);
    DDCMaster = HDMITX_ReadI2C_Byte(REG_TX_DDC_MASTER_CTRL);


    HDMITX_WriteI2C_Byte(REG_TX_HDCP_DESIRE,0x08|(CPDesire&(~B_CPDESIRE))); // @emily change order
    HDMITX_WriteI2C_Byte(REG_TX_SW_RST,SWReset|B_HDCP_RST);         // @emily change order
    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERDDC|B_MASTERHOST);

    // 2009/01/15 modified by Mingchih.Lung@ite.com.tw
    // do abort DDC twice.
    for( i = 0 ; i < 2 ; i++ )
    {
        HDMITX_WriteI2C_Byte(REG_TX_DDC_CMD,CMD_DDC_ABORT);

        for( timeout = 0 ; timeout < 200 ; timeout++ )
        {
            uc = HDMITX_ReadI2C_Byte(REG_TX_DDC_STATUS);
            if (uc&B_DDC_DONE)
            {
                break ; // success
            }

            if( uc & (B_DDC_NOACK|B_DDC_WAITBUS|B_DDC_ARBILOSE) )
            {
#ifdef Debug_message
                HDMITX_DEBUG_PRINTF(("AbortDDC Fail by reg16=%02X\n",uc));
#endif
                break ;
            }
            delay1ms(1); // delay 1 ms to stable.
        }
    }
    //~Mingchih.Lung@ite.com.tw


    // 2009/01/15 modified by Mingchih.Lung@ite.com.tw
    //// restore the SW reset,DDC master,and CP Desire setting.
    HDMITX_WriteI2C_Byte(REG_TX_SW_RST,SWReset);
    HDMITX_WriteI2C_Byte(REG_TX_HDCP_DESIRE,0x08|CPDesire);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,DDCMaster);
    //~Mingchih.Lung@ite.com.tw

}

//////////////////////////////////////////////////////////////////////
// Packet and InfoFrame
//////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////////
// // Function: SetAVMute()
// // Parameter: N/A
// // Return: N/A
// // Remark: set AVMute as TRUE and enable GCP sending.
// // Side-Effect: N/A
// ////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// void
// SetAVMute()
// {
//     Switch_HDMITX_Bank(0);
//     HDMITX_WriteI2C_Byte(REG_TX_GCP,B_SET_AVMUTE);
//     HDMITX_WriteI2C_Byte(REG_TX_PKT_GENERAL_CTRL,B_ENABLE_PKT|B_REPEAT_PKT);
// }

// ////////////////////////////////////////////////////////////////////////////////
// // Function: SetAVMute(FALSE)
// // Parameter: N/A
// // Return: N/A
// // Remark: clear AVMute as TRUE and enable GCP sending.
// // Side-Effect: N/A
// ////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// void
// SetAVMute(FALSE)
// {
//     Switch_HDMITX_Bank(0);
//     HDMITX_WriteI2C_Byte(REG_TX_GCP,B_CLR_AVMUTE);
//     HDMITX_WriteI2C_Byte(REG_TX_PKT_GENERAL_CTRL,B_ENABLE_PKT|B_REPEAT_PKT);
// }



//////////////////////////////////////////////////////////////////////
// Function: ReadEDID
// Parameter: pData - the pointer of buffer to receive EDID ucdata.
//            bSegment - the segment of EDID readback.
//            offset - the offset of EDID ucdata in the segment. in byte.
//            count - the read back bytes count,cannot exceed 32
// Return: ER_SUCCESS if successfully getting EDID. ER_FAIL otherwise.
// Remark: function for read EDID ucdata from reciever.
// Side-Effect: DDC master will set to be HOST. DDC FIFO will be used and dirty.
//////////////////////////////////////////////////////////////////////

static SYS_STATUS
ReadEDID(BYTE *pData,BYTE bSegment,BYTE offset,SHORT Count)
{
    SHORT RemainedCount,ReqCount ;
    BYTE bCurrOffset ;
    SHORT TimeOut ;
    BYTE *pBuff = pData ;
    BYTE ucdata ;

#ifdef Debug_message
    // HDMITX_DEBUG_PRINTF(("ReadEDID(%08lX,%d,%d,%d)\n",(ULONG)pData,bSegment,offset,Count));
#endif
    if(!pData)
    {
#ifdef Debug_message
//        HDMITX_DEBUG_PRINTF(("ReadEDID(): Invallid pData pointer %08lX\n",(ULONG)pData));
#endif
        return ER_FAIL ;
    }

    if(HDMITX_ReadI2C_Byte(REG_TX_INT_STAT1) & B_INT_DDC_BUS_HANG)
    {
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("Called AboutDDC()\n"));
#endif
        AbortDDC();

    }

    ClearDDCFIFO();

    RemainedCount = Count ;
    bCurrOffset = offset ;

    Switch_HDMITX_Bank(0);

    while(RemainedCount > 0)
    {

        ReqCount = (RemainedCount > DDC_FIFO_MAXREQ)?DDC_FIFO_MAXREQ:RemainedCount ;
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("ReadEDID(): ReqCount = %d,bCurrOffset = %d\n",(WORD)ReqCount,(WORD)bCurrOffset));
#endif
        HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERDDC|B_MASTERHOST);
        HDMITX_WriteI2C_Byte(REG_TX_DDC_CMD,CMD_FIFO_CLR);

        for(TimeOut = 0 ; TimeOut < 200 ; TimeOut++)
        {
            ucdata = HDMITX_ReadI2C_Byte(REG_TX_DDC_STATUS);

            if(ucdata&B_DDC_DONE)
            {
                break ;
            }

            if((ucdata & B_DDC_ERROR)||(HDMITX_ReadI2C_Byte(REG_TX_INT_STAT1) & B_INT_DDC_BUS_HANG))
            {
#ifdef Debug_message
                HDMITX_DEBUG_PRINTF(("Called AboutDDC()\n"));
#endif
                AbortDDC();
                return ER_FAIL ;
            }
        }

        HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERDDC|B_MASTERHOST);
        HDMITX_WriteI2C_Byte(REG_TX_DDC_HEADER,DDC_EDID_ADDRESS); // for EDID ucdata get
        HDMITX_WriteI2C_Byte(REG_TX_DDC_REQOFF,bCurrOffset);
        HDMITX_WriteI2C_Byte(REG_TX_DDC_REQCOUNT,(BYTE)ReqCount);
        HDMITX_WriteI2C_Byte(REG_TX_DDC_EDIDSEG,bSegment);
        HDMITX_WriteI2C_Byte(REG_TX_DDC_CMD,CMD_EDID_READ);

        bCurrOffset += ReqCount ;
        RemainedCount -= ReqCount ;

        for(TimeOut = 250 ; TimeOut > 0 ; TimeOut --)
        {
            delay1ms(1);
            ucdata = HDMITX_ReadI2C_Byte(REG_TX_DDC_STATUS);
            if(ucdata & B_DDC_DONE)
            {
                break ;
            }

            if(ucdata & B_DDC_ERROR)
            {
#ifdef Debug_message
//                HDMITX_DEBUG_PRINTF(("ReadEDID(): DDC_STATUS = %02X,fail.\n",ucdata));
#endif
                return ER_FAIL ;
            }
        }

        if(TimeOut == 0)
        {
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("ReadEDID(): DDC TimeOut. %02X\n",(WORD)ucdata));
#endif
            return ER_FAIL ;
        }

        do
        {
            *(pBuff++) = HDMITX_ReadI2C_Byte(REG_TX_DDC_READFIFO);
            ReqCount -- ;
        }while(ReqCount > 0);

    }

    return ER_SUCCESS ;
}



#ifdef SUPPORT_HDCP
//////////////////////////////////////////////////////////////////////
// Authentication
//////////////////////////////////////////////////////////////////////
static void
HDCP_ClearAuthInterrupt()
{
    BYTE uc ;
    uc = HDMITX_ReadI2C_Byte(REG_TX_INT_MASK2) & (~(B_KSVLISTCHK_MASK|B_T_AUTH_DONE_MASK|B_AUTH_FAIL_MASK));
    HDMITX_WriteI2C_Byte(REG_TX_INT_CLR0,B_CLR_AUTH_FAIL|B_CLR_AUTH_DONE|B_CLR_KSVLISTCHK);
    HDMITX_WriteI2C_Byte(REG_TX_INT_CLR1,0);
    HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,B_INTACTDONE);
}

static void
HDCP_ResetAuth()
{
    HDMITX_WriteI2C_Byte(REG_TX_LISTCTRL,0);
    HDMITX_WriteI2C_Byte(REG_TX_HDCP_DESIRE,0x08);
    HDMITX_OrREG_Byte(REG_TX_SW_RST,B_HDCP_RST);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERDDC|B_MASTERHOST);
    HDCP_ClearAuthInterrupt();
    AbortDDC();
}
//////////////////////////////////////////////////////////////////////
// Function: HDCP_EnableEncryption
// Parameter: N/A
// Return: ER_SUCCESS if done.
// Remark: Set regC1 as zero to enable continue authentication.
// Side-Effect: register bank will reset to zero.
//////////////////////////////////////////////////////////////////////

static SYS_STATUS
HDCP_EnableEncryption()
{
    Switch_HDMITX_Bank(0);
    return HDMITX_WriteI2C_Byte(REG_TX_ENCRYPTION,B_ENABLE_ENCRYPTION);
}


//////////////////////////////////////////////////////////////////////
// Function: HDCP_Auth_Fire()
// Parameter: N/A
// Return: N/A
// Remark: write anything to reg21 to enable HDCP authentication by HW
// Side-Effect: N/A
//////////////////////////////////////////////////////////////////////

static void
HDCP_Auth_Fire()
{
#ifdef Debug_message
    // HDMITX_DEBUG_PRINTF(("HDCP_Auth_Fire():\n"));
#endif
    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERDDC|B_MASTERHDCP); // MASTERHDCP,no need command but fire.
    HDMITX_WriteI2C_Byte(REG_TX_AUTHFIRE,1);
}

//////////////////////////////////////////////////////////////////////
// Function: HDCP_StartAnCipher
// Parameter: N/A
// Return: N/A
// Remark: Start the Cipher to free run for random number. When stop,An is
//         ready in Reg30.
// Side-Effect: N/A
//////////////////////////////////////////////////////////////////////

static void
HDCP_StartAnCipher()
{
    HDMITX_WriteI2C_Byte(REG_TX_AN_GENERATE,B_START_CIPHER_GEN);
    delay1ms(1); // delay 1 ms
}

//////////////////////////////////////////////////////////////////////
// Function: HDCP_StopAnCipher
// Parameter: N/A
// Return: N/A
// Remark: Stop the Cipher,and An is ready in Reg30.
// Side-Effect: N/A
//////////////////////////////////////////////////////////////////////

static void
HDCP_StopAnCipher()
{
    HDMITX_WriteI2C_Byte(REG_TX_AN_GENERATE,B_STOP_CIPHER_GEN);
}

//////////////////////////////////////////////////////////////////////
// Function: HDCP_GenerateAn
// Parameter: N/A
// Return: N/A
// Remark: start An ciper random run at first,then stop it. Software can get
//         an in reg30~reg38,the write to reg28~2F
// Side-Effect:
//////////////////////////////////////////////////////////////////////

static void
HDCP_GenerateAn()
{
    BYTE Data[8] ;
    BYTE i=0;
#if 1
    HDCP_StartAnCipher();
    // HDMITX_WriteI2C_Byte(REG_TX_AN_GENERATE,B_START_CIPHER_GEN);
    // delay1ms(1); // delay 1 ms
    // HDMITX_WriteI2C_Byte(REG_TX_AN_GENERATE,B_STOP_CIPHER_GEN);

    HDCP_StopAnCipher();

    Switch_HDMITX_Bank(0);
    // new An is ready in reg30
    HDMITX_ReadI2C_ByteN(REG_TX_AN_GEN,Data,8);
#else
    Data[0] = 0 ;Data[1] = 0 ;Data[2] = 0 ;Data[3] = 0 ;
    Data[4] = 0 ;Data[5] = 0 ;Data[6] = 0 ;Data[7] = 0 ;
#endif
    for(i=0;i<8;i++)
        HDMITX_WriteI2C_Byte(REG_TX_AN,Data[i]);

}


//////////////////////////////////////////////////////////////////////
// Function: HDCP_GetBCaps
// Parameter: pBCaps - pointer of byte to get BCaps.
//            pBStatus - pointer of two bytes to get BStatus
// Return: ER_SUCCESS if successfully got BCaps and BStatus.
// Remark: get B status and capability from HDCP reciever via DDC bus.
// Side-Effect:
//////////////////////////////////////////////////////////////////////

static SYS_STATUS
HDCP_GetBCaps(PBYTE pBCaps ,PUSHORT pBStatus)
{
    BYTE ucdata ;
    BYTE TimeOut ;

    Switch_HDMITX_Bank(0);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERDDC|B_MASTERHOST);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_HEADER,DDC_HDCP_ADDRESS);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_REQOFF,0x40); // BCaps offset
    HDMITX_WriteI2C_Byte(REG_TX_DDC_REQCOUNT,3);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_CMD,CMD_DDC_SEQ_BURSTREAD);

    for(TimeOut = 200 ; TimeOut > 0 ; TimeOut --)
    {
        delay1ms(1);

        ucdata = HDMITX_ReadI2C_Byte(REG_TX_DDC_STATUS);
        if(ucdata & B_DDC_DONE)
        {
#ifdef Debug_message
            //HDMITX_DEBUG_PRINTF(("HDCP_GetBCaps(): DDC Done.\n"));
#endif
            break ;
        }

        if(ucdata & B_DDC_ERROR)
        {
#ifdef Debug_message
//            HDMITX_DEBUG_PRINTF(("HDCP_GetBCaps(): DDC fail by reg16=%02X.\n",ucdata));
#endif
            return ER_FAIL ;
        }
    }

    if(TimeOut == 0)
    {
        return ER_FAIL ;
    }

    //HDMITX_ReadI2C_ByteN(REG_TX_BSTAT,(PBYTE)pBStatus,2);
    *pBStatus &=0x0000;
    *pBStatus=(HDMITX_ReadI2C_Byte(REG_TX_BSTAT)&0xff);
    *pBStatus +=((HDMITX_ReadI2C_Byte(REG_TX_BSTAT1)&0xff)<<8);
    *pBCaps = HDMITX_ReadI2C_Byte(REG_TX_BCAP);
    return ER_SUCCESS ;

}


//////////////////////////////////////////////////////////////////////
// Function: HDCP_GetBKSV
// Parameter: pBKSV - pointer of 5 bytes buffer for getting BKSV
// Return: ER_SUCCESS if successfuly got BKSV from Rx.
// Remark: Get BKSV from HDCP reciever.
// Side-Effect: N/A
//////////////////////////////////////////////////////////////////////

static SYS_STATUS
HDCP_GetBKSV(BYTE *pBKSV)
{
    BYTE ucdata ;
    BYTE TimeOut ;

    Switch_HDMITX_Bank(0);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERDDC|B_MASTERHOST);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_HEADER,DDC_HDCP_ADDRESS);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_REQOFF,0x00); // BKSV offset
    HDMITX_WriteI2C_Byte(REG_TX_DDC_REQCOUNT,5);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_CMD,CMD_DDC_SEQ_BURSTREAD);

    for(TimeOut = 200 ; TimeOut > 0 ; TimeOut --)
    {
        delay1ms(1);

        ucdata = HDMITX_ReadI2C_Byte(REG_TX_DDC_STATUS);
        if(ucdata & B_DDC_DONE)
        {
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("HDCP_GetBCaps(): DDC Done.\n"));
#endif
            break ;
        }

        if(ucdata & B_DDC_ERROR)
        {
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("HDCP_GetBCaps(): DDC No ack or arbilose,%x,maybe cable did not connected. Fail.\n",(WORD)ucdata));
#endif
            return ER_FAIL ;
        }
    }

    if(TimeOut == 0)
    {
        return ER_FAIL ;
    }

    HDMITX_ReadI2C_ByteN(REG_TX_BKSV,(PBYTE)pBKSV,5);

    return ER_SUCCESS ;
}

//////////////////////////////////////////////////////////////////////
// Function:HDCP_Authenticate
// Parameter: N/A
// Return: ER_SUCCESS if Authenticated without error.
// Remark: do Authentication with Rx
// Side-Effect:
//  1. Instance[0].bAuthenticated global variable will be TRUE when authenticated.
//  2. Auth_done interrupt and AUTH_FAIL interrupt will be enabled.
//////////////////////////////////////////////////////////////////////
static BYTE
countbit(BYTE b)
{
    BYTE i,count ;
    for( i = 0, count = 0 ; i < 8 ; i++ )
    {
        if( b & (1<<i) )
        {
            count++ ;
        }
    }
    return count ;
}

static void
HDCP_Reset()
{
    BYTE uc ;
    uc = HDMITX_ReadI2C_Byte(REG_TX_SW_RST) | B_HDCP_RST ;
    HDMITX_WriteI2C_Byte(REG_TX_SW_RST,uc);
    HDMITX_WriteI2C_Byte(REG_TX_HDCP_DESIRE,0x08);
    HDMITX_WriteI2C_Byte(REG_TX_LISTCTRL,0);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERHOST);
    ClearDDCFIFO();
    AbortDDC();
}

static SYS_STATUS
HDCP_Authenticate()
{
    BYTE ucdata ;
    BYTE BCaps ;
    USHORT BStatus ;
    USHORT TimeOut ;

    BYTE revoked = FALSE ;
    BYTE BKSV[5] ;

    Instance[0].bAuthenticated = FALSE ;

    // Authenticate should be called after AFE setup up.
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("HDCP_Authenticate():\n"));
#endif
    HDCP_Reset();
    // ClearDDCFIFO();
    // AbortDDC();

    Switch_HDMITX_Bank(0);

    for( TimeOut = 0 ; TimeOut < 20 ; TimeOut++ )
    {
        delay1ms(15);

        if(HDCP_GetBCaps(&BCaps,&BStatus) != ER_SUCCESS)
        {
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("HDCP_GetBCaps fail.\n"));
#endif
            return ER_FAIL ;
        }

        if(B_TX_HDMI_MODE == (HDMITX_ReadI2C_Byte(REG_TX_HDMI_MODE) & B_TX_HDMI_MODE ))
        {
            if((BStatus & B_CAP_HDMI_MODE)==B_CAP_HDMI_MODE)
            {
                break;
            }
        }
        else
        {
            if((BStatus & B_CAP_HDMI_MODE)!=B_CAP_HDMI_MODE)
            {
                break;
            }
        }
    }
#ifdef Debug_message
//    HDMITX_DEBUG_PRINTF(("BCaps = %02X BStatus = %04X\n",BCaps,BStatus));
#endif
    /*
    if((BStatus & M_DOWNSTREAM_COUNT)> 6)
    {
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("Down Stream Count %d is over maximum supported number 6,fail.\n",(BStatus & M_DOWNSTREAM_COUNT)));
#endif
        return ER_FAIL ;
    }
    */

    HDCP_GetBKSV(BKSV);
#ifdef Debug_message
//    HDMITX_DEBUG_PRINTF(("BKSV %02X %02X %02X %02X %02X\n",BKSV[0],BKSV[1],BKSV[2],BKSV[3],BKSV[4]));
#endif
    for(TimeOut = 0, ucdata = 0 ; TimeOut < 5 ; TimeOut ++)
    {
        ucdata += countbit(BKSV[TimeOut]);
    }
    if( ucdata != 20 ) return ER_FAIL ;



    Switch_HDMITX_Bank(0); // switch bank action should start on direct register writting of each function.

    // 2006/08/11 added by jjtseng
    // enable HDCP on CPDired enabled.
    HDMITX_AndREG_Byte(REG_TX_SW_RST,~(B_HDCP_RST));
    //~jjtseng 2006/08/11

//    if(BCaps & B_CAP_HDCP_1p1)
//    {
#ifdef Debug_message
//        HDMITX_DEBUG_PRINTF(("RX support HDCP 1.1\n"));
#endif
//        HDMITX_WriteI2C_Byte(REG_TX_HDCP_DESIRE,B_ENABLE_HDPC11|B_CPDESIRE);
//    }
//    else
//    {
#ifdef Debug_message
//        HDMITX_DEBUG_PRINTF(("RX not support HDCP 1.1\n"));
#endif
    HDMITX_WriteI2C_Byte(REG_TX_HDCP_DESIRE,0x08|B_CPDESIRE);
//    }


    // HDMITX_WriteI2C_Byte(REG_TX_INT_CLR0,B_CLR_AUTH_DONE|B_CLR_AUTH_FAIL|B_CLR_KSVLISTCHK);
    // HDMITX_WriteI2C_Byte(REG_TX_INT_CLR1,0); // don't clear other settings.
    // ucdata = HDMITX_ReadI2C_Byte(REG_TX_SYS_STATUS);
    // ucdata = (ucdata & M_CTSINTSTEP) | B_INTACTDONE ;
    // HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,ucdata); // clear action.

    // HDMITX_AndREG_Byte(REG_TX_INT_MASK2,~(B_AUTH_FAIL_MASK|B_T_AUTH_DONE_MASK));    // enable GetBCaps Interrupt
    HDCP_ClearAuthInterrupt();
#ifdef Debug_message
//    HDMITX_DEBUG_PRINTF(("int2 = %02X DDC_Status = %02X\n",HDMITX_ReadI2C_Byte(REG_TX_INT_STAT2),HDMITX_ReadI2C_Byte(REG_TX_DDC_STATUS)));
#endif

    HDCP_GenerateAn();
    HDMITX_WriteI2C_Byte(REG_TX_LISTCTRL,0);
    Instance[0].bAuthenticated = FALSE ;
    //DumpCatHDMITXReg();
    if((BCaps & B_CAP_HDMI_REPEATER) == 0)
    {
        HDCP_Auth_Fire();
        // wait for status ;

        for(TimeOut = 250 ; TimeOut > 0 ; TimeOut --)
        {
            delay1ms(5); // delay 1ms
            ucdata = HDMITX_ReadI2C_Byte(REG_TX_AUTH_STAT);
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("reg46 = %02x reg16 = %02x reg1A = %02x\n",(WORD)ucdata,(WORD)HDMITX_ReadI2C_Byte(0x16),(WORD)HDMITX_ReadI2C_Byte(0x1A)));
#endif
            if(ucdata & B_T_AUTH_DONE)
            {
                Instance[0].bAuthenticated = TRUE ;
                break ;
            }

            ucdata = HDMITX_ReadI2C_Byte(REG_TX_INT_STAT2);
            if(ucdata & B_INT_AUTH_FAIL)
            {
                /*
                HDMITX_WriteI2C_Byte(REG_TX_INT_CLR0,B_CLR_AUTH_FAIL);
                HDMITX_WriteI2C_Byte(REG_TX_INT_CLR1,0);
                HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,B_INTACTDONE);
                HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,0);
                */
#ifdef Debug_message
                HDMITX_DEBUG_PRINTF(("HDCP_Authenticate(): Authenticate fail\n"));
#endif
                Instance[0].bAuthenticated = FALSE ;
                return ER_FAIL ;
            }
        }

        if(TimeOut == 0)
        {
#ifdef Debug_message
             HDMITX_DEBUG_PRINTF(("HDCP_Authenticate(): Time out. return fail\n"));
#endif
             Instance[0].bAuthenticated = FALSE ;
             return ER_FAIL ;
        }
        return ER_SUCCESS ;
    }

    return HDCP_Authenticate_Repeater();
}

//////////////////////////////////////////////////////////////////////
// Function: HDCP_VerifyIntegration
// Parameter: N/A
// Return: ER_SUCCESS if success,if AUTH_FAIL interrupt status,return fail.
// Remark: no used now.
// Side-Effect:
//////////////////////////////////////////////////////////////////////

static SYS_STATUS
HDCP_VerifyIntegration()
{
    // if any interrupt issued a Auth fail,returned the Verify Integration fail.

    if(HDMITX_ReadI2C_Byte(REG_TX_INT_STAT1) & B_INT_AUTH_FAIL)
    {
        HDCP_ClearAuthInterrupt();
        Instance[0].bAuthenticated = FALSE ;
        return ER_FAIL ;
    }

    if(Instance[0].bAuthenticated == TRUE)
    {
        return ER_SUCCESS ;
    }

    return ER_FAIL ;
}

//////////////////////////////////////////////////////////////////////
// Function: HDCP_Authenticate_Repeater
// Parameter: BCaps and BStatus
// Return: ER_SUCCESS if success,if AUTH_FAIL interrupt status,return fail.
// Remark:
// Side-Effect: as Authentication
//////////////////////////////////////////////////////////////////////
//static _IDATA BYTE KSVList[32] ;
static _IDATA  BYTE Vr[20] ;
static _IDATA  BYTE M0[8] ;
//static _XDATA BYTE SHABuff[64] ;
extern _XDATA  BYTE cBuf[128] ;

static _IDATA  BYTE V[20] ;
#define SHA_BUFF_COUNT 17
static   ULONG w[SHA_BUFF_COUNT];
static _IDATA  ULONG sha[5] ;

#define rol(x,y) (((x) << (y)) | (((ULONG)x) >> (32-y)))

static void
HDCP_CancelRepeaterAuthenticate()
{
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("HDCP_CancelRepeaterAuthenticate"));
#endif
    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERDDC|B_MASTERHOST);
    AbortDDC();
    HDMITX_WriteI2C_Byte(REG_TX_LISTCTRL,B_LISTFAIL|B_LISTDONE);
    HDCP_ClearAuthInterrupt();
    HDMITX_WriteI2C_Byte(REG_TX_SW_RST,B_HDCP_RST|HDMITX_ReadI2C_Byte(REG_TX_SW_RST));
}

static void
HDCP_ResumeRepeaterAuthenticate()
{
    HDMITX_WriteI2C_Byte(REG_TX_LISTCTRL,B_LISTDONE);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERHDCP);
}


static SYS_STATUS
HDCP_GetKSVList(BYTE *pKSVList,BYTE cDownStream)
{
    BYTE TimeOut = 100 ;
    BYTE ucdata ;

    if( cDownStream == 0 )
    {
        return ER_SUCCESS ;
    }

    if( /* cDownStream == 0 || */ pKSVList == NULL)
    {
        return ER_FAIL ;
    }

    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERHOST);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_HEADER,0x74);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_REQOFF,0x43);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_REQCOUNT,cDownStream * 5);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_CMD,CMD_DDC_SEQ_BURSTREAD);


    for(TimeOut = 200 ; TimeOut > 0 ; TimeOut --)
    {

        ucdata = HDMITX_ReadI2C_Byte(REG_TX_DDC_STATUS);
        if(ucdata & B_DDC_DONE)
        {
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("HDCP_GetKSVList(): DDC Done.\n"));
#endif
            break ;
        }

        if(ucdata & B_DDC_ERROR)
        {
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("HDCP_GetKSVList(): DDC Fail by REG_TX_DDC_STATUS = %x.\n",(WORD)ucdata));
#endif
            return ER_FAIL ;
        }
        delay1ms(5);
    }

    if(TimeOut == 0)
    {
        return ER_FAIL ;
    }
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("HDCP_GetKSVList(): KSV"));
#endif
    for(TimeOut = 0 ; TimeOut < cDownStream * 5 ; TimeOut++)
    {
        pKSVList[TimeOut] = HDMITX_ReadI2C_Byte(REG_TX_DDC_READFIFO);
#ifdef Debug_message
//        HDMITX_DEBUG_PRINTF((" %02X",pKSVList[TimeOut]));
#endif
    }
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("\n"));
#endif
    return ER_SUCCESS ;
}

static SYS_STATUS
HDCP_GetVr(BYTE *pVr)
{
    BYTE TimeOut  ;
    BYTE ucdata ;

    if(pVr == NULL)
    {
        return ER_FAIL ;
    }

    HDMITX_WriteI2C_Byte(REG_TX_DDC_MASTER_CTRL,B_MASTERHOST);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_HEADER,0x74);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_REQOFF,0x20);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_REQCOUNT,20);
    HDMITX_WriteI2C_Byte(REG_TX_DDC_CMD,CMD_DDC_SEQ_BURSTREAD);


    for(TimeOut = 200 ; TimeOut > 0 ; TimeOut --)
    {
        ucdata = HDMITX_ReadI2C_Byte(REG_TX_DDC_STATUS);
        if(ucdata & B_DDC_DONE)
        {
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("HDCP_GetVr(): DDC Done.\n"));
#endif
            break ;
        }

        if(ucdata & B_DDC_ERROR)
        {
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("HDCP_GetVr(): DDC fail by REG_TX_DDC_STATUS = %x.\n",(WORD)ucdata));
#endif
            return ER_FAIL ;
        }
        delay1ms(5);
    }

    if(TimeOut == 0)
    {
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("HDCP_GetVr(): DDC fail by timeout.%x\n",(WORD)ucdata));
#endif
        return ER_FAIL ;
    }

    Switch_HDMITX_Bank(0);

    for(TimeOut = 0 ; TimeOut < 5 ; TimeOut++)
    {
        HDMITX_WriteI2C_Byte(REG_TX_SHA_SEL ,TimeOut);
        pVr[TimeOut*4+3]  = (ULONG)HDMITX_ReadI2C_Byte(REG_TX_SHA_RD_BYTE1);
        pVr[TimeOut*4+2] = (ULONG)HDMITX_ReadI2C_Byte(REG_TX_SHA_RD_BYTE2);
        pVr[TimeOut*4+1] = (ULONG)HDMITX_ReadI2C_Byte(REG_TX_SHA_RD_BYTE3);
        pVr[TimeOut*4] = (ULONG)HDMITX_ReadI2C_Byte(REG_TX_SHA_RD_BYTE4);
#ifdef Debug_message
        //HDMITX_DEBUG_PRINTF(("Read V' = %02X %02X %02X %02X\n",(WORD)pVr[TimeOut*4],(WORD)pVr[TimeOut*4+1],(WORD)pVr[TimeOut*4+2],(WORD)pVr[TimeOut*4+3]));
#endif
    }

    return ER_SUCCESS ;
}

static SYS_STATUS
HDCP_GetM0(BYTE *pM0)
{
    int i ;

    if(!pM0)
    {
        return ER_FAIL ;
    }

    HDMITX_WriteI2C_Byte(REG_TX_SHA_SEL,5); // read m0[31:0] from reg51~reg54
    pM0[0] = HDMITX_ReadI2C_Byte(REG_TX_SHA_RD_BYTE1);
    pM0[1] = HDMITX_ReadI2C_Byte(REG_TX_SHA_RD_BYTE2);
    pM0[2] = HDMITX_ReadI2C_Byte(REG_TX_SHA_RD_BYTE3);
    pM0[3] = HDMITX_ReadI2C_Byte(REG_TX_SHA_RD_BYTE4);
    HDMITX_WriteI2C_Byte(REG_TX_SHA_SEL,0); // read m0[39:32] from reg55
    pM0[4] = HDMITX_ReadI2C_Byte(REG_TX_AKSV_RD_BYTE5);
    HDMITX_WriteI2C_Byte(REG_TX_SHA_SEL,1); // read m0[47:40] from reg55
    pM0[5] = HDMITX_ReadI2C_Byte(REG_TX_AKSV_RD_BYTE5);
    HDMITX_WriteI2C_Byte(REG_TX_SHA_SEL,2); // read m0[55:48] from reg55
    pM0[6] = HDMITX_ReadI2C_Byte(REG_TX_AKSV_RD_BYTE5);
    HDMITX_WriteI2C_Byte(REG_TX_SHA_SEL,3); // read m0[63:56] from reg55
    pM0[7] = HDMITX_ReadI2C_Byte(REG_TX_AKSV_RD_BYTE5);
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("M[] ="));
#endif
    for(i = 0 ; i < 8 ; i++){
//        HDMITX_DEBUG_PRINTF(("0x%02x,",pM0[i]));
    }
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("\n"));
#endif
    return ER_SUCCESS ;
}


void SHATransform(ULONG * h)
{
    ULONG    t;
    ULONG tmp;


    h[0]=0x67452301;
    h[1]=0xefcdab89;
    h[2]=0x98badcfe;
    h[3]=0x10325476;
    h[4]=0xc3d2e1f0;

    for( t = 0 ; t < 80 ; t++ )
    {
        if((t>=16)&&(t<80))
        {
            tmp=w[(t+SHA_BUFF_COUNT-3)%SHA_BUFF_COUNT] ^ w[(t+SHA_BUFF_COUNT-8)%SHA_BUFF_COUNT] ^ w[(t+SHA_BUFF_COUNT-14)%SHA_BUFF_COUNT] ^ w[(t+SHA_BUFF_COUNT-16)%SHA_BUFF_COUNT];
            w[t%SHA_BUFF_COUNT]=rol(tmp,1);
        }

        if((t>=0)&&(t<20))
        {
            tmp=rol(h[0],5) + ((h[1] & h[2]) | (h[3] & ~h[1])) + h[4] + w[t%SHA_BUFF_COUNT] + 0x5a827999;
#ifdef Debug_message
        // TXHDCP_DEBUG_PRINT(("%08lX %08lX %08lX %08lX %08lX\n",h[0],h[1],h[2],h[3],h[4]));
#endif
            h[4]=h[3];
            h[3]=h[2];
            h[2]=rol(h[1],30);
            h[1]=h[0];
            h[0]=tmp;
        }
        if((t>=20)&&(t<40))
        {
            tmp=rol(h[0],5) + (h[1] ^ h[2] ^ h[3]) + h[4] + w[t%SHA_BUFF_COUNT] + 0x6ed9eba1;
#ifdef Debug_message
        // TXHDCP_DEBUG_PRINT(("%08lX %08lX %08lX %08lX %08lX\n",h[0],h[1],h[2],h[3],h[4]));
#endif
            h[4]=h[3];
            h[3]=h[2];
            h[2]=rol(h[1],30);
            h[1]=h[0];
            h[0]=tmp;
        }
        if((t>=40)&&(t<60))
        {
            tmp=rol(h[0],5) + ((h[1] & h[2]) | (h[1] & h[3]) | (h[2] & h[3])) + h[4] + w[t%SHA_BUFF_COUNT] + 0x8f1bbcdc;
#ifdef Debug_message
        //TXHDCP_DEBUG_PRINT(("%08lX %08lX %08lX %08lX %08lX\n",h[0],h[1],h[2],h[3],h[4]));
#endif
            h[4]=h[3];
            h[3]=h[2];
            h[2]=rol(h[1],30);
            h[1]=h[0];
            h[0]=tmp;
        }
        if((t>=60)&&(t<80))
        {
            tmp=rol(h[0],5) + (h[1] ^ h[2] ^ h[3]) + h[4] + w[t%SHA_BUFF_COUNT] + 0xca62c1d6;
#ifdef Debug_message
         //TXHDCP_DEBUG_PRINT(("%08lX %08lX %08lX %08lX %08lX\n",h[0],h[1],h[2],h[3],h[4]));
#endif
            h[4]=h[3];
            h[3]=h[2];
            h[2]=rol(h[1],30);
            h[1]=h[0];
            h[0]=tmp;
        }
    }
#ifdef Debug_message
    //TXHDCP_DEBUG_PRINT(("%08lX %08lX %08lX %08lX %08lX\n",h[0],h[1],h[2],h[3],h[4]));
#endif
    h[0] +=0x67452301;
    h[1] +=0xefcdab89;
    h[2] +=0x98badcfe;
    h[3] +=0x10325476;
    h[4] +=0xc3d2e1f0;

#ifdef Debug_message
    //TXHDCP_DEBUG_PRINT(("%08lX %08lX %08lX %08lX %08lX\n",h[0],h[1],h[2],h[3],h[4]));
#endif
}


/* ----------------------------------------------------------------------
 * Outer SHA algorithm: take an arbitrary length byte string,
 * convert it into 16-word blocks with the prescribed padding at
 * the end,and pass those blocks to the core SHA algorithm.
 */


void SHA_Simple(void *p,LONG len,BYTE *output)
{
    // SHA_State s;
    WORD i,t;
    ULONG c;
    BYTE *pBuff=p;
    for(i=0;i < len;i++)
    {
        t=i/4;
        if(i%4==0)
        {
            w[t]=0;
        }
        c=pBuff[i];
        c <<=(3-(i%4))*8;
        w[t] |=c;
    }
    t=i/4;
    if(i%4==0)
    {
        w[t]=0;
    }
    //c=0x80 << ((3-i%4)*24);
    c=0x80;
    c <<=((3-i%4)*8);
    w[t]|=c;t++;
    for(; t < 15;t++)
    {
        w[t]=0;
    }
    w[15]=len*8;
    for(i=0;i < len;i++)
    {
        t=i/4;
#ifdef Debug_message
        //HDMITX_DEBUG_PRINTF(("pBuff[%d]=%X,w[%d]=%08lX\n",(WORD)i,(WORD)pBuff[i],t,w[t]));
#endif
    }

    SHATransform(sha);
    for(i=0;i < 5;i++)
    {
        output[i*4]=(BYTE)((sha[i]>>24)&0xFF);
        output[i*4+1]=(BYTE)((sha[i]>>16)&0xFF);
        output[i*4+2]=(BYTE)((sha[i]>>8)&0xFF);
        output[i*4+3]=(BYTE)(sha[i]&0xFF);
#ifdef Debug_message
        //HDMITX_DEBUG_PRINTF(("Read V' = %02X %02X %02X %02X\n",(WORD)output[i*4],(WORD)output[i*4+1],(WORD)output[i*4+2],(WORD)output[i*4+3]));
#endif
    }
}


static SYS_STATUS
HDCP_CheckSHA(BYTE pM0[],USHORT BStatus,BYTE pKSVList[],int cDownStream,BYTE Vr[])
{
    int i,n ;
    for(i = 0 ; i < cDownStream*5 ; i++)
    {
        cBuf[i] = pKSVList[i] ;
    }
    i = cDownStream*5;
    cBuf[i++] = BStatus & 0xFF ;
    cBuf[i++] = (BStatus>>8) & 0xFF ;
    for(n = 0 ; n < 8 ; n++,i++)
    {
        cBuf[i] = pM0[n] ;
    }
    n = i ;
    // SHABuff[i++] = 0x80 ; // end mask
    for(; i < 64 ; i++)
    {
        cBuf[i] = 0 ;
    }
    // n = cDownStream * 5 + 2 /* for BStatus */ + 8 /* for M0 */ ;
    // n *= 8 ;
    // SHABuff[62] = (n>>8) & 0xff ;
    // SHABuff[63] = (n>>8) & 0xff ;
    /*
#ifdef Debug_message
    for(i = 0 ; i < 64 ; i++)
    {
        if(i % 16 == 0)
        {
            HDMITX_DEBUG_PRINTF(("SHA[]: "));
        }
        HDMITX_DEBUG_PRINTF((" %02X",(WORD)cBuf[i]));
        if((i%16)==15)
        {
            HDMITX_DEBUG_PRINTF(("\n"));
        }
    }
#endif
    */
    SHA_Simple(cBuf,n,V);
    /*
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("V[] ="));
    for(i = 0 ; i < 20 ; i++)
    {
        HDMITX_DEBUG_PRINTF((" %02X",(WORD)V[i]));
    }
    HDMITX_DEBUG_PRINTF(("\nVr[] ="));
    for(i = 0 ; i < 20 ; i++)
    {
        HDMITX_DEBUG_PRINTF((" %02X",(WORD)Vr[i]));
    }
#endif
*/
    for(i = 0 ; i < 20 ; i++)
    {
        if(V[i] != Vr[i])
        {
            return ER_FAIL ;
        }
    }
    return ER_SUCCESS ;
}

static SYS_STATUS
HDCP_Authenticate_Repeater()
{
    BYTE uc ;
//    BYTE revoked ;
    // int i ;
    BYTE cDownStream ;

    BYTE BCaps;
    USHORT BStatus ;
    USHORT TimeOut ;
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("Authentication for repeater\n"));
#endif
    // emily add for test,abort HDCP
    // 2007/10/01 marked by jj_tseng@chipadvanced.com
    // HDMITX_WriteI2C_Byte(0x20,0x00);
    // HDMITX_WriteI2C_Byte(0x04,0x01);
    // HDMITX_WriteI2C_Byte(0x10,0x01);
    // HDMITX_WriteI2C_Byte(0x15,0x0F);
    // delay1ms(100);
    // HDMITX_WriteI2C_Byte(0x04,0x00);
    // HDMITX_WriteI2C_Byte(0x10,0x00);
    // HDMITX_WriteI2C_Byte(0x20,0x01);
    // delay1ms(100);
    // test07 = HDMITX_ReadI2C_Byte(0x7);
    // test06 = HDMITX_ReadI2C_Byte(0x6);
    // test08 = HDMITX_ReadI2C_Byte(0x8);
    //~jj_tseng@chipadvanced.com
    // end emily add for test
    //////////////////////////////////////
    // Authenticate Fired
    //////////////////////////////////////

    HDCP_GetBCaps(&BCaps,&BStatus);
    delay1ms(2);
    HDCP_Auth_Fire();
    delay1ms(550); // emily add for test

    for(TimeOut = 250*6 ; TimeOut > 0 ; TimeOut --)
    {

        uc = HDMITX_ReadI2C_Byte(REG_TX_INT_STAT1);
        if(uc & B_INT_DDC_BUS_HANG)
        {
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("DDC Bus hang\n"));
#endif
            goto HDCP_Repeater_Fail ;
        }

        uc = HDMITX_ReadI2C_Byte(REG_TX_INT_STAT2);

        if(uc & B_INT_AUTH_FAIL)
        {
            /*
            HDMITX_WriteI2C_Byte(REG_TX_INT_CLR0,B_CLR_AUTH_FAIL);
            HDMITX_WriteI2C_Byte(REG_TX_INT_CLR1,0);
            HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,B_INTACTDONE);
            HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,0);
            */
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("HDCP_Authenticate_Repeater(): B_INT_AUTH_FAIL.\n"));
#endif
            goto HDCP_Repeater_Fail ;
        }
        // emily add for test
        // test =(HDMITX_ReadI2C_Byte(0x7)&0x4)>>2 ;
        if(uc & B_INT_KSVLIST_CHK)
        {
            HDMITX_WriteI2C_Byte(REG_TX_INT_CLR0,B_CLR_KSVLISTCHK);
            HDMITX_WriteI2C_Byte(REG_TX_INT_CLR1,0);
            HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,B_INTACTDONE);
            HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,0);
#ifdef Debug_message
            HDMITX_DEBUG_PRINTF(("B_INT_KSVLIST_CHK\n"));
#endif
            break ;
        }

        if(HDCP_GetBCaps(&BCaps,&BStatus) == ER_FAIL)
            {goto HDCP_Repeater_Fail ;}
        if(BCaps & B_CAP_KSV_FIFO_RDY)
            {break ;}

        delay1ms(5);
    }

    if(TimeOut == 0)
    {
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("Time out for wait KSV List checking interrupt\n"));
#endif
        goto HDCP_Repeater_Fail ;
    }

    ///////////////////////////////////////
    // clear KSVList check interrupt.
    ///////////////////////////////////////

    for(TimeOut = (500) ; TimeOut > 0 ; TimeOut --)
    {
    /*
#ifdef Debug_message
        if((TimeOut % 100) == 0)
        {
            HDMITX_DEBUG_PRINTF(("Wait KSV FIFO Ready %d\n",(WORD)TimeOut));
        }
#endif
        */
        if(HDCP_GetBCaps(&BCaps,&BStatus) == ER_FAIL)
        {
#ifdef Debug_message
            //HDMITX_DEBUG_PRINTF(("Get BCaps fail\n"));
#endif
            goto HDCP_Repeater_Fail ;
        }
#ifdef Debug_message
        //HDMITX_DEBUG_PRINTF(("BCaps = %X , BStatus = %X\n",(WORD)BCaps,BStatus));
#endif
        if(BCaps & B_CAP_KSV_FIFO_RDY)
        {
#ifdef Debug_message
             //HDMITX_DEBUG_PRINTF(("FIFO Ready\n"));
#endif
             break ;
        }
        delay1ms(5);

    }

    if(TimeOut == 0)
    {
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("Get KSV FIFO ready TimeOut\n"));
#endif
        goto HDCP_Repeater_Fail ;
    }
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("Wait timeout = %d\n",(WORD)TimeOut));
#endif
    ClearDDCFIFO();
    GenerateDDCSCLK();
    cDownStream =  (BStatus & M_DOWNSTREAM_COUNT);

    if(/*cDownStream == 0 ||*/ cDownStream > 6 || BStatus & (B_MAX_CASCADE_EXCEEDED|B_DOWNSTREAM_OVER))
    {
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("Invalid Down stream count,fail\n"));
#endif
        goto HDCP_Repeater_Fail ;
    }


    if(HDCP_GetKSVList(&cBuf[64],cDownStream) == ER_FAIL)
    {
        goto HDCP_Repeater_Fail ;
    }

#if 0
    for(i = 0 ; i < cDownStream ; i++)
    {
        revoked=FALSE ; uc = 0 ;
        for( TimeOut = 0 ; TimeOut < 5 ; TimeOut++ )
        {
            // check bit count
            uc += countbit(KSVList[i*5+TimeOut]);
        }
        if( uc != 20 ) revoked = TRUE ;

        if(revoked)
        {
#ifdef Debug_message
//            HDMITX_DEBUG_PRINTF(("KSVFIFO[%d] = %02X %02X %02X %02X %02X is revoked\n",i,KSVList[i*5],KSVList[i*5+1],KSVList[i*5+2],KSVList[i*5+3],KSVList[i*5+4]));
#endif
             goto HDCP_Repeater_Fail ;
        }
    }
#endif


    if(HDCP_GetVr(Vr) == ER_FAIL)
    {
        goto HDCP_Repeater_Fail ;
    }

    if(HDCP_GetM0(M0) == ER_FAIL)
    {
        goto HDCP_Repeater_Fail ;
    }

    // do check SHA
    if(HDCP_CheckSHA(M0,BStatus,&cBuf[64],cDownStream,Vr) == ER_FAIL)
    {
        goto HDCP_Repeater_Fail ;
    }


    HDCP_ResumeRepeaterAuthenticate();
    Instance[0].bAuthenticated = TRUE ;
    return ER_SUCCESS ;

HDCP_Repeater_Fail:
    HDCP_CancelRepeaterAuthenticate();
    return ER_FAIL ;
}

//////////////////////////////////////////////////////////////////////
// Function: HDCP_ResumeAuthentication
// Parameter: N/A
// Return: N/A
// Remark: called by interrupt handler to restart Authentication and Encryption.
// Side-Effect: as Authentication and Encryption.
//////////////////////////////////////////////////////////////////////

static void
HDCP_ResumeAuthentication()
{
    SetAVMute(TRUE);
    if(HDCP_Authenticate() == ER_SUCCESS)
    {
        HDCP_EnableEncryption();
    }
    SetAVMute(FALSE);
}



#endif // SUPPORT_HDCP


static void
ENABLE_NULL_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_NULL_CTRL,B_ENABLE_PKT|B_REPEAT_PKT);
}


static void
ENABLE_ACP_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_ACP_CTRL,B_ENABLE_PKT|B_REPEAT_PKT);
}


static void
ENABLE_ISRC1_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_ISRC1_CTRL,B_ENABLE_PKT|B_REPEAT_PKT);
}


static void
ENABLE_ISRC2_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_ISRC2_CTRL,B_ENABLE_PKT|B_REPEAT_PKT);
}


static void
ENABLE_AVI_INFOFRM_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_AVI_INFOFRM_CTRL,B_ENABLE_PKT|B_REPEAT_PKT);
}


static void
ENABLE_AUD_INFOFRM_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_AUD_INFOFRM_CTRL,B_ENABLE_PKT|B_REPEAT_PKT);
}


static void
ENABLE_SPD_INFOFRM_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_SPD_INFOFRM_CTRL,B_ENABLE_PKT|B_REPEAT_PKT);
}


static void
ENABLE_MPG_INFOFRM_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_MPG_INFOFRM_CTRL,B_ENABLE_PKT|B_REPEAT_PKT);
}

static void
DISABLE_NULL_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_NULL_CTRL,0);
}


static void
DISABLE_ACP_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_ACP_CTRL,0);
}


static void
DISABLE_ISRC1_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_ISRC1_CTRL,0);
}


static void
DISABLE_ISRC2_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_ISRC2_CTRL,0);
}


static void
DISABLE_AVI_INFOFRM_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_AVI_INFOFRM_CTRL,0);
}


static void
DISABLE_AUD_INFOFRM_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_AUD_INFOFRM_CTRL,0);
}


static void
DISABLE_SPD_INFOFRM_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_SPD_INFOFRM_CTRL,0);
}


static void
DISABLE_MPG_INFOFRM_PKT()
{

    HDMITX_WriteI2C_Byte(REG_TX_MPG_INFOFRM_CTRL,0);
}



//////////////////////////////////////////////////////////////////////
// Function: SetAVIInfoFrame()
// Parameter: pAVIInfoFrame - the pointer to HDMI AVI Infoframe ucData
// Return: N/A
// Remark: Fill the AVI InfoFrame ucData,and count checksum,then fill into
//         AVI InfoFrame registers.
// Side-Effect: N/A
//////////////////////////////////////////////////////////////////////

static SYS_STATUS
SetAVIInfoFrame(AVI_InfoFrame *pAVIInfoFrame)
{
    int i ;
    byte ucData ;

    if(!pAVIInfoFrame)
    {
        return ER_FAIL ;
    }

    Switch_HDMITX_Bank(1);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB1,pAVIInfoFrame->pktbyte.AVI_DB[0]);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB2,pAVIInfoFrame->pktbyte.AVI_DB[1]);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB3,pAVIInfoFrame->pktbyte.AVI_DB[2]);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB4,pAVIInfoFrame->pktbyte.AVI_DB[3]);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB5,pAVIInfoFrame->pktbyte.AVI_DB[4]);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB6,pAVIInfoFrame->pktbyte.AVI_DB[5]);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB7,pAVIInfoFrame->pktbyte.AVI_DB[6]);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB8,pAVIInfoFrame->pktbyte.AVI_DB[7]);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB9,pAVIInfoFrame->pktbyte.AVI_DB[8]);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB10,pAVIInfoFrame->pktbyte.AVI_DB[9]);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB11,pAVIInfoFrame->pktbyte.AVI_DB[10]);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB12,pAVIInfoFrame->pktbyte.AVI_DB[11]);
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_DB13,pAVIInfoFrame->pktbyte.AVI_DB[12]);
    for(i = 0,ucData = 0; i < 13 ; i++)
    {
        ucData -= pAVIInfoFrame->pktbyte.AVI_DB[i] ;
    }
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF(("SetAVIInfo(): "));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB1)));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB2)));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB3)));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB4)));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB5)));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB6)));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB7)));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB8)));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB9)));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB10)));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB11)));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB12)));
    //HDMITX_DEBUG_PRINTF(("%02X ",HDMITX_ReadI2C_Byte(REG_TX_AVIINFO_DB13)));
    HDMITX_DEBUG_PRINTF(("\n"));
#endif
    ucData -= 0x80+AVI_INFOFRAME_VER+AVI_INFOFRAME_TYPE+AVI_INFOFRAME_LEN ;
    HDMITX_WriteI2C_Byte(REG_TX_AVIINFO_SUM,ucData);


    Switch_HDMITX_Bank(0);
    ENABLE_AVI_INFOFRM_PKT();
    return ER_SUCCESS ;
}

//////////////////////////////////////////////////////////////////////
// Function: SetAudioInfoFrame()
// Parameter: pAudioInfoFrame - the pointer to HDMI Audio Infoframe ucData
// Return: N/A
// Remark: Fill the Audio InfoFrame ucData,and count checksum,then fill into
//         Audio InfoFrame registers.
// Side-Effect: N/A
//////////////////////////////////////////////////////////////////////

static SYS_STATUS
SetAudioInfoFrame(Audio_InfoFrame *pAudioInfoFrame)
{
//    int i ;
    BYTE uc ;

    if(!pAudioInfoFrame)
    {
        return ER_FAIL ;
    }

    Switch_HDMITX_Bank(1);
    uc = 0x80-(AUDIO_INFOFRAME_VER+AUDIO_INFOFRAME_TYPE+AUDIO_INFOFRAME_LEN );
    HDMITX_WriteI2C_Byte(REG_TX_PKT_AUDINFO_CC,pAudioInfoFrame->pktbyte.AUD_DB[0]);
    uc -= HDMITX_ReadI2C_Byte(REG_TX_PKT_AUDINFO_CC); uc &= 0xFF ;
    HDMITX_WriteI2C_Byte(REG_TX_PKT_AUDINFO_SF,pAudioInfoFrame->pktbyte.AUD_DB[1]);
    uc -= HDMITX_ReadI2C_Byte(REG_TX_PKT_AUDINFO_SF); uc &= 0xFF ;
    HDMITX_WriteI2C_Byte(REG_TX_PKT_AUDINFO_CA,pAudioInfoFrame->pktbyte.AUD_DB[3]);
    uc -= HDMITX_ReadI2C_Byte(REG_TX_PKT_AUDINFO_CA); uc &= 0xFF ;

    HDMITX_WriteI2C_Byte(REG_TX_PKT_AUDINFO_DM_LSV,pAudioInfoFrame->pktbyte.AUD_DB[4]);
    uc -= HDMITX_ReadI2C_Byte(REG_TX_PKT_AUDINFO_DM_LSV); uc &= 0xFF ;

    HDMITX_WriteI2C_Byte(REG_TX_PKT_AUDINFO_SUM,uc);


    Switch_HDMITX_Bank(0);
    ENABLE_AUD_INFOFRM_PKT();
    return ER_SUCCESS ;
}

#if 0
// //////////////////////////////////////////////////////////////////////
// // Function: SetSPDInfoFrame()
// // Parameter: pSPDInfoFrame - the pointer to HDMI SPD Infoframe ucData
// // Return: N/A
// // Remark: Fill the SPD InfoFrame ucData,and count checksum,then fill into
// //         SPD InfoFrame registers.
// // Side-Effect: N/A
// //////////////////////////////////////////////////////////////////////
//
// static SYS_STATUS
// SetSPDInfoFrame(SPD_InfoFrame *pSPDInfoFrame)
// {
//     int i ;
//     BYTE ucData ;
//
//     if(!pSPDInfoFrame)
//     {
//         return ER_FAIL ;
//     }
//
//     Switch_HDMITX_Bank(1);
//     for(i = 0,ucData = 0 ; i < 25 ; i++)
//     {
//         ucData -= pSPDInfoFrame->pktbyte.SPD_DB[i] ;
//         HDMITX_WriteI2C_Byte(REG_TX_PKT_SPDINFO_PB1+i,pSPDInfoFrame->pktbyte.SPD_DB[i]);
//     }
//     ucData -= 0x80+SPD_INFOFRAME_VER+SPD_INFOFRAME_TYPE+SPD_INFOFRAME_LEN ;
//     HDMITX_WriteI2C_Byte(REG_TX_PKT_SPDINFO_SUM,ucData); // checksum
//     Switch_HDMITX_Bank(0);
//     ENABLE_SPD_INFOFRM_PKT();
//     return ER_SUCCESS ;
// }
//
// //////////////////////////////////////////////////////////////////////
// // Function: SetMPEGInfoFrame()
// // Parameter: pMPEGInfoFrame - the pointer to HDMI MPEG Infoframe ucData
// // Return: N/A
// // Remark: Fill the MPEG InfoFrame ucData,and count checksum,then fill into
// //         MPEG InfoFrame registers.
// // Side-Effect: N/A
// //////////////////////////////////////////////////////////////////////
//
// static SYS_STATUS
// SetMPEGInfoFrame(MPEG_InfoFrame *pMPGInfoFrame)
// {
//     int i ;
//     BYTE ucData ;
//
//     if(!pMPGInfoFrame)
//     {
//         return ER_FAIL ;
//     }
//
//     Switch_HDMITX_Bank(1);
//
//     HDMITX_WriteI2C_Byte(REG_TX_PKT_MPGINFO_FMT,pMPGInfoFrame->info.FieldRepeat|(pMPGInfoFrame->info.MpegFrame<<1));
//     HDMITX_WriteI2C_Byte(REG_TX_PKG_MPGINFO_DB0,pMPGInfoFrame->pktbyte.MPG_DB[0]);
//     HDMITX_WriteI2C_Byte(REG_TX_PKG_MPGINFO_DB1,pMPGInfoFrame->pktbyte.MPG_DB[1]);
//     HDMITX_WriteI2C_Byte(REG_TX_PKG_MPGINFO_DB2,pMPGInfoFrame->pktbyte.MPG_DB[2]);
//     HDMITX_WriteI2C_Byte(REG_TX_PKG_MPGINFO_DB3,pMPGInfoFrame->pktbyte.MPG_DB[3]);
//
//     for(ucData = 0,i = 0 ; i < 5 ; i++)
//     {
//         ucData -= pMPGInfoFrame->pktbyte.MPG_DB[i] ;
//     }
//     ucData -= 0x80+MPEG_INFOFRAME_VER+MPEG_INFOFRAME_TYPE+MPEG_INFOFRAME_LEN ;
//
//     HDMITX_WriteI2C_Byte(REG_TX_PKG_MPGINFO_SUM,ucData);
//
//     Switch_HDMITX_Bank(0);
//     ENABLE_SPD_INFOFRM_PKT();
//
//     return ER_SUCCESS ;
// }
#endif

// 2008/08/18 added by jj_tseng@chipadvanced.com
/////////////////////////////////////////////////////////////////////////////////////
// IT626X part
/////////////////////////////////////////////////////////////////////////////////////
void setIT626X_ChStat(BYTE ucIEC60958ChStat[]);
void setIT626X_UpdateChStatFs(ULONG Fs);
void setIT626X_LPCMAudio(BYTE AudioSrcNum, BYTE AudSWL, BOOL bSPDIF);
void setIT626X_NLPCMAudio(void);
void setIT626X_HBRAudio(BOOL bSPDIF);
void setIT626X_DSDAudio(void);


void
setIT626X_ChStat(BYTE ucIEC60958ChStat[])
{
    BYTE uc ;

    Switch_HDMITX_Bank(1);
    uc = (ucIEC60958ChStat[0] <<1)& 0x7C ;
    HDMITX_WriteI2C_Byte(REG_TX_AUDCHST_MODE,uc);
    HDMITX_WriteI2C_Byte(REG_TX_AUDCHST_CAT,ucIEC60958ChStat[1]); // 192, audio CATEGORY
    HDMITX_WriteI2C_Byte(REG_TX_AUDCHST_SRCNUM,ucIEC60958ChStat[2]&0xF);
    HDMITX_WriteI2C_Byte(REG_TX_AUD0CHST_CHTNUM,(ucIEC60958ChStat[2]>>4)&0xF);
    HDMITX_WriteI2C_Byte(REG_TX_AUDCHST_CA_FS,ucIEC60958ChStat[3]); // choose clock
    HDMITX_WriteI2C_Byte(REG_TX_AUDCHST_OFS_WL,ucIEC60958ChStat[4]);
    Switch_HDMITX_Bank(0);
}

void
setIT626X_UpdateChStatFs(ULONG Fs)
{
    BYTE uc ;

    /////////////////////////////////////
    // Fs should be the following value.
    // #define AUDFS_22p05KHz  4
    // #define AUDFS_44p1KHz 0
    // #define AUDFS_88p2KHz 8
    // #define AUDFS_176p4KHz    12
    //
    // #define AUDFS_24KHz  6
    // #define AUDFS_48KHz  2
    // #define AUDFS_96KHz  10
    // #define AUDFS_192KHz 14
    //
    // #define AUDFS_768KHz 9
    //
    // #define AUDFS_32KHz  3
    // #define AUDFS_OTHER    1
    /////////////////////////////////////

    Switch_HDMITX_Bank(1);
    uc = HDMITX_ReadI2C_Byte(REG_TX_AUDCHST_CA_FS); // choose clock
    HDMITX_WriteI2C_Byte(REG_TX_AUDCHST_CA_FS,uc); // choose clock
    uc &= 0xF0 ;
    uc |= (Fs&0xF);

    uc = HDMITX_ReadI2C_Byte(REG_TX_AUDCHST_OFS_WL);
    uc &= 0xF ;
    uc |= ((~Fs) << 4)&0xF0 ;
    HDMITX_WriteI2C_Byte(REG_TX_AUDCHST_OFS_WL,uc);

    Switch_HDMITX_Bank(0);


}

void
setIT626X_LPCMAudio(BYTE AudioSrcNum, BYTE AudSWL, BOOL bSPDIF)
{

    BYTE AudioEnable, AudioFormat ;

    AudioEnable = 0 ;
    AudioFormat = Instance[0].bOutputAudioMode ;


    switch(AudSWL)
    {
    case 16:
        AudioEnable |= M_AUD_16BIT ;
        break ;
    case 18:
        AudioEnable |= M_AUD_18BIT ;
        break ;
    case 20:
        AudioEnable |= M_AUD_20BIT ;
        break ;
    case 24:
    default:
        AudioEnable |= M_AUD_24BIT ;
        break ;
    }

    if( bSPDIF )
    {
        AudioFormat &= ~0x40 ;
        AudioEnable |= B_AUD_SPDIF|B_AUD_EN_I2S0 ;
    }
    else
    {
        AudioFormat |= 0x40 ;
        switch(AudioSrcNum)
        {
        case 4:
            AudioEnable |= B_AUD_EN_I2S3|B_AUD_EN_I2S2|B_AUD_EN_I2S1|B_AUD_EN_I2S0 ;
            break ;

        case 3:
            AudioEnable |= B_AUD_EN_I2S2|B_AUD_EN_I2S1|B_AUD_EN_I2S0 ;
            break ;

        case 2:
            AudioEnable |= B_AUD_EN_I2S1|B_AUD_EN_I2S0 ;
            break ;

        case 1:
        default:
            AudioFormat &= ~0x40 ;
            AudioEnable |= B_AUD_EN_I2S0 ;
            break ;

        }
    }
    AudioFormat|=0x01;//mingchih add
    Instance[0].bAudioChannelEnable=AudioEnable;

    Switch_HDMITX_Bank(0);
    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL0,AudioEnable&0xF0);

    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL1,AudioFormat); // regE1 bOutputAudioMode should be loaded from ROM image.
    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_FIFOMAP,0xE4); // default mapping.
#ifdef USE_SPDIF_CHSTAT
    if( bSPDIF )
    {
        HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL3,B_CHSTSEL);
    }
    else
    {
        HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL3,0);
    }
#else // not USE_SPDIF_CHSTAT
    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL3,0);
#endif // USE_SPDIF_CHSTAT

    HDMITX_WriteI2C_Byte(REG_TX_AUD_SRCVALID_FLAT,0x00);
    HDMITX_WriteI2C_Byte(REG_TX_AUD_HDAUDIO,0x00); // regE5 = 0 ;

    if( bSPDIF )
    {
        BYTE i ;
        HDMI_OrREG_TX_Byte(0x5c,(1<<6));
        for( i = 0 ; i < 100 ; i++ )
        {
            if(HDMITX_ReadI2C_Byte(REG_TX_CLK_STATUS2) & B_OSF_LOCK)
            {
                break ; // stable clock.
            }
        }
    }
//    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL0, Instance[0].bAudioChannelEnable);
}

void
setIT626X_NLPCMAudio() // no Source Num, no I2S.
{
    BYTE AudioEnable, AudioFormat ;
    BYTE i ;

    AudioFormat = 0x01 ; // NLPCM must use standard I2S mode.
    AudioEnable = M_AUD_24BIT|B_AUD_SPDIF|B_AUD_EN_I2S0 ;

    Switch_HDMITX_Bank(0);
    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL0, M_AUD_24BIT|B_AUD_SPDIF);
    //HDMITX_AndREG_Byte(REG_TX_SW_RST,~(B_AUD_RST|B_AREF_RST));

    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL1,0x01); // regE1 bOutputAudioMode should be loaded from ROM image.
    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_FIFOMAP,0xE4); // default mapping.

#ifdef USE_SPDIF_CHSTAT
    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL3,B_CHSTSEL);
#else // not USE_SPDIF_CHSTAT
    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL3,0);
#endif // USE_SPDIF_CHSTAT

    HDMITX_WriteI2C_Byte(REG_TX_AUD_SRCVALID_FLAT,0x00);
    HDMITX_WriteI2C_Byte(REG_TX_AUD_HDAUDIO,0x00); // regE5 = 0 ;

    for( i = 0 ; i < 100 ; i++ )
    {
        if(HDMITX_ReadI2C_Byte(REG_TX_CLK_STATUS2) & B_OSF_LOCK)
        {
            break ; // stable clock.
        }
    }
    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL0, M_AUD_24BIT|B_AUD_SPDIF|B_AUD_EN_I2S0);
}

void
setIT626X_HBRAudio(BOOL bSPDIF)
{
    BYTE rst,uc ;
    Switch_HDMITX_Bank(0);

    rst = HDMITX_ReadI2C_Byte(REG_TX_SW_RST);

    //HDMITX_WriteI2C_Byte(REG_TX_SW_RST, rst | (B_AUD_RST|B_AREF_RST) );

    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL1,0x47); // regE1 bOutputAudioMode should be loaded from ROM image.
    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_FIFOMAP,0xE4); // default mapping.

    if( bSPDIF )
    {
        HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL0, M_AUD_24BIT|B_AUD_SPDIF);
        HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL3,B_CHSTSEL);
    }
    else
    {
        HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL0, M_AUD_24BIT);
        HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL3,0);
    }

    HDMITX_WriteI2C_Byte(REG_TX_AUD_SRCVALID_FLAT,0x08);
    HDMITX_WriteI2C_Byte(REG_TX_AUD_HDAUDIO,B_HBR); // regE5 = 0 ;
    uc = HDMITX_ReadI2C_Byte(REG_TX_CLK_CTRL1);
    uc &= ~M_AUD_DIV ;
    HDMITX_WriteI2C_Byte(REG_TX_CLK_CTRL1, uc);

    if( bSPDIF )
    {
        BYTE i ;
        for( i = 0 ; i < 100 ; i++ )
        {
            if(HDMITX_ReadI2C_Byte(REG_TX_CLK_STATUS2) & B_OSF_LOCK)
            {
                break ; // stable clock.
            }
        }
        HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL0, M_AUD_24BIT|B_AUD_SPDIF|B_AUD_EN_SPDIF);
    }
    else
    {
        HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL0, M_AUD_24BIT|B_AUD_EN_I2S3|B_AUD_EN_I2S2|B_AUD_EN_I2S1|B_AUD_EN_I2S0);
    }
    HDMI_AndREG_TX_Byte(0x5c,~(1<<6));
    Instance[0].bAudioChannelEnable=HDMITX_ReadI2C_Byte(REG_TX_AUDIO_CTRL0);
    //HDMITX_WriteI2C_Byte(REG_TX_SW_RST, rst & ~(B_AUD_RST|B_AREF_RST) );
}

void
setIT626X_DSDAudio()
{
    // to be continue
    BYTE rst, uc ;
    rst = HDMITX_ReadI2C_Byte(REG_TX_SW_RST);

    //HDMITX_WriteI2C_Byte(REG_TX_SW_RST, rst | (B_AUD_RST|B_AREF_RST) );

    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL1,0x41); // regE1 bOutputAudioMode should be loaded from ROM image.
    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_FIFOMAP,0xE4); // default mapping.

    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL0, M_AUD_24BIT);
    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL3,0);

    HDMITX_WriteI2C_Byte(REG_TX_AUD_SRCVALID_FLAT,0x00);
    HDMITX_WriteI2C_Byte(REG_TX_AUD_HDAUDIO,B_DSD); // regE5 = 0 ;
    //HDMITX_WriteI2C_Byte(REG_TX_SW_RST, rst & ~(B_AUD_RST|B_AREF_RST) );

    uc = HDMITX_ReadI2C_Byte(REG_TX_CLK_CTRL1);
    uc &= ~M_AUD_DIV ;
    HDMITX_WriteI2C_Byte(REG_TX_CLK_CTRL1, uc);



    HDMITX_WriteI2C_Byte(REG_TX_AUDIO_CTRL0, M_AUD_24BIT|B_AUD_EN_I2S3|B_AUD_EN_I2S2|B_AUD_EN_I2S1|B_AUD_EN_I2S0);
}

void
EnableHDMIAudio(BYTE AudioType, BOOL bSPDIF,  ULONG SampleFreq,  BYTE ChNum, BYTE *pIEC60958ChStat, ULONG TMDSClock)
{
    static _IDATA BYTE ucIEC60958ChStat[5] ;
    BYTE Fs ;
    Instance[0].TMDSClock=TMDSClock;
    Instance[0].bAudioChannelEnable=0;
    Instance[0].bSPDIF_OUT=bSPDIF;
    HDMITX_WriteI2C_Byte(REG_TX_CLK_CTRL0,B_AUTO_OVER_SAMPLING_CLOCK|B_EXT_256FS|0x01);
    if(bSPDIF)
    {
        if(AudioType==T_AUDIO_HBR)
        {
            HDMITX_WriteI2C_Byte(REG_TX_CLK_CTRL0,0x81);
        }
        HDMITX_OrREG_Byte(REG_TX_AUDIO_CTRL0,B_AUD_SPDIF);
    }
    else
    {
        HDMITX_AndREG_Byte(REG_TX_AUDIO_CTRL0,(~B_AUD_SPDIF));
    }

    if( AudioType != T_AUDIO_DSD)
    {
        // one bit audio have no channel status.
        switch(SampleFreq)
        {
        case  44100L: Fs =  AUDFS_44p1KHz ; break ;
        case  88200L: Fs =  AUDFS_88p2KHz ; break ;
        case 176400L: Fs = AUDFS_176p4KHz ; break ;
        case  32000L: Fs =    AUDFS_32KHz ; break ;
        case  48000L: Fs =    AUDFS_48KHz ; break ;
        case  96000L: Fs =    AUDFS_96KHz ; break ;
        case 192000L: Fs =   AUDFS_192KHz ; break ;
        case 768000L: Fs =   AUDFS_768KHz ; break ;
        default:
            SampleFreq = 48000L ;
            Fs =    AUDFS_48KHz ;
            break ; // default, set Fs = 48KHz.
        }

#ifdef SUPPORT_AUDIO_MONITOR
    Instance[0].bAudFs=AUDFS_OTHER;
#else
    Instance[0].bAudFs=Fs;
#endif

        if( pIEC60958ChStat == NULL )
        {
            ucIEC60958ChStat[0] = 0 ;
            ucIEC60958ChStat[1] = 0 ;
            ucIEC60958ChStat[2] = (ChNum+1)/2 ;

            if(ucIEC60958ChStat[2]<1)
            {
                ucIEC60958ChStat[2] = 1 ;
            }
            else if( ucIEC60958ChStat[2] >4 )
            {
                ucIEC60958ChStat[2] = 4 ;
            }

            ucIEC60958ChStat[3] = Fs ;
#if(SUPPORT_AUDI_AudSWL==16)
            ucIEC60958ChStat[4] = ((~Fs)<<4) & 0xF0 | 0x02 ; // Fs | 24bit word length
#elif(SUPPORT_AUDI_AudSWL==18)
            ucIEC60958ChStat[4] = ((~Fs)<<4) & 0xF0 | 0x04 ; // Fs | 24bit word length
#elif(SUPPORT_AUDI_AudSWL==20)
            ucIEC60958ChStat[4] = ((~Fs)<<4) & 0xF0 | 0x03 ; // Fs | 24bit word length
#else
            ucIEC60958ChStat[4] = ((~Fs)<<4) & 0xF0 | 0x0B ; // Fs | 24bit word length
#endif
            pIEC60958ChStat = ucIEC60958ChStat ;
        }
    }

    switch(AudioType)
    {
    case T_AUDIO_HBR:
#ifdef Debug_message		
        HDMITX_DEBUG_PRINTF(("T_AUDIO_HBR\n"));
#endif
        pIEC60958ChStat[0] |= 1<<1 ;
        pIEC60958ChStat[2] = 0;
        pIEC60958ChStat[3] &= 0xF0 ;
        pIEC60958ChStat[3] |= AUDFS_768KHz ;
        pIEC60958ChStat[4] |= (((~AUDFS_768KHz)<<4) & 0xF0)| 0xB ;
        setIT626X_ChStat(pIEC60958ChStat);
        setIT626X_HBRAudio(bSPDIF);

        break ;
    case T_AUDIO_DSD:
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("T_AUDIO_DSD\n"));
#endif
        setIT626X_DSDAudio();
        break ;
    case T_AUDIO_NLPCM:
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("T_AUDIO_NLPCM\n"));
#endif
        pIEC60958ChStat[0] |= 1<<1 ;
        setIT626X_ChStat(pIEC60958ChStat);
        setIT626X_NLPCMAudio();
        break ;
    case T_AUDIO_LPCM:
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF(("T_AUDIO_LPCM\n"));
#endif
        pIEC60958ChStat[0] &= ~(1<<1);

        setIT626X_ChStat(pIEC60958ChStat);
        setIT626X_LPCMAudio((ChNum+1)/2, /*24*/SUPPORT_AUDI_AudSWL, bSPDIF);
        // can add auto adjust
        break ;
    }
    HDMITX_AndREG_Byte(REG_TX_INT_MASK1,(~B_AUDIO_OVFLW_MASK));
    HDMITX_AndREG_Byte(REG_TX_SW_RST,~(B_AUD_RST|B_AREF_RST));
}
void SoftWareVideoReset(void)
{
    BYTE ResetCount;
    BYTE intclr = 0;

   // Switch_HDMITX_Bank(0) ;
   // CurrCTS=0;
    ResetCount=5;
    I2C_OrReg_Byte(REG_TX_SW_RST, B_VID_RST);     //add 20080702  hermes
    delay1ms(1);
    I2C_AndReg_Byte(REG_TX_SW_RST,~(B_VID_RST));
    while(0 == (HDMITX_ReadI2C_Byte(REG_TX_SYS_STATUS) & B_TXVIDSTABLE) )
    {
        delay1ms(100);
        ResetCount--;
        if(ResetCount==0)break;
#ifdef Debug_message
        HDMITX_DEBUG_PRINTF((" wait video stable \n")) ;
#endif
    }
#ifdef Debug_message
    HDMITX_DEBUG_PRINTF((" =======================reset video==========================\n")) ;
#endif
    I2C_OrReg_Byte(REG_TX_INT_CLR1, B_CLR_VIDSTABLE) ;
    intclr |= (HDMITX_ReadI2C_Byte(REG_TX_SYS_STATUS)&(~B_CLR_AUD_CTS))| B_INTACTDONE ;
    HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,intclr) ;
    intclr &= ~B_INTACTDONE ;
    HDMITX_WriteI2C_Byte(REG_TX_SYS_STATUS,intclr) ; // INTACTDONE reset to zero.

}
BOOL GetVideoStatus(void)
{
    if(HDMITX_ReadI2C_Byte(REG_TX_SYS_STATUS)&B_TXVIDSTABLE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
void UpDateAFE(ULONG PixelClock)
{
    BYTE uc,level;
    unsigned long cTMDSClock = PixelClock;//*(pixelrep+1);
    uc = HDMITX_ReadI2C_Byte(0xc1);
    switch(uc & 0x70)
    {
    case 0x50:
        cTMDSClock *= 5 ;
        cTMDSClock /= 4 ;
        break ;
    case 0x60:
        cTMDSClock *= 3 ;
        cTMDSClock /= 2 ;
    }

    if( cTMDSClock>80000000L )
    {
        level = PCLK_HIGH ;
    }
    else if(cTMDSClock>20000000L)
    {
        level = PCLK_MEDIUM ;
    }
    else
    {
        level = PCLK_LOW ;
    }
    SetupAFE(level);
    //FireAFE();
}
void EnableDeOnly(BOOL DeOnly)
{
    if(DeOnly)
    {
        EnableHVToolDetect(FALSE);
        HDMITX_OrREG_Byte(0xa5,(1<<5));
        ReGenTimingEnable=TRUE;
    }
    else
    {
        EnableHVToolDetect(TRUE);
        HDMITX_AndREG_Byte(0xa5,~(1<<5));
        ReGenTimingEnable=FALSE;
    }
}
void EnableHVToolDetect(BOOL HVenable)
{
    if(HVenable)
        HDMITX_OrREG_Byte(0xa8,(1<<3));
    else
        HDMITX_AndREG_Byte(0xa8,~(1<<3));
}
UINT GetHTotal(void)
{
    return (((WORD)HDMITX_ReadI2C_Byte(0x91))<<4)+((WORD)(HDMITX_ReadI2C_Byte(0x90)>>4))+1;
}
UINT GetHActive(void)
{
    return abs(((((WORD)(HDMITX_ReadI2C_Byte(0x94)&0xf0))<<4)+((WORD)HDMITX_ReadI2C_Byte(0x93)))-
               ((((WORD)(HDMITX_ReadI2C_Byte(0x94)&0x0f))<<8)+((WORD)HDMITX_ReadI2C_Byte(0x92))));
}
UINT GetHBlank(void)
{
    return (GetHTotal()-GetHActive());
}

UINT GetVTotal(void)
{
    return ((WORD)(HDMITX_ReadI2C_Byte(0x99)&0x07)<<8)+((WORD)HDMITX_ReadI2C_Byte(0x98))+1;
}
UINT GetVActive(void)
{
    return abs(((((WORD)(HDMITX_ReadI2C_Byte(0x9C)&0x70))<<4)+((WORD)HDMITX_ReadI2C_Byte(0x9B)))-
               ((((WORD)(HDMITX_ReadI2C_Byte(0x9C)&0x07))<<8)+((WORD)HDMITX_ReadI2C_Byte(0x9A))));
}
UINT GetVBlank(void)
{
    return (GetVTotal()-GetVActive());
}
#ifdef SUPPORT_SPLASH
void FastOutput(void)
{
    Switch_HDMITX_Bank(0) ;
    HDMITX_WriteI2C_Byte(REG_TX_SW_RST,B_VID_RST|B_AUD_RST|B_AREF_RST|B_HDCP_RST|B_REF_RST);
    HDMITX_WriteI2C_Byte(REG_TX_SW_RST,          B_AUD_RST|B_AREF_RST|B_HDCP_RST);
    InitLVDS();
    SetLVDSinterface();
    EnableAVIInfoFrame(FALSE ,NULL);
    HDMITX_WriteI2C_Byte(REG_TX_HDMI_MODE,B_TX_DVI_MODE);
    UpDateAFE(GetInputPclk());
    FireAFE();
    SetAVMute(FALSE);
    delay1ms(4200);
    //DisableVideoOutput();
}
#endif
#ifdef SUPPORT_GPIO_MUTE
BOOL LastGPIO_Status=TRUE;
void GPIO_Panel_En()
{
    if(LastGPIO_Status == TRUE &&
        P1_7 == FALSE)
    {
        DisableAudioOutput();
        printf("P1_7 == FALSE\n");
    }

    if(LastGPIO_Status == FALSE &&
        P1_7 == TRUE)
    {
        HDMITX_AndREG_Byte(0x04,~(0x14));
        SetAVMute(FALSE);
        printf("P1_7 == TRUE\n");
    }

    LastGPIO_Status=P1_7;
}
#endif

void CheckAudioVideoInput()
{
    BYTE LVDS_Lock;
    BYTE NoAudio_flag,AudioOFF_flag;
#ifdef SUPPORT_GPIO_MUTE
    GPIO_Panel_En();
#else
    LVDS_Lock = (0x03&LVDS_ReadI2C_Byte(0x30));
    NoAudio_flag = ((1<<4)&HDMITX_ReadI2C_Byte(0x5f));
    AudioOFF_flag = ((1<<4)&HDMITX_ReadI2C_Byte(0xc5));
    if(0x03!=LVDS_Lock)
    {
#ifdef SUPPORT_HDCP
        if(Instance[0].bAuthenticated)
            {EnableHDCP(FALSE);}
#endif
        DisableAudioOutput();
    }
    if(NoAudio_flag!=AudioOFF_flag)
    {
        HDMITX_WriteI2C_Byte(0xc5,(((0xEF)&HDMITX_ReadI2C_Byte(0xc5))|NoAudio_flag));
    }
#endif
}
//~jj_tseng@chipadvanced.com 2008/08/18
//////////////////////////////////////////////////////////////////////
// Function: DumpCatHDMITXReg()
// Parameter: N/A
// Return: N/A
// Remark: Debug function,dumps the registers of CAT6611.
// Side-Effect: N/A
//////////////////////////////////////////////////////////////////////

#ifdef Debug_message
void DumpLVDSReg()
{
    int i,j ;
    BYTE ucData ;
    HDMITX_DEBUG_PRINTF(("        "));
    for( j = 0 ; j < 16 ; j++ )
    {

        if(j<0x10) HDMITX_DEBUG_PRINTF(("0%X ",j));
        else HDMITX_DEBUG_PRINTF(("%X ",j));
        if( (j == 3)||(j==7)||(j==11))
        {
            HDMITX_DEBUG_PRINTF(("  "));
        }
    }
    HDMITX_DEBUG_PRINTF(("\n        -----------------------------------------------------\n"));

    Switch_HDMITX_Bank(0);

    for(i = 0 ; i < 0x100 ; i+=16 )
    {
        if(i<0x10) HDMITX_DEBUG_PRINTF(("[ 0%X ]  ",i));
        else HDMITX_DEBUG_PRINTF(("[ %X ]  ",i));
        for( j = 0 ; j < 16 ; j++ )
        {
            i2c_read_byte(0x66, (i+j), 1, &ucData, I2CDEV);
    	     //I2C_ReadBuf_FromDevice(0x66, (i+j),  &ucData,1);
            if(ucData<0x10) HDMITX_DEBUG_PRINTF(("0%X ",(WORD)ucData));
            else HDMITX_DEBUG_PRINTF(("%X ",(WORD)ucData));
            if( (j == 3)||(j==7)||(j==11))
            {
                HDMITX_DEBUG_PRINTF((" -"));
            }
        }
        HDMITX_DEBUG_PRINTF(("\n"));
        if( (i % 0x40) == 0x30)
        {
            HDMITX_DEBUG_PRINTF(("        -----------------------------------------------------\n"));
        }
    }

}
void DumpCatHDMITXReg()
{
    int i,j ;
    BYTE ucData ;

    HDMITX_DEBUG_PRINTF(("        "));
    for( j = 0 ; j < 16 ; j++ )
    {

        if(j<0x10) HDMITX_DEBUG_PRINTF(("0%X ",j));
        else HDMITX_DEBUG_PRINTF(("%X ",j));
        if( (j == 3)||(j==7)||(j==11))
        {
            HDMITX_DEBUG_PRINTF(("  "));
        }
    }
    HDMITX_DEBUG_PRINTF(("\n        -----------------------------------------------------\n"));

    Switch_HDMITX_Bank(0);

    for(i = 0 ; i < 0x100 ; i+=16 )
    {
        if(i<0x10) HDMITX_DEBUG_PRINTF(("[ 0%X ]  ",i));
        else HDMITX_DEBUG_PRINTF(("[ %X ]  ",i));
        for( j = 0 ; j < 16 ; j++ )
        {
            ucData = HDMITX_ReadI2C_Byte((BYTE)((i+j)&0xFF));
            if(ucData<0x10) HDMITX_DEBUG_PRINTF(("0%X ",(WORD)ucData));
            else HDMITX_DEBUG_PRINTF(("%X ",(WORD)ucData));
            if( (j == 3)||(j==7)||(j==11))
            {
                HDMITX_DEBUG_PRINTF((" -"));
            }
        }
        HDMITX_DEBUG_PRINTF(("\n"));
        if( (i % 0x40) == 0x30)
        {
            HDMITX_DEBUG_PRINTF(("        -----------------------------------------------------\n"));
        }
    }
    Switch_HDMITX_Bank(1);
    for(i = 0x00; i < 0x100 ; i+=16 )
    {
        if(i<0x10) HDMITX_DEBUG_PRINTF(("[10%X ]  ",i));
        else HDMITX_DEBUG_PRINTF(("[1%X ]  ",i));
        for( j = 0 ; j < 16 ; j++ )
        {
            ucData = HDMITX_ReadI2C_Byte((BYTE)((i+j)&0xFF));
            if(ucData<0x10) HDMITX_DEBUG_PRINTF(("0%X ",(WORD)ucData));
            else HDMITX_DEBUG_PRINTF(("%X ",(WORD)ucData));
            if( (j == 3)||(j==7)||(j==11))
            {
                HDMITX_DEBUG_PRINTF((" -"));
            }
        }
        HDMITX_DEBUG_PRINTF(("\n"));
        if( (i % 0x40) == 0x30)
        {
            HDMITX_DEBUG_PRINTF(("        -----------------------------------------------------\n"));
        }

    }
    Switch_HDMITX_Bank(0);
}

#endif
