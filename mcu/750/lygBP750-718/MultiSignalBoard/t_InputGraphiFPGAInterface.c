/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      t_InputGraphicFPGAInterface.c												*/
/*																			*/
/* USAGE:       This file lists all the Input Graphic FPGA Interface port table config			*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

#include "../System Code/type.h"
#include "../System Code/global.h"
#include "InputBoardGraphicFPGA.h"

/****************************************************************************/
/*	G L O B A L    A D D R E S S    T A B L E								*/
/****************************************************************************/
const ST_DEVICE_INPUT_GRAPHIC_FPGA_INTERFACE St_InputGraphicFPGAInterface[] =
{
	{
		//0---- 4 SiI9233 4 HDMI Input
		4,										// VideoInputNum
		INPUT_DEV_VDPA,		// VideoInputType
		7,										// Background0GTP(Background0 Switch Chip In Port index)
		6,										// Foreground0GTP(Foreground0 Switch Chip In Port index)
		5,										// Background1GTP(Background1 Switch Chip In Port index)
		4,										// Foreground1GTP(Foreground1 Switch Chip In Port index)
		3,										// Background2GTP(Background2 Switch Chip In Port index)
		2,										// Foreground2GTP(Foreground2 Switch Chip In Port index)
		1,										// Background3GTP(Background3 Switch Chip In Port index)
		0,										// Foreground3GTP(Foreground3 Switch Chip In Port index)
	},
	{
		//4---- 2 CNW6602 4 IP Input
		4,										// VideoInputNum
		INPUT_DEV_IP_DECODER,	// VideoInputType
		3,										// Background0GTP(Background0 Switch Chip In Port index)
		6,										// Foreground0GTP(Foreground0 Switch Chip In Port index)
		1,										// Background1GTP(Background1 Switch Chip In Port index)
		4,										// Foreground1GTP(Foreground1 Switch Chip In Port index)
		3,										// Background2GTP(Background2 Switch Chip In Port index)
		6,										// Foreground2GTP(Foreground2 Switch Chip In Port index)
		1,										// Background3GTP(Background3 Switch Chip In Port index)
		4,										// Foreground3GTP(Foreground3 Switch Chip In Port index)
	},
	{0}
};
