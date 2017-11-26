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
#include "xpgscan.h"
#include "xpgbolt.h"
#include "xpgcodegen.h"
#include "xpgdatastore.h"
#include "xpg_fpga_defs.h"
#include "xpgocl.h"
#include <sstream>
#include "sqlcmds.h"

extern "C" {
#include "postgres.h"
#include "storage/bufmgr.h"
#include "access/heapam.h"
#include "access/relscan.h"
#include "access/printtup.h"
#include "catalog/pg_namespace.h"
#include "nodes/extensible.h"
#include "nodes/nodes.h"
#include "nodes/relation.h"
#include "optimizer/pathnode.h"
#include "optimizer/paths.h"
#include "optimizer/restrictinfo.h"
#include "utils/lsyscache.h"
#include "utils/rel.h"
}

instr_time overallBeg, overallEnd;

static CustomPathMethods xpg_path_methods;
static CustomScanMethods xpg_scan_methods;
static CustomExecMethods xpg_exec_methods;

/* Postgres Hooks */
static set_rel_pathlist_hook_type set_rel_pathlist_hook_prev;
static int result_count = 0;

void
xpg_init_scan_hooks()
{
  /* Setup xpg scan path methods to create plans*/
  xpg_path_methods.CustomName = "XPGScan";
  /* Callback Prototype: PlanCustomPath(PlannerInfo*, 
   *                                    RelOptInfo*,
   *                                    CustomPath *,
   *                                    List *, List*, List*)
   */
  xpg_path_methods.PlanCustomPath = xpg_plan_scan_path;

  /* Setup xpg scan plan methods to postgres CustomPlanMethods hooks*/
  xpg_scan_methods.CustomName = "XPGScan";
  /* This callback shall be invoked when the core backend tries to populate
   * CustomScanState node according to the input CustomScanNode*/
  /* Prototype: xpgscan_create_scan_state(CustomScan * cscan)*/
  xpg_scan_methods.CreateCustomScanState = xpg_create_scan_state;

  /* Setup xpg scan methods to postgres CustomExecMethods hooks*/
  xpg_exec_methods.CustomName = "XPGScan";
  /* xpg_begin(CustomScanState *, EState*, int eflags) */
  /* xpg_exec(CustomScanState *node) */
  /* xpg_end(CustomScanState *node) */
  /* xpg_rescan(CustomScanState *node) */
  /* xpg_explain(CustomScanState *node) */
  xpg_exec_methods.BeginCustomScan = xpg_begin_scan;
  xpg_exec_methods.ExecCustomScan = xpg_exec_scan;
  xpg_exec_methods.EndCustomScan = xpg_end_scan;
  xpg_exec_methods.ReScanCustomScan = xpg_rescan_scan;
  xpg_exec_methods.ExplainCustomScan = xpg_explain_scan;

  /* Provide custom scan path*/
  /*  This API calls postgres nativ add_path API*/
  set_rel_pathlist_hook_prev = set_rel_pathlist_hook;
  set_rel_pathlist_hook = xpg_add_rel_scan_path;
}

/**
 *  Description: Entry point of custom-scan execution. 
 *               It adds CustomPath if xpg.enabled is turned on
 *  Called by: postgres set_rel_pathlist_hook
 *
 *  Details: 
 */
