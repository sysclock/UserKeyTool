/*----------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: 'Retarget' layer for target-dependent low level functions
 * Note(s):
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <rt_misc.h>
#include "lpc17xx.h"
#include "global.h"

#pragma import(__use_no_semihosting_swi)

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int c, FILE *f) 
{
#if(DEBUG_OUT_UART_NUM == 1)
  while (!(LPC_UART1->LSR & 0x20)) ;
  LPC_UART1->THR = c;
#else
	while (!(LPC_UART0->LSR & 0x20)) ;
  LPC_UART0->THR = c;
#endif

  return (c);
}

int fgetc(FILE *f) 
{
#if(DEBUG_OUT_UART_NUM == 1)
	while (!(LPC_UART1->LSR & 0x01));
	return (LPC_UART1->RBR);
#else
	while (!(LPC_UART0->LSR & 0x01));
	return (LPC_UART0->RBR);
#endif
}

int ferror(FILE *f) 
{
  /* Your implementation of ferror */
  return EOF;
}

void _ttywrch(int c) 
{
#if	DEBUG_OUT_UART_NUM
	while (!(LPC_UART1->LSR & 0x20)) ;
	LPC_UART1->THR = c;
#else
	while (!(LPC_UART0->LSR & 0x20)) ;
	LPC_UART0->THR = c;
#endif
}

void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}
