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



#include "optimizer/planner.h"
#include "utils/builtins.h"
#include "utils/guc.h"
#include "utils/int8.h"
}

#include "xpgpreagg.h"
#include "xpgscan.h"
#include "xpgbolt.h"
#ifndef SQLENGINE_DECIMAL_H
#include "decimal.h"
#include <set>
#endif

bool xpg_enabled;
bool xpg_asciiFlow;
bool xpg_fpgaoffload;
bool xpg_codegendebug;
bool xpg_timerOn;

double xpg_datasendTime;
double xpg_scanTime;
double xpg_receiveTime;
int *agg_cols;
instr_time receiveBeg;
instr_time receiveEnd;
set<int> xpg_set_cols_agg;
XpgBufferSetContainer xpg_buffer_container;

extern "C" {
PG_MODULE_MAGIC;

static planner_hook_type planner_hook_prev;

static void
xpg_init_guc()
{
  /* XPG main switch */
  DefineCustomBoolVariable(
      "xpgbolt.enabled", 
      "Enables XPG features",
      NULL /*Long Desc*/,
      &xpg_enabled,
      true /*initVal*/,
      PGC_USERSET,
      GUC_NOT_IN_SAMPLE,
      NULL /*GucBoolCheckHook*/, 
      NULL /*GucBollAssignHook*/, 
      NULL /*GucShowHook*/);
  DefineCustomBoolVariable(
     "xpgbolt.asciiFlow",
     "Enables ascii data flow",
     NULL /*Long Desc*/,
     &xpg_asciiFlow,
     true /*initVal*/,
     PGC_USERSET,
     GUC_NOT_IN_SAMPLE,
     NULL /*GucBoolCheckHook*/,
     NULL /*GucBollAssignHook*/,
     NULL /*GucShowHook*/);
  DefineCustomBoolVariable(
     "xpgbolt.fpga_offload",
     "Enables FPGA offload calls",
     NULL /*Long Desc*/,
     &xpg_fpgaoffload,
     false /*initVal*/,
     PGC_USERSET,
     GUC_NOT_IN_SAMPLE,
     NULL /*GucBoolCheckHook*/,
     NULL /*GucBollAssignHook*/,
     NULL /*GucShowHook*/);
  DefineCustomBoolVariable(
     "xpgbolt.timerOn",
     "print runtime info",
     NULL /*Long Desc*/,
     &xpg_timerOn,
     true /*initVal*/,
     PGC_USERSET,
     GUC_NOT_IN_SAMPLE,
     NULL /*GucBoolCheckHook*/,
     NULL /*GucBollAssignHook*/,
     NULL /*GucShowHook*/);
  DefineCustomBoolVariable(
     "xpgbolt.codegen_debug",
     "Enables debug print messages in codegen",
     NULL /*Long Desc*/,
     &xpg_codegendebug,
     false /*initVal*/,
     PGC_USERSET,
     GUC_NOT_IN_SAMPLE,
     NULL /*GucBoolCheckHook*/,
     NULL /*GucBollAssignHook*/,
     NULL /*GucShowHook*/);

}

void release_fpga() {
  #ifdef HW_INTEGRATION
  if(xpg_fpgaoffload) {
    elog(LOG, "Releasing FPGA\n");
    //printf("done with xpg\n");
    xpg_rel_fpga();
    xpg_fpgaoffload = false;
  } else {
    elog(LOG, "No FPGA to release!\n");
  }
  #endif
}

PG_FUNCTION_INFO_V1(init_fpga);

Datum init_fpga(PG_FUNCTION_ARGS) {
  #ifdef HW_INTEGRATION
  char * xclbinPathName = text_to_cstring(PG_GETARG_TEXT_PP(0));

  xpg_init_fpga_defs(xclbinPathName); 
  xpg_fpgaoffload = true;
  #endif

  PG_RETURN_VOID();
}

void set_cols_agg(int2vector * t) {
  int16 *x = t->values;
  int s = t->dim1;
  //  elog(INFO,"%d",s);
  //  vector<int> add_cols
  while(s) {
    if(*x!=0) {
      elog(INFO,"Adding column to the filter%d",*x);
      xpg_set_cols_agg.insert(*x);
      s--;
    }
    x++;
  }

}

extern void
_PG_init(void)
{
  elog(LOG, " ");
  elog(LOG, "XPG-Bolt Version 0.1");

  xpg_init_guc();
  xpg_init_datastore();

  xpg_init_scan_hooks();
  xpg_init_preagg_hooks();

  planner_hook_prev = planner_hook;
  planner_hook = xpg_planner_entrypoint;
  
  // Use init_fpga UDF to download binary and turn ON fpga offloading
  //Init FPGA defs
  /*#ifdef HW_INTEGRATION
  xpg_init_fpga_defs(); //Comment this line for HW execution
  #endif*/
}


/**
 *  Description: Overrides the planner to inject scanAggr.
 * 
 *               Postgresql doesn't provide its native scanaggr 
 *               hook, so we need to be very careful on modifying 
 *               native planner tree
 *  Called by: planner_hook (postgresql internal)
 */
PlannedStmt *
xpg_planner_entrypoint(Query * parse,
                       int cursorOptions,
                       ParamListInfo boundParams)
{
  PlannedStmt * result;

  if (planner_hook_prev)
    result = planner_hook_prev(parse, cursorOptions, boundParams);
  else
    result = standard_planner(parse, cursorOptions, boundParams);

  if (xpg_enabled) {
    xpg_recursive_plantree_traverse(result, NULL/*parentPlan*/, result->planTree);
  }

  return result;
}

void
xpg_recursive_plantree_traverse(PlannedStmt *pstmt, 
                                Plan * parentPlan,
                                Plan * currPlan)
{
  //if (parentPlan == NULL)
  //  pprint(pstmt);
  if(xpg_codegendebug)
    elog (INFO, "calling xpg_recursive_plantree_traverse");
  switch (nodeTag(currPlan))
  {
    case T_Agg:
      if(xpg_codegendebug) 
        elog(INFO, "inserting agg node");
      //pprint(currPlan);      
      xpgpreagg_insert_preagg(pstmt, (Agg*)currPlan);
      break;

    default:
      break;
  }

  if (currPlan->lefttree)
    xpg_recursive_plantree_traverse(pstmt, currPlan, currPlan->lefttree);
  if (currPlan->righttree)
    xpg_recursive_plantree_traverse(pstmt, currPlan, currPlan->righttree);
}
}
