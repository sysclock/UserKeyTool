#include "Adv7441.h"
#include "header.h"


void InitAdv7441(void)
{
	//Device_adv7441Reset();
	//Device_adv7441OutputEnable(__FALSE);
/*	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x03,0x10);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x05,0x06);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x1d,0x40);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x37,0x00);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x68,0xf2);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xba,0xa0);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xc8,0x08);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x7b,0x0d);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xf4,0x3f);

	
	I2C_ByteWrite_ToDevice(USER_MAP2_ADDR,0xF0,0x10);
	I2C_ByteWrite_ToDevice(USER_MAP2_ADDR,0xF1,0x0F);
	I2C_ByteWrite_ToDevice(USER_MAP2_ADDR,0xF4,0x20);
         
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR,0x03,0x0c);
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR,0x15,0xEC);
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR,0x1C,0x58);
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR,0x1D,0x04);
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR,0x3C,0x42);
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR,0x3D,0x40);
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR,0x5A,0x01);    
	*/
/*	I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0x03, 0x04);//; Disable TOD
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0x05, 0x05);//; Prim_Mode =001b COMP
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0x06, 0x0A);//; VID_STD=1010b for 720P 1x1
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0x1D, 0x40);//; Disable TRI_LLC
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0x3C, 0xA8);//; SOG Sync level for atenuated 
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0x47, 0x0A);//; Enable Automatic PLL_Qpump and 
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0x67, 0x27);//; Embsynconall and INP_CSPACE now 
	
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0x6B, 0xE2);//; Setting cp_op_sel = 2 & Enable DE
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,  0x7b,0x04);
	//I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0x7B, 0x1D);//; Disable AV_Codes
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0x85, 0x19);//; Turn off SSPD and force SOY. 
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0xBA, 0xA0);//; Enable HDMI and Analog in
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0xF4, 0x3F);//; Max Drive Strength
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR, 0xC8, 0x08);//; Digital Fine Clamp Start position
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR, 0xF0,0x10);//; ADI Recommended Write
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR, 0xF1,0x0F);//; ADI Recommended Write
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR, 0xF4,0x20);//; ADI Recommended Write
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR, 0x15,0xEC);//; Disable these mute mask bits
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR, 0x1C,0x49);//; Set MCLKOUT to output 256fs
	I2C_ByteWrite_ToDevice(HDMI_MAP_ADDR, 0x1D,0x04);//;  & Set Unmute Delay to 1_5 sec.
	*/
	
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x03,0x11);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x05,0x06);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x06,0x02);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x1D,0x40);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x3C,0xA8);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x31,0x18);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x32,0x81);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x35,0x22);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x36,0x9E);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x37,0xA1);//0x00
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x47,0x0A);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x68,0xF2);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x6B,0xE2);//0xF2
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x7B,0x1D);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x7C,0x0F);//0x00	
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x7E,0xFF);//0x00
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xF4,0x1D);//0x88	
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x87,0x20);//0xE8
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x88,0x5A);//0x70
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x8F,0x80);//0x01
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xAB,0x00);//0x4E
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xAC,0x00);//0x20
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0xC8,0x08);
	I2C_ByteWrite_ToDevice(USER_MAP_ADDR,0x91,0x10);

	
	   
}






















