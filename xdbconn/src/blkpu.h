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

#ifndef XDBCONN_BLKPU_H
#define XDBCONN_BLKPU_H

#include <ap_int.h>
#include <hls_stream.h>
/*
// Settings for 1024 Bytes (64*32) block of data
// Minimum blocksize must be ProcUnitBlockSize*ProcUnitCountForBlock
const unsigned int ProcUnitBlockSize = 32768;
const unsigned int ProcUnitCountForBlock = 64;
// DdrBurstSize is fixed to WordSize*BurstLen, must be less than ProcUnitBlockSize
const unsigned int DdrWordSize = 8;
const unsigned int DdrBurstLen = 1024; 
const unsigned int DdrBurstSize = DdrWordSize*DdrBurstLen;
*/
// Settings for 1024 Bytes (64*32) block of data
// Minimum blocksize must be ProcUnitBlockSize*ProcUnitCountForBlock
const unsigned int ProcUnitBlockSize =64;
const unsigned int ProcUnitCountForBlock = 32;
// DdrBurstSize is fixed to WordSize*BurstLen, must be less than ProcUnitBlockSize
const unsigned int DdrWordSize = 8;
const unsigned int DdrBurstLen = 4; 
const unsigned int DdrBurstSize = DdrWordSize*DdrBurstLen;

typedef ap_int<8*DdrWordSize> DdrWord;

const unsigned int DataWordSize = 4;
struct Reg {
  ap_int<DataWordSize*8> A;
  ap_int<DataWordSize*8> B;
  ap_int<DataWordSize*8> C;
};
typedef ap_int<8> Ops;

static Reg reg;
static Ops opcodes;

extern "C" void blkpu(DdrWord *inbuf, DdrWord *outbuf);

#endif
