///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <typedef.h>
//   @author Jau-chih.Tseng@ite.com.tw
//   @date   2012/06/29
//   @fileversion: IT626X_BASE_M1_MYSON_V206
//******************************************/

#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_


typedef enum _SYS_STATUS {
    ER_SUCCESS = 0,
    ER_FAIL,
    ER_RESERVED
} SYS_STATUS ;

#define abs(x) (((x)>0)?(x):(-(x)))


#define _CODE
#define _DATA
#define _XDATA
#define _IDATA

typedef _CODE unsigned char    cBYTE;       // "code" is necessary,  Clive 100808

typedef unsigned int UINT, uint;
typedef unsigned char BOOL;

typedef char CHAR, *PCHAR ;
typedef unsigned char uchar, *puchar ;
typedef unsigned char UCHAR, *PUCHAR ;
typedef unsigned char byte, *pbyte ;
typedef unsigned char BYTE, *PBYTE ;

typedef short SHORT, *PSHORT ;
typedef unsigned short ushort, *pushort ;
typedef unsigned short USHORT, *PUSHORT ;
typedef unsigned short word, *pword ;
typedef unsigned short WORD, *PWORD ;

typedef long LONG, *PLONG ;
typedef unsigned long ulong, *pulong ;
typedef unsigned long ULONG, *PULONG ;
typedef unsigned long dword, *pdword ;
typedef unsigned long DWORD, *PDWORD ;


//#define FALSE 0
//#define TRUE 1


//#define SUCCESS 0
#define FAIL -1

#define ON 1
#define OFF 0

#define LO_ACTIVE TRUE
#define HI_ACTIVE FALSE


#endif    // _TYPEDEF_H_
