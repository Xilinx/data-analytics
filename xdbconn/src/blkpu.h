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
