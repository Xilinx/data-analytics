//----------------------------------------------------------------------------
// UDF to package tuple data for xql6-kernel
// 2017-06-11 22:25:39 parik 
//
// Build in environment without SDX (x). Needs postgresql-server-dev-9.6
//
// Add LD_LIBRARY_PATH and XILINX_SDX to /etc/postgresql/9.6/main/environment
// Eg:
// LD_LIBRARY_PATH = '/proj/xbuilds/2016.4_sdx_0310_1/installs/lin64/SDx/2016.4/lib/lnx64.o:/proj/xbuilds/2016.4_sdx_0310_1/installs/lin64/SDx/2016.4/runtime/lib/x86_64'
// XILINX_SDX = '/proj/xbuilds/2016.4_sdx_0310_1/installs/lin64/SDx/2016.4'
//
// Touch and og+w the log files
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
//#include "utils/varlena.h"

#include "access/relscan.h"
#include "access/heapam.h"
#include "utils/snapmgr.h"
#include "utils/lsyscache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
//#include <sys/time.h>
#include "xql6.h"
#include <CL/opencl.h>

PG_MODULE_MAGIC;

void xql6_build_state ();
void xql6_reset_state ();
void xql6_init (const char* krnlName);
long xql6_run_hw (Relation rel);
long xql6_run_sw (Relation rel);
void xql6_end (Relation rel);
bool xql6FillBlock (Relation rel, unsigned char *p_bufptr, long *p_nTups, 
                    Xql6FillState *p_fillState);

int xql6kernel_stop ();
int xql6kernel_init (const char *krnlName);

// replacement for fn_extra
static Xql6State *p_xql6state = NULL;

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
static const char xql6Log[] = "/wrk/parik/sdaccel/pgdb/xql6/v4/xql6.log";
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
  //uint32    blkno = PG_GETARG_UINT32(1);

  const char* relname_str = text_to_cstring (PG_GETARG_TEXT_PP (0));
  const char* krnlname = text_to_cstring (PG_GETARG_TEXT_PP (1));

  if (!hlogFile) hlogFile = fopen (xql6Log, "w"); 
  hlog ("start relname %s krnlname %s\n", relname_str, krnlname); 

  RangeVar *relrv = makeRangeVarFromNameList (textToQualifiedNameList(relname));
  Relation rel = relation_openrv (relrv, AccessShareLock);

  /* Check that this relation has storage */
  if (rel->rd_rel->relkind == RELKIND_VIEW)
    ereport(ERROR,
        (errcode(ERRCODE_WRONG_OBJECT_TYPE),
         errmsg("cannot get raw page from view \"%s\"",
            RelationGetRelationName(rel))));
  if (rel->rd_rel->relkind == RELKIND_COMPOSITE_TYPE)
    ereport(ERROR,
        (errcode(ERRCODE_WRONG_OBJECT_TYPE),
         errmsg("cannot get raw page from composite type \"%s\"",
            RelationGetRelationName(rel))));
  if (rel->rd_rel->relkind == RELKIND_FOREIGN_TABLE)
    ereport(ERROR,
        (errcode(ERRCODE_WRONG_OBJECT_TYPE),
         errmsg("cannot get raw page from foreign table \"%s\"",
            RelationGetRelationName(rel))));

  /* not supported?
  if (rel->rd_rel->relkind == RELKIND_PARTITIONED_TABLE)
    ereport(ERROR,
        (errcode(ERRCODE_WRONG_OBJECT_TYPE),
         errmsg("cannot get raw page from partitioned table \"%s\"",
            RelationGetRelationName(rel))));
  */

  char sBuf[1024] = "";
  sprintf (sBuf, "Rel %s has %d blocks. ", 
    RelationGetRelationName (rel), 
    RelationGetNumberOfBlocksInFork (rel, MAIN_FORKNUM));

  if (strncmp (krnlname, "stop", 4) == 0) {
    int status = xql6kernel_stop ();
    sprintf (sBuf, "\nKernel Stopped. Status %d\n", status);
    if (hlogFile) {
      fclose (hlogFile); hlogFile = NULL;
    }

  } else {
    char sTmp[1024];
    struct timespec tBeg, tEnd;

    sTmp[0] = '\0';
    clock_gettime (CLOCK_REALTIME, &tBeg);
    xql6_init (krnlname);
    clock_gettime (CLOCK_REALTIME, &tEnd);
    sprintf (sTmp, "\nxql6_init %ld ns\n", diffTime (&tBeg, &tEnd));
    strcat (sBuf, sTmp);

    sTmp[0] = '\0';
    long res=0;
    clock_gettime (CLOCK_REALTIME, &tBeg);
    if (p_xql6state-> runMode == XQL6_HW) {
      res = xql6_run_hw (rel);
    } else if (p_xql6state-> runMode == XQL6_SW) {
      res = xql6_run_sw (rel);
    }
    clock_gettime (CLOCK_REALTIME, &tEnd);
    sprintf (sTmp, "\nxql6_run result %ld %ld ns\n", res, diffTime (&tBeg, &tEnd));
    strcat (sBuf, sTmp);
    hlog ("xql6_run done. Time %ld ns\n", diffTime (&tBeg, &tEnd));

    sTmp[0] = '\0';
    clock_gettime (CLOCK_REALTIME, &tBeg);
    xql6_end (rel);
    clock_gettime (CLOCK_REALTIME, &tEnd);
    sprintf (sTmp, "\nxql6_end %ld ns\n", diffTime (&tBeg, &tEnd));
    strcat (sBuf, sTmp);
  }

  hlog ("xql6_end done\n");

  relation_close(rel, AccessShareLock);

  char *result = pstrdup (sBuf);

  if (hlogFile) {fclose (hlogFile); hlogFile = NULL; }

  PG_RETURN_CSTRING (result);
}

