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
