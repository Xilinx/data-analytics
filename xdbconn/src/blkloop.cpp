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

#include "blkloop.h"
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
	      hls::stream<DdrWord> bufStream[ProcUnitCountForBlock]) {
  
  DdrWord bufWord;
  unsigned int startAddr;
  
  LOOP_BURST_BLOCK: for (unsigned int ml=0; ml<ProcUnitBlockSize/DdrBurstSize; ml++) {
  LOOP_PU_BUF: for (unsigned int pu=0; pu<ProcUnitCountForBlock; pu++) {
      startAddr = pu*ProcUnitBlockSize/DdrWordSize + ml*DdrBurstLen;
    LOOP_BURST_I : for (unsigned int i=0; i < DdrBurstLen; i++) {
#pragma HLS PIPELINE
	bufStream[pu].read(bufWord);
	outbuf[startAddr + i] = bufWord;	
      }
    }
  }  
}

// This kernel takes inbuf and loops back data to outbuf for KernelBlockDepth Bytes
extern "C" void blkloop(DdrWord *inbuf, DdrWord *outbuf) {
#pragma HLS INTERFACE m_axi port=inbuf offset=slave depth=2097152 bundle=gmem
#pragma HLS INTERFACE m_axi port=outbuf offset=slave depth=2097152 bundle=gmem
#pragma HLS INTERFACE s_axilite port=inbuf bundle=control
#pragma HLS INTERFACE s_axilite port=outbuf bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATAFLOW

  hls::stream<DdrWord> bufStream[ProcUnitCountForBlock];
#pragma HLS STREAM variable=bufStream depth=64 dim=1
#pragma HLS ARRAY_PARTITION variable=bufStream complete dim=1

  readBuf(inbuf, bufStream);
  writeBuf(outbuf,bufStream);

}
