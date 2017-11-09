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

#include "blkpu.h"
#include "stdlib.h"
#include <cstring>

void readBuf(DdrWord *inbuf,
             hls::stream<DdrWord> bufStream[ProcUnitCountForBlock]) {
  
  DdrWord bufWord;
  unsigned int startAddr;
  
  LOOP_BURST_BLOCK: for (unsigned int ml=0; ml<ProcUnitBlockSize/DdrBurstSize; ml++) {
  LOOP_PU_BUF: for (unsigned int pu=0; pu<ProcUnitCountForBlock; pu++) {
      startAddr = pu*ProcUnitBlockSize/DdrWordSize + ml*DdrBurstLen;
    LOOP_BURST_I : for (unsigned int i=0; i < DdrBurstLen; i++) {
#pragma HLS PIPELINE
	bufWord = inbuf[startAddr + i];
	bufStream[pu].write(bufWord);
      }
    }
  }
}

void writeBuf(DdrWord *outbuf,
	      hls::stream<DdrWord> bufStream[ProcUnitCountForBlock],
	      Ops& opcodes,	      
	      const char totalOps) {
  
  DdrWord bufWord;
  unsigned int startAddr;
  //  int opsDone = 0;
  LOOP_BURST_BLOCK: for (unsigned int ml=0; ml<ProcUnitBlockSize/DdrBurstSize; ml++) {
  LOOP_PU_BUF: for (unsigned int pu=0; pu<ProcUnitCountForBlock; pu++) {
      startAddr = pu*ProcUnitBlockSize/DdrWordSize + ml*DdrBurstLen;
    LOOP_BURST_I : for (unsigned int i=0; i < DdrBurstLen; i++) {
#pragma HLS PIPELINE
	bufStream[pu].read(bufWord);
	DdrWord outBufWord = bufWord ;
	//todo	if (opsDone < totalOps) {
	  // for now only support addition
	  char a = bufWord & 0x000000FF;
	  char b = bufWord>>8 & 0x000000FF;	
	  char c = 0;
	  if (opcodes == 0x01)
	    c=a+b;
	  else if (opcodes == 0x02)
	    c= b-a;
	  else if (opcodes == 0x03)
	    c = b*a;
	  outBufWord = outBufWord | (c << 24);
	  //	  opsDone++;
	  //	}
	outbuf[startAddr + i] = outBufWord;	
      }
    }
  }  
}


void processData(DdrWord *inbuf, DdrWord *outbuf, Reg& reg, Ops& opcodes, const char totalOps) {
#pragma HLS DATAFLOW

  hls::stream<DdrWord> bufStream[ProcUnitCountForBlock];
#pragma HLS STREAM variable=bufStream depth=1024 dim=1
#pragma HLS ARRAY_PARTITION variable=bufStream complete dim=1

  readBuf(inbuf, bufStream);
  writeBuf(outbuf,bufStream,opcodes,totalOps);
}

void readCmdBuf(DdrWord *inbuf,
		hls::stream<DdrWord>& bufStream) {
  DdrWord bufWord;
 LOOP_CMD_BURST_I : for (unsigned int i=0; i < DdrBurstLen; i++) {
#pragma HLS PIPELINE
    bufWord = inbuf[i];
    bufStream.write(bufWord);
  }
}

void loadCommand(DdrWord *inbuf, DdrWord *outbuf, Reg& reg, Ops& opcodes) {
  //#pragma HLS DATAFLOW

  hls::stream<DdrWord> bufStream;
#pragma HLS STREAM variable=bufStream depth=1024
  readCmdBuf(inbuf, bufStream);
  DdrWord bufWord;
  bufStream.read(bufWord);
  opcodes = bufWord & 0x000000FF;
  reg.A = bufWord & 0x0000FF00;
  reg.B = bufWord & 0x00FF0000;
  reg.C = bufWord & 0xFF000000;

  // Flush the rest of burst data, readcmd buf only read one burst
 LOOP_BURST_BLOCK: for (unsigned int ml=1; ml<DdrBurstLen; ml++) {
    bufStream.read(bufWord);
  }
}

// This kernel takes inbuf and loops back data to outbuf for KernelBlockDepth Bytes
extern "C" void blkpu(DdrWord *inbuf, DdrWord *outbuf) {
#pragma HLS INTERFACE m_axi port=inbuf offset=slave depth=2097152 bundle=gmem
#pragma HLS INTERFACE m_axi port=outbuf offset=slave depth=2097152 bundle=gmem
#pragma HLS INTERFACE s_axilite port=inbuf bundle=control
#pragma HLS INTERFACE s_axilite port=outbuf bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

  DdrWord bufWord = inbuf[0];
  char blkType = bufWord & 0x000000FF;
  if (blkType == 0) {
    const char totalOps = (bufWord>>16) & 0x000000FF;
    processData(inbuf,outbuf,reg,opcodes,totalOps);
  } else {
    loadCommand(inbuf,outbuf,reg,opcodes);
  }
}
