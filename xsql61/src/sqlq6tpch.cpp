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
#include "sqlq6tpch.h"
#include "sqlq6defs.h"
#include "sqlq6procunit.h"

void SQLEVAL(readQ6Buf,SQLKERNEL_ID)(SqlDefs::RdDdrWord *inbuf,
             hls::stream<SqlDefs::RdDdrWord> bufStream[SqlQ6Defs::ProcUnitCountForBlock],
             hls::stream<unsigned char> &noOfProcUnitBlocksStream) {
  SqlDefs::RdDdrWord bufWord;
  unsigned char noOfProcUnitBlocks;
  unsigned int startAddr;
  //For each word, feed each proc unit
  LOOP_BURST_BLOCK: for (unsigned int ml=0; ml<SqlQ6Defs::ProcUnitBlockSize/SqlDefs::RdDdrBurstSize; ml++) {
  LOOP_PU_BUF: for (unsigned int pu=0; pu<SqlQ6Defs::ProcUnitCountForBlock; pu++) {
          startAddr = pu*SqlQ6Defs::ProcUnitBlockSize/SqlDefs::RdDdrWordSize + ml*SqlDefs::RdDdrBurstLen;
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

SqlDec17P2D SQLEVAL(getPURevenue,SQLKERNEL_ID)(hls::stream<SqlDefs::RdDdrWord> &axibufStream,
                         unsigned char block_num, unsigned char noOfProcUnitBlocks) {
#pragma HLS FUNCTION_INSTANTIATE variable=block_num

  SqlDec17P2D ret_val;

  //char *start_addr;
  //start_addr = axibuf + block_num*SqlDefs::ProcUnitBlockSize;
  
  //  SqlDefs::RdDdrWord bufRam [SqlDefs::ProcUnitBlockSize/SqlDefs::RdDdrWordSize];

  //if (block_num < noOfProcUnitBlocks) {
    //  LOOP_INMCPY: for (unsigned int ml=0; ml<SqlDefs::ProcUnitBlockSize; ml++) {
    //#pragma HLS PIPELINE
    //        bufRam[ml] = axibufStream.read();
    //    }
  ret_val = SQLEVAL(sqlq6procunit,SQLKERNEL_ID)(axibufStream,  block_num, noOfProcUnitBlocks);
  //}

  return ret_val;
}

SqlDec17P2D SQLEVAL(getPURevenueAll,SQLKERNEL_ID)(hls::stream<SqlDefs::RdDdrWord> bufStream[SqlQ6Defs::ProcUnitCountForBlock],
                            hls::stream<unsigned char> &noOfProcUnitBlocksStream) {
  SqlDec17P2D ret_val;
  SqlDec17P2D revenues[SqlQ6Defs::ProcUnitCountForBlock];
  unsigned char noOfProcUnitBlocks;
  noOfProcUnitBlocks = noOfProcUnitBlocksStream.read();

 LOOP_PUPROC: for (unsigned int pu=0; pu<SqlQ6Defs::ProcUnitCountForBlock; pu++) {
#pragma HLS UNROLL
    revenues[pu].fixedBin = SQLEVAL(getPURevenue,SQLKERNEL_ID)(bufStream[pu], pu, noOfProcUnitBlocks).fixedBin;
    ret_val.fixedBin += revenues[pu].fixedBin;
  }
  return ret_val;
}

void SQLEVAL(writeQ6Buf,SQLKERNEL_ID)(char *outbuf, SqlDec17P2D revenue) {
  char revStr[SqlQ6Defs::ReadBlockSize];
  revenue.SQLEVAL(binStr,SQLKERNEL_ID)(revStr,0);
LOOP_OUTMEMCPY: for (unsigned int ml=0; ml<SqlQ6Defs::ReadBlockSize; ml++) {
#pragma HLS PIPELINE
    outbuf[ml] = revStr[ml];
  }
}

extern "C" void SQLEVAL(sqlq6tpch,SQLKERNEL_ID)  (SqlDefs::RdDdrWord *inbuf, char *outbuf) {
#pragma HLS INTERFACE m_axi port=inbuf offset=slave depth=262144 bundle=gmem0
#pragma HLS INTERFACE m_axi port=outbuf offset=slave depth=256 bundle=gmem1
#pragma HLS INTERFACE s_axilite port=inbuf bundle=control
#pragma HLS INTERFACE s_axilite port=outbuf bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATAFLOW
  hls::stream<SqlDefs::RdDdrWord> bufStream[SqlQ6Defs::ProcUnitCountForBlock];
#pragma HLS STREAM variable=bufStream depth=1024 dim=1
#pragma HLS ARRAY_PARTITION variable=bufStream complete dim=1
  hls::stream<unsigned char> noOfProcUnitBlocksStream;
  //unsigned char noOfProcUnitBlocks;
  SqlDec17P2D revenue;

  SQLEVAL(readQ6Buf,SQLKERNEL_ID)(inbuf, bufStream, noOfProcUnitBlocksStream);
  revenue.fixedBin = SQLEVAL(getPURevenueAll,SQLKERNEL_ID)(bufStream, noOfProcUnitBlocksStream).fixedBin;
  SQLEVAL(writeQ6Buf,SQLKERNEL_ID)(outbuf, revenue);
}

