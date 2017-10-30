/*******************************************************************************
Vendor: Xilinx
Revision History:
Oct 11, 2016: initial release
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

/*

CREATE TABLE lineitem ( L_ORDERKEY    INTEGER NOT NULL,
1                             L_PARTKEY     INTEGER NOT NULL,
2                             L_SUPPKEY     INTEGER NOT NULL,
3                             L_LINENUMBER  INTEGER NOT NULL,
4                             L_QUANTITY    DECIMAL(15,2) NOT NULL,
5                             L_EXTENDEDPRICE  DECIMAL(15,2) NOT NULL,
6                             L_DISCOUNT    DECIMAL(15,2) NOT NULL,
7                             L_TAX         DECIMAL(15,2) NOT NULL,
8                             L_RETURNFLAG  CHAR(1) NOT NULL,
9                             L_LINESTATUS  CHAR(1) NOT NULL,
10                             L_SHIPDATE    DATE NOT NULL,
11                             L_COMMITDATE  DATE NOT NULL,
12                             L_RECEIPTDATE DATE NOT NULL,
13                             L_SHIPINSTRUCT CHAR(25) NOT NULL,
14                             L_SHIPMODE     CHAR(10) NOT NULL,
15                             L_COMMENT      VARCHAR(44) NOT NULL);

select
	sum(l_extendedprice * l_discount) as revenue
from
	lineitem
where
	l_shipdate >= date '1994-01-01'
	and l_shipdate < date '1994-01-01' + interval '1' year
	and l_discount between 0.06 - 0.01 and 0.06 + 0.01
	and l_quantity < 24;

*/
#include <string.h>
#include <hls_stream.h>
#include "sqldecimal.h"
#include "sqldate.h"
#include "sqldefs.h"
#include "sqlq1defs.h"
#include "sqlq1tpch.h"

void SQLEVAL(readQ1Buf,SQLKERNEL_ID)(SqlDefs::RdDdrWord *inbuf,
             hls::stream<SqlDefs::RdDdrWord> bufStream[SqlQ1Defs::ProcUnitCountForBlock],
             hls::stream<unsigned char> &noOfProcUnitBlocksStream) {
  SqlDefs::RdDdrWord bufWord;
  unsigned char noOfProcUnitBlocks;
  unsigned int startAddr;
  //For each word, feed each proc unit
  LOOP_BURST_BLOCK: for (unsigned int ml=0; ml<SqlQ1Defs::ProcUnitBlockSize/SqlDefs::RdDdrBurstSize; ml++) {
  LOOP_PU_BUF: for (unsigned int pu=0; pu<SqlQ1Defs::ProcUnitCountForBlock; pu++) {
          startAddr = pu*SqlQ1Defs::ProcUnitBlockSize/SqlDefs::RdDdrWordSize + ml*SqlDefs::RdDdrBurstLen;
      LOOP_BURST_I : for (unsigned int i=0; i < SqlDefs::RdDdrBurstLen; i++) {
#pragma HLS PIPELINE
              bufWord = inbuf[startAddr + i];
              if ((ml==0) && (pu==0) && (i==0)) {
                  noOfProcUnitBlocks = bufWord & 0x00FF;
                  noOfProcUnitBlocksStream.write(noOfProcUnitBlocks);
                  //printf("noOfProcUnitBlocks=%d\n", noOfProcUnitBlocks);
              }
      
              //if (pu < noOfProcUnitBlocks)
              bufStream[pu].write(bufWord);
          }
      }
  }
}

SqlQ1GrpData SQLEVAL(getPUResult,SQLKERNEL_ID)(hls::stream<SqlDefs::RdDdrWord> &axibufStream,
                         unsigned char block_num, unsigned char noOfProcUnitBlocks) {
#pragma HLS FUNCTION_INSTANTIATE variable=block_num

  SqlQ1GrpData result;
  SQLEVAL(sqlq1procunit,SQLKERNEL_ID)(result, axibufStream, block_num, noOfProcUnitBlocks);
  return result;

}

void SQLEVAL(getPUResultAll,SQLKERNEL_ID)(SqlQ1GrpData& result,
		    hls::stream<SqlDefs::RdDdrWord> bufStream[SqlQ1Defs::ProcUnitCountForBlock],
		    hls::stream<unsigned char> &noOfProcUnitBlocksStream) {

  SqlQ1GrpData results[SqlQ1Defs::ProcUnitCountForBlock];
  unsigned char noOfProcUnitBlocks;
  noOfProcUnitBlocks = noOfProcUnitBlocksStream.read();

 LOOP_PUPROC: for (unsigned int pu=0; pu<SqlQ1Defs::ProcUnitCountForBlock; pu++) {
#pragma HLS UNROLL
    results[pu].SQLEVAL(init,SQLKERNEL_ID)();
    results[pu] = SQLEVAL(getPUResult,SQLKERNEL_ID)(bufStream[pu], pu, noOfProcUnitBlocks);    
    result.SQLEVAL(add,SQLKERNEL_ID)(results[pu]);
  }
}

// todo make it more efficient
void SQLEVAL(writeQ1Buf,SQLKERNEL_ID)(char *outbuf, SqlQ1GrpData& result) {

  char resStr[SqlQ1Defs::ReadBlockSize];
  unsigned int binLoc = 0;
  binLoc = result.SQLEVAL(binStr,SQLKERNEL_ID)(resStr,binLoc);

  /*  SqlQ1GrpData newData;
  newData.binStrToBin(resStr,0);
  newData.print();*/
  
 LOOP_BUF: for (unsigned int sz=0; sz<SqlQ1Defs::ReadBlockSize; sz++) {
    if (sz == binLoc) break;
#pragma HLS PIPELINE
    outbuf[sz] = resStr[sz];
  }
}

extern "C" void SQLEVAL(sqlq1tpch,SQLKERNEL_ID)(SqlDefs::RdDdrWord *inbuf, char *outbuf) {
#pragma HLS INTERFACE m_axi port=inbuf offset=slave depth=262144 bundle=gmem0
#pragma HLS INTERFACE m_axi port=outbuf offset=slave depth=256 bundle=gmem1
#pragma HLS INTERFACE s_axilite port=inbuf bundle=control
#pragma HLS INTERFACE s_axilite port=outbuf bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATAFLOW
  hls::stream<SqlDefs::RdDdrWord> bufStream[SqlQ1Defs::ProcUnitCountForBlock];
#pragma HLS STREAM variable=bufStream depth=1024 dim=1
#pragma HLS ARRAY_PARTITION variable=bufStream complete dim=1
  hls::stream<unsigned char> noOfProcUnitBlocksStream;

  SqlQ1GrpData result;
  result.SQLEVAL(init,SQLKERNEL_ID)();
  SQLEVAL(readQ1Buf,SQLKERNEL_ID)(inbuf, bufStream, noOfProcUnitBlocksStream); 
  SQLEVAL(getPUResultAll,SQLKERNEL_ID)(result,bufStream, noOfProcUnitBlocksStream);
  SQLEVAL(writeQ1Buf,SQLKERNEL_ID)(outbuf, result);
}

