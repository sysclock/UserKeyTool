#ifndef _FLASH_H
#define _FLASH_H


//Flash 主存储区的后2K作为用户信息，程序大小不能超过126K

#define LOGO_STARTADDR  ((u32)0x0803FC00)
#define LOGO_ENDADDR    ((u32)0x08040000)

#define TYPE_STARTADDR  ((u32)0x0803F400)
#define TYPE_ENDADDR    ((u32)0x0803F800)


#define PWDFLAG_STARTADDR  ((u32)0x0803F800)
#define PWDFLAG_ENDADDR    ((u32)0x0803FC00)

#define PAGESIZE     ((u16)0x400)

void FlashEraserPage(unsigned int startaddr,unsigned int endaddr);
void FlashWriteLogo(void);
void FlashReadLogo(void);
void FlashWriteType(void);
void FlashReadType(void);

void FlashWritePWD_Flag(void);
void FlashReadPWD_Flag(void);

#endif
