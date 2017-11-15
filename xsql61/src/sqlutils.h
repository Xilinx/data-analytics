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
#ifndef XSQL61_SQLUTILS_H
#define XSQL61_SQLUTILS_H

#include "sqldefs.h"
#include "xdbutils.h"
#include "sqldecimal.h"

int checkBlock(const char *bufRam, int puBlockSize, uint8_t puBlockCount, bool print);
int checkPage(const char *bufRam, int blockSize, bool print);
int comparePages(const char *buf0, const char *buf1, int blockSize);

void printStrLen(const char* ch, unsigned st, unsigned len);
void printStr(SqlDefs::RdDdrWord& rdWord);
#endif
