/*
 * adv7604 - Analog Devices ADV7604 video decoder driver
*/
/*
 * References (c = chapter, p = page):
 * REF_01 - Analog devices, ADV7604, Register Settings Recommendations,
 *              Revision 2.5, June 2010
 * REF_02 - Analog devices, Register map documentation, Documentation of
 *              the register maps, Software manual, Rev. F, June 2010
 * REF_03 - Analog devices, ADV7604, Hardware Manual, Rev. F, August 2010
 */

/****************************************************************************/
/*	I N C L U D E    F I L E S												*/
/****************************************************************************/
#include <stdio.h>
#include <string.h>
//#include "../../System Code/lpc17xx.h"
#include "../../System Code/ASCII.h"
#include "../../System Code/global.h"
#include "../../System Code/type.h"
#include "../../System Code/Timer.h"
#include "../../IIC Comm/D_IIC0.h"
#include "../Device/InputFPGA/InputFPGA.h"
#include "adv7604.h"

extern BYTE g_InputEdidData[128];
/* ADV7604 system clock frequency */
#define ADV7604_fsc (28636360)

#define DIGITAL_INPUT (state->mode == ADV7604_MODE_HDMI)

#define ARRAY_SIZE(arr) sizeof(arr)/sizeof((arr)[0])

#define ADV7604_DEBUG						0
//#define ADV7604_ANALOG_INPUT_MAX_FREQ					170000000
#define ADV7604_ANALOG_INPUT_MAX_FREQ					175000000
#define ADV7604_DIGITAL_INPUT_MAX_FREQ				225000000
/*
 **********************************************************************
 *
 *  Arrays with configuration parameters for the ADV7604
 *
 **********************************************************************
 */
/* Supported CEA and DMT timings */
const struct v4l2_dv_timings adv7604_timings[] = {
        V4L2_DV_BT_CEA_720X480P59_94,						// 0
        V4L2_DV_BT_CEA_720X576P50,              // 1
        V4L2_DV_BT_CEA_1280X720P24,             // 2
        V4L2_DV_BT_CEA_1280X720P25,             // 3
        V4L2_DV_BT_CEA_1280X720P50,             // 4
        V4L2_DV_BT_CEA_1280X720P60,             // 5
        V4L2_DV_BT_CEA_1920X1080P24,            // 6
        V4L2_DV_BT_CEA_1920X1080P25,            // 7
        V4L2_DV_BT_CEA_1920X1080P30,            // 8
        V4L2_DV_BT_CEA_1920X1080P50,            // 9
        V4L2_DV_BT_CEA_1920X1080P60,            // 10

        /* sorted by DMT ID */
        V4L2_DV_BT_DMT_640X350P85,              // 11
        V4L2_DV_BT_DMT_640X400P85,              // 12
        V4L2_DV_BT_DMT_720X400P85,              // 13
        V4L2_DV_BT_DMT_640X480P60,              // 14
        V4L2_DV_BT_DMT_640X480P72,              // 15
        V4L2_DV_BT_DMT_640X480P75,              // 16
        V4L2_DV_BT_DMT_640X480P85,              // 17
        V4L2_DV_BT_DMT_800X600P56,              // 18
        V4L2_DV_BT_DMT_800X600P60,              // 19
        V4L2_DV_BT_DMT_800X600P72,              // 20
        V4L2_DV_BT_DMT_800X600P75,              // 21
        V4L2_DV_BT_DMT_800X600P85,              // 22
        V4L2_DV_BT_DMT_848X480P60,              // 23
        V4L2_DV_BT_DMT_1024X768P60,             // 24
        V4L2_DV_BT_DMT_1024X768P70,             // 25
        V4L2_DV_BT_DMT_1024X768P75,             // 26
        V4L2_DV_BT_DMT_1024X768P85,             // 27
        V4L2_DV_BT_DMT_1152X864P75,             // 28
        V4L2_DV_BT_DMT_1280X768P60_RB,          // 29
        V4L2_DV_BT_DMT_1280X768P60,             // 30
        V4L2_DV_BT_DMT_1280X768P75,             // 31
        V4L2_DV_BT_DMT_1280X768P85,             // 32
        V4L2_DV_BT_DMT_1280X800P60_RB,          // 33
        V4L2_DV_BT_DMT_1280X800P60,             // 34
        V4L2_DV_BT_DMT_1280X800P75,             // 35
        V4L2_DV_BT_DMT_1280X800P85,             // 36
        V4L2_DV_BT_DMT_1280X960P60,             // 37
        V4L2_DV_BT_DMT_1280X960P85,             // 38
        V4L2_DV_BT_DMT_1280X1024P60,            // 39
        V4L2_DV_BT_DMT_1280X1024P75,            // 40
        V4L2_DV_BT_DMT_1280X1024P85,            // 41
        V4L2_DV_BT_DMT_1360X768P60,             // 42
        V4L2_DV_BT_DMT_1400X1050P60_RB,         // 43
        V4L2_DV_BT_DMT_1400X1050P60,            // 44
        V4L2_DV_BT_DMT_1400X1050P75,            // 45
        V4L2_DV_BT_DMT_1400X1050P85,            // 46
        V4L2_DV_BT_DMT_1440X900P60_RB,          // 47
        V4L2_DV_BT_DMT_1440X900P60,             // 48
        V4L2_DV_BT_DMT_1600X1200P60,            // 49
        V4L2_DV_BT_DMT_1680X1050P60_RB,         // 50
        V4L2_DV_BT_DMT_1680X1050P60,            // 51
        V4L2_DV_BT_DMT_1792X1344P60,            // 52
        V4L2_DV_BT_DMT_1856X1392P60,            // 53
        V4L2_DV_BT_DMT_1920X1200P60_RB,         // 54
        V4L2_DV_BT_DMT_1366X768P60,             // 55
        V4L2_DV_BT_DMT_1920X1080P60,            // 56
				V4L2_DV_BT_DMT_1920X1200P60,						// 57
        {0},
};

struct adv7604_video_standards {
        struct v4l2_dv_timings timings;
        unsigned char vid_std;
        unsigned char v_freq;
};

/* sorted by number of lines */
const struct adv7604_video_standards adv7604_prim_mode_comp[] = {
        /* { V4L2_DV_BT_CEA_720X480P59_94, 0x0a, 0x00 }, TODO flickering */
        { V4L2_DV_BT_CEA_720X576P50, 0x0b, 0x00 },
        { V4L2_DV_BT_CEA_1280X720P50, 0x19, 0x01 },
        { V4L2_DV_BT_CEA_1280X720P60, 0x19, 0x00 },
        { V4L2_DV_BT_CEA_1920X1080P24, 0x1e, 0x04 },
        { V4L2_DV_BT_CEA_1920X1080P25, 0x1e, 0x03 },
        { V4L2_DV_BT_CEA_1920X1080P30, 0x1e, 0x02 },
        { V4L2_DV_BT_CEA_1920X1080P50, 0x1e, 0x01 },
        { V4L2_DV_BT_CEA_1920X1080P60, 0x1e, 0x00 },
        /* TODO add 1920x1080P60_RB (CVT timing) */
        {0, 0, 0},
};

/* sorted by number of lines */
const struct adv7604_video_standards adv7604_prim_mode_gr[] = {
        { V4L2_DV_BT_DMT_640X480P60, 0x08, 0x00 },
        { V4L2_DV_BT_DMT_640X480P72, 0x09, 0x00 },
        { V4L2_DV_BT_DMT_640X480P75, 0x0a, 0x00 },
        { V4L2_DV_BT_DMT_640X480P85, 0x0b, 0x00 },
        { V4L2_DV_BT_DMT_800X600P56, 0x00, 0x00 },
        { V4L2_DV_BT_DMT_800X600P60, 0x01, 0x00 },
        { V4L2_DV_BT_DMT_800X600P72, 0x02, 0x00 },
        { V4L2_DV_BT_DMT_800X600P75, 0x03, 0x00 },
        { V4L2_DV_BT_DMT_800X600P85, 0x04, 0x00 },
        { V4L2_DV_BT_DMT_1024X768P60, 0x0c, 0x00 },
        { V4L2_DV_BT_DMT_1024X768P70, 0x0d, 0x00 },
        { V4L2_DV_BT_DMT_1024X768P75, 0x0e, 0x00 },
        { V4L2_DV_BT_DMT_1024X768P85, 0x0f, 0x00 },
        { V4L2_DV_BT_DMT_1280X1024P60, 0x05, 0x00 },
        { V4L2_DV_BT_DMT_1280X1024P75, 0x06, 0x00 },
        { V4L2_DV_BT_DMT_1360X768P60, 0x12, 0x00 },
        { V4L2_DV_BT_DMT_1366X768P60, 0x13, 0x00 },
        { V4L2_DV_BT_DMT_1400X1050P60, 0x14, 0x00 },
        { V4L2_DV_BT_DMT_1400X1050P75, 0x15, 0x00 },
        { V4L2_DV_BT_DMT_1600X1200P60, 0x16, 0x00 }, /* TODO not tested */
        /* TODO add 1600X1200P60_RB (not a DMT timing) */
        { V4L2_DV_BT_DMT_1680X1050P60, 0x18, 0x00 },
        { V4L2_DV_BT_DMT_1920X1200P60_RB, 0x19, 0x00 }, /* TODO not tested */
//				{ V4L2_DV_BT_DMT_1920X1080P60, 0x1E, 0x00},
				{ V4L2_DV_BT_DMT_1920X1200P60, 0x19, 0x00 },
        {0},
};

/* sorted by number of lines */
const struct adv7604_video_standards adv7604_prim_mode_hdmi_comp[] = {
        { V4L2_DV_BT_CEA_720X480P59_94, 0x0a, 0x00 },
        { V4L2_DV_BT_CEA_720X576P50, 0x0b, 0x00 },
        { V4L2_DV_BT_CEA_1280X720P50, 0x13, 0x01 },
        { V4L2_DV_BT_CEA_1280X720P60, 0x13, 0x00 },
        { V4L2_DV_BT_CEA_1920X1080P24, 0x1e, 0x04 },
        { V4L2_DV_BT_CEA_1920X1080P25, 0x1e, 0x03 },
        { V4L2_DV_BT_CEA_1920X1080P30, 0x1e, 0x02 },
        { V4L2_DV_BT_CEA_1920X1080P50, 0x1e, 0x01 },
        { V4L2_DV_BT_CEA_1920X1080P60, 0x1e, 0x00 },
        {0},
};

/* sorted by number of lines */
const struct adv7604_video_standards adv7604_prim_mode_hdmi_gr[] = {
        { V4L2_DV_BT_DMT_640X480P60, 0x08, 0x00 },
        { V4L2_DV_BT_DMT_640X480P72, 0x09, 0x00 },
        { V4L2_DV_BT_DMT_640X480P75, 0x0a, 0x00 },
        { V4L2_DV_BT_DMT_640X480P85, 0x0b, 0x00 },
        { V4L2_DV_BT_DMT_800X600P56, 0x00, 0x00 },
        { V4L2_DV_BT_DMT_800X600P60, 0x01, 0x00 },
        { V4L2_DV_BT_DMT_800X600P72, 0x02, 0x00 },
        { V4L2_DV_BT_DMT_800X600P75, 0x03, 0x00 },
        { V4L2_DV_BT_DMT_800X600P85, 0x04, 0x00 },
        { V4L2_DV_BT_DMT_1024X768P60, 0x0c, 0x00 },
        { V4L2_DV_BT_DMT_1024X768P70, 0x0d, 0x00 },
        { V4L2_DV_BT_DMT_1024X768P75, 0x0e, 0x00 },
        { V4L2_DV_BT_DMT_1024X768P85, 0x0f, 0x00 },
        { V4L2_DV_BT_DMT_1280X1024P60, 0x05, 0x00 },
        { V4L2_DV_BT_DMT_1280X1024P75, 0x06, 0x00 },
        {0},
};

struct stdi_readback {
        unsigned short bl, lcf, lcvs;
        unsigned char hs_pol, vs_pol;
        unsigned char interlaced;
};

/****************************************************************************/
/*	G L O B A L    V A R I A B L E S										*/
/****************************************************************************/
struct adv7604_state gSt_ADV7604State;

/****************************************************************************/
/*	G L O B A L    F U N C T I O N    P R O T O T Y P E S					*/
/****************************************************************************/
unsigned char ADV7604_Init(struct adv7604_state *state);
void ADV7604_EnableInput(struct adv7604_state *state);
void ADV7604_DisableInput(struct adv7604_state *state);
void ADV7604_SelectInput(struct adv7604_state *state);
int ADV7604_SetRouting(struct adv7604_state *state, unsigned long input, unsigned long output, unsigned long config);
int ADV7604_SetTimings(struct adv7604_state *state, unsigned long TimingsIndex);
void ADV7604_SetInternalEdid(struct adv7604_state *state, unsigned char EdidMode);
int adv7604_isr(struct adv7604_state *state, unsigned long status, unsigned char *handled);
int adv7604_log_status(struct adv7604_state *state);
void ADV7604_LoadEdid(struct adv7604_state *state, unsigned char EdidSource);
void ADV7604_WriteEdidToProm(struct adv7604_state *state, unsigned char EdidMode);
int ADV7604_GetSignalActiveWidthHeight(struct adv7604_state *state, long *pWidth, long *pHeight);
int adv7604_set_ctrl(struct adv7604_state *state, int CtrlId, unsigned long CtrlVal);
/****************************************************************************/
/*	S T A T I C   V A R I A B L E S											*/
/****************************************************************************/
static struct v4l2_dv_timings StaticAdv7604Timings;

/****************************************************************************/
/*	S T A T I C   R O U T I N E S											*/
/****************************************************************************/
static unsigned char GetChipRevision(struct adv7604_state *state, unsigned char *RxRev);
static unsigned hblanking(const struct v4l2_bt_timings *t);
static unsigned htotal(const struct v4l2_bt_timings *t);
static unsigned vblanking(const struct v4l2_bt_timings *t);
static unsigned vtotal(const struct v4l2_bt_timings *t);
static int find_and_set_predefined_video_timings(struct adv7604_state *state, unsigned char prim_mode, const struct adv7604_video_standards *predef_vid_timings, const struct v4l2_dv_timings *timings);
static int configure_predefined_video_timings(struct adv7604_state *state, struct v4l2_dv_timings *timings);
static void configure_custom_video_timings(struct adv7604_state *state, const struct v4l2_bt_timings *bt);
static void set_rgb_quantization_range(struct adv7604_state *state);
static int stdi2dv_timings(struct adv7604_state *state, struct stdi_readback *stdi, struct v4l2_dv_timings *timings);
static int adv7604_get_input_status(struct adv7604_state *state, unsigned long *status);
static int read_stdi(struct adv7604_state *state, struct stdi_readback *stdi);
static int adv7604_enum_dv_timings(struct adv7604_state *state, struct v4l2_enum_dv_timings *timings);
static int adv7604_dv_timings_cap(struct adv7604_state *state, struct v4l2_dv_timings_cap *cap);
static int adv7604_query_dv_timings(struct adv7604_state *state, struct v4l2_dv_timings *timings);
static int adv7604_set_dv_timings(struct adv7604_state *state, struct v4l2_dv_timings *timings);
static int adv7604_get_dv_timings(struct adv7604_state *state, struct v4l2_dv_timings *timings);
static void ADV7604_WriteInternalEdid(struct adv7604_state *state, unsigned char EdidMode);
static unsigned char no_power(struct adv7604_state *sd);
static unsigned char no_signal_tmds(struct adv7604_state *sd);
static unsigned char no_lock_tmds(struct adv7604_state *sd);
static unsigned char is_hdmi(struct adv7604_state *sd);
static unsigned char no_lock_sspd(struct adv7604_state *sd);
static unsigned char no_lock_stdi(struct adv7604_state *sd);
static unsigned char no_signal(struct adv7604_state *sd);
static unsigned char no_lock_cp(struct adv7604_state *sd);
static unsigned char io_read(struct adv7604_state *state, unsigned char reg);
static unsigned char io_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
static unsigned char io_write_and_or(struct adv7604_state *state, unsigned char reg, unsigned char mask, unsigned char val);
static unsigned char avlink_read(struct adv7604_state *state, unsigned char reg);
static unsigned char avlink_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
static unsigned char cec_read(struct adv7604_state *state, unsigned char reg);
static unsigned char cec_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
static unsigned char cec_write_and_or(struct adv7604_state *state, unsigned char reg, unsigned char mask, unsigned char val);
static unsigned char infoframe_read(struct adv7604_state *state, unsigned char reg);
static unsigned char infoframe_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
static unsigned char esdp_read(struct adv7604_state *state, unsigned char reg);
static unsigned char esdp_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
static unsigned char dpp_read(struct adv7604_state *state, unsigned char reg);
static unsigned char dpp_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
static unsigned char afe_read(struct adv7604_state *state, unsigned char reg);
static unsigned char afe_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
static unsigned char rep_read(struct adv7604_state *state, unsigned char reg);
static unsigned char rep_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
static unsigned char rep_write_and_or(struct adv7604_state *state, unsigned char reg, unsigned char mask, unsigned char val);
static unsigned char edid_read(struct adv7604_state *state, unsigned char reg);
static unsigned char edid_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
static unsigned char hdmi_read(struct adv7604_state *state, unsigned char reg);
static unsigned char hdmi_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
static unsigned char test_read(struct adv7604_state *state, unsigned char reg);
static unsigned char test_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
static unsigned char cp_read(struct adv7604_state *state, unsigned char reg);
static unsigned char cp_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
static unsigned char cp_write_and_or(struct adv7604_state *state, unsigned char reg, unsigned char mask, unsigned char val);
static unsigned char vdp_read(struct adv7604_state *state, unsigned char reg);
static unsigned char vdp_write(struct adv7604_state *state, unsigned char reg, unsigned char val);
/****************************************************************************/
/*	C O D E																	*/
/****************************************************************************/

