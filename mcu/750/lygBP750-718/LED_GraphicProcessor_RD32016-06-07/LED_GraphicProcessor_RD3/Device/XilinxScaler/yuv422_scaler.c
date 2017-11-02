#include <string.h>
#include <stdio.h>
#include "../../System Code/lpc17xx.h"
#include "../../System Code/global.h"
#include "../../System Code/type.h"
#include "../../System Code/Timer.h"
#include "../../IIC Comm/D_IIC1.h"
#include "../SerialToParallel/STP_74HC595.h"
#include "../IIC_BusSwitch/PCA9546A.h"
#include "../InputFPGA/InputFPGA.h"
#include "../OutputFPGA/OutputFPGA.h"

#include "xil_types.h"
#include "bin1_tap4.h"
#include "yuv422_scaler.h"
#include "mscaler_hw.h"

#define XILINX_SCALER_DEBUG					1
/*
 * Scaler Device related data structures
 */
//XScaler ScalerInstance;			/* Device driver instance */
XScalerAperture Aperture;		/* Aperture setting */
XScalerStartFraction StartFraction;	/* Luma/Chroma Start Fraction setting*/
XScalerCoeffBank CoeffBank;		/* Coefficient bank */

#if	1	//	remove all <<xxx_xxx(_inp)>> functions
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Functions for Inptut Card Scaler
//
//		All <<xxx_xxx(_inp)>> functions are used for Input Board
//
////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t read_scaler_axi4_inp(uint32_t scaler_num, uint32_t axi_addr)
{
	WORD i = 0, j = 0;

	i = ((axi_addr & 0x0000001FF) << 8) + ((scaler_num & 0x00000003) << 2) + 0x00F1;
	InputFPGA_WriteWord(AXI4_COMMAND_REG, i);

	i = 0;
	j = 0;
	//	Check DATA Ready 100 times
	while(((j & 0x02000000) == 0) && (i < 100))
	{
		j = InputFPGA_ReadWord(AXI4_COMMAND_REG);
		i++;
	}

	if((j & 0x02000000) == 0)
	{
		#if(XILINX_SCALER_DEBUG > 0)
		printf("Read AXI4-Lite Port(Input Board) Error!!!\r\n");
		#endif
		return	0xFFFFFFFF;
	}
	else
	{
		j = InputFPGA_ReadWord(AXI4_READ_DATA_REG);
//		Delay1ms(5);
		#if(XILINX_SCALER_DEBUG == 2)
		printf("Read AXI4-Lite Port 0x%04X = 0x%08X.\r\n", axi_addr, j); 
		#endif
		return	j;
	}
}

uint32_t write_scaler_axi4_inp(uint32_t scaler_num, uint32_t axi_addr, uint32_t axi_data)
{
	uint32_t	i = 0, j = 0;

	InputFPGA_WriteWord(AXI4_WRITE_DATA_REG, axi_data);
	i = ((axi_addr & 0x0000001FF) << 8) + ((scaler_num & 0x00000003) << 2) + 0x00F3;
	InputFPGA_WriteWord(AXI4_COMMAND_REG, i);
	i = 0;
	j = 0;
	//	Check DATA Ready 100 times
	while(((j & 0x01000000) == 0) && (i < 100))
	{
		j = InputFPGA_ReadWord(AXI4_COMMAND_REG);
		i++;
	}

	if((j & 0x01000000) == 0)
	{
		#if(XILINX_SCALER_DEBUG > 0)
		printf("Write AXI4-Lite Port(Input Board) 0x%04X Error!!!\r\n", axi_addr); 
		#endif
		return	0xFFFFFFFF;
	}
	else
	{
		#if(XILINX_SCALER_DEBUG == 2)
		printf("Write AXI4-Lite Port 0x%04X = 0x%08X.\r\n", axi_addr, axi_data); 
		#endif
		return	0;
	}
}

void reset_scaler_axi4_inp(uint32_t scaler_num)
{
	uint32_t	i = 0;

	#if(XILINX_SCALER_DEBUG > 0)
	printf("Reset Input Board AXI4-Lite Port #%d!\r\n", scaler_num); 
	#endif

	i = BIT20 << scaler_num;
	InputFPGA_WriteWord(AXI4_COMMAND_REG, i);
	Delay1ms(5);
	InputFPGA_WriteWord(AXI4_COMMAND_REG, 0);
	Delay1ms(2);
}

void Init_Scaler_inp(void)
{
	BYTE i = 0;
	memset(&Aperture, 0, sizeof(Aperture));
	Aperture.InVertSize = 1080;						// < Vertical size of the input video
	Aperture.InHoriSize = 1920;						// < Horizontal size of the input video
	Aperture.InFirstLine = 1080 >> 1;			// < The first line index in the input video
	Aperture.InLastLine = 1079;						// < The last line index in the input video
	Aperture.InFirstPixel = 1920 >> 1;		// < The first pixel index in the input video
	Aperture.InLastPixel = 1919;					// < The last pixel index in the input video
	Aperture.OutVertSize = 1080;					// < Vertical size of the output video
	Aperture.OutHoriSize = 1920;					// < Horizontal size of the output video
	for(i = 0; i < MAX_INPUT_SCALER; i++)
	{
		reset_scaler_axi4_inp(i);
		reset_scaler_inp(i);
		ScalerSetup_inp(i, &Aperture);
	}
}
/*****************************************************************************/
/**
 * This function sets up aperture of a Scaler device.  The aperture setting
 * consists of input video aperture and output video size. This function
 * calculates the scale factor accordingly based on the aperture setting and
 * sets up the Scaler appropriately.
 *
 * @param  InstancePtr is a pointer to the Scaler device instance to be worked
 *		   on.
 * @param  AperturePtr points to the aperture setting structure to set up
 *		   the Scaler device.
 * @return XST_SUCCESS
 *
 *****************************************************************************/
