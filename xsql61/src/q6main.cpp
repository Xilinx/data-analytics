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
#include "sqlq6defs.h"
#include "sqldecimalutils.h"

//Query 6 processing 
void readSqlQuery6BufCopy(XdbConn *xdbConn, int queueNo, cl_event event, void *userData) {
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
    SqlDec17P2D *query6Result = (SqlDec17P2D *)userData;
    SqlDec17P2D pageResult ;
    pageResult.binStrToBin(readBuf,0); 
    query6Result->fixedBin = query6Result->fixedBin + pageResult.fixedBin;
    //DEBUG: printf("Page result= %d ", queueNo); pageResult.printDec();  query6Result->printDec();    
  }
  if (event != NULL)  { // in case of event syncs
    cl_int err = clSetUserEventStatus(event, CL_COMPLETE);
    xdbConn->getOclAccel().printError(err,"Read event generation failed");
  }
}

void writeSqlQuery6BufCopy(XdbConn *xdbConn, int queueNo, cl_event event, void *userData) {
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

int processQuery6(XdbConn **xdbConns, int kernels, SqlTbl& sqlHost) { 
  
  // Current query assumes that the result is less than the max page size, okay for query6
  //      improve it to handle multiple page return when needed
  // Query 6 -
  //     Iterate over linetable, get appropriate fields
  //     Create pages and send to FPGA
  //     merge results into host CPU

  // linetable has now following elements in the order
  // 4  L_QUANTITY    DECIMAL(15,2) NOT NULL,
  // 5  L_EXTENDEDPRICE  DECIMAL(15,2) NOT NULL,
  // 6  L_DISCOUNT    DECIMAL(15,2) NOT NULL,
  // 10  L_SHIPDATE    DATE NOT NULL,
  sqlHost.pageFields = (0x1<<4) | (0x1<<5) | (0x1<<6) | (0x1<<10) ;
  sqlHost.rowBuf.clear();
  sqlHost.maxPUBlockSize = SqlQ6Defs::ProcUnitBlockSize;
  sqlHost.maxRowsInPUBlock =  SqlQ6Defs::ProcUnitBlockMaxRows;
  bool remainingPages = true;
  int noOfPages=0;
  
  SqlDec17P2D query6Result ;
  query6Result.fixedBin = 0;
  char puBlockBuffer[SqlQ6Defs::ProcUnitBlockSize];
  uint8_t puBlockCount=0;
  //DEBUG: int noOfRows = 0;
  printf("INFO: FPGA programmed with %d Query 6 PUs processing %d PU Block size. Each Write Data Block (transferred from Host to FPGA Device DDR) Size is %d B and Read Data Block (transferred from FPGA Device DDR to Host Memory) Size is %d B\n",SqlQ6Defs::ProcUnitCountForBlock, SqlQ6Defs::ProcUnitBlockSize, SqlQ6Defs::WriteBlockSize, SqlQ6Defs::ReadBlockSize);
  printf("INFO: Processing data blocks on FPGA device ");
  int curKernel = 0;
  for (int i =0; i< kernels; i++) {
    xdbConns[i]->setWriteBufCopyFuncPtr(writeSqlQuery6BufCopy);
    xdbConns[i]->setReadBufCopyFuncPtr(readSqlQuery6BufCopy);
  }
  
  XdbTimer timer;
  char *tbInbuf = xdbConns[curKernel]->getWriteBuf();
  char *tbInbufPtr = tbInbuf;
  timer.start();
  while (remainingPages) {
    remainingPages = sqlHost.fetchNextPage(puBlockBuffer);
    puBlockCount++;
    memcpy(tbInbufPtr,puBlockBuffer,SqlQ6Defs::ProcUnitBlockSize);
    tbInbufPtr += SqlQ6Defs::ProcUnitBlockSize;
    if (puBlockCount == SqlQ6Defs::ProcUnitCountForBlock || !remainingPages) {      
      *((uint8_t *)tbInbuf ) = puBlockCount;
      //DEBUG: noOfRows += checkBlock(tbInbuf, SqlQ6Defs::ProcUnitBlockSize, puBlockCount, false);
      xdbConns[curKernel]->enqueueCompute(&query6Result);
      noOfPages++;
      curKernel++;
      curKernel = curKernel%kernels; // wrap around when max kernels reached	
      //reset pu block counters
      puBlockCount = 0;
      tbInbuf = xdbConns[curKernel]->getWriteBuf();	
      tbInbufPtr = tbInbuf;      
      printf(".");fflush(stdout);
    }
  }
  for (int i =0; i< kernels; i++)
    xdbConns[i]->finishCommands();
  timer.end();
  printf(" Done\n");fflush(stdout);
  double secTime = timer.duration();
  double mbDataXfer = double(noOfPages)*double(SqlQ6Defs::WriteBlockSize)/double(1024*1024);
  printf("INFO: Total time: %.4lf sec. Total data: %.2lf MB. Efffective Rate: %.3f MB/s\n", secTime, mbDataXfer, mbDataXfer/secTime);   
  printf("INFO: For query 6 processed %d pages\n", noOfPages);
  //DEBUG: printf("INFO: For query 6 processed %d rows\n", noOfRows);  
  printf ("INFO: Query6 result = ");
  query6Result.printDec(2); // One mult shifts by 1 dec
  printf("  Hex = ");
  query6Result.printHex();
  return EXIT_SUCCESS;
}
