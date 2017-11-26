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
//#include "postgres.h"

#include "xpgdatastore.h"

#include "xpgbolt.h"
#include "xpgnumeric.h"
#include "xpgdate.h"
#include "xpgvarattrdetails.h"
#include "xpg_fpga_defs.h"
extern "C" {
#include "access/heapam.h"
#include "access/relscan.h"
#include "common/relpath.h"
#include "storage/block.h"
#include "storage/bufmgr.h"
#include "utils/rel.h"
#include "utils/builtins.h"
#include "funcapi.h"
}
#include "xpgocl.h"

#include "xpgcodegen.h"
#include "xpg_fpga_csim.h"
//#include "xpgoperators.h"
extern "C" {
#include "catalog/pg_operator.h"
#include "catalog/pg_namespace.h"
#include "catalog/pg_type.h"
#include "access/sysattr.h"
#include "optimizer/clauses.h"
#include "lib/stringinfo.h"
#include <nodes/nodes.h>
#include "nodes/relation.h"
#include "nodes/value.h"
#include "utils/builtins.h"
#include "utils/date.h"
#include "utils/lsyscache.h"
#include "utils/guc.h"
#include "utils/numeric.h"
#include "utils/rel.h"
#include "utils/syscache.h"
}

//----------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------
// logging related
static const char xql6Log[] = "./xql62.log";
static FILE *hlogFile = NULL;
void hlog (const char* fmt, ...)
{
  if (hlogFile) {
    va_list args;
    va_start (args, fmt);
    vfprintf (hlogFile, fmt, args);
    va_end (args);
    fflush (hlogFile);
  }
}
//----------------------------------------------------------------------------

#ifndef SQLENGINE_DECIMAL_H
#include "decimal.h"
#endif
/**
 *  Description:
 *  Fetch next chunk of blocks and store in XPG container
 *  
 *  Called by: xpg_exec_scan_access_mtd
 */
/*void convertdate_to_string(Datum data) {
  elog(INFO,"convertdate_to_string");
  DateADT     date;
  struct pg_tm tm;
  char        buf[MAXDATELEN + 1];
  date = DatumGetDateADT(data);
  elog(INFO,"argBo-consttype %d ",date);
  // Same as date_out(), but forcing DateStyle
  if (DATE_NOT_FINITE(date))
    EncodeSpecialDate(date, buf);
  else
    {
      j2date(date + POSTGRES_EPOCH_JDATE,
	     &(tm.tm_year), &(tm.tm_mon), &(tm.tm_mday));
      EncodeDateOnly(&tm, USE_XSD_DATES, buf);
    }
  elog(INFO,"arg2-consttype %s ",buf);
  //  printf("-%s-",buf);
}*/

/**
 *  Defines size of var stream
 *  Size = Size(int) << VAR_STREAM_SIZE
 *  (e.g. if 10, then stream size = 4KB)
 */
#define XPG_BUFFER_SIZE 10

static int xpg_page_size_kb;

/**
 *  Description:
 *  Utility function - returning length of the var stream buffer size
 */
Size
xpg_get_varstream_size(void)
{
  return ((Size)xpg_page_size_kb << XPG_BUFFER_SIZE);
}

Size
xpg_get_buffer_size(void)
{
  return ((Size)xpg_page_size_kb << XPG_BUFFER_SIZE);
}