uint32_t XScaler_SetAperture_inp(uint32_t scaler_num, XScalerAperture *AperturePtr)
{
	double VertScaleFactor;
	double HoriScaleFactor;
	uint32_t InSize;
	uint32_t InLine;
	uint32_t InPixel;
	uint32_t OutSize;
	uint32_t QuantizedHoriSize;
	uint32_t QuantizedVertSize;
	uint32_t QuantizedInLastPixel;
	uint32_t QuantizedInLastLine;

	/* Calculate vertical and horizontal scale factors */
	VertScaleFactor = (float)(AperturePtr->InLastLine - AperturePtr->InFirstLine + 1);
	HoriScaleFactor = (float)(AperturePtr->InLastPixel - AperturePtr->InFirstPixel + 1);

	VertScaleFactor /= (float)(AperturePtr->OutVertSize);
	HoriScaleFactor /= (float)AperturePtr->OutHoriSize;

	/* Convert HoriScaleFactor and VertScaleFactor values into a format
	 * to write to HSF and VSF registers.
	 */
	VertScaleFactor *= XSCL_SHRINK_FACTOR;
	HoriScaleFactor *= XSCL_SHRINK_FACTOR;

	/* Quantize Aperture - feed scale-factor back in to provide the
	 * actual aperture required to generate the desired number of output
	 * samples.
	 */
	QuantizedHoriSize = AperturePtr->OutHoriSize - 1;
	QuantizedHoriSize =	(uint32_t)(((float)QuantizedHoriSize * HoriScaleFactor) / XSCL_SHRINK_FACTOR);
	QuantizedHoriSize += 1 + ((XSCL_TAP_NUM + 1) >> 1);
	QuantizedInLastPixel = AperturePtr->InFirstPixel + QuantizedHoriSize - 1;
	if (QuantizedInLastPixel > AperturePtr->InLastPixel)
	{
		QuantizedInLastPixel = AperturePtr->InLastPixel;
	}

	QuantizedVertSize = AperturePtr->OutVertSize - 1;
	QuantizedVertSize =	(uint32_t)(((float)QuantizedVertSize * VertScaleFactor) / XSCL_SHRINK_FACTOR);
	QuantizedVertSize += 1 + ((XSCL_TAP_NUM + 1) >> 1);
	QuantizedInLastLine = AperturePtr->InFirstLine + QuantizedVertSize - 1;
	if (QuantizedInLastLine > AperturePtr->InLastLine)
	{
		QuantizedInLastLine = AperturePtr->InLastLine;
	}

	/* Calculate input line, pixel and output size values */
	InLine = AperturePtr->InFirstLine & XSCL_APTVERT_FIRSTLINE_MASK;
	InLine |= (QuantizedInLastLine << XSCL_APTVERT_LASTLINE_SHIFT) & XSCL_APTVERT_LASTLINE_MASK;
	InPixel = AperturePtr->InFirstPixel & XSCL_APTHORI_FIRSTPXL_MASK;
	InPixel |= (QuantizedInLastPixel << XSCL_APTHORI_LASTPXL_SHIFT)	& XSCL_APTHORI_LASTPXL_MASK;
	InSize = AperturePtr->InHoriSize & XSCL_OUTSIZE_NUMPXL_MASK;
	InSize |= (AperturePtr->InVertSize << XSCL_OUTSIZE_NUMLINE_SHIFT) & XSCL_OUTSIZE_NUMLINE_MASK;
	OutSize = AperturePtr->OutHoriSize & XSCL_OUTSIZE_NUMPXL_MASK;
	OutSize |= (AperturePtr->OutVertSize << XSCL_OUTSIZE_NUMLINE_SHIFT) & XSCL_OUTSIZE_NUMLINE_MASK;
/*
	DEBUG_OUT(("Input Board : Scaler #%d : Inline=0x%08X, InPixel=0x%08X, InSize=0x%08X, OutSize=0x%08X.\r\n", 
				scaler_num, InLine, InPixel, InSize, OutSize));
	DEBUG_OUT(("Input Board : Scaler #%d : HoriScaleFactor=0x%08X, VertScaleFactor=0x%08X.\r\n", 
				scaler_num, (uint32_t)(round(HoriScaleFactor)),(uint32_t)(round(VertScaleFactor))));
*/	
	/* Set up aperture related register in the Scaler */
	write_scaler_axi4_inp(scaler_num, XSCL_APTVERT, InLine);
	write_scaler_axi4_inp(scaler_num, XSCL_APTHORI, InPixel);
	write_scaler_axi4_inp(scaler_num, XSCL_INSIZE, InSize);
	write_scaler_axi4_inp(scaler_num, XSCL_OUTSIZE, OutSize);
	write_scaler_axi4_inp(scaler_num, XSCL_HSF, (uint32_t)(round(HoriScaleFactor)));
	write_scaler_axi4_inp(scaler_num, XSCL_VSF, (uint32_t)(round(VertScaleFactor)));

	return 0;	//XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This function gets aperture of a Scaler device. The aperture setting
 * consists of input video aperture and output video size.
 *
 * @param  InstancePtr is a pointer to the Scaler device instance to be worked
 *	   on.
 * @param  AperturePtr points to the aperture structure to store the current
 *	   Scaler device setting after this function returns.
 * @return None
 *
 *****************************************************************************/
void XScaler_GetAperture_inp(uint32_t scaler_num, XScalerAperture *AperturePtr)
{
	uint32_t InLine;
	uint32_t InPixel;
	uint32_t OutSize;

	/* Read the first/last line and pixel info for input side and
	 * vertical/horizontal size for output size
	 */
	InLine = read_scaler_axi4_inp(scaler_num, XSCL_APTVERT);
	InPixel = read_scaler_axi4_inp(scaler_num, XSCL_APTHORI);
	OutSize = read_scaler_axi4_inp(scaler_num, XSCL_OUTSIZE);

	/* Parse the info and populate the aperture structure */
	AperturePtr->InFirstLine = InLine & XSCL_APTVERT_FIRSTLINE_MASK;
	AperturePtr->InLastLine = (InLine & XSCL_APTVERT_LASTLINE_MASK) >> XSCL_APTVERT_LASTLINE_SHIFT;

	AperturePtr->InFirstPixel = InPixel & XSCL_APTHORI_FIRSTPXL_MASK;
	AperturePtr->InLastPixel = (InPixel & XSCL_APTHORI_LASTPXL_MASK) >>	XSCL_APTHORI_LASTPXL_SHIFT;

	AperturePtr->OutHoriSize = OutSize & XSCL_OUTSIZE_NUMPXL_MASK;
	AperturePtr->OutVertSize = (OutSize & XSCL_OUTSIZE_NUMLINE_MASK) >>	XSCL_OUTSIZE_NUMLINE_SHIFT;

	return;
}

/*****************************************************************************/
/**
 * This function sets up Luma and Chroma start fractional values used by a
 * Scaler device.
 *
 * @param  InstancePtr is a pointer to the Scaler device instance to be worked
 *	   on.
 * @param  StartFractionPtr is a pointer to a start fractional value set to be
 *	   used.
 * @return None
 *
 *****************************************************************************/
void XScaler_SetStartFraction_inp(uint32_t scaler_num, XScalerStartFraction *StartFractionPtr)
{
	/* Set up the fractional values */
	write_scaler_axi4_inp(scaler_num, XSCL_HPA_Y,
			 (uint32_t)StartFractionPtr->LumaLeftHori & XSCL_FRCTLUMALEFT_VALUE_MASK);

	write_scaler_axi4_inp(scaler_num, XSCL_HPA_C, 
			 (uint32_t)StartFractionPtr->ChromaLeftHori & XSCL_FRCTCHROMALEFT_VALUE_MASK);

	write_scaler_axi4_inp(scaler_num, XSCL_VPA_Y,
			 (uint32_t)StartFractionPtr->LumaTopVert & XSCL_FRCTLUMATOP_VALUE_MASK);

	write_scaler_axi4_inp(scaler_num, XSCL_VPA_C,
			 (uint32_t)StartFractionPtr->ChromaTopVert & XSCL_FRCTCHROMATOP_VALUE_MASK);

	return;
}

/*****************************************************************************/
/**
 * This function gets Luma and Chroma start fractional values currently used
 * by a Scaler device.
 *
 * @param  InstancePtr is a pointer to the Scaler device instance to be worked
 *	   on.
 * @param  StartFractionPtr is a pointer to a start fractional value structure
 *	   to be populated with the fractional values after this function
 *	   returns.
 * @return None
 *
 *****************************************************************************/
void XScaler_GetStartFraction_inp(uint32_t scaler_num, XScalerStartFraction *StartFractionPtr)
{
	/* Fetch the fractional values */
	StartFractionPtr->LumaLeftHori = (s32) read_scaler_axi4_inp(scaler_num, XSCL_HPA_Y)
			& XSCL_FRCTLUMALEFT_VALUE_MASK;

	StartFractionPtr->LumaTopVert = (s32) read_scaler_axi4_inp(scaler_num, XSCL_VPA_Y)
			& XSCL_FRCTLUMATOP_VALUE_MASK;

	StartFractionPtr->ChromaLeftHori = (s32) read_scaler_axi4_inp(scaler_num, XSCL_HPA_C)
			& XSCL_FRCTCHROMALEFT_VALUE_MASK;

	StartFractionPtr->ChromaTopVert = (s32)	read_scaler_axi4_inp(scaler_num, XSCL_VPA_C)
			& XSCL_FRCTCHROMATOP_VALUE_MASK;

	return;
}

/*****************************************************************************/
/**
 * This function sets the numbers of vertical and horizontal phases to be used
 * by a Scaler device.
 *
 * @param  InstancePtr is a pointer to the Scaler device instance to be worked
 *		   on.
 * @param  VertPhaseNum is the number of vertical phase to set to
 * @param  HoriPhaseNum is the number of horizontal phase to set to
 * @return None
 *
 *****************************************************************************/
void XScaler_SetPhaseNum_inp(uint32_t scaler_num, u16 VertPhaseNum, u16 HoriPhaseNum)
{
	u32 PhaseRegValue;

	/* Calculate the value to write to "Number of Phases Register" */
	PhaseRegValue =	(VertPhaseNum << XSCL_NUMPHASE_VERT_SHIFT) & XSCL_NUMPHASE_VERT_MASK;
	PhaseRegValue |= HoriPhaseNum & XSCL_NUMPHASE_HORI_MASK;

	/* Set up the Scaler core using the numbers of phases */
	write_scaler_axi4_inp(scaler_num, XSCL_NUMPHASE, PhaseRegValue);

	return;
}

/*****************************************************************************/
/**
*
* This function chooses the active vertical and horizontal coefficient sets to
* be used by a Scaler device.
*
* Each coefficient set contains 4 banks: Horizontal Luma, Horizontal Chroma,
* Vertical Luma and Vertical Chroma. The horizontal part is independent from
* the vertical part and the Scaler device supports using the horizontal part
* of one coefficient set w/ the vertical part of a different coefficient set.
*
* @param  InstancePtr is a pointer to the Scaler device instance to be worked
*	  on
*
* @param  VertSetIndex indicates the index of the coefficient set in which
*	  the vertical part will be used by the Scaler device. Valid value
*	  is from 0 to (the number of the coefficient sets implemented by the
*	  Scaler device - 1)
*
* @param  HoriSetIndex indicates the index of the coefficient set in which
*	  the horizontal part will be used by the Scaler device. Valid value
*	  is from 0 to (the number of the coefficient sets implemented by the
*	  Scaler device - 1)
*
* @return None.
*
* @note	  None.
*
******************************************************************************/
void XScaler_SetActiveCoeffSet_inp(uint32_t scaler_num,	u8 VertSetIndex, u8 HoriSetIndex)
{
	u32 RegValue;

	RegValue = ((u32)HoriSetIndex) & XSCL_COEFFSETS_HORI_MASK;
	RegValue |= (((u32)VertSetIndex) << XSCL_COEFFSETS_VERT_SHIFT) & XSCL_COEFFSETS_VERT_MASK;

	write_scaler_axi4_inp(scaler_num, XSCL_COEFFSETS, RegValue);

	return;
}

/*****************************************************************************/
/**
 * This function loads a coefficient bank to the Scaler core. A complete
 * coefficient set contains 4 banks (if Luma, Chroma, Horizontal and Vertical
 * filter operations do not share common banks. For more details see
 * XScaler_GetCoeffBankSharingInfo()): Horizontal Luma, Horizontal Chroma,
 * Vertical Luma and Vertical Chroma. all 4 banks must be loaded back to back
 * in the order listed here. The caller is responsible for ensuring the
 * sequence and this function does not check it.
 *
 * An example sequence to load an whole coefficient set is like:
 * <pre>
 *	   XScaler_LoadCoeffBank(&Scaler, &HoriLumaCoeffBank);
 *	   XScaler_LoadCoeffBank(&Scaler, &HoriChromaCoeffBank);
 *	   XScaler_LoadCoeffBank(&Scaler, &VertLumaCoeffBank);
 *	   XScaler_LoadCoeffBank(&Scaler, &VertChromaCoeffBank);
 * </pre>
 *
 * @param  InstancePtr is a pointer to the Scaler device instance to be worked
 *	   on.
 * @param  CoeffBankPtr is a pointer to a coefficient bank that is to be loaded
 * @return None
 *
 *****************************************************************************/
void XScaler_LoadCoeffBank_inp(uint32_t scaler_num, XScalerCoeffBank *CoeffBankPtr)
{
	int PhaseIndex;
	int TapIndex;

	s16 *CoeffValueTapBase;
	u32 CoeffValue;
//???
	//Start the coefficient bank loading by writing the bank index first
//	write_scaler_axi4(scaler_num, XSCL_COEFFSETADDR, CoeffBankPtr->SetIndex & XSCL_COEFFSETADDR_ADDR_MASK);

	/* Now load the valid values */
	CoeffValueTapBase = CoeffBankPtr->CoeffValueBuf;
	
	for (PhaseIndex = 0; PhaseIndex < CoeffBankPtr->PhaseNum; PhaseIndex++) 
	{
		for (TapIndex = 0; TapIndex < CoeffBankPtr->TapNum; ) 
		{
			CoeffValue =  ((u32)CoeffValueTapBase[TapIndex++]) & 0xFFFF;
			
			if (TapIndex < CoeffBankPtr->TapNum) 
			{
				CoeffValue |= (((u32)CoeffValueTapBase[TapIndex++]) & 0xFFFF) << 16;
			}

			write_scaler_axi4_inp(scaler_num, XSCL_COEFFVALUE, CoeffValue);
		}

		CoeffValueTapBase +=  CoeffBankPtr->TapNum;
	}

	//Load padding if the real phase number is less than the maximum phase	number
	for (PhaseIndex = CoeffBankPtr->PhaseNum; PhaseIndex < MAX_PHASE_NUM; PhaseIndex++) 
	{
		for (TapIndex = 0; TapIndex < (CoeffBankPtr->TapNum + 1) / 2; TapIndex++) 
		{
			write_scaler_axi4_inp(scaler_num, XSCL_COEFFVALUE, 0);
		}
	}

	return;
}

/*****************************************************************************/
/**
 * This function returns the pointer to the coefficients for a scaling
 * operation given input/output sizes and the Tap and Phase numbers.
 *
 * @param  InSize indicates the size (width or height) of the input video.
 * @param  OutSize indicates the size (width or height) of the output video.
 * @param  Tap indicates the Tap number.
 * @param  Phase indicates the Phase number.
 * @return The points to the coefficients ready for the scaling operation.
 * @note   None.
 *			
 *****************************************************************************/
const s16 *XScaler_CoefValueLookup_inp(u32 InSize, u32 OutSize, u32 Phase)
{
	u32 CoeffBinIndex;
	u32 CoeffValueOffset;

	/* Find the index of the Coefficient Bin */
	if (OutSize > InSize)
	{
		CoeffBinIndex = 0;
	}
	else
	{
		CoeffBinIndex = OutSize * 16 / InSize;
	}

	/* Find the offset of the Coefficients within the Bin */
	CoeffValueOffset = 	(((Phase * (Phase + 1)) / 2) - 1) * XSCL_TAP_NUM;

	return &(XScaler_Coefficients[CoeffBinIndex][CoeffValueOffset]);
}

/*****************************************************************************/
/**
*
* This function sets up the scaler core for the feature demonstrations below.
* After the execution of this function, a set of coefficient value (containing
* 2 vertical and 2 horizontal coefficient banks) are loaded; aperture is set
* up and the scaling operation is started. This function is utilized by
* DownScale() and UpScale().
*
* @param	ScalerInstPtr is the pointer to the instance of the Scaler
*		device.
*
* @param	ScalerInWidth is the width of the input aperture.
*
* @param	ScalerInHeight is the height of the input aperture.
*
* @param	ScalerOutWidth is the width of the output aperture.
*
* @param	ScalerOutHeight is the height of the output aperture.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void ScalerSetup_inp(uint32_t scaler_num, XScalerAperture *AperturePtr)
{
//	int i;
//	u8 ChromaFormat;
	u8 ChromaLumaShareCoeffBank;
	u8 HoriVertShareCoeffBank;

//	DEBUG_OUT(("\r\n>>ScalerSetup : \r\nScaler #%d : Version = 0x%08X!\r\n", scaler_num, read_scaler_axi4((scaler_num), XSCL_VERSION)));

	/*
	 * Disable the scaler before setup and tell the device not to pick up
	 * the register updates until all are done
	 */
	XScaler_Disable_inp(scaler_num);
//	XScaler_Enable_inp(scaler_num);
	XScaler_DisableRegUpdate_inp(scaler_num);
/*
	XScaler_IRQ_Enable_inp(scaler_num);

	DEBUG_OUT(("Waiting for Coeff. FIFO Ready!\r\n"));
	while ((read_scaler_axi4(scaler_num, XSCL_STS) & 0x00000001 == 0));
	DEBUG_OUT(("Ready to Write Coeff. FIFO!\r\n"));
*/
/////////////////////////////////
	XScaler_SetPhaseNum_inp(scaler_num, COEFF_TAP_NUM, COEFF_PHASE_NUM);
	XScaler_SetActiveCoeffSet_inp(scaler_num, 0, 0);

	ChromaLumaShareCoeffBank = 1;		//	Share Luma/Chroma Coeff.
	HoriVertShareCoeffBank = 0;			//	Not Share Horizontal/Vertical Coeff.

	CoeffBank.SetIndex 	= COEFF_SET_INDEX;
	CoeffBank.PhaseNum 	= COEFF_PHASE_NUM;
	CoeffBank.TapNum 	= COEFF_TAP_NUM;
	/* Locate coefficients for Horizontal scaling */
	CoeffBank.CoeffValueBuf = (s16 *) XScaler_CoefValueLookup_inp(AperturePtr->InHoriSize, AperturePtr->OutHoriSize, CoeffBank.PhaseNum);

//	Set Coeff Addr
	write_scaler_axi4_inp(scaler_num, XSCL_COEFFSETADDR, CoeffBank.SetIndex & XSCL_COEFFSETADDR_ADDR_MASK);
//
	/* Load coefficient bank for Horizontal Luma */
	XScaler_LoadCoeffBank_inp(scaler_num, &CoeffBank);

	/* Horizontal Chroma bank is loaded only if chroma/luma sharing flag is not set */
	if (!ChromaLumaShareCoeffBank)
	{
		XScaler_LoadCoeffBank_inp(scaler_num, &CoeffBank);
	}

	// Vertical coeff banks are loaded only if horizontal/vertical sharing flag is not set
	if (!HoriVertShareCoeffBank) 
	{
		/* Locate coefficients for Vertical scaling */
		CoeffBank.CoeffValueBuf = (s16 *) XScaler_CoefValueLookup_inp(AperturePtr->InVertSize, AperturePtr->OutVertSize, CoeffBank.PhaseNum);

		/* Load coefficient bank for Vertical Luma */
		XScaler_LoadCoeffBank_inp(scaler_num, &CoeffBank);

		/* Vertical Chroma coeff bank is loaded only if chroma/luma
		 * sharing flag is not set	*/
		if (!ChromaLumaShareCoeffBank)
		{
			XScaler_LoadCoeffBank_inp(scaler_num, &CoeffBank);
		}
	}
	
	if	((read_scaler_axi4_inp(scaler_num, XSCL_STSERR) & 0x00000010))
	{
		#if(XILINX_SCALER_DEBUG >= 1)
		printf("Input Board : Scaler #%d : Coeff. FIFO Write Error!!!\r\n", scaler_num);
		#endif
	}
	else
 	{
		#if(XILINX_SCALER_DEBUG >= 1)
		printf("Input Board : Scaler #%d : Coeff. FIFO Write Successfully!\r\n", scaler_num);
		#endif
	}

	//Load phase-offsets into scaler
	StartFraction.LumaLeftHori = 0;
	StartFraction.LumaTopVert = 0;
	StartFraction.ChromaLeftHori = 0;
	StartFraction.ChromaTopVert = 0;
	XScaler_SetStartFraction_inp(scaler_num, &StartFraction);
/*
	//Set up Aperture.
	Aperture.InFirstLine = 0;
	Aperture.InLastLine = ScalerInHeight - 1;

	Aperture.InFirstPixel = 0;
	Aperture.InLastPixel = ScalerInWidth - 1;

	Aperture.OutVertSize = ScalerInHeight;
	Aperture.OutHoriSize = ScalerInWidth;

	Aperture.OutVertSize = ScalerOutHeight;
	Aperture.OutHoriSize = ScalerOutWidth;
*/
	XScaler_SetAperture_inp(scaler_num, AperturePtr);

//	XScaler_GetAperture_inp(scaler_num, &Aperture);
/*	
	//Set up phases
	XScaler_SetPhaseNum_inp(scaler_num, COEFF_PHASE_NUM, COEFF_PHASE_NUM);
//	XScaler_SetPhaseNum_inp(scaler_num, 0, COEFF_PHASE_NUM);

	//Choose active set indexes for both vertical and horizontal directions
	XScaler_SetActiveCoeffSet_inp(scaler_num, COEFF_SET_INDEX, COEFF_SET_INDEX);
//	XScaler_SetActiveCoeffSet_inp(scaler_num, 0, 0);
*/
/*
DEBUG_OUT(("\r\n<1>Read AXI4-Lite Registers : \r\n"));
for	(i = 0; i < 16; i++)
{
	read_scaler_axi4_inp(scaler_num, 0x0100 + (i * 4));
}
*/
	//Enable the scaling operation
	XScaler_EnableRegUpdate_inp(scaler_num);
	XScaler_Enable_inp(scaler_num);
/*
DEBUG_OUT(("\r\n<2>Read AXI4-Lite Registers : \r\n"));
for	(i = 0; i < 16; i++)
{
	read_scaler_axi4_inp(scaler_num, 0x0100 + (i * 4));
}
*/
	return;
}

