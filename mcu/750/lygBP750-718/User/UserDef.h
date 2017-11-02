/******************************************************************************
 Copyright   All rights reserved.

 [Module Name]: strings.h
 [Date]:        11-May-2012
 [Comment]:
   define strings file.
 [Reversion History]:
*******************************************************************************/


#ifndef _USERDEF_H_
#define _USERDEF_H_

#include "header.h"

////试用机器定义
////#define TRIAL_MACHINE

#ifdef _USERDEF_C_
  #define _USERDEFDEC_
#else
  #define _USERDEFDEC_ extern
#endif


////#define DEBUG_DEBUGCMD

////#define	USESETUPWIZARD

_USERDEFDEC_ const U8 g_MachineTypeStr[6];
_USERDEFDEC_ const U8 g_MachineTypeStrEn[6];
_USERDEFDEC_ const U8 g_SoftVersionStr[6];
_USERDEFDEC_ U8 g_MachineTypeXPStr[21];

_USERDEFDEC_ const U8 g_ReleaseStr[7];
_USERDEFDEC_ U8 g_ContactTel1[13];
_USERDEFDEC_ U8 g_ContactTel2[13];


_USERDEFDEC_ const U8 g_Factory[6];





//////////============内置企业logo==================

_USERDEFDEC_ const U8 g_NoEnterpriseStr[21];
_USERDEFDEC_ const U8 g_EnterpriseStr[21];
_USERDEFDEC_ const U8 g_Enterprise1StrBYCOX[21];

/*
_USERDEFDEC_ const U8 g_Enterprise2Str[21];
_USERDEFDEC_ const U8 g_Enterprise3Str[21];
_USERDEFDEC_ const U8 g_Enterprise4Str[21];
_USERDEFDEC_ const U8 g_Enterprise5Str[21];
_USERDEFDEC_ const U8 g_Enterprise6Str[21];
_USERDEFDEC_ const U8 g_Enterprise7Str[21];
_USERDEFDEC_ const U8 g_Enterprise8Str[21];
_USERDEFDEC_ const U8 g_Enterprise9Str[21];
_USERDEFDEC_ const U8 g_Enterprise10Str[21];
_USERDEFDEC_ const U8 g_Enterprise11Str[21];
_USERDEFDEC_ const U8 g_Enterprise12Str[21];
_USERDEFDEC_ const U8 g_Enterprise13Str[21];
_USERDEFDEC_ const U8 g_Enterprise201Str[21];
_USERDEFDEC_ const U8 g_Enterprise202Str[21];
_USERDEFDEC_ const U8 g_Enterprise203Str[21];
_USERDEFDEC_ const U8 g_Enterprise204Str[21];
_USERDEFDEC_ const U8 g_Enterprise205Str[21];
_USERDEFDEC_ const U8 g_Enterprise206Str[21];
_USERDEFDEC_ const U8 g_Enterprise301Str[21];

_USERDEFDEC_ const U8 g_Enterprise4958Str[21];


_USERDEFDEC_ const U8 g_Enterprise7075Str[21];
_USERDEFDEC_ const U8 g_Enterprise7076Str[21];

_USERDEFDEC_ const U8 g_Enterprise8476Str[21];
_USERDEFDEC_ const U8 g_Enterprise8721Str[21];
_USERDEFDEC_ const U8 g_Enterprise8722Str[21];
_USERDEFDEC_ const U8 g_Enterprise8723Str[21];
_USERDEFDEC_ const U8 g_Enterprise8724Str[21];
_USERDEFDEC_ const U8 g_Enterprise8725Str[21];
_USERDEFDEC_ const U8 g_Enterprise8726Str[21];
*/
//////////============内置企业logo==================


#endif

