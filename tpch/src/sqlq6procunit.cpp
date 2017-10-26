/*******************************************************************************
Vendor: Xilinx
Revision History:
Oct 11, 2016: initial release
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

select
	sum(l_extendedprice * l_discount) as revenue
from
	lineitem
where
	l_shipdate >= date '1994-01-01'
	and l_shipdate < date '1994-01-01' + interval '1' year
	and l_discount between 0.06 - 0.01 and 0.06 + 0.01
	and l_quantity < 24;

*/
#include <string.h>
#include "sqldecimal.h"
#include "sqldate.h"
#include "sqldefs.h"
#include "sqlq6defs.h"

SqlDec17P2D SQLEVAL(sqlq6procunit,SQLKERNEL_ID)(hls::stream<SqlDefs::RdDdrWord> &bufStream,
                          unsigned char block_num,
                          unsigned char noOfProcUnitBlocks)
{
#pragma HLS inline
  
  SqlDec17P2D revenue;
  revenue.fixedBin = SqlDec17P2D::Decimal(0);

  // inbuf has now following elements in the order
  // 4  L_QUANTITY    DECIMAL(15,2) NOT NULL,
  // 5  L_EXTENDEDPRICE  DECIMAL(15,2) NOT NULL,
  // 6  L_DISCOUNT    DECIMAL(15,2) NOT NULL,
  // 10  L_SHIPDATE    DATE NOT NULL,
  SqlDec17P2D l_quantity;
  SqlDec17P2D l_extendedprice;
  SqlDec17P2D l_discount;

  SqlDefs::RdTracker rdTracker;
  rdTracker.curChIdx = 0;
  rdTracker.curIdx = 0;  
  
  SqlDefs::readChar(bufStream,rdTracker);
  unsigned int noOfRows = SqlDefs::readChar(bufStream,rdTracker);
  noOfRows = (noOfRows<<8) + SqlDefs::readChar(bufStream,rdTracker);
  noOfRows = (noOfRows<<8) + SqlDefs::readChar(bufStream,rdTracker);
  // procUnitBlock[0]; reserved for other blocks
  
  if (block_num >= noOfProcUnitBlocks)
      noOfRows = 0;

  SqlDate l_shipdate;  

  //processing byte by byte
 LOOP_PU_ROWS: for (unsigned int i =0; i<SqlQ6Defs::ProcUnitBlockMaxRows; i++) {
    //#pragma HLS PIPELINE OFF
    if (i == noOfRows) break;
    l_quantity.SQLEVAL(decToBin,SQLKERNEL_ID)(bufStream,rdTracker);
    l_extendedprice.SQLEVAL(decToBin,SQLKERNEL_ID)(bufStream,rdTracker);
    l_discount.SQLEVAL(decToBin,SQLKERNEL_ID)(bufStream,rdTracker);
    l_shipdate.SQLEVAL(toBin,SQLKERNEL_ID)(bufStream,rdTracker);
    char ch = SqlDefs::readChar(bufStream,rdTracker);
#ifdef HLS_DEBUG
    if (ch != '\n') { // end of row reached, new line is row marker
      printf("Error: row end marker %c is not new line\n",ch);
    }
#endif
    if ( l_shipdate.year >= SqlDate::Year(1994)) {// l_shipdate >= date '1994-01-01'
      if (l_shipdate.year < SqlDate::Year(1995)) { // and l_shipdate < date '1994-01-01' + interval '1' year
        if (l_quantity.fixedBin < SqlDec17P2D::Decimal(2400)) {  // and l_quantity < 24;
          if (SqlDec17P2D::Decimal(5) <= l_discount.fixedBin && l_discount.fixedBin <= SqlDec17P2D::Decimal(7)) { // and l_discount between 0.06 - 0.01 and 0.06 + 0.01
            revenue.fixedBin = revenue.fixedBin + (l_extendedprice.fixedBin * l_discount.fixedBin); // take care of decimals
          }
        }
      }
    }

  }
  
  //Flush only during HLS SIM to avoid warnign clutter
  LOOP_INFLSH: for (unsigned int ml=rdTracker.curIdx; ml<(SqlQ6Defs::ProcUnitBlockSize/SqlDefs::RdDdrWordSize); ml++) {
#pragma HLS LOOP_TRIPCOUNT min=128 max=128 avg=128
#pragma HLS PIPELINE
    bufStream.read();
  }
  
  return revenue;
  
}
