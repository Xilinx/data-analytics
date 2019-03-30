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
 * Read a Burst and put into Stream
 */
void gatherReadInBurstOnly(GMEM_DATA_TYPE *inBufSel, hls::stream<GMEM_DATA_TYPE> inBurstStream[1]){
	GATHER_READIN_BURST_LOOP_BURST_COUNT: for(int burst_count = 0; burst_count < totalInBurst; burst_count++) {
		#pragma HLS PIPELINE
		inBurstStream[0].write(inBufSel[burst_count]);
	}
}

/***
 * Select Input Buffer
 * Read a Burst and put into Stream
 */
void gatherReadInBurst(int opmode, GMEM_DATA_TYPE *inBuf0, GMEM_DATA_TYPE *inBuf1, hls::stream<GMEM_DATA_TYPE> inBurstStream[1]){
	/***
	 * Select the Input buffer
	***/
	GMEM_DATA_TYPE *inBufSel;
	if ((opmode & PING_PONG_IN_MASK) == 0)
		gatherReadInBurstOnly(inBuf0, inBurstStream);
		//inBufSel = inBuf0;
	else
		gatherReadInBurstOnly(inBuf1, inBurstStream);
		//inBufSel = inBuf1;
	/***
	 * Read a Burst and put into Stream
	***/
	/*GATHER_READIN_BURST_LOOP_BURST_COUNT: for(int burst_count = 0; burst_count < totalInBurst; burst_count++) {
		#pragma HLS PIPELINE
		inBurstStream[0].write(inBufSel[burst_count]);
	}*/
}

template<int STREAM>
void gatherReadIn_Template(hls::stream<GMEM_DATA_TYPE> inBurstStream[1], hls::stream<unsigned int> dinstream[MAX_STREAM]) {
	// for each burst
	// write burst into appropriate stream
	GATHER_READIN_TEMPLATE_LT_EQ_LOOP_BURST_COUNT: for(int burst_count = 0; burst_count < totalInBurst; burst_count++) {
		#pragma HLS PIPELINE
		GMEM_DATA_TYPE burst = inBurstStream[0].read();
		#pragma HLS ARRAY_PARTITION variable=burst complete
		// STREAM less than or equal to IN_ENTRIES_PER_BURST
		if (STREAM <= IN_ENTRIES_PER_BURST){
			// number of cycle to write to streams
			GATHER_READIN_TEMPLATE_LT_EQ_LOOP_I: for (int i = 0; i < IN_ENTRIES_PER_BURST/STREAM; i++){
				#pragma HLS PIPELINE
				// write burst to stream
				GATHER_READIN_TEMPLATE_LT_EQ_LOOP_IDX:for (int idx = 0; idx < STREAM; idx++){
					#pragma HLS UNROLL
					dinstream[idx].write(burst.data[(i * STREAM) + idx]);
				}
			}
		}
		// STREAM greater than IN_ENTRIES_PER_BURST
		else {
			GATHER_READIN_TEMPLATE_GT_LOOP_IDX:for (int idx = 0; idx < IN_ENTRIES_PER_BURST; idx++){
				#pragma HLS UNROLL
				dinstream[((burst_count%(STREAM/IN_ENTRIES_PER_BURST))*IN_ENTRIES_PER_BURST) + idx].write(burst.data[idx]);
			}
		}
	}
}

/***
* Distribute the work to the streams
***/
void gatherReadWorkDistribution(int opmode, hls::stream<GMEM_DATA_TYPE> inBurstStream[1], hls::stream<unsigned int> dinstream[MAX_STREAM]){
#define IN_TEMPLATEFUNC(x) \
	else if (opmode & STREAM_MASK_##x){ \
		gatherReadIn_Template<x> (inBurstStream, dinstream); \
	}
	if (false){
	}
	IN_TEMPLATEFUNC(1)
	IN_TEMPLATEFUNC(2)
	IN_TEMPLATEFUNC(4)
	IN_TEMPLATEFUNC(8)
	IN_TEMPLATEFUNC(16)
}