void
xpg_add_rel_scan_path(PlannerInfo *root,
                      RelOptInfo *baserel,
                      Index rtindex,
                      RangeTblEntry *rte)
{
  CustomPath * pathnode;
  List * device_quals = NULL; /* filters that can run on FPGA */
  List * host_quals = NULL; /* filters that not supported by FPGA */
  ListCell * cell = NULL;

  /* If there is a previous hook, call it first */
  if (set_rel_pathlist_hook_prev)
    set_rel_pathlist_hook_prev(root, baserel, rtindex, rte);


  if (!xpg_enabled)
    return;
  
  if(xpg_codegendebug)
    elog(INFO, "xpgscan enabled!");

  /* For now we only support ordinary relation */
  /* Other relations include: SUBQUERY, JOIN, FUNCTION, etc*/
  if (baserel->rtekind != RTE_RELATION || baserel->relid == 0)
    return;

  /* If the sql command is a system query (e.g. \d+; \dn; etc.)*/
  if (get_rel_namespace(rte->relid) == PG_CATALOG_NAMESPACE)
    return;
      //printf("BEGIN EXPR");
    //pprint(baserel->baserestrictinfo);
    //printf("END EXPR");
    //  temp0_used=false;
    //    temp1_used=false;

  instr_time tBeg, tEnd;
  
  double elapsedTime_ms;
  
  INSTR_TIME_SET_CURRENT(tBeg);
  /* Check if the operators in the scan filters are supported by XPGBolt */
  xpg_initialiseTemp();
  setupstream.setup = 
    (char*)palloc0(sizeof(char) * 2*4096);
  //memset(setupstream.setup, 0, 2 *4096);

  setupstream.length = 0;
  int counter = 0;

  //  ExpressionStack expression_stack;
  bool is_first_cell = true;
  //  bool constraint = false;
  foreach (cell, baserel->baserestrictinfo) {
    RestrictInfo * restrictInfo = (RestrictInfo *) lfirst(cell);
    counter++;
    //elog(INFO,"%d",counter++);
    /*
    // Get first node
    Node *root_node = (Node *)restrictInfo->clause;
    // Generate node graph for this restrict info
    TreeTraversalData tree_traversal_data;
    node_graph_generator(root_node, (void *)&tree_traversal_data);
    // Make sure that all vertexes are processed
    if (!tree_traversal_data.m_processing_vertex_stack.empty()) {
      elog(INFO, "ERROR: processing vertex stack not empty at end of traversal");
    } else {
      // Add to expression stack
      add_to_expression_stack(tree_traversal_data, expression_stack);
      // If this is not the first restrict info, we have to assume that that the previous
      // result was 'ANDed' with the current one
      if (!is_first_cell) {
        // Add AND expression
        ExpressionData expression;
        expression.m_operator_enum = OperatorType::AND;
        expression.m_local_vars.push_back(0);
        expression.m_local_vars.push_back(1);
        expression.m_result_var = 0;
        expression_stack.push_back(expression);
      }
    }
    */
    is_first_cell = false;

    if (xpg_is_expr_supported(restrictInfo->clause,baserel->relid,false))
      device_quals = lappend(device_quals, restrictInfo);
    else
      host_quals = lappend(host_quals, restrictInfo);

    if(returnConstraint()) {
      elog(INFO,"Constraints not supported; returning...\n");
      return;
    }
  }
  // Print expression stack
  /*  for (const ExpressionData &expression : expression_stack) {
    std::stringstream ss;
    ss << expression;
    elog(INFO, "Expression  %s", ss.str().c_str());
  }
  */
  //andTempRegs(); 
  //build_constream(constream);
  //std::string constream_static = "5|-2191|-1826|500|700|240000|\n";
  //addExprVecToString();
  //build_xpgsetupstream(&setupstream,(char*)constream_static.c_str(),qual_stream,exprcount);
  //build_xpgsetupstream(&setupstream,constream,qual_stream,exprcount);

  INSTR_TIME_SET_CURRENT(tEnd);
  
  INSTR_TIME_SUBTRACT(tEnd, tBeg);
  
  elapsedTime_ms = INSTR_TIME_GET_MILLISEC(tEnd);
 
  if(xpg_codegendebug)
    elog(INFO,"Time taken for parsing expressions %f ms \n",elapsedTime_ms);
  
  /* If no filter can be offloaded to device, return early */
  if (device_quals == NULL or returnConstraint())
    return;
  
  /* populate CustomPath struct */
  pathnode = makeNode(CustomPath);
  pathnode->path.pathtype = T_CustomScan;
  pathnode->path.parent = baserel;
  pathnode->path.pathtarget = baserel->reltarget;
  pathnode->path.param_info = 
   get_baserel_parampathinfo(root, baserel, baserel->lateral_relids);
  pathnode->path.pathkeys = NIL; /* unsorted result */
  pathnode->flags = 0;
  pathnode->custom_private = NIL; 
  pathnode->methods = &xpg_path_methods;
  
  /* cost estimation - fill in below fields:
   *      pathnode->path.startup_cost
   *      pathnode->path.total_cost
   *
   * WIP: Sudha is investigating cost function
   */
  xpg_cost_scan_path(pathnode, root, baserel, device_quals, host_quals);
  
  if(xpg_codegendebug)
    elog(INFO, "cost function finished, adding path");

  add_path(baserel, &pathnode->path);
  if(xpg_codegendebug)
    elog (INFO, "Done adding custom scan path\n");
}

