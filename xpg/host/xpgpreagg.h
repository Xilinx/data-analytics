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
/**
 *  xpgpreagg.h
 *  
 *  Header for XPG custom pre-aggregation methods
 *
 */

#ifndef XPG_PREAGG_H
#define XPG_PREAGG_H

extern "C" {
#include "postgres.h"
}

#include "xpgbolt.h"

extern "C" {
#include "commands/explain.h"
#include "nodes/execnodes.h"
#include "nodes/parsenodes.h"
#include "nodes/plannodes.h"
#include "nodes/relation.h"
}


typedef struct FpgaPreAggState
{
  FpgaTaskState fpgaTaskState;
  int num_agg_cols;
  int num_grp_cols;

  AttrNumber * agg_cols;
  AttrNumber * grp_cols;

} FpgaPreAggState;

extern void xpg_init_preagg_hooks(void);

extern bool xpg_codegendebug;

extern Node * xpgpreagg_create_scan_state(CustomScan * cscan);

extern void xpgpreagg_begin_scan(CustomScanState * node,
                                 EState * estate,
                                 int eflags);
extern TupleTableSlot * xpgpreagg_exec_scan(CustomScanState * node);
extern void xpgpreagg_end_scan(CustomScanState * css);
extern void xpgpreagg_rescan_scan(CustomScanState * css);
extern void xpgpreagg_explain_scan(CustomScanState * css, 
                                   List * ancestors,
                                   ExplainState * explainState);


extern void xpgpreagg_insert_preagg(PlannedStmt * pstmt, Agg * agg);



#endif
