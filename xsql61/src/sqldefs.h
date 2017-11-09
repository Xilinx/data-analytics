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
