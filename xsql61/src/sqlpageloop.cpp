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
