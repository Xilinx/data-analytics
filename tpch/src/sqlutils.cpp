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

#include "sqldate.h"
#include "sqldecimal.h"
#include "sqldecimalutils.h"
#include "sqlutils.h"
#include "sqlq1procunit.h"

// SQL data format is 4 year like YYYY-MM-DD e.g. 2016-01-21
int SqlDate::toBin(char *dateStr, int curLoc) {
  year = dateStr[curLoc+3]-'0';
  year = 10*(dateStr[curLoc+2]-'0')+year;
  year = 100*(dateStr[curLoc+1]-'0')+year;
  year = 1000*(dateStr[curLoc]-'0')+year;

  month = dateStr[curLoc+6]-'0';
  month = 10*(dateStr[curLoc+5]-'0')+month;
    
  day = dateStr[curLoc+9]-'0';
  day = 10*(dateStr[curLoc+8]-'0')+day;
  curLoc = curLoc+11;
  return curLoc;
}

void SqlDate::print() {
  unsigned int yr = year;
  unsigned int mo = month;
  unsigned int dy = day;
  printf("%4d-%2d-%2d\n",yr,mo,dy);
}

void printStrLen(const char* ch, unsigned st, unsigned len) {
  for (unsigned i=st; i<len+st; i++) {
    std::cout << *ch ;
    ch++;
  }
  std::cout << std::endl;
}

void printStr(SqlDefs::RdDdrWord& rdWord) {
  for (unsigned int chIdx=0; chIdx < SqlDefs::RdDdrWordSize; chIdx++) {
    char ch = (rdWord>>(chIdx*8)) & 0x000FF;
    printf("%c",ch);
  }
  printf("\n");
}

int checkBlock(const char *bufRam, int puBlockSize, uint8_t puBlockCount, bool print) {
  uint8_t noOfPUBlocks = *((uint8_t *) bufRam);
  if (noOfPUBlocks != puBlockCount) {
    printf("Error: PU block counnt %d in block different than %d\n",noOfPUBlocks,puBlockCount);
  }
  int noOfRows = 0;
  const char *curBufRam = bufRam;
  for (int i=0; i<puBlockCount; i++) {
    noOfRows += checkPage(curBufRam, puBlockSize, false);
    curBufRam += puBlockSize;
  }
  if (print) printf("Info: Processed %d PUBlocks with %d rows\n",noOfPUBlocks, noOfRows);
  return noOfRows;
}

int checkPage(const char *bufRam, int blockSize, bool print) {
  //char noRows[4] = {0x0,bufRam[1],bufRam[2],bufRam[3]};
  unsigned int no_of_rows = (unsigned char) bufRam[3];
  no_of_rows += (unsigned char) bufRam[2] * (0x1<<8);
  no_of_rows += (unsigned char) bufRam[1] * (0x1<<16);
  bufRam = bufRam+4;
  if (print) printf("No of Rows = %d\n",no_of_rows);
  unsigned int cur_row = 0;
  int cur_buf_ptr = 0;
  while (cur_row < no_of_rows) {
    char cur_ch = bufRam[cur_buf_ptr++];
    if (cur_buf_ptr >= blockSize) {
      printf("Error: Corrupt DB page with mismatching no of rows\n");
      break;
    }
    if (print) printf("%c",cur_ch);
    if (cur_ch == '\n')
      cur_row++;
  }
  if (cur_row != no_of_rows) {
    printf("Error: Corrupt DB page mismatching no of rows %d and %d\n",no_of_rows,cur_row);    
  }
  return no_of_rows;
}

int comparePages(const char *buf0, const char *buf1, int blockSize) {
  int rows0 = checkPage(buf0,blockSize,false);
  int rows1 = checkPage(buf1,blockSize,false);
  if (rows0 != rows1) {
    printf("Error: Pages with different rows %d and %d\n",rows0,rows1);
    return rows0;
  }
  int cur_row = 0;
  int cur_buf_ptr = 0;
  while (cur_row < rows0) {
    char ch0 = buf0[cur_buf_ptr];
    char ch1 = buf1[cur_buf_ptr++];    
    if (ch0 != ch1) {
      printf("Error: Pages with mismatching entries %c and %c\n", ch0, ch1);
      return rows0;
    }
    if (ch0 == '\n') cur_row++;
  }
  return rows0;
}
/*
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
*/
void SqlQ1GrpData::print() {
  printf("l_returnflag | l_linestatus | sum_qty | sum_base_price | sum_disc_price | sum_charge | avg_qty | avg_price | avg_disc | count_order | \n");
  printf("A|F|");
  fAsF.print();
  printf("N|F|");
  fNsF.print();
  printf("N|O|");
  fNsO.print();
  printf("R|F|");
  fRsF.print();
};

void SqlQ1GrpData::Q1Data::print() {
  double d_sum_qty = sum_qty.printDec(1); printf(" | ");
  double d_sum_base_price = sum_base_price.printDec(1);printf(" | ");
  sum_disc_price.printDec(2);printf(" | ");
  sum_charge.printDec(3);printf(" | ");
  double d_count_order = count_order.printDec(0,false);
  double d_avg_qty = d_sum_qty/d_count_order;
  double d_avg_price  = d_sum_base_price/d_count_order;
  double d_sum_disc = sum_disc.printDec(1,false);
  double d_avg_disc = d_sum_disc/d_count_order;
  printf("%.2lf|%.2lf|%.3lf|",d_avg_qty,d_avg_price,d_avg_disc);
  printf("%.0lf|\n",d_count_order);
}

int SqlQ1GrpData::Q1Data::binStrToBin(char *bin, int binLoc) {
  binLoc = sum_qty.binStrToBin(bin,binLoc);
  if (bin[binLoc++] != '|')
    printf("ERROR: in SQLGrpData %c instead of new line.\n",bin[binLoc]);  
  binLoc = sum_base_price.binStrToBin(bin,binLoc);
  if (bin[binLoc++] != '|')
    printf("ERROR: in SQLGrpData %c instead of new line.\n",bin[binLoc]);  
  binLoc = sum_disc_price.binStrToBin(bin,binLoc);
  if (bin[binLoc++] != '|')
    printf("ERROR: in SQLGrpData %c instead of new line.\n",bin[binLoc]);  
  binLoc = sum_charge.binStrToBin(bin,binLoc);
  if (bin[binLoc++] != '|')
    printf("ERROR: in SQLGrpData %c instead of new line.\n",bin[binLoc]);  
  binLoc = sum_disc.binStrToBin(bin,binLoc);
  if (bin[binLoc++] != '|')
    printf("ERROR: in SQLGrpData %c instead of new line.\n",bin[binLoc]);  
  binLoc = count_order.binStrToBin(bin,binLoc);
  if (bin[binLoc++] != '|')
    printf("ERROR: in SQLGrpData %c instead of new line.\n",bin[binLoc]);  
  if (bin[binLoc++] != '\n')
    printf("ERROR: in SQLGrpData %c instead of new line.\n",bin[binLoc]);
  return binLoc;
}

int SqlQ1GrpData::binStrToBin(char *bin, int binLoc) {
  binLoc = fNsO.binStrToBin(bin,binLoc);
  binLoc = fRsF.binStrToBin(bin,binLoc);
  binLoc = fAsF.binStrToBin(bin,binLoc);
  binLoc = fNsF.binStrToBin(bin,binLoc);    
  return binLoc;
}
