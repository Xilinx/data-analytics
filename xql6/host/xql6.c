//----------------------------------------------------------------------------
// xql6: UDF to package tuple data for xql6-kernel
// 2017-06-11 22:25:39 parik 
//
// Touch and og+w the log files to get debug logs
//----------------------------------------------------------------------------
#include "postgres.h"
#include "fmgr.h"
#include "storage/bufpage.h"

#include "access/htup_details.h"
#include "catalog/catalog.h"
#include "catalog/namespace.h"
#include "catalog/pg_type.h"
#include "catalog/pg_class.h"

#include "storage/bufmgr.h"
#include "storage/checksum.h"
#include "utils/builtins.h"
#include "utils/pg_lsn.h"
#include "utils/rel.h"

#include "access/relscan.h"
#include "access/heapam.h"
#include "utils/snapmgr.h"
#include "utils/lsyscache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "xql6.h"
#include <CL/opencl.h>

PG_MODULE_MAGIC;

void xql6_init ();
bool xql6_run_hw (Relation, char*, const char*);
bool xql6_run_sw (Relation, char*);
bool xql6_init_kernel (const char*, char*);
bool xql6_stop_kernel (char*);
bool xql6_check_relation (Relation rel);

bool xql6FillBlock (Relation, unsigned char*, long*, Xql6FillState*);
int xql6GetNextPage (Relation, unsigned char*, Xql6FillState*);

static char* p_fpgaKernelName = NULL;

//----------------------------------------------------------------------------
extern unsigned char* pInp[10];
extern unsigned char* pOut[10];
extern cl_event rdEvent [10];

extern long q6_swemu (unsigned char *p_blk);
extern int fpga_q6_enq_compute (int bufId);
extern int start_fpga (const char *xclbin);
extern int end_fpga();

//----------------------------------------------------------------------------
// logging related. Create a writable file with this name to get debug logs
static const char xql6Log[] = "/tmp/xql6.log";
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

long diffTime (struct timespec *tBeg, struct timespec *tEnd)
{
  return ((tEnd-> tv_sec - tBeg-> tv_sec) * 1000000000L + tEnd-> tv_nsec - tBeg-> tv_nsec);
}

//----------------------------------------------------------------------------


PG_FUNCTION_INFO_V1 (xql6);

/*
 * \brief Entry for Postgres UDF. Executes TPCH Query 6 and returns the result
 * as a string
 *
 * UDF Args:
 *  ARGS[0] - Relname
 *  ARGS[1] - Kernel filename or command to kernel. 
 *          - 'fpga_q6.xclbin' : start fpga session using the xclbin file
 *          - 'stop' : close fpga session
 *          - 'swemu': software simulation of q6
 */
Datum xql6 (PG_FUNCTION_ARGS)
{
  text *relname = PG_GETARG_TEXT_PP(0);

  const char* p_relName = text_to_cstring (PG_GETARG_TEXT_PP (0));
  const char* p_krnName = text_to_cstring (PG_GETARG_TEXT_PP (1));

  char sBuf[1024] = "";

  RangeVar *relrv = makeRangeVarFromNameList (textToQualifiedNameList(relname));
  Relation rel = relation_openrv (relrv, AccessShareLock);

  if (!xql6_check_relation (rel)) {
    sprintf (sBuf, "xql6 cannot use the relation %s\n", p_relName);
  } else {

    xql6_init ();

    if (strncmp (p_krnName, "stop", 4) == 0) {
      xql6_stop_kernel (sBuf);
    } else if (strncmp (p_krnName, "swemu", 5) == 0) {
      xql6_run_sw (rel, sBuf);
    } else {
      xql6_run_hw (rel, sBuf, p_krnName);
    }
  }

  relation_close(rel, AccessShareLock);
  char *result = pstrdup (sBuf);

  hlog ("xql6 done\n");
  if (hlogFile) {fclose (hlogFile); hlogFile = NULL; }

  PG_RETURN_CSTRING (result);
}

void xql6_init ()
{
  int eno;
  if (!hlogFile) {
    hlogFile = fopen (xql6Log, "w"); eno = errno;
  }
  //hlog ("Start Rel %s kernel name %s\n", p_relName, krnlName); 
}