unsigned char ADV7604_Init(struct adv7604_state *state)
{
//        struct adv7604_state *state = to_state(sd);
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char B_TempData = 0;

	if(GetChipRevision(state, &B_TempData) == 0xFF)
		return 0xFF;
	if(B_TempData != 0x03)
	{
		#if(ADV7604_DEBUG > 0)
		printf("Chip Is Not ADV7604\r\n");
		#endif
		return 0xFF;
	}

//	printf("ARRAY_SIZE(adv7604_timings) = %d\r\n", ARRAY_SIZE(adv7604_timings));
	hdmi_write(state, 0x48,
					(pdata->disable_pwrdnb ? 0x80 : 0) |
					(pdata->disable_cable_det_rst ? 0x40 : 0));

	ADV7604_DisableInput(state);

	// power */
	io_write(state, 0x0c, 0x42);   // Power up part and power down VDP
	io_write(state, 0x0b, 0x44);   // Power down ESDP block
	cp_write(state, 0xcf, 0x01);   // Power down macrovision

	// video format
	io_write_and_or(state, 0x02, 0xf0,
									pdata->alt_gamma << 3 |
									pdata->op_656_range << 2 |
									pdata->rgb_out << 1 |
									pdata->alt_data_sat << 0);
	io_write(state, 0x03, pdata->op_format_sel);
	io_write_and_or(state, 0x04, 0x1f, pdata->op_ch_sel << 5);
	io_write_and_or(state, 0x05, 0xf0, pdata->blank_data << 3 |
																	pdata->insert_av_codes << 2 |
																	pdata->replicate_av_codes << 1 |
																	pdata->invert_cbcr << 0);

	// TODO from platform data
	cp_write(state, 0x69, 0x30);   // Enable CP CSC
	io_write(state, 0x06, 0xa6);   // positive VS and HS
	io_write(state, 0x14, 0x7f);   // Drive strength adjusted to max
	cp_write(state, 0xba, (pdata->hdmi_free_run_mode << 1) | 0x01); // HDMI free run
	cp_write(state, 0xf3, 0xdc); // Low threshold to enter/exit free run mode
	cp_write(state, 0xf9, 0x23); //  STDI ch. 1 - LCVS change threshold -
														//  ADI recommended setting [REF_01, c. 2.3.3]
	cp_write(state, 0x45, 0x23); //  STDI ch. 2 - LCVS change threshold -
														//  ADI recommended setting [REF_01, c. 2.3.3]
	cp_write(state, 0xc9, 0x2d); // use prim_mode and vid_std as free run resolution
														// for digital formats

	// TODO from platform data
	afe_write(state, 0xb5, 0x01);  // Setting MCLK to 256Fs

	afe_write(state, 0x02, pdata->ain_sel); // Select analog input muxing mode
	io_write_and_or(state, 0x30, ~(1 << 4), pdata->output_bus_lsb_to_msb << 4);

	// interrupts
	io_write(state, 0x40, 0xc2); // Configure INT1
	io_write(state, 0x41, 0xd7); // STDI irq for any change, disable INT2
	io_write(state, 0x46, 0x98); // Enable SSPD, STDI and CP unlocked interrupts
	io_write(state, 0x6e, 0xc0); // Enable V_LOCKED and DE_REGEN_LCK interrupts
	io_write(state, 0x73, 0x10); // Enable CABLE_DET_A_ST (+5v) interrupt
	adv7604_set_ctrl(state, V4L2_CID_ADV_RX_FREE_RUN_COLOR_MANUAL, 1);
	adv7604_set_ctrl(state, V4L2_CID_ADV_RX_FREE_RUN_COLOR, 0x007F7F);
	
	adv7604_set_ctrl(state, V4L2_CID_BRIGHTNESS, 0);
	adv7604_set_ctrl(state, V4L2_CID_CONTRAST, 128);
	adv7604_set_ctrl(state, V4L2_CID_SATURATION, 128);
	adv7604_set_ctrl(state, V4L2_CID_HUE, 0);
	{
		state->prev_input_status = V4L2_IN_ST_NO_SIGNAL | V4L2_IN_ST_NO_POWER;
//		ADV7604_DisableInput(state);
		ADV7604_SelectInput(state);
		ADV7604_EnableInput(state);
		if(state->mode == ADV7604_MODE_COMP)
			ADV7604_SetTimings(state, 5);
		if(state->mode == ADV7604_MODE_GR)
			ADV7604_SetTimings(state, 49);
		if(state->mode == ADV7604_MODE_HDMI)
			ADV7604_SetTimings(state, 10);
	}
	return 1;
}

void ADV7604_EnableInput(struct adv7604_state *state)
{
	switch (state->mode)
	{
		case ADV7604_MODE_COMP:
		case ADV7604_MODE_GR:
			// enable
			io_write(state, 0x15, 0xB8);   // Disable Tristate of Pins (no audio)
			break;
		case ADV7604_MODE_HDMI:
			// enable
			hdmi_write(state, 0x1A, 0x0A); // Unmute audio
			hdmi_write(state, 0x01, 0x00); // Enable HDMI clock terminators
			io_write(state, 0x15, 0xA0);   // Disable Tristate of Pins
			break;
		default:
			printf("Unknown mode %d\r\n", state->mode);
			break;
	}
}

void ADV7604_DisableInput(struct adv7604_state *state)
{
	// disable
	io_write(state, 0x15, 0xBE);   // Tristate all outputs from video core
	hdmi_write(state, 0x1A, 0x1A); // Mute audio
	hdmi_write(state, 0x01, 0x78); // Disable HDMI clock terminators
}

void ADV7604_SelectInput(struct adv7604_state *state)
{
//	unsigned char B_TempData = 0;
	switch (state->mode)
	{
		case ADV7604_MODE_COMP:
		case ADV7604_MODE_GR:
			// reset ADI recommended settings for HDMI:
			// "ADV7604 Register Settings Recommendations (rev. 2.5, June 2010)" p. 4.
			io_write(state, 0x00, 0x02); /* video std */
//			B_TempData = io_read(state, 0x01);
//			printf("ADV7604_SelectInput: B_TempData = 0x%02X\r\n", B_TempData);
			io_write(state, 0x01, 0x02); /* prim mode HDMI-GR */
			hdmi_write(state, 0x0d, 0x04); // HDMI filter optimization
			hdmi_write(state, 0x3d, 0x00); // DDC bus active pull-up control
			hdmi_write(state, 0x3e, 0x74); // TMDS PLL optimization
			hdmi_write(state, 0x4e, 0x3b); // TMDS PLL optimization
			hdmi_write(state, 0x57, 0x74); // TMDS PLL optimization
			hdmi_write(state, 0x58, 0x63); // TMDS PLL optimization
			hdmi_write(state, 0x8d, 0x18); // equaliser
			hdmi_write(state, 0x8e, 0x34); // equaliser
			hdmi_write(state, 0x93, 0x88); // equaliser
			hdmi_write(state, 0x94, 0x2e); // equaliser
			hdmi_write(state, 0x96, 0x00); // enable automatic EQ changing

			afe_write(state, 0x00, 0x08); // power up ADC
			afe_write(state, 0x01, 0x06); // power up Analog Front End
			afe_write(state, 0xc8, 0x00); // phase control
			afe_write(state, 0x15, 0x85); // EMB_SYNC_1_SEL Sync3 pin, Adjusting Sync filter clamp

			// set ADI recommended settings for digitizer
			// "ADV7604 Register Settings Recommendations (rev. 2.5, June 2010)" p. 17.
			afe_write(state, 0x12, 0x7b); // ADC noise shaping filter controls
			afe_write(state, 0x0c, 0x1f); // CP core gain controls
//			cp_write(state, 0x3e, 0x04); // CP core pre-gain control
			cp_write(state, 0x3e, 0x84); // CP core pre-gain control, Video Adjustment Enable
			cp_write(state, 0xc3, 0x39); // CP coast control. Graphics mode
//			cp_write(state, 0x40, 0x5c); // CP core pre-gain control. Graphics mode
			cp_write(state, 0x40, 0x56); // CP core pre-gain control. Graphics mode
			break;
		case ADV7604_MODE_HDMI:
			// set ADI recommended settings for HDMI:
			// "ADV7604 Register Settings Recommendations (rev. 2.5, June 2010)" p. 4.
			hdmi_write(state, 0x0d, 0x84); // HDMI filter optimization
			hdmi_write(state, 0x3d, 0x10); // DDC bus active pull-up control
			hdmi_write(state, 0x3e, 0x39); // TMDS PLL optimization
			hdmi_write(state, 0x4e, 0x3b); // TMDS PLL optimization
			hdmi_write(state, 0x57, 0xb6); // TMDS PLL optimization
			hdmi_write(state, 0x58, 0x03); // TMDS PLL optimization
			hdmi_write(state, 0x8d, 0x18); // equaliser
			hdmi_write(state, 0x8e, 0x34); // equaliser
			hdmi_write(state, 0x93, 0x8b); // equaliser
			hdmi_write(state, 0x94, 0x2d); // equaliser
			hdmi_write(state, 0x96, 0x01); // enable automatic EQ changing

			afe_write(state, 0x00, 0xff); // power down ADC
			afe_write(state, 0x01, 0xfe); // power down Analog Front End
			afe_write(state, 0xc8, 0x40); // phase control

			// reset ADI recommended settings for digitizer
			// "ADV7604 Register Settings Recommendations (rev. 2.5, June 2010)" p. 17.
			afe_write(state, 0x12, 0xfb); // ADC noise shaping filter controls
			afe_write(state, 0x0c, 0x0d); // CP core gain controls
//			cp_write(state, 0x3e, 0x00); // CP core pre-gain control
			cp_write(state, 0x3e, 0x80); // CP core pre-gain control, Video Adjustment Enable
			cp_write(state, 0xc3, 0x39); // CP coast control. Graphics mode
			cp_write(state, 0x40, 0x80); // CP core pre-gain control. Graphics mode
			break;
		default:
			printf("ADV7604_SelectInput: Unknown mode %d\r\n", state->mode);
		break;
	}
}

int ADV7604_SetRouting(struct adv7604_state *state, unsigned long input, unsigned long output, unsigned long config)
{
	#if(ADV7604_DEBUG > 0)
	printf("ADV7604_SetRouting: Input %d\r\n", (enum adv7604_mode) input);
	#endif
//	if(state->mode == (enum adv7604_mode) input)
//		return 1;
	state->mode = (enum adv7604_mode) input;
//	ADV7604_DisableInput(state);
	ADV7604_SelectInput(state);
	ADV7604_EnableInput(state);
	if(state->mode == ADV7604_MODE_COMP)
		ADV7604_SetTimings(state, 5);
	if(state->mode == ADV7604_MODE_GR)
		ADV7604_SetTimings(state, 49);
	if(state->mode == ADV7604_MODE_HDMI)
		ADV7604_SetTimings(state, 10);
	state->prev_input_status = V4L2_IN_ST_NO_SIGNAL | V4L2_IN_ST_NO_POWER;
	return 1;
}

int ADV7604_SetTimings(struct adv7604_state *state, unsigned long TimingsIndex)
{
//	struct v4l2_dv_timings St_Timings = {0};
	const struct v4l2_dv_timings *Stp_Temptimings = NULL;
	#if(ADV7604_DEBUG > 0)
	printf("ADV7604_SetTimings: TimingsIndex %d\r\n", TimingsIndex);
	#endif
	if(TimingsIndex > (ARRAY_SIZE(adv7604_timings) - 1))
		return 0;
	Stp_Temptimings = &adv7604_timings[TimingsIndex];
	memcpy(&StaticAdv7604Timings, Stp_Temptimings, sizeof(StaticAdv7604Timings));
	adv7604_set_dv_timings(state, &StaticAdv7604Timings);
//	memcpy(&(state->timings), &St_Timings, sizeof(St_Timings));
//	configure_custom_video_timings(&state, &(Stp_Temptimings->Unionv4l2_bt_timings.bt));
	return 1;
}

/*********** avi info frame CEA-861-E **************/

void ADV7604_PrintAVIInfoframe(struct adv7604_state *state)
{
        int i;
        unsigned char buf[14];
        unsigned char avi_len;
        unsigned char avi_ver;

        if (!is_hdmi(state)) {
                printf("receive DVI-D signal (AVI infoframe not supported)\r\n");
                return;
        }
        if (!(io_read(state, 0x60) & 0x01)) {
                printf("AVI infoframe not received\r\n");
                return;
        }

        if (io_read(state, 0x83) & 0x01) {
                printf("AVI infoframe checksum error has occurred earlier\r\n");
                io_write(state, 0x85, 0x01); /* clear AVI_INF_CKS_ERR_RAW */
                if (io_read(state, 0x83) & 0x01) {
                        printf("AVI infoframe checksum error still present\r\n");
                        io_write(state, 0x85, 0x01); /* clear AVI_INF_CKS_ERR_RAW */
                }
        }

        avi_len = infoframe_read(state, 0xe2);
        avi_ver = infoframe_read(state, 0xe1);
        printf("AVI infoframe version %d (%d byte)\r\n",
                        avi_ver, avi_len);

        if (avi_ver != 0x02)
                return;

        for (i = 0; i < 14; i++)
                buf[i] = infoframe_read(state, i);

        printf("\t%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",
                buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
                buf[8], buf[9], buf[10], buf[11], buf[12], buf[13]);
}

void ADV7604_WriteEdidToProm(struct adv7604_state *state, unsigned char EdidMode)
{
	ADV7604_SetInternalEdid(state, EdidMode);
	rep_write(state, 0x7E, 0x01);
	Delay100ms(5);
}

void ADV7604_LoadEdid(struct adv7604_state *state, unsigned char EdidSource)
{
	if(EdidSource == 0)
	{
		ADV7604_SetInternalEdid(state, EdidSource);
		rep_write(state, 0x7E, 0x00);
	}
	else
	{
		rep_write(state, 0x77, 0x00);
		rep_write(state, 0x7E, 0x02);
		rep_write(state, 0x77, 0x0F);
	}
	Delay10ms(5);
}

void ADV7604_SetInternalEdid(struct adv7604_state *state, unsigned char EdidMode)
{
	rep_write(state, 0x77, 0x00);
	ADV7604_WriteInternalEdid(state, EdidMode);
	rep_write(state, 0x77, 0x0F);
}