void xql6_init (const char* krnlName)
{
  hlog ("xql6_init kernel %s\n", krnlName);

  xql6_build_state ();

  if (strncmp (krnlName, "swemu", 5) == 0) {
    p_xql6state-> runMode = XQL6_SW;
    hlog ("xql6_init init-ed sw-sim mode");
    return;
  } 
  
  // hw kernel. Init if first time. Stop and re-init if kernel changed
  if (p_xql6state-> fpgaInitPassed && 
      strcmp (p_xql6state-> fpgaKernelName, krnlName) != 0) 
  {
    hlog ("xql6_init stopping current hw kernel %s\n", p_xql6state-> fpgaKernelName);
    int stopStatus = xql6kernel_stop ();
    if (stopStatus != EXIT_SUCCESS) {
      hlog ("xql6_init xql6kernel_stop failed status %d\n", stopStatus);
      return;
    }
  }

  if (xql6kernel_init (krnlName) != EXIT_SUCCESS) {
    hlog ("xql6_init failed\n");
    return;
  }

  p_xql6state-> fpgaInitPassed = true;
  p_xql6state-> fpgaKernelName = strdup (krnlName);
  p_xql6state-> runMode = XQL6_HW;

  hlog ("xql6_init passed\n");
}

long xql6_run_hw (Relation rel)
{
  if (! p_xql6state-> fpgaInitPassed) return 0;

  long q6_sum = 0;
  long nTups = 0;

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
    int status = fpga_q6_enq_compute (blkId%10);
    clock_gettime (CLOCK_REALTIME, &tEnd);
    tC += diffTime (&tBeg, &tEnd);

    if (status != EXIT_SUCCESS) break;
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

  p_xql6state-> numTups = nTups;
  p_xql6state-> numCalls += 1;

  return q6_sum;
}

