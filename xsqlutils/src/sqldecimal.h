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
#ifndef XSQLUTILS_SQLDECIMAL_H
#define XSQLUTILS_SQLDECIMAL_H

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
  void SQLEVAL(decToBin,SQLKERNEL_ID)(hls::stream<SqlDefs::RdDdrWord>& bufStream, SqlDefs::RdTracker& rdTracker);
  int SQLEVAL(binStr,SQLKERNEL_ID)(char *bin, int binLoc);

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
  inline void SqlDecimal<INTWIDTH,DECWIDTH,BINWIDTH>::SQLEVAL(decToBin,SQLKERNEL_ID)(hls::stream<SqlDefs::RdDdrWord>& bufStream, SqlDefs::RdTracker& rdTracker) {
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
    fixedBin = (Decimal(10)*fixedBin)+dig;
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
inline int SqlDecimal<INTWIDTH,DECWIDTH,BINWIDTH>::SQLEVAL(binStr,SQLKERNEL_ID)(char *bin, int binLoc) {
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