int adv7604_log_status(struct adv7604_state *state)
{
        struct v4l2_dv_timings timings;
        struct stdi_readback stdi;
        unsigned char reg_io_0x02 = io_read(state, 0x02);
				unsigned char audio_pll_locked = 0;
				unsigned char audio_sample_packet_detect = 0;
				unsigned char audio_mute = 0;

        const char *csc_coeff_sel_rb[16] = {
                "bypassed", "YPbPr601 -> RGB", "reserved", "YPbPr709 -> RGB",
                "reserved", "RGB -> YPbPr601", "reserved", "RGB -> YPbPr709",
                "reserved", "YPbPr709 -> YPbPr601", "YPbPr601 -> YPbPr709",
                "reserved", "reserved", "reserved", "reserved", "manual"
        };
        const char *input_color_space_txt[16] = {
                "RGB limited range (16-235)", "RGB full range (0-255)",
                "YCbCr Bt.601 (16-235)", "YCbCr Bt.709 (16-235)",
                "XvYCC Bt.601", "XvYCC Bt.709",
                "YCbCr Bt.601 (0-255)", "YCbCr Bt.709 (0-255)",
                "invalid", "invalid", "invalid", "invalid", "invalid",
                "invalid", "invalid", "automatic"
        };
        const char *rgb_quantization_range_txt[] = {
                "Automatic",
                "RGB limited range (16-235)",
                "RGB full range (0-255)",
        };
        const char *deep_color_mode_txt[4] = {
                "8-bits per channel",
                "10-bits per channel",
                "12-bits per channel",
                "16-bits per channel (not supported)"
        };

        printf("-----Chip status-----\r\n");
        printf("Chip power: %s\r\n", no_power(state) ? "off" : "on");
        printf("Connector type: %s\r\n", state->connector_hdmi ?
                        "HDMI" : (DIGITAL_INPUT ? "DVI-D" : "DVI-A"));
        printf("EDID: %s\r\n", ((rep_read(state, 0x7d) & 0x01) &&
                        (rep_read(state, 0x77) & 0x01)) ? "enabled" : "disabled ");
        printf("CEC: %s\r\n", !!(cec_read(state, 0x2a) & 0x01) ?
                        "enabled" : "disabled");

        printf("-----Signal status-----\r\n");
        printf("Cable detected (+5V power): %s\r\n",
                        (io_read(state, 0x6f) & 0x10) ? "true" : "false");
        printf("TMDS signal detected: %s\r\n",
                        no_signal_tmds(state) ? "false" : "true");
        printf("TMDS signal locked: %s\r\n",
                        no_lock_tmds(state) ? "false" : "true");
        printf("SSPD locked: %s\r\n", no_lock_sspd(state) ? "false" : "true");
        printf("STDI locked: %s\r\n", no_lock_stdi(state) ? "false" : "true");
        printf("CP locked: %s\r\n", no_lock_cp(state) ? "false" : "true");
        printf("CP free run: %s\r\n",
                        (!!(cp_read(state, 0xff) & 0x10) ? "on" : "off"));
        printf("Prim-mode = 0x%X, video std = 0x%X, v_freq = 0x%X\r\n",
                        io_read(state, 0x01) & 0x0f, io_read(state, 0x00) & 0x3f,
                        (io_read(state, 0x01) & 0x70) >> 4);

        printf("-----Video Timings-----\r\n");
        if (read_stdi(state, &stdi))
                printf("STDI: not locked\r\n");
        else
                printf("STDI: lcf (frame height - 1) = %d, bl = %d, lcvs (vsync) = %d, %s, %chsync, %cvsync\r\n",
                                stdi.lcf, stdi.bl, stdi.lcvs,
                                stdi.interlaced ? "interlaced" : "progressive",
                                stdi.hs_pol, stdi.vs_pol);
        if (adv7604_query_dv_timings(state, &timings))
                printf("No video detected\r\n");
        else
                v4l2_print_dv_timings("ADV7604", "Detected format: ",
                                      &timings, TRUE);
        v4l2_print_dv_timings("ADV7604", "Configured format: ",
                              &state->timings, TRUE);

        if (no_signal(state))
                return 0;

        printf("-----Color space-----\r\n");
        printf("RGB quantization range ctrl: %s\r\n",
                        rgb_quantization_range_txt[state->rgb_quantization_range]);
        printf("Input color space: %s\r\n",
                        input_color_space_txt[reg_io_0x02 >> 4]);
        printf("Output color space: %s %s, saturator %s\r\n",
                        (reg_io_0x02 & 0x02) ? "RGB" : "YCbCr",
                        (reg_io_0x02 & 0x04) ? "(16-235)" : "(0-255)",
                        ((reg_io_0x02 & 0x04) ^ (reg_io_0x02 & 0x01)) ?
                                "enabled" : "disabled");
        printf("Color space conversion: %s\r\n",
                        csc_coeff_sel_rb[cp_read(state, 0xfc) >> 4]);

        if (!DIGITAL_INPUT)
                return 0;

        printf("-----%s status-----\r\n", is_hdmi(state) ? "HDMI" : "DVI-D");
        printf("HDCP encrypted content: %s\r\n", (hdmi_read(state, 0x05) & 0x40) ? "true" : "false");
        printf("HDCP keys read: %s%s\r\n",
                        (hdmi_read(state, 0x04) & 0x20) ? "yes" : "no",
                        (hdmi_read(state, 0x04) & 0x10) ? "ERROR" : "");
        if (!is_hdmi(state)) {
                audio_pll_locked = hdmi_read(state, 0x04) & 0x01;
                audio_sample_packet_detect = hdmi_read(state, 0x18) & 0x01;
                audio_mute = io_read(state, 0x65) & 0x40;

                printf("Audio: pll %s, samples %s, %s\r\n",
                                audio_pll_locked ? "locked" : "not locked",
                                audio_sample_packet_detect ? "detected" : "not detected",
                                audio_mute ? "muted" : "enabled");
                if (audio_pll_locked && audio_sample_packet_detect) {
                        printf("Audio format: %s\r\n",
                                        (hdmi_read(state, 0x07) & 0x20) ? "multi-channel" : "stereo");
                }
                printf("Audio CTS: %u\r\n", (hdmi_read(state, 0x5b) << 12) +
                                (hdmi_read(state, 0x5c) << 8) +
                                (hdmi_read(state, 0x5d) & 0xf0));
                printf("Audio N: %u\r\n", ((hdmi_read(state, 0x5d) & 0x0f) << 16) +
                                (hdmi_read(state, 0x5e) << 8) +
                                hdmi_read(state, 0x5f));
                printf("AV Mute: %s\r\n", (hdmi_read(state, 0x04) & 0x40) ? "on" : "off");

                printf("Deep color mode: %s\r\n", deep_color_mode_txt[(hdmi_read(state, 0x0b) & 0x60) >> 5]);

                ADV7604_PrintAVIInfoframe(state);
        }

        return 0;
}

static unsigned char GetChipRevision(struct adv7604_state *state, unsigned char *RxRev)
{
	unsigned char ChipRev;

	ChipRev = io_read(state, 0x11);
	*RxRev = ChipRev;
	if(ChipRev == 0xFF)
		return 0xFF;
	else
    return 1;
}

static unsigned hblanking(const struct v4l2_bt_timings *t)
{
	return V4L2_DV_BT_BLANKING_WIDTH(t);
}

static unsigned htotal(const struct v4l2_bt_timings *t)
{
	return V4L2_DV_BT_FRAME_WIDTH(t);
}

static unsigned vblanking(const struct v4l2_bt_timings *t)
{
	return V4L2_DV_BT_BLANKING_HEIGHT(t);
}

static unsigned vtotal(const struct v4l2_bt_timings *t)
{
	return V4L2_DV_BT_FRAME_HEIGHT(t);
}

static int find_and_set_predefined_video_timings(struct adv7604_state *state,
                unsigned char prim_mode,
                const struct adv7604_video_standards *predef_vid_timings,
                const struct v4l2_dv_timings *timings)
{
//        struct adv7604_state *state = to_state(sd);
        int i;

        for (i = 0; predef_vid_timings[i].timings.Unionv4l2_bt_timings.bt.width; i++) {
                if (!v4l2_match_dv_timings(timings, &predef_vid_timings[i].timings,
                                        DIGITAL_INPUT ? 250000 : 1000000))
                        continue;
                io_write(state, 0x00, predef_vid_timings[i].vid_std); /* video std */
                io_write(state, 0x01, (predef_vid_timings[i].v_freq << 4) +
                                prim_mode); /* v_freq and prim mode */
				#if(ADV7604_DEBUG)
        printf("%s i = %d, vid_std = 0x%02X, prim_mode = %d\r\n", __func__, i, predef_vid_timings[i].vid_std, prim_mode);
				#endif
                return 0;
        }

        return -1;
}

static int configure_predefined_video_timings(struct adv7604_state *state,
                struct v4l2_dv_timings *timings)
{
//        struct adv7604_state *state = to_state(sd);
        int err;
				#if(ADV7604_DEBUG)
        printf("%s\r\n", __func__);
				#endif
        /* reset to default values */
        io_write(state, 0x16, 0x43);
        io_write(state, 0x17, 0x5a);
        /* disable embedded syncs for auto graphics mode */
        cp_write_and_or(state, 0x81, 0xef, 0x00);
        cp_write(state, 0x8f, 0x00);
        cp_write(state, 0x90, 0x00);
        cp_write(state, 0xa2, 0x00);
        cp_write(state, 0xa3, 0x00);
        cp_write(state, 0xa4, 0x00);
        cp_write(state, 0xa5, 0x00);
        cp_write(state, 0xa6, 0x00);
        cp_write(state, 0xa7, 0x00);
        cp_write(state, 0xab, 0x00);
        cp_write(state, 0xac, 0x00);

        switch (state->mode) {
        case ADV7604_MODE_COMP:
                err = find_and_set_predefined_video_timings(state,
                                0x01, adv7604_prim_mode_comp, timings);
                if (err)
                        err = find_and_set_predefined_video_timings(state,
                                        0x01, adv7604_prim_mode_gr, timings);
                break;
        case ADV7604_MODE_GR:
								err = find_and_set_predefined_video_timings(state,
																0x02, adv7604_prim_mode_gr, timings);
				/*
                if (err)
												err = find_and_set_predefined_video_timings(state,
																				0x02, adv7604_prim_mode_comp, timings);
				*/
                break;
        case ADV7604_MODE_HDMI:
                err = find_and_set_predefined_video_timings(state,
                                0x05, adv7604_prim_mode_hdmi_comp, timings);
                if (err)
                        err = find_and_set_predefined_video_timings(state,
                                        0x06, adv7604_prim_mode_hdmi_gr, timings);
                break;
        default:
                printf("Unknown mode %d\r\n", state->mode);
                err = -1;
                break;
        }


        return err;
}

static void configure_custom_video_timings(struct adv7604_state *state,
                const struct v4l2_bt_timings *bt)
{
//        struct adv7604_state *state = to_state(sd);
//        struct i2c_client *client = v4l2_get_subdevdata(sd);
        unsigned long width = htotal(bt);
        unsigned long height = vtotal(bt);
        unsigned short cp_start_sav = bt->hsync + bt->hbackporch - 4;
        unsigned short cp_start_eav = width - bt->hfrontporch;
        unsigned short cp_start_vbi = height - bt->vfrontporch;
        unsigned short cp_end_vbi = bt->vsync + bt->vbackporch;
        unsigned short ch1_fr_ll = (((unsigned long)bt->pixelclock / 100) > 0) ?
                ((width * (ADV7604_fsc / 100)) / ((unsigned long)bt->pixelclock / 100)) : 0;
        unsigned char pll[2] = {0, 0};

				pll[0] = 0xc0 | ((width >> 8) & 0x1f);
				pll[1] = width & 0xff;
				#if(ADV7604_DEBUG)
        printf("%s: width = %d, Pll = 0x%02X%02X\r\n", __func__, width, pll[0], pll[1]);
				#endif
        switch (state->mode) {
        case ADV7604_MODE_COMP:
        case ADV7604_MODE_GR:
                /* auto graphics */
                io_write(state, 0x00, 0x07); /* video std */
                io_write(state, 0x01, 0x02); /* prim mode */
                /* enable embedded syncs for auto graphics mode */
                cp_write_and_or(state, 0x81, 0xef, 0x10);

                /* Should only be set in auto-graphics mode [REF_02, p. 91-92] */
                /* setup PLL_DIV_MAN_EN and PLL_DIV_RATIO */
                /* IO-map reg. 0x16 and 0x17 should be written in sequence */
                if (I2C0WriteByte(ADV7604_SLAVE_ADDR, 0x16, I2C_DEV_MEMORY_ADDRESS_8BIT, 2, pll) == (BYTE) ERR)
								{
                        printf("%s writing to reg 0x16 and 0x17 failed\r\n", __func__);
                        break;
                }

                /* active video - horizontal timing */
                cp_write(state, 0xa2, (cp_start_sav >> 4) & 0xff);
                cp_write(state, 0xa3, ((cp_start_sav & 0x0f) << 4) |
                                        ((cp_start_eav >> 8) & 0x0f));
                cp_write(state, 0xa4, cp_start_eav & 0xff);

                /* active video - vertical timing */
                cp_write(state, 0xa5, (cp_start_vbi >> 4) & 0xff);
                cp_write(state, 0xa6, ((cp_start_vbi & 0xf) << 4) |
                                        ((cp_end_vbi >> 8) & 0xf));
                cp_write(state, 0xa7, cp_end_vbi & 0xff);
                break;
        case ADV7604_MODE_HDMI:
                /* set default prim_mode/vid_std for HDMI
                   accoring to [REF_03, c. 4.2] */
//                io_write(state, 0x00, 0x02); /* video std */
//                io_write(state, 0x01, 0x06); /* prim mode */
                io_write(state, 0x00, 0x1E); /* video std */
                io_write(state, 0x01, 0x05); /* prim mode */
                break;
        default:
                printf("%s: Unknown mode %d\r\n", __func__, state->mode);
                break;
        }

        cp_write(state, 0x8f, (ch1_fr_ll >> 8) & 0x7);
        cp_write(state, 0x90, ch1_fr_ll & 0xff);
        cp_write(state, 0xab, (height >> 4) & 0xff);
        cp_write(state, 0xac, (height & 0x0f) << 4);
}

static void set_rgb_quantization_range(struct adv7604_state *state)
{
//        struct adv7604_state *state = to_state(sd);

        switch (state->rgb_quantization_range)
				{
								case V4L2_DV_RGB_RANGE_AUTO:
												/* automatic */
												if (DIGITAL_INPUT && !(hdmi_read(state, 0x05) & 0x80)) {
																/* receiving DVI-D signal */

																/* ADV7604 selects RGB limited range regardless of
																	 input format (CE/IT) in automatic mode */
																if (state->timings.Unionv4l2_bt_timings.bt.standards & V4L2_DV_BT_STD_CEA861) {
																				/* RGB limited range (16-235) */
																				io_write_and_or(state, 0x02, 0x0f, 0x00);

																} else {
																				/* RGB full range (0-255) */
																				io_write_and_or(state, 0x02, 0x0f, 0x10);
																}
												} else {
																/* receiving HDMI or analog signal, set automode */
																io_write_and_or(state, 0x02, 0x0f, 0xf0);
												}
												break;
								case V4L2_DV_RGB_RANGE_LIMITED:
												/* RGB limited range (16-235) */
												io_write_and_or(state, 0x02, 0x0f, 0x00);
												break;
								case V4L2_DV_RGB_RANGE_FULL:
												/* RGB full range (0-255) */
												io_write_and_or(state, 0x02, 0x0f, 0x10);
												break;
        }
}


int adv7604_set_ctrl(struct adv7604_state *state, int CtrlId, unsigned long CtrlVal)
{
//        struct v4l2_subdev *sd = to_sd(ctrl);
//        struct adv7604_state *state = to_state(sd);

        switch (CtrlId) {
        case V4L2_CID_BRIGHTNESS:
                cp_write(state, 0x3c, CtrlVal);
                return 0;
        case V4L2_CID_CONTRAST:
                cp_write(state, 0x3a, CtrlVal);
                return 0;
        case V4L2_CID_SATURATION:
                cp_write(state, 0x3b, CtrlVal);
                return 0;
        case V4L2_CID_HUE:
                cp_write(state, 0x3d, CtrlVal);
                return 0;
        case  V4L2_CID_DV_RX_RGB_RANGE:
                state->rgb_quantization_range = CtrlVal;
                set_rgb_quantization_range(state);
                return 0;
        case V4L2_CID_ADV_RX_ANALOG_SAMPLING_PHASE:
                /* Set the analog sampling phase. This is needed to find the
                   best sampling phase for analog video: an application or
                   driver has to try a number of phases and analyze the picture
                   quality before settling on the best performing phase. */
                afe_write(state, 0xc8, CtrlVal);
                return 0;
        case V4L2_CID_ADV_RX_FREE_RUN_COLOR_MANUAL:
                /* Use the default blue color for free running mode,
                   or supply your own. */
                cp_write_and_or(state, 0xbf, ~0x04, (CtrlVal << 2));
                return 0;
        case V4L2_CID_ADV_RX_FREE_RUN_COLOR:
                cp_write(state, 0xc0, (CtrlVal & 0xff0000) >> 16);
                cp_write(state, 0xc1, (CtrlVal & 0x00ff00) >> 8);
                cp_write(state, 0xc2, (unsigned char)(CtrlVal & 0x0000ff));
                return 0;
        }
        return -1;
}

