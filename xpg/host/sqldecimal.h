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
#ifndef SQL_SQLDECIMAL_H
#define SQL_SQLDECIMAL_H

#include <ap_fixed.h>
#include "sqldefs.h"

// Decimal with 17 Bits of precision, 2 bits of scaling
//
// decimal digits      Bytes neede
//    7-9                 4
//    5-6                 3
//    3-4                 2
//    1-2                 1
//
// 15 decimal values of integer need 7 bytes
// 2 decimal digits of integer need 1 byte
// use ap_int<68> decimal value intrinsic, all numbers scaled by 100
// Useing ap_fixed<64,8,AP_RND,AP_SAT> 64 total bits, 8 bits after decimal causes decimal
//  value round offs, and results into error

// Using template metaprogramming to create a generic decimal
// Sql Decimal Precision is (INTWIDTH+DECWIDTH);Decimal is DECWIDTH;
// BINWIDTH is the minimum binary with to hold this valu

template<unsigned int INTWIDTH, unsigned int DECWIDTH, unsigned int BINWIDTH> struct SqlDecimal {

  typedef ap_int<BINWIDTH> Decimal; 

  // convert decimal to binary in the fixed_bin format shown below
  // const char* decimal can be up to (INT+DEC) width that is the maximum value, beyond which it will overflow
  void decToBin(hls::stream<SqlDefs::RdDdrWord>& bufStream, SqlDefs::RdTracker& rdTracker);
  int binStr(char *bin, int binLoc);

  // must have DDD.DD| format
  int binStrToBin(const char *bin, int curLoc);
  int decToBin(const char *dec, int curLoc);
  int binToHex(char *dec, int decLoc);
  // Debug functions
  void printHex();
  /*{
      char hexstr[20];
      int cur_loc = binToHex(hexstr,0);
      hexstr[cur_loc] = 0;
      printf("%s\n",hexstr);
      }*/

  void printBin();
  double printDec(int decPos, bool print=true);
  
  Decimal  fixedBin;
  //constructor for initialization only
  SqlDecimal() {
    fixedBin = 0;
  }
};

// Typedef most common use values here
// 17-digits of of precision, 2-digits of decima, 60 bits are sufficient to hold the maximum value
// Decimal with 17 Bits of precision, 2 bits of scaling
//
// decimal digits      Bytes needed
//    7-9                 4
//    5-6                 3
//    3-4                 2
//    1-2                 1
//
  
// const char* decimal can be up to 17 char long, that is the maximum value, beyond which it will overflow
// Negative decimal numbers are not handled for now , no negative numbers so far in decimal for TPCH
typedef SqlDecimal<15,2,60> SqlDec17P2D;

// 10-precision int, 32 bits of integer
typedef SqlDecimal<10,0,32> SqlInt10P;

// convert decimal to binary in the fixed_bin format shown below
template<unsigned int INTWIDTH, unsigned int DECWIDTH, unsigned int BINWIDTH>
  inline void SqlDecimal<INTWIDTH,DECWIDTH,BINWIDTH>::decToBin(hls::stream<SqlDefs::RdDdrWord>& bufStream, SqlDefs::RdTracker& rdTracker) {
  fixedBin = 0;
  char ch = SqlDefs::readChar(bufStream,rdTracker);
  bool neg = false;
  if (ch == '-') {
    neg = true;
    ch = SqlDefs::readChar(bufStream,rdTracker);
  }
 LOOP_INTW: for (unsigned int i=1; i<=INTWIDTH; i++) {
    if (ch == '|' || ch == ';')
      break;
    if (ch == '.') {
      ch = SqlDefs::readChar(bufStream,rdTracker);
      break;
    }
#ifdef HLS_DEBUG
    if (ch < '0' || ch > '9') 
      printf("Error: Incorrect value %c in sql decimal\n",ch);
#endif
    Decimal dig = Decimal(ch -'0');
    fixedBin = (Decimal(10)*fixedBin)+dig;
    ch = SqlDefs::readChar(bufStream,rdTracker);
  }

 LOOP_DECW: for (unsigned int i=1; i<=DECWIDTH; i++) {  
    Decimal dig = 0;
    if (ch != '|' && ch != ';') {
#ifdef HLS_DEBUG
    if (ch < '0' || ch > '9') 
      printf("Error: Incorrect value %c in sql decimal\n",ch);
#endif
      dig = Decimal(ch-'0');
      ch = SqlDefs::readChar(bufStream,rdTracker);
    }
    //fixedBin = (Decimal(10)*fixedBin)+dig;
  }

  if (neg)
    fixedBin = -fixedBin;
  // now must be |
#ifdef HLS_DEBUG
  if (ch != '|' && ch != ';') {
    printf("Error: Incorrect pipe %c in sql decimal\n",ch);
  }
  //int tmp = fixedBin;
  //printf("DEBUG: fixedBin = %d\n", tmp);
#endif
}

// Convert bin string to binLoc
template<unsigned int INTWIDTH, unsigned int DECWIDTH, unsigned int BINWIDTH>
inline int SqlDecimal<INTWIDTH,DECWIDTH,BINWIDTH>::binStr(char *bin, int binLoc) {
  for (int i=BINWIDTH-1; i>=0; i--) {
#pragma HLS PIPELINE
    if (fixedBin[i])
      bin[binLoc] = '1';
    else
      bin[binLoc] = '0';
    binLoc++;
  }
  bin[binLoc++] = '|';
  return binLoc;
}

#endif
