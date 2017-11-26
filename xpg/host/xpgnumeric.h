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
#ifndef XPG_NUMERIC_H
#define XPG_NUMERIC_H

extern "C"
{
#include "postgres.h"
}

#include "xpgbolt.h"

bool 
xpg_store_numeric_to_varstream(Datum datum, 
                               XpgVarStream * varstream);

void
xpg_print_numeric(Datum datum);

bool
xpg_store_numeric_to_asciistream(Datum datum,
                               XpgVarStream * varstream,
                               std::string &dataStr);


#endif
