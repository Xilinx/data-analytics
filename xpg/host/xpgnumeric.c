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
extern "C"
{
#include "postgres.h"
#include "fmgr.h"
#include "utils/numeric.h"
}

#include "xpgnumeric.h"

#include "xpgbolt.h"
#include "xpgdatastore.h"

#ifndef SQLENGINE_DECIMAL_H
#include "decimal.h"
#endif
static int limit = 0;

bool
xpg_store_numeric_to_varstream(Datum datum,
                               XpgVarStream * varstream)
{
  limit++;
  //elog(INFO, "storing numeric to var");
  Size size = VARSIZE_SHORT((struct varlena*)DatumGetPointer(datum)) - 1;
  /* Check if container full */
  if (varstream->length + size > xpg_get_varstream_size()) {
    return false;
  }

  unsigned short * stream = (unsigned short*)(varstream->var + varstream->length);
  *stream = size - 2;

  varstream->length += 2;
  
  memcpy(varstream->var + varstream->length,
         VARDATA_SHORT((struct varlena*)datum), size);
  short * header = (short*) (varstream->var + varstream->length);
  varstream->length += size;

  return true;
}

void
xpg_print_numeric(Datum datum)
{
  Size size = VARSIZE_SHORT((struct varlena*)DatumGetPointer(datum)) - 1;

  short * numeric = (short*)VARDATA_SHORT((struct varlena*)datum);
  int i = 0;
  for (i = 0; i < size / 2; i++) {
    elog(INFO, "numeric: %d", (unsigned short)numeric[i]);
  }

}
bool
xpg_store_numeric_to_asciistream(Datum datum,
                              XpgVarStream * varstream,
                              std::string &dataStr)
{
   Numeric num = DatumGetNumeric(datum);
   std::string stnum = numeric_to_string(num);
   size_t ss_len = stnum.length();
   varstream->length += ss_len;

   dataStr += stnum + ";";

   if (limit < 25) {
      limit++;
      //printf("%s", dataStr.c_str());
   }
   return true;
}

