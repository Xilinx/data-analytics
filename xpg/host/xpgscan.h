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
 * xpgscan.h
 * 
 * Header for XPG custom scan methods
 */

#ifndef XPG_SCAN_H
#define XPG_SCAN_H

#include "xpgbolt.h"
extern "C" {
#include "postgres.h"


#include "commands/explain.h"
#include "nodes/execnodes.h"
#include "nodes/parsenodes.h"
#include "nodes/plannodes.h"
#include "nodes/relation.h"
}

#include "xpgdatastore.h"

typedef struct FpgaScanState
{
  FpgaTaskState fpgaTaskState;
	HeapTupleData	scan_tuple;		/* buffer to fetch tuple */
  int curr_result_id;
} FpgaScanState;

extern "C" void xpg_init_scan_hooks(void);

extern "C"  Node * xpg_create_scan_state(CustomScan * cscan);

extern "C" void xpg_add_rel_scan_path(PlannerInfo *root, 
                           RelOptInfo *rel, 
                           Index rtidx, 
                           RangeTblEntry *rte);

extern "C" Plan * xpg_plan_scan_path(PlannerInfo * root,
                                 RelOptInfo * rel,
                                 CustomPath * best_path,
                                 List * tlist,
                                 List * clauses,
                                 List * custom_plans);

extern "C" void xpg_cost_scan_path(CustomPath * pathnode,
                               PlannerInfo * root,
                               RelOptInfo * baserel,
                               List * device_quals,
                               List * host_quals);

extern "C" void xpg_begin_scan(CustomScanState * css_node, EState *estate, int eflags);
extern "C" TupleTableSlot * xpg_exec_scan(CustomScanState * css_node);
extern "C" void xpg_end_scan(CustomScanState * css_node);
extern "C" void xpg_rescan_scan(CustomScanState * css_node);
extern "C" void xpg_explain_scan(CustomScanState *node, 
                             List *ancestors, 
                             ExplainState *es);

/*FpgaScanState is a wrapper of defalut CustomScanState*/
extern "C" TupleTableSlot * xpg_exec_scan_access_mtd(FpgaTaskState * fpgaState);
extern "C" TupleTableSlot * xpg_exec_scan_access_ocl_mtd(FpgaTaskState * fpgaState);

extern "C" bool xpg_exec_scan_recheck_mtd(CustomScanState * css, TupleTableSlot * slot);

extern "C" TupleTableSlot * get_next_result_tuple(FpgaTaskState * fpgaTaskState);
extern "C" TupleTableSlot * get_next_result_tuple_ocl(FpgaTaskState * fpgaTaskState);

extern bool xpg_codegendebug;
#endif
