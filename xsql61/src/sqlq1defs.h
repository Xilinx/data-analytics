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
