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

#ifndef XSQL61_SQLDEFS_H
#define XSQL61_SQLDEFS_H
#include <ap_int.h>
#include <hls_stream.h>

#define SQLPASTER(x,y) x ## y
#define SQLEVAL(x,y)  SQLPASTER(x,y)

// #define SQLKERNEL_ID 0 define in makefile for multiple kernels

namespace SqlDefs {

  const unsigned int RdDdrWordSize = 8; // written from host to FPGA
  const unsigned int RdDdrBurstLen = 1024; 
  const unsigned int RdDdrBurstSize = RdDdrWordSize*RdDdrBurstLen;
  
  const unsigned int WrDdrWordSize = 8; 
  const unsigned int WrDdrBurstLen = 1024; 
  const unsigned int WrDdrBurstSize = WrDdrWordSize*WrDdrBurstLen;
  
  typedef ap_int<8*RdDdrWordSize> RdDdrWord;  
  typedef ap_int<8*WrDdrWordSize> WrDdrWord;


 struct RdTracker {
    SqlDefs::RdDdrWord dataWord;
    unsigned char curChIdx;
    unsigned int curIdx;
  };

  inline char readChar(hls::stream<RdDdrWord>& bufStream, RdTracker& rdTracker) {
    if (rdTracker.curChIdx == 0)  {
      rdTracker.dataWord = bufStream.read();
      rdTracker.curIdx++;
    }
    //char ch = (rdTracker.dataWord>>(rdTracker.curChIdx*8)) & 0x000FF;
    char ch = rdTracker.dataWord & 0x0000000FF;
    rdTracker.dataWord = rdTracker.dataWord >> 8;
    rdTracker.curChIdx++;
    if (rdTracker.curChIdx == RdDdrWordSize)
      rdTracker.curChIdx = 0;
    return ch;
  }

  inline void dbgChkSame(char ch1, char ch2) {
#ifdef HLS_DEBUG
    if (ch1 != ch2 ) { // end of row reached, new line is row marker
      printf("ERROR: Expecting %c, found %c\n",ch1,ch2);
    }
#endif    
  }

  inline void dbgChkDiff(char ch1, char ch2) {
#ifdef HLS_DEBUG
    if (ch1 == ch2 ) { // end of row reached, new line is row marker
      printf("ERROR: Musit be different from  %c, found same %c\n",ch1,ch2);
    }
#endif    
  }
  
};

#endif
