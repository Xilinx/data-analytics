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