/**
 *  Description: Create plan given the new path
 *  Called by: postgres callback function pointer PlanCustomPath
 */
Plan *
xpg_plan_scan_path(PlannerInfo * root,
                   RelOptInfo *rel,
                   CustomPath *best_path,
                   List * tlist,
                   List * clauses,
                   List * custom_plans)
{
  List * xpgscan_quals = best_path->custom_private;
  CustomScan * cscan = makeNode(CustomScan);
  
  if(xpg_codegendebug)
    elog(INFO, "calling xpg_plan_scan_path");


  cscan->flags = best_path->flags;
  cscan->methods = &xpg_scan_methods;

  cscan->scan.scanrelid = rel->relid;
  cscan->scan.plan.targetlist = tlist;
  /* implicitly-ANDed qual conditions */
  cscan->scan.plan.qual = extract_actual_clauses(clauses, false);
  /* exprs that custom node may evaluate */
  cscan->custom_exprs = xpgscan_quals;

  return &cscan->scan.plan;
}

/**
 *  Description: Allocation of XNGScanState, overriding CustomScanState
 *  Called by: postgres callback function pointer CreateCustomScanState
 */
Node *
xpg_create_scan_state(CustomScan * cscan)
{
  FpgaScanState * fpgaScanState = (FpgaScanState *) palloc0(sizeof(FpgaScanState));
  if(xpg_codegendebug)
    elog (INFO, "calling xpg_create_scan_state");

  /* Set exec methods callbacks */
  NodeSetTag(fpgaScanState, T_CustomScanState);
  fpgaScanState->fpgaTaskState.css.flags = cscan->flags;
  fpgaScanState->fpgaTaskState.relid = cscan->scan.scanrelid;

  if (cscan->methods == &xpg_scan_methods)
    fpgaScanState->fpgaTaskState.css.methods = &xpg_exec_methods;
  else
    elog(ERROR, "Unexpected CustomPlanMethods");

  return (Node *) fpgaScanState;
}

/**
 *  Description: Initializes CustomScanState
 *  Called by: CustomScanState.BeginCustomScan callback
 */
