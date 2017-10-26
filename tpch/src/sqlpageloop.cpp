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

#include "sqlpageloop.h"
#include "sqldefs.h"
#include "sqlq6defs.h"
#include <string.h>
#include <stdio.h>


void SQLEVAL(readBuf,SQLKERNEL_ID)(SqlDefs::RdDdrWord *rdDdrBuf,
             hls::stream<SqlDefs::RdDdrWord>& bufStream) {
  unsigned int totalRdWords = SqlQ6Defs::WriteBlockSize/SqlDefs::RdDdrWordSize;
 LOOP_INMCPY: for (unsigned int ml=0; ml<totalRdWords; ml++) {
#pragma HLS PIPELINE
    SqlDefs::RdDdrWord bufRead = rdDdrBuf[ml];
    bufStream.write(bufRead);
  }
}

void SQLEVAL(writeBuf,SQLKERNEL_ID)(SqlDefs::RdDdrWord *wrDdrBuf,
	      hls::stream<SqlDefs::RdDdrWord>& bufStream) {	      

  unsigned int totalWrWords = SqlQ6Defs::WriteBlockSize/SqlDefs::RdDdrWordSize;
 LOOP_OUTMCPY: for (unsigned int ml=0; ml< totalWrWords; ml++) {
#pragma HLS PIPELINE
    wrDdrBuf[ml] = bufStream.read();
  }
}

void SQLEVAL(writeBuf,SQLKERNEL_ID)(char *outbuf,
	      hls::stream<SqlDefs::RdDdrWord>& bufStream) {	      	      

  SqlDefs::RdTracker rdTrack;
  rdTrack.curChIdx = 0;
  rdTrack.curIdx = 0;;

 LOOP_OUTMCPY: for (unsigned int ml=0; ml< SqlQ6Defs::WriteBlockSize; ml++) {
#pragma HLS PIPELINE
    char ch = SqlDefs::readChar(bufStream,rdTrack);
    outbuf[ml] = ch;
  }
}

extern "C" void SQLEVAL(sqlpageloop,SQLKERNEL_ID)(SqlDefs::RdDdrWord *inbuf, char /*SqlDefs::RdDdrWord*/ *outbuf) {
#pragma HLS INTERFACE m_axi port=inbuf offset=slave depth=262144 bundle=gmem
#pragma HLS INTERFACE m_axi port=outbuf offset=slave depth=2097152 bundle=gmem
#pragma HLS INTERFACE s_axilite port=inbuf bundle=control
#pragma HLS INTERFACE s_axilite port=outbuf bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATAFLOW

  hls::stream<SqlDefs::RdDdrWord> bufStream;
#pragma HLS STREAM variable=bufStream depth=16
  
  SQLEVAL(readBuf,SQLKERNEL_ID)(inbuf, bufStream);  
  SQLEVAL(writeBuf,SQLKERNEL_ID)(outbuf, bufStream);  
}