void reset_scaler_inp(uint32_t scaler_num)
{
//	uint32_t	i;

//	DEBUG_OUT(("\r\nReset Scaler #%d!\r\n", scaler_num));

	write_scaler_axi4_inp((scaler_num), XSCL_CTL, 0x80000000);

	Delay1ms(5);

	write_scaler_axi4_inp((scaler_num), XSCL_CTL, 0);
	Delay1ms(2);
}

void print_out_coeff_inp(uint32_t scaler_num)
{
	uint32_t	i, j, k;

//	DEBUG_OUT(("\r\n>>print_out_coeff : \r\nScaler #%d : Version = 0x%08X!\r\n", scaler_num, read_scaler_axi4((scaler_num), XSCL_VERSION)));

//	XScaler_Disable_inp(scaler_num);
//	XScaler_Enable_inp(scaler_num);

//	XScaler_IRQ_Enable_inp(scaler_num);

//	XScaler_CoeffRD_Disable_inp(scaler_num);	
	write_scaler_axi4_inp(scaler_num, XSCL_CTL, 0x00000008);	

	write_scaler_axi4_inp(scaler_num, XSCL_COEFFBANKADDR, 0);	

	XScaler_CoeffRD_Enable_inp(scaler_num);

	#if(XILINX_SCALER_DEBUG == 2)
	printf("Input Board : Waiting to read Coefficients!\r\n");
	#endif
	//	Waiting
	//for (i = 0; i < 100000; i++);
	Delay10ms(1);

	k = 0;
	while ((k & 0x00000020) == 0)
		k = InputFPGA_ReadWord(AXI4_INTC_IF_REG + scaler_num);
	#if(XILINX_SCALER_DEBUG == 2)
	printf("Ready to read Coefficients!\r\n");
	#endif

	for (i = 0; i < COEFF_PHASE_NUM; i++)
	{
//		DEBUG_OUT(("PHASE #[%d] : ", i));
		for (j = 0; j < COEFF_TAP_NUM; j++)
		{
			// Bank Addr
			write_scaler_axi4_inp(scaler_num, XSCL_COEFFBANKADDR, 0);	
			//	Tap, Phase Address
			k = (i << 8) + j;
			write_scaler_axi4_inp(scaler_num, XSCL_COEFF_RDADDR, k);
			
			k = read_scaler_axi4_inp(scaler_num, XSCL_COEFFDATAOUT);
//			DEBUG_OUT(("0x%04X ", k));
		}
//		DEBUG_OUT(("\r\n"));
	}
	
	write_scaler_axi4_inp(scaler_num, XSCL_CTL, 0x00000000);	
//	XScaler_CoeffRD_Disable_inp(scaler_num);	
//	XScaler_Enable_inp(scaler_num);
//	XScaler_Disable_inp(scaler_num);
}

