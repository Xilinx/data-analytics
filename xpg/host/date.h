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
#ifndef SQLENGINE_DATE_H
#define SQLENGINE_DATE_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define POSTGRES_EPOCH_JDATE 2451545
#define PG_TM_2_DATE(tm) (date2j(tm) - POSTGRES_EPOCH_JDATE)

typedef struct pg_tm_t
{
  int year;
  int month;
  int day;
}pg_tm_t;

void make_date(char *str, pg_tm_t *tm);
int date2j(pg_tm_t tm);

#endif
