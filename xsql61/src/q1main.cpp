/**********
Copyright (c) 2017, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/
#include "main.h"
#include "sqlq1defs.h"

/// Query 1 Functions
void readSqlQuery1BufCopy(XdbConn *xdbConn, int queueNo, cl_event event, void *userData) {
  printf("+");fflush(stdout);  
  if (xdbConn == nullptr) {
    printf("Error: No xdbconn in Read %d\n", queueNo);
    return;
  }  
  char *readBuf = xdbConn->getReadBuf(queueNo);
  if (readBuf == nullptr) {
    printf("Error: Read %d failed\n", queueNo);
    return;
  }
  if (userData != nullptr) {
    SqlQ1GrpData pageResult;
    pageResult.binStrToBin(readBuf, 0);
    SqlQ1GrpData *query1Result = (SqlQ1GrpData *)userData;
    query1Result->SQLEVAL(add,SQLKERNEL_ID)(pageResult);
    //DEBUG: printf("Page result= %d ", queueNo); pageResult.printDec();  query6Result->printDec();    
  }
  if (event != NULL)  { // in case of event syncs
    cl_int err = clSetUserEventStatus(event, CL_COMPLETE);
    xdbConn->getOclAccel().printError(err,"Read event generation failed");
  }
}

void writeSqlQuery1BufCopy(XdbConn *xdbConn, int queueNo, cl_event event, void *userData) {
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

int processQuery1(XdbConn **xdbConns, int kernels, SqlTbl& sqlHost) { 

  // Current query assumes that the result is less than the max page size, okay for query1
  //      improve it to handle multiple page return when needed
  // Query 1 -
  //     Iterate over linetable, get appropriate fields
  //     Create pages and send to FPGA
  //     merge results into host CPU

  // linetable has now following elements in the order
  // 4  L_QUANTITY    DECIMAL(15,2) NOT NULL,
  // 5  L_EXTENDEDPRICE  DECIMAL(15,2) NOT NULL,
  // 6  L_DISCOUNT    DECIMAL(15,2) NOT NULL,
  // 7  L_TAX         DECIMAL(15,2) NOT NULL,  
  // 8  L_RETURNFLAG  CHAR(1) NOT NULL,
  // 9  L_LINESTATUS  CHAR(1) NOT NULL,
  // 10  L_SHIPDATE    DATE NOT NULL,
  sqlHost.pageFields = (0x1<<4) | (0x1<<5) | (0x1<<6) |  (0x1<<7) | (0x1<<8) | (0x1<<9) |(0x1<<10) ;
  sqlHost.rowBuf.clear();
  sqlHost.maxPUBlockSize = SqlQ1Defs::ProcUnitBlockSize;
  sqlHost.maxRowsInPUBlock =  SqlQ1Defs::ProcUnitBlockMaxRows;
  bool remainingPages = true;
  int noOfPages=0;
  
  SqlQ1GrpData query1Result;
  query1Result.SQLEVAL(init,SQLKERNEL_ID)();

  char puBlockBuffer[SqlQ1Defs::ProcUnitBlockSize];
  uint8_t puBlockCount=0;
  //DEBUG: int noOfRows = 0;

  printf("INFO: FPGA programmed with %d Query 6 PUs processing %d PU Block size. Each Write Data Block (transferred from Host to FPGA Device DDR) Size is %d B and Read Data Block (transferred from FPGA Device DDR to Host Memory) Size is %d B\n",SqlQ1Defs::ProcUnitCountForBlock, SqlQ1Defs::ProcUnitBlockSize, SqlQ1Defs::WriteBlockSize, SqlQ1Defs::ReadBlockSize);
  printf("INFO: Processing data blocks on FPGA device ");  
  XdbTimer timer;
  for (int i=0; i< kernels; i++) {
    xdbConns[i]->setWriteBufCopyFuncPtr(writeSqlQuery1BufCopy);
    xdbConns[i]->setReadBufCopyFuncPtr(readSqlQuery1BufCopy);
  }
  int curKernel = 0;
  char *tbInbuf = xdbConns[curKernel]->getWriteBuf();
  char *tbInbufPtr = tbInbuf;
  timer.start();
  while (remainingPages) {
    remainingPages = sqlHost.fetchNextPage(puBlockBuffer);
    puBlockCount++;
    memcpy(tbInbufPtr,puBlockBuffer,SqlQ1Defs::ProcUnitBlockSize);
    tbInbufPtr += SqlQ1Defs::ProcUnitBlockSize;
    if (puBlockCount == SqlQ1Defs::ProcUnitCountForBlock || !remainingPages) {      
      *((uint8_t *)tbInbuf ) = puBlockCount;
      //DEBUG: noOfRows += checkBlock(tbInbuf, SqlQ1Defs::ProcUnitBlockSize, puBlockCount, false);
      xdbConns[curKernel]->enqueueCompute(&query1Result);
      curKernel++;
      curKernel = curKernel%kernels;
      noOfPages++;
      //reset pu block counters
      puBlockCount = 0;
      tbInbuf = xdbConns[curKernel]->getWriteBuf();	
      tbInbufPtr = tbInbuf;      
      printf("."); fflush(stdout);
    }
  }
  for (int i =0; i< kernels; i++)  
    xdbConns[curKernel]->finishCommands();
  timer.end();
  printf(" Done\n");fflush(stdout);
  double secTime = timer.duration();
  double mbDataXfer = double(noOfPages)*double(SqlQ1Defs::WriteBlockSize)/double(1024*1024);
  printf("Total time: %.4lf sec. Total data: %.2lf MB. Efffective Rate: %.3f MB/s\n", secTime, mbDataXfer, mbDataXfer/secTime);   
  printf("INFO: For query 1 processed %d pages\n", noOfPages);
  //DEBUG: printf("INFO: For query 1 processed %d rows\n", noOfRows);  
  printf ("Query1 result = \n");
  query1Result.print();
  return EXIT_SUCCESS;
}
