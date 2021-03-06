#include "header.h"


#define _USERDEF_C_




//机器型号:
const U8 g_MachineTypeStr[6]={0xD0,0xCD,0xBA,0xC5,0x3A,0};
const U8 g_MachineTypeStrEn[6]="Type:";
U8 g_MachineTypeXPStr[21]="XP750";

//软件版本:
const U8 g_SoftVersionStr[6]={0xC8,0xED,0xBC,0xFE,0x3A,0};
const U8 g_ReleaseStr[7]="160905";//整数部分2位，小数部分1�


U8 g_ContactTel1[13]={
'4','0','0','-','0','9','6','-','5','7','6','8',0  //400-096-5768
};
U8 g_ContactTel2[13]={'4','0','0','-','0','9','6','-','5','7','6','8',0};


const U8 g_Factory[6]={0xD4,0xAD,0xB3,0xA7,0x3A,0};





//////////============内置企业logo==================

//欢迎使用
const U8 g_NoEnterpriseStr[21]=
{
	' ',' ',' ',' ',' ',' ',0xBB,0xB6,0xD3,0xAD,0XCA,0XB9,0xD3,0xC3,0
};
//北京拓北公司
const U8 g_Enterprise1StrBYCOX[21]={
' ',' ',' ',' ',0xB1,0xB1,0xBE,0xA9,0xcd,0xd8,0xb1,0xb1,0xB9,0xAB,0xCB,0xBE,0
};


