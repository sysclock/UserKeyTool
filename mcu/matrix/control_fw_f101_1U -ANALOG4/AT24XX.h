#ifndef ATXX_H
#define ATXX_H

u8 AT24xxWrite(u16 Addr,u8* pBuf,u8 Count);
u8 AT24xxRead(u16 Addr,u8* pBuf,u16 Count);


#endif