#endif	//	remove all <<xxx_xxx(_inp)>> functions

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Functions for Ouptut Card Scaler
//
////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t read_scaler_axi4(uint32_t scaler_num, uint32_t axi_addr)
{
	uint32_t	i = 0, j = 0;

//	Set_I2C_OUT_FPGA(board_num);
	
	i = ((axi_addr & 0x0000001FF) << 8) + ((scaler_num & 0x00000003) << 2) + 0x00F1;
	OutputFPGA_WriteWord(AXI4_COMMAND_REG, i);

	i = 0;
	j = 0;
	//	Check DATA Ready 100 times
	while(((j & 0x02000000) == 0) && (i < 100))
	{
		j = OutputFPGA_ReadWord(AXI4_COMMAND_REG);
		i++;
	}

	if((j & 0x02000000) == 0)
	{
		#if(XILINX_SCALER_DEBUG > 0)
		printf("Read AXI4-Lite Port Error!!!\r\n");
		#endif
		return	0xFFFFFFFF;
	}
	else
	{
		j = OutputFPGA_ReadWord(AXI4_READ_DATA_REG);
		#if(XILINX_SCALER_DEBUG == 2)
		printf("Read AXI4-Lite Port 0x%04X = 0x%08X.\r\n", axi_addr, j);
		#endif
		return	j;
	}
}

