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

extern "C" {
#include "postgres.h"
#include "fmgr.h"
}

#include "xpgpreagg.h"
#include "xpgbolt.h"

extern "C" {
#include "nodes/extensible.h"
#include "nodes/nodes.h"
#include "nodes/print.h"
#include "nodes/relation.h"
#include "optimizer/pathnode.h"
#include "optimizer/paths.h"
#include "optimizer/restrictinfo.h"
}

static CustomScanMethods xpgpreagg_scan_methods;
static CustomExecMethods xpgpreagg_exec_methods;

void
xpg_init_preagg_hooks()
{
  /* Setup xpg scan plan methods to postgres CustomPlanMethods hooks*/
  xpgpreagg_scan_methods.CustomName = "XPGPreagg";
  /* This callback shall be invoked when the core backend tries to populate
   * CustomScanState node according to the input CustomScanNode*/
  /* Prototype: xpgscan_create_scan_state(CustomScan * cscan)*/
  xpgpreagg_scan_methods.CreateCustomScanState = 
    xpgpreagg_create_scan_state;

  /* Setup xpg scan methods to postgres CustomExecMethods hooks*/
  xpgpreagg_exec_methods.CustomName = "XPGPreAgg";
  /* xpg_begin(CustomScanState *, EState*, int eflags) */
  /* xpg_exec(CustomScanState *node) */
  /* xpg_end(CustomScanState *node) */
  /* xpg_rescan(CustomScanState *node) */
  /* xpg_explain(CustomScanState *node) */
  xpgpreagg_exec_methods.BeginCustomScan = xpgpreagg_begin_scan;
  xpgpreagg_exec_methods.ExecCustomScan = xpgpreagg_exec_scan;
  xpgpreagg_exec_methods.EndCustomScan = xpgpreagg_end_scan;
  xpgpreagg_exec_methods.ReScanCustomScan = xpgpreagg_rescan_scan;
  xpgpreagg_exec_methods.ExplainCustomScan = xpgpreagg_explain_scan;
}

/**
 *  Description: Allocation of XPGScanState, overriding CustomScanState
 *  Called by: postgres call back funcion pointer CreateCustomScanState
 */
Node*
xpgpreagg_create_scan_state(CustomScan * cscan)
{
  FpgaPreAggState * fpgaPreAggState = (FpgaPreAggState *) palloc0(sizeof(FpgaPreAggState));

  /* set exec methods callbacks */
  NodeSetTag(fpgaPreAggState, T_CustomScanState);

  fpgaPreAggState->fpgaTaskState.css.flags = cscan->flags;
  fpgaPreAggState->fpgaTaskState.relid = cscan->scan.scanrelid;


  fpgaPreAggState->fpgaTaskState.css.methods = &xpgpreagg_exec_methods;

  return (Node*) fpgaPreAggState;
}

void
xpgpreagg_begin_scan(CustomScanState *node, EState *estate, int eflags)
{
}

TupleTableSlot *
xpgpreagg_exec_scan(CustomScanState * node) 
{
  /* WIP */
  return NULL; // temp
}

void
xpgpreagg_end_scan(CustomScanState * css)
{
  /*WIP*/
  return;
}

void
xpgpreagg_rescan_scan(CustomScanState * css)
{
  return;
}

void
xpgpreagg_explain_scan(CustomScanState * css,
                       List * ancestors,
                       ExplainState * explainState)
{
  /*WIP*/
  return;
}

/**
 *  Description: 
 *  If we find an AGG node in the plan tree, we can try inserting
 *  our xpgpreagg method.
 *
 *  Called by: xpg_recursive_plantree_traverse
 * 
 */
void
xpgpreagg_insert_preagg(PlannedStmt * pstmt, 
                        Agg * agg)
{
  if(xpg_codegendebug)
    elog(INFO, "calling xpgpreagg_insert_preagg");
  double numGroups;
  Plan * outer_node;
  Index outer_scanrelid;

  CustomScan * cscan;

  // For now not inserting anything until we fully test preagg
  return;

  if (!xpg_enabled)
    return;

  /* If the AGG is not operating on all input nodes, early return
   * (we are currenly not supporting hash aggr)
   */
  if (agg->aggstrategy != AGG_PLAIN)
    return;


  numGroups = Max(agg->plan.plan_rows, 1.0);
  if (numGroups != 1.0) {
    elog(WARNING, "Number of estimated groups: %f exceeds \
        expected value (1.0)", numGroups);
    return;
  }

  outer_node = outerPlan(agg);
  outer_scanrelid = ((Scan *) outer_node)->scanrelid;
  if(xpg_codegendebug)
    elog(INFO, "Creating custom scan node");

  /* Create custom scan node and inject it */
  cscan = makeNode(CustomScan);
  cscan->scan.plan.startup_cost = 0.0;
  cscan->scan.plan.total_cost   = 0.0;
  cscan->scan.plan.plan_rows    = 0.0;
  cscan->scan.plan.plan_width   = 0.0;
  cscan->scan.plan.targetlist   = agg->plan.targetlist;
  cscan->scan.plan.qual         = NIL; 
  cscan->scan.plan.lefttree     = NULL;
  cscan->scan.scanrelid         = outer_scanrelid;
  cscan->flags                  = 0;
  cscan->custom_scan_tlist      = NIL;
  cscan->methods                = &xpgpreagg_scan_methods;

  pprint(pstmt);

  outerPlan(agg) = &cscan->scan.plan;

  pprint(pstmt);
}

