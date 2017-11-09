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

/*

CREATE TABLE lineitem ( L_ORDERKEY    INTEGER NOT NULL,
1                             L_PARTKEY     INTEGER NOT NULL,
2                             L_SUPPKEY     INTEGER NOT NULL,
3                             L_LINENUMBER  INTEGER NOT NULL,
4                             L_QUANTITY    DECIMAL(15,2) NOT NULL,
5                             L_EXTENDEDPRICE  DECIMAL(15,2) NOT NULL,
6                             L_DISCOUNT    DECIMAL(15,2) NOT NULL,
7                             L_TAX         DECIMAL(15,2) NOT NULL,
8                             L_RETURNFLAG  CHAR(1) NOT NULL,
9                             L_LINESTATUS  CHAR(1) NOT NULL,
10                             L_SHIPDATE    DATE NOT NULL,
11                             L_COMMITDATE  DATE NOT NULL,
12                             L_RECEIPTDATE DATE NOT NULL,
13                             L_SHIPINSTRUCT CHAR(25) NOT NULL,
14                             L_SHIPMODE     CHAR(10) NOT NULL,
15                             L_COMMENT      VARCHAR(44) NOT NULL);

"select \
	l_returnflag, \
	l_linestatus, \
	sum(l_quantity) as sum_qty, \
	sum(l_extendedprice) as sum_base_price, \
	sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, \
	sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, \
	avg(l_quantity) as avg_qty, \
	avg(l_extendedprice) as avg_price, \
	avg(l_discount) as avg_disc, \
	count(*) as count_order \
from \
	lineitem \
where \
	l_shipdate <= date '1998-12-01' - interval '108' day \
group by \
	l_returnflag, \
	l_linestatus \
order by \
	l_returnflag, \
	l_linestatus"
*/
#include <string.h>
#include "sqldecimal.h"
#include "sqldate.h"
#include "sqldefs.h"
#include "sqlq1defs.h"
#include "sqlq1procunit.h"

void SqlQ1GrpData::Q1Data::SQLEVAL(init,SQLKERNEL_ID)() {
  sum_qty.fixedBin = 0;
  sum_base_price.fixedBin = 0;
  sum_disc_price.fixedBin = 0;
  sum_charge.fixedBin = 0;
  sum_disc.fixedBin = 0;
  count_order.fixedBin = 0;
};

void SqlQ1GrpData::SQLEVAL(init,SQLKERNEL_ID)() {
  fNsO.SQLEVAL(init,SQLKERNEL_ID)();
  fRsF.SQLEVAL(init,SQLKERNEL_ID)();
  fAsF.SQLEVAL(init,SQLKERNEL_ID)();
  fNsF.SQLEVAL(init,SQLKERNEL_ID)();
};

void SqlQ1GrpData::Q1Data::SQLEVAL(add,SQLKERNEL_ID)(Q1Data& qd) {
  sum_qty.fixedBin = sum_qty.fixedBin + qd.sum_qty.fixedBin;
  sum_base_price.fixedBin = sum_base_price.fixedBin + qd.sum_base_price.fixedBin ;
  sum_disc_price.fixedBin = sum_disc_price.fixedBin + qd.sum_disc_price.fixedBin ;
  sum_charge.fixedBin  = sum_charge.fixedBin  + qd.sum_charge.fixedBin;
  sum_disc.fixedBin  = sum_disc.fixedBin  + qd.sum_disc.fixedBin;  
  count_order.fixedBin = count_order.fixedBin + qd.count_order.fixedBin;
};

void SqlQ1GrpData::SQLEVAL(add,SQLKERNEL_ID)(SqlQ1GrpData& qgd) {
  fNsO.SQLEVAL(add,SQLKERNEL_ID)(qgd.fNsO);
  fRsF.SQLEVAL(add,SQLKERNEL_ID)(qgd.fRsF);
  fAsF.SQLEVAL(add,SQLKERNEL_ID)(qgd.fAsF);
  fNsF.SQLEVAL(add,SQLKERNEL_ID)(qgd.fNsF);
}

