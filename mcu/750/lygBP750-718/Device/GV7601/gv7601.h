#ifndef __GV7601_H__
#define __GV7601_H__

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/
#include "../../System Code/global.h"
#include "../../System Code/type.h"
/****************************************************************************/
/*	G L O B A L    D E F I N I T I O N										*/
/****************************************************************************/
typedef enum
{
	GV7601_STD_720X480I_60Hz,
	GV7601_STD_720X576I_50Hz,
	GV7601_STD_1280X720P_24,
	GV7601_STD_1280X720P_25,
	GV7601_STD_1280X720P_30,
	GV7601_STD_1280X720P_60,
	GV7601_STD_1280X720P_50,
	GV7601_STD_1920X1080I_60,
	GV7601_STD_1920X1080I_50,
	GV7601_STD_1920X1080P_30,
	GV7601_STD_1920X1080P_25,
	GV7601_STD_1920X1080P_24,
	GV7601_STD_1920X1080P_60,
	GV7601_STD_1920X1080P_50,
	GV7601_STD_1920X1035I_60,
	GV7601_STD_1920X1035I_50,
	GV7601_STD_1920X1035P_60,
	GV7601_STD_1920X1035P_50,
	GV7601_STD_INVALID,
}GV7601_VIDEO_STD;

typedef struct {
//	uint8_t AudioState;
	GV7601_VIDEO_STD VideoState;
	unsigned int VideoHTotalSize;
	unsigned int VideoVTotalSize;
	unsigned int VideoHSize;
	unsigned int VideoVSize;
	unsigned char VideoRefresh;
	unsigned char VideoInterlaced;
}GV7601_GlobalStatus_t; 

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
extern GV7601_GlobalStatus_t gSt_GV7601Status_t;
/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
void GV7601_GSPI_Init(void);
int GV7601_Init(BYTE B_SPIIndex);
extern int GV7601_GetStatus(BYTE B_SPIIndex);
extern int GV7601_getVD_STD(BYTE B_SPIIndex, GV7601_VIDEO_STD *std);
extern WORD GV7601_ReadWord(BYTE B_SPI_Cs, WORD W_Addr, WORD* pReadData);
extern WORD GV7601_WriteWord(BYTE B_SPI_Cs, WORD W_Addr, WORD W_Data);

#endif

