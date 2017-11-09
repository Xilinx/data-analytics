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
