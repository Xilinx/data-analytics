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

#ifndef OMX_OMX_H
#define OMX_OMX_H

//USED FOR CO-SIMULATION
//#define SMALL_SIZE

//	TODO: Move to HEADER (shared by all projects)
/***
***	MACROs for PRAGMA
***/
#define PRAGMA_SUB(x) _Pragma (#x)
#define PRAGMA_HLS(x) PRAGMA_SUB(x)

/***
***	MACROs for PRAGMA
***/
/* definition to expand macro then apply to pragma message */
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "=" VALUE(var)

//#define PRINT(var) PRAGMA_SUB( message (VAR_NAME_VALUE(var)))
#define PRINT(var) PRAGMA_SUB( message ( #var "=" VALUE(var)))


// Example
// #pragma message(VAR_NAME_VALUE(VAR))


/***
***	MACROs for SIZE
***/
#define BYTE 1
#define KILO 1024
#define KB KILO*BYTE
#define MB KILO*KB
#define GB KILO*MB

/***
***	MACROs for String
***/
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

/***
***	MACROs for TOTAL_WORK
***/
#define TOTAL_WORK_SIZE (1*MB)

/***
***	MACROs for DDR
***/
#define DDR_SIZE (256*MB)

/***
***	MACROs for DDR
***/
#define MAX_DDR_SIZE (256*MB)

/***
	TODO: Move to HEADER (shared by host and kernel)
	OPMODE INFO
***/
/***
***	MACROs for MASK
***/
#define BIT_MASK(x) (1U << x)
#define BIT_OFFSET(x,y) (x << y)
#define BIT_ENABLE_ENC(x) (BIT_MASK(x) - 1)
#define START 0
/***
	PING_PONG
***/
#define PING_PONG_IN_MASK_SIZE 1
#define PING_PONG_IN_MASK_OFFSET (START)
#define PING_PONG_IN_MASK BIT_OFFSET(BIT_ENABLE_ENC(PING_PONG_IN_MASK_SIZE), PING_PONG_IN_MASK_OFFSET)
#define PING_PONG_IN_MASK_OFF (~PING_PONG_IN_MASK)

#define PING_PONG_OUT_MASK_SIZE 1
#define PING_PONG_OUT_MASK_OFFSET (PING_PONG_IN_MASK_OFFSET + PING_PONG_IN_MASK_SIZE)
#define PING_PONG_OUT_MASK BIT_OFFSET(BIT_ENABLE_ENC(PING_PONG_OUT_MASK_SIZE), PING_PONG_OUT_MASK_OFFSET)
#define PING_PONG_OUT_MASK_OFF (~PING_PONG_OUT_MASK)
/***
	STREAM
***/
#define STREAM_MASK_SIZE 5
#define STREAM_MASK_OFFSET (PING_PONG_OUT_MASK_OFFSET + PING_PONG_OUT_MASK_SIZE)
// 1 STREAM
#define STREAM_MASK_1 BIT_OFFSET(BIT_MASK(0), STREAM_MASK_OFFSET)
#define STREAM_MASK_1_OFF (~STREAM_MASK_1)
// 2 STREAM
#define STREAM_MASK_2 BIT_OFFSET(BIT_MASK(1), STREAM_MASK_OFFSET)
#define STREAM_MASK_2_OFF (~STREAM_MASK_2)
// 4 STREAM
#define STREAM_MASK_4 BIT_OFFSET(BIT_MASK(2), STREAM_MASK_OFFSET)
#define STREAM_MASK_4_OFF (~STREAM_MASK_4)
// 8 STREAM
#define STREAM_MASK_8 BIT_OFFSET(BIT_MASK(3), STREAM_MASK_OFFSET)
#define STREAM_MASK_8_OFF (~STREAM_MASK_8)
// 16 STREAM
#define STREAM_MASK_16 BIT_OFFSET(BIT_MASK(4), STREAM_MASK_OFFSET)
#define STREAM_MASK_16_OFF (~STREAM_MASK_16)
// All STREAM
#define STREAM_MASK BIT_OFFSET(BIT_ENABLE_ENC(STREAM_MASK_SIZE), STREAM_MASK_OFFSET)

