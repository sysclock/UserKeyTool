#ifndef _FLASH_H
#define _FLASH_H

//Flash 主存储区的后8K作为用户信息，程序大小不能超过56K
#define STARTADDR  ((u32)0x0800E000)
#define ENDADDR      ((u32)0x08010000)
#define PAGESIZE     ((u16)0x400)

void FlashTest(void);



//+++++++++++++++++++++++++++++++++++++++++++++++++




#define TYPE_STARTADDR  ((u32)0x0803F400)
#define TYPE_ENDADDR      ((u32)0x0803F800)

void FlashEraserPage(u32 startaddr,u32 endaddr);
void FlashWriteType(void);
void FlashReadType(void);


//+++++++++++++++++++++++++++++++++++++++++++++++++


#endif
