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

#ifndef XSQLUTILS_XPGDATE_H
#define XSQLUTILS_XPGDATE_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Postgres equivalent Julian dating mechanism, converts dates into a julian number and vice-versa
// #define POSTGRES_EPOCH_JDATE 2451545
// #define PG_TM_2_DATE(tm) (date2j(tm) - POSTGRES_EPOCH_JDATE)

class XpgDate {
 public:
  XpgDate() : y(0),m(0),d(0) { }
  XpgDate(int year, int month, int day) : y(year), m(month), d(day) { }
  static const int POSTGRES_EPOCH_JDATE=2451545;
  int y;
  int m;
  int d;

  void makeDate(char *str);
  int toJulian();
  int toEpochDate() {
    return (toJulian() - POSTGRES_EPOCH_JDATE);
  }
  static int getPgAbsDay(int year, int month, int day) {
    XpgDate tm(year,month,day);
    return (tm.toEpochDate());
  }
  
};



#endif