bool xql6_check_relation (Relation rel)
{
  bool retVal = true;

  /* Check that this relation has storage */
  if (rel->rd_rel->relkind == RELKIND_VIEW) {
    ereport(ERROR,
        (errcode(ERRCODE_WRONG_OBJECT_TYPE),
         errmsg("cannot get raw page from view \"%s\"",
            RelationGetRelationName(rel))));
    retVal = false;
  }

  if (rel->rd_rel->relkind == RELKIND_COMPOSITE_TYPE) {
    ereport(ERROR,
        (errcode(ERRCODE_WRONG_OBJECT_TYPE),
         errmsg("cannot get raw page from composite type \"%s\"",
            RelationGetRelationName(rel))));
    retVal = false;
  }

  if (rel->rd_rel->relkind == RELKIND_FOREIGN_TABLE) {
    ereport(ERROR,
        (errcode(ERRCODE_WRONG_OBJECT_TYPE),
         errmsg("cannot get raw page from foreign table \"%s\"",
            RelationGetRelationName(rel))));
    retVal = false;
  }

  /* not supported?
  if (rel->rd_rel->relkind == RELKIND_PARTITIONED_TABLE)
    ereport(ERROR,
        (errcode(ERRCODE_WRONG_OBJECT_TYPE),
         errmsg("cannot get raw page from partitioned table \"%s\"",
            RelationGetRelationName(rel))));
  */

  return retVal;
}

bool xql6_run_hw (Relation rel, char* p_sBuf, const char* p_krnName)
{
  if (p_fpgaKernelName && strcmp (p_fpgaKernelName, p_krnName) != 0) {
    // new kernel and old one exists. stop old
    if (!xql6_stop_kernel (p_sBuf)) {
      return false;
    }
  }

  if (!p_fpgaKernelName) {
    if (!xql6_init_kernel (p_krnName, p_sBuf)) {
      return false;
    }
  }

  long q6_sum = 0, nTups = 0;
  int nBlks = RelationGetNumberOfBlocksInFork (rel, MAIN_FORKNUM);

  Xql6FillState xql6FillState;
  xql6FillState. nBlks = nBlks;
  xql6FillState. blkNum = 0;
  xql6FillState. linNum = 0;

  hlog ("xql6_run start. nBlks %d\n", nBlks);

  struct timespec tBegTotal, tEndTotal, tBeg, tEnd;
  long tF=0, tC=0, tR=0, tT=0;
  clock_gettime (CLOCK_REALTIME, &tBegTotal);

  bool bRealPages = true;
  int blkId, blkIdDone=0;
  int status = EXIT_SUCCESS;

  for (blkId=0; bRealPages; ++blkId) {

    // read-back results of last 5 computes. Overlaps compute with transfer.
    // Can also make this completely event driven
    if (blkId % 5 == 0 && blkId >= 10) {
      size_t j = ((blkId - 10)/5) % 2;

      clock_gettime (CLOCK_REALTIME, &tBeg);
      if (0) hlog ("clWaitForEvents blkId %d\n", blkId);

      clWaitForEvents(5, &rdEvent [j * 5]);
      int k;
      for (k=0; k<5; ++k) {
        long curSum = 0;
        memcpy ((unsigned char*)&curSum, pOut[j*5 +k], sizeof(long));
        q6_sum += curSum;
        if (0)hlog ("Buf [%d] %d curSum %ld q6_sum %ld\n", blkId, j*5+k, curSum, q6_sum);
      }
      blkIdDone = blkId-5;

      clock_gettime (CLOCK_REALTIME, &tEnd);
      tR += diffTime (&tBeg, &tEnd);
    }

    int nBlkPages=0, nBlkTups=0;

    // fill new data from postgres
    clock_gettime (CLOCK_REALTIME, &tBeg);
    bRealPages = xql6FillBlock (rel, pInp [blkId%10], &nTups, &xql6FillState);
    clock_gettime (CLOCK_REALTIME, &tEnd);
    tF += diffTime (&tBeg, &tEnd);
    if (0) hlog ("filled blk [%d]\n", blkId);

    // process new data
    clock_gettime (CLOCK_REALTIME, &tBeg);
    status = fpga_q6_enq_compute (blkId%10);
    clock_gettime (CLOCK_REALTIME, &tEnd);
    tC += diffTime (&tBeg, &tEnd);

    if (status != EXIT_SUCCESS){
      hlog ("enque failed for blk %d\n", blkId);
      break;
    }
  }

  // wait for remaining enques to complete
  clock_gettime (CLOCK_REALTIME, &tBeg);
  for ( ; blkIdDone < blkId; ++blkIdDone) {
    if (0)hlog ("clWaitForEvents blkIdDone %d\n", blkIdDone);
    clWaitForEvents (1, &rdEvent[blkIdDone % 10]);

    long curSum=0;
    memcpy ((unsigned char*)&curSum, pOut[blkIdDone%10], sizeof(long));
    q6_sum += curSum;
    if (0)hlog ("Buf [%d] curSum %ld q6_sum %ld\n", blkIdDone, curSum, q6_sum);
  }
  clock_gettime (CLOCK_REALTIME, &tEnd);
  tR += diffTime (&tBeg, &tEnd);

  clock_gettime (CLOCK_REALTIME, &tEndTotal);
  tT += diffTime (&tBegTotal, &tEndTotal);

  hlog ("xql6_run: Total: %d tups %d blks\n", nTups, blkIdDone);
  hlog ("xql6_run: result %ld.%d\n", q6_sum/(100*100), q6_sum%(100*100));
  hlog ("xql6_run: Runtimes: Fill %ld, W+Comp %ld Read %ld Total %ld\n", tF, tC, tR, tT);

  char sTmp[1024];
  if (status == EXIT_SUCCESS) {
    sprintf (sTmp, "xql6_run (hw) result %ld.%ld\n"
      "Runtimes (us) Fill %ld, W+Comp %ld, Rd %ld, Tot %ld. #Tups %ld\n", 
      q6_sum/(100*100), q6_sum%(100*100), tF/1000, tC/1000, tR/1000, tT/1000, nTups);
  } else {
    sprintf (sTmp, "xql6_run failed enque-compute\n");
  }
  strcat (p_sBuf, sTmp);

  return status == EXIT_SUCCESS;
}

