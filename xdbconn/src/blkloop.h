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

#ifndef XDBCONN_BLKLOOP_H
#define XDBCONN_BLKLOOP_H

#include <ap_int.h>
#include <hls_stream.h>

#ifdef HW_EMU_DEBUG
// Settings for 1024 Bytes (64*32) block of data
// Minimum blocksize must be ProcUnitBlockSize*ProcUnitCountForBlock
const unsigned int ProcUnitBlockSize =64;
const unsigned int ProcUnitCountForBlock = 32;
// DdrBurstSize is fixed to WordSize*BurstLen, must be less than ProcUnitBlockSize
const unsigned int DdrWordSize = 64;
const unsigned int DdrBurstLen = 4; 
const unsigned int DdrBurstSize = DdrWordSize*DdrBurstLen;

#else
// Settings for ~2 MB (64*32768) block of data
// Minimum blocksize must be ProcUnitBlockSize*ProcUnitCountForBlock
const unsigned int ProcUnitBlockSize = 32*32768;
const unsigned int ProcUnitCountForBlock = 2;
// DdrBurstSize is fixed to WordSize*BurstLen, must be less than ProcUnitBlockSize
const unsigned int DdrWordSize = 64;
const unsigned int DdrBurstLen = 64; 
const unsigned int DdrBurstSize = DdrWordSize*DdrBurstLen;
#endif

typedef ap_int<8*DdrWordSize> DdrWord;

extern "C" void blkloop(DdrWord *inbuf, DdrWord *outbuf);

#endif
