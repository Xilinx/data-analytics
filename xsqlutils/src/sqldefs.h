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

#ifndef XSQLUTILS_SQLDEFS_H
#define XSQLUTILS_SQLDEFS_H
#include <ap_int.h>
#include <hls_stream.h>

#define SQLPASTER(x,y) x ## y
#define SQLEVAL(x,y)  SQLPASTER(x,y)

// #define SQLKERNEL_ID 0 define in makefile for multiple kernels

namespace SqlDefs {

  const unsigned int RdDdrWordSize = 8; // written from host to FPGA
#ifdef HW_EMU_DEBUG
  const unsigned int RdDdrBurstLen = 128;
#else
  const unsigned int RdDdrBurstLen = 1024;
#endif
  const unsigned int RdDdrBurstSize = RdDdrWordSize*RdDdrBurstLen;
  
  const unsigned int WrDdrWordSize = 8;
#ifdef HW_EMU_DEBUG  
  const unsigned int WrDdrBurstLen = 128;
#else
  const unsigned int WrDdrBurstLen = 1024;  
#endif
  const unsigned int WrDdrBurstSize = WrDdrWordSize*WrDdrBurstLen;
  
  typedef ap_int<8*RdDdrWordSize> RdDdrWord;  
  typedef ap_int<8*WrDdrWordSize> WrDdrWord;


 struct RdTracker {
    SqlDefs::RdDdrWord dataWord;
    char lastCh;
    char curCh;
    unsigned char curChIdx;
    unsigned int curIdx;
  };

  inline char readChar(hls::stream<RdDdrWord>& bufStream, RdTracker& rdTracker) {
    if (rdTracker.curChIdx == 0)  {
      rdTracker.dataWord = bufStream.read();
      rdTracker.curIdx++;
    }
    //char ch = (rdTracker.dataWord>>(rdTracker.curChIdx*8)) & 0x000FF;
    rdTracker.lastCh = rdTracker.curCh;
    rdTracker.curCh = rdTracker.dataWord & 0x0000000FF;
    rdTracker.dataWord = rdTracker.dataWord >> 8;
    rdTracker.curChIdx++;
    if (rdTracker.curChIdx == RdDdrWordSize)
      rdTracker.curChIdx = 0;
    return rdTracker.curCh;
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