long xql6_run_sw (Relation rel)
{
  int nBlks = RelationGetNumberOfBlocksInFork (rel, MAIN_FORKNUM);
  unsigned char *xql6_blk = (unsigned char*) palloc (XQL6_BLK_SZ);

  Xql6FillState xql6FillState;
  xql6FillState. nBlks = nBlks;
  xql6FillState. blkNum = 0;
  xql6FillState. linNum = 0;

  hlog ("\nxql6_run start. nBlks %d\n", nBlks);

  int nFB=0;
  long q6_sum = 0;
  long nTups = 0;
  struct timespec tBeg, tEnd;
  long timeFill=0, timeEnq=0;

  bool bRealPages = true;
  while (bRealPages) {

    clock_gettime (CLOCK_REALTIME, &tBeg);
    bRealPages = xql6FillBlock (rel, xql6_blk, &nTups, &xql6FillState);
    clock_gettime (CLOCK_REALTIME, &tEnd);
    timeFill += diffTime (&tBeg, &tEnd);

    clock_gettime (CLOCK_REALTIME, &tBeg);
    q6_sum += q6_swemu (xql6_blk);
    clock_gettime (CLOCK_REALTIME, &tEnd);
    timeEnq += diffTime (&tBeg, &tEnd);
    ++nFB;

    hlog ("q6_sum %ld\n", q6_sum);
  }

  //clock_gettime (CLOCK_REALTIME, &tBeg);
  //bRealPages = xql6FillBlock (rel, xql6_blk, &nTups, &xql6FillState);
  //clock_gettime (CLOCK_REALTIME, &tEnd);
  //timeFill += diffTime (&tBeg, &tEnd);
  //hlog ("xql6FillBlock %ld ns\n", tf);

  pfree (xql6_blk);

  p_xql6state-> numTups = nTups;

  hlog ("xql6_run. nFB %d, nTups %ld\n", nFB, nTups);
  hlog ("xql6_run: Fill %ld, Enq %ld\n", timeFill, timeEnq);

  return q6_sum;
}

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

    //hlog ("xql6FillBlock. page %d, real %d tups %d\n", pgNum, bRealPages, pgTups); 
  }

  //hlog ("xql6FillBlock done. %d pages, %ld tups\n", pgNum, *p_nTups);

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

    //hlog ("blkNum %d, nLines %d, pfsLinNum %d\n", blkNum, nLines, p_fillState-> linNum);

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

  //hlog ("xql6GetNextPage blkNum %d linNum %d\n", blkNum, linNum);

  return nTups;
}

void xql6_end (Relation rel)
{
  // nothing for now
}

int xql6kernel_init (const char *krnlName)
{
  hlog ("xql6kernel_init: starting fpga...\n");
  int status = start_fpga (krnlName);

  if (status != EXIT_SUCCESS) {
    hlog ("xql6kernel_init: start_fpga Failed.\n");
    p_xql6state-> runMode = XQL6_SW;
    return 0;
  }
  return status;
}

int xql6kernel_stop ()
{
  int retVal = EXIT_SUCCESS;
  hlog ("xql6kernel_stop...\n");

  if (p_xql6state-> runMode == XQL6_HW) {
    retVal = end_fpga ();
    hlog ("end_fpga status %d\n", retVal);
  }

  xql6_reset_state ();

  hlog ("xql6kernel_stop.\nEOF\n");

  return retVal;
}

void xql6_build_state ()
{
  if (p_xql6state) return;

  p_xql6state = malloc (sizeof (Xql6State));
  p_xql6state-> fpgaKernelName = NULL;
  xql6_reset_state ();
}

void xql6_reset_state ()
{
  if (p_xql6state-> fpgaKernelName) {
    free (p_xql6state-> fpgaKernelName);
  }

  p_xql6state-> fpgaKernelName = NULL;
  p_xql6state-> fpgaInitPassed = false;
  p_xql6state-> numCalls = 0;
  p_xql6state-> numTups = 0;
  p_xql6state-> runMode = XQL6_SW;

}
