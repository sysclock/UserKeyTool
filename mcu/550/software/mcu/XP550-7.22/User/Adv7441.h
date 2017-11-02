#ifndef _ADT7441_
#define _ADT7441_

#include "common.h"

#define ADT7441_EDID_ADDR      0x6E
#define USER_MAP_ADDR           0x42
#define USER_MAP1_ADDR         0x46
#define USER_MAP2_ADDR         0x62
#define VDP_MAP_ADDR             0x4A
#define HDMI_MAP_ADDR           0x6A
#define KSV_MAP_ADDR              0x66


//#define U50_MAP_ADDR 0x28

void InitAdv7441(void);

#endif