int xpg_get_next_page (Relation rel, unsigned char* p_buf, FpgaTaskState * fpgaTaskState)
{
  XpgSendState * sendState = &fpgaTaskState->send_state;
  int currQueueId = sendState->curr_queue_id;
  XpgBuffer * tupBuffer = &(xpg_buffer_container.list[currQueueId].tup_buffer);
  XpgTupleCache * tupCache = &(xpg_buffer_container.list[currQueueId].tuple_cache);
  //printf("currQueue %d, tupBuffer address %p, buf address %p\n", currQueueId, (void*)tupBuffer, (void*) (tupBuffer->buf));

  unsigned char *p_dst = p_buf;
  unsigned char *p_dstEnd = p_buf + XQL_PAGE_SZ;
  int nTups = 0;
  bool bFilled = false;

  p_dst += 2;

  int blkNum, linNum;
  Size tupHeaderSize = sizeof(uint16) + sizeof(ItemPointerData);

  for (blkNum = sendState-> blkNum; blkNum < sendState-> nBlks; ++blkNum) {

    Buffer buf = ReadBufferExtended(rel, MAIN_FORKNUM, blkNum, RBM_NORMAL, NULL); 
    LockBuffer(buf, BUFFER_LOCK_SHARE);

    Page page = (Page) BufferGetPage (buf);
    int nLines = PageGetMaxOffsetNumber (page);

    //printf ("blkNum %d, nLines %d, pfsLinNum %d\n", blkNum, nLines, sendState-> linNum);

    linNum = sendState-> linNum;
    sendState-> linNum = 0;
    for ( ; linNum <= nLines; ++linNum) {
      ItemId id = PageGetItemId (page, linNum);
      //uint16 lp_offset = ItemIdGetOffset (id);
      uint16 lp_len = ItemIdGetLength(id);

      HeapTupleHeader tuphdr = (HeapTupleHeader) PageGetItem(page, id);
      int tuple_data_len = lp_len - tuphdr->t_hoff;
      if (tuple_data_len == 0) {
        continue;
      }

      if (p_dst + tuple_data_len < p_dstEnd) {
        memcpy (p_dst, (unsigned char*) tuphdr + tuphdr->t_hoff, tuple_data_len);

        /********/

        Size size = (lp_len + tupHeaderSize);
        if (tupBuffer->length + size > xpg_get_buffer_size()) {
          //printf("ERRORRRR!!!\n");
          elog(INFO, "blkNum %d, nLines %d, pfsLinNum %d\n", blkNum, nLines, sendState-> linNum);
          elog(INFO, "currQueue %d, tupBuffer address %p, buf address %p, lenght: %d, size: %d,xpg_get_buffer_size: %d \n", currQueueId, (void*)tupBuffer, (void*) (tupBuffer->buf), tupBuffer->length, size, xpg_get_buffer_size());
          elog(ERROR, "The tupBuffer size is full and fpgaTaskState->next_cache will be incorrect");
        }

        //printf("full tup size %d, modified size %d tupBufLength %d, nativestr length %d\n", 
         //    size, tuple_data_len,
           //  tupBuffer->length, p_dst-p_buf);

        XpgTuple * xpgTup;
        xpgTup = (XpgTuple*) ((char*) tupBuffer->buf + tupBuffer->length);
        xpgTup->t_len = lp_len;
        ItemPointerSet(&xpgTup->t_self, blkNum, linNum);

//        memcpy(&(xpgTup->t_data), (HeapTupleHeader) PageGetItem((Page)page, id), xpgTup->t_len);
        memcpy(&(xpgTup->t_data), (HeapTupleHeader) tuphdr, xpgTup->t_len);

        int id = tupBuffer->numTups;
        //printf ("SREERESMEMCPY for id : %d\n", id);
        //fpgaTaskState->next_cache.cache[id] = (uintptr_t)xpgTup;
        tupCache->cache[id] = (uintptr_t)xpgTup;

        tupBuffer->length += size;
        tupBuffer->numTups++; 
        /********/

        p_dst += tuple_data_len;
        ++ nTups;

      } else {
        bFilled = true;
        break;
      }
    }

    LockBuffer(buf, BUFFER_LOCK_UNLOCK);
    ReleaseBuffer(buf);

    if (bFilled) break;
  }

  p_buf [0] = (unsigned char) (nTups & 0xFF);
  p_buf [1] = (unsigned char) (nTups >> 8 & 0xFF);
  sendState->blkNum = blkNum;
  sendState->linNum = linNum;

  //printf ("xql6GetNextPage blkNum %d linNum %d\n", blkNum, linNum);

  return nTups;
}

