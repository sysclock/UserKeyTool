#ifndef MYLIB_H
#define MYLIB_H

u8 strtohex8(u8 *pbuf);
u16 strtohex16(u8 *pbuf);
void hextostr1(u8 val8,u8 *pbuf);
u8 CalculateCheckSum(u8 *pbuf,u16 buflen);
#endif