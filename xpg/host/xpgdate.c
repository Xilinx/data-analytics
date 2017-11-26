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

#include "xpgdate.h"

#include "xpgbolt.h"
#include "xpgdatastore.h"

extern "C"
{
#include "utils/date.h"
}

bool
xpg_store_date_to_varstream(Datum datum,
                            XpgVarStream * varstream)
{
  DateADT date = DatumGetDateADT(datum);
  Size size = sizeof(DateADT);

  if (varstream->length + size > xpg_get_varstream_size()) {
    return false;
  }

  memcpy(varstream->var + varstream->length, &(date), size);
  varstream->length += size;

  return true;
}

void
xpg_print_date(Datum datum)
{
  DateADT date = DatumGetDateADT(datum);
  //elog(INFO, "date: %d", (unsigned int)date);
}

static int limit1 = 0;
bool
xpg_store_date_to_asciistream(Datum datum,
                              XpgVarStream * varstream,
                              std::string &dataStr)
{
   DateADT date = DatumGetDateADT(datum);
   Size size = sizeof(DateADT);

   char buffer[10];
   sprintf(buffer, "%d;\n", date);

   size_t ss_len = strlen(buffer);
   varstream->length += ss_len;

   dataStr += buffer;


   if (limit1 < 5) {
      //printf("%s", dataStr.c_str());
      //outfile << date << "|"<<std::endl;
      limit1++;
   }
   return true;
}
