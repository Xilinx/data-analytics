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
#include "date.h"

void
make_date(char *str, pg_tm_t *tm)
{
  int num_field = 3;
  int *tm_field[] = {&tm->year, &tm->month, &tm->day};
  int i = 0;
  const char *dem = "-";
  char *p = NULL;
  p = strtok(str,dem); 
  while ( NULL != p && i < num_field)
    {
      *tm_field[i] = atoi(p);
      p = strtok(NULL, dem);
      i++;
    }
}

int
date2j(pg_tm_t tm)
{
  int julian;
  int century;
  int y = tm.year;
  int m = tm.month;
  int d = tm.day;

  if (m > 2)
    {
      m += 1;
      y += 4800;
    }
  else
    {
      m += 13;
      y += 4799;
    }

  century = y / 100;
  julian = y * 365 - 32167;
  julian += y / 4 - century + century / 4;
  julian += 7834 * m / 256 + d;

  return julian;
}    

