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
#include "xpgocl.h"
#include "qdefs.h"
#include "sqlcmds.h"
#include "xdbconn.h"
#include "xdbutils.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>


extern "C" {
#include "xpgbolt.h"
#include "xpgdatastore.h"
}

extern "C" {
  #include "nodes/relation.h"
}

typedef unsigned char uchar;
extern unsigned char* confq6 (unsigned char* p_buf, unsigned char scanType);
extern bool confq6Verify (unsigned char* p_buf, unsigned char scanType);

#define SCAN
#define NUM_BYTES (2*1024*1024)
#define NUM_RES_WORDS NUM_BYTES/64
#define NUM_PU 32
#define PAGES_PER_PU 8

const int nAttr = 16;
//const int attrLens [] = {8, 8, 8, 4, -1, -1, -1, -1, -1, -1,  4,  4,  4, -1, -1, -1};
//const bool align   [] = {0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0};
const uchar attrTyps [] = {0, 0, 0, 0,  2,  2,  2,  0,  0,  0,  1,  0,  0,  0,  0,  0};
const bool attrAlign [] = {0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0};
const bool  attrSelScanAggr  [] = {0, 0, 0, 0,  1,  1,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0};
const bool  attrSelScan      [] = {0, 0, 0, 0,  1,  0,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0};
const int attrLens[] =   { 8, 8, 8, 4, -1, -1, -1, -1, -1, -1,  4,  4,  4, -1, -1, -1 };

long q6_sum = 0, curSum=0;


unsigned int countMatches (const std::vector <unsigned long>& vVals, 
                 unsigned long resTups,
                 unsigned long totTups,
                 XpgBuffer* bufData, int& count
                 )
{
  long cntTups = 0, cntMats = 0, resTups2 = resTups;

  for (std::vector <unsigned long>::const_iterator vItr = vVals. begin ();
      vItr != vVals. end (); ++vItr) 
  {
    unsigned long val = *vItr;

    // leading bit is reserved. So the largest value for startk is 62 (this is
    // index not count). resTups is a count. Have to -1 to get index
    int startk = (int) std::min (62L, resTups2-1);
    for (int k = startk; k>=0; --k, ++cntTups) {
      ++count;
      if (val >> k & 0x1) {
        bufData->buf[count] = 1;
        //std::cout << "Matched Tup[" << totTups + cntTups << "]" << std::endl;
        ++ cntMats;
      }
    }

    resTups2 -= (startk+1);
  }
  if (cntTups != resTups) 
    std::cout << "ERROR: insufficient #vals " << vVals. size () * 63 << " #resTups " << resTups << " #cntTups " << cntTups << std::endl; 
  return cntMats;
}

void processResult2 (long *p_res, long& curSum, unsigned long& totTups, XpgBuffer* bufData, int& count)
{
  // PU_i, PG_j, Tup_k, tups packed into 63 bits. followed by footer

  int tupIdx = 0, pgIdx = 0, puIdx = 0;
  unsigned long nMatsVerif = 0;
  unsigned long nMats = 0, nTups = 0;
  std::vector <unsigned long> vVals;

  int i;
  for (i=0; i < NUM_RES_WORDS; ++i) {
    unsigned long val = (unsigned long) (p_res [i]);

    if (val & 1UL << 63) {
      // footer
      nTups = val & 0xFFFF;
      nMats = val >> 16 & 0xFFFF;
      unsigned int nMatsVerif = countMatches (vVals, nTups, totTups, bufData, count);

      /*/std::cout << "Results: PU[" << puIdx << "] Pg[" << pgIdx << "] " << nTups << " tups and " 
                << nMats << " matches. nMatsVerif " << nMatsVerif << ((nMatsVerif == nMats)?" PASS":" FAIL") 
                << std::endl;*/

      totTups += nTups;

      vVals. clear ();
      ++ puIdx;
      if (puIdx >= NUM_PU) {
        ++ pgIdx; puIdx = 0;
      }
      if (pgIdx >= PAGES_PER_PU) {
        break;
      }

    } else {
      if (xpg_codegendebug && val)
        std::cout << "val 0x" << std::hex << std::setfill('0') << std::setw(16) << val << std::dec << std::endl;
      vVals. push_back (val);
    }

  }

  if (i < NUM_RES_WORDS && pgIdx == PAGES_PER_PU) {
    long val = (p_res [++i]);
    //std::cout << "Results: aggregate " << val << std::endl;
  }
}