static int stdi2dv_timings(struct adv7604_state *state,
                struct stdi_readback *stdi,
                struct v4l2_dv_timings *timings)
{
//        struct adv7604_state *state = to_state(sd);
        unsigned long hfreq = (ADV7604_fsc * 8) / stdi->bl;
        unsigned long pix_clk, TempVtotal, TempVsync;
        int i;
        for (i = 0; adv7604_timings[i].Unionv4l2_bt_timings.bt.height; i++) {
					/*
                if (vtotal(&adv7604_timings[i].Unionv4l2_bt_timings.bt) != stdi->lcf + 1)
                        continue;
                if (adv7604_timings[i].Unionv4l2_bt_timings.bt.vsync != stdi->lcvs)
                        continue;
					*/
					TempVtotal = vtotal(&adv7604_timings[i].Unionv4l2_bt_timings.bt);
					TempVsync = adv7604_timings[i].Unionv4l2_bt_timings.bt.vsync;
                if((TempVtotal <= stdi->lcf + 1 + 6) &&
										(TempVtotal >= stdi->lcf + 1 - 6) &&
										(TempVsync <= stdi->lcvs + 3) &&
										(TempVsync >= stdi->lcvs - 3))
								{
												#if(ADV7604_DEBUG)
//                        printf("%s: adv7604_timings Index = %d\r\n", __func__, i);
												#endif
									;
								}
								else
									continue;

                pix_clk = hfreq * htotal(&adv7604_timings[i].Unionv4l2_bt_timings.bt);
//								printf("%s: TimingsIndex = %d, pix_clk = %ld, hfreq = %d, htotal = %d\r\n", __func__, i, pix_clk, hfreq, htotal);

                if ((pix_clk < adv7604_timings[i].Unionv4l2_bt_timings.bt.pixelclock + 1000000) &&
                    (pix_clk > adv7604_timings[i].Unionv4l2_bt_timings.bt.pixelclock - 1000000)) {
                        //*timings = adv7604_timings[i];
												#if(ADV7604_DEBUG >= 0)
												printf("%s: Select TimingsIndex = %d, pix_clk = %ld, hfreq = %d, htotal = %d\r\n", __func__, i, pix_clk, hfreq, htotal);
												#endif
												memcpy(timings, &adv7604_timings[i], sizeof(struct v4l2_dv_timings));
                        return 0;
                }
        }

        if (v4l2_detect_cvt(stdi->lcf + 1, hfreq, stdi->lcvs,
                        (stdi->hs_pol == '+' ? V4L2_DV_HSYNC_POS_POL : 0) |
                        (stdi->vs_pol == '+' ? V4L2_DV_VSYNC_POS_POL : 0),
                        timings))
                return 0;

        if (v4l2_detect_gtf(stdi->lcf + 1, hfreq, stdi->lcvs,
                        (stdi->hs_pol == '+' ? V4L2_DV_HSYNC_POS_POL : 0) |
                        (stdi->vs_pol == '+' ? V4L2_DV_VSYNC_POS_POL : 0),
                        state->aspect_ratio, timings))
                return 0;
				#if(ADV7604_DEBUG)
        printf("%s: No format candidate found for lcvs = %d, lcf=%d, bl = %d, %chsync, %cvsync\r\n",
                __func__, stdi->lcvs, stdi->lcf, stdi->bl,
                stdi->hs_pol, stdi->vs_pol);
				#endif
        return -1;
}

static int read_stdi(struct adv7604_state *state, struct stdi_readback *stdi)
{
        if (no_lock_stdi(state) || no_lock_sspd(state)) {
								#if(ADV7604_DEBUG)
                printf("%s: STDI and/or SSPD not locked\r\n", __func__);
								#endif
                return -1;
        }

        /* read STDI */
        stdi->bl = ((cp_read(state, 0xb1) & 0x3f) << 8) | cp_read(state, 0xb2);
        stdi->lcf = ((cp_read(state, 0xb3) & 0x7) << 8) | cp_read(state, 0xb4);
        stdi->lcvs = cp_read(state, 0xb3) >> 3;
        stdi->interlaced = io_read(state, 0x12) & 0x10;

        /* read SSPD */
        if ((cp_read(state, 0xb5) & 0x03) == 0x01) {
                stdi->hs_pol = ((cp_read(state, 0xb5) & 0x10) ?
                                ((cp_read(state, 0xb5) & 0x08) ? '+' : '-') : 'x');
                stdi->vs_pol = ((cp_read(state, 0xb5) & 0x40) ?
                                ((cp_read(state, 0xb5) & 0x20) ? '+' : '-') : 'x');
        } else {
                stdi->hs_pol = 'x';
                stdi->vs_pol = 'x';
        }

        if (no_lock_stdi(state) || no_lock_sspd(state)) {
								#if(ADV7604_DEBUG)
                printf("%s: signal lost during readout of STDI/SSPD\r\n", __func__);
								#endif
                return -1;
        }

        if (stdi->lcf < 239 || stdi->bl < 8 || stdi->bl == 0x3fff) {
								#if(ADV7604_DEBUG)
                printf("%s: invalid signal\r\n", __func__);
								#endif
                memset(stdi, 0, sizeof(struct stdi_readback));
                return -1;
        }
				#if(ADV7604_DEBUG)
        printf("%s: lcf (frame height - 1) = %d, bl = %d, lcvs (vsync) = %d, %chsync, %cvsync, %s\r\n",
                __func__, stdi->lcf, stdi->bl, stdi->lcvs,
                stdi->hs_pol, stdi->vs_pol,
                stdi->interlaced ? "interlaced" : "progressive");
				#endif
        return 0;
}

static int adv7604_enum_dv_timings(struct adv7604_state *state,
                        struct v4l2_enum_dv_timings *timings)
{
        if (timings->index >= ARRAY_SIZE(adv7604_timings) - 1)
                return -1;
        memset(timings->reserved, 0, sizeof(timings->reserved));
        timings->timings = adv7604_timings[timings->index];
        return 0;
}

static int adv7604_dv_timings_cap(struct adv7604_state *state,
                        struct v4l2_dv_timings_cap *cap)
{
//        struct adv7604_state *state = to_state(state);

        cap->type = V4L2_DV_BT_656_1120;
        cap->Union_v4l2_bt_timings_cap.bt.max_width = 1920;
        cap->Union_v4l2_bt_timings_cap.bt.max_height = 1200;
        cap->Union_v4l2_bt_timings_cap.bt.min_pixelclock = 25000000;
        if (DIGITAL_INPUT)
                cap->Union_v4l2_bt_timings_cap.bt.max_pixelclock = ADV7604_DIGITAL_INPUT_MAX_FREQ;
        else
                cap->Union_v4l2_bt_timings_cap.bt.max_pixelclock = ADV7604_ANALOG_INPUT_MAX_FREQ;
        cap->Union_v4l2_bt_timings_cap.bt.standards = V4L2_DV_BT_STD_CEA861 | V4L2_DV_BT_STD_DMT |
                         V4L2_DV_BT_STD_GTF | V4L2_DV_BT_STD_CVT;
        cap->Union_v4l2_bt_timings_cap.bt.capabilities = V4L2_DV_BT_CAP_PROGRESSIVE |
                V4L2_DV_BT_CAP_REDUCED_BLANKING | V4L2_DV_BT_CAP_CUSTOM;
        return 0;
}

/* Fill the optional fields .standards and .flags in struct v4l2_dv_timings
   if the format is listed in adv7604_timings[] */
static void adv7604_fill_optional_dv_timings_fields(struct adv7604_state *state,
                struct v4l2_dv_timings *timings)
{
//        struct adv7604_state *state = to_state(state);
        int i;

        for (i = 0; adv7604_timings[i].Unionv4l2_bt_timings.bt.width; i++) {
                if (v4l2_match_dv_timings(timings, &adv7604_timings[i],
                                        DIGITAL_INPUT ? 250000 : 1000000)) {
												#if(ADV7604_DEBUG)
												printf("%s: TimingsIndex = %d\r\n", __func__, i);
												#endif
                       //*timings = adv7604_timings[i];
												memcpy(timings, &adv7604_timings[i], sizeof(struct v4l2_dv_timings));
                        break;
                }
        }
}

static int adv7604_query_dv_timings(struct adv7604_state *state,
                        struct v4l2_dv_timings *timings)
{
//        struct adv7604_state *state = to_state(state);
        struct v4l2_bt_timings *bt = &timings->Unionv4l2_bt_timings.bt;
        struct stdi_readback stdi;

        if (!timings)
                return -1;

        memset(timings, 0, sizeof(struct v4l2_dv_timings));

        if (no_signal(state)) {
								#if(ADV7604_DEBUG)
                printf("%s: no valid signal\r\n", __func__);
								#endif
                return -1;
        }

        /* read STDI */
        if (read_stdi(state, &stdi)) {
								#if(ADV7604_DEBUG)
                printf("%s: STDI/SSPD not locked\r\n", __func__);
								#endif
                return -1;
        }
        bt->interlaced = stdi.interlaced ?
                V4L2_DV_INTERLACED : V4L2_DV_PROGRESSIVE;

        if (DIGITAL_INPUT) {
                uint32_t freq;

                timings->type = V4L2_DV_BT_656_1120;

                bt->width = (hdmi_read(state, 0x07) & 0x0f) * 256 + hdmi_read(state, 0x08);
                bt->height = (hdmi_read(state, 0x09) & 0x0f) * 256 + hdmi_read(state, 0x0a);
                freq = (hdmi_read(state, 0x06) * 1000000) +
                        ((hdmi_read(state, 0x3b) & 0x30) >> 4) * 250000;
                if (is_hdmi(state)) {
                        /* adjust for deep color mode */
                        unsigned bits_per_channel = ((hdmi_read(state, 0x0b) & 0x60) >> 4) + 8;

                        freq = freq * 8 / bits_per_channel;
                }
                bt->pixelclock = freq;
                bt->hfrontporch = (hdmi_read(state, 0x20) & 0x03) * 256 +
                        hdmi_read(state, 0x21);
                bt->hsync = (hdmi_read(state, 0x22) & 0x03) * 256 +
                        hdmi_read(state, 0x23);
                bt->hbackporch = (hdmi_read(state, 0x24) & 0x03) * 256 +
                        hdmi_read(state, 0x25);
                bt->vfrontporch = ((hdmi_read(state, 0x2a) & 0x1f) * 256 +
                        hdmi_read(state, 0x2b)) / 2;
                bt->vsync = ((hdmi_read(state, 0x2e) & 0x1f) * 256 +
                        hdmi_read(state, 0x2f)) / 2;
                bt->vbackporch = ((hdmi_read(state, 0x32) & 0x1f) * 256 +
                        hdmi_read(state, 0x33)) / 2;
                bt->polarities = ((hdmi_read(state, 0x05) & 0x10) ? V4L2_DV_VSYNC_POS_POL : 0) |
                        ((hdmi_read(state, 0x05) & 0x20) ? V4L2_DV_HSYNC_POS_POL : 0);
                if (bt->interlaced == V4L2_DV_INTERLACED) {
                        bt->height += (hdmi_read(state, 0x0b) & 0x0f) * 256 +
                                        hdmi_read(state, 0x0c);
                        bt->il_vfrontporch = ((hdmi_read(state, 0x2c) & 0x1f) * 256 +
                                        hdmi_read(state, 0x2d)) / 2;
                        bt->il_vsync = ((hdmi_read(state, 0x30) & 0x1f) * 256 +
                                        hdmi_read(state, 0x31)) / 2;
                        bt->vbackporch = ((hdmi_read(state, 0x34) & 0x1f) * 256 +
                                        hdmi_read(state, 0x35)) / 2;
                }
                adv7604_fill_optional_dv_timings_fields(state, timings);
        } else {
                /* find format
                 * Since LCVS values are inaccurate [REF_03, p. 275-276],
                 * stdi2dv_timings() is called with lcvs +-2 if the first attempt fails.
                 */
                if (!stdi2dv_timings(state, &stdi, timings))
                        goto found;
                stdi.lcvs += 2;
								#if(ADV7604_DEBUG)
                printf("%s: lcvs + 2 = %d\r\n", __func__, stdi.lcvs);
								#endif
                if (!stdi2dv_timings(state, &stdi, timings))
                        goto found;
                stdi.lcvs -= 4;
								#if(ADV7604_DEBUG)
                printf("%s: lcvs - 2 = %d\r\n", __func__, stdi.lcvs);
								#endif
                if (stdi2dv_timings(state, &stdi, timings)) {
                        /*
                         * The STDI block may measure wrong values, especially
                         * for lcvs and lcf. If the driver can not find any
                         * valid timing, the STDI block is restarted to measure
                         * the video timings again. The function will return an
                         * error, but the restart of STDI will generate a new
                         * STDI interrupt and the format detection process will
                         * restart.
                         */
                        if (state->restart_stdi_once) {
																#if(ADV7604_DEBUG)
                                printf("%s: restart STDI\r\n", __func__);
																#endif
                                /* TODO restart STDI for Sync Channel 2 */
                                /* enter one-shot mode */
                                cp_write_and_or(state, 0x86, 0xf9, 0x00);
                                /* trigger STDI restart */
                                cp_write_and_or(state, 0x86, 0xf9, 0x04);
                                /* reset to continuous mode */
                                cp_write_and_or(state, 0x86, 0xf9, 0x02);
                                state->restart_stdi_once = FALSE;
                                return -1;
                        }
												#if(ADV7604_DEBUG)
                        printf("%s: format not supported\r\n", __func__);
												#endif
                        return -1;
                }
                state->restart_stdi_once = TRUE;
        }
found:

        if (no_signal(state)) {
								#if(ADV7604_DEBUG)
                printf("%s: signal lost during readout\r\n", __func__);
								#endif
                memset(timings, 0, sizeof(struct v4l2_dv_timings));
                return -1;
        }

        if ((!DIGITAL_INPUT && bt->pixelclock > ADV7604_ANALOG_INPUT_MAX_FREQ) ||
                        (DIGITAL_INPUT && bt->pixelclock > ADV7604_DIGITAL_INPUT_MAX_FREQ)) {
								#if(ADV7604_DEBUG)
                printf("%s: pixelclock out of range %d\r\n",
                                __func__, (unsigned long)bt->pixelclock);
								#endif
                return -1;
        }

        #if(ADV7604_DEBUG)
                v4l2_print_dv_timings("ADV7604", "adv7604_query_dv_timings: ",
                                      timings, TRUE);
				#endif

        return 0;
}

static int adv7604_set_dv_timings(struct adv7604_state *state,
                struct v4l2_dv_timings *timings)
{
//        struct adv7604_state *state = to_state(state);
        struct v4l2_bt_timings *bt;
        int err;

        if (!timings)
                return -1;

        bt = &timings->Unionv4l2_bt_timings.bt;

        if ((!DIGITAL_INPUT && bt->pixelclock > ADV7604_ANALOG_INPUT_MAX_FREQ) ||
                        (DIGITAL_INPUT && bt->pixelclock > ADV7604_DIGITAL_INPUT_MAX_FREQ)) {
								#if(ADV7604_DEBUG)
                printf("%s: pixelclock out of range %d\r\n",
                                __func__, (unsigned long)bt->pixelclock);
								#endif
                return -1;
        }

        adv7604_fill_optional_dv_timings_fields(state, timings);

//        state->timings = *timings;
				memcpy(&(state->timings), timings, sizeof(struct v4l2_dv_timings));

        cp_write(state, 0x91, bt->interlaced ? 0x50 : 0x10);

        /* Use prim_mode and vid_std when available */
        err = configure_predefined_video_timings(state, timings);
        if (err) {
                /* custom settings when the video format
                 does not have prim_mode/vid_std */
                configure_custom_video_timings(state, bt);
        }

        set_rgb_quantization_range(state);


        #if(ADV7604_DEBUG)
                v4l2_print_dv_timings("ADV7604", "adv7604_s_dv_timings: ",
                                      timings, TRUE);
				#endif
        return 0;
}

static int adv7604_get_dv_timings(struct adv7604_state *state,
                struct v4l2_dv_timings *timings)
{
//        struct adv7604_state *state = to_state(state);

        *timings = state->timings;
        return 0;
}

