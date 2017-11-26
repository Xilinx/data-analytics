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
#ifndef XPG_OPCODEDATA_H
#define XPG_OPCODEDATA_H

struct Opcode {
  int operandType;
  int index;
  int datatype;
};
//typdef struct Opcode Opcode;

struct OpcodeSet {
  int expr;
  struct Opcode *arg1;
  struct Opcode *arg2;
  struct Opcode *result;
};

struct TableDetail {
  int colid;
  int length;
  int align;
  int sel;
  int type;
};
//typdef struct OpcodeSet OpcodeSet;
extern int c_opcodeset;
//extern struct OpcodeSet* opcodesetarray=malloc(9 * sizeof(struct OpcodeSet));
extern struct OpcodeSet* initOpcodeSet();
extern struct Opcode* initOpcode();
extern struct OpcodeSet* addOpcodeSet(int expr,struct Opcode* lhs,struct Opcode* rhs,struct Opcode* output);
extern struct Opcode* addOpcode(int opType,int ind,int dt);
extern struct OpcodeSet* addExpr(int expr,int arg1_opType,int arg1_index,int arg1_dataype,
		    int arg2_opType,int arg2_index,int arg2_datatype,
		    int out_opType,int out_index,int out_datatype);
extern struct TableDetail* addTableInfo(int colid,int length,int type,int align,int sel);
#endif
