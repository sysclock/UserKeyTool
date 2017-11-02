#ifndef STANDARDSIGNALMODE_H_
#define STANDARDSIGNALMODE_H_

#include "../System Code/global.h"

#define NumberOfDecoderVideoModes 			2
#define NumberOfCompVideoModes 					9
#define NumberOfGraphicModes						46

#define NumberOfPresetModes		(NumberOfDecoderVideoModes +		\
								 												 NumberOfCompVideoModes +	\
								 												 NumberOfGraphicModes + 1)

typedef struct
{
	WORD W_StdHFreq;							// Standard H. frequency in 100 Hz unit
	WORD W_DeltaHFreq;					// H. frequency tolerance in 100 Hz unit
	WORD W_StdVFreq;							// Standard V. frequency in 1/10 Hz unit
	BYTE B_DeltaVFreq;					// V. frequency tolerance in 1/10 Hz unit
	WORD W_StdVTotal;						// Standard V. total in line unit
	BYTE B_DeltaVTotal; 				// V. total tolerance in line unit
	WORD W_StdHTotal;						// Standard H. total in pixel unit

	WORD W_StdHBeforePorch;		// Standard H Before Porch in pixel unit
	WORD W_StdHSync;							// Standard H Sync in pixel unit
	WORD W_StdHAfterPorch;			// Standard H After Porch in pixel unit
	WORD W_StdHStart;						// Standard H. start in pixel unit
	WORD W_StdWidth;							// Standard width in pixel unit
	BYTE B_HPolarity;						// Standard H polarity

	WORD W_StdVBeforePorch;		// Standard V Before Porch in line unit
	WORD W_StdVSync;							// Standard V Sync in line unit
	WORD W_StdVAfterPorch;			// Standard V After Porch in line unit
	WORD W_StdVStart;						// Standard V. start in line unit
	WORD W_StdHeight;						// Standard Height in line unit
	BYTE B_VPolarity;						// Standard V polarity

	WORD W_ModeType;							// Mode type
} StandardSignalModeStruct;

extern const StandardSignalModeStruct  StandardSignalModeTable[NumberOfPresetModes];
#endif 
/*STANDARDSIGNALMODE_H_*/