int SqlQ1GrpData::Q1Data::SQLEVAL(binStr,SQLKERNEL_ID)(char *bin, int binLoc) {
  binLoc = sum_qty.SQLEVAL(binStr,SQLKERNEL_ID)(bin,binLoc);
  binLoc = sum_base_price.SQLEVAL(binStr,SQLKERNEL_ID)(bin,binLoc);
  binLoc = sum_disc_price.SQLEVAL(binStr,SQLKERNEL_ID)(bin,binLoc);
  binLoc = sum_charge.SQLEVAL(binStr,SQLKERNEL_ID)(bin,binLoc);
  binLoc = sum_disc.SQLEVAL(binStr,SQLKERNEL_ID)(bin,binLoc);
  binLoc = count_order.SQLEVAL(binStr,SQLKERNEL_ID)(bin,binLoc);
  bin[binLoc++] = '\n';  
  return binLoc;
};

int SqlQ1GrpData::SQLEVAL(binStr,SQLKERNEL_ID)(char *bin, int binLoc) {
  binLoc = fNsO.SQLEVAL(binStr,SQLKERNEL_ID)(bin,binLoc);
  binLoc = fRsF.SQLEVAL(binStr,SQLKERNEL_ID)(bin,binLoc);
  binLoc = fAsF.SQLEVAL(binStr,SQLKERNEL_ID)(bin,binLoc);
  binLoc = fNsF.SQLEVAL(binStr,SQLKERNEL_ID)(bin,binLoc);    
  return binLoc;
}


void SQLEVAL(updateOrder,SQLKERNEL_ID)(SqlQ1GrpData::Q1Data& res, SqlDec17P2D& l_quantity, SqlDec17P2D& l_extendedprice, SqlDec17P2D& l_discount, SqlDec17P2D& l_tax) {
  res.sum_qty.fixedBin = res.sum_qty.fixedBin + l_quantity.fixedBin;
  res.sum_base_price.fixedBin = res.sum_base_price.fixedBin + l_extendedprice.fixedBin;
  SqlDec17P2D discExtendPrice;
  discExtendPrice.fixedBin =   l_extendedprice.fixedBin*(100-l_discount.fixedBin);
  res.sum_disc_price.fixedBin = res.sum_disc_price.fixedBin +  discExtendPrice.fixedBin;
  res.sum_charge.fixedBin = res.sum_charge.fixedBin + discExtendPrice.fixedBin*(100+l_tax.fixedBin);
  res.sum_disc.fixedBin = res.sum_disc.fixedBin + l_discount.fixedBin;
  res.count_order.fixedBin = res.count_order.fixedBin + 1;
}