void
xpg_begin_scan(CustomScanState *node, EState *estate, int eflags)
{

  result_count = 0;

  HeapScanDesc scanDesc = NULL;
  FpgaScanState * fpgaScanState = (FpgaScanState *) node;
  //CustomScan *cscan = (CustomScan *) node->ss.ps.plan;
  FpgaTaskState * fpgaTaskState = &fpgaScanState->fpgaTaskState;
  if(xpg_codegendebug)
    elog (INFO, "begin scan");
  /* This function populates fpgaScanState */
  INSTR_TIME_SET_CURRENT(overallBeg);
  xpg_initialiseTemp();

  if (fpgaTaskState->css.ss.ss_currentRelation) {
    Relation scan_rel = fpgaTaskState->css.ss.ss_currentRelation;
    scanDesc = heap_beginscan(scan_rel, estate->es_snapshot, 0, NULL);
    fpgaTaskState->css.ss.ss_currentScanDesc = scanDesc;
    TupleDesc desc = scan_rel->rd_att;
    for (int i = 0; i < scan_rel->rd_rel->relnatts; i++ ) {
      FormData_pg_attribute* att;
      att = desc->attrs[(i)];
      addTableColInfo(att->attnum,att->attlen,att->atttypid,0,0);
    }
  }
  
  if (scanDesc != NULL &&
      scanDesc->rs_cblock == InvalidBlockNumber)
    scanDesc->rs_cblock = scanDesc->rs_startblock;
  else if (scanDesc->rs_cblock == scanDesc->rs_startblock)
    return;

  List * test = fpgaTaskState->css.ss.ps.qual;
  ListCell * cell = NULL;
 
  setupstream.setup = 
    (char*)palloc0(sizeof(char) * 2 * 4096);
  //memset(setupstream.setup, 0, 2 * 4096);
  setupstream.length = 0;
  fpgaTaskState->nativeSetupStream.setup = (unsigned char*)malloc (NUM_BYTES);
  fpgaTaskState->nativeSetupStream.setup_ptr =fpgaTaskState->nativeSetupStream.setup;  
  memset (fpgaTaskState->nativeSetupStream.setup, 0, NUM_BYTES);
  
  int counter = 0;

  //  ExpressionStack expression_stack;
  bool is_first_cell = true;
  //  bool constraint = false;
  foreach (cell, test) {
    ExprState * exprstate = (ExprState *) lfirst(cell);
    counter++;
    //elog(INFO,"%d",counter++);
    is_first_cell = false;
    if(xpg_is_expr_supported(exprstate->expr,fpgaScanState->fpgaTaskState.relid,false))
     if(xpg_codegendebug)
       elog(INFO,"xpg_is_expr returned true");
    else
     if(xpg_codegendebug)
       elog(INFO,"xpg_is_expr returned false");
  }
  andTempRegs(); 
  build_constream(constream);
  addExprVecToString();
  //char *p_filterstream = NULL;
  if(xpg_fpgaoffload) {
     //p_filterstream = (char*)malloc (NUM_BYTES);
     buildFilterStream(fpgaTaskState->nativeSetupStream.setup);
     //memcpy(fpgaTaskState->nativeSetupStream.setup,filterstream, 4096);
     if(xpg_codegendebug) 
      elog(INFO,"Done Setting Filter stream\n");
  }
  unsigned char scanType = KERNEL_MODE_SCANONLY;
  
  build_xpgsetupstream(&setupstream,fpgaTaskState->nativeSetupStream.setup + 4096,constream,qual_stream,exprcount,scanType);
  //elog(INFO, "setupstream after setupbuf filterstream %s", setupstream.setup + 4098);

  fpgaTaskState->next_buffer.buf = 
    (char*)palloc0(sizeof(char) * xpg_get_buffer_size());
  fpgaTaskState->prev_buffer.buf = 
    (char*) palloc0(sizeof(char) * xpg_get_buffer_size());
  fpgaTaskState->result_buffer.buf = 
    (char*) palloc0(sizeof(char) * xpg_get_buffer_size());

  bool * bitResult = (bool*)fpgaTaskState->result_buffer.buf;
  memset(bitResult, 1, sizeof(char) * xpg_get_buffer_size());

  //elog(INFO, "next_buffer %d, prev_buffer %d", fpgaTaskState->next_buffer.buf, fpgaTaskState->prev_buffer.buf);

  fpgaTaskState->next_buffer.length = 0;
  fpgaTaskState->prev_buffer.length = 0;
  fpgaTaskState->result_buffer.length = 0;

  fpgaTaskState->next_buffer.numTups = 0;
  fpgaTaskState->prev_buffer.numTups = 0;
  fpgaTaskState->result_buffer.numTups = 0;

  fpgaTaskState->next_cache.cache = 
    (uintptr_t*)palloc0(sizeof(uintptr_t) * xpg_get_buffer_size());
  fpgaTaskState->prev_cache.cache = 
    (uintptr_t*)palloc0(sizeof(uintptr_t) * xpg_get_buffer_size());

  fpgaTaskState->varstream.var = 
    (char*)palloc0(sizeof(char) * xpg_get_buffer_size());
  fpgaTaskState->varstream.length = 0;

  fpgaTaskState->nativestream.blk = 
    (unsigned char*)palloc0(XQL_BLK_SZ);
  fpgaTaskState->nativestream.length = 0;
  fpgaTaskState->nativestream.numTups = 0;
  if (fpgaTaskState->css.ss.ss_currentRelation) {
    Relation rel = fpgaTaskState->css.ss.ss_currentRelation;
    fpgaTaskState->send_state.nBlks = RelationGetNumberOfBlocksInFork (rel, MAIN_FORKNUM);
  }
  fpgaTaskState->send_state.blkNum = 0;
  fpgaTaskState->send_state.linNum = 0;
  fpgaTaskState->send_state.is_first_blk = true;

  fpgaTaskState->receive_state.curr_queue_id = 0;
  fpgaTaskState->receive_state.curr_result_id = 0;
  fpgaTaskState->receive_state.total_queues = 0;

  fpgaScanState->curr_result_id = 0;

  if (!xpg_fpgaoffload)
    if (xpg_fetch_next_block(fpgaTaskState) == false)
      return;

}



/**
 *  Description: Fetching a tuple from the relation, if exists
 *  Called by: CustomScanState.ExecScanState
 */