bool xql6_run_sw (Relation rel, char* p_sBuf)
{
  int nBlks = RelationGetNumberOfBlocksInFork (rel, MAIN_FORKNUM);
  unsigned char *p_xql6_blk = (unsigned char*) palloc (XQL6_BLK_SZ);

  Xql6FillState xql6FillState;
  xql6FillState. nBlks = nBlks; xql6FillState. blkNum = 0; xql6FillState. linNum = 0;

  hlog ("\nxql6_run start. nBlks %d\n", nBlks);

  long nFB=0, q6_sum = 0, nTups = 0, tF=0, tC=0;
  struct timespec tBeg, tEnd;

  bool bRealPages = true;
  while (bRealPages) {

    clock_gettime (CLOCK_REALTIME, &tBeg);
    bRealPages = xql6FillBlock (rel, p_xql6_blk, &nTups, &xql6FillState);
    clock_gettime (CLOCK_REALTIME, &tEnd);
    tF += diffTime (&tBeg, &tEnd);

    clock_gettime (CLOCK_REALTIME, &tBeg);
    q6_sum += q6_swemu (p_xql6_blk);
    clock_gettime (CLOCK_REALTIME, &tEnd);
    tC += diffTime (&tBeg, &tEnd);
    ++nFB;

    hlog ("q6_sum %ld\n", q6_sum);
  }

  pfree (p_xql6_blk);

  hlog ("xql6_run. nFB %d, nTups %ld\n", nFB, nTups);
  hlog ("xql6_run: Fill %ld, Enq %ld\n", tF, tC);
  
  sprintf (p_sBuf, "xql6_run (sw) result %ld.%ld\n"
    "Runtimes (us) Fill %ld, Comp %ld. #Tups %ld", 
      q6_sum/(100*100), q6_sum%(100*100), tF/1000, tC/1000, nTups);

  return true;
}

