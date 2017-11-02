/****************************************************************************/
/*																			*/
/*				Copyright (C) 2013.  TRIOLION INC.							*/
/*		All rights reserved.  No part of this program may be reproduced.	*/
/*																			*/
/*==========================================================================*/
/*																			*/
/* MODULE:      t_InputBoardTVP5158Interface.c												*/
/*																			*/
/* USAGE:       This file lists all the Input Board TVP5158 Interface Board port table config			*/
/*																			*/
/* DESIGNER:	HAN LONG													*/
/*																			*/
/****************************************************************************/

#include "../System Code/type.h"
#include "../System Code/global.h"
#include "../Device/TVP5158/TVP5158.h"
#include "InputBoardTVP5158.h"

/****************************************************************************/
/*	G L O B A L    A D D R E S S    T A B L E								*/
/****************************************************************************/
const ST_INPUT_BOARD_TVP5158_SIGNAL_INDEX St_InputBoardTVP5158SignalIndex[] =
{
	// CVBS0
	{
		TVP5158_DEV0,
		TVP5158_DECODER0,
	},
	// CVBS1
	{
		TVP5158_DEV0,
		TVP5158_DECODER1,
	},
	// CVBS2
	{
		TVP5158_DEV0,
		TVP5158_DECODER2,
	},
	// CVBS3
	{
		TVP5158_DEV0,
		TVP5158_DECODER3,
	},
	// CVBS4
	{
		TVP5158_DEV1,
		TVP5158_DECODER0,
	},
	// CVBS5
	{
		TVP5158_DEV1,
		TVP5158_DECODER1,
	},
	// CVBS6
	{
		TVP5158_DEV1,
		TVP5158_DECODER2,
	},
	// CVBS7
	{
		TVP5158_DEV1,
		TVP5158_DECODER3,
	},
	// CVBS8
	{
		TVP5158_DEV2,
		TVP5158_DECODER1,
	},
	// CVBS9
	{
		TVP5158_DEV2,
		TVP5158_DECODER0,
	},
	// CVBS10
	{
		TVP5158_DEV2,
		TVP5158_DECODER3,
	},
	// CVBS11
	{
		TVP5158_DEV2,
		TVP5158_DECODER2,
	},
	// CVBS12
	{
		TVP5158_DEV3,
		TVP5158_DECODER0,
	},
	// CVBS13
	{
		TVP5158_DEV3,
		TVP5158_DECODER1,
	},
	// CVBS14
	{
		TVP5158_DEV3,
		TVP5158_DECODER2,
	},
	// CVBS15
	{
		TVP5158_DEV3,
		TVP5158_DECODER3,
	},
	// CVBS16
	{
		TVP5158_DEV4,
		TVP5158_DECODER0,
	},
	// CVBS17
	{
		TVP5158_DEV4,
		TVP5158_DECODER1,
	},
	// CVBS18
	{
		TVP5158_DEV4,
		TVP5158_DECODER2,
	},
	// CVBS19
	{
		TVP5158_DEV4,
		TVP5158_DECODER3,
	},
	// CVBS20
	{
		TVP5158_DEV5,
		TVP5158_DECODER1,
	},
	// CVBS21
	{
		TVP5158_DEV5,
		TVP5158_DECODER0,
	},
	// CVBS22
	{
		TVP5158_DEV5,
		TVP5158_DECODER3,
	},
	// CVBS23
	{
		TVP5158_DEV5,
		TVP5158_DECODER2,
	},

	{
		TVP5158_DEV_MAX,
		TVP5158_DECODER_MAX,
	}
};
