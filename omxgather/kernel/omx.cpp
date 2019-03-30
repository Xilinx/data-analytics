/*
 * Copyright (C) 2019, Xilinx Inc - All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <string.h>
#include <stdio.h>
#include <ap_int.h>
#include <hls_stream.h>
#include <stdint.h>
#include "omx.h"

/***
	Top level Kernel
***/
extern "C" void omx (int opmode,
	GMEM_DATA_TYPE inbuf0[totalInBurst],
	GMEM_DATA_TYPE inbuf1[totalInBurst],
	GMEM_DATA_TYPE outbuf0[totalOutBurst],
	GMEM_DATA_TYPE outbuf1[totalOutBurst],
	GMEM_DATA_TYPE ddr0[totalDDRBurst],
	GMEM_DATA_TYPE ddr1[totalDDRBurst],
	GMEM_DATA_TYPE ddr2[totalDDRBurst],
	GMEM_DATA_TYPE ddr3[totalDDRBurst],
	GMEM_DATA_TYPE ddr4[totalDDRBurst],
	GMEM_DATA_TYPE ddr5[totalDDRBurst],
	GMEM_DATA_TYPE ddr6[totalDDRBurst],
	GMEM_DATA_TYPE ddr7[totalDDRBurst],
	GMEM_DATA_TYPE ddr8[totalDDRBurst],
	GMEM_DATA_TYPE ddr9[totalDDRBurst],
	GMEM_DATA_TYPE ddr10[totalDDRBurst],
	GMEM_DATA_TYPE ddr11[totalDDRBurst],
	GMEM_DATA_TYPE ddr12[totalDDRBurst],
	GMEM_DATA_TYPE ddr13[totalDDRBurst],
	GMEM_DATA_TYPE ddr14[totalDDRBurst],
	GMEM_DATA_TYPE ddr15[totalDDRBurst]) {

#pragma HLS INTERFACE s_axilite port=return bundle=control
#pragma HLS INTERFACE s_axilite port=opmode bundle=control

#pragma HLS INTERFACE m_axi port=inbuf0 bundle=gmem_inbuf0  num_write_outstanding=32 num_read_outstanding=32 max_write_burst_length=64 max_read_burst_length=64  latency=125
#pragma HLS INTERFACE m_axi port=inbuf1 bundle=gmem_inbuf1  num_write_outstanding=32 num_read_outstanding=32 max_write_burst_length=64 max_read_burst_length=64  latency=125
#pragma HLS INTERFACE m_axi port=outbuf0 bundle=gmem_outbuf0  num_write_outstanding=32 num_read_outstanding=32 max_write_burst_length=64 max_read_burst_length=64 latency=125
#pragma HLS INTERFACE m_axi port=outbuf1 bundle=gmem_outbuf1  num_write_outstanding=32 num_read_outstanding=32 max_write_burst_length=64 max_read_burst_length=64 latency=125  
#pragma HLS INTERFACE s_axilite port=inbuf0 bundle=control
#pragma HLS INTERFACE s_axilite port=inbuf1 bundle=control
#pragma HLS INTERFACE s_axilite port=outbuf0 bundle=control
#pragma HLS INTERFACE s_axilite port=outbuf1 bundle=control
#pragma HLS data_pack variable=inbuf0
#pragma HLS data_pack variable=inbuf1
#pragma HLS data_pack variable=outbuf0
#pragma HLS data_pack variable=outbuf1

#define DDR_PORT_DIRECTIVE_0(x) PRAGMA_HLS(HLS INTERFACE m_axi port=ddr##x  bundle=gmem_ddr##x num_write_outstanding=32 num_read_outstanding=32 max_write_burst_length=64 max_read_burst_length=64 latency=125)
#define DDR_PORT_DIRECTIVE_1(x) PRAGMA_HLS(HLS INTERFACE s_axilite port=ddr##x bundle=control)
#define DDR_PORT_DIRECTIVE_2(x) PRAGMA_HLS(HLS data_pack variable=ddr##x)
	// DDR0
	DDR_PORT_DIRECTIVE_0(0)
	DDR_PORT_DIRECTIVE_1(0)
	DDR_PORT_DIRECTIVE_2(0)
	// DDR1
	DDR_PORT_DIRECTIVE_0(1)
	DDR_PORT_DIRECTIVE_1(1)
	DDR_PORT_DIRECTIVE_2(1)
	// DDR2
	DDR_PORT_DIRECTIVE_0(2)
	DDR_PORT_DIRECTIVE_1(2)
	DDR_PORT_DIRECTIVE_2(2)
	// DDR3
	DDR_PORT_DIRECTIVE_0(3)
	DDR_PORT_DIRECTIVE_1(3)
	DDR_PORT_DIRECTIVE_2(3)
	// DDR4
	DDR_PORT_DIRECTIVE_0(4)
	DDR_PORT_DIRECTIVE_1(4)
	DDR_PORT_DIRECTIVE_2(4)
	// DDR5
	DDR_PORT_DIRECTIVE_0(5)
	DDR_PORT_DIRECTIVE_1(5)
	DDR_PORT_DIRECTIVE_2(5)
	// DDR6
	DDR_PORT_DIRECTIVE_0(6)
	DDR_PORT_DIRECTIVE_1(6)
	DDR_PORT_DIRECTIVE_2(6)
	// DDR7
	DDR_PORT_DIRECTIVE_0(7)
	DDR_PORT_DIRECTIVE_1(7)
	DDR_PORT_DIRECTIVE_2(7)
	// DDR8
	DDR_PORT_DIRECTIVE_0(8)
	DDR_PORT_DIRECTIVE_1(8)
	DDR_PORT_DIRECTIVE_2(8)
	// DDR9
	DDR_PORT_DIRECTIVE_0(9)
	DDR_PORT_DIRECTIVE_1(9)
	DDR_PORT_DIRECTIVE_2(9)
	// DDR10
	DDR_PORT_DIRECTIVE_0(10)
	DDR_PORT_DIRECTIVE_1(10)
	DDR_PORT_DIRECTIVE_2(10)
	// DDR11
	DDR_PORT_DIRECTIVE_0(11)
	DDR_PORT_DIRECTIVE_1(11)
	DDR_PORT_DIRECTIVE_2(11)
	// DDR12
	DDR_PORT_DIRECTIVE_0(12)
	DDR_PORT_DIRECTIVE_1(12)
	DDR_PORT_DIRECTIVE_2(12)
	// DDR13
	DDR_PORT_DIRECTIVE_0(13)
	DDR_PORT_DIRECTIVE_1(13)
	DDR_PORT_DIRECTIVE_2(13)
	// DDR14
	DDR_PORT_DIRECTIVE_0(14)
	DDR_PORT_DIRECTIVE_1(14)
	DDR_PORT_DIRECTIVE_2(14)
	// DDR15
	DDR_PORT_DIRECTIVE_0(15)
	DDR_PORT_DIRECTIVE_1(15)
	DDR_PORT_DIRECTIVE_2(15)

gather (opmode,
	inbuf0,
	inbuf1,
	outbuf0,
	outbuf1,
	ddr0,
	ddr1,
	ddr2,
	ddr3,
	ddr4,
	ddr5,
	ddr6,
	ddr7,
	ddr8,
	ddr9,
	ddr10,
	ddr11,
	ddr12,
	ddr13,
	ddr14,
	ddr15);

}
