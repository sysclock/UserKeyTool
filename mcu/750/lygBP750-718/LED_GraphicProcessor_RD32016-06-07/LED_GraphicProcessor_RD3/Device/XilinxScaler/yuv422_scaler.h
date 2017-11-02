#ifndef __YUV422_SCALER_H 
#define __YUV422_SCALER_H

#include <stdint.h>
#include "xil_types.h"

#define		AXI4_INTC_IF_REG		0x0101
#define		AXI4_WRITE_DATA_REG		0x0104
#define		AXI4_COMMAND_REG		0x0105
#define		AXI4_READ_DATA_REG		0x0106

#define XSCL_CTL			0x0000	 /**< Control */
#define XSCL_STS			0x0004	 /**< General status */
#define XSCL_STSERR			0x0008	 /**< Error status */
#define XSCL_IRQ_ENABLE		0x000C	 /**<  */
#define XSCL_VERSION		0x0010	 /* */

#define XSCL_HSF			0x0100	 /**< Horizontal Shrink Factor */
#define XSCL_VSF			0x0104	 /**< Vertical Shrink Factor */
#define XSCL_INSIZE			0x0108
#define XSCL_APTHORI		0x010C	 /**< First and last subject pixels in input line */
#define XSCL_APTVERT		0x0110	 /**< First and last subject lines in input image */
#define XSCL_OUTSIZE		0x0114	 /**< Output image size:width and height */

#define XSCL_NUMPHASE		0x0118	 /**< The numbers of phases in current filter */
#define XSCL_COEFFSETS		0x011C	 /**< Active horizontal and vertical coefficient sets to use */

#define XSCL_HPA_Y			0x0120
#define XSCL_HPA_C			0x0124
#define XSCL_VPA_Y			0x0128
#define XSCL_VPA_C			0x012C

#define XSCL_COEFFSETADDR	0x0130	 /**< Address of Coefficient set to	write */
#define XSCL_COEFFVALUE		0x0134	 /**< Coefficient values to write */
#define XSCL_COEFFBANKADDR	0x0138	 /** */
#define XSCL_COEFF_RDADDR	0x013C	 /**<  */
#define XSCL_COEFFDATAOUT	0x0140	 /**					   */

/** @name Shrink factor constants
 *	@{
 */
#define XSCL_SHRINK_FACTOR	0x100000 /**< For calculating HSF/VSF values */
/*@}*/

#define	COEFF_SET_INDEX		0
#define	COEFF_TAP_NUM		4
#define	COEFF_PHASE_NUM		4

#define	XSCL_TAP_NUM		4
#define	MAX_PHASE_NUM		16

#define	MAX_INPUT_SCALER		4
#define	MAX_OUTPUT_SCALER		4
/**
 * This typedef contains configuration information for a Scaler device.
 * Each Scaler device should have a configuration structure associated
 */
typedef struct {
	u16 DeviceId;	   /**< DeviceId is the unique ID  of the device */
	u32 BaseAddress;   /**< BaseAddress is the physical base address of the
			     *  device's registers */
	u16 VertTapNum;	   /**< The number of Vertical Taps */
	u16 HoriTapNum;	   /**< The number of Horizontal Taps */
	u16 MaxPhaseNum;   /**< The maximum number of Phases */
	u8 CoeffSetNum;	   /**< The number of coefficient sets implemented */
	u8 ChromaFormat;   /**< Chroma Format */
	u8 SeparateYcCoef; /**< Separate Chroma/Luma Coefficient banks */
	u8 SeparateHvCoef; /**< Separate Horizontal/Vertical Coefficient
				banks. Common only possible if num_h_taps =
				num_v_taps */

} XScaler_Config;

/**
 * The XScalerAperture data structure for Aperture and scale factor control.
 * The scale factor values are calculated using the field in this structure
 */