int adv7604_isr(struct adv7604_state *state, unsigned long status, unsigned char *handled)
{
//        struct adv7604_state *state = to_state(sd);
        unsigned char fmt_change, fmt_change_digital;
//        unsigned char tx_5v;
        unsigned long input_status;
				struct v4l2_bt_timings *Tempbt;
        unsigned long htot, vtot, StdVFreqSource, StdVFreqNew;

        // format change
//				SelectDeviceI2CBus(2, 0);
//				PCA9546A_SelectBusPort(BIT0);
        fmt_change = io_read(state, 0x43) & 0x98;
        if (fmt_change)
                io_write(state, 0x44, fmt_change);
        fmt_change_digital = DIGITAL_INPUT ? (io_read(state, 0x6b) & 0xc0) : 0;
        if (fmt_change_digital)
                io_write(state, 0x6c, fmt_change_digital);
        if (fmt_change || fmt_change_digital || (state->prev_input_status & V4L2_IN_ST_NO_POWER)) {
								#if(ADV7604_DEBUG)
/*
                printf("%s: fmt_change = 0x%X, fmt_change_digital = 0x%X\r\n",
                        __func__, fmt_change, fmt_change_digital);
*/
								#endif

                adv7604_get_input_status(state, &input_status);
                if (input_status != state->prev_input_status) {
												#if(ADV7604_DEBUG)
                        printf("%s: input_status = 0x%X, prev_input_status = 0x%X\r\n",
                                __func__, input_status, state->prev_input_status);
												#endif
												if((input_status != 0) || (state->prev_input_status != 0))
												{
													if(adv7604_query_dv_timings(state, &StaticAdv7604Timings))
													{
															#if(ADV7604_DEBUG)
															printf("No video detected\r\n");
															#endif
															//return 0;
													}
													else
													{
														Tempbt = &(state->timings.Unionv4l2_bt_timings.bt);
														htot = V4L2_DV_BT_FRAME_WIDTH(Tempbt);
														vtot = V4L2_DV_BT_FRAME_HEIGHT(Tempbt);
														StdVFreqSource = (htot * vtot) > 0 ? ((unsigned long)Tempbt->pixelclock / (htot * vtot)) : 0;
														Tempbt = &(StaticAdv7604Timings.Unionv4l2_bt_timings.bt);
														htot = V4L2_DV_BT_FRAME_WIDTH(Tempbt);
														vtot = V4L2_DV_BT_FRAME_HEIGHT(Tempbt);
														StdVFreqNew = (htot * vtot) > 0 ? ((unsigned long)Tempbt->pixelclock / (htot * vtot)) : 0;
														if((state->timings.Unionv4l2_bt_timings.bt.width != StaticAdv7604Timings.Unionv4l2_bt_timings.bt.width) ||
																(state->timings.Unionv4l2_bt_timings.bt.height != StaticAdv7604Timings.Unionv4l2_bt_timings.bt.height) ||
																(StdVFreqSource != StdVFreqNew)
															)
														{
															/*
															printf("%s, New State:width = %ld, height = %ld, hfrontporch = %ld, hsync = %ld, hbackporch = %ld, vfrontporch = %ld, vsync = %ld, vbackporch = %ld, il_vfrontporch = %ld, il_vsync = %ld, il_vbackporch = %ld\r\n", __func__,
																							StaticAdv7604Timings.Unionv4l2_bt_timings.bt.width,
																							StaticAdv7604Timings.Unionv4l2_bt_timings.bt.height,
																							StaticAdv7604Timings.Unionv4l2_bt_timings.bt.hfrontporch,
																							StaticAdv7604Timings.Unionv4l2_bt_timings.bt.hsync,
																							StaticAdv7604Timings.Unionv4l2_bt_timings.bt.hbackporch,
																							StaticAdv7604Timings.Unionv4l2_bt_timings.bt.vfrontporch,
																							StaticAdv7604Timings.Unionv4l2_bt_timings.bt.vsync,
																							StaticAdv7604Timings.Unionv4l2_bt_timings.bt.vbackporch,
																							StaticAdv7604Timings.Unionv4l2_bt_timings.bt.il_vfrontporch,
																							StaticAdv7604Timings.Unionv4l2_bt_timings.bt.il_vsync,
																							StaticAdv7604Timings.Unionv4l2_bt_timings.bt.il_vbackporch
																		);
															*/
															if(adv7604_set_dv_timings(state, &StaticAdv7604Timings) == -1)
																printf("%s adv7604_set_dv_timings Error\r\n", __func__);
														}
														
													}
												}
                        state->prev_input_status = input_status;
//                        v4l2_subdev_notify(state, ADV7604_FMT_CHANGE, NULL);
                }

                if (handled)
                        *handled = TRUE;
        }
				#if 1						// HL ADD
				StdVFreqSource = io_read(state, 0x01) & 0x0f;
				StdVFreqNew = io_read(state, 0x00) & 0x3f;
				if((StdVFreqSource == 0) && (StdVFreqNew == 0))
				{
					ADV7604_SelectInput(state);
					ADV7604_EnableInput(state);
					if(state->mode == ADV7604_MODE_COMP)
						ADV7604_SetTimings(state, 5);
					if(state->mode == ADV7604_MODE_GR)
						ADV7604_SetTimings(state, 49);
					if(state->mode == ADV7604_MODE_HDMI)
						ADV7604_SetTimings(state, 10);
					state->prev_input_status = V4L2_IN_ST_NO_SIGNAL | V4L2_IN_ST_NO_POWER;
					printf("%s: adv7604 ReSet Input and timing\r\n", __func__);
				}
				#endif
        // tx 5v detect
				/*
        tx_5v = io_read(state, 0x70) & 0x10;
        if (tx_5v) {
                v4l2_dbg(1, debug, state, "%s: tx_5v: 0x%X\r\n", __func__, tx_5v);
                io_write(state, 0x71, tx_5v);
                adv7604_s_detect_tx_5v_ctrl(state);
                if (handled)
                        *handled = true;
        }
				*/
        return 0;
}

int ADV7604_GetSignalActiveWidthHeight(struct adv7604_state *state, long *pWidth, long *pHeight)
{
	unsigned long input_status;
	adv7604_get_input_status(state, &input_status);
	if((input_status & V4L2_IN_ST_NO_SIGNAL) || (input_status & V4L2_IN_ST_NO_POWER))
	{
		*pWidth = 0;
		*pHeight = 0;
		return 0;
	}
	else
	{
		*pWidth = state->timings.Unionv4l2_bt_timings.bt.width;
		*pHeight = state->timings.Unionv4l2_bt_timings.bt.height;
	}
	return 1;
}

static unsigned char no_power(struct adv7604_state *state)
{
	// Entire chip or CP powered off
	return io_read(state, 0x0c) & 0x24;
}

static unsigned char no_signal_tmds(struct adv7604_state *state)
{
	// TODO port B, C and D
	unsigned char Ret = 0;
	Ret = !(io_read(state, 0x6a) & 0x10);
	return Ret;
}

static unsigned char no_lock_tmds(struct adv7604_state *state)
{
	unsigned char Ret = 0;
	Ret = (io_read(state, 0x6a) & 0xe0) != 0xe0;
	return Ret;
}

static unsigned char is_hdmi(struct adv7604_state *state)
{
	unsigned char Ret = 0;
	Ret = hdmi_read(state, 0x05) & 0x80;
	return Ret;
}

static unsigned char no_lock_sspd(struct adv7604_state *state)
{
	// TODO channel 2
	unsigned char Ret = 0;
	if(state->mode == ADV7604_MODE_COMP)
		Ret = ((cp_read(state, 0xb5) & 0x83) != 0x83);
	else
		Ret = ((cp_read(state, 0xb5) & 0xd0) != 0xd0);
	return Ret;
}

static unsigned char no_lock_stdi(struct adv7604_state *state)
{
	// TODO channel 2
	unsigned char Ret = 0;
	Ret = !(cp_read(state, 0xb1) & 0x80);
	return Ret;
}

static unsigned char no_signal(struct adv7604_state *state)
{
	unsigned char ret;

	ret = no_power(state);

	ret |= no_lock_stdi(state);
	ret |= no_lock_sspd(state);

	if (DIGITAL_INPUT)
	{
		ret |= no_lock_tmds(state);
		ret |= no_signal_tmds(state);
	}

	return ret;
}

static unsigned char no_lock_cp(struct adv7604_state *state)
{
	// CP has detected a non standard number of lines on the incoming
	// video compared to what it is configured to receive by s_dv_timings
	unsigned char Ret = 0;
	Ret = io_read(state, 0x12) & 0x01;
	return Ret;
}

static int adv7604_get_input_status(struct adv7604_state *state, unsigned long *status)
{
//        struct adv7604_state *state = to_state(state);

        *status = 0;
        *status |= no_power(state) ? V4L2_IN_ST_NO_POWER : 0;
        *status |= no_signal(state) ? V4L2_IN_ST_NO_SIGNAL : 0;
        if (no_lock_cp(state))
                *status |= DIGITAL_INPUT ? V4L2_IN_ST_NO_SYNC : V4L2_IN_ST_NO_H_LOCK;

//        printf("%s: status = 0x%X\r\n", __func__, *status);

        return 0;
}