/***
***	MACROs for NUM_PU
***/
#define NUM_PU 16

#if NUM_PU > 16
#error "Invalid NUM_PU value!"
#endif

/***
***	MACROs for DATA_TYPE_SIZE, DATA_TYPE
***/
#define DATA_TYPE int
//TODO: fix
#define DATA_TYPE_SIZE 4*BYTE//sizeof(DATA_TYPE)


/***
***	MACROs for BURST_WIDTH type
***/
// BURST_WIDTH from DDR
#define BURST_WIDTH 32*BYTE

// BURST_WIDTH_TYPEDEF declaration to define type
#define BURST_WIDTH_TYPENAME(x) Data##x##B
#define BURST_WIDTH_TYPEDEF(x) typedef struct __wide##x{\
	unsigned DATA_TYPE data[x/DATA_TYPE_SIZE];\
} BURST_WIDTH_TYPENAME(x);

// Select/enable valid burst type
#if (BURST_WIDTH == 32)
BURST_WIDTH_TYPEDEF(32)
#define BURST_DATATYPE BURST_WIDTH_TYPENAME(32)
#elif (BURST_WIDTH == 64)
BURST_WIDTH_TYPEDEF(64)
#define BURST_DATATYPE BURST_WIDTH_TYPENAME(64)
#elif (BURST_WIDTH == 128)
BURST_WIDTH_TYPEDEF(128)
#define BURST_DATATYPE BURST_WIDTH_TYPENAME(128)
#else
#error "Invalid BURST_WIDTH value!"
#endif

#define GMEM_DATA_TYPE BURST_DATATYPE

#define BYTES_PER_BURST (BURST_WIDTH)
#define BYTES_PER_IN_ENTRY (4*BYTE)
#define BYTES_PER_OUT_ENTRY (8*BYTE)
#define IN_ENTRIES_PER_BURST (BYTES_PER_BURST/(BYTES_PER_IN_ENTRY))
#define OUT_ENTRIES_PER_BURST (BYTES_PER_BURST/(BYTES_PER_OUT_ENTRY))
#define OUT_IN_RATIO (BYTES_PER_OUT_ENTRY/BYTES_PER_IN_ENTRY)


/***
	Check BYTES_PER_BURST and DATA_TYPE_SIZE
***/
#if (((BYTES_PER_BURST%DATA_TYPE_SIZE)) != 0)
#error "Invalid BYTES_PER_BURST and DATA_TYPE_SIZE value combination!"
#endif

/***
	Check BYTES_PER_BURST and BYTES_PER_IN_ENTRY
***/
#if (((BYTES_PER_BURST%BYTES_PER_IN_ENTRY)) != 0)
#error "Invalid BYTES_PER_BURST and BYTES_PER_IN_ENTRY value combination!"
#endif

/***
	Check BYTES_PER_BURST and BYTES_PER_OUT_ENTRY
***/
#if (((BYTES_PER_BURST%BYTES_PER_OUT_ENTRY)) != 0)
#error "Invalid BYTES_PER_BURST and BYTES_PER_OUT_ENTRY value combination!"
#endif


static const unsigned int MAX_STREAM = NUM_PU;
// total number of burst from inbuf
static const unsigned int totalInBurst = (TOTAL_WORK_SIZE/BYTES_PER_BURST);
// total number of entries/work from inbuf
static const unsigned int totalInWork = (TOTAL_WORK_SIZE/BYTES_PER_IN_ENTRY);
// total number of burst from outbuf
static const unsigned int totalOutBurst = ((OUT_IN_RATIO*TOTAL_WORK_SIZE)/BYTES_PER_BURST);
#ifdef SMALL_SIZE
// total number of burst from DDR
static const unsigned int  totalDDRBurst = 1U << 18 ;
#else
// total number of burst from DDR
static const unsigned int totalDDRBurst = (MAX_DDR_SIZE/BYTES_PER_BURST);
#endif

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
	GMEM_DATA_TYPE ddr15[totalDDRBurst]);

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
	GMEM_DATA_TYPE *ddr15);

#endif
