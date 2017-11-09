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
#ifndef XSQLUTILS_SQLDECIMALUTILS_H
#define XSQLUTILS_SQLDECIMALUTILS_H

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
  if (dec[curLoc] == '.') 
    curLoc++;

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
