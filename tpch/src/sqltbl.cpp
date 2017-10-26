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

#include "sqltbl.h"

int SqlTbl::readTbl() {
  // Load tbl file to memory
  tblFile= NULL;
  const char * filename = tblFileName.c_str();
  size_t size = 0;
  FILE *f = fopen(filename, "rb");
  if (f == NULL) {
    fprintf(stderr,"ERROR: Cannot open tbl file: %s\n", filename);
    return EXIT_FAILURE;
  } 
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);
  tblFile = (char *)malloc(size+1);
  if (size != fread(tblFile, sizeof(char), size, f)) {
    fprintf(stderr,"ERROR: Cannot read file: %s\n", filename);
    free(tblFile);
    return EXIT_FAILURE;    
  }
  fclose(f);
  tblFile[size] = 0;
  curVarCh = tblFile;
  processedTuples = 0;
  
  return EXIT_SUCCESS;
}

//  char rowBuf[globals::max_row_size];
// Use 0, 1,2,3 to store page length
// return false if no more pages to be fetced
bool SqlTbl::fetchNextPage(char *bufRam) {
  
  unsigned int noOfRows = 0;
  unsigned int curBuf = 4;

  bool rowsRemaining = false;

  int curField = 0;

  while(processedTuples < ntuples) {
    char *lastVarCh = curVarCh;

    while (true) {
      if (*curVarCh == delimiter) {
	if (0x1<<curField & pageFields)      
	  bufRam[curBuf++] = *curVarCh;
	curField++;
	curVarCh++;
	continue;
      }

      if (0x1<<curField & pageFields)      
	bufRam[curBuf++] = *curVarCh;
      curVarCh++;
      
      if (*curVarCh == '\n') {
	//bufRam[curBuf++] = delimiter;
	bufRam[curBuf++] = *curVarCh++;
	curField = 0;
	break;
      }
      if (curBuf >= (maxPUBlockSize-1)) { // Need min of 2 chars to accomodate next row
	curVarCh = lastVarCh;
	rowsRemaining = true;
	break;
      }
    }
    
    if (rowsRemaining)
      break;
    processedTuples++;
    noOfRows++;
    if (noOfRows >= maxRowsInPUBlock) {
      lastVarCh = curVarCh;
      rowsRemaining = true;
      break;
    }
  }

  bufRam[0] = 0x00; // populated by the number of publocks in qmain.cpp
  bufRam[1]= (noOfRows>>16 & 0xFF);    
  bufRam[2]= (noOfRows>>8 & 0xFF);  
  bufRam[3]= (noOfRows & 0xFF);
  
  return rowsRemaining;
}
