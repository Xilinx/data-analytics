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

#ifndef XPG_VARATTRDETAILS_H
#define XPG_VARATTRDETAILS_H

#include "xpgbolt.h"

extern "C" 
{
#include "catalog/pg_type.h"
#include "utils/numeric.h"
}

#include <vector>
#include <algorithm>    // std::sort

//Expand the below structure as required to include details required to populate varstream

typedef struct XpgVarAttrDetails 
{
  XpgVarAttrDetails(int col, Oid relId, Oid type) : colId(col), relationId(relId), colType(type) { }
  XpgVarAttrDetails() : colId(-1), relationId(-1), colType(-1) { }

  int colId; 
  Oid relationId;
  Oid colType;
  bool operator<(const XpgVarAttrDetails& rhs) const { return colId < rhs.colId; }
} XpgVarAttrDetails;

/*inline bool operator<(const varAtrrDetails& lhs, const foo& varAtrrDetails)
{
  return lhs.key < rhs.key;
}*/

// Keep the vector unique based on colId and sorted based on colIds

extern std::vector<XpgVarAttrDetails> varAttrDetailsVec;

//std::sort(varAtrrDetailsVec.begin(), varAtrrDetailsVec.end());

extern int 
xpg_get_num_attr();

extern void 
xpg_alloc_var_attr(int size);

extern XpgVarAttrDetails 
xpg_get_nth_attr(int n);


#endif
