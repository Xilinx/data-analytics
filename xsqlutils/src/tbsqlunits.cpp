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
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "sqldecimal.h"
#include "sqldecimalutils.h"
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
  dec1.binStr0(binStr,0);
  SqlDec17P2D dec2;
  dec2.binStrToBin(binStr,0);
  printf("Hex values for %s = ", dec);
  dec1.printDec(1);
  printf(" = ");  
  dec2.printDec(1);
  printf("\n");
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
  dec1.fixedBin = dec1.fixedBin * dec2.fixedBin;
  printf("Mult Result = %lf FixedBin Result =" , multRes);
  dec1.printDec(2);
  printf("\n");

  SqlDate date;
  date.toBin("1996-06-15",0);
  printf("1996-06-15= ");
  date.print();

  SqlDate date2;  
  date2.toBin("2015-01-01",0);
  printf("2015-01-01= ");
  date2.print();

  return 0;
}

  