typedef struct {
	uint32_t InVertSize;	/**< Vertical size of the input video */
	uint32_t InHoriSize;	/**< Horizontal size of the input video */
	uint32_t InFirstLine;	/**< The first line index in the input video */
	uint32_t InLastLine;	/**< The last line index in the input video */
	uint32_t InFirstPixel;	/**< The first pixel index in the input video*/
	uint32_t InLastPixel;	/**< The last pixel index in the input video */
	uint32_t OutVertSize;	/**< Vertical size of the output video */
	uint32_t OutHoriSize;	/**< Horizontal size of the output video */
} XScalerAperture;

/**
 * The XScalerStartFraction data structure for Luma and Chroma Start Fraction
 * setting
 */
typedef struct {
	s32 LumaLeftHori;  		/**< Horizontal accumulator at rectangle left edge  for Luma */
	s32 ChromaLeftHori;		/**< Horizontal accumulator at rectangle left edge 	for Chroma */
	s32 LumaTopVert;   		/**< Vertical accumulator at rectangle top edge for Luma */
	s32 ChromaTopVert; 		/**< Vertical accumulator at rectangle top edge for 	Chroma */
} XScalerStartFraction;

/**
 * The XScalerCoeffBank data structure for loading a Bank in a Coefficient Set,
 * which contains 4 banks.
 */
typedef struct {
	u16 SetIndex;			/**< Coefficient Set Index (0 based). */
	s16 *CoeffValueBuf;		/**< Pointer to a coefficient value data buffer	 */
	u16 PhaseNum;			/**< The number of the phases associated w/ the bank */
	u16 TapNum;				/**< The number of the Tap associated w/ the bank */
} XScalerCoeffBank;

/*****************************************************************************/
/**
*
* This macro calculates the integral value nearest to x rounding half-way cases
* away from zero, regardless of the current rounding direction.
*
* @param  x has a float type value
*
* @return the integral value nearest to x rounding half-way cases away from
*		  zero, regardless of the current rounding direction.
*
* @note
* C-style signature:
*  s32 round(float x);
*
******************************************************************************/
#define round(x) ((x) >= 0 ? (s32)((x) + 0.5) : (s32)((x) - 0.5))

/*****************************************************************************/
/**
 * This macro calculates the N-th Triangular number: 1 + 2 + ... + N
 *
 * @param  N indicates the positive integer number to calculate the N-th
 *	   Triangular number.
 * @return The N-th triangular number.
 *
 * @note
 * C-style signature:
 *	   u32 XScaler_TriangularNumber(u32 N);
 *
 *****************************************************************************/
#define XScaler_TriangularNumber(N) ((N) * ((N) + 1) / 2)

#define XScaler_TriangularNumber_inp(N) ((N) * ((N) + 1) / 2)


/*****************************************************************************
/
#define read_scaler_axi4_inp	  read_scaler_axi4
#define write_scaler_axi4_inp	  write_scaler_axi4

#define XScaler_SetAperture_inp	  XScaler_SetAperture
#define XScaler_GetAperture_inp	  XScaler_GetAperture

#define ScalerSetup_inp			  ScalerSetup

#define set_video_in_inp		  set_video_in

#define print_out_coeff_inp		  print_out_coeff
#define reset_out_fpga_inp		  reset_out_fpga
#define reset_scaler_axi4_inp	  reset_scaler_axi4
#define reset_scaler_inp		  reset_scaler
/
*****************************************************************************/


/*****************************************************************************/
/**
 * This macro calculates the offset of a coefficient Tap from the beginning of
 * a coefficient Bin.
 *
 * @param  Tap indicates the index of the coefficient tap in the coefficient
 *		   Bin
 * @return The offset of the coefficient TAP from the beginning of a
 *		   coefficient Bin
 * @note
 * C-style signature:
 *	   u32 XScaler_CoefTapOffset(u32 Tap);
 *
 *****************************************************************************/
#define XScaler_CoefTapOffset(Tap) \
	((XScaler_TriangularNumber((Tap) - 1) - 1) * \
		(XScaler_TriangularNumber(16) - 1 + 32 + 64))

/*****************************************************************************/
#define XScaler_CoeffRD_Enable(scaler_num) \
	write_scaler_axi4((scaler_num), XSCL_CTL, \
		read_scaler_axi4((scaler_num), XSCL_CTL) \
			| XSCL_CTL_COEFF_RD_ENABLE)

