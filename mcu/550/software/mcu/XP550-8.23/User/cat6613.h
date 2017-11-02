#ifndef _Cat6613_
#define _Cat6613_

#include "common.h"

#define CAT6613ADDR 0x98
//#define CAT6613ADDR1 0x9A
#define P412_WADDR 0xde


unsigned char InitCAT6613(void);
unsigned char disableCAT6613(void);

unsigned char  InitP412(void);
unsigned char OUTPUT_HiZ_P412(void);

















#endif

