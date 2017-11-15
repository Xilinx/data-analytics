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
#ifndef XSQL61_SQLDATE_H
#define XSQL61_SQLDATE_H

#include "ap_int.h"
#include "sqldefs.h"

struct SqlDate {

  void SQLEVAL(toBin,SQLKERNEL_ID)(hls::stream<SqlDefs::RdDdrWord>& bufStream, SqlDefs::RdTracker& rdTracker);
  // data format is 4 year like YYYY-MM-DD e.g. 2016-01-21
  int toBin(char *dateStr, int curLoc);
  void print();
  
  typedef ap_uint<12> Year;
  typedef ap_uint<4>  Month;
  typedef ap_uint<5>  Day;
  
  Year  year;  // up to 2048 needs 12 bits
  Month month; // 1 - Jan, 2- Feb ... 12 - Dec
  Day   day;   // 1-31
  
};


#endif