TupleTableSlot *
xpg_exec_scan(CustomScanState * node)
{
  //elog (INFO, "calling xpg_exec_scan");
  /* Pass in two methods for ExecScan:
   * 1. xpg_exec_fpgaTask: load opcodes and data to fpga
   * 2. xpg_scan_exec_recheck: custom check func to confirm correctness
   *
   */

  TupleTableSlot * tupleSlot = NULL;
  if (!xpg_fpgaoffload)
    tupleSlot = ExecScan(&node->ss,
       (ExecScanAccessMtd)xpg_exec_scan_access_mtd,
       (ExecScanRecheckMtd)xpg_exec_scan_recheck_mtd);
  else
    tupleSlot = ExecScan(&node->ss,
       (ExecScanAccessMtd)xpg_exec_scan_access_ocl_mtd,
       (ExecScanRecheckMtd)xpg_exec_scan_recheck_mtd);

  return tupleSlot;
}

/**
 *  Description: End Fpga scanning
 *  Called by: CustomScanState.EndScanState
 */
void
xpg_end_scan(CustomScanState * css)
{
   FpgaScanState * fpgaScanState = (FpgaScanState *)css;
   FpgaTaskState * fpgaTaskState = &fpgaScanState->fpgaTaskState;

   if(xpg_codegendebug)
    elog(INFO, "calling xpg_end_scan");
   /*WIP*/
   if (fpgaTaskState->css.ss.ss_currentScanDesc)
      heap_endscan(fpgaTaskState->css.ss.ss_currentScanDesc);

   // print overall run time
   INSTR_TIME_SET_CURRENT(overallEnd);
   INSTR_TIME_SUBTRACT(overallEnd, overallBeg);
   double elapsedTime_ms = INSTR_TIME_GET_MILLISEC(overallEnd);
   if (xpg_asciiFlow) {
      printf("\nASCII Flow total time %f ms \n", elapsedTime_ms);
   }
   else {
      printf("\nVARSTREAM Flow total time %f ms \n", elapsedTime_ms);
   }
  if(xpg_codegendebug)  
   elog(INFO, "Total result count is: %d", result_count);

  return;
}

/**
 *  Decsiption: Rewind Fpga scan
 *  Called by : CustomScanState.RescanScanState
 */
void
xpg_rescan_scan(CustomScanState * css)
{
  /*WIP*/
  return;
}

/**
 *  Description: Explain Fpga Custom Scan
 *  Called by: CustomScanState.ExplainScanState
 */
void
xpg_explain_scan(CustomScanState * css,
                 List * ancestors,
                 ExplainState * explainState)
{
  /*WIP*/
  return;
}

/**
 *  Description: Access the heap tuple table and fetch the next chunk of
 *               tuples, and send to FPGA for exec
 *               This Call uses OpenCL APIs
 *  Called by: ExecScan (postgres internal)
 */
TupleTableSlot*
xpg_exec_scan_access_ocl_mtd(FpgaTaskState * fpgaTaskState)
{
  TupleTableSlot * slot = NULL;
  HeapTuple heapTuple;
  FpgaScanState * fpgaScanState = (FpgaScanState*) fpgaTaskState;
  //HeapScanDesc scanDesc;
  xpg_setupstream = setupstream.setup;
  
  //scanDesc = fpgaTaskState->css.ss.ss_currentScanDesc;
  /* Repeatedly fetch from result buffer (sent from device) and store 
   * valid tuple. 
   * If there is no more valid tuple from the result buffer,
   * fetch the next chunk from db and send to fpga for execution */
  bool isBreak = false;
  //elog(INFO, "calling xpg_exec_scan_access_mtd");

  int status = EXIT_SUCCESS;

  while(!(slot = get_next_result_tuple_ocl(fpgaTaskState))) {
    /* Launch fpga kernel on the next chunk of data*/

    /* This loop should break when processQuerySwEmu returns an END state */
    fpgaTaskState->receive_state.curr_queue_id = 0;
    fpgaTaskState->receive_state.curr_result_id = 0;
    if (status == EXIT_FINISH)
      return NULL;
    
    //elog(INFO, "About to call processQuerySwEmu");
    status = processQuerySwEmu(p_xdbConns,
                                   kernels,
                                   fpgaTaskState->nativeSetupStream.setup_ptr,
                                   fpgaTaskState);

    /*if (status == EXIT_FINISH)
      break;*/

  }

  return slot;
}

/**
 *  Description: Access the heap tuple table and fetch the next chunk of
 *               tuples, and send to FPGA for exec
 *  Called by: ExecScan (postgres internal)
 */
