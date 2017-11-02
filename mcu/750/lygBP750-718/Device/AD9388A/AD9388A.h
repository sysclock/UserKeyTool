/*
 * ad9388a - Analog Devices AD9388A video decoder driver
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

#ifndef _AD9388A_
#define _AD9388A_
#include <stdint.h>                           /* Include standard types */
#include "../../System Code/global.h"

#include "AD9388A_reg.h"
#include "../ADV7604Driver/v4l2-dv-timings.h"
#include "../ADV7604Driver/media-v4l2-dv-timings.h"

#define	AD9388A_SLAVE_ADDR 							0x40
#define	AD9388A_HDMI_SLAVE_ADDR 				0x68
#define	AD9388A_CP_SLAVE_ADDR 					0x44
#define	AD9388A_AFE_SLAVE_ADDR 					0x4C
#define	AD9388A_HDMI_SLAVE_ADDR 				0x68
#define	AD9388A_AVLINK_SLAVE_ADDR				0x84
#define	AD9388A_CEC_SLAVE_ADDR 					0x80
#define	AD9388A_INFOFRAME_SLAVE_ADDR		0x7C
#define	AD9388A_ESDP_SLAVE_ADDR					0x70
#define	AD9388A_DPP_SLAVE_ADDR					0x78
#define	AD9388A_KSV_SLAVE_ADDR					0x64					// KSV_SLAVE_ADDR(Repeater Map)
#define	AD9388A_EDID_SLAVE_ADDR					0x6C
#define	AD9388A_TEST_SLAVE_ADDR					0x60
#define	AD9388A_VDP_SLAVE_ADDR					0x48

/* Analog input muxing modes (AFE register 0x02, [2:0]) */
enum ad9388a_ain_sel {
        AD9388A_AIN1_2_3_NC_SYNC_1_2 = 0,
        AD9388A_AIN4_5_6_NC_SYNC_2_1 = 1,
        AD9388A_AIN7_8_9_NC_SYNC_3_1 = 2,
        AD9388A_AIN10_11_12_NC_SYNC_4_1 = 3,
        AD9388A_AIN9_4_5_6_SYNC_2_1 = 4,
};

/* Bus rotation and reordering (IO register 0x04, [7:5]) */
enum ad9388a_op_ch_sel {
        AD9388A_OP_CH_SEL_GBR = 0,
        AD9388A_OP_CH_SEL_GRB = 1,
        AD9388A_OP_CH_SEL_BGR = 2,
        AD9388A_OP_CH_SEL_RGB = 3,
        AD9388A_OP_CH_SEL_BRG = 4,
        AD9388A_OP_CH_SEL_RBG = 5,
};

/* Input Color Space (IO register 0x02, [7:4]) */
enum ad9388a_inp_color_space {
        AD9388A_INP_COLOR_SPACE_LIM_RGB = 0,
        AD9388A_INP_COLOR_SPACE_FULL_RGB = 1,
        AD9388A_INP_COLOR_SPACE_LIM_YCbCr_601 = 2,
        AD9388A_INP_COLOR_SPACE_LIM_YCbCr_709 = 3,
        AD9388A_INP_COLOR_SPACE_XVYCC_601 = 4,
        AD9388A_INP_COLOR_SPACE_XVYCC_709 = 5,
        AD9388A_INP_COLOR_SPACE_FULL_YCbCr_601 = 6,
        AD9388A_INP_COLOR_SPACE_FULL_YCbCr_709 = 7,
        AD9388A_INP_COLOR_SPACE_AUTO = 0xf,
};

