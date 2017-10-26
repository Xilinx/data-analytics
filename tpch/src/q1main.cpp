/*******************************************************************************
Vendor: Xilinx
Revision History:
Oct 25, 2016: initial release
*******************************************************************************
Copyright (C) 2016 XILINX, Inc.

This file contains confidential and proprietary information of Xilinx, Inc. and
is protected under U.S. and international copyright and other intellectual
property laws.

DISCLAIMER
This disclaimer is not a license and does not grant any rights to the materials
distributed herewith. Except as otherwise provided in a valid license issued to
you by Xilinx, and to the maximum extent permitted by applicable law:
(1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, AND XILINX
HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY,
INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, OR
FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable (whether
in contract or tort, including negligence, or under any other theory of
liability) for any loss or damage of any kind or nature related to, arising under
or in connection with these materials, including for any direct, or any indirect,
special, incidental, or consequential loss or damage (including loss of data,
profits, goodwill, or any type of loss or damage suffered as a result of any
action brought by a third party) even if such damage or loss was reasonably
foreseeable or Xilinx had been advised of the possibility of the same.

CRITICAL APPLICATIONS
Xilinx products are not designed or intended to be fail-safe, or for use in any
application requiring fail-safe performance, such as life-support or safety
devices or systems, Class III medical devices, nuclear facilities, applications
related to the deployment of airbags, or any other applications that could lead
to death, personal injury, or severe property or environmental damage
(individually and collectively, "Critical Applications"). Customer assumes the
sole risk and liability of any use of Xilinx products in Critical Applications,
subject only to applicable laws and regulations governing limitations on product
liability.

THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT
ALL TIMES.

*******************************************************************************/

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