#define XScaler_CoeffRD_Disable(scaler_num) \
	write_scaler_axi4((scaler_num), XSCL_CTL, \
		read_scaler_axi4((scaler_num), XSCL_CTL) \
			& (~XSCL_CTL_COEFF_RD_ENABLE))

/*****************************************************************************/
#define XScaler_IRQ_Enable(scaler_num) \
	write_scaler_axi4((scaler_num), XSCL_IRQ_ENABLE, \
		read_scaler_axi4((scaler_num), XSCL_IRQ_ENABLE) \
			| XSCL_IRQ_ENABLE_MASK)

#define XScaler_IRQ_Disable(scaler_num) \
	write_scaler_axi4((scaler_num), XSCL_IRQ_ENABLE, \
		read_scaler_axi4((scaler_num), XSCL_IRQ_ENABLE) \
			& (~XSCL_IRQ_ENABLE_MASK))

/*****************************************************************************/
/*****************************************************************************/
#define XScaler_CoefTapOffset_inp(Tap) \
	((XScaler_TriangularNumber_inp((Tap) - 1) - 1) * \
		(XScaler_TriangularNumber_inp(16) - 1 + 32 + 64))

/*****************************************************************************/
#define XScaler_CoeffRD_Enable_inp(scaler_num) \
	write_scaler_axi4_inp((scaler_num), XSCL_CTL, \
		read_scaler_axi4_inp((scaler_num), XSCL_CTL) \
			| XSCL_CTL_COEFF_RD_ENABLE)

#define XScaler_CoeffRD_Disable_inp(scaler_num) \
	write_scaler_axi4_inp((scaler_num), XSCL_CTL, \
		read_scaler_axi4_inp((scaler_num), XSCL_CTL) \
			& (~XSCL_CTL_COEFF_RD_ENABLE))

/*****************************************************************************/
#define XScaler_IRQ_Enable_inp(scaler_num) \
	write_scaler_axi4_inp((scaler_num), XSCL_IRQ_ENABLE, \
		read_scaler_axi4_inp((scaler_num), XSCL_IRQ_ENABLE) \
			| XSCL_IRQ_ENABLE_MASK)

#define XScaler_IRQ_Disable_inp(scaler_num) \
	write_scaler_axi4_inp((scaler_num), XSCL_IRQ_ENABLE, \
		read_scaler_axi4_inp((scaler_num), XSCL_IRQ_ENABLE) \
			& (~XSCL_IRQ_ENABLE_MASK))



/*****************************************************************************/
/**
*
* This macro enables a Scaler device.
*
* @param  InstancePtr is a pointer to the Scaler device instance to be worked
*	  on.
*
* @return None.
*
* @note
* C-style signature:
*	 void XScaler_Enable(XScaler *InstancePtr);
*
******************************************************************************/
#define XScaler_Enable(scaler_num) \
	write_scaler_axi4((scaler_num), XSCL_CTL, \
		read_scaler_axi4((scaler_num), XSCL_CTL) \
			| XSCL_CTL_ENABLE)

#define XScaler_Enable_inp(scaler_num) \
	write_scaler_axi4_inp((scaler_num), XSCL_CTL, \
		read_scaler_axi4_inp((scaler_num), XSCL_CTL) \
			| XSCL_CTL_ENABLE)

/*****************************************************************************/
/**
*
* This macro disables a Scaler device.
*
* @param  InstancePtr is a pointer to the Scaler device instance to be worked
*	  on.
*
* @return None.
*
* @note
* C-style signature:
*	 void XScaler_Disable(XScaler *InstancePtr);
*
******************************************************************************/
#define XScaler_Disable(scaler_num) \
	write_scaler_axi4((scaler_num), XSCL_CTL, \
		read_scaler_axi4((scaler_num), XSCL_CTL) \
			& (~XSCL_CTL_ENABLE))

#define XScaler_Disable_inp(scaler_num) \
	write_scaler_axi4_inp((scaler_num), XSCL_CTL, \
		read_scaler_axi4_inp((scaler_num), XSCL_CTL) \
			& (~XSCL_CTL_ENABLE))