/* Select output format (IO register 0x03, [7:0]) */
enum ad9388a_op_format_sel {
        AD9388A_OP_FORMAT_SEL_SDR_ITU656_8 = 0x00,
        AD9388A_OP_FORMAT_SEL_SDR_ITU656_10 = 0x01,
        AD9388A_OP_FORMAT_SEL_SDR_ITU656_12_MODE0 = 0x02,
        AD9388A_OP_FORMAT_SEL_SDR_ITU656_12_MODE1 = 0x06,
        AD9388A_OP_FORMAT_SEL_SDR_ITU656_12_MODE2 = 0x0a,
        AD9388A_OP_FORMAT_SEL_DDR_422_8 = 0x20,
        AD9388A_OP_FORMAT_SEL_DDR_422_10 = 0x21,
        AD9388A_OP_FORMAT_SEL_DDR_422_12_MODE0 = 0x22,
        AD9388A_OP_FORMAT_SEL_DDR_422_12_MODE1 = 0x23,
        AD9388A_OP_FORMAT_SEL_DDR_422_12_MODE2 = 0x24,
        AD9388A_OP_FORMAT_SEL_SDR_444_24 = 0x40,
        AD9388A_OP_FORMAT_SEL_SDR_444_30 = 0x41,
        AD9388A_OP_FORMAT_SEL_SDR_444_36_MODE0 = 0x42,
        AD9388A_OP_FORMAT_SEL_DDR_444_24 = 0x60,
        AD9388A_OP_FORMAT_SEL_DDR_444_30 = 0x61,
        AD9388A_OP_FORMAT_SEL_DDR_444_36 = 0x62,
        AD9388A_OP_FORMAT_SEL_SDR_ITU656_16 = 0x80,
        AD9388A_OP_FORMAT_SEL_SDR_ITU656_20 = 0x81,
        AD9388A_OP_FORMAT_SEL_SDR_ITU656_24_MODE0 = 0x82,
        AD9388A_OP_FORMAT_SEL_SDR_ITU656_24_MODE1 = 0x86,
        AD9388A_OP_FORMAT_SEL_SDR_ITU656_24_MODE2 = 0x8a,
};

/* Platform dependent definition */
struct ad9388a_platform_data {
        /* connector - HDMI or DVI? */
        unsigned connector_hdmi:1;

        /* DIS_PWRDNB: 1 if the PWRDNB pin is unused and unconnected */
        unsigned disable_pwrdnb:1;

        /* DIS_CABLE_DET_RST: 1 if the 5V pins are unused and unconnected */
        unsigned disable_cable_det_rst:1;

        /* Analog input muxing mode */
        enum ad9388a_ain_sel ain_sel;

        /* Bus rotation and reordering */
        enum ad9388a_op_ch_sel op_ch_sel;

        /* Select output format */
        enum ad9388a_op_format_sel op_format_sel;

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
};

/*
 * Mode of operation.
 * This is used as the input argument of the s_routing video op.
 */
enum ad9388a_mode {
        AD9388A_MODE_COMP,
        AD9388A_MODE_GR,
        AD9388A_MODE_HDMI,
};

#define V4L2_CID_ADV_RX_ANALOG_SAMPLING_PHASE   (V4L2_CID_DV_CLASS_BASE + 0x1000)
#define V4L2_CID_ADV_RX_FREE_RUN_COLOR_MANUAL   (V4L2_CID_DV_CLASS_BASE + 0x1001)
#define V4L2_CID_ADV_RX_FREE_RUN_COLOR          (V4L2_CID_DV_CLASS_BASE + 0x1002)

/* notify events */
#define AD9388A_HOTPLUG         1
#define AD9388A_FMT_CHANGE      2

struct ad9388a_state {
        struct ad9388a_platform_data pdata;
        enum ad9388a_mode mode;
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


extern struct ad9388a_state gSt_AD9388AState;
extern const struct v4l2_dv_timings ad9388a_timings[];

extern unsigned char AD9388A_Init(struct ad9388a_state *state);
extern void AD9388A_EnableInput(struct ad9388a_state *state);
extern void AD9388A_DisableInput(struct ad9388a_state *state);
extern int AD9388A_SetRouting(struct ad9388a_state *state, unsigned long input, unsigned long output, unsigned long config);
extern int AD9388A_SetTimings(struct ad9388a_state *state, unsigned long TimingsIndex);
extern void AD9388A_SetInternalEdid(struct ad9388a_state *state, unsigned char EdidMode);
extern int ad9388a_isr(struct ad9388a_state *state, unsigned long status, unsigned char *handled);
extern int ad9388a_log_status(struct ad9388a_state *state);
extern void AD9388A_LoadEdid(struct ad9388a_state *state, unsigned char EdidSource);
extern void AD9388A_WriteEdidToProm(struct ad9388a_state *state, unsigned char EdidMode);
extern int AD9388A_GetSignalActiveWidthHeight(struct ad9388a_state *state, long *pWidth, long *pHeight);
#endif