TupleTableSlot*
xpg_exec_scan_access_mtd(FpgaTaskState * fpgaTaskState)
{
  TupleTableSlot * slot = NULL;
  HeapTuple heapTuple;
  FpgaScanState * fpgaScanState = (FpgaScanState*) fpgaTaskState;
  //HeapScanDesc scanDesc;
  
  //scanDesc = fpgaTaskState->css.ss.ss_currentScanDesc;
  /* Repeatedly fetch from result buffer (sent from device) and store 
   * valid tuple. 
   * If there is no more valid tuple from the result buffer,
   * fetch the next chunk from db and send to fpga for execution */
  bool isBreak = false;
  //elog(INFO, "calling xpg_exec_scan_access_mtd");

  while(!(slot = get_next_result_tuple(fpgaTaskState))) {
    /* Launch fpga kernel on the next chunk of data*/

    /**
     *  TEMP: This is the place where the next chunk of VAR should be read in
     *  and sent to FPGA for execution
     */     
     if (isBreak == true)
      return NULL;

    if (xpg_receive_results(fpgaTaskState) == false)
      return NULL;

    fpgaScanState->curr_result_id = 0;

    if (xpg_fetch_next_block(fpgaTaskState) == false)   {
      //elog(INFO, "returning NULL next iter");
      isBreak = true;
    }

    //slot = fpgaTaskState->css.ss.ss_ScanTupleSlot;
    //heapTuple = &(fpgaTaskState->tupleBuffer.heapTuple);

      //ExecStoreTuple(heapTuple, slot, InvalidBuffer, false);

  }
  return slot;
}

/**
 *  Description : Read the next available tuple from result buffer, if any
 *  Called by: xpg_exec_scan_access_ocl_mtd
 */

TupleTableSlot *
get_next_result_tuple_ocl(FpgaTaskState * fpgaTaskState) {
  /* fpgaTaskState keeps a pointer to result buffer and the index of the recently
   * fetched resut. 
   * 
   * This function will fetch the entry from the next index and increment the index
   */
/*  if (xpg_timerOn) {
     INSTR_TIME_SET_CURRENT(receiveEnd);
     INSTR_TIME_SUBTRACT(receiveEnd, receiveBeg);
     xpg_receiveTime = INSTR_TIME_GET_MILLISEC(receiveEnd);
     //printf("recieve time %f ms \n", xpg_receiveTime);
     elog(INFO,"block process time %f (send: %f + scan: %f + receive:%f) ms \n", 
        (xpg_datasendTime + xpg_scanTime + xpg_receiveTime),
        xpg_datasendTime, xpg_scanTime, xpg_receiveTime);

  }*/
  //return NULL;
  TupleTableSlot * slot = NULL;
  FpgaScanState * fpgaScanState = (FpgaScanState*)fpgaTaskState;

  int currQueueId = fpgaTaskState->receive_state.curr_queue_id;
  for (int qid = currQueueId; qid < fpgaTaskState->receive_state.total_queues; qid++) {
    bool * bitResults = (bool*)xpg_buffer_container.list[qid].result_buffer.buf;
    //elog(INFO, "getting next result tuple for queue %d, total tuples %d", qid, xpg_buffer_container.list[qid].result_buffer.numTups);
    int id;
    for (id = fpgaTaskState->receive_state.curr_result_id; 
        id < xpg_buffer_container.list[qid].result_buffer.numTups;
        id++){ 
      //elog(INFO, "result id %d in queue %d of %d, result %d", id, qid, xpg_buffer_container.list[qid].result_buffer.numTups, bitResults[id]);
      if (bitResults[id] == false)
        continue;
      //elog(INFO, "result id %d in queue %d of %d, result %d", id, qid, xpg_buffer_container.list[qid].result_buffer.numTups, bitResults[id]);

      XpgTuple * xpgTup = (XpgTuple*)(xpg_buffer_container.list[qid].tuple_cache.cache[id]);


      HeapTuple tuple = &fpgaScanState->scan_tuple;
      tuple->t_len = xpgTup->t_len;
      tuple->t_self = xpgTup->t_self;
      tuple->t_data = &xpgTup->t_data;

      slot = fpgaTaskState->css.ss.ss_ScanTupleSlot;

      fpgaTaskState->receive_state.curr_result_id = ++id;
      fpgaTaskState->receive_state.curr_queue_id = qid;
      ExecStoreTuple(tuple, slot, InvalidBuffer, false);

      //xpg_print_attr(tuple, fpgaTaskState);

      result_count++;
      return slot;

    }
    fpgaTaskState->receive_state.curr_result_id = 0;
    xpg_buffer_container.list[qid].result_buffer.numTups = 0;
  }
  //elog(INFO, "Done with all queues, return NULL ");
  fpgaTaskState->receive_state.total_queues = 0;

  if (xpg_timerOn && xpg_codegendebug) {
     INSTR_TIME_SET_CURRENT(receiveEnd);
     INSTR_TIME_SUBTRACT(receiveEnd, receiveBeg);
     xpg_receiveTime = INSTR_TIME_GET_MILLISEC(receiveEnd);
     //printf("recieve time %f ms \n", xpg_receiveTime);
     elog(INFO,"block process time %f (send: %f + scan: %f + receive:%f) ms \n", 
        (xpg_datasendTime + xpg_scanTime + xpg_receiveTime),
        xpg_datasendTime, xpg_scanTime, xpg_receiveTime);

  }

  return NULL;
}

