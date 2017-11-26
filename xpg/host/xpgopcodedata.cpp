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
#include "xpgopcodedata.h"
#include <stddef.h>

struct Opcode* initOpcode() {
  struct Opcode* newopcode = new Opcode();
  newopcode->operandType=0;
  newopcode->index=0;
  newopcode->datatype=0;
  return newopcode;
}

struct Opcode* addOpcode(int opType,int ind,int dt) {
  struct Opcode* newopcode = new Opcode();
  newopcode->operandType=opType;
  newopcode->index=ind;
  newopcode->datatype=dt;
  return newopcode;
}

struct OpcodeSet* initOpcodeSet() {
  struct OpcodeSet* newopcodeset = new OpcodeSet();
  newopcodeset->expr = 0;
  newopcodeset->arg1=NULL;
  newopcodeset->arg2=NULL;
  newopcodeset->result=NULL;
  return newopcodeset;
}

struct TableDetail* addTableInfo(int colid,int length,int type,int align,int sel) {
  struct TableDetail* tabledet  = new TableDetail();
  tabledet->colid=colid;
  tabledet->length=length;
  tabledet->align=align;
  tabledet->sel=sel;
  tabledet->type=type;
  return tabledet;
}

struct OpcodeSet* addOpcodeSet(int expr,struct Opcode* lhs,struct Opcode* rhs,struct Opcode* output) {
  struct OpcodeSet* newopcodeset = new OpcodeSet();
  newopcodeset->expr = expr;
  newopcodeset->arg1=lhs;
  newopcodeset->arg2=rhs;
  newopcodeset->result=output;
  return newopcodeset;
}



struct OpcodeSet* addExpr(int expr,int arg1_opType,int arg1_index,int arg1_dataype,
		  int arg2_opType,int arg2_index,int arg2_datatype,
		  int out_opType,int out_index,int out_datatype) {
  struct Opcode *lhs = addOpcode(arg1_opType,arg1_index,arg1_dataype);
  struct Opcode *rhs = addOpcode(arg2_opType,arg2_index,arg2_datatype);
  struct Opcode *out = addOpcode(out_opType,out_index,out_datatype);
  return addOpcodeSet(expr,lhs,rhs,out);
}
