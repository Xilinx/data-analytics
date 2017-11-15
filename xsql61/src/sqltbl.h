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
#ifndef XSQL61_SQLTBL_H
#define XSQL61_SQLTBL_H

#include "sqldefs.h"
#include <vector>
#include <string>

struct SqlTbl {
  void setSqlDb(const char *db, char delim) { tblFileName = db; delimiter=delim; }
  
  int  readTbl();  
  void close();
  
  // For now support up to 32 fields, expand later as needed
  bool fetchNextPage(char *bufRam);

  // TBL related options
  std::string tblFileName;
  char *tblFile;
  int ntuples;
  int processedTuples;
  char *curVarCh;
  char delimiter;

  std::vector<char> rowBuf;
  unsigned int pageFields;

  unsigned int maxPUBlockSize;
  unsigned int maxRowsInPUBlock;
  
};

#endif
