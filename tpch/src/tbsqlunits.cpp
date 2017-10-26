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
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "sqldecimal.h"
#include "sqldate.h"

using namespace std;

int getDec(char c) {
  switch (c) {
  case '0' ... '9': return c-'0';
  case 'A': return 10;
  case 'B': return 11;
  case 'C': return 12;
  case 'D': return 13;
  case 'E': return 14;
  case 'F': return 15;
  default:
    printf("Error bit %c\n",c);
    return 0;
  }
}

bool checkDecimal(const char *dec) {
  SqlDec17P2D dec1;
  dec1.decToBin(dec,0);
  char binStr[70];
  dec1.binStr(binStr,0);
  SqlDec17P2D dec2;
  dec2.binStrToBin(binStr,0);
  printf("Hex values for %s = ", dec);
  dec1.printDec();
  dec2.printDec();
  dec1.printHex();
  dec2.printHex();
  dec1.printBin();
  dec2.printBin();  
  if (dec1.fixedBin != dec2.fixedBin ) {
    printf("Error: Failed for %s\n",dec);
    return false;
  }    
  return true;
}

long int getDec(const char *cstr, int base) {
  long int pow =1;
  unsigned int dec = 0;
  int len = strlen(cstr);
  for (int i=len-1; i>=0; i--) {
    dec += pow*getDec(cstr[i]);
    pow = pow*base;
  }
  printf("Base: %d Str: %s Dec: %d\n",base, cstr, dec);
  return dec;
}

int main() {

  const char *dec_str = "234567890";
  const char *hex_str = "0DFB38D2";

  long int pow = getDec(hex_str,16);
  pow = getDec(dec_str,10);
  checkDecimal("17.00|");
  checkDecimal("17.25|");    
  checkDecimal("17.37|");  
  checkDecimal("234567890.00|");
  checkDecimal("000023456789000.00|");
  checkDecimal("123456789012345.67|");
  
  // Change data here

  SqlDec17P2D dec1;
  SqlDec17P2D dec2;
  dec1.decToBin("125.12|",0);
  dec2.decToBin("1225.1|",0);
  double multRes = 125.12*1225.1;
  dec1.fixedBin = (dec1.fixedBin * dec2.fixedBin)/100;
  printf("Mult Result = %lf FixedBin Result =" , multRes);
  dec1.printDec();

  SqlDate date;
  date.toBin("1996-06-15",0); 
  date.print();

  SqlDate date2;  
  date2.toBin("2015-01-01",0);
  date2.print();

  return 0;
}

  