//Query processing 
void readSqlQueryBufCopy(XdbConn *xdbConn, int queueNo, cl_event event, void *userData) {
  if (xdbConn == nullptr) {
    printf("Error: No xdbconn in Read %d\n", queueNo);
    return;
  }  
  char *readBuf = xdbConn->getReadBuf(queueNo);
  if (readBuf == nullptr) {
    printf("Error: Read %d failed\n", queueNo);
    return;
  }

	int count = -1;
  //printf("Queue %d total tups: %d\n",queueNo,xpg_buffer_container.list[queueNo].tup_buffer.numTups);
  XpgBuffer* bufData = static_cast<XpgBuffer*>(userData);
  long* ll = (long*)readBuf;
  unsigned long nResTotTups = 0;
  long curSum=0;
  #ifdef SCAN
  processResult2(ll, curSum, nResTotTups, bufData, count); 

  #else
  memcpy ((char*)&curSum, readBuf, sizeof(long));
  q6_sum += curSum;
  elog(INFO, "curSum: %ld, q6_sum: %ld, qNo %d\n", curSum, q6_sum, queueNo);
  #endif
  std::cout.flush();
  // todo read these directly from the device later, for now this is ok
  /*if (userData != nullptr) {
    memcpy(userData,readBuf,32768);
  }*/
  if (event != NULL)  { // in case of event syncs
    cl_int err = clSetUserEventStatus(event, CL_COMPLETE);
    xdbConn->getOclAccel().printError(err,"Read event generation failed");
  }
}

void writeSqlQueryBufCopy(XdbConn *xdbConn, int queueNo, cl_event event, void *userData) {
  if (xdbConn == nullptr) {
    printf("Error: No xdbconn in Write %d\n", queueNo);
    return;
  }  
  char *writeBuf = xdbConn->getWriteBuf(queueNo);
  if (writeBuf == nullptr) {
    printf("Error: Write buf called on %d, but write buf pointer is null.\n", queueNo);
    return;
  }
  // nothing else to to do
  if (event != NULL)  { // in case of event syncs  
    cl_int err = clSetUserEventStatus(event, CL_COMPLETE);
    xdbConn->getOclAccel().printError(err,"Write event generation failed");
  }
}

/* 'process_var_stream_type' generates varstreamtype consisting of enumeration value correspond to each column type in var stream
 */
int process_var_stream_type_scan(char *varstream_type, int query) {
  std::string ss;
  if(query == 6) {
    int no_cols = 3;
    ss.append(std::to_string(no_cols));
    ss.push_back('|');
    //DECIMAL is of type Enum 1
    //DATE is of type Enum 2
    //Query6 has DECIMAL/DECIMAL/DATE in sequence
    ss.push_back('1');
    ss.push_back('|');
    ss.push_back('1');
    ss.push_back('|');
    ss.push_back('2');
    ss.push_back('|');
    ss.push_back('\n');
  }

  //printf("absday(%d-%d-%d) = %d\n", tm.year, tm.month, tm.day, absday);
  int ss_len = ss.length();
  //*constream = (char *)malloc(ss_len);
  memcpy(varstream_type, ss.c_str(), ss_len);
  return ss_len;
}

int process_var_stream_type_scan_aggr(char *varstream_type, int query) {
  string ss;
  if(query == 6) {
    int no_cols = 4;
    ss.append(tostring(no_cols));
    ss.push_back('|');
    //DECIMAL is of type Enum 1
    //DATE is of type Enum 2
    //Query6 has DECIMAL/DECIMAL/DATE in sequence
    ss.push_back('1');
    ss.push_back('|');
    ss.push_back('1');
    ss.push_back('|');
    ss.push_back('1');
    ss.push_back('|');
    ss.push_back('2');
    ss.push_back('|');
    ss.push_back('\n');
  }

  //printf("absday(%d-%d-%d) = %d\n", tm.year, tm.month, tm.day, absday);
  int ss_len = ss.length();
  //*constream = (char *)malloc(ss_len);
  memcpy(varstream_type, ss.c_str(), ss_len);
  return ss_len;
}