/***
	Core work of a STREAM, read index of A
***/
void readAWork(
		int go,
		int totalWork,
		hls::stream<unsigned int>& dinstream,
		hls::stream<unsigned int>& doutmeasurestream,
		hls::stream<unsigned int>& doutgroupstream,
		GMEM_DATA_TYPE *ddr) {
	GATHER_READA_WORK_LOOP_I: for(int i = 0; i < totalWork; i++) {
		#pragma HLS PIPELINE
		if (go){
			// read A[i]
			unsigned int addrA = dinstream.read();
			// convert A[i] to burst index using outbuf format
			unsigned int burstAddrA = addrA/OUT_ENTRIES_PER_BURST;
			// calculate offset in burst
			unsigned int offsetA0 = 2*(addrA%OUT_ENTRIES_PER_BURST);
			unsigned int offsetA1 = 2*(addrA%OUT_ENTRIES_PER_BURST)+1;
			#pragma HLS ARRAY_PARTITION variable=ddr[burstAddrA].data complete
			GMEM_DATA_TYPE dout;
			#pragma HLS ARRAY_PARTITION variable=dout.data complete
			// read burst
			dout = ddr[burstAddrA];
			// read offset from burst and write to stream
			doutmeasurestream.write(dout.data[offsetA0]);
			doutgroupstream.write(dout.data[offsetA1]);
		}
	}
}

/***
	Work of a STREAM, read index of A
***/
void readA(	int opmode,
		int id,
		hls::stream<unsigned int>& dinstream,
		hls::stream<unsigned int>& doutmeasurestream,
		hls::stream<unsigned int>& doutgroupstream,
		GMEM_DATA_TYPE *ddr) {
	int opmode_stream = ((opmode & STREAM_MASK) >> STREAM_MASK_OFFSET);
	int totalWork = 0;
	int go = 0;
	if (id < opmode_stream){
		go = 1;
	}
#define READAFUNC(x) \
	else if (opmode & STREAM_MASK_##x){ \
		totalWork = totalInWork/x; \
	}
	if (false){
	}
	READAFUNC(1)
	READAFUNC(2)
	READAFUNC(4)
	READAFUNC(8)
	READAFUNC(16)
	readAWork(go, totalWork, dinstream, doutmeasurestream, doutgroupstream, ddr);
#undef READAFUNC
}

/***
	Work of all STREAMs
***/
void gatherReadAll(
	int opmode,
	hls::stream<unsigned int> dinstream[MAX_STREAM],
	hls::stream<unsigned int> doutmeasurestream [MAX_STREAM],
	hls::stream<unsigned int> doutgroupstream [MAX_STREAM],
	GMEM_DATA_TYPE *ddr0,
	GMEM_DATA_TYPE *ddr1,
	GMEM_DATA_TYPE *ddr2,
	GMEM_DATA_TYPE *ddr3,
	GMEM_DATA_TYPE *ddr4,
	GMEM_DATA_TYPE *ddr5,
	GMEM_DATA_TYPE *ddr6,
	GMEM_DATA_TYPE *ddr7,
	GMEM_DATA_TYPE *ddr8,
	GMEM_DATA_TYPE *ddr9,
	GMEM_DATA_TYPE *ddr10,
	GMEM_DATA_TYPE *ddr11,
	GMEM_DATA_TYPE *ddr12,
	GMEM_DATA_TYPE *ddr13,
	GMEM_DATA_TYPE *ddr14,
	GMEM_DATA_TYPE *ddr15) {
	#pragma HLS DATAFLOW
	#pragma HLS ARRAY_PARTITION variable=dinstream complete dim=1
	#pragma HLS ARRAY_PARTITION variable=doutmeasurestream complete dim=1
	#pragma HLS ARRAY_PARTITION variable=doutgroupstream complete dim=1
	/***
		Enable the port connectivity
	***/
#define READAFUNC(x) readA(opmode, x, dinstream[x], doutmeasurestream[x], doutgroupstream[x], ddr##x)
	READAFUNC(0);
	READAFUNC(1);
	READAFUNC(2);
	READAFUNC(3);
	READAFUNC(4);
	READAFUNC(5);
	READAFUNC(6);
	READAFUNC(7);
	READAFUNC(8);
	READAFUNC(9);
	READAFUNC(10);
	READAFUNC(11);
	READAFUNC(12);
	READAFUNC(13);
	READAFUNC(14);
	READAFUNC(15);
#undef READAFUNC
}

