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
#ifndef XPG_CODEGEN_H
#define XPG_CODEGEN_H

#include "boost/graph/adjacency_list.hpp"
#include <set>
#include <ostream>
#include "xpgbolt.h"
extern "C" {
#include "postgres.h"

#include "nodes/primnodes.h"
}

extern "C" unsigned char* confWriteLong (unsigned char* p_buf, long data);
extern "C" unsigned char* confWriteShort (unsigned char* p_buf, short data);
extern "C" bool xpg_is_expr_supported(Expr * expr,int varno,bool andclause);
extern "C" bool xpg_is_operator_supported_new(Oid opId);
extern "C" bool xpg_is_operator_supported(Oid opId);
extern "C" void xpg_initialiseTemp();
extern "C" void checkConstraint(int count,int type); 
extern "C" int xpg_fill_constantstream(Oid typeID, Datum data);
extern "C" int  xpg_get_operator_enum(Oid opId);
extern "C" int  xpg_get_operator_enum_new(Oid opId);

extern "C" int  xpg_get_bool_enum(BoolExprType boolop);
extern "C" int  xpg_get_datatype_enum(Oid datatypeId);
extern "C" void andTempRegs();
extern "C" void build_constream(char* constream);
extern "C" void buildFilterStream(unsigned char *filterstream);
extern "C" void reduceTempReg(int op);
extern "C" bool returnConstraint();
extern "C" void addExprstring(int expr,int arg1_opType,int arg1_index,int arg1_datatype,
		  int arg2_opType,int arg2_index,int arg2_datatype,
		  int out_opType,int out_index,int out_datatype, char* qual_stream);
extern "C" void addExprToVec(int expr,int arg1_opType,int arg1_index,int arg1_datatype,
		  int arg2_opType,int arg2_index,int arg2_datatype,
			     int out_opType,int out_index,int out_datatype, int relid);
extern "C" void addTableColInfo(int colid,int length,int type,int align,int sel);
extern "C" void addExprVecToString();
extern "C" bool xpg_parse_OpExpr(OpExpr * expr,int varno);
extern "C" int getIndex(int colid);
extern "C" void mapColidToIndex();
extern "C" int addColid(int colid);
extern bool xpg_codegendebug;
extern bool build_xpgsetupstream(XpgSetupStream * setupstream,unsigned char* filterstream, char* constream, char* qual_stream, int exprcount,unsigned char scanType);
extern char *getInt(char *ch, int& val, int sf, char& lastch);
//extern char *getInt(char *ch, Int32& val, int sf, char& lastch);
extern char *getDec(char *ch, int& val);
//extern char *getDec(char *ch, Int32& val);
extern char *xpgnextTok(char *ch, int& val, int& len, char& lastch);

extern "C" char constream[4096];
extern "C" char qual_stream[4096];
extern "C" int exprcount;
extern XpgSetupStream setupstream;
extern std::set<int> xpg_set_cols_agg;

#define PG_TM_2_DATE_LOCAL(tm) (date2j(tm.year,tm.month,tm.day) - POSTGRES_EPOCH_JDATE)
#define NUM_BYTES (2*1024*1024)               // 4Mb FilterSetupMem
#endif