// Hard coded sqlengine query 6 setup, const and opcode, etc.
// Replace it with a call to codeGen
void createSqlengineSetup(char *setupbuf)
{
  // first byte tells the type of block it is
  char *setupstream = setupbuf;
  *setupstream++ = QDefs::PUSetupBlock;
  
  // follow by query type
  #ifdef SCAN
    *setupstream++ = QDefs::QueryScan;
  #else
    *setupstream++ = QDefs::QueryScanAggr;
  #endif

  int len = process_const(setupstream, false);
  setupstream += len;
#ifdef SCAN
  len = process_qualifier_scan(setupstream, 6);
  setupstream += len;
  len = process_var_stream_type_scan(setupstream, 6);
  setupstream += len;  
#else
  len = process_qualifier_scan_aggr(setupstream, 6);
  setupstream += len;
  len = process_var_stream_type_scan_aggr(setupstream, 6);
  setupstream += len;  
  len = process_aggregation(setupstream, 6);
  setupstream+= len;
#endif
}

bool createSqlengineSetupBinary (unsigned char* p_buf, unsigned char scanType)
{
  unsigned char* p_end = confq6 (p_buf, scanType);
  std::cout << "createSqlengineSetupBinary mode " << (scanType?"SCANAGGR":"SCANONLY")
            << " size " << (p_end - p_buf) << std::endl;

  if (! confq6Verify (p_buf, scanType)) {
    std::cout << "confq6Verify failed" << std::endl;
    return false;
  }
  std::cout << "confq6Verify passed" << std::endl;
  return true;

}

bool fillSetupBlock (char* p_bufptr)
{
  memset (p_bufptr, 0, NUM_BYTES);

  int i=0;
  p_bufptr[i++] = 0xFF;
  p_bufptr[i++] = 0xFF;
  p_bufptr[i++] = (unsigned char)nAttr;

  for (int a=0; a < nAttr; ++a) {
    p_bufptr[i++] = (char) attrLens [a];
    p_bufptr[i++] = (uchar) attrTyps [a];
    p_bufptr[i++] =         attrAlign [a];
  
  #ifdef SCAN
    	p_bufptr[i++] = attrSelScan [a];
  #else
    	p_bufptr[i++] = attrSelScanAggr [a];
  #endif
  }

  // first 4k is for filter, second is for sqleng
  if (i >= 4*1024) {
    std::cout << "filterBuf > 4k" << std::endl;
    return false;
  }
  i = 4*1024;

  //createSqlengineSetup((char *)(p_bufptr + i));
  unsigned char scanType = KERNEL_MODE_SCANONLY;
      //scanType = KERNEL_MODE_SCANAGGR;
  if (!createSqlengineSetupBinary((unsigned char*)(p_bufptr + i), scanType)) {
    elog(INFO, "createSqlengineSetupBinary failed");
    return false;
  }
  return true;
}
bool fillSetupStream (char* p_bufptr)
{
  //memset (p_bufptr, 0, NUM_BYTES);

  int i=0;
  p_bufptr[i++] = 0xFF;
  p_bufptr[i++] = 0xFF;
  p_bufptr[i++] = (unsigned char)nAttr;

  for (int a=0; a < nAttr; ++a) {
    p_bufptr[i++] = (char) attrLens [a];
    p_bufptr[i++] = (uchar) attrTyps [a];
    p_bufptr[i++] =         attrAlign [a];
  #ifdef SCAN
    	p_bufptr[i++] = attrSelScan [a];
  #else
    	p_bufptr[i++] = attrSelScanAggr [a];
  #endif
  }

  // first 4k is for filter, second is for sqleng
  if (i >= 4*1024) {
    std::cout << "filterBuf > 4k" << std::endl;
    return false;
  }
  i = 4*1024;

  //createSqlengineSetup((char *)(p_bufptr + i));
  return true;
}

