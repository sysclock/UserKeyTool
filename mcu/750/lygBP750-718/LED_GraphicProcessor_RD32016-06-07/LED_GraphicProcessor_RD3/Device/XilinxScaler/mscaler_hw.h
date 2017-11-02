#ifndef XSCALER_HW_H		/* prevent circular inclusions */
#define XSCALER_HW_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/************************** Constant Definitions *****************************/

/** @name Device Register Offsets
 *  @{
 */
#define XSCL_CTL			0x0000	 /**< Control */
#define XSCL_STS			0x0004	 /**< General status */
#define XSCL_STSERR			0x0008	 /**< Error status */
#define XSCL_IRQ_ENABLE		0x000C	 /**< For detecting operation status */
#define XSCL_VERSION		0x0010	 /* */

#define XSCL_HSF			0x0100	 /**< Horizontal Shrink Factor */
#define XSCL_VSF			0x0104	 /**< Vertical Shrink Factor */
#define XSCL_IN_HVSIZE		0x0108
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

/*@}*/

/** @name Control Register bit definition
 *  @{
 */
#define XSCL_CTL_COEFF_RD_ENABLE		0x00000008

#define XSCL_CTL_REGUPDATE				0x00000002 /**< Register Update Enable for
						register HSF thru
						FRCTCHROMATOP */
#define XSCL_CTL_ENABLE					0x00000001 /**< Enable the scaler on the next
						video frame */

#define XSCL_IRQ_ENABLE_MASK			0x0000001F
/*@}*/

/** @name Status Register bit definition
 *  @{
 */
#define XSCL_STS_COEF_W_RDY_MASK 0x00000001 /**< If 1, Coefficient values can
						 be written into the core */
/*@}*/

/** @name Error Status Register bit definition (to be defined)
 *  @{
 */
#define XSCL_STSERR_CODE3_MASK	0xFF000000  /**< code 3 */
#define XSCL_STSERR_CODE2_MASK	0x00FF0000  /**< code 2 */
#define XSCL_STSERR_CODE1_MASK	0x0000FF00  /**< code 1 */
#define XSCL_STSERR_CODE0_MASK	0x000000FF  /**< code 0 */
/*@}*/

/** @name Done Status Register bit definition
 *  @{
 */
#define XSCL_STSDONE_DONE	0x00000001 /**< indicator of end of scaler
						operation */
/*@}*/

/** @name Horizontal Shrink Factor Register bit definition
 *  @{
 */
#define XSCL_HSF_INT_MASK	0x00F00000 /**< Horizontal Shrink Factor
						integer */
#define XSCL_HSF_FRAC_MASK	0x000FFFFF /**< Horizontal Shrink Factor
						fractional */
/*@}*/

/** @name Vertical Shrink Factor Register bit definition
 *  @{
 */
#define XSCL_VSF_INT_MASK	0x00F00000 /**< Vertical Shrink Factor integer
					     */
#define XSCL_VSF_FRAC_MASK	0x000FFFFF /**< Vertical Shrink Factor
						fractional */
/*@}*/

/** @name Aperture Horizontal Register bit definition
 *	@{
 */
#define XSCL_APTHORI_LASTPXL_MASK	0x07FF0000 /**< Location of last pixel
							in line */
#define XSCL_APTHORI_LASTPXL_SHIFT	16	   /**< Shift for location of
							last pixel */
#define XSCL_APTHORI_FIRSTPXL_MASK	0x000007FF /**< Location of first pixel
							in line */
/*@}*/

/** @name Aperture Vertical Register bit definition
 *  @{
 */
#define XSCL_APTVERT_LASTLINE_MASK	0x07FF0000 /**< Location of last line
							in active video */
#define XSCL_APTVERT_LASTLINE_SHIFT	16	   /**< Shift for location of
							last line */
#define XSCL_APTVERT_FIRSTLINE_MASK	0x000007FF /**< Location of first line
							in active video */
/*@}*/

/** @name Output Size Register bit definition
 *  @{
 */
#define XSCL_OUTSIZE_NUMLINE_MASK	0x07FF0000 /**< The number of lines in
							output image */
#define XSCL_OUTSIZE_NUMLINE_SHIFT	16	   /**< Shift for the number of
							lines */
#define XSCL_OUTSIZE_NUMPXL_MASK	0x000007FF /**< The number of pixels in
							output image */
/*@}*/

/** @name Number of Phases Register bit definition
 *  @{
 */
#define XSCL_NUMPHASE_VERT_MASK		0x00007F00 /**< The number of vertical
							phases */
#define XSCL_NUMPHASE_VERT_SHIFT	8	   /**< Shift for the number of
							vertical phases */
#define XSCL_NUMPHASE_HORI_MASK		0x0000007F /**< The number of
							horizontal phases */
/*@}*/

/** @name Active Coefficient Set Register bit definition
 *  @{
 */
#define XSCL_COEFFSETS_VERT_MASK	0x000000F0 /**< Active vertical
							coefficient set */
#define XSCL_COEFFSETS_VERT_SHIFT	4	   /**< Active vertical
							coefficient set
							shift */
#define XSCL_COEFFSETS_HORI_MASK	0x0000000F /**< Active horizontal
							coefficient set */
/*@}*/

/** @name Luma left edge horizontal accumulator fractional value register
 *  @{
 */
#define XSCL_FRCTLUMALEFT_VALUE_MASK	0x001FFFFF /**< Fractional value to
							initialize horizontal
							accumulator for luma */
/*@}*/

/** @name Chroma left edge horizontal accumulator fractional value register
 *  @{
 */
