/**********
Copyright 2017 Xilinx, Inc.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**********/
#ifndef XPG_BOLT_H
#define XPG_BOLT_H

extern "C" {
#include "postgres.h"
#include "c.h"
#include "fmgr.h"
#include "access/htup_details.h"
#include "nodes/execnodes.h"
#include "nodes/parsenodes.h"
#include "nodes/plannodes.h"
#include "portability/instr_time.h"
}

#include "xpg_fpga_defs.h"

#define EXIT_FINISH 2
#define XdbConnQueDepth 256

extern bool xpg_enabled;
extern bool xpg_asciiFlow;
extern bool xpg_fpgaoffload;
extern bool xpg_timerOn;

extern double xpg_datasendTime;
extern double xpg_scanTime;
extern double xpg_receiveTime;

extern instr_time receiveBeg;
extern instr_time receiveEnd;

/**
 *  Description: Flattened version of HeapTupleData
 */
typedef struct XpgTuple
{
  uint16 t_len; /* 16 bit */
  ItemPointerData t_self; /* 16 + 16 + 16 bit*/
  HeapTupleHeaderData t_data;
} XpgTuple;

/**
 *  Description: 
 *  Buffer to communicate between host and device
 */
typedef struct XpgBuffer
{
  Size length;
  char * buf;
  int numTups;
} XpgBuffer;

/**
 *
 */
typedef struct XpgVarStream
{
  Size length;
  char * var;
  int numTups;
} XpgVarStream;

/**
 * Description:
 * Records the state of data sender
 */
typedef struct XpgSendState {
  int blkNum;
  int nBlks;
  int linNum;  // init to FirstOffsetNumber
  int curr_queue_id;
  bool is_first_blk;
} XpgSendState; 

/**
 * Description:
 * Records the state of data receiver
 */
typedef struct XpgReceiveState {
  int curr_result_id;  
  int curr_queue_id;
  int total_queues;
} XpgReceiveState;

typedef struct XpgNativeStream 
{
  Size length;
  unsigned char * blk;
  long numTups;
} XpgNativeStream;

typedef struct XpgSetupStream
{
  Size length;
  char * setup;
} XpgSetupStream;

typedef struct XpgNativeSetupStream
{
  unsigned char * setup;
  unsigned char * setup_ptr;
} XpgNativeSetupStream;

typedef struct XpgTupleCache
{
  uintptr_t * cache;
} XpgTupleCache;



/* Data buffers requires for each OpenCL queue */
typedef struct XpgBufferSet
{
  /* Stream to be send to Fpga 
   * Mapped to xdbconn write buffer
   * DO NOT allocate blk field in this struct
   */
  XpgNativeStream native_stream;

  /* Tuple Buffer that holds full tuples only on host*/
  XpgBuffer tup_buffer;

  /* Starting address of tuples cached on host */
  XpgTupleCache tuple_cache;

  /* Result Buffer */
  XpgBuffer result_buffer;

} XpgBufferSet;

/* Container storing buffer set for multiple queues */
typedef struct XpgBufferSetContainer
{
  XpgBufferSet * list;
} XpgBufferSetContainer;

extern XpgBufferSetContainer xpg_buffer_container;


/** Declare FPGA specific CustomScanState 
 *  This will override CustomScanState with more customized fields
 */
typedef struct FpgaTaskState
{
  CustomScanState css;

  XpgBuffer next_buffer;  /* Buffer carrying tuples to be sent to FPGA*/
  XpgBuffer prev_buffer ; /* Buffer carrying tuples being processed on FPGA */

  XpgTupleCache next_cache;
  XpgTupleCache prev_cache;

  XpgBuffer result_buffer;
  int relid;
  XpgVarStream varstream;
  XpgNativeStream nativestream;
  
  XpgNativeSetupStream nativeSetupStream;

  OffsetNumber line_offset;   /* last stopped offset when reading buffer */

  XpgSendState send_state;
  XpgReceiveState receive_state;
} FpgaTaskState;

#define FTS_GET_SCAN_TUPDESC(fts)				\
	(((FpgaTaskState *)(fts))->css.ss.ss_ScanTupleSlot->tts_tupleDescriptor)

extern "C" void _PG_init(void);
extern  "C" PlannedStmt * xpg_planner_entrypoint(Query * parse,
                                            int cursorOptions,
                                            ParamListInfo boundParams);
extern "C" void xpg_recursive_plantree_traverse(PlannedStmt * pstmt,
                                            Plan * parentPlan,
                                            Plan * currPlan);

#define XQL_WORD_SZ 16
#define XQL_PAGE_SZ 8192                              // 8KB
#define XQL_BLK_SZ (2*1024*1024)

#define XQL_WORDS_PER_PAGE (XQL_PAGE_SZ/XQL_WORD_SZ)  // 512

#define XQL_PAGES_PER_BLK  (XQL_BLK_SZ/XQL_PAGE_SZ) // 256

#define XQL_NUM_WORDS (XQL_BLK_SZ/XQL_WORD_SZ)        // 131072 words

#define XQL_NUM_RES_WORDS 1

#endif