uint32_t write_scaler_axi4(uint32_t scaler_num, uint32_t axi_addr, uint32_t axi_data)
{
	uint32_t	i = 0, j = 0;
	OutputFPGA_WriteWord(AXI4_WRITE_DATA_REG, axi_data);
	i = ((axi_addr & 0x0000001FF) << 8) + ((scaler_num & 0x00000003) << 2) + 0x00F3;
	OutputFPGA_WriteWord(AXI4_COMMAND_REG, i);
	i = 0;
	j = 0;
	//	Check DATA Ready 100 times
	while(((j & 0x01000000) == 0) && (i < 100))
	{
		j = OutputFPGA_ReadWord(AXI4_COMMAND_REG);
		i++;
	}

	if((j & 0x01000000) == 0)
	{
		#if(XILINX_SCALER_DEBUG > 0)
		printf("Write AXI4-Lite Port 0x%04X Error!!!\r\n", axi_addr); 
		#endif
		return	0xFFFFFFFF;
	}
	else
	{
		#if(XILINX_SCALER_DEBUG == 2)
		prnitf("Write AXI4-Lite Port 0x%04X = 0x%08X.\r\n", axi_addr, axi_data); 
		#endif
		return	0;
	}
}

void reset_scaler_axi4(uint32_t scaler_num)
{
	uint32_t	i = 0;
	#if(XILINX_SCALER_DEBUG > 0)
	printf("Reset Ouptput Board AXI4-Lite Port #%d!\r\n", scaler_num); 
	#endif
	i = 0x00100000 << scaler_num;
	OutputFPGA_WriteWord(AXI4_COMMAND_REG, i);
	Delay1ms(5);
	OutputFPGA_WriteWord(AXI4_COMMAND_REG, 0);
}

/*****************************************************************************/
/**
 * This function sets up aperture of a Scaler device.  The aperture setting
 * consists of input video aperture and output video size. This function
 * calculates the scale factor accordingly based on the aperture setting and
 * sets up the Scaler appropriately.
 *
 * @param  InstancePtr is a pointer to the Scaler device instance to be worked
 *		   on.
 * @param  AperturePtr points to the aperture setting structure to set up
 *		   the Scaler device.
 * @return XST_SUCCESS
 *
 *****************************************************************************/
