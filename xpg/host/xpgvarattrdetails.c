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

#include "xpgvarattrdetails.h"

std::vector<XpgVarAttrDetails> varAttrDetailsVec;

int 
xpg_get_num_attr() 
{ 
  return varAttrDetailsVec.size(); 
}

void 
xpg_alloc_var_attr(int size) 
{ 
  varAttrDetailsVec.resize(size); 
}

XpgVarAttrDetails 
xpg_get_nth_attr(int n) 
{ 
  return varAttrDetailsVec.at(n); 
}