/**
 *  Description : Read the next available tuple from result buffer, if any
 *  Called by: xpg_exec_scan_access_mtd
 */

TupleTableSlot *
get_next_result_tuple(FpgaTaskState * fpgaTaskState) {
  /* fpgaTaskState keeps a pointer to result buffer and the index of the recently
   * fetched resut. 
   * 
   * This function will fetch the entry from the next index and increment the index
   */

  TupleTableSlot * slot = NULL;
  FpgaScanState * fpgaScanState = (FpgaScanState*)fpgaTaskState;
  bool * bitResults = (bool*)fpgaTaskState->result_buffer.buf;

  //elog(INFO, "getting next result tuple, total tuples %d", fpgaTaskState->result_buffer.numTups);

  int id;
  for (id = fpgaScanState->curr_result_id; 
       id < fpgaTaskState->result_buffer.numTups;
       id++) {
    if (bitResults[id] == false)
      continue;

    //elog(INFO, "result id %d of %d, result %d", id, fpgaTaskState->result_buffer.numTups, bitResults[id]);
    XpgTuple * xpgTup = (XpgTuple*)(fpgaTaskState->prev_cache.cache[id]);

    HeapTuple tuple = &fpgaScanState->scan_tuple;
    tuple->t_len = xpgTup->t_len;
    tuple->t_self = xpgTup->t_self;
    tuple->t_data = &xpgTup->t_data;

    slot = fpgaTaskState->css.ss.ss_ScanTupleSlot;

    fpgaScanState->curr_result_id = ++id;

    ExecStoreTuple(tuple, slot, InvalidBuffer, false);

    //xpg_print_attr(tuple, fpgaTaskState);

    result_count++;
    return slot;

  }
  if (xpg_timerOn && xpg_codegendebug) {
     INSTR_TIME_SET_CURRENT(receiveEnd);
     INSTR_TIME_SUBTRACT(receiveEnd, receiveBeg);
     xpg_receiveTime = INSTR_TIME_GET_MILLISEC(receiveEnd);
     //printf("recieve time %f ms \n", xpg_receiveTime);
     printf("block process time %f (send: %f + scan: %f + receive:%f) ms \n", 
        (xpg_datasendTime + xpg_scanTime + xpg_receiveTime),
        xpg_datasendTime, xpg_scanTime, xpg_receiveTime);

  }

  return NULL;
}

/**
 *  Description: Rewind to CPU scan if fpgaScan failed
 *  Called by: ExecScan (postgres internal)
 */
bool
xpg_exec_scan_recheck_mtd(CustomScanState * css, TupleTableSlot * slot)
{
  return true;
}

/**
 *  Description: Estimate the cost of custom scan path
 *  Called by: xpg_add_rel_scan_path
 */
void 
xpg_cost_scan_path(CustomPath * cPath, 
                   PlannerInfo * root,
                   RelOptInfo * baserel, 
                   List * device_quals,
                   List * host_quals)
{
  Path * path = &cPath->path;
  Cost startup_cost = 0;
  Cost run_cost = 0;
  if(xpg_codegendebug)
    elog(INFO, "calling xpg_cost_scan_path");

  path->rows = baserel->rows;

  /* TEMP: always pick XPG scan */
  startup_cost += baserel->baserestrictcost.startup;
  run_cost = 0;


  path->startup_cost = startup_cost;
  path->total_cost = startup_cost + run_cost;

  //pprint(path);
}
