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

#ifndef SQLENGINE_QDEFS_H
#define SQLENGINE_QDEFS_H
#include <ap_int.h>
#include <hls_stream.h>

namespace QDefs {

  // Number of possible ProcUnit in FPGA
#ifdef HW_EMU_DEBUG
  const unsigned int  ProcUnitCountForBlock = 2;//32;  // Number of proc unit operations on the Blocksize
#else
  const unsigned int  ProcUnitCountForBlock = 16;  // Number of proc unit operations on the Blocksize  
#endif  
  // Each compute unit works on CUBlockSize = 32K
#ifdef HW_EMU_DEBUG
  const unsigned int ProcUnitBlockSize = 2097152; //2048;//4*32768; // 2*32768,  32*1024,  2**5 * 2**10 = 2**15
#else
  const unsigned int ProcUnitBlockSize = 2097152; //4*32768; // 2*32768,  32*1024,  2**5 * 2**10 = 2**15
#endif
  // Maximum number of rows in a CUBlock block
#ifdef HW_EMU_DEBUG  
  const unsigned int ProcUnitBlockMaxRows = 256;//4*1024; // 16*1024,  2**14 32  Bytes in each row
#else
  const unsigned int ProcUnitBlockMaxRows = 4*1024; // 16*1024,  2**14 32  Bytes in each row  
#endif
  
  // Block size - 2*1024 *1024; KU60 1080 36Kb = 4*1080 KB RAM; 2 2MB page in single FPGA
  // This gets transferred over PCIe link
  // BlockSize is equal to ProcUnitCount*ProcUnitBlockSize
#ifdef HW_EMU_DEBUG  
  const unsigned int  WriteBlockSize = 2097152; //4096;//2097152; //1048576; // 2097152, 2**21 ProcUnitCount*ProcUnitBlockSize, 2*1024*1024;
  const unsigned int ReadBlockSize = 4096;
  const unsigned int ScanReadBlockSize = 65536;
#else
  const unsigned int  WriteBlockSize = 2097152; //1048576; // 2097152, 2**21 ProcUnitCount*ProcUnitBlockSize, 2*1024*1024;
  const unsigned int ReadBlockSize = 2097152;  
  const unsigned int ScanReadBlockSize = 65536;
#endif

  // Block data types
  const char PUSetupBlock = 0x00; // For data block, this char represents publockcount todo change data format
  const char PUSetupBlockDone = 0xff; // For data block, this char represents publockcount todo change data format

  // Query types
  const char QueryScan = 0xf0;
  const char QueryScanAggr = 0xf1;
};

#endif
