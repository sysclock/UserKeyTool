/*
 * adv7604 - Analog Devices ADV7604 video decoder driver
 *
 * Copyright 2012 Cisco Systems, Inc. and/or its affiliates. All rights reserved.
 *
 * This program is free software; you may redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
*/

#ifndef _ADV7604_
#define _ADV7604_
#include <stdint.h>                           /* Include standard types */
#include "global.h"

#include "v4l2-dv-timings.h"
#include "media-v4l2-dv-timings.h"

#define	ADV7604_SLAVE_ADDR 							0x40
#define	ADV7604_HDMI_SLAVE_ADDR 				0x68
#define	ADV7604_CP_SLAVE_ADDR 					0x44
#define	ADV7604_AFE_SLAVE_ADDR 					0x4C
#define	ADV7604_HDMI_SLAVE_ADDR 				0x68
#define	ADV7604_AVLINK_SLAVE_ADDR				0x84
#define	ADV7604_CEC_SLAVE_ADDR 					0x80
#define	ADV7604_INFOFRAME_SLAVE_ADDR		0x7C
#define	ADV7604_ESDP_SLAVE_ADDR					0x70
#define	ADV7604_DPP_SLAVE_ADDR					0x78
#define	ADV7604_KSV_SLAVE_ADDR					0x64					// KSV_SLAVE_ADDR(Repeater Map)
#define	ADV7604_EDID_SLAVE_ADDR					0x6C
#define	ADV7604_TEST_SLAVE_ADDR					0x60
#define	ADV7604_VDP_SLAVE_ADDR					0x48

/* Analog input muxing modes (AFE register 0x02, [2:0]) */
enum adv7604_ain_sel {
        ADV7604_AIN1_2_3_NC_SYNC_1_2 = 0,
        ADV7604_AIN4_5_6_NC_SYNC_2_1 = 1,
        ADV7604_AIN7_8_9_NC_SYNC_3_1 = 2,
        ADV7604_AIN10_11_12_NC_SYNC_4_1 = 3,
        ADV7604_AIN9_4_5_6_SYNC_2_1 = 4,
};

/* Bus rotation and reordering (IO register 0x04, [7:5]) */
enum adv7604_op_ch_sel {
        ADV7604_OP_CH_SEL_GBR = 0,
        ADV7604_OP_CH_SEL_GRB = 1,
        ADV7604_OP_CH_SEL_BGR = 2,
        ADV7604_OP_CH_SEL_RGB = 3,
        ADV7604_OP_CH_SEL_BRG = 4,
        ADV7604_OP_CH_SEL_RBG = 5,
};

/* Input Color Space (IO register 0x02, [7:4]) */
enum adv7604_inp_color_space {
        ADV7604_INP_COLOR_SPACE_LIM_RGB = 0,
        ADV7604_INP_COLOR_SPACE_FULL_RGB = 1,
        ADV7604_INP_COLOR_SPACE_LIM_YCbCr_601 = 2,
        ADV7604_INP_COLOR_SPACE_LIM_YCbCr_709 = 3,
        ADV7604_INP_COLOR_SPACE_XVYCC_601 = 4,
        ADV7604_INP_COLOR_SPACE_XVYCC_709 = 5,
        ADV7604_INP_COLOR_SPACE_FULL_YCbCr_601 = 6,
        ADV7604_INP_COLOR_SPACE_FULL_YCbCr_709 = 7,
        ADV7604_INP_COLOR_SPACE_AUTO = 0xf,
};

/* Select output format (IO register 0x03, [7:0]) */
enum adv7604_op_format_sel {
        ADV7604_OP_FORMAT_SEL_SDR_ITU656_8 = 0x00,
        ADV7604_OP_FORMAT_SEL_SDR_ITU656_10 = 0x01,
        ADV7604_OP_FORMAT_SEL_SDR_ITU656_12_MODE0 = 0x02,
        ADV7604_OP_FORMAT_SEL_SDR_ITU656_12_MODE1 = 0x06,
        ADV7604_OP_FORMAT_SEL_SDR_ITU656_12_MODE2 = 0x0a,
        ADV7604_OP_FORMAT_SEL_DDR_422_8 = 0x20,
        ADV7604_OP_FORMAT_SEL_DDR_422_10 = 0x21,
        ADV7604_OP_FORMAT_SEL_DDR_422_12_MODE0 = 0x22,
        ADV7604_OP_FORMAT_SEL_DDR_422_12_MODE1 = 0x23,
        ADV7604_OP_FORMAT_SEL_DDR_422_12_MODE2 = 0x24,
        ADV7604_OP_FORMAT_SEL_SDR_444_24 = 0x40,
        ADV7604_OP_FORMAT_SEL_SDR_444_30 = 0x41,
        ADV7604_OP_FORMAT_SEL_SDR_444_36_MODE0 = 0x42,
        ADV7604_OP_FORMAT_SEL_DDR_444_24 = 0x60,
        ADV7604_OP_FORMAT_SEL_DDR_444_30 = 0x61,
        ADV7604_OP_FORMAT_SEL_DDR_444_36 = 0x62,
        ADV7604_OP_FORMAT_SEL_SDR_ITU656_16 = 0x80,
        ADV7604_OP_FORMAT_SEL_SDR_ITU656_20 = 0x81,
        ADV7604_OP_FORMAT_SEL_SDR_ITU656_24_MODE0 = 0x82,
        ADV7604_OP_FORMAT_SEL_SDR_ITU656_24_MODE1 = 0x86,
        ADV7604_OP_FORMAT_SEL_SDR_ITU656_24_MODE2 = 0x8a,
};