/*
//凯视达科技
const U8 g_Enterprise2Str[21]={
' ',' ',' ',' ',0xBF,0xAD,0xCA,0xD3,0xB4,0xEF,0xBF,0xC6,0xBC,0xBC,0x0
};
//唯奥视讯
const U8 g_Enterprise3Str[21]={
	' ',' ',' ',' ',' ',' ',0xCE,0xA8,0xB0,0xC2,0xCA,0xD3,0xD1,0xB6,0x0
};
//视睿讯科技
const U8 g_Enterprise4Str[21]={
	' ',' ',' ',' ',0xCA,0xD3,0xEE,0xA3,0xD1,0xB6,0xBF,0xC6,0xBC,0xBC,0x0
};
//视诚科技
const U8 g_Enterprise5Str[21]={
' ',' ',' ',' ',' ',' ',0xCA,0xD3,0xB3,0xCF,0xBF,0xC6,0xBC,0xBC,0x0
};
//迈普视通技术
const U8 g_Enterprise6Str[21]={
	' ',' ',' ',' ',0xC2,0xF5,0xC6,0xD5,0xCA,0xD3,0xCD,0xA8,0xBC,0xBC,0xCA,0xF5,0x0
};
//爱普视科技
const U8 g_Enterprise7Str[21]={
	' ',' ',' ',' ',0xB0,0xAE,0xC6,0xD5,0xCA,0xD3,0xBF,0xC6,0xBC,0xBC,0x0
};
//	8：瑞江科技
const U8 g_Enterprise8Str[21]={
	' ',' ',' ',' ',' ',' ',0xC8,0xF0,0xBD,0xAD,0xBF,0xC6,0xBC,0xBC,0x0
};
//	9：神达实业
const U8 g_Enterprise9Str[21]={
	' ',' ',' ',' ',' ',' ',0xC9,0xF1,0xB4,0xEF,0xCA,0xB5,0xD2,0xB5,0x0
};
//	10：洲泰科技
const U8 g_Enterprise10Str[21]={
	' ',' ',' ',' ',' ',' ',0xD6,0xDE,0xCC,0xA9,0xBF,0xC6,0xBC,0xBC,0x0
};
//	11：杰思创科技
const U8 g_Enterprise11Str[21]={
	' ',' ',' ',' ',0xBD,0xDC,0xCB,0xBC,0xB4,0xB4,0xBF,0xC6,0xBC,0xBC,0x0
};
//	12：创凯电子
const U8 g_Enterprise12Str[21]={
	' ',' ',' ',' ',' ',' ',0xB4,0xB4,0xBF,0xAD,0xB5,0xE7,0xD7,0xD3,0x0
};
//	13：富视威科技
const U8 g_Enterprise13Str[21]={
	' ',' ',' ',' ',0xB8,0xBB,0xCA,0xD3,0xCD,0xFE,0xBF,0xC6,0xBC,0xBC,0x0
};




//北京利亚德公司
const U8 g_Enterprise201Str[21]={
' ',' ',0xB1,0xB1,0xBE,0xA9,0xC0,0xFB,0xD1,0xC7,0xB5,0xC2,0xB9,0xAB,0xCB,0xBE,0
};
//深圳锐拓公司
const U8 g_Enterprise202Str[21]={
' ',' ',' ',' ',0xC9,0xEE,0xDB,0xDA,0xC8,0xF1,0xCD,0xD8,0xB9,0xAB,0xCB,0xBE,0
};

//深圳洲明公司
const U8 g_Enterprise203Str[21]={
' ',' ',' ',' ',0xC9,0xEE,0xDB,0xDA,0xD6,0xDE,0xC3,0xF7,0xB9,0xAB,0xCB,0xBE,0
};




//北京远程悦动信息技术
const U8 g_Enterprise204Str[21]={
0xB1,0xB1,0xBE,0xA9,0xD4,0xB6,0xB3,0xCC,0xD4,0xC3,0xB6,0xAF,0xD0,0xC5,0xCF,0xA2,0xBC,0xBC,0xCA,0xF5,0
};
//四川国彬电子科技
const U8 g_Enterprise205Str[21]={
' ',' ',0xCB,0xC4,0xB4,0xA8,0xB9,0xFA,0xB1,0xF2,0xB5,0xE7,0xD7,0xD3,0xBF,0xC6,0xBC,0xBC,0
};
//颐景丰科技
const U8 g_Enterprise206Str[21]={
' ',' ',' ',' ',0xD2,0xC3,0xBE,0xB0,0xB7,0xE1,0xBF,0xC6,0xBC,0xBC,0
};

//灰度科技
const U8 g_Enterprise301Str[21]={
' ',' ',0xBB,0xD2,0xB6,0xC8,0xBF,0xC6,0xBC,0xBC,0
};


//上海诣阔科技
const U8 g_Enterprise4958Str[21]={
' ',' ',' ',' ',0xC9,0xCF,0xBA,0xA3,0xD2,0xE8,0xC0,0xAB,0xBF,0xC6,0xBC,0xBC,0
};
//三台奇玉广告
const U8 g_Enterprise7075Str[21]={
' ',' ',' ',' ',0xC8,0xFD,0xCC,0xA8,0xC6,0xE6,0xD3,0xF1,0xB9,0xE3,0xB8,0xE6,0
};
//绵阳崎升光电
const U8 g_Enterprise7076Str[21]={
' ',' ',' ',' ',0xC3,0xE0,0xD1,0xF4,0xC6,0xE9,0xC9,0xFD,0xB9,0xE2,0xB5,0xE7,0
};
//北京世纪瑞彩光电科技
const U8 g_Enterprise8476Str[21]={
0xB1,0xB1,0xBE,0xA9,0xCA,0xC0,0xBC,0xCD,0xC8,0xF0,0xB2,0xCA,0xB9,0xE2,0xB5,0xE7,0xBF,0xC6,0xBC,0xBC,0
};

//	8721：双惠电子
const U8 g_Enterprise8721Str[21]={
	' ',' ',' ',' ',' ',' ',0xCB,0xAB,0xBB,0xDD,0xB5,0xE7,0xD7,0xD3,0x0
};
//	8722：天河电子
const U8 g_Enterprise8722Str[21]={
	' ',' ',' ',' ',' ',' ',0xCC,0xEC,0xBA,0xD3,0xB5,0xE7,0xD7,0xD3,0x0
};
//	8723：员明电子
const U8 g_Enterprise8723Str[21]={
	' ',' ',' ',' ',' ',' ',0xD4,0xB1,0xC3,0xF7,0xB5,0xE7,0xD7,0xD3,0x0
};
//	8724：方普电子
const U8 g_Enterprise8724Str[21]={
	' ',' ',' ',' ',' ',' ',0xB7,0xBD,0xC6,0xD5,0xB5,0xE7,0xD7,0xD3,0x0
};
//	8725：博升科技
const U8 g_Enterprise8725Str[21]={
	' ',' ',' ',' ',' ',' ',0xB2,0xA9,0xC9,0xFD,0xBF,0xC6,0xBC,0xBC,0x0
};
//	8726：博安电子
const U8 g_Enterprise8726Str[21]={
	' ',' ',' ',' ',' ',' ',0xB2,0xA9,0xB0,0xB2,0xB5,0xE7,0xD7,0xD3,0x0
};

*/


////============内置企业logo==================




