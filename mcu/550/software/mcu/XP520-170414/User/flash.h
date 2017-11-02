#ifndef _FLASH_H
#define _FLASH_H


//Flash 主存储区的后1K作为用户信息，程序大小不能超过127K

#define LOGO_STARTADDR  ((u32)0x0801FC00)
#define LOGO_ENDADDR      ((u32)0x08020000)

//#define TYPE_STARTADDR  ((u32)0x0801F800)
//#define TYPE_ENDADDR      ((u32)0x0801FC00)

#define PAGESIZE     ((u16)0x400)

void FlashEraserPage(u32 startaddr,u32 endaddr);
void FlashWriteLogo(void);
void FlashReadLogo(void);
//void FlashWriteType(void);
//void FlashReadType(void);

#endif

