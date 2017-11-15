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
#ifndef XSQL61_SQLDECIMALUTILS_H
#define XSQL61_SQLDECIMALUTILS_H

#include "sqldecimal.h"

template<unsigned int INTWIDTH, unsigned int DECWIDTH, unsigned int BINWIDTH>
inline void SqlDecimal<INTWIDTH,DECWIDTH,BINWIDTH>::printHex() {
  char hexstr[20];
  int cur_loc = binToHex(hexstr,0);
  hexstr[cur_loc] = 0;
  printf("%s\n",hexstr);
}
  
template<unsigned int INTWIDTH, unsigned int DECWIDTH, unsigned int BINWIDTH>
inline void SqlDecimal<INTWIDTH,DECWIDTH,BINWIDTH>::printBin() {
  char binstr[70];
  int bin_loc = SQLEVAL(binStr,SQLKERNEL_ID)(binstr,0);
  binstr[bin_loc] = 0;
  printf("%s\n",binstr);
}

// Normal decPos is 1, with each multiplication it increases by 1
template<unsigned int INTWIDTH, unsigned int DECWIDTH, unsigned int BINWIDTH>
double SqlDecimal<INTWIDTH,DECWIDTH,BINWIDTH>::printDec(int decPos, bool print) {
  double decval =0;
  double pow = 1;
  for (unsigned int i=0; i<BINWIDTH; i++) {
    if (fixedBin[i])
      decval += pow;
    pow = pow*2;
  }
  for (unsigned int i=0; i<DECWIDTH*decPos;i++)
    decval = decval/10;
  if (print)
    printf("%.2lf",decval);
  return decval;
}

// convert decimal to binary in the fixed_bin format shown below
// const char* decimal can be up to 17 char long, that is the maximum value, beyond which it will
// overflow
// should be 16 bytes logn - 64 bits
// Negative decimal numbers are not handled for now , no negative numbers so far in decimal for TPCH
template<unsigned int INTWIDTH, unsigned int DECWIDTH, unsigned int BINWIDTH>
inline int SqlDecimal<INTWIDTH,DECWIDTH,BINWIDTH>::decToBin(const char *dec, int curLoc) {
  fixedBin = 0;
  for (unsigned int i=1; i<=INTWIDTH; i++) {
    if (dec[curLoc] == '.') {
      curLoc++;
      break;
    }
#ifdef HLS_DEBUG
    if (dec[curLoc] < '0' || dec[curLoc] > '9') 
      printf("Error: Incorrect value %c in sql decimal\n",dec[curLoc]);
#endif
    Decimal dig = Decimal(dec[curLoc]-'0');
    fixedBin = (Decimal(10)*fixedBin)+dig;
    curLoc++;
  }

  for (unsigned int i=1; i<=DECWIDTH; i++) {  
    Decimal dig = 0;
    if (dec[curLoc] != '|') {
#ifdef HLS_DEBUG
    if (dec[curLoc] < '0' || dec[curLoc] > '9') 
      printf("Error: Incorrect value %c in sql decimal\n",dec[curLoc]);
#endif
      dig = Decimal(dec[curLoc]-'0');
      curLoc++;
    }
    fixedBin = (Decimal(10)*fixedBin)+dig;
  }

  // now must be |
#ifdef HLS_DEBUG
  if (dec[curLoc] != '|') {
    printf("Error: Incorrect pipe %c in sql decimal\n",dec[curLoc]);
  }
  //int tmp = fixedBin;
  //printf("DEBUG: fixedBin = %d\n", tmp);
#endif
  curLoc++;
  return curLoc;
}

// converts binary string to fixedBin here
// binary number must be "111000111" without decimal, dec is intrinsic
template<unsigned int INTWIDTH, unsigned int DECWIDTH, unsigned int BINWIDTH>
inline int SqlDecimal<INTWIDTH,DECWIDTH,BINWIDTH>::binStrToBin(const char *bin, int curLoc) {
  fixedBin = 0;
  for (unsigned int i=0;i<BINWIDTH;i++) {
    if (bin[curLoc] == '|') {
      curLoc++;
      break;
    }      
    fixedBin = fixedBin << 1;
    if (bin[curLoc] == '1')
      fixedBin[0] = 1;
    curLoc++;
  }
  return curLoc;
}

template<unsigned int INTWIDTH, unsigned int DECWIDTH, unsigned int BINWIDTH>
inline int SqlDecimal<INTWIDTH,DECWIDTH,BINWIDTH>::binToHex(char *dec, int decLoc) {
  char binToDecChar[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  for (int i=BINWIDTH-1; i>=0; i=i-4) {
    int dec_val = 0;
    if (fixedBin[i])   dec_val += 8;
    if (fixedBin[i-1]) dec_val += 4;
    if (fixedBin[i-2]) dec_val += 2;
    if (fixedBin[i-3]) dec_val += 1;            
    dec[decLoc++] = binToDecChar[dec_val];
  }
  dec[decLoc++] = '|';
  return decLoc;
}

#endif