static unsigned char io_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	aByte = I2C0ReadByte(ADV7604_SLAVE_ADDR, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", ADV7604_SLAVE_ADDR, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char io_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", ADV7604_SLAVE_ADDR, reg, val);
	#endif
	/*
	if(reg == 0x01)
		printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", ADV7604_SLAVE_ADDR, reg, val);
	*/
	Ret = I2C0WriteByte(ADV7604_SLAVE_ADDR, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static unsigned char io_write_and_or(struct adv7604_state *state, unsigned char reg, unsigned char mask, unsigned char val)
{
	unsigned char Ret = 0;
	Ret = io_write(state, reg, (io_read(state, reg) & mask) | val);
	return Ret;
}

static unsigned char avlink_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	struct adv7604_platform_data *pdata = &state->pdata;
	aByte = I2C0ReadByte(pdata->i2c_avlink, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_avlink, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char avlink_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_avlink, reg, val);
	#endif
	Ret = I2C0WriteByte(pdata->i2c_avlink, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static unsigned char cec_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	struct adv7604_platform_data *pdata = &state->pdata;
	aByte = I2C0ReadByte(pdata->i2c_cec, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_cec, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char cec_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_cec, reg, val);
	#endif
	Ret = I2C0WriteByte(pdata->i2c_cec, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static unsigned char cec_write_and_or(struct adv7604_state *state, unsigned char reg, unsigned char mask, unsigned char val)
{
	unsigned char Ret = 0;
	Ret = cec_write(state, reg, (cec_read(state, reg) & mask) | val);
	return Ret;
}

static unsigned char infoframe_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	struct adv7604_platform_data *pdata = &state->pdata;
	aByte = I2C0ReadByte(pdata->i2c_infoframe, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_infoframe, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char infoframe_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_infoframe, reg, val);
	#endif
	Ret = I2C0WriteByte(pdata->i2c_infoframe, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static unsigned char esdp_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	struct adv7604_platform_data *pdata = &state->pdata;
	aByte = I2C0ReadByte(pdata->i2c_esdp, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_esdp, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char esdp_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_esdp, reg, val);
	#endif
	Ret = I2C0WriteByte(pdata->i2c_esdp, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static unsigned char dpp_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	struct adv7604_platform_data *pdata = &state->pdata;
	aByte = I2C0ReadByte(pdata->i2c_dpp, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_dpp, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char dpp_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_dpp, reg, val);
	#endif
	Ret = I2C0WriteByte(pdata->i2c_dpp, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static unsigned char afe_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	struct adv7604_platform_data *pdata = &state->pdata;
	aByte = I2C0ReadByte(pdata->i2c_afe, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_afe, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char afe_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_afe, reg, val);
	#endif
	Ret = I2C0WriteByte(pdata->i2c_afe, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static unsigned char rep_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	struct adv7604_platform_data *pdata = &state->pdata;
	aByte = I2C0ReadByte(pdata->i2c_repeater, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_repeater, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char rep_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_repeater, reg, val);
	#endif
	Ret = I2C0WriteByte(pdata->i2c_repeater, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static unsigned char rep_write_and_or(struct adv7604_state *state, unsigned char reg, unsigned char mask, unsigned char val)
{
	unsigned char Ret = 0;
	Ret = rep_write(state, reg, (rep_read(state, reg) & mask) | val);
	return Ret;
}

static unsigned char edid_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	struct adv7604_platform_data *pdata = &state->pdata;
	aByte = I2C0ReadByte(pdata->i2c_edid, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_edid, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char edid_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_edid, reg, val);
	#endif
	Ret = I2C0WriteByte(pdata->i2c_edid, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static unsigned char hdmi_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	struct adv7604_platform_data *pdata = &state->pdata;
	aByte = I2C0ReadByte(pdata->i2c_hdmi, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_hdmi, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char hdmi_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_hdmi, reg, val);
	#endif
	Ret = I2C0WriteByte(pdata->i2c_hdmi, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static unsigned char test_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	struct adv7604_platform_data *pdata = &state->pdata;
	aByte = I2C0ReadByte(pdata->i2c_test, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_test, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char test_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_test, reg, val);
	#endif
	Ret = I2C0WriteByte(pdata->i2c_test, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static unsigned char cp_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	struct adv7604_platform_data *pdata = &state->pdata;
	aByte = I2C0ReadByte(pdata->i2c_cp, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_cp, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char cp_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_cp, reg, val);
	#endif
	Ret = I2C0WriteByte(pdata->i2c_cp, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static unsigned char cp_write_and_or(struct adv7604_state *state, unsigned char reg, unsigned char mask, unsigned char val)
{
	unsigned char Ret = 0;
	Ret = (cp_read(state, reg) & mask) | val;
	Ret =cp_write(state, reg, Ret);
	return Ret;
//	return cp_write(state, reg, (cp_read(state, reg) & mask) | val);
}

static unsigned char vdp_read(struct adv7604_state *state, unsigned char reg)
{
	unsigned char *aByte = NULL;
	struct adv7604_platform_data *pdata = &state->pdata;
	aByte = I2C0ReadByte(pdata->i2c_vdp, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1);
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_READ 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_vdp, reg, *aByte);
	#endif
	return (*aByte);
}

static unsigned char vdp_write(struct adv7604_state *state, unsigned char reg, unsigned char val)
{
	struct adv7604_platform_data *pdata = &state->pdata;
	unsigned char Ret = 0;
	#if(ADV7604_DEBUG == 2)
	printf("ADV7604_WRITE 0x%02X 0x%02X 0x%02X\r\n", pdata->i2c_vdp, reg, val);
	#endif
	Ret = I2C0WriteByte(pdata->i2c_vdp, reg, I2C_DEV_MEMORY_ADDRESS_8BIT, 1, &val);
	return Ret;
}

static void ADV7604_WriteInternalEdid(struct adv7604_state *state, unsigned char EdidMode)
{
	int i = 0;
	if(EdidMode > 10)
	{
		InputFPGA_SetCustomEdid(EdidMode - 10);
		for(i = 0; i <= 127; i++)
			edid_write(state, i, g_InputEdidData[i]);
		for(i = 0x80; i <= 0xFF; i++)
			edid_write(state, i, 0);
		return;
	}
	
	if(EdidMode == 0)
	{
		edid_write(state, 0x00, 0x00);
		edid_write(state, 0x01, 0xFF);
		edid_write(state, 0x02, 0xFF);
		edid_write(state, 0x03, 0xFF);
		edid_write(state, 0x04, 0xFF);
		edid_write(state, 0x05, 0xFF);
		edid_write(state, 0x06, 0xFF);
		edid_write(state, 0x07, 0x00);
		edid_write(state, 0x08, 0x41);
		edid_write(state, 0x09, 0x0C);
		edid_write(state, 0x0A, 0x7B);
		edid_write(state, 0x0B, 0xC0);
		edid_write(state, 0x0C, 0x64);
		edid_write(state, 0x0D, 0x06);
		edid_write(state, 0x0E, 0x00);
		edid_write(state, 0x0F, 0x00);
		edid_write(state, 0x10, 0x17);
		edid_write(state, 0x11, 0x16);
		edid_write(state, 0x12, 0x01);
		edid_write(state, 0x13, 0x03);
		edid_write(state, 0x14, 0x80);
		edid_write(state, 0x15, 0x30);
		edid_write(state, 0x16, 0x1B);
		edid_write(state, 0x17, 0x78);
		edid_write(state, 0x18, 0x2A);
		edid_write(state, 0x19, 0x0A);
		edid_write(state, 0x1A, 0x45);
		edid_write(state, 0x1B, 0xA7);
		edid_write(state, 0x1C, 0x54);
		edid_write(state, 0x1D, 0x4A);
		edid_write(state, 0x1E, 0x99);
		edid_write(state, 0x1F, 0x25);
		edid_write(state, 0x20, 0x11);
		edid_write(state, 0x21, 0x50);
		edid_write(state, 0x22, 0x54);
		edid_write(state, 0x23, 0xBD);
		edid_write(state, 0x24, 0x4B);
		edid_write(state, 0x25, 0x00);
		edid_write(state, 0x26, 0xD1);
		edid_write(state, 0x27, 0xC0);
		edid_write(state, 0x28, 0x81);
		edid_write(state, 0x29, 0x80);
		edid_write(state, 0x2A, 0x95);
		edid_write(state, 0x2B, 0x0F);
		edid_write(state, 0x2C, 0x95);
		edid_write(state, 0x2D, 0x00);
		edid_write(state, 0x2E, 0xB3);
		edid_write(state, 0x2F, 0x00);
		edid_write(state, 0x30, 0x01);
		edid_write(state, 0x31, 0x01);
		edid_write(state, 0x32, 0x01);
		edid_write(state, 0x33, 0x01);
		edid_write(state, 0x34, 0x01);
		edid_write(state, 0x35, 0x01);
		edid_write(state, 0x36, 0x02);
		edid_write(state, 0x37, 0x3A);
		edid_write(state, 0x38, 0x80);
		edid_write(state, 0x39, 0x18);
		edid_write(state, 0x3A, 0x71);
		edid_write(state, 0x3B, 0x38);
		edid_write(state, 0x3C, 0x2D);
		edid_write(state, 0x3D, 0x40);
		edid_write(state, 0x3E, 0x58);
		edid_write(state, 0x3F, 0x2C);
		edid_write(state, 0x40, 0x45);
		edid_write(state, 0x41, 0x00);
		edid_write(state, 0x42, 0xE0);
		edid_write(state, 0x43, 0x0C);
		edid_write(state, 0x44, 0x11);
		edid_write(state, 0x45, 0x00);
		edid_write(state, 0x46, 0x00);
		edid_write(state, 0x47, 0x1E);
		edid_write(state, 0x48, 0x00);
		edid_write(state, 0x49, 0x00);
		edid_write(state, 0x4A, 0x00);
		edid_write(state, 0x4B, 0xFF);
		edid_write(state, 0x4C, 0x00);
		edid_write(state, 0x4D, 0x41);
		edid_write(state, 0x4E, 0x55);
		edid_write(state, 0x4F, 0x33);
		edid_write(state, 0x50, 0x31);
		edid_write(state, 0x51, 0x32);
		edid_write(state, 0x52, 0x32);
		edid_write(state, 0x53, 0x33);
		edid_write(state, 0x54, 0x30);
		edid_write(state, 0x55, 0x30);
		edid_write(state, 0x56, 0x31);
		edid_write(state, 0x57, 0x36);
		edid_write(state, 0x58, 0x33);
		edid_write(state, 0x59, 0x36);
		edid_write(state, 0x5A, 0x00);
		edid_write(state, 0x5B, 0x00);
		edid_write(state, 0x5C, 0x00);
		edid_write(state, 0x5D, 0xFC);
		edid_write(state, 0x5E, 0x00);
		edid_write(state, 0x5F, 0x48);
		edid_write(state, 0x60, 0x44);
		edid_write(state, 0x61, 0x4D);
		edid_write(state, 0x62, 0x49);
		edid_write(state, 0x63, 0x2D);
		edid_write(state, 0x64, 0x53);
		edid_write(state, 0x65, 0x57);
		edid_write(state, 0x66, 0x49);
		edid_write(state, 0x67, 0x54);
		edid_write(state, 0x68, 0x43);
		edid_write(state, 0x69, 0x48);
		edid_write(state, 0x6A, 0x0A);
		edid_write(state, 0x6B, 0x20);
		edid_write(state, 0x6C, 0x00);
		edid_write(state, 0x6D, 0x00);
		edid_write(state, 0x6E, 0x00);
		edid_write(state, 0x6F, 0xFD);
		edid_write(state, 0x70, 0x00);
		edid_write(state, 0x71, 0x38);
		edid_write(state, 0x72, 0x4C);
		edid_write(state, 0x73, 0x1E);
		edid_write(state, 0x74, 0x53);
		edid_write(state, 0x75, 0x11);
		edid_write(state, 0x76, 0x00);
		edid_write(state, 0x77, 0x0A);
		edid_write(state, 0x78, 0x20);
		edid_write(state, 0x79, 0x20);
		edid_write(state, 0x7A, 0x20);
		edid_write(state, 0x7B, 0x20);
		edid_write(state, 0x7C, 0x20);
		edid_write(state, 0x7D, 0x20);
		edid_write(state, 0x7E, 0x01);
		edid_write(state, 0x7F, 0x4D);
		edid_write(state, 0x80, 0x02);
		edid_write(state, 0x81, 0x03);
		edid_write(state, 0x82, 0x22);
		edid_write(state, 0x83, 0xF1);
		edid_write(state, 0x84, 0x4F);
		edid_write(state, 0x85, 0x01);
		edid_write(state, 0x86, 0x02);
		edid_write(state, 0x87, 0x03);
		edid_write(state, 0x88, 0x05);
		edid_write(state, 0x89, 0x06);
		edid_write(state, 0x8A, 0x07);
		edid_write(state, 0x8B, 0x10);
		edid_write(state, 0x8C, 0x11);
		edid_write(state, 0x8D, 0x12);
		edid_write(state, 0x8E, 0x13);
		edid_write(state, 0x8F, 0x14);
		edid_write(state, 0x90, 0x15);
		edid_write(state, 0x91, 0x16);
		edid_write(state, 0x92, 0x1F);
		edid_write(state, 0x93, 0x04);
		edid_write(state, 0x94, 0x23);
		edid_write(state, 0x95, 0x09);
		edid_write(state, 0x96, 0x17);
		edid_write(state, 0x97, 0x07);
		edid_write(state, 0x98, 0x83);
		edid_write(state, 0x99, 0x01);
		edid_write(state, 0x9A, 0x00);
		edid_write(state, 0x9B, 0x00);
		edid_write(state, 0x9C, 0x65);
		edid_write(state, 0x9D, 0x03);
		edid_write(state, 0x9E, 0x0C);
		edid_write(state, 0x9F, 0x00);
		edid_write(state, 0xA0, 0x10);
		edid_write(state, 0xA1, 0x00);
		edid_write(state, 0xA2, 0x02);
		edid_write(state, 0xA3, 0x3A);
		edid_write(state, 0xA4, 0x80);
		edid_write(state, 0xA5, 0x18);
		edid_write(state, 0xA6, 0x71);
		edid_write(state, 0xA7, 0x38);
		edid_write(state, 0xA8, 0x2D);
		edid_write(state, 0xA9, 0x40);
		edid_write(state, 0xAA, 0x58);
		edid_write(state, 0xAB, 0x2C);
		edid_write(state, 0xAC, 0x45);
		edid_write(state, 0xAD, 0x00);
		edid_write(state, 0xAE, 0xE0);
		edid_write(state, 0xAF, 0x0C);
		edid_write(state, 0xB0, 0x11);
		edid_write(state, 0xB1, 0x00);
		edid_write(state, 0xB2, 0x00);
		edid_write(state, 0xB3, 0x1E);
		edid_write(state, 0xB4, 0x8C);
		edid_write(state, 0xB5, 0x0A);
		edid_write(state, 0xB6, 0xD0);
		edid_write(state, 0xB7, 0x8A);
		edid_write(state, 0xB8, 0x20);
		edid_write(state, 0xB9, 0xE0);
		edid_write(state, 0xBA, 0x2D);
		edid_write(state, 0xBB, 0x10);
		edid_write(state, 0xBC, 0x10);
		edid_write(state, 0xBD, 0x3E);
		edid_write(state, 0xBE, 0x96);
		edid_write(state, 0xBF, 0x00);
		edid_write(state, 0xC0, 0xE0);
		edid_write(state, 0xC1, 0x0C);
		edid_write(state, 0xC2, 0x11);
		edid_write(state, 0xC3, 0x00);
		edid_write(state, 0xC4, 0x00);
		edid_write(state, 0xC5, 0x18);
		edid_write(state, 0xC6, 0x01);
		edid_write(state, 0xC7, 0x1D);
		edid_write(state, 0xC8, 0x00);
		edid_write(state, 0xC9, 0x72);
		edid_write(state, 0xCA, 0x51);
		edid_write(state, 0xCB, 0xD0);
		edid_write(state, 0xCC, 0x1E);
		edid_write(state, 0xCD, 0x20);
		edid_write(state, 0xCE, 0x6E);
		edid_write(state, 0xCF, 0x28);
		edid_write(state, 0xD0, 0x55);
		edid_write(state, 0xD1, 0x00);
		edid_write(state, 0xD2, 0xE0);
		edid_write(state, 0xD3, 0x0C);
		edid_write(state, 0xD4, 0x11);
		edid_write(state, 0xD5, 0x00);
		edid_write(state, 0xD6, 0x00);
		edid_write(state, 0xD7, 0x1E);
		edid_write(state, 0xD8, 0x8C);
		edid_write(state, 0xD9, 0x0A);
		edid_write(state, 0xDA, 0xD0);
		edid_write(state, 0xDB, 0x90);
		edid_write(state, 0xDC, 0x20);
		edid_write(state, 0xDD, 0x40);
		edid_write(state, 0xDE, 0x31);
		edid_write(state, 0xDF, 0x20);
		edid_write(state, 0xE0, 0x0C);
		edid_write(state, 0xE1, 0x40);
		edid_write(state, 0xE2, 0x55);
		edid_write(state, 0xE3, 0x00);
		edid_write(state, 0xE4, 0xE0);
		edid_write(state, 0xE5, 0x0C);
		edid_write(state, 0xE6, 0x11);
		edid_write(state, 0xE7, 0x00);
		edid_write(state, 0xE8, 0x00);
		edid_write(state, 0xE9, 0x18);
		edid_write(state, 0xEA, 0x00);
		edid_write(state, 0xEB, 0x00);
		edid_write(state, 0xEC, 0x00);
		edid_write(state, 0xED, 0x00);
		edid_write(state, 0xEE, 0x00);
		edid_write(state, 0xEF, 0x00);
		edid_write(state, 0xF0, 0x00);
		edid_write(state, 0xF1, 0x00);
		edid_write(state, 0xF2, 0x00);
		edid_write(state, 0xF3, 0x00);
		edid_write(state, 0xF4, 0x00);
		edid_write(state, 0xF5, 0x00);
		edid_write(state, 0xF6, 0x00);
		edid_write(state, 0xF7, 0x00);
		edid_write(state, 0xF8, 0x00);
		edid_write(state, 0xF9, 0x00);
		edid_write(state, 0xFA, 0x00);
		edid_write(state, 0xFB, 0x00);
		edid_write(state, 0xFC, 0x00);
		edid_write(state, 0xFD, 0x00);
		edid_write(state, 0xFE, 0x00);
		edid_write(state, 0xFF, 0xAA);
	}
	else if(EdidMode == 1)
	{
		edid_write(state, 0x00, 0x00);
		edid_write(state, 0x01, 0xFF);
		edid_write(state, 0x02, 0xFF);
		edid_write(state, 0x03, 0xFF);
		edid_write(state, 0x04, 0xFF);
		edid_write(state, 0x05, 0xFF);
		edid_write(state, 0x06, 0xFF);
		edid_write(state, 0x07, 0x00);
		edid_write(state, 0x08, 0x15);
		edid_write(state, 0x09, 0xC3);
		edid_write(state, 0x0A, 0x59);
		edid_write(state, 0x0B, 0x17);
		edid_write(state, 0x0C, 0x01);
		edid_write(state, 0x0D, 0x01);
		edid_write(state, 0x0E, 0x01);
		edid_write(state, 0x0F, 0x01);
		edid_write(state, 0x10, 0x20);
		edid_write(state, 0x11, 0x0E);
		edid_write(state, 0x12, 0x01);
		edid_write(state, 0x13, 0x03);
		edid_write(state, 0x14, 0x80);
		edid_write(state, 0x15, 0x29);
		edid_write(state, 0x16, 0x1F);
		edid_write(state, 0x17, 0x78);
		edid_write(state, 0x18, 0xEA);
		edid_write(state, 0x19, 0xD6);
		edid_write(state, 0x1A, 0xF5);
		edid_write(state, 0x1B, 0xA4);
		edid_write(state, 0x1C, 0x56);
		edid_write(state, 0x1D, 0x4B);
		edid_write(state, 0x1E, 0x9A);
		edid_write(state, 0x1F, 0x24);
		edid_write(state, 0x20, 0x17);
		edid_write(state, 0x21, 0x50);
		edid_write(state, 0x22, 0x54);
		edid_write(state, 0x23, 0xA1);
		edid_write(state, 0x24, 0x08);
		edid_write(state, 0x25, 0x00);
		edid_write(state, 0x26, 0xA9);
		edid_write(state, 0x27, 0x40);
		edid_write(state, 0x28, 0x81);
		edid_write(state, 0x29, 0x80);
		edid_write(state, 0x2A, 0x01);
		edid_write(state, 0x2B, 0x01);
		edid_write(state, 0x2C, 0x01);
		edid_write(state, 0x2D, 0x01);
		edid_write(state, 0x2E, 0x01);
		edid_write(state, 0x2F, 0x01);
		edid_write(state, 0x30, 0x01);
		edid_write(state, 0x31, 0x01);
		edid_write(state, 0x32, 0x01);
		edid_write(state, 0x33, 0x01);
		edid_write(state, 0x34, 0x01);
		edid_write(state, 0x35, 0x01);
		edid_write(state, 0x36, 0x48);
		edid_write(state, 0x37, 0x3F);
		edid_write(state, 0x38, 0x40);
		edid_write(state, 0x39, 0x30);
		edid_write(state, 0x3A, 0x62);
		edid_write(state, 0x3B, 0xB0);
		edid_write(state, 0x3C, 0x32);
		edid_write(state, 0x3D, 0x40);
		edid_write(state, 0x3E, 0x40);
		edid_write(state, 0x3F, 0xC0);
		edid_write(state, 0x40, 0x13);
		edid_write(state, 0x41, 0x00);
		edid_write(state, 0x42, 0x98);
		edid_write(state, 0x43, 0x32);
		edid_write(state, 0x44, 0x11);
		edid_write(state, 0x45, 0x00);
		edid_write(state, 0x46, 0x00);
		edid_write(state, 0x47, 0x1E);
		edid_write(state, 0x48, 0x00);
		edid_write(state, 0x49, 0x00);
		edid_write(state, 0x4A, 0x00);
		edid_write(state, 0x4B, 0xFF);
		edid_write(state, 0x4C, 0x00);
		edid_write(state, 0x4D, 0x31);
		edid_write(state, 0x4E, 0x0A);
		edid_write(state, 0x4F, 0x20);
		edid_write(state, 0x50, 0x20);
		edid_write(state, 0x51, 0x20);
		edid_write(state, 0x52, 0x20);
		edid_write(state, 0x53, 0x20);
		edid_write(state, 0x54, 0x20);
		edid_write(state, 0x55, 0x20);
		edid_write(state, 0x56, 0x20);
		edid_write(state, 0x57, 0x20);
		edid_write(state, 0x58, 0x20);
		edid_write(state, 0x59, 0x20);
		edid_write(state, 0x5A, 0x00);
		edid_write(state, 0x5B, 0x00);
		edid_write(state, 0x5C, 0x00);
		edid_write(state, 0x5D, 0xFD);
		edid_write(state, 0x5E, 0x00);
		edid_write(state, 0x5F, 0x3B);
		edid_write(state, 0x60, 0x3D);
		edid_write(state, 0x61, 0x1F);
		edid_write(state, 0x62, 0x4C);
		edid_write(state, 0x63, 0x11);
		edid_write(state, 0x64, 0x00);
		edid_write(state, 0x65, 0x0A);
		edid_write(state, 0x66, 0x20);
		edid_write(state, 0x67, 0x20);
		edid_write(state, 0x68, 0x20);
		edid_write(state, 0x69, 0x20);
		edid_write(state, 0x6A, 0x20);
		edid_write(state, 0x6B, 0x20);
		edid_write(state, 0x6C, 0x00);
		edid_write(state, 0x6D, 0x00);
		edid_write(state, 0x6E, 0x00);
		edid_write(state, 0x6F, 0xFC);
		edid_write(state, 0x70, 0x00);
		edid_write(state, 0x71, 0x4C);
		edid_write(state, 0x72, 0x38);
		edid_write(state, 0x73, 0x38);
		edid_write(state, 0x74, 0x37);
		edid_write(state, 0x75, 0x0A);
		edid_write(state, 0x76, 0x20);
		edid_write(state, 0x77, 0x20);
		edid_write(state, 0x78, 0x20);
		edid_write(state, 0x79, 0x20);
		edid_write(state, 0x7A, 0x20);
		edid_write(state, 0x7B, 0x20);
		edid_write(state, 0x7C, 0x20);
		edid_write(state, 0x7D, 0x20);
		edid_write(state, 0x7E, 0x00);
		edid_write(state, 0x7F, 0x61);
		edid_write(state, 0x80, 0x00);
		edid_write(state, 0x81, 0x00);
		edid_write(state, 0x82, 0x00);
		edid_write(state, 0x83, 0x00);
		edid_write(state, 0x84, 0x00);
		edid_write(state, 0x85, 0x00);
		edid_write(state, 0x86, 0x00);
		edid_write(state, 0x87, 0x00);
		edid_write(state, 0x88, 0x00);
		edid_write(state, 0x89, 0x00);
		edid_write(state, 0x8A, 0x00);
		edid_write(state, 0x8B, 0x00);
		edid_write(state, 0x8C, 0x00);
		edid_write(state, 0x8D, 0x00);
		edid_write(state, 0x8E, 0x00);
		edid_write(state, 0x8F, 0x00);
		edid_write(state, 0x90, 0x00);
		edid_write(state, 0x91, 0x00);
		edid_write(state, 0x92, 0x00);
		edid_write(state, 0x93, 0x00);
		edid_write(state, 0x94, 0x00);
		edid_write(state, 0x95, 0x00);
		edid_write(state, 0x96, 0x00);
		edid_write(state, 0x97, 0x00);
		edid_write(state, 0x98, 0x00);
		edid_write(state, 0x99, 0x00);
		edid_write(state, 0x9A, 0x00);
		edid_write(state, 0x9B, 0x00);
		edid_write(state, 0x9C, 0x00);
		edid_write(state, 0x9D, 0x00);
		edid_write(state, 0x9E, 0x00);
		edid_write(state, 0x9F, 0x00);
		edid_write(state, 0xA0, 0x00);
		edid_write(state, 0xA1, 0x00);
		edid_write(state, 0xA2, 0x00);
		edid_write(state, 0xA3, 0x00);
		edid_write(state, 0xA4, 0x00);
		edid_write(state, 0xA5, 0x00);
		edid_write(state, 0xA6, 0x00);
		edid_write(state, 0xA7, 0x00);
		edid_write(state, 0xA8, 0x00);
		edid_write(state, 0xA9, 0x00);
		edid_write(state, 0xAA, 0x00);
		edid_write(state, 0xAB, 0x00);
		edid_write(state, 0xAC, 0x00);
		edid_write(state, 0xAD, 0x00);
		edid_write(state, 0xAE, 0x00);
		edid_write(state, 0xAF, 0x00);
		edid_write(state, 0xB0, 0x00);
		edid_write(state, 0xB1, 0x00);
		edid_write(state, 0xB2, 0x00);
		edid_write(state, 0xB3, 0x00);
		edid_write(state, 0xB4, 0x00);
		edid_write(state, 0xB5, 0x00);
		edid_write(state, 0xB6, 0x00);
		edid_write(state, 0xB7, 0x00);
		edid_write(state, 0xB8, 0x00);
		edid_write(state, 0xB9, 0x00);
		edid_write(state, 0xBA, 0x00);
		edid_write(state, 0xBB, 0x00);
		edid_write(state, 0xBC, 0x00);
		edid_write(state, 0xBD, 0x00);
		edid_write(state, 0xBE, 0x00);
		edid_write(state, 0xBF, 0x00);
		edid_write(state, 0xC0, 0x00);
		edid_write(state, 0xC1, 0x00);
		edid_write(state, 0xC2, 0x00);
		edid_write(state, 0xC3, 0x00);
		edid_write(state, 0xC4, 0x00);
		edid_write(state, 0xC5, 0x00);
		edid_write(state, 0xC6, 0x00);
		edid_write(state, 0xC7, 0x00);
		edid_write(state, 0xC8, 0x00);
		edid_write(state, 0xC9, 0x00);
		edid_write(state, 0xCA, 0x00);
		edid_write(state, 0xCB, 0x00);
		edid_write(state, 0xCC, 0x00);
		edid_write(state, 0xCD, 0x00);
		edid_write(state, 0xCE, 0x00);
		edid_write(state, 0xCF, 0x00);
		edid_write(state, 0xD0, 0x00);
		edid_write(state, 0xD1, 0x00);
		edid_write(state, 0xD2, 0x00);
		edid_write(state, 0xD3, 0x00);
		edid_write(state, 0xD4, 0x00);
		edid_write(state, 0xD5, 0x00);
		edid_write(state, 0xD6, 0x00);
		edid_write(state, 0xD7, 0x00);
		edid_write(state, 0xD8, 0x00);
		edid_write(state, 0xD9, 0x00);
		edid_write(state, 0xDA, 0x00);
		edid_write(state, 0xDB, 0x00);
		edid_write(state, 0xDC, 0x00);
		edid_write(state, 0xDD, 0x00);
		edid_write(state, 0xDE, 0x00);
		edid_write(state, 0xDF, 0x00);
		edid_write(state, 0xE0, 0x00);
		edid_write(state, 0xE1, 0x00);
		edid_write(state, 0xE2, 0x00);
		edid_write(state, 0xE3, 0x00);
		edid_write(state, 0xE4, 0x00);
		edid_write(state, 0xE5, 0x00);
		edid_write(state, 0xE6, 0x00);
		edid_write(state, 0xE7, 0x00);
		edid_write(state, 0xE8, 0x00);
		edid_write(state, 0xE9, 0x00);
		edid_write(state, 0xEA, 0x00);
		edid_write(state, 0xEB, 0x00);
		edid_write(state, 0xEC, 0x00);
		edid_write(state, 0xED, 0x00);
		edid_write(state, 0xEE, 0x00);
		edid_write(state, 0xEF, 0x00);
		edid_write(state, 0xF0, 0x00);
		edid_write(state, 0xF1, 0x00);
		edid_write(state, 0xF2, 0x00);
		edid_write(state, 0xF3, 0x00);
		edid_write(state, 0xF4, 0x00);
		edid_write(state, 0xF5, 0x00);
		edid_write(state, 0xF6, 0x00);
		edid_write(state, 0xF7, 0x00);
		edid_write(state, 0xF8, 0x00);
		edid_write(state, 0xF9, 0x00);
		edid_write(state, 0xFA, 0x00);
		edid_write(state, 0xFB, 0x00);
		edid_write(state, 0xFC, 0x00);
		edid_write(state, 0xFD, 0x00);
		edid_write(state, 0xFE, 0x00);
		edid_write(state, 0xFF, 0x00);
	}
	else if(EdidMode == 2)
	{
		edid_write(state, 0x00, 0x00);
		edid_write(state, 0x01, 0xFF);
		edid_write(state, 0x02, 0xFF);
		edid_write(state, 0x03, 0xFF);
		edid_write(state, 0x04, 0xFF);
		edid_write(state, 0x05, 0xFF);
		edid_write(state, 0x06, 0xFF);
		edid_write(state, 0x07, 0x00);
		edid_write(state, 0x08, 0x41);
		edid_write(state, 0x09, 0x0C);
		edid_write(state, 0x0A, 0x7D);
		edid_write(state, 0x0B, 0x08);
		edid_write(state, 0x0C, 0x60);
		edid_write(state, 0x0D, 0xD2);
		edid_write(state, 0x0E, 0x0A);
		edid_write(state, 0x0F, 0x00);
		edid_write(state, 0x10, 0x14);
		edid_write(state, 0x11, 0x15);
		edid_write(state, 0x12, 0x01);
		edid_write(state, 0x13, 0x03);
		edid_write(state, 0x14, 0x80);
		edid_write(state, 0x15, 0x34);
		edid_write(state, 0x16, 0x20);
		edid_write(state, 0x17, 0x78);
		edid_write(state, 0x18, 0xEE);
		edid_write(state, 0x19, 0x9F);
		edid_write(state, 0x1A, 0xF5);
		edid_write(state, 0x1B, 0xA6);
		edid_write(state, 0x1C, 0x56);
		edid_write(state, 0x1D, 0x4B);
		edid_write(state, 0x1E, 0x9A);
		edid_write(state, 0x1F, 0x25);
		edid_write(state, 0x20, 0x12);
		edid_write(state, 0x21, 0x50);
		edid_write(state, 0x22, 0x54);
		edid_write(state, 0x23, 0xBF);
		edid_write(state, 0x24, 0xEF);
		edid_write(state, 0x25, 0x80);
		edid_write(state, 0x26, 0x71);
		edid_write(state, 0x27, 0x40);
		edid_write(state, 0x28, 0x81);
		edid_write(state, 0x29, 0x40);
		edid_write(state, 0x2A, 0x81);
		edid_write(state, 0x2B, 0xC0);
		edid_write(state, 0x2C, 0x95);
		edid_write(state, 0x2D, 0x00);
		edid_write(state, 0x2E, 0x95);
		edid_write(state, 0x2F, 0x0F);
		edid_write(state, 0x30, 0xA9);
		edid_write(state, 0x31, 0x40);
		edid_write(state, 0x32, 0xB3);
		edid_write(state, 0x33, 0x00);
		edid_write(state, 0x34, 0x01);
		edid_write(state, 0x35, 0x01);
		edid_write(state, 0x36, 0x28);
		edid_write(state, 0x37, 0x3C);
		edid_write(state, 0x38, 0x80);
		edid_write(state, 0x39, 0xA0);
		edid_write(state, 0x3A, 0x70);
		edid_write(state, 0x3B, 0xB0);
		edid_write(state, 0x3C, 0x23);
		edid_write(state, 0x3D, 0x40);
		edid_write(state, 0x3E, 0x30);
		edid_write(state, 0x3F, 0x20);
		edid_write(state, 0x40, 0x36);
		edid_write(state, 0x41, 0x00);
		edid_write(state, 0x42, 0x07);
		edid_write(state, 0x43, 0x44);
		edid_write(state, 0x44, 0x21);
		edid_write(state, 0x45, 0x00);
		edid_write(state, 0x46, 0x00);
		edid_write(state, 0x47, 0x1A);
		edid_write(state, 0x48, 0x00);
		edid_write(state, 0x49, 0x00);
		edid_write(state, 0x4A, 0x00);
		edid_write(state, 0x4B, 0xFF);
		edid_write(state, 0x4C, 0x00);
		edid_write(state, 0x4D, 0x44);
		edid_write(state, 0x4E, 0x4C);
		edid_write(state, 0x4F, 0x34);
		edid_write(state, 0x50, 0x31);
		edid_write(state, 0x51, 0x31);
		edid_write(state, 0x52, 0x32);
		edid_write(state, 0x53, 0x30);
		edid_write(state, 0x54, 0x37);
		edid_write(state, 0x55, 0x30);
		edid_write(state, 0x56, 0x39);
		edid_write(state, 0x57, 0x32);
		edid_write(state, 0x58, 0x31);
		edid_write(state, 0x59, 0x36);
		edid_write(state, 0x5A, 0x00);
		edid_write(state, 0x5B, 0x00);
		edid_write(state, 0x5C, 0x00);
		edid_write(state, 0x5D, 0xFC);
		edid_write(state, 0x5E, 0x00);
		edid_write(state, 0x5F, 0x50);
		edid_write(state, 0x60, 0x68);
		edid_write(state, 0x61, 0x69);
		edid_write(state, 0x62, 0x6C);
		edid_write(state, 0x63, 0x69);
		edid_write(state, 0x64, 0x70);
		edid_write(state, 0x65, 0x73);
		edid_write(state, 0x66, 0x20);
		edid_write(state, 0x67, 0x32);
		edid_write(state, 0x68, 0x34);
		edid_write(state, 0x69, 0x30);
		edid_write(state, 0x6A, 0x53);
		edid_write(state, 0x6B, 0x0A);
		edid_write(state, 0x6C, 0x00);
		edid_write(state, 0x6D, 0x00);
		edid_write(state, 0x6E, 0x00);
		edid_write(state, 0x6F, 0xFD);
		edid_write(state, 0x70, 0x00);
		edid_write(state, 0x71, 0x30);
		edid_write(state, 0x72, 0x55);
		edid_write(state, 0x73, 0x18);
		edid_write(state, 0x74, 0x5E);
		edid_write(state, 0x75, 0x11);
		edid_write(state, 0x76, 0x00);
		edid_write(state, 0x77, 0x0A);
		edid_write(state, 0x78, 0x20);
		edid_write(state, 0x79, 0x20);
		edid_write(state, 0x7A, 0x20);
		edid_write(state, 0x7B, 0x20);
		edid_write(state, 0x7C, 0x20);
		edid_write(state, 0x7D, 0x20);
		edid_write(state, 0x7E, 0x00);
		edid_write(state, 0x7F, 0xFB);
		edid_write(state, 0x80, 0x00);
		edid_write(state, 0x81, 0x00);
		edid_write(state, 0x82, 0x00);
		edid_write(state, 0x83, 0x00);
		edid_write(state, 0x84, 0x00);
		edid_write(state, 0x85, 0x00);
		edid_write(state, 0x86, 0x00);
		edid_write(state, 0x87, 0x00);
		edid_write(state, 0x88, 0x00);
		edid_write(state, 0x89, 0x00);
		edid_write(state, 0x8A, 0x00);
		edid_write(state, 0x8B, 0x00);
		edid_write(state, 0x8C, 0x00);
		edid_write(state, 0x8D, 0x00);
		edid_write(state, 0x8E, 0x00);
		edid_write(state, 0x8F, 0x00);
		edid_write(state, 0x90, 0x00);
		edid_write(state, 0x91, 0x00);
		edid_write(state, 0x92, 0x00);
		edid_write(state, 0x93, 0x00);
		edid_write(state, 0x94, 0x00);
		edid_write(state, 0x95, 0x00);
		edid_write(state, 0x96, 0x00);
		edid_write(state, 0x97, 0x00);
		edid_write(state, 0x98, 0x00);
		edid_write(state, 0x99, 0x00);
		edid_write(state, 0x9A, 0x00);
		edid_write(state, 0x9B, 0x00);
		edid_write(state, 0x9C, 0x00);
		edid_write(state, 0x9D, 0x00);
		edid_write(state, 0x9E, 0x00);
		edid_write(state, 0x9F, 0x00);
		edid_write(state, 0xA0, 0x00);
		edid_write(state, 0xA1, 0x00);
		edid_write(state, 0xA2, 0x00);
		edid_write(state, 0xA3, 0x00);
		edid_write(state, 0xA4, 0x00);
		edid_write(state, 0xA5, 0x00);
		edid_write(state, 0xA6, 0x00);
		edid_write(state, 0xA7, 0x00);
		edid_write(state, 0xA8, 0x00);
		edid_write(state, 0xA9, 0x00);
		edid_write(state, 0xAA, 0x00);
		edid_write(state, 0xAB, 0x00);
		edid_write(state, 0xAC, 0x00);
		edid_write(state, 0xAD, 0x00);
		edid_write(state, 0xAE, 0x00);
		edid_write(state, 0xAF, 0x00);
		edid_write(state, 0xB0, 0x00);
		edid_write(state, 0xB1, 0x00);
		edid_write(state, 0xB2, 0x00);
		edid_write(state, 0xB3, 0x00);
		edid_write(state, 0xB4, 0x00);
		edid_write(state, 0xB5, 0x00);
		edid_write(state, 0xB6, 0x00);
		edid_write(state, 0xB7, 0x00);
		edid_write(state, 0xB8, 0x00);
		edid_write(state, 0xB9, 0x00);
		edid_write(state, 0xBA, 0x00);
		edid_write(state, 0xBB, 0x00);
		edid_write(state, 0xBC, 0x00);
		edid_write(state, 0xBD, 0x00);
		edid_write(state, 0xBE, 0x00);
		edid_write(state, 0xBF, 0x00);
		edid_write(state, 0xC0, 0x00);
		edid_write(state, 0xC1, 0x00);
		edid_write(state, 0xC2, 0x00);
		edid_write(state, 0xC3, 0x00);
		edid_write(state, 0xC4, 0x00);
		edid_write(state, 0xC5, 0x00);
		edid_write(state, 0xC6, 0x00);
		edid_write(state, 0xC7, 0x00);
		edid_write(state, 0xC8, 0x00);
		edid_write(state, 0xC9, 0x00);
		edid_write(state, 0xCA, 0x00);
		edid_write(state, 0xCB, 0x00);
		edid_write(state, 0xCC, 0x00);
		edid_write(state, 0xCD, 0x00);
		edid_write(state, 0xCE, 0x00);
		edid_write(state, 0xCF, 0x00);
		edid_write(state, 0xD0, 0x00);
		edid_write(state, 0xD1, 0x00);
		edid_write(state, 0xD2, 0x00);
		edid_write(state, 0xD3, 0x00);
		edid_write(state, 0xD4, 0x00);
		edid_write(state, 0xD5, 0x00);
		edid_write(state, 0xD6, 0x00);
		edid_write(state, 0xD7, 0x00);
		edid_write(state, 0xD8, 0x00);
		edid_write(state, 0xD9, 0x00);
		edid_write(state, 0xDA, 0x00);
		edid_write(state, 0xDB, 0x00);
		edid_write(state, 0xDC, 0x00);
		edid_write(state, 0xDD, 0x00);
		edid_write(state, 0xDE, 0x00);
		edid_write(state, 0xDF, 0x00);
		edid_write(state, 0xE0, 0x00);
		edid_write(state, 0xE1, 0x00);
		edid_write(state, 0xE2, 0x00);
		edid_write(state, 0xE3, 0x00);
		edid_write(state, 0xE4, 0x00);
		edid_write(state, 0xE5, 0x00);
		edid_write(state, 0xE6, 0x00);
		edid_write(state, 0xE7, 0x00);
		edid_write(state, 0xE8, 0x00);
		edid_write(state, 0xE9, 0x00);
		edid_write(state, 0xEA, 0x00);
		edid_write(state, 0xEB, 0x00);
		edid_write(state, 0xEC, 0x00);
		edid_write(state, 0xED, 0x00);
		edid_write(state, 0xEE, 0x00);
		edid_write(state, 0xEF, 0x00);
		edid_write(state, 0xF0, 0x00);
		edid_write(state, 0xF1, 0x00);
		edid_write(state, 0xF2, 0x00);
		edid_write(state, 0xF3, 0x00);
		edid_write(state, 0xF4, 0x00);
		edid_write(state, 0xF5, 0x00);
		edid_write(state, 0xF6, 0x00);
		edid_write(state, 0xF7, 0x00);
		edid_write(state, 0xF8, 0x00);
		edid_write(state, 0xF9, 0x00);
		edid_write(state, 0xFA, 0x00);
		edid_write(state, 0xFB, 0x00);
		edid_write(state, 0xFC, 0x00);
		edid_write(state, 0xFD, 0x00);
		edid_write(state, 0xFE, 0x00);
		edid_write(state, 0xFF, 0x00);
	}
	else
	{                               
		edid_write(state, 0x00, 0x00);
		edid_write(state, 0x01, 0xFF);
		edid_write(state, 0x02, 0xFF);
		edid_write(state, 0x03, 0xFF);
		edid_write(state, 0x04, 0xFF);
		edid_write(state, 0x05, 0xFF);
		edid_write(state, 0x06, 0xFF);
		edid_write(state, 0x07, 0x00);
		edid_write(state, 0x08, 0x10);
		edid_write(state, 0x09, 0xAC);
		edid_write(state, 0x0A, 0x24);
		edid_write(state, 0x0B, 0xA0);
		edid_write(state, 0x0C, 0x53);
		edid_write(state, 0x0D, 0x52);
		edid_write(state, 0x0E, 0x4A);
		edid_write(state, 0x0F, 0x31);
		edid_write(state, 0x10, 0x01);
		edid_write(state, 0x11, 0x11);
		edid_write(state, 0x12, 0x01);
		edid_write(state, 0x13, 0x03);
		edid_write(state, 0x14, 0x68);
		edid_write(state, 0x15, 0x26);
		edid_write(state, 0x16, 0x1E);
		edid_write(state, 0x17, 0x78);
		edid_write(state, 0x18, 0xEE);
		edid_write(state, 0x19, 0xCA);
		edid_write(state, 0x1A, 0xF6);
		edid_write(state, 0x1B, 0xA3);
		edid_write(state, 0x1C, 0x57);
		edid_write(state, 0x1D, 0x47);
		edid_write(state, 0x1E, 0x9E);
		edid_write(state, 0x1F, 0x23);
		edid_write(state, 0x20, 0x11);
		edid_write(state, 0x21, 0x4F);
		edid_write(state, 0x22, 0x54);
		edid_write(state, 0x23, 0xA5);
		edid_write(state, 0x24, 0x4B);
		edid_write(state, 0x25, 0x00);
		edid_write(state, 0x26, 0x71);
		edid_write(state, 0x27, 0x4F);
		edid_write(state, 0x28, 0x81);
		edid_write(state, 0x29, 0x80);
		edid_write(state, 0x2A, 0x01);
		edid_write(state, 0x2B, 0x01);
		edid_write(state, 0x2C, 0x01);
		edid_write(state, 0x2D, 0x01);
		edid_write(state, 0x2E, 0x01);
		edid_write(state, 0x2F, 0x01);
		edid_write(state, 0x30, 0x01);
		edid_write(state, 0x31, 0x01);
		edid_write(state, 0x32, 0x01);
		edid_write(state, 0x33, 0x01);
		edid_write(state, 0x34, 0x01);
		edid_write(state, 0x35, 0x01);
		edid_write(state, 0x36, 0x30);
		edid_write(state, 0x37, 0x2A);
		edid_write(state, 0x38, 0x00);
		edid_write(state, 0x39, 0x98);
		edid_write(state, 0x3A, 0x51);
		edid_write(state, 0x3B, 0x00);
		edid_write(state, 0x3C, 0x2A);
		edid_write(state, 0x3D, 0x40);
		edid_write(state, 0x3E, 0x30);
		edid_write(state, 0x3F, 0x70);
		edid_write(state, 0x40, 0x13);
		edid_write(state, 0x41, 0x00);
		edid_write(state, 0x42, 0x7C);
		edid_write(state, 0x43, 0x31);
		edid_write(state, 0x44, 0x11);
		edid_write(state, 0x45, 0x00);
		edid_write(state, 0x46, 0x00);
		edid_write(state, 0x47, 0x1E);
		edid_write(state, 0x48, 0x00);
		edid_write(state, 0x49, 0x00);
		edid_write(state, 0x4A, 0x00);
		edid_write(state, 0x4B, 0xFD);
		edid_write(state, 0x4C, 0x00);
		edid_write(state, 0x4D, 0x38);
		edid_write(state, 0x4E, 0x4B);
		edid_write(state, 0x4F, 0x1F);
		edid_write(state, 0x50, 0x50);
		edid_write(state, 0x51, 0x0E);
		edid_write(state, 0x52, 0x00);
		edid_write(state, 0x53, 0x0A);
		edid_write(state, 0x54, 0x20);
		edid_write(state, 0x55, 0x20);
		edid_write(state, 0x56, 0x20);
		edid_write(state, 0x57, 0x20);
		edid_write(state, 0x58, 0x20);
		edid_write(state, 0x59, 0x20);
		edid_write(state, 0x5A, 0x00);
		edid_write(state, 0x5B, 0x00);
		edid_write(state, 0x5C, 0x00);
		edid_write(state, 0x5D, 0xFF);
		edid_write(state, 0x5E, 0x00);
		edid_write(state, 0x5F, 0x55);
		edid_write(state, 0x60, 0x48);
		edid_write(state, 0x61, 0x35);
		edid_write(state, 0x62, 0x37);
		edid_write(state, 0x63, 0x30);
		edid_write(state, 0x64, 0x37);
		edid_write(state, 0x65, 0x31);
		edid_write(state, 0x66, 0x32);
		edid_write(state, 0x67, 0x31);
		edid_write(state, 0x68, 0x4A);
		edid_write(state, 0x69, 0x52);
		edid_write(state, 0x6A, 0x53);
		edid_write(state, 0x6B, 0x0A);
		edid_write(state, 0x6C, 0x00);
		edid_write(state, 0x6D, 0x00);
		edid_write(state, 0x6E, 0x00);
		edid_write(state, 0x6F, 0xFC);
		edid_write(state, 0x70, 0x00);
		edid_write(state, 0x71, 0x44);
		edid_write(state, 0x72, 0x45);
		edid_write(state, 0x73, 0x4C);
		edid_write(state, 0x74, 0x4C);
		edid_write(state, 0x75, 0x20);
		edid_write(state, 0x76, 0x45);
		edid_write(state, 0x77, 0x31);
		edid_write(state, 0x78, 0x39);
		edid_write(state, 0x79, 0x37);
		edid_write(state, 0x7A, 0x46);
		edid_write(state, 0x7B, 0x50);
		edid_write(state, 0x7C, 0x0A);
		edid_write(state, 0x7D, 0x20);
		edid_write(state, 0x7E, 0x00);
		edid_write(state, 0x7F, 0x29);
		edid_write(state, 0x80, 0x00);
		edid_write(state, 0x81, 0x00);
		edid_write(state, 0x82, 0x00);
		edid_write(state, 0x83, 0x00);
		edid_write(state, 0x84, 0x00);
		edid_write(state, 0x85, 0x00);
		edid_write(state, 0x86, 0x00);
		edid_write(state, 0x87, 0x00);
		edid_write(state, 0x88, 0x00);
		edid_write(state, 0x89, 0x00);
		edid_write(state, 0x8A, 0x00);
		edid_write(state, 0x8B, 0x00);
		edid_write(state, 0x8C, 0x00);
		edid_write(state, 0x8D, 0x00);
		edid_write(state, 0x8E, 0x00);
		edid_write(state, 0x8F, 0x00);
		edid_write(state, 0x90, 0x00);
		edid_write(state, 0x91, 0x00);
		edid_write(state, 0x92, 0x00);
		edid_write(state, 0x93, 0x00);
		edid_write(state, 0x94, 0x00);
		edid_write(state, 0x95, 0x00);
		edid_write(state, 0x96, 0x00);
		edid_write(state, 0x97, 0x00);
		edid_write(state, 0x98, 0x00);
		edid_write(state, 0x99, 0x00);
		edid_write(state, 0x9A, 0x00);
		edid_write(state, 0x9B, 0x00);
		edid_write(state, 0x9C, 0x00);
		edid_write(state, 0x9D, 0x00);
		edid_write(state, 0x9E, 0x00);
		edid_write(state, 0x9F, 0x00);
		edid_write(state, 0xA0, 0x00);
		edid_write(state, 0xA1, 0x00);
		edid_write(state, 0xA2, 0x00);
		edid_write(state, 0xA3, 0x00);
		edid_write(state, 0xA4, 0x00);
		edid_write(state, 0xA5, 0x00);
		edid_write(state, 0xA6, 0x00);
		edid_write(state, 0xA7, 0x00);
		edid_write(state, 0xA8, 0x00);
		edid_write(state, 0xA9, 0x00);
		edid_write(state, 0xAA, 0x00);
		edid_write(state, 0xAB, 0x00);
		edid_write(state, 0xAC, 0x00);
		edid_write(state, 0xAD, 0x00);
		edid_write(state, 0xAE, 0x00);
		edid_write(state, 0xAF, 0x00);
		edid_write(state, 0xB0, 0x00);
		edid_write(state, 0xB1, 0x00);
		edid_write(state, 0xB2, 0x00);
		edid_write(state, 0xB3, 0x00);
		edid_write(state, 0xB4, 0x00);
		edid_write(state, 0xB5, 0x00);
		edid_write(state, 0xB6, 0x00);
		edid_write(state, 0xB7, 0x00);
		edid_write(state, 0xB8, 0x00);
		edid_write(state, 0xB9, 0x00);
		edid_write(state, 0xBA, 0x00);
		edid_write(state, 0xBB, 0x00);
		edid_write(state, 0xBC, 0x00);
		edid_write(state, 0xBD, 0x00);
		edid_write(state, 0xBE, 0x00);
		edid_write(state, 0xBF, 0x00);
		edid_write(state, 0xC0, 0x00);
		edid_write(state, 0xC1, 0x00);
		edid_write(state, 0xC2, 0x00);
		edid_write(state, 0xC3, 0x00);
		edid_write(state, 0xC4, 0x00);
		edid_write(state, 0xC5, 0x00);
		edid_write(state, 0xC6, 0x00);
		edid_write(state, 0xC7, 0x00);
		edid_write(state, 0xC8, 0x00);
		edid_write(state, 0xC9, 0x00);
		edid_write(state, 0xCA, 0x00);
		edid_write(state, 0xCB, 0x00);
		edid_write(state, 0xCC, 0x00);
		edid_write(state, 0xCD, 0x00);
		edid_write(state, 0xCE, 0x00);
		edid_write(state, 0xCF, 0x00);
		edid_write(state, 0xD0, 0x00);
		edid_write(state, 0xD1, 0x00);
		edid_write(state, 0xD2, 0x00);
		edid_write(state, 0xD3, 0x00);
		edid_write(state, 0xD4, 0x00);
		edid_write(state, 0xD5, 0x00);
		edid_write(state, 0xD6, 0x00);
		edid_write(state, 0xD7, 0x00);
		edid_write(state, 0xD8, 0x00);
		edid_write(state, 0xD9, 0x00);
		edid_write(state, 0xDA, 0x00);
		edid_write(state, 0xDB, 0x00);
		edid_write(state, 0xDC, 0x00);
		edid_write(state, 0xDD, 0x00);
		edid_write(state, 0xDE, 0x00);
		edid_write(state, 0xDF, 0x00);
		edid_write(state, 0xE0, 0x00);
		edid_write(state, 0xE1, 0x00);
		edid_write(state, 0xE2, 0x00);
		edid_write(state, 0xE3, 0x00);
		edid_write(state, 0xE4, 0x00);
		edid_write(state, 0xE5, 0x00);
		edid_write(state, 0xE6, 0x00);
		edid_write(state, 0xE7, 0x00);
		edid_write(state, 0xE8, 0x00);
		edid_write(state, 0xE9, 0x00);
		edid_write(state, 0xEA, 0x00);
		edid_write(state, 0xEB, 0x00);
		edid_write(state, 0xEC, 0x00);
		edid_write(state, 0xED, 0x00);
		edid_write(state, 0xEE, 0x00);
		edid_write(state, 0xEF, 0x00);
		edid_write(state, 0xF0, 0x00);
		edid_write(state, 0xF1, 0x00);
		edid_write(state, 0xF2, 0x00);
		edid_write(state, 0xF3, 0x00);
		edid_write(state, 0xF4, 0x00);
		edid_write(state, 0xF5, 0x00);
		edid_write(state, 0xF6, 0x00);
		edid_write(state, 0xF7, 0x00);
		edid_write(state, 0xF8, 0x00);
		edid_write(state, 0xF9, 0x00);
		edid_write(state, 0xFA, 0x00);
		edid_write(state, 0xFB, 0x00);
		edid_write(state, 0xFC, 0x00);
		edid_write(state, 0xFD, 0x00);
		edid_write(state, 0xFE, 0x00);
		edid_write(state, 0xFF, 0x00);
	}
}

/****************************************************************************/
/*																			*/
/* FUNCTION:    ADV7604_DummyCall(void)                                       	*/
/*																			*/
/* USAGE:       To get by the data overlay problem when function is not     */
/*				being called.												*/
/*																			*/
/****************************************************************************/
void ADV7604_DummyCall(void)
{
	GetChipRevision(NULL, NULL);
	hblanking(NULL);
	htotal(NULL);
	vblanking(NULL);
	vtotal(NULL);
	find_and_set_predefined_video_timings(NULL, 0, 0, 0);
	configure_predefined_video_timings(NULL, NULL);
	configure_custom_video_timings(NULL, NULL);
	adv7604_set_ctrl(NULL, 0, 0);
	stdi2dv_timings(NULL, NULL, NULL);
	read_stdi(NULL, NULL);
	adv7604_enum_dv_timings(NULL, NULL);
	adv7604_dv_timings_cap(NULL, NULL);
	adv7604_query_dv_timings(NULL, NULL);
	adv7604_set_dv_timings(NULL, NULL);
	adv7604_get_dv_timings(NULL, NULL);
	adv7604_isr(NULL, 0, NULL);
	no_power(NULL);
	no_signal_tmds(NULL);
	no_lock_tmds(NULL);
	is_hdmi(NULL);
	no_lock_sspd(NULL);
	no_lock_stdi(NULL);
	no_signal(NULL);
	no_lock_cp(NULL);
	io_read(NULL, 0);
	io_write(NULL, 0, 0);
	io_write_and_or(NULL, 0, 0, 0);
	avlink_read(NULL, 0);
	avlink_write(NULL, 0, 0);
	cec_read(NULL, 0);
	cec_write(NULL, 0, 0);
	cec_write_and_or(NULL, 0, 0, 0);
	infoframe_read(NULL, 0);
	infoframe_write(NULL, 0, 0);
	esdp_read(NULL, 0);
	esdp_write(NULL, 0, 0);
	dpp_read(NULL, 0);
	dpp_write(NULL, 0, 0);
	afe_read(NULL, 0);
	afe_write(NULL, 0, 0);
	rep_read(NULL, 0);
	rep_write(NULL, 0, 0);
	rep_write_and_or(NULL, 0, 0, 0);
	edid_read(NULL, 0);
	edid_write(NULL, 0, 0);
	hdmi_read(NULL, 0);
	hdmi_write(NULL, 0, 0);
	test_read(NULL, 0);
	test_write(NULL, 0, 0);
	cp_read(NULL, 0);
	cp_write(NULL, 0, 0);
	cp_write_and_or(NULL, 0, 0, 0);
	vdp_read(NULL, 0);
	vdp_write(NULL, 0, 0);
}
/********************************  END  *************************************/
