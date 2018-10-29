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

#include "storage/smgr.h"

//#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

PG_MODULE_MAGIC;

void xql6_init ();
bool xql6_run_hw (Relation, char*, const char*);
bool xql6_run_sw (Relation, char*);
bool xql6_init_kernel (const char*, char*);
bool xql6_stop_kernel (char*);
bool xql6_check_relation (Relation rel);

bool xql6FillBlock (Relation, unsigned char*, long*, Xql6FillState*);
size_t xql6FillBlock_new (Relation rel, char *p2pbuf, BlockNumber blk, BlockNumber nblks);
int xql6GetNextPage (Relation, unsigned char*, Xql6FillState*);

void xpgopen(Relation rel, int isdirect);
size_t
xpgread_bigd(Relation rel, BlockNumber blocknum, BlockNumber nblks, char *buffer);
void xpgclose(Relation rel);
extern size_t
mdread_bigd(SMgrRelation reln, ForkNumber forknum, BlockNumber blocknum,
           BlockNumber nblks, char *buffer);


static char* p_fpgaKernelName = NULL;
BlockNumber	g_nblks;

//----------------------------------------------------------------------------
#if 1
#define NPIPES 10
#define NCOMPUTES 5
#else
#define NPIPES 20
#define NCOMPUTES 10
#endif
extern unsigned char* pInp[NPIPES];
extern unsigned char* pOut[NPIPES];
extern cl_event rdEvent [NPIPES];

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

int g_devid;

