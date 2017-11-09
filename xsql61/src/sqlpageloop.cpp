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
