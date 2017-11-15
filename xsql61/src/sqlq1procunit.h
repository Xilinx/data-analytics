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
#ifndef XSQL61_SQLQ1PROCUNIT_H
#define XSQL61_SQLQ1PROCUNIT_H
#include "sqldefs.h"

struct SqlQ1GrpData {
  struct Q1Data {
    SqlDec17P2D sum_qty;
    SqlDec17P2D sum_base_price;
    SqlDec17P2D sum_disc_price;
    SqlDec17P2D sum_charge;
    SqlDec17P2D sum_disc;
    // Sql integer with 10-precision for coutn_order, 32 bit int
    SqlInt10P   count_order;
    void SQLEVAL(add,SQLKERNEL_ID)(Q1Data& qd);  
    void SQLEVAL(init,SQLKERNEL_ID)();
    int SQLEVAL(binStr,SQLKERNEL_ID)(char *bin, int binLoc);

    int binStrToBin(char *bin, int binLoc);
    void print();    
  };
  
  // ReturnFlag f and LineStatus s
  Q1Data fNsO;
  Q1Data fRsF;
  Q1Data fAsF;
  Q1Data fNsF;
  void SQLEVAL(init,SQLKERNEL_ID)();
  void SQLEVAL(add,SQLKERNEL_ID)(SqlQ1GrpData& qgd);
  int SQLEVAL(binStr,SQLKERNEL_ID)(char *bin, int binLoc);

  int binStrToBin(char *bin, int binLoc);
  void print();
};

void SQLEVAL(sqlq1procunit,SQLKERNEL_ID)(SqlQ1GrpData& res,
		   hls::stream<SqlDefs::RdDdrWord> &bufStream,
		   unsigned char block_num,
		   unsigned char noOfProcUnitBlocks);

#endif