#define XSCL_FRCTCHROMALEFT_VALUE_MASK	0x001FFFFF /**< Fractional value to
							initialize horizontal
							accumulator for
							chroma */
/*@}*/

/** @name Luma top edge vertical accumulator fractional value register
 *  @{
 */
#define XSCL_FRCTLUMATOP_VALUE_MASK	0x001FFFFF /**< Fractional value to
							initialize vertical
							accumulator for luma */
/*@}*/

/** @name Chroma top edge vertical accumulator fractional value register
 *  @{
 */
#define XSCL_FRCTCHROMATOP_VALUE_MASK	0x001FFFFF /**< Fractional value to
							initialize vertical
							accumulator for
							chroma */
/*@}*/

/** @name Coefficient band address register bit definition
 *  @{
 */
#define XSCL_COEFFSETADDR_ADDR_MASK	0x0000000F /**< Address of the
							Coefficient bank to
							write next */
/*@}*/

/** @name Coefficient Value Register bit definition
 *  @{
 */
#define XSCL_COEFFVALUE_NPLUS1_MASK	0xFFFF0000 /**< Second value in the
							pair */
#define XSCL_COEFFVALUE_N_MASK		0x0000FFFF /**< First value in the
							pair */
/*@}*/

/** @name Version Register bit definition
 *  @{
 */
#define XSCL_VER_MAJOR_MASK	0xF0000000	/**< Major Version*/
#define XSCL_VER_MAJOR_SHIFT	28		/**< Major Version Bit Shift*/
#define XSCL_VER_MINOR_MASK	0x0FF00000	/**< Minor Version */
#define XSCL_VER_MINOR_SHIFT	20		/**< Minor Version Bit Shift*/
#define XSCL_VER_REV_MASK	0x000F0000	/**< Revision Version */
#define XSCL_VER_REV_SHIFT	16		/**< Revision Bit Shift*/
/*@}*/

/** @name Reset Register bit definition
 *  @{
 */
#define XSCL_RESET_RESET_MASK		0x0000000A /**< Software reset bit */
/*@}*/

/** @name Global Interrupt Enable Register bit definition
 *  @{
 */
#define XSCL_GIER_GIE_MASK		0x80000000 /**< Global interrupt
							enable */
/*@}*/

/** @name Interrupt Status/Enable Register bit definition
 *  @{
 */
#define XSCL_IXR_REG_UPDATE_DONE_MASK	0x00000020 /**< Shadow Register
							Update Interrupt */
#define XSCL_IXR_COEF_WR_ERROR_MASK	0x00000010 /**< Coefficient FIFO
							Write Error
							Interrupt */
#define XSCL_IXR_OUTPUT_ERROR_MASK	0x00000008 /**< Output Error
							Interrupt */
#define XSCL_IXR_INPUT_ERROR_MASK	0x00000004 /**< Input Error
							Interrupt */
#define XSCL_IXR_COEF_FIFO_READY_MASK	0x00000002 /**< Coefficient FIFO
							Ready Interrupt */
#define XSCL_IXR_OUTPUT_FRAME_DONE_MASK 0x00000001 /**< Video Frame Done
							Interrupt */

#define XSCL_IXR_EVENT_MASK	(XSCL_IXR_OUTPUT_FRAME_DONE_MASK | \
					XSCL_IXR_COEF_FIFO_READY_MASK | \
					XSCL_IXR_REG_UPDATE_DONE_MASK)
						   /**< Mask for all
							normal event
							interrupts */
#define XSCL_IXR_ERROR_MASK	(XSCL_IXR_INPUT_ERROR_MASK | \
					XSCL_IXR_OUTPUT_ERROR_MASK | \
					XSCL_IXR_COEF_WR_ERROR_MASK)
						   /**< Mask for all
							error interrupts */
#define XSCL_IXR_ALLINTR_MASK	(XSCL_IXR_EVENT_MASK | \
					XSCL_IXR_ERROR_MASK) /**< Mask for all
								  interrupts */
/*@}*/

/** @name Chroma Format Type Definition
 *  @{
 */
#define XSCL_CHROMA_FORMAT_420		1	/**< YUV4:2:0 */
#define XSCL_CHROMA_FORMAT_422		2	/**< YUV4:2:2 */
#define XSCL_CHROMA_FORMAT_444		3	/**< YUV4:4:4 */
/*@}*/

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/** @name Scaler Register Access Macro Definition
 *  @{
 */
#define XScaler_In32  XIo_In32
#define XScaler_Out32 XIo_Out32

/*****************************************************************************/
/**
*
* Read the given register.
*
* @param	BaseAddress is the base address of the device
* @param	RegOffset is the register offset to be read
*
* @return	The 32-bit value of the register
*
* @note
* C-style signature:
*	 u32 XScaler_ReadReg(u32 BaseAddress, u32 RegOffset)
*
#define XScaler_ReadReg(BaseAddress, RegOffset)				\
			XScaler_In32((BaseAddress) + (RegOffset))

******************************************************************************/
/*****************************************************************************/
/**
*
* Write the given register.
*
* @param	BaseAddress is the base address of the device
* @param	RegOffset is the register offset to be written
* @param	Data is the 32-bit value to write to the register
*
* @return	None.
*
* @note
* C-style signature:
*	 void XScaler_WriteReg(u32 BaseAddress, u32 RegOffset, u32 Data)
*
#define XScaler_WriteReg(BaseAddress, RegOffset, Data)			\
			XScaler_Out32((BaseAddress) + (RegOffset), (Data))

******************************************************************************/
/*@}*/

/************************** Function Prototypes ******************************/

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
