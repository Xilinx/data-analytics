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

  