uint32_t XScaler_SetAperture(uint32_t scaler_num, XScalerAperture *AperturePtr)
{
	double VertScaleFactor;
	double HoriScaleFactor;
	uint32_t InSize;
	uint32_t InLine;
	uint32_t InPixel;
	uint32_t OutSize;
	uint32_t QuantizedHoriSize;
	uint32_t QuantizedVertSize;
	uint32_t QuantizedInLastPixel;
	uint32_t QuantizedInLastLine;

	/* Calculate vertical and horizontal scale factors */
	VertScaleFactor = (float)(AperturePtr->InLastLine - AperturePtr->InFirstLine + 1);
	HoriScaleFactor = (float)(AperturePtr->InLastPixel - AperturePtr->InFirstPixel + 1);

	VertScaleFactor /= (float)(AperturePtr->OutVertSize);
	HoriScaleFactor /= (float)AperturePtr->OutHoriSize;

	/* Convert HoriScaleFactor and VertScaleFactor values into a format
	 * to write to HSF and VSF registers.
	 */
	VertScaleFactor *= XSCL_SHRINK_FACTOR;
	HoriScaleFactor *= XSCL_SHRINK_FACTOR;

	/* Quantize Aperture - feed scale-factor back in to provide the
	 * actual aperture required to generate the desired number of output
	 * samples.
	 */
	QuantizedHoriSize = AperturePtr->OutHoriSize - 1;
	QuantizedHoriSize =	(uint32_t)(((float)QuantizedHoriSize * HoriScaleFactor) / XSCL_SHRINK_FACTOR);
//	QuantizedHoriSize += 1 + (InstancePtr->Config.HoriTapNum + 1) / 2;
	QuantizedHoriSize += 1 + ((XSCL_TAP_NUM + 1) >> 1);
	QuantizedInLastPixel = AperturePtr->InFirstPixel + QuantizedHoriSize - 1;
	if (QuantizedInLastPixel > AperturePtr->InLastPixel)
	{
		QuantizedInLastPixel = AperturePtr->InLastPixel;
	}

	QuantizedVertSize = AperturePtr->OutVertSize - 1;
	QuantizedVertSize =	(uint32_t)(((float)QuantizedVertSize * VertScaleFactor) / XSCL_SHRINK_FACTOR);
//	QuantizedVertSize += 1 + (InstancePtr->Config.VertTapNum + 1) / 2;
	QuantizedVertSize += 1 + ((XSCL_TAP_NUM + 1) >> 1);
	QuantizedInLastLine = AperturePtr->InFirstLine + QuantizedVertSize - 1;
	if (QuantizedInLastLine > AperturePtr->InLastLine)
	{
		QuantizedInLastLine = AperturePtr->InLastLine;
	}

	/* Calculate input line, pixel and output size values */
	InLine = AperturePtr->InFirstLine & XSCL_APTVERT_FIRSTLINE_MASK;
	InLine |= (QuantizedInLastLine << XSCL_APTVERT_LASTLINE_SHIFT) & XSCL_APTVERT_LASTLINE_MASK;
	InPixel = AperturePtr->InFirstPixel & XSCL_APTHORI_FIRSTPXL_MASK;
	InPixel |= (QuantizedInLastPixel << XSCL_APTHORI_LASTPXL_SHIFT)	& XSCL_APTHORI_LASTPXL_MASK;
	InSize = AperturePtr->InHoriSize & XSCL_OUTSIZE_NUMPXL_MASK;
	InSize |= (AperturePtr->InVertSize << XSCL_OUTSIZE_NUMLINE_SHIFT) & XSCL_OUTSIZE_NUMLINE_MASK;
	OutSize = AperturePtr->OutHoriSize & XSCL_OUTSIZE_NUMPXL_MASK;
	OutSize |= (AperturePtr->OutVertSize << XSCL_OUTSIZE_NUMLINE_SHIFT) & XSCL_OUTSIZE_NUMLINE_MASK;

	#if(XILINX_SCALER_DEBUG == 2)
	printf("Inline=0x%08X, InPixel=0x%08X, InSize=0x%08X, OutSize=0x%08X.\r\n", InLine, InPixel, InSize, OutSize);
	printf("HoriScaleFactor=0x%08X, VertScaleFactor=0x%08X.\r\n",
			(uint32_t)(round(HoriScaleFactor)),(uint32_t)(round(VertScaleFactor)));
	#endif
	
	/* Set up aperture related register in the Scaler */
	write_scaler_axi4(scaler_num, XSCL_APTVERT, InLine);
	write_scaler_axi4(scaler_num, XSCL_APTHORI, InPixel);
	write_scaler_axi4(scaler_num, XSCL_INSIZE, InSize);
	write_scaler_axi4(scaler_num, XSCL_OUTSIZE, OutSize);
	write_scaler_axi4(scaler_num, XSCL_HSF, (uint32_t)(round(HoriScaleFactor)));
	write_scaler_axi4(scaler_num, XSCL_VSF, (uint32_t)(round(VertScaleFactor)));

	return 0;	//XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This function gets aperture of a Scaler device. The aperture setting
 * consists of input video aperture and output video size.
 *
 * @param  InstancePtr is a pointer to the Scaler device instance to be worked
 *	   on.
 * @param  AperturePtr points to the aperture structure to store the current
 *	   Scaler device setting after this function returns.
 * @return None
 *
 *****************************************************************************/
void XScaler_GetAperture(uint32_t scaler_num, XScalerAperture *AperturePtr)
{
	uint32_t InLine;
	uint32_t InPixel;
	uint32_t OutSize;

	/* Read the first/last line and pixel info for input side and
	 * vertical/horizontal size for output size
	 */
	InLine = read_scaler_axi4(scaler_num, XSCL_APTVERT);
	InPixel = read_scaler_axi4(scaler_num, XSCL_APTHORI);
	OutSize = read_scaler_axi4(scaler_num, XSCL_OUTSIZE);

	/* Parse the info and populate the aperture structure */
	AperturePtr->InFirstLine = InLine & XSCL_APTVERT_FIRSTLINE_MASK;
	AperturePtr->InLastLine = (InLine & XSCL_APTVERT_LASTLINE_MASK) >> XSCL_APTVERT_LASTLINE_SHIFT;

	AperturePtr->InFirstPixel = InPixel & XSCL_APTHORI_FIRSTPXL_MASK;
	AperturePtr->InLastPixel = (InPixel & XSCL_APTHORI_LASTPXL_MASK) >>	XSCL_APTHORI_LASTPXL_SHIFT;

	AperturePtr->OutHoriSize = OutSize & XSCL_OUTSIZE_NUMPXL_MASK;
	AperturePtr->OutVertSize = (OutSize & XSCL_OUTSIZE_NUMLINE_MASK) >>	XSCL_OUTSIZE_NUMLINE_SHIFT;

	return;
}

/*****************************************************************************/
/**
 * This function sets up Luma and Chroma start fractional values used by a
 * Scaler device.
 *
 * @param  InstancePtr is a pointer to the Scaler device instance to be worked
 *	   on.
 * @param  StartFractionPtr is a pointer to a start fractional value set to be
 *	   used.
 * @return None
 *
 *****************************************************************************/
void XScaler_SetStartFraction(uint32_t scaler_num, XScalerStartFraction *StartFractionPtr)
{
	/* Set up the fractional values */
	write_scaler_axi4(scaler_num, XSCL_HPA_Y,
			 (uint32_t)StartFractionPtr->LumaLeftHori & XSCL_FRCTLUMALEFT_VALUE_MASK);

	write_scaler_axi4(scaler_num, XSCL_HPA_C, 
			 (uint32_t)StartFractionPtr->ChromaLeftHori & XSCL_FRCTCHROMALEFT_VALUE_MASK);

	write_scaler_axi4(scaler_num, XSCL_VPA_Y,
			 (uint32_t)StartFractionPtr->LumaTopVert & XSCL_FRCTLUMATOP_VALUE_MASK);

	write_scaler_axi4(scaler_num, XSCL_VPA_C,
			 (uint32_t)StartFractionPtr->ChromaTopVert & XSCL_FRCTCHROMATOP_VALUE_MASK);

	return;
}

/*****************************************************************************/
/**
 * This function gets Luma and Chroma start fractional values currently used
 * by a Scaler device.
 *
 * @param  InstancePtr is a pointer to the Scaler device instance to be worked
 *	   on.
 * @param  StartFractionPtr is a pointer to a start fractional value structure
 *	   to be populated with the fractional values after this function
 *	   returns.
 * @return None
 *
 *****************************************************************************/
void XScaler_GetStartFraction(uint32_t scaler_num, XScalerStartFraction *StartFractionPtr)
{
	/* Fetch the fractional values */
	StartFractionPtr->LumaLeftHori = (s32) read_scaler_axi4(scaler_num, XSCL_HPA_Y)
			& XSCL_FRCTLUMALEFT_VALUE_MASK;

	StartFractionPtr->LumaTopVert = (s32) read_scaler_axi4(scaler_num, XSCL_VPA_Y)
			& XSCL_FRCTLUMATOP_VALUE_MASK;

	StartFractionPtr->ChromaLeftHori = (s32) read_scaler_axi4(scaler_num, XSCL_HPA_C)
			& XSCL_FRCTCHROMALEFT_VALUE_MASK;

	StartFractionPtr->ChromaTopVert = (s32)	read_scaler_axi4(scaler_num, XSCL_VPA_C)
			& XSCL_FRCTCHROMATOP_VALUE_MASK;

	return;
}

/*****************************************************************************/
/**
 * This function sets the numbers of vertical and horizontal phases to be used
 * by a Scaler device.
 *
 * @param  InstancePtr is a pointer to the Scaler device instance to be worked
 *		   on.
 * @param  VertPhaseNum is the number of vertical phase to set to
 * @param  HoriPhaseNum is the number of horizontal phase to set to
 * @return None
 *
 *****************************************************************************/
void XScaler_SetPhaseNum(uint32_t scaler_num, u16 VertPhaseNum, u16 HoriPhaseNum)
{
	u32 PhaseRegValue;

	/* Calculate the value to write to "Number of Phases Register" */
	PhaseRegValue =	(VertPhaseNum << XSCL_NUMPHASE_VERT_SHIFT) & XSCL_NUMPHASE_VERT_MASK;
	PhaseRegValue |= HoriPhaseNum & XSCL_NUMPHASE_HORI_MASK;

	/* Set up the Scaler core using the numbers of phases */
	write_scaler_axi4(scaler_num, XSCL_NUMPHASE, PhaseRegValue);

	return;
}

/*****************************************************************************/
/**
*
* This function chooses the active vertical and horizontal coefficient sets to
* be used by a Scaler device.
*
* Each coefficient set contains 4 banks: Horizontal Luma, Horizontal Chroma,
* Vertical Luma and Vertical Chroma. The horizontal part is independent from
* the vertical part and the Scaler device supports using the horizontal part
* of one coefficient set w/ the vertical part of a different coefficient set.
*
* @param  InstancePtr is a pointer to the Scaler device instance to be worked
*	  on
*
* @param  VertSetIndex indicates the index of the coefficient set in which
*	  the vertical part will be used by the Scaler device. Valid value
*	  is from 0 to (the number of the coefficient sets implemented by the
*	  Scaler device - 1)
*
* @param  HoriSetIndex indicates the index of the coefficient set in which
*	  the horizontal part will be used by the Scaler device. Valid value
*	  is from 0 to (the number of the coefficient sets implemented by the
*	  Scaler device - 1)
*
* @return None.
*
* @note	  None.
*
******************************************************************************/
void XScaler_SetActiveCoeffSet(uint32_t scaler_num,	u8 VertSetIndex, u8 HoriSetIndex)
{
	u32 RegValue;

	RegValue = ((u32)HoriSetIndex) & XSCL_COEFFSETS_HORI_MASK;
	RegValue |= (((u32)VertSetIndex) << XSCL_COEFFSETS_VERT_SHIFT) & XSCL_COEFFSETS_VERT_MASK;

	write_scaler_axi4(scaler_num, XSCL_COEFFSETS, RegValue);

	return;
}

/*****************************************************************************/
/**
 * This function loads a coefficient bank to the Scaler core. A complete
 * coefficient set contains 4 banks (if Luma, Chroma, Horizontal and Vertical
 * filter operations do not share common banks. For more details see
 * XScaler_GetCoeffBankSharingInfo()): Horizontal Luma, Horizontal Chroma,
 * Vertical Luma and Vertical Chroma. all 4 banks must be loaded back to back
 * in the order listed here. The caller is responsible for ensuring the
 * sequence and this function does not check it.
 *
 * An example sequence to load an whole coefficient set is like:
 * <pre>
 *	   XScaler_LoadCoeffBank(&Scaler, &HoriLumaCoeffBank);
 *	   XScaler_LoadCoeffBank(&Scaler, &HoriChromaCoeffBank);
 *	   XScaler_LoadCoeffBank(&Scaler, &VertLumaCoeffBank);
 *	   XScaler_LoadCoeffBank(&Scaler, &VertChromaCoeffBank);
 * </pre>
 *
 * @param  InstancePtr is a pointer to the Scaler device instance to be worked
 *	   on.
 * @param  CoeffBankPtr is a pointer to a coefficient bank that is to be loaded
 * @return None
 *
 *****************************************************************************/
void XScaler_LoadCoeffBank(uint32_t scaler_num, XScalerCoeffBank *CoeffBankPtr)
{
	int PhaseIndex;
	int TapIndex;

	s16 *CoeffValueTapBase;
	u32 CoeffValue;
//???
	//Start the coefficient bank loading by writing the bank index first
//	write_scaler_axi4(scaler_num, XSCL_COEFFSETADDR, CoeffBankPtr->SetIndex & XSCL_COEFFSETADDR_ADDR_MASK);

	/* Now load the valid values */
	CoeffValueTapBase = CoeffBankPtr->CoeffValueBuf;
	
	for (PhaseIndex = 0; PhaseIndex < CoeffBankPtr->PhaseNum; PhaseIndex++) 
	{
		for (TapIndex = 0; TapIndex < CoeffBankPtr->TapNum; ) 
		{
			CoeffValue =  ((u32)CoeffValueTapBase[TapIndex++]) & 0xFFFF;
			
			if (TapIndex < CoeffBankPtr->TapNum) 
			{
				CoeffValue |= (((u32)CoeffValueTapBase[TapIndex++]) & 0xFFFF) << 16;
			}

			write_scaler_axi4(scaler_num, XSCL_COEFFVALUE, CoeffValue);
		}

		CoeffValueTapBase +=  CoeffBankPtr->TapNum;
	}

	//Load padding if the real phase number is less than the maximum phase	number
	for (PhaseIndex = CoeffBankPtr->PhaseNum; PhaseIndex < MAX_PHASE_NUM; PhaseIndex++) 
	{
		for (TapIndex = 0; TapIndex < (CoeffBankPtr->TapNum + 1) / 2; TapIndex++) 
		{
			write_scaler_axi4(scaler_num, XSCL_COEFFVALUE, 0);
		}
	}

	return;
}

/*****************************************************************************/
/**
 * This function returns the pointer to the coefficients for a scaling
 * operation given input/output sizes and the Tap and Phase numbers.
 *
 * @param  InSize indicates the size (width or height) of the input video.
 * @param  OutSize indicates the size (width or height) of the output video.
 * @param  Tap indicates the Tap number.
 * @param  Phase indicates the Phase number.
 * @return The points to the coefficients ready for the scaling operation.
 * @note   None.
 *			
 *****************************************************************************/
const s16 *XScaler_CoefValueLookup(u32 InSize, u32 OutSize, u32 Phase)
{
	u32 CoeffBinIndex;
	u32 CoeffValueOffset;

	/* Find the index of the Coefficient Bin */
	if (OutSize > InSize)
	{
		CoeffBinIndex = 0;
	}
	else
	{
		CoeffBinIndex = OutSize * 16 / InSize;
	}

	/* Find the offset of the Coefficients within the Bin */
	CoeffValueOffset = 	(((Phase * (Phase + 1)) / 2) - 1) * XSCL_TAP_NUM;

	return &(XScaler_Coefficients[CoeffBinIndex][CoeffValueOffset]);
}

/*****************************************************************************/
/**
*
* This function sets up the scaler core for the feature demonstrations below.
* After the execution of this function, a set of coefficient value (containing
* 2 vertical and 2 horizontal coefficient banks) are loaded; aperture is set
* up and the scaling operation is started. This function is utilized by
* DownScale() and UpScale().
*
* @param	ScalerInstPtr is the pointer to the instance of the Scaler
*		device.
*
* @param	ScalerInWidth is the width of the input aperture.
*
* @param	ScalerInHeight is the height of the input aperture.
*
* @param	ScalerOutWidth is the width of the output aperture.
*
* @param	ScalerOutHeight is the height of the output aperture.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void ScalerSetup(uint32_t scaler_num, XScalerAperture *AperturePtr)
{
//	int i;
//	u8 ChromaFormat;
	u8 ChromaLumaShareCoeffBank;
	u8 HoriVertShareCoeffBank;

//	DEBUG_OUT(("\r\n>>ScalerSetup : \r\nScaler #%d : Version = 0x%08X!\r\n", scaler_num, read_scaler_axi4((scaler_num), XSCL_VERSION)));

	/*
	 * Disable the scaler before setup and tell the device not to pick up
	 * the register updates until all are done
	 */
	XScaler_Disable(scaler_num);
//	XScaler_Enable(scaler_num);
	XScaler_DisableRegUpdate(scaler_num);
/*
	XScaler_IRQ_Enable(scaler_num);

	DEBUG_OUT(("Waiting for Coeff. FIFO Ready!\r\n"));
	while ((read_scaler_axi4(scaler_num, XSCL_STS) & 0x00000001 == 0));
	DEBUG_OUT(("Ready to Write Coeff. FIFO!\r\n"));
*/
/////////////////////////////////
	XScaler_SetPhaseNum(scaler_num, COEFF_TAP_NUM, COEFF_PHASE_NUM);
	XScaler_SetActiveCoeffSet(scaler_num, 0, 0);

	ChromaLumaShareCoeffBank = 1;		//	Share Luma/Chroma Coeff.
	HoriVertShareCoeffBank = 0;			//	Not Share Horizontal/Vertical Coeff.

	CoeffBank.SetIndex 	= COEFF_SET_INDEX;
	CoeffBank.PhaseNum 	= COEFF_PHASE_NUM;
	CoeffBank.TapNum 	= COEFF_TAP_NUM;
	/* Locate coefficients for Horizontal scaling */
	CoeffBank.CoeffValueBuf = (s16 *) XScaler_CoefValueLookup(AperturePtr->InHoriSize, AperturePtr->OutHoriSize, CoeffBank.PhaseNum);

//	Set Coeff Addr
	write_scaler_axi4(scaler_num, XSCL_COEFFSETADDR, CoeffBank.SetIndex & XSCL_COEFFSETADDR_ADDR_MASK);
//
	/* Load coefficient bank for Horizontal Luma */
	XScaler_LoadCoeffBank(scaler_num, &CoeffBank);

	/* Horizontal Chroma bank is loaded only if chroma/luma sharing flag is not set */
	if (!ChromaLumaShareCoeffBank)
	{
		XScaler_LoadCoeffBank(scaler_num, &CoeffBank);
	}

	// Vertical coeff banks are loaded only if horizontal/vertical sharing flag is not set
	if (!HoriVertShareCoeffBank) 
	{
		/* Locate coefficients for Vertical scaling */
		CoeffBank.CoeffValueBuf = (s16 *) XScaler_CoefValueLookup(AperturePtr->InVertSize, AperturePtr->OutVertSize, CoeffBank.PhaseNum);

		/* Load coefficient bank for Vertical Luma */
		XScaler_LoadCoeffBank(scaler_num, &CoeffBank);

		/* Vertical Chroma coeff bank is loaded only if chroma/luma
		 * sharing flag is not set	*/
		if (!ChromaLumaShareCoeffBank)
		{
			XScaler_LoadCoeffBank(scaler_num, &CoeffBank);
		}
	}
	
	if	((read_scaler_axi4(scaler_num, XSCL_STSERR) & 0x00000010))
	{
		#if(XILINX_SCALER_DEBUG >= 1)
		printf("Output Board : Scaler #%d : Coeff. FIFO Write Error!!!\r\n", scaler_num);
		#endif
	}
	else
 	{
		#if(XILINX_SCALER_DEBUG >= 1)
		printf("Output Board : Scaler #%d : Coeff. FIFO Write Successfully!\r\n", scaler_num);
		#endif
	}

	//Load phase-offsets into scaler
	StartFraction.LumaLeftHori = 0;
	StartFraction.LumaTopVert = 0;
	StartFraction.ChromaLeftHori = 0;
	StartFraction.ChromaTopVert = 0;
	XScaler_SetStartFraction(scaler_num, &StartFraction);

	XScaler_SetAperture(scaler_num, AperturePtr);

/*
DEBUG_OUT(("\r\n<1>Read AXI4-Lite Registers : \r\n"));
for	(i = 0; i < 16; i++)
{
	read_scaler_axi4(scaler_num, 0x0100 + (i * 4));
}
*/
	//Enable the scaling operation
	XScaler_EnableRegUpdate(scaler_num);
	XScaler_Enable(scaler_num);
/*
DEBUG_OUT(("\r\n<2>Read AXI4-Lite Registers : \r\n"));
for	(i = 0; i < 16; i++)
{
	read_scaler_axi4(scaler_num, 0x0100 + (i * 4));
}
*/
	return;
}

