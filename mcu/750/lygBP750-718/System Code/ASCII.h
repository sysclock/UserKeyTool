 /*---------------------------------------------------------
ASCII.h

一些控制ASCII码的定义。
---------------------------------------------------------*/

#ifndef __ASCII_H__
#define __ASCII_H__

/*Constant*/

#define	ASCII_NUL		0x00
#define	ASCII_SOH		0x01		//start of heading
#define	ASCII_STX		0x02		//start of text
#define	ASCII_ETX		0x03		//end of text
#define	ASCII_EOT		0x04		//end of transmission
#define	ASCII_ENQ		0x05		//enquiry
#define	ASCII_ACK		0x06		//acknowledge
#define	ASCII_BEL		0x07		//bell
#define	ASCII_BS 		0x08		//backspace
#define	ASCII_TAB		0x09		//horizontal tab
#define	ASCII_LF 		0x0A		//NL line feed, new line
#define	ASCII_VT 		0x0B		//vertical tab
#define	ASCII_FF 		0x0C		//NP form feed, new page
#define	ASCII_CR 		0x0D		//carriage return
#define	ASCII_SO 		0x0E		//shift out
#define	ASCII_SI 		0x0F		//shift in
#define	ASCII_DLE		0x10		//data link escape
#define	ASCII_DC1		0x11		//device control 1
#define	ASCII_DC2		0x12		//device control 2
#define	ASCII_DC3		0x13		//device control 3
#define	ASCII_DC4		0x14		//device control 4
#define	ASCII_NAK		0x15		//negative acknowledge
#define	ASCII_SYN		0x16		//synchronous idle
#define	ASCII_ETB		0x17		//end of trans. block
#define	ASCII_CAN		0x18		//cancel
#define	ASCII_EM 		0x19		//end of medium
#define	ASCII_SUB		0x1A		//substitute
#define	ASCII_ESC		0x1B		//escape
#define	ASCII_FS 		0x1C		//file separator
#define	ASCII_GS  		0x1D		//group separator
#define	ASCII_RS  		0x1E		//record separator
#define	ASCII_US  		0x1F		//unit separator

#define	ASCII_BLK  		0x20		//

#define	ASCII_INC  		0x2B		//+
#define	ASCII_DEC  		0x2D		//-

#define	ASCII_LS 		0x3C		//<
#define	ASCII_GT  		0x3E		//>
#define	ASCII_A			0x41		//A
#define	ASCII_Z			0x5A		//Z
#define	ASCII_a			0x61		//a
#define	ASCII_z			0x7A		//Z

#define	ASCII_DEL  		0x7F 		//


#endif		//End of ASCII.h