int xpg_fill_block (Relation rel, unsigned char *p_bufptr,
                    FpgaTaskState * fpgaTaskState)
{
  bool bRealPages = true;
  int pgNum;

  /* Reset buffers */
  XpgSendState * sendState = &fpgaTaskState->send_state;
  XpgBuffer * tupBuffer = 
    &(xpg_buffer_container.list[sendState->curr_queue_id].tup_buffer);
  tupBuffer->length = 0;
  tupBuffer->numTups = 0;
  
  XpgBuffer * resultBuffer = 
    &(xpg_buffer_container.list[sendState->curr_queue_id].result_buffer);
  resultBuffer->numTups = 0;
          //elog(INFO, "currQueue %d, tupBuffer address %p, buf address %p, lenght: %d, xpg_get_buffer_size: %d \n", sendState->curr_queue_id, (void*)tupBuffer, (void*) (tupBuffer->buf), tupBuffer->length, xpg_get_buffer_size());

  for (pgNum = 0; pgNum < XQL_PAGES_PER_BLK; ++pgNum) {
    int pgTups = xpg_get_next_page (rel, p_bufptr, fpgaTaskState);
    resultBuffer->numTups += pgTups;

    p_bufptr += XQL_PAGE_SZ;

  }
          //elog(INFO, "currQueue %d, tupBuffer address %p, buf address %p, lenght: %d, xpg_get_buffer_size: %d \n", sendState->curr_queue_id, (void*)tupBuffer, (void*) (tupBuffer->buf), tupBuffer->length, xpg_get_buffer_size());

  //printf ("xql6FillBlock done. %d pages\n", pgNum);
  tupBuffer->length = 0;

  return tupBuffer->numTups;
  //return resultBuffer->numTups;
}

bool
xpg_fetch_next_block(FpgaTaskState * fpgaTaskState)
{
  //elog(INFO, "calling xpg_fetch_next_chunk");
  Relation rel;
  HeapScanDesc scanDesc;

  rel = fpgaTaskState->css.ss.ss_currentRelation;
  scanDesc = fpgaTaskState->css.ss.ss_currentScanDesc;

  XpgBuffer * buffer;
  buffer = &(fpgaTaskState->next_buffer);
  buffer->length = 0;
  buffer->numTups = 0;

  /* reset varstream */

  if (scanDesc->rs_cblock >= scanDesc->rs_nblocks)
    return false;

  while(1) {
    bool isBreak = false;
    if (!xpg_insert_block(fpgaTaskState, buffer, rel, 
          scanDesc->rs_cblock,
          fpgaTaskState->line_offset,
          scanDesc->rs_strategy, 
          &fpgaTaskState->line_offset)) {
       isBreak = true;
    }

    if (!isBreak && (fpgaTaskState->line_offset == FirstOffsetNumber)) {
      scanDesc->rs_cblock++;
      // temp, currently read one tuple every time
      //break;
    }
    if (scanDesc->rs_cblock >= scanDesc->rs_nblocks)
      break;

    if(isBreak) 
      break;
  }
  return true;
}


/**
 *  Description: 
 *  Insert the next block of tuples from PG database to datastore
 *  Returns false if the datastore is full
 *  
 *  Called by: xpg_fetch_next_chunk
 */
extern "C" bool
xpg_insert_block(FpgaTaskState * fpgaTaskState, 
                 XpgBuffer * tupBuffer, 
                 Relation rel,
                 BlockNumber blknum,
                 OffsetNumber startOffsetNum,
                 BufferAccessStrategy strategy, 
                 OffsetNumber * stopOffsetNum)
{
  Buffer buffer;
  Page page;
  int lines;
  OffsetNumber lineoff;
  ItemId lpp;

  buffer = ReadBufferExtended(rel, MAIN_FORKNUM, blknum, RBM_NORMAL, strategy);

  heap_page_prune_opt(rel, buffer);

  page = (Page) BufferGetPage(buffer);
  lines = PageGetMaxOffsetNumber(page);
  //elog(INFO, "lines: %d\n", lines);

  Size tupHeaderSize = sizeof(uint16) + sizeof(ItemPointerData);

  for (lineoff = startOffsetNum, lpp = PageGetItemId(page, lineoff); 
      lineoff <= lines;
      lineoff++, lpp++) {
    XpgTuple * xpgTup;
    Datum datum;
    bool isNull;
    Size accumSize = 0;

    uint16 tupleLen = ItemIdGetLength(lpp);

    if (!ItemIdIsNormal(lpp))
      continue;
    //elog (INFO, "starting address is 0x%lx", (HeapTupleHeader) PageGetItem((Page)page, lpp));

    //elog(INFO, "curr size is %d, limit is %d", tupBuffer->length, xpg_get_buffer_size());

    Size size = (tupleLen + tupHeaderSize);

    if (tupBuffer->length + size > xpg_get_buffer_size()) {
      *stopOffsetNum = lineoff;
      break;
    }

    xpgTup = (XpgTuple*) ((char*) tupBuffer->buf + tupBuffer->length);
    xpgTup->t_len = tupleLen;
    ItemPointerSet(&xpgTup->t_self, blknum, lineoff);

    memcpy(&(xpgTup->t_data), (HeapTupleHeader) PageGetItem((Page)page, lpp), xpgTup->t_len);

    int id = tupBuffer->numTups;
    fpgaTaskState->next_cache.cache[id] = (uintptr_t)xpgTup;


    //elog(INFO, "header %d cur size is %d , limit %d", ((char*)(&xpgTup->t_data) - (char*)(xpgTup)), tupBuffer->length,  xpg_get_buffer_size());


    tupBuffer->length += size;
    tupBuffer->numTups++;

    //elog(INFO, "size is %d, total size is %d", size, tupBuffer->length);

    // temp - read one tuple every time
    *stopOffsetNum = lineoff + 1;
    // end temp
  }

  ReleaseBuffer(buffer);

  /* If stopOffsetNum exceeds lines, meaning it finished one block
   * then return true
   */
  if (*stopOffsetNum > lines) {
    *stopOffsetNum = FirstOffsetNumber;
    return true;
  }

  // temp, will return true
  return false;
}

