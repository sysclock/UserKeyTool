#include "Init.h"
#include "stm32f10x_flash.h"

/* 2位16进制字符串转1字节 */
u8 strtohex8(u8 *pbuf)
{
  u8 i,j,k;
  if (pbuf[0] < 0x3a) i=pbuf[0]-0x30;else i=pbuf[0]-55;
  if (pbuf[1] < 0x3a) j=pbuf[1]-0x30;else j=pbuf[1]-55;
  k = (i<<4);
  k = k+j;
  return(k);
} 

/* 16进制字符串转2字节 */
u16 strtohex16(u8 *pbuf)
{
  u8 i,j,val1,val2;
  u16 val16;
  
  if (pbuf[0] < 0x3a) i=pbuf[0]-0x30;else i=pbuf[0]-55;
  if (pbuf[1] < 0x3a) j=pbuf[1]-0x30;else j=pbuf[1]-55;
  val1 = (i<<4);
  val1 = val1+j;
  
  if (pbuf[2] < 0x3a) i=pbuf[2]-0x30;else i=pbuf[2]-55;
  if (pbuf[3] < 0x3a) j=pbuf[3]-0x30;else j=pbuf[3]-55;
  val2 = (i<<4);
  val2 = val2+j;
  
  val16 = val1;
  val16 = (val16 << 8);
  val16 = val16+val2;
  return(val16);
}

/* 1字节转16进制字符串 */
void hextostr1(u8 val8,u8 *pbuf)
{
  u8 i;
  i = val8 >> 4;
  if (i<10) pbuf[0]=0x30+i;else pbuf[0]=55+i;
  i = (val8 & 0x0f);
  if (i<10) pbuf[1]=0x30+i;else pbuf[1]=55+i;
} 

/* CheckSum */
u8 CalculateCheckSum(u8 *pbuf,u16 buflen)
{
  u8 CheckSum = 0;
  u16 i16;
  
  for(i16=0;i16<buflen;i16++) CheckSum+=pbuf[i16];
  return(CheckSum);
}