/*****************************************************************************/
/**
*
* This macro tells a Scaler device to pick up the register value changes made
* so far.
*
* @param  InstancePtr is a pointer to the Scaler device instance to be worked
*	  on.
*
* @return None.
*
* @note
* C-style signature:
*	 void XScaler_EnableRegUpdate(XScaler *InstancePtr);
*
******************************************************************************/
#define XScaler_EnableRegUpdate(scaler_num) \
	write_scaler_axi4((scaler_num), XSCL_CTL, \
		read_scaler_axi4((scaler_num), XSCL_CTL) \
			| XSCL_CTL_REGUPDATE)

#define XScaler_EnableRegUpdate_inp(scaler_num) \
	write_scaler_axi4_inp((scaler_num), XSCL_CTL, \
		read_scaler_axi4_inp((scaler_num), XSCL_CTL) \
			| XSCL_CTL_REGUPDATE)

/*****************************************************************************/
/**
*
* This macro tells a Scaler device not to pick up the register value changes
* until XScaler_EnableRegUpdate() is invoked again. This is very useful when
* multiple registers need to be updated. All register updates could be made
* with no tight time constraints with the help of this macro.
*
* @param  InstancePtr is a pointer to the Scaler device instance to be worked
*	  on.
*
* @return None.
*
* @note
* C-style signature:
*	 void XScaler_DisableRegUpdate(XScaler *InstancePtr);
*
******************************************************************************/
#define XScaler_DisableRegUpdate(scaler_num) \
	write_scaler_axi4((scaler_num), XSCL_CTL, \
		read_scaler_axi4((scaler_num), XSCL_CTL) \
			& ~XSCL_CTL_REGUPDATE)

#define XScaler_DisableRegUpdate_inp(scaler_num) \
	write_scaler_axi4_inp((scaler_num), XSCL_CTL, \
		read_scaler_axi4_inp((scaler_num), XSCL_CTL) \
			& ~XSCL_CTL_REGUPDATE)
/*
*/
extern uint32_t read_scaler_axi4_inp(uint32_t scaler_num, uint32_t axi_addr);
extern uint32_t write_scaler_axi4_inp(uint32_t scaler_num, uint32_t axi_addr, uint32_t axi_data);

extern uint32_t XScaler_SetAperture_inp(uint32_t scaler_num, XScalerAperture *AperturePtr);
extern void XScaler_GetAperture_inp(uint32_t scaler_num, XScalerAperture *AperturePtr);

extern void ScalerSetup_inp(uint32_t scaler_num, XScalerAperture *AperturePtr);

extern void set_video_in_inp(uint32_t scaler_num, uint32_t inport, 
						uint32_t start_pixel, uint32_t start_line, uint32_t num_of_pixel, uint32_t num_of_line);

extern void print_out_coeff_inp(uint32_t scaler_num);
extern void reset_out_fpga_inp(uint32_t fpga_num);
extern void reset_scaler_axi4_inp(uint32_t scaler_num);
extern void reset_scaler_inp(uint32_t scaler_num);
extern void Init_Scaler_inp(void);

/******************************************************************************/
extern uint32_t read_scaler_axi4(uint32_t scaler_num, uint32_t axi_addr);
extern uint32_t write_scaler_axi4(uint32_t scaler_num, uint32_t axi_addr, uint32_t axi_data);

extern uint32_t XScaler_SetAperture(uint32_t scaler_num, XScalerAperture *AperturePtr);
extern void XScaler_GetAperture(uint32_t scaler_num, XScalerAperture *AperturePtr);

extern void ScalerSetup(uint32_t scaler_num, XScalerAperture *AperturePtr);

extern void set_video_in(uint32_t scaler_num, uint32_t inport, 
						uint32_t start_pixel, uint32_t start_line, uint32_t num_of_pixel, uint32_t num_of_line);

extern void print_out_coeff(uint32_t scaler_num);
extern void reset_out_fpga(uint32_t fpga_num);
extern void reset_scaler_axi4(uint32_t scaler_num);
extern void reset_scaler(uint32_t scaler_num);

#endif	//__YUV422_SCALER_H
