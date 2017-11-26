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
#ifndef XPGCSIM_H
#define XPGCSIM_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "date.h"
#include <string>
#include "ap_int.h"


typedef ap_int<32>   Int32;
//var parser
bool process_var_file(char **varstream, std::string varfile);
char* process_const_xpg(char *constream, bool keep_decimal);
const char* append_expr_xpg(char *setupstream, int (*expr) [10], int no_exprs);
const char* process_aggregation_xpg(char *aggstream, int query);
const char* process_qualifier_xpg(char *qualstream, int query);
int getPgAbsDay(int year, int month, int day);
char *nextInt(char *ch, int& val, int sf, char& lastch);
char *nextInt(char *ch, Int32& val, int sf, char& lastch);
char *nextTok(char *ch, int& val, int& len, char& lastch);
char *nextDec(char *ch, int& val);
char *nextDec(char *ch, Int32& val);

char *nextVarDec(char *ch, int& val);
char *nextVarDec(char *ch, Int32& val);
char *nextVarInt(char *ch, int& val);
char *nextVarInt(char *ch, Int32& val);

void processCscan(char *setupbuf, char *varstream, int ntuples, char *result, int pcielanes,
		int deviceslrs, int query, bool verifyCompute);


void processCscanNew(char *setupbuf, char * varstream, int ntuples, char *result, int pcielanes,
   int deviceslrs, int query, bool verifyCompute);


void processCscanAggregate(char *setupbuf, char *varstream, int ntuples, char *result, int pcielanes,
		int deviceslrs, int query, bool verifyCompute);


#endif