/***
	Write Stream Values to a Output Buffer
***/
template<int STREAM>
void gatherWriteOut_Template(hls::stream<unsigned int> doutmeasurestream [MAX_STREAM],
		    hls::stream<unsigned int> doutgroupstream [MAX_STREAM],
			hls::stream<GMEM_DATA_TYPE> outBurstStream[1]) {
	GMEM_DATA_TYPE dout;
	#pragma HLS ARRAY_PARTITION variable=dout complete
	GATHER_WRITEOUT_LOOP_LT_EQ_BURST_COUNT: for(int burst_count = 0; burst_count < totalOutBurst; burst_count++) {
		#pragma HLS PIPELINE
		// STREAM less than or equal to OUT_ENTRIES_PER_BURST
		if (STREAM <= OUT_ENTRIES_PER_BURST){
			// number of cycle to read from streams
			GATHER_WRITEOUT_LOOP_LT_EQ_CYCLE_COUNT: for (int cycle_count = 0; cycle_count < OUT_ENTRIES_PER_BURST/STREAM; cycle_count++) {
				// write stream to burst
				GATHER_WRITEOUT_LOOP_LT_EQ_IDX: for (int idx = 0; idx < STREAM; idx++) {
					#pragma HLS UNROLL
					dout.data[OUT_IN_RATIO*((STREAM*cycle_count) + idx) + 0] = doutmeasurestream[idx].read();
					dout.data[OUT_IN_RATIO*((STREAM*cycle_count) + idx) + 1] = doutgroupstream[idx].read();
				}
			}
		}
		// STREAM greater than OUT_ENTRIES_PER_BURST
		else {
			#pragma HLS ARRAY_PARTITION variable=dout complete
			// for each burst
			GATHER_WRITEOUT_LOOP_GT_ENTRY: for (int entry = 0; entry < OUT_ENTRIES_PER_BURST; entry++) {
				#pragma HLS UNROLL
				dout.data[2*entry] = doutmeasurestream[((burst_count%(STREAM/OUT_ENTRIES_PER_BURST))*OUT_ENTRIES_PER_BURST) + entry].read();
				dout.data[2*entry+1] = doutgroupstream[((burst_count%(STREAM/OUT_ENTRIES_PER_BURST))*OUT_ENTRIES_PER_BURST) + entry].read();
			}
		}
		outBurstStream[0].write(dout);
	}
}

void gatherWriteBurstResult(int opmode, hls::stream<unsigned int> doutmeasurestream[MAX_STREAM], hls::stream<unsigned int> doutgroupstream[MAX_STREAM], hls::stream<GMEM_DATA_TYPE> outBurstStream[1]){
	#define OUT_TEMPLATEFUNC(x) \
		else if (opmode & STREAM_MASK_##x){ \
			gatherWriteOut_Template<x> (doutmeasurestream, doutgroupstream, outBurstStream); \
		}
		if (false){
		}
		OUT_TEMPLATEFUNC(1)
		OUT_TEMPLATEFUNC(2)
		OUT_TEMPLATEFUNC(4)
		OUT_TEMPLATEFUNC(8)
		OUT_TEMPLATEFUNC(16)
	#undef OUT_TEMPLATEFUNC
}

/***
 * Select Output Buffer To Write Result/Burst
 */
void gatherWriteOutResultOnly(hls::stream<GMEM_DATA_TYPE> outBurstStream[1], GMEM_DATA_TYPE *outBufSel) {
	/***
	 * Write Stream Values to a Output Buffer
	 */
	GATHER_WRITEOUT_RESULT_LOOP_BURST_COUNT: for(int burst_count = 0; burst_count < totalOutBurst; burst_count++) {
		#pragma HLS PIPELINE
		outBufSel[burst_count] = outBurstStream[0].read();
	}
}

/***
 * Select Output Buffer To Write Result/Burst
 */