void SQLEVAL(sqlq1procunit,SQLKERNEL_ID)(SqlQ1GrpData& res,
		   hls::stream<SqlDefs::RdDdrWord> &bufStream,
		   unsigned char block_num,
		   unsigned char noOfProcUnitBlocks)
{
#pragma HLS inline
  
  // inbuf has now following elements in the order
  // 4  L_QUANTITY    DECIMAL(15,2) NOT NULL,
  // 5  L_EXTENDEDPRICE  DECIMAL(15,2) NOT NULL,
  // 6  L_DISCOUNT    DECIMAL(15,2) NOT NULL,
  // 7  L_TAX         DECIMAL(15,2) NOT NULL,  
  // 8  L_RETURNFLAG  CHAR(1) NOT NULL,
  // 9  L_LINESTATUS  CHAR(1) NOT NULL,
  // 10  L_SHIPDATE    DATE NOT NULL,
  SqlDec17P2D l_quantity;
  SqlDec17P2D l_extendedprice;
  SqlDec17P2D l_discount;
  SqlDec17P2D l_tax;
  char l_returnflag;
  char l_linestatus;
  SqlDate l_shipdate;

  SqlDefs::RdTracker rdTracker;
  rdTracker.curChIdx = 0;
  rdTracker.curIdx = 0;  
  
  //int no_of_rows = *((int*)inbuf); // todo see if synthesizable
  //todo see if this needs optimization
  SqlDefs::readChar(bufStream,rdTracker);
  unsigned int noOfRows = SqlDefs::readChar(bufStream,rdTracker);
  noOfRows = (noOfRows<<8) + SqlDefs::readChar(bufStream,rdTracker);
  noOfRows = (noOfRows<<8) + SqlDefs::readChar(bufStream,rdTracker);
  // procUnitBlock[0]; reserved for other blocks
  
  if (block_num >= noOfProcUnitBlocks)
      noOfRows = 0;

  //for now i am processing byte by byte, if HLS not able to handle, may need
  //   different architecture for parallelization
  // in this arch no pipelining is feasible, use multiple of these
  
 LOOP_PU_ROWS: for (unsigned int i =0; i<SqlQ1Defs::ProcUnitBlockMaxRows; i++) {
    //#pragma HLS PIPELINE OFF
    if (i == noOfRows) break;
    l_quantity.SQLEVAL(decToBin,SQLKERNEL_ID)(bufStream,rdTracker);
    l_extendedprice.SQLEVAL(decToBin,SQLKERNEL_ID)(bufStream,rdTracker);
    l_discount.SQLEVAL(decToBin,SQLKERNEL_ID)(bufStream,rdTracker);
    l_tax.SQLEVAL(decToBin,SQLKERNEL_ID)(bufStream,rdTracker);    
    l_returnflag = SqlDefs::readChar(bufStream,rdTracker);
#ifdef HLS_DEBUG
    if (l_returnflag != 'N' && l_returnflag != 'R' && l_returnflag != 'A')
      printf("Error: returnflag %c mut be N, R, A \n",l_returnflag);
#endif    
    char ch = SqlDefs::readChar(bufStream,rdTracker);
    l_linestatus = SqlDefs::readChar(bufStream,rdTracker);
#ifdef HLS_DEBUG
    if (l_linestatus != 'O' && l_linestatus != 'F')
      printf("Error: linestatus %c mut be N, R, A \n",l_linestatus);
#endif 
    ch = SqlDefs::readChar(bufStream,rdTracker);
    l_shipdate.SQLEVAL(toBin,SQLKERNEL_ID)(bufStream,rdTracker);
    ch = SqlDefs::readChar(bufStream,rdTracker);
    SqlDefs::dbgChkSame(ch,'\n');
    //printf("%c%c;",l_returnflag,l_linestatus);

    // l_shipdate <= date '1998-12-01' - interval '108' day
    // same as l_shipdate <= '1998-08-15'
    // todo write lessthan in sqldate
    if ( (l_shipdate.year <= SqlDate::Year(1997)) ||
	 (l_shipdate.year == SqlDate::Year(1998) && l_shipdate.month <= SqlDate::Month(7) ) ||
	 (l_shipdate.year == SqlDate::Year(1998) && l_shipdate.month == SqlDate::Month(8) && l_shipdate.day <= SqlDate::Day(15)) ) {
	// Use predetermined groups here
	// l_returnflag | l_linestatus 
	// --------------+--------------
	// N            | O
	// R            | F
	// A            | F
	// N            | F
	if (l_returnflag == 'N' && l_linestatus == 'O') 
	  SQLEVAL(updateOrder,SQLKERNEL_ID)(res.fNsO, l_quantity, l_extendedprice, l_discount, l_tax);
	else if (l_returnflag == 'R' && l_linestatus == 'F') 
	  SQLEVAL(updateOrder,SQLKERNEL_ID)(res.fRsF, l_quantity, l_extendedprice, l_discount, l_tax);
	else if (l_returnflag == 'A' && l_linestatus == 'F') 
	  SQLEVAL(updateOrder,SQLKERNEL_ID)(res.fAsF, l_quantity, l_extendedprice, l_discount, l_tax);
	else if (l_returnflag == 'N' && l_linestatus == 'F') 
	  SQLEVAL(updateOrder,SQLKERNEL_ID)(res.fNsF, l_quantity, l_extendedprice, l_discount, l_tax);
#ifdef HLS_DEBUG
	else
	  printf("Error: returnflag/linestatus pair %c %c not handled\n",l_returnflag, l_linestatus);
#endif    
    }
  }
  
  //Flush rest of cluster
  LOOP_INFLSH: for (unsigned int ml=rdTracker.curIdx; ml<(SqlQ1Defs::ProcUnitBlockSize/SqlDefs::RdDdrWordSize); ml++) {
#pragma HLS LOOP_TRIPCOUNT min=128 max=128 avg=128
#pragma HLS PIPELINE
    bufStream.read();
  }
  
}
