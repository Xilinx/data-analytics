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

#ifndef XSQL61_SQLQ1DEFS_H
#define XSQL61_SQLQ1DEFS_H
#include <ap_int.h>
#include <hls_stream.h>

namespace SqlQ1Defs {

  // Number of possible ProcUnit in FPGA
  const unsigned int  ProcUnitCountForBlock = 8;  // Number of proc unit operations on the Blocksize
  // Each compute unit works on CUBlockSize = 32K
  const unsigned int ProcUnitBlockSize = 4*32768; // 2*32768,  32*1024,  2**5 * 2**10 = 2**15
  // Maximum number of rows in a CUBlock block
  const unsigned int ProcUnitBlockMaxRows = 2048; // 16*1024,  2**14 32  Bytes in each row
  // Block size - 2*1024 *1024; KU60 1080 36Kb = 4*1080 KB RAM; 2 2MB page in single FPGA
  // This gets transferred over PCIe link
  // BlockSize is equal to ProcUnitCount*ProcUnitBlockSize
  const unsigned int  WriteBlockSize = 2097152; //1048576; // 2097152, 2**21 ProcUnitCount*ProcUnitBlockSize, 2*1024*1024;
  const unsigned int ReadBlockSize = 4096;
  
};

#endif