void gatherWriteOutResult(int opmode, hls::stream<GMEM_DATA_TYPE> outBurstStream[1], GMEM_DATA_TYPE *outBuf0, GMEM_DATA_TYPE *outBuf1) {

	/***
	 * Select the Output buffer
	***/
	GMEM_DATA_TYPE *outBufSel;
	if ((opmode & PING_PONG_OUT_MASK) == 0)
		gatherWriteOutResultOnly (outBurstStream, outBuf0);
		//outBufSel = outBuf0;
	else
		gatherWriteOutResultOnly (outBurstStream, outBuf1);
		//outBufSel = outBuf1;

	/***
	 * Write Stream Values to a Output Buffer
	 */
	/*GATHER_WRITEOUT_RESULT_LOOP_BURST_COUNT: for(int burst_count = 0; burst_count < totalOutBurst; burst_count++) {
		#pragma HLS PIPELINE
		outBufSel[burst_count] = outBurstStream[0].read();
	}*/
}

/***
	Top level gather
***/
void gather(int opmode,
	GMEM_DATA_TYPE *inbuf0,
	GMEM_DATA_TYPE *inbuf1,
	GMEM_DATA_TYPE *outbuf0,
	GMEM_DATA_TYPE *outbuf1,
	GMEM_DATA_TYPE *ddr0,
	GMEM_DATA_TYPE *ddr1,
	GMEM_DATA_TYPE *ddr2,
	GMEM_DATA_TYPE *ddr3,
	GMEM_DATA_TYPE *ddr4,
	GMEM_DATA_TYPE *ddr5,
	GMEM_DATA_TYPE *ddr6,
	GMEM_DATA_TYPE *ddr7,
	GMEM_DATA_TYPE *ddr8,
	GMEM_DATA_TYPE *ddr9,
	GMEM_DATA_TYPE *ddr10,
	GMEM_DATA_TYPE *ddr11,
	GMEM_DATA_TYPE *ddr12,
	GMEM_DATA_TYPE *ddr13,
	GMEM_DATA_TYPE *ddr14,
	GMEM_DATA_TYPE *ddr15) {
	// DATAFLOW Execution
	#pragma HLS DATAFLOW

	/***
		Declare the streams
	***/

	// inBurstStream
	// FIXME: Needed a dimension to remove the warning, ask why?
	hls::stream<GMEM_DATA_TYPE> inBurstStream[1];
	#pragma HLS STREAM variable=inBurstStream depth=64 dim=1
	#pragma HLS ARRAY_PARTITION variable=inBurstStream complete dim=1

	// outBurstStream
	// FIXME: Needed a dimension to remove the warning, ask why?
	hls::stream<GMEM_DATA_TYPE> outBurstStream[1];
	#pragma HLS STREAM variable=outBurstStream depth=64 dim=1
	#pragma HLS ARRAY_PARTITION variable=outBurstStream complete dim=1

	// dInStream
	hls::stream<unsigned int> dinstream [MAX_STREAM];
	#pragma HLS STREAM variable=dinstream depth=64 dim=1
	#pragma HLS ARRAY_PARTITION variable=dinstream complete dim=1

	// dOutStreams
	hls::stream<unsigned int> doutmeasurestream [MAX_STREAM];
	#pragma HLS STREAM variable=doutmeasurestream depth=64 dim=1
	#pragma HLS ARRAY_PARTITION variable=doutmeasurestream complete dim=1

	hls::stream<unsigned int> doutgroupstream [MAX_STREAM];
	#pragma HLS STREAM variable=doutgroupstream depth=64 dim=1
	#pragma HLS ARRAY_PARTITION variable=doutgroupstream complete dim=1
	/***
	 * Select Input Buffer
	 * Read a Burst and put into Stream
	 ***/
	gatherReadInBurst(opmode, inbuf0, inbuf1, inBurstStream);

	/***
	 * Distribute the work to the streams
	 ***/
	gatherReadWorkDistribution(opmode, inBurstStream, dinstream);

	/***
	 * Perform Work
	***/
	gatherReadAll(opmode, dinstream, doutmeasurestream, doutgroupstream,
			ddr0, ddr1, ddr2, ddr3, ddr4, ddr5, ddr6, ddr7,
			ddr8, ddr9, ddr10, ddr11, ddr12, ddr13, ddr14, ddr15);

	/***
	 *  Create Burst
	 */
	gatherWriteBurstResult(opmode, doutmeasurestream, doutgroupstream, outBurstStream);

	/***
	 *  Write result to Output Buffer
	 */
	gatherWriteOutResult(opmode, outBurstStream, outbuf0, outbuf1);
}