//----------------------------------------------------------------------------
// Kernel management routines
//----------------------------------------------------------------------------
bool xql6_init_kernel (const char *p_krnlName, char* p_sBuf)
{
  if (p_fpgaKernelName) { 
    hlog ("xql6_init_kernel found running kernel %s\n", p_fpgaKernelName);
    return false; 
  }

  struct timespec tBeg, tEnd;

  clock_gettime (CLOCK_REALTIME, &tBeg);
  int status = start_fpga (p_krnlName);
  clock_gettime (CLOCK_REALTIME, &tEnd);
  hlog ("xql6_init_kernel %s status %d\n", p_krnlName, status);

  char sTmp[1024];
  if (status != EXIT_SUCCESS) {
    sprintf (sTmp, "Failed to start kernel %s, Status %d, Time %ld us\n", 
             p_krnlName, status, diffTime (&tBeg, &tEnd)/1000);
    strcat (p_sBuf, sTmp);
    return false;
  } else {
    sprintf (sTmp, "xql6 init (fpga config) %ld us\n", diffTime (&tBeg, &tEnd)/1000);
    strcat (p_sBuf, sTmp);
    p_fpgaKernelName = strdup (p_krnlName);
    return true;
  }
}

bool xql6_stop_kernel (char* p_sBuf)
{
  char sTmp[1024];
  if (! p_fpgaKernelName) {
    sprintf (sTmp, "No running kernel. ");
    strcat (p_sBuf, sTmp);
    return true;
  }

  int status = end_fpga ();
  hlog ("xql6_stop_kernel status %d\n", status);

  if (status == EXIT_SUCCESS) {
    sprintf (sTmp, "Stopped kernel %s\n", p_fpgaKernelName);
    strcat (p_sBuf, sTmp);
  } else {
    sprintf (sTmp, "Failed to stop kernel %s status %d\n", p_fpgaKernelName, status);
    strcat (p_sBuf, sTmp);
  }

  free (p_fpgaKernelName);
  p_fpgaKernelName = NULL;
  return status == EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Routines to move data from PG to OCL buffers
//----------------------------------------------------------------------------
bool xql6FillBlock (Relation rel, unsigned char *p_bufptr, long *p_nTups,
                    Xql6FillState *p_fillState)
{
  bool bRealPages = true;

  int pgNum;
  for (pgNum = 0; pgNum < XQL6_PAGES_PER_BLK; ++pgNum) {
    int pgTups = xql6GetNextPage (rel, p_bufptr, p_fillState);
    bRealPages = (pgTups!=0);
    *p_nTups += pgTups;

    p_bufptr += XQL_PAGE_SZ;
  }

  return bRealPages;
}

int xql6GetNextPage (Relation rel, unsigned char* p_buf, Xql6FillState *p_fillState)
{
  unsigned char *p_dst = p_buf;
  unsigned char *p_dstEnd = p_buf + XQL_PAGE_SZ;
  int nTups = 0;
  bool bFilled = false;

  p_dst += 2;

  int blkNum, linNum;

  for (blkNum = p_fillState-> blkNum; blkNum < p_fillState-> nBlks; ++blkNum) {

    Buffer buf = ReadBufferExtended(rel, MAIN_FORKNUM, blkNum, RBM_NORMAL, NULL); 
    LockBuffer(buf, BUFFER_LOCK_SHARE);

    Page page = (Page) BufferGetPage (buf);
    int nLines = PageGetMaxOffsetNumber (page);

    linNum = p_fillState-> linNum;
    p_fillState-> linNum = 0;
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

        if (0 && hlogFile) {
          unsigned char* p_tmp = p_dst;
          fprintf (hlogFile, "\nTup[%d] ", nTups);
          int d;
          for (d=0; d<tuple_data_len; ++d) 
            fprintf (hlogFile, "%02x", p_tmp[d]);
          fprintf (hlogFile, "\n");
          fflush (hlogFile);
        }

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
  p_fillState-> blkNum = blkNum;
  p_fillState-> linNum = linNum;

  return nTups;
}