void reset_scaler(uint32_t scaler_num)
{
//	uint32_t	i;

//	DEBUG_OUT(("\r\nReset Scaler #%d!\r\n", scaler_num));

	write_scaler_axi4((scaler_num), XSCL_CTL, 0x80000000);
	Delay1ms(5);
	write_scaler_axi4((scaler_num), XSCL_CTL, 0);
}

void print_out_coeff(uint32_t scaler_num)
{
	uint32_t	i, j, k;

//	DEBUG_OUT(("\r\n>>print_out_coeff : \r\nScaler #%d : Version = 0x%08X!\r\n", scaler_num, read_scaler_axi4((scaler_num), XSCL_VERSION)));

//	XScaler_Disable(scaler_num);
//	XScaler_Enable(scaler_num);

//	XScaler_IRQ_Enable(scaler_num);

//	XScaler_CoeffRD_Disable(scaler_num);	
	write_scaler_axi4(scaler_num, XSCL_CTL, 0x00000008);	

	write_scaler_axi4(scaler_num, XSCL_COEFFBANKADDR, 0);	

	XScaler_CoeffRD_Enable(scaler_num);

	#if(XILINX_SCALER_DEBUG == 2)
	printf("Waiting to read Coefficients!\r\n");
	#endif
	//	Waiting
	//for (i = 0; i < 100000; i++);
	Delay10ms(1);

	k = 0;
	while ((k & 0x00000020) == 0)
	{
		k = OutputFPGA_ReadWord(AXI4_INTC_IF_REG + scaler_num);
	}
	#if(XILINX_SCALER_DEBUG == 2)
	printf("Ready to read Coefficients!\r\n");
	#endif

	for (i = 0; i < COEFF_PHASE_NUM; i++)
	{
//		DEBUG_OUT(("PHASE #[%d] : ", i));
		for (j = 0; j < COEFF_TAP_NUM; j++)
		{
			// Bank Addr
			write_scaler_axi4(scaler_num, XSCL_COEFFBANKADDR, 0);	
			//	Tap, Phase Address
			k = (i << 8) + j;
			write_scaler_axi4(scaler_num, XSCL_COEFF_RDADDR, k);
			
			k = read_scaler_axi4(scaler_num, XSCL_COEFFDATAOUT);
//			DEBUG_OUT(("0x%04X ", k));
		}
//		DEBUG_OUT(("\r\n"));
	}
	
	write_scaler_axi4(scaler_num, XSCL_CTL, 0x00000000);	
//	XScaler_CoeffRD_Disable(scaler_num);	
//	XScaler_Enable(scaler_num);
//	XScaler_Disable(scaler_num);
}