/* Platform dependent definition */
struct adv7604_platform_data {
        /* connector - HDMI or DVI? */
        unsigned connector_hdmi:1;

        /* DIS_PWRDNB: 1 if the PWRDNB pin is unused and unconnected */
        unsigned disable_pwrdnb:1;

        /* DIS_CABLE_DET_RST: 1 if the 5V pins are unused and unconnected */
        unsigned disable_cable_det_rst:1;

        /* Analog input muxing mode */
        enum adv7604_ain_sel ain_sel;

        /* Bus rotation and reordering */
        enum adv7604_op_ch_sel op_ch_sel;

        /* Select output format */
        enum adv7604_op_format_sel op_format_sel;

        /* IO register 0x02 */
        unsigned alt_gamma:1;
        unsigned op_656_range:1;
        unsigned rgb_out:1;
        unsigned alt_data_sat:1;

        /* IO register 0x05 */
        unsigned blank_data:1;
        unsigned insert_av_codes:1;
        unsigned replicate_av_codes:1;
        unsigned invert_cbcr:1;

        /* IO register 0x30 */
        unsigned output_bus_lsb_to_msb:1;

        /* Free run */
        unsigned hdmi_free_run_mode;

        /* i2c addresses: 0 == use default */
        unsigned char i2c_avlink;
        unsigned char i2c_cec;
        unsigned char i2c_infoframe;
        unsigned char i2c_esdp;
        unsigned char i2c_dpp;
        unsigned char i2c_afe;
        unsigned char i2c_repeater;
        unsigned char i2c_edid;
        unsigned char i2c_hdmi;
        unsigned char i2c_test;
        unsigned char i2c_cp;
        unsigned char i2c_vdp;
        unsigned char i2c_busindex;
};

/*
 * Mode of operation.
 * This is used as the input argument of the s_routing video op.
 */
enum adv7604_mode {
        ADV7604_MODE_COMP,
        ADV7604_MODE_GR,
        ADV7604_MODE_HDMI,
};

#define V4L2_CID_ADV_RX_ANALOG_SAMPLING_PHASE   (V4L2_CID_DV_CLASS_BASE + 0x1000)
#define V4L2_CID_ADV_RX_FREE_RUN_COLOR_MANUAL   (V4L2_CID_DV_CLASS_BASE + 0x1001)
#define V4L2_CID_ADV_RX_FREE_RUN_COLOR          (V4L2_CID_DV_CLASS_BASE + 0x1002)

/* notify events */
#define ADV7604_HOTPLUG         1
#define ADV7604_FMT_CHANGE      2

struct adv7604_state {
        struct adv7604_platform_data pdata;
        enum adv7604_mode mode;
        struct v4l2_dv_timings timings;
//        struct v4l2_dv_timings prev_timings;
//        unsigned char edid[256];
//        unsigned edid_blocks;
        struct v4l2_fract aspect_ratio;
        unsigned long rgb_quantization_range;
        BOOL connector_hdmi;
        BOOL restart_stdi_once;
        unsigned long prev_input_status;
};


extern struct adv7604_state gSt_ADV7604State;
extern const struct v4l2_dv_timings adv7604_timings[];

extern unsigned char ADV7604_Init(struct adv7604_state *state);
extern void ADV7604_EnableInput(struct adv7604_state *state);
extern void ADV7604_DisableInput(struct adv7604_state *state);
extern int ADV7604_SetRouting(struct adv7604_state *state, unsigned long input, unsigned long output, unsigned long config);
extern int ADV7604_SetTimings(struct adv7604_state *state, unsigned long TimingsIndex);
extern void ADV7604_SetInternalEdid(struct adv7604_state *state, unsigned char EdidMode);
extern int adv7604_isr(struct adv7604_state *state, unsigned long status, unsigned char *handled);
extern int adv7604_log_status(struct adv7604_state *state);
extern void ADV7604_LoadEdid(struct adv7604_state *state, unsigned char EdidSource);
extern void ADV7604_WriteEdidToProm(struct adv7604_state *state, unsigned char EdidMode);
extern int ADV7604_GetSignalActiveWidthHeight(struct adv7604_state *state, long *pWidth, long *pHeight);
extern int adv7604_set_ctrl(struct adv7604_state *state, int CtrlId, unsigned long CtrlVal);
#endif