bool
xpg_receive_results(FpgaTaskState * fpgaTaskState)
{
  // now send the buffer to c model
  //
  //elog(INFO, "calling xpg_receive_results");

  /* push next_buffer to prev_buffer 
   * Use prev_buffer as next_buffer for next fetch*/
  XpgBuffer temp = fpgaTaskState->prev_buffer;
  fpgaTaskState->prev_buffer = fpgaTaskState->next_buffer;
  fpgaTaskState->next_buffer = temp;

  /* Flush next buffer to prepare for next block of tuples*/
  fpgaTaskState->next_buffer.length = 0;

  XpgTupleCache tempCache = fpgaTaskState->prev_cache;
  fpgaTaskState->prev_cache = fpgaTaskState->next_cache;
  fpgaTaskState->next_cache = tempCache;

  fpgaTaskState->result_buffer.numTups = 
    fpgaTaskState->prev_buffer.numTups;

  instr_time dataMoveBeg, dataMoveEnd;
  instr_time scanBeg, scanEnd;

  if (xpg_fpgaoffload) {
    static long xql6kernel_sum = 0;
    if (xpg_timerOn) {
      INSTR_TIME_SET_CURRENT(scanBeg);
    }
     XpgVarStream * varstream = &fpgaTaskState->varstream;
    // if(xpg_fpgaoffload) {
    processQuerySwEmu(p_xdbConns,
                      kernels,
                      fpgaTaskState->nativeSetupStream.setup_ptr, 
                      fpgaTaskState);
    //}
    if (xpg_timerOn) {
      INSTR_TIME_SET_CURRENT(scanEnd);
        INSTR_TIME_SUBTRACT(scanEnd, scanBeg);
        xpg_scanTime = INSTR_TIME_GET_MILLISEC(scanEnd);
      //printf("scan time %f ms \n", xpg_scanTime);
    }
    printf("SREE: xql6kernel_sum : %ld\n", xql6kernel_sum);
  } 
  else if (xpg_asciiFlow) { /* populate varstream */
     if (xpg_timerOn) {
        INSTR_TIME_SET_CURRENT(dataMoveBeg);
     }
     xpg_populate_asciistream(fpgaTaskState);
     if (xpg_timerOn) {
        INSTR_TIME_SET_CURRENT(dataMoveEnd);
        INSTR_TIME_SUBTRACT(dataMoveEnd, dataMoveBeg);
        xpg_datasendTime = INSTR_TIME_GET_MILLISEC(dataMoveEnd);
        //printf("datasend time %f ms \n", xpg_datasendTime);
     }
     //******************
     /* Call C-Model uncomment this and remove the return true above*/
     XpgVarStream * varstream = &fpgaTaskState->varstream;
     //char resultStr[fpgaTaskState->prev_buffer.numTups];
     //char *result = resultStr ;
     //std::string setupstr = "7|-2191|-1826|500|700|240000|-504|10000|\n9|5|1|2|2|0|0|2|2|0|0|4|1|2|2|0|1|2|2|1|0|0|2|0|0|2|1|0|2|0|0|5|1|1|1|0|2|1|2|1|0|0|2|0|0|2|1|0|2|0|0|6|1|1|1|0|3|1|2|1|0|0|2|0|0|2|1|0|2|0|0|4|1|0|1|0|4|1|2|1|0|0|2|0|0|2|1|0|2|0|0|\n";
     //xpg_setupstream = (char*)setupstr.c_str();
     xpg_setupstream = setupstream.setup;
     //printf("Setupstream: %s\n",xpg_setupstream);
     if (xpg_timerOn) {
        INSTR_TIME_SET_CURRENT(scanBeg);
     }
     if(xpg_fpgaoffload) {
       //printf("Running ProcessQuery...\n");

       /*TEMP COMMENT OUT*/
       //processQuery(p_xdbConns,kernels,tpchRunMode,xpg_setupstream,varstream->var,fpgaTaskState->prev_buffer.numTups,fpgaTaskState->result_buffer.buf);
     }
     else {
     processCscan(xpg_setupstream,
                 varstream->var,
                 fpgaTaskState->prev_buffer.numTups,
                 fpgaTaskState->result_buffer.buf,
                 8, 1, 6, false);
     }
     free(varstream->var);
     if (xpg_timerOn) {
        INSTR_TIME_SET_CURRENT(scanEnd);
        INSTR_TIME_SUBTRACT(scanEnd, scanBeg);
        xpg_scanTime = INSTR_TIME_GET_MILLISEC(scanEnd);
        //printf("scan time %f ms \n", xpg_scanTime);
     }
  }
  else {
     if (xpg_timerOn) {
        INSTR_TIME_SET_CURRENT(dataMoveBeg);
     }
     xpg_populate_varstream(fpgaTaskState);
     if (xpg_timerOn) {
        INSTR_TIME_SET_CURRENT(dataMoveEnd);
        INSTR_TIME_SUBTRACT(dataMoveEnd, dataMoveBeg);
        xpg_datasendTime = INSTR_TIME_GET_MILLISEC(dataMoveEnd);
        //printf("datasend time %f ms \n", xpg_datasendTime);
     }
     //******************
     /* Call C-Model uncomment this and remove the return true above*/
     XpgVarStream * varstream = &fpgaTaskState->varstream;
     xpg_setupstream = setupstream.setup;
     if (xpg_timerOn) {
        INSTR_TIME_SET_CURRENT(scanBeg);
     }

     processCscanNew(xpg_setupstream,
        varstream->var,
        fpgaTaskState->prev_buffer.numTups,
        fpgaTaskState->result_buffer.buf,
        8, 1, 6, false);
     //free(varstream->var);
     if (xpg_timerOn) {
        INSTR_TIME_SET_CURRENT(scanEnd);
        INSTR_TIME_SUBTRACT(scanEnd, scanBeg);
        xpg_scanTime = INSTR_TIME_GET_MILLISEC(scanEnd);
        //printf("scan time %f ms \n", xpg_scanTime);
     }
  }
  
  //printf("datamover + scan time %f ms \n", elapsedTime_ms);

  if (xpg_timerOn) {
     INSTR_TIME_SET_CURRENT(receiveBeg);
  }

  return true;
}
static int row = 0;
void
xpg_populate_asciistream(FpgaTaskState * fpgaTaskState)
{
   int tupId;
   HeapTupleData htupData;
   TupleDesc tupleDesc = fpgaTaskState->css.ss.ss_ScanTupleSlot->tts_tupleDescriptor;
   XpgVarStream * varstream = &fpgaTaskState->varstream;
   varstream->length = 0;
   bool isNull;
   Datum datum;

   int varAttrDetailsId;
   int varAttrDetailsSize = xpg_get_num_attr();
   std::string dataStr = "";
   for (tupId = 0; tupId < fpgaTaskState->prev_buffer.numTups; tupId++) {
      row++;
      XpgTuple * xpgTup = (XpgTuple*)(fpgaTaskState->prev_cache.cache[tupId]);

      htupData.t_len = xpgTup->t_len;
      htupData.t_self = xpgTup->t_self;
      htupData.t_data = &xpgTup->t_data;

      for (varAttrDetailsId = 0; varAttrDetailsId < varAttrDetailsSize; varAttrDetailsId++) {
         XpgVarAttrDetails varAttrDetails = xpg_get_nth_attr(varAttrDetailsId);
         int attrId = varAttrDetails.colId;
         Oid attrType = varAttrDetails.colType;
         //printf("ROW: %d\n", row);

         datum = heap_getattr(&htupData, attrId, tupleDesc, &isNull);
         //printf("datum to ascii...\n"); 
         if (xpg_store_datum_to_asciistream(datum, attrType, varstream, dataStr) == false)
            elog(INFO, "not able to store datum to varstream");
         //printf("datum to ascii end..\n");
      }
   }
   size_t datalen = dataStr.length();
   varstream->var = (char *)malloc(sizeof(char) * datalen + 1);
   memcpy(varstream->var, dataStr.c_str(), datalen);
   varstream->var[datalen] = '\0';
}
void
xpg_populate_varstream(FpgaTaskState * fpgaTaskState)
{
  int tupId;
  HeapTupleData htupData;
  TupleDesc tupleDesc = fpgaTaskState->css.ss.ss_ScanTupleSlot->tts_tupleDescriptor;
  XpgVarStream * varstream = &fpgaTaskState->varstream;
  varstream->length = 0;
  bool isNull;
  Datum datum;

  int varAttrDetailsId;
  int varAttrDetailsSize = xpg_get_num_attr();

  for (tupId = 0; tupId < fpgaTaskState->prev_buffer.numTups; tupId++) {

    XpgTuple * xpgTup = (XpgTuple*)(fpgaTaskState->prev_cache.cache[tupId]);

    htupData.t_len = xpgTup->t_len;
    htupData.t_self = xpgTup->t_self;
    htupData.t_data = &xpgTup->t_data;

    for (varAttrDetailsId = 0; varAttrDetailsId < varAttrDetailsSize; varAttrDetailsId++) {
      XpgVarAttrDetails varAttrDetails = xpg_get_nth_attr(varAttrDetailsId);
      int attrId = varAttrDetails.colId;
      Oid attrType = varAttrDetails.colType;

      datum = heap_getattr(&htupData, attrId, tupleDesc, &isNull);

      if (xpg_store_datum_to_varstream(datum, attrType, varstream) == false)
        elog(INFO, "not able to store datum to varstream");
    }
  }
}