Datum xql6(PG_FUNCTION_ARGS)
{
  text *relname = PG_GETARG_TEXT_PP(0);
  uint32 devid = PG_GETARG_UINT32(2);
  g_devid = devid;

  const char* p_relName = text_to_cstring (PG_GETARG_TEXT_PP (0));
  const char* p_krnName = text_to_cstring (PG_GETARG_TEXT_PP (1));
  const char* p_devid = text_to_cstring (PG_GETARG_TEXT_PP (1));

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
  BlockNumber pgblk, pgnblks;
  size_t	rdcnt, sz;
  g_nblks = nBlks;

  Xql6FillState xql6FillState;
  xql6FillState. nBlks = nBlks;
  xql6FillState. blkNum = 0;
  xql6FillState. linNum = 0;

  hlog ("xql6_run start. nBlks %d\n", nBlks);
  hlog ("xql6_run start. devid %d\n", g_devid);

  struct timespec tBegTotal, tEndTotal, tBeg, tEnd;
  long tF=0, tC=0, tR=0, tR2=0, tT=0;
  clock_gettime (CLOCK_REALTIME, &tBegTotal);

  bool bRealPages = true;
  int blkId, blkIdDone=0;
  int status = EXIT_SUCCESS;

  xpgopen(rel, 1);

  for (blkId=0; bRealPages; ++blkId) {

    // read-back results of last 5 computes. Overlaps compute with transfer.
    // Can also make this completely event driven
    if (blkId % NCOMPUTES == 0 && blkId >= NPIPES) {
      size_t j = ((blkId - NPIPES)/NCOMPUTES) % 2;

      clock_gettime (CLOCK_REALTIME, &tBeg);
      if (1) hlog ("clWaitForEvents blkId %d j %d \n", blkId, j);

      clWaitForEvents(NCOMPUTES, &rdEvent [j * NCOMPUTES]);
      int k;
      for (k=0; k<NCOMPUTES; ++k) {
        long curSum = 0;
        memcpy ((unsigned char*)&curSum, pOut[j*NCOMPUTES +k], sizeof(long));
        q6_sum += curSum;
        if (1)hlog ("Buf [%d] %d curSum %ld q6_sum %ld\n", blkId, j*5+k, curSum, q6_sum);
      }
      blkIdDone = blkId-NCOMPUTES;

      clock_gettime (CLOCK_REALTIME, &tEnd);
      tR += diffTime (&tBeg, &tEnd);
    }

    int nBlkPages=0, nBlkTups=0;

    // fill new data from postgres
    pgblk = (blkId * XQL6_BLK_SZ) / BLCKSZ;
    pgnblks = XQL6_BLK_SZ / BLCKSZ;
    sz = XQL6_BLK_SZ;
    clock_gettime (CLOCK_REALTIME, &tBeg);
    if (1) hlog ("hw: b xfbn: blkId %lld pgblk %lld pgnblks %lld sz %lld\n", blkId, pgblk, pgnblks, sz);
    rdcnt = xql6FillBlock_new (rel, pInp [blkId%NPIPES], pgblk, pgnblks);
    if (1) hlog ("hw: a xfbn: pgblk %lld pgnblks %lld sz %lld rdcnt %lld\n", pgblk, pgnblks, sz, rdcnt);
    if (rdcnt != sz) {
	char	*p = pInp [blkId%NPIPES];
	memset(p+rdcnt, 0, (sz - rdcnt));
    	bRealPages = false;
    	if (1) hlog ("hw: brp false: pgblk %lld pgnblks %lld sz %lld rdcnt %lld\n", pgblk, pgnblks, sz, rdcnt);
    } else {
    	if (1) hlog ("hw: brp true: pgblk %lld pgnblks %lld sz %lld rdcnt %lld\n", pgblk, pgnblks, sz, rdcnt);
    }
    clock_gettime (CLOCK_REALTIME, &tEnd);
    tF += diffTime (&tBeg, &tEnd);
    if (1) hlog ("filled blk [%d] bRealPages %d \n", blkId, bRealPages);

    // process new data
    clock_gettime (CLOCK_REALTIME, &tBeg);
    status = fpga_q6_enq_compute (blkId%NPIPES);
    clock_gettime (CLOCK_REALTIME, &tEnd);
    tC += diffTime (&tBeg, &tEnd);

    if (status != EXIT_SUCCESS){
      hlog ("enque failed for blk %d\n", blkId);
      break;
    }
  }

  xpgclose(rel);

  // wait for remaining enques to complete
  clock_gettime (CLOCK_REALTIME, &tBeg);
  for ( ; blkIdDone < blkId; ++blkIdDone) {
    if (1)hlog ("clWaitForEvents blkIdDone %d blkId %d\n", blkIdDone, blkId);
    clWaitForEvents (1, &rdEvent[blkIdDone % NPIPES]);

    long curSum=0;
    memcpy ((unsigned char*)&curSum, pOut[blkIdDone%NPIPES], sizeof(long));
    q6_sum += curSum;
    if (1)hlog ("Buf [%d] curSum %ld q6_sum %ld\n", blkIdDone, curSum, q6_sum);
  }
  clock_gettime (CLOCK_REALTIME, &tEnd);
  tR2 += diffTime (&tBeg, &tEnd);

  clock_gettime (CLOCK_REALTIME, &tEndTotal);
  tT += diffTime (&tBegTotal, &tEndTotal);

  hlog ("xql6_run: Total: %d tups %d blks\n", nTups, blkIdDone);
  hlog ("xql6_run: result %ld.%d\n", q6_sum/(100*100), q6_sum%(100*100));
  hlog ("xql6_run: Runtimes: Fill %ld, W+Comp %ld Read %ld %ld Total %ld\n", tF, tC, tR, tR2, tT);

  char sTmp[1024];
  if (status == EXIT_SUCCESS) {
#if 1
    sprintf (sTmp, "xql6_run (hw) result %ld.%ld\n"
      "Runtimes (us) Fill %ld, W+Comp %ld, Rd %ld, Tot %ld. #Tups %ld\n", 
      q6_sum/(100*100), q6_sum%(100*100), tF/1000, tC/1000, tR/1000, tT/1000, nTups);
#else
    sprintf (sTmp, "xql6_run (hw) result %ld.%ld\n"
      "Runtimes (us) Fill %ld, W+Comp %ld, Rd %ld, %ld Tot %ld. #Tups %ld\n", 
      q6_sum/(100*100), q6_sum%(100*100), tF/1000, tC/1000, tR/1000, tR2/1000, tT/1000, nTups);
#endif
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
    sprintf (sTmp, "xql6 init device %d (fpga config) %ld us\n", g_devid, diffTime (&tBeg, &tEnd)/1000);
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

size_t xql6FillBlock_new (Relation rel, char *p2pbuf, BlockNumber blk, BlockNumber nblks)
{

	size_t	ret, sz;
  	hlog ("\nxfbn: blk %lld nblks %lld \n", blk, nblks);

	ret = xpgread_bigd(rel, blk, nblks, p2pbuf);

	if(ret < 0) {
  		hlog ("\nxfbn: xpgrb ERROR: blk %lld nblks %lld :%s:\n", blk, nblks, strerror(errno));
		return ret;
	}
  	hlog ("\nxfbn: xpgrbd ret: blk %lld nblks %lld ret %lld \n", blk, nblks, ret);
	sz = nblks * BLCKSZ;
	if(ret != sz) {
		hlog("xfbn: xpgrbd read eof\n");
	} else {
		hlog("xfbn: xpgrbd full read\n");
	}
	return ret;
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

void
xpgopen(Relation rel, int isdirect)
{

	char	*path = NULL;
	SMgrRelation reln;
  	hlog ("\nxpgopen before rd_smgr 0x%p isdirect %d\n", rel->rd_smgr, isdirect);

	if (rel->rd_smgr == NULL) {
		RelationOpenSmgr(rel);
	}
  	hlog ("\nxpgopen after rd_smgr 0x%p\n", rel->rd_smgr);
	if (rel->rd_smgr) {
		reln =rel->rd_smgr;
		path = GetRelationPath(reln->smgr_rnode.node.dbNode, reln->smgr_rnode.node.spcNode, reln->smgr_rnode.node.relNode, reln->smgr_rnode.backend, MAIN_FORKNUM);
	}

	if (path) {
  		hlog ("\nxpgopen start. relpath %s\n", path);
		pfree(path);
	}

	//tblmdfd = mdopen(reln, MAIN_FORKNUM, EXTENSION_FAIL);
	//mdopen(reln, MAIN_FORKNUM, EXTENSION_FAIL);
	if (!mdexists(reln, MAIN_FORKNUM, isdirect)) {
  		hlog ("\nxpgopen ERROR ext 0x%p\n", rel->rd_smgr);
	} else {
  		hlog ("\nxpgopen SUCCESS ext 0x%p\n", rel->rd_smgr);
	}
  	hlog ("\nxpgopen ext 0x%p\n", rel->rd_smgr);

	//return tblmdfd;
}

void
xpgclose(Relation rel)
{

	char	*path = NULL;
	SMgrRelation reln;
  	hlog ("\nxpgclose before rd_smgr 0x%p \n", rel->rd_smgr);

	if (rel->rd_smgr == NULL) {
		RelationOpenSmgr(rel);
	}
  	hlog ("\nxpgclose after rd_smgr 0x%p\n", rel->rd_smgr);
	if (rel->rd_smgr) {
		reln =rel->rd_smgr;
		path = GetRelationPath(reln->smgr_rnode.node.dbNode, reln->smgr_rnode.node.spcNode, reln->smgr_rnode.node.relNode, reln->smgr_rnode.backend, MAIN_FORKNUM);
	}

	if (path) {
  		hlog ("\nxpgopen start. relpath %s\n", path);
		pfree(path);
	}

	mdclose(reln, MAIN_FORKNUM);
  	hlog ("\nxpgclose ext 0x%p\n", rel->rd_smgr);

}

void
xpgread(Relation rel, BlockNumber blocknum, char *buffer)
{
	char	*path = NULL;
	SMgrRelation reln;
	int	isreln = 0;
  	if (0) hlog ("\nxpgread before rd_smgr 0x%p %d %p \n", rel->rd_smgr, blocknum, buffer);

	if (rel->rd_smgr == NULL) {
		RelationOpenSmgr(rel);
	}
  	if (0) hlog ("\nxpgread after rd_smgr 0x%p\n", rel->rd_smgr);
	if (rel->rd_smgr) {
		reln =rel->rd_smgr;
		//path = GetRelationPath(reln->smgr_rnode.node.dbNode, reln->smgr_rnode.node.spcNode, reln->smgr_rnode.node.relNode, reln->smgr_rnode.backend, MAIN_FORKNUM);
		isreln =1;
	}

	if (path) {
  		if (0) hlog ("\nxpgread start. relpath %s\n", path);
		pfree(path);
	}

	if (isreln) {
		mdread(reln, MAIN_FORKNUM, blocknum, buffer);
  		if (1) hlog ("\nxpgread SUCCESS reln\n");
	} else {
  		hlog ("\nxpgread ERROR reln\n");
	}
}

size_t
xpgread_bigd(Relation rel, BlockNumber blocknum, BlockNumber nblks, char *buffer)
{
	char	*path = NULL;
	SMgrRelation reln;
	int	isreln = 0;
	size_t	rdsz;
  	if (1) hlog ("\nxpgread_bigd before rd_smgr 0x%p %d %d %p \n", rel->rd_smgr, blocknum, nblks, buffer);

	if (rel->rd_smgr == NULL) {
		RelationOpenSmgr(rel);
	}
  	if (1) hlog ("\nxpgread after rd_smgr 0x%p\n", rel->rd_smgr);
	if (rel->rd_smgr) {
		reln =rel->rd_smgr;
		isreln =1;
	}

	if (isreln) {
		rdsz = mdread_bigd(reln, MAIN_FORKNUM, blocknum, nblks, buffer);
  		if (1) hlog ("\nxpgread_bigd returned rdsz %lld \n", rdsz);
  		if (rdsz > 0) {
			hlog ("\nxpgread SUCCESS reln\n");
		} else {
			hlog ("\nxpgread FAILURE reln\n");
		}
	} else {
  		hlog ("\nxpgread ERROR reln\n");
	}
	return rdsz;
}