int processQuerySwEmu(XdbConn **xdbConns, int kernels, 
                      unsigned char *setupbuf, 
                      FpgaTaskState * fpgaTaskState) {
  // Current query assumes that the result is less than the max page size, okay for query
  //      improve it to handle multiple page return when needed
  // Query Processing - 
  //     Iterate over linetable, get appropriate fields
  //     Create pages and send to FPGA
  //     merge results into host CPU

  // todo One PUBlocksize sufficient to hold all setup data use more if needed
  int status = EXIT_SUCCESS;
  bool remainingPages = true;
  int noOfPages=0;
  Relation rel = fpgaTaskState->css.ss.ss_currentRelation;
 
  XdbTimer timer;
  bool isFirstBlk = fpgaTaskState->send_state.is_first_blk;
  if(isFirstBlk==true) {
	fpgaTaskState->send_state.is_first_blk = false;
    //char *tbInbuf = xdbConns[0]->getWriteBuf();
    char *tbInbuf = xdbConns[0]->getWriteBuf();

	// hardocded filter
  //fillSetupBlock(tbInbuf);
  //return EXIT_FINISH;

	//progammable filter
	memcpy(tbInbuf, setupbuf, 4096 * 2);//4096);
  //confDump(setupbuf + 4096);
  /*
  std::cout << "Calling Query....\n" << std::endl;
  unsigned char scanType = KERNEL_MODE_SCANONLY;
  if (! confq6Verify (setupbuf + 4096, scanType)) {
    std::cout << "confq6Verify failed" << std::endl;
  }
  std::cout << "confq6Verify passed" << std::endl;
  */
#if 1
   // FILTER STREAM 
  /* 
  int i = 0;
   printf("hardcoded filterstream\n");
   for (i = 3; i<4096; i++) {
      printf("0x%02x ", tbInbuf[i]);
      if (((i + 2) % 4 == 0) && i)
         printf("\n");
   }
   */
    // engine stream
   // printf("Done memcpy Setupbuf: %s\n",tbInbuf);
   //elog(INFO, "setupstream after setupbuf filterstream %s", setupbuf + 4098);
   //elog(INFO, "setupstream after filterstream %s", tbInbuf + 4098);
#endif
    timer.start();
    if(xpg_codegendebug)
      printf("Calling Enqueue compute...\n");
    xdbConns[0]->enqueueCompute();
    if(xpg_codegendebug)
      printf("Calling Finish commands..\n");
    xdbConns[0]->finishCommands();
    if(xpg_codegendebug)
      printf("Done finish command..\n");
  }

  timer.end();
  double secTime = timer.duration();
  if(xpg_codegendebug)
    printf("Total setup time: %.4lf sec. \n", secTime);    
  
  //return 1;
  // Board offload todo add threading later
  int curQueue = 0;
  int lastQueue = 0;
  for (int i =0; i< kernels; i++) {
    xdbConns[i]->setWriteBufCopyFuncPtr(writeSqlQueryBufCopy);
    xdbConns[i]->setReadBufCopyFuncPtr(readSqlQueryBufCopy);
  }

  //XdbTimer timer;
  //timer.start();
  int numFetchedTups = 0;
  instr_time dataMoveBeg, dataMoveEnd;
  if (xpg_timerOn) {
    INSTR_TIME_SET_CURRENT(dataMoveBeg);
  }

  int queueDepth = xdbConns[0]->getQueueDepth();
  for (int queueNum = 0; queueNum < queueDepth; ++queueNum) {
    unsigned char *tbInbufPtr = (unsigned char*)xdbConns[0]->getWriteBuf(queueNum);
    fpgaTaskState->send_state.curr_queue_id = queueNum;

    //elog(INFO, "SREE: about to call xpg_fill_block for queue %d", queueNum);

    numFetchedTups = xpg_fill_block(rel, tbInbufPtr, fpgaTaskState);
    if (numFetchedTups == 0) {
      status = EXIT_FINISH;
      break;
    }
    fpgaTaskState->receive_state.total_queues = queueNum+1;
    xdbConns[0]->enqueueCompute(&(xpg_buffer_container.list[queueNum].result_buffer));
  }
  if (xpg_timerOn) {
    INSTR_TIME_SET_CURRENT(dataMoveEnd);
    INSTR_TIME_SUBTRACT(dataMoveEnd, dataMoveBeg);
    xpg_datasendTime = INSTR_TIME_GET_MILLISEC(dataMoveEnd);
    if(xpg_codegendebug) 
      elog(INFO,"datasend time %f ms \n", xpg_datasendTime);
  }

  xdbConns[0]->finishCommands();
  
  if (xpg_timerOn) {
    INSTR_TIME_SET_CURRENT(receiveBeg);
  }

  //}

  //timer.end();
  //double secTime = timer.duration();
  //double mbDataXfer = double(noOfPages)*double(QDefs::WriteBlockSize)/double(1024*1024);
  //printf("Total time: %.4lf sec. Total data: %.2lf MB. Efffective Rate: %.3f MB/s\n", secTime, mbDataXfer, mbDataXfer/secTime);   
  //printf("Info: For query processed %d pages. %d rows\n", noOfPages,noOfRows);
  return status;
}
