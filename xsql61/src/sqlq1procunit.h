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