bool
xpg_store_datum_to_varstream(Datum datum,
                             Oid attrType,
                             XpgVarStream * varstream)
{
  bool success = true;
  switch (attrType) {
    case NUMERICOID:
      success = xpg_store_numeric_to_varstream(datum, varstream);
      break;

    case DATEOID:
      success = xpg_store_date_to_varstream(datum, varstream);
      break;

    default:
      break;
  }

  return success;
}

bool
xpg_store_datum_to_asciistream(Datum datum,
                               Oid attrType,
                               XpgVarStream * varstream,
                               std::string &dataStr)
{
   bool success = true;
   switch (attrType) {
   case NUMERICOID:
      success = xpg_store_numeric_to_asciistream(datum, varstream, dataStr);
      break;
   case DATEOID:
      success = xpg_store_date_to_asciistream(datum, varstream, dataStr);
      break;
   default:
      break;
   }

   return success;
}
void
xpg_print_datum(Datum datum,
                Oid attrType)
{
  switch (attrType) {
    case NUMERICOID:
      xpg_print_numeric(datum);
      break;

    case DATEOID:
      xpg_print_date(datum);
      break;

    default:
      break;
  }

  return;
}

void
xpg_print_attr(HeapTuple tuple,
                FpgaTaskState * fpgaTaskState)
{

  TupleDesc tupleDesc = fpgaTaskState->css.ss.ss_ScanTupleSlot->tts_tupleDescriptor;
  bool isNull;
  int varAttrDetailsId;
  int varAttrDetailsSize = xpg_get_num_attr();
  Datum datum;

  for (varAttrDetailsId = 0; varAttrDetailsId < varAttrDetailsSize; varAttrDetailsId++) {
    XpgVarAttrDetails varAttrDetails = xpg_get_nth_attr(varAttrDetailsId);
    int attrId = varAttrDetails.colId;
    Oid attrType = varAttrDetails.colType;

    datum = heap_getattr(tuple, attrId, tupleDesc, &isNull);

    xpg_print_datum(datum, attrType);
  }
}

void
xpg_init_datastore(void)
{
  DefineCustomIntVariable(
      "xpg.page_size",
      "Size for xpg datastore",
      NULL,
      &xpg_page_size_kb,
      4096, /*default*/
      2048, /*min*/
      MAX_KILOBYTES, /*max*/
      PGC_USERSET,
      GUC_NOT_IN_SAMPLE | GUC_UNIT_KB,
      NULL,
      NULL,
      NULL);


  // TEST: hardcode var attr list for query 6
  //xpg_init_query6();

  // TEST: hardcode var attr list for mini table
  // col1: integer; col2: numeric
  //xpg_init_minitable();
}
