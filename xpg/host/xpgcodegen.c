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
#include "xpgcodegen.h"
#include "decimal.h"
#include "boost/graph/depth_first_search.hpp"
#include <sstream>
#include "xpgopcodedata.h"
#include "xpgvarattrdetails.h"
#include "qdefs.h"
#include "sqlcmds.h"
#include "xpg_oid_enum.h"

extern "C" {
#include "postgres.h"
//#include "xpgoperators.h"
//#include "date.h" //error in dup def of pg_tm struct
#include "utils/datetime.h"
#include "miscadmin.h"
#include "utils/date.h"
#include "utils/numeric.h"
#include "nodes/nodeFuncs.h"
#include "catalog/pg_operator.h"
#include "utils/lsyscache.h"

//Constream 
//int const_array[4096];
char const_stream[4096];
char qual_stream[4096];
char constream[4096];
vector<struct OpcodeSet*> expr_s;
vector<struct TableDetail*> tableInfo;
vector<int> currColIds;
vector<long> currConsts;
vector<short> currExprs;
vector<int> currAggrs;
map<int,int> colid_to_index;
map<std::string,int> const_to_index;
int constidx; 
int qual_length;
int exprcount;
static bool constraint;
static int usedTemp = 0;
static bool temp[10];
static int leveloperator[10];
static int levelUsed=-1;
#define MAXTEMP 9
#define MAXCONST 9
#define MAXVAR 9
#define MAXEXPR 20
#define IsTableVar(node)						\
  ((node) !=NULL							\
   && ((Var *) (node))->varattno >0					\
   && ((Var *) (node))->varlevelsup == 0)				\

// big endian
unsigned char* confWriteLongStream (unsigned char* p_buf, long data)
{
  for (int i=7; i >= 0; --i) {
    unsigned char ch = (data >> i * 8) & 0xFF;
    *p_buf++ = ch;
  }
  return p_buf;
}

unsigned char* confWriteShortStream (unsigned char* p_buf, short data)
{
  for (int i=1; i >= 0; --i) {
    unsigned char ch = (data >> i * 8) & 0xFF;
    *p_buf++ = ch;
  }
  return p_buf;
}

/**
 *  Description: Checks if the expression provided is supported by XPGBolt
 *               It is able to check items like opcodes, variables, etc.
 *  Called by: xpg_add_rel_scan_path
 */

char* converttimestamp_to_string(Datum data) {
  //elog(INFO,"convertdatetime_to_string");
  Timestamp   timestamp;
  int idx = 0;
  struct pg_tm tm;
  fsec_t      fsec;
  char        buf[MAXDATELEN + 1];
  char*       result;
  timestamp = DatumGetTimestamp(data);
  /* Same as timestamp_out(), but forcing DateStyle */
  if (TIMESTAMP_NOT_FINITE(timestamp))
    EncodeSpecialTimestamp(timestamp, buf);
  else if (timestamp2tm(timestamp, NULL, &tm, &fsec, NULL, NULL) == 0)
    EncodeDateTime(&tm, fsec, false, 0, NULL, USE_XSD_DATES, buf);
  else
    ereport(ERROR,
	    (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
	     errmsg("timestamp out of range")));
  //elog(INFO,"arg2-consttype %s ",buf);
  result = buf;
  while (*result++ != 'T') idx++;
  //elog(INFO,"INDEX: %d",idx);
  char        buf_out[idx];
  //char        buf_out[10];
  strncpy(buf_out,buf,idx);
  buf_out[idx] = '\0';
  //elog(INFO,"arg2-consttype:%s",buf_out);
  result = buf_out;
  return result;
}
  bool returnConstraint(){
    return constraint;
  }

void xpg_initialiseTemp() {
  //TODO - clear pointers
  expr_s.clear();
  currColIds.clear();
  currConsts.clear();
  currExprs.clear();
  currAggrs.clear();
  colid_to_index.clear();
  const_to_index.clear();
  varAttrDetailsVec.clear();
  tableInfo.clear();
  constidx=0; 
  qual_length=0;
  constraint =  false;
  memset(qual_stream, 0 ,4096);
  memset(const_stream, 0 ,4096);
  exprcount=0;
  usedTemp=0;
  levelUsed=-1;
  int i;
  for(i=0;i<10;i++)
    temp[i]=false;
}

void andTempRegs() {
  while(usedTemp>1) {
    reduceTempReg(false);
  }
}

  void reduceTempReg(int op){
  if(temp[usedTemp-1] && temp[usedTemp-2]) {
  if(xpg_codegendebug) {
    if(!op) 
      elog(INFO,"TEMP%d = TEMP%d && TEMP%d",usedTemp-2,usedTemp-2,usedTemp-1);
    else
      elog(INFO,"TEMP%d = TEMP%d || TEMP%d",usedTemp-2,usedTemp-2,usedTemp-1);
      }
    //    addExprstring(op,2,usedTemp-2,0,2,usedTemp-1,0,2,usedTemp-2,0,qual_stream);
    addExprToVec(op,TEMP,usedTemp-2,BOOL,TEMP,usedTemp-1,BOOL,TEMP,usedTemp-2,BOOL,0);
    temp[usedTemp-1]=false;
    usedTemp--;
    levelUsed--;
  }
}

bool
xpg_is_expr_supported(Expr * expr,int varno,bool orclause)
{
  constraint = false;
  if(IsA(expr, OpExpr)) {
    xpg_parse_OpExpr((OpExpr *)expr,varno);
  if(xpg_codegendebug) 
    elog(INFO,"current usedTemp %d",usedTemp);;
    /*    for(int i=1;i<10;i++){
      if(temp[i-1])
	elog(INFO,"TRUE %d",i-1);
      else
	elog(INFO,"FALSE %d",i-1);
      if(temp[i])
	elog(INFO,"TRUE %d",i);
      else
	elog(INFO,"FALSE %d",i);


    }
    */
    levelUsed++;
    leveloperator[levelUsed]=0;

    orclause = false;
  }
  else if(IsA(expr, BoolExpr)){
    BoolExpr   *b = (BoolExpr *) expr;

    bool orclause=false;

    if (b->boolop == AND_EXPR) {
      orclause=true;
      if(xpg_codegendebug) 
        elog(INFO, "BOOL EXpr: %d AND", (int) b->boolop);
    }
    else
      if(xpg_codegendebug) 
      	elog(INFO, "BOOL EXpr: %d OR", (int) b->boolop);
    levelUsed++;
    leveloperator[levelUsed]=(int) b->boolop;
    ListCell	   *cell;
    foreach (cell, b->args) {
      if(IsA(lfirst(cell), BoolExpr))
	xpg_is_expr_supported((Expr *)lfirst(cell),varno,orclause);
      else
	xpg_parse_OpExpr((OpExpr *)lfirst(cell), varno);
    }
  }
  /*for(int i=0;i<levelUsed;i++) {
    elog(INFO,"levl %d",leveloperator[levelUsed]);
    }*/
  //  elog(INFO,"Calling reduce %d",levelUsed);
  checkConstraint(usedTemp,TEMP);
  reduceTempReg(leveloperator[levelUsed]);
  //  elog(INFO,"Used temp %d",usedTemp);
  return true;
}

bool
xpg_parse_OpExpr(OpExpr * expr,int varno)
{
  if(IsA(expr, OpExpr)) {
    if(xpg_codegendebug) 
    elog(INFO,"======BEGIN-EXPR======");
    OpExpr   *op = (OpExpr *) expr;
    if(xpg_codegendebug) {
      elog(INFO,"OpNo %d",op->opno);
      elog(INFO,"OpFuncId %d",op->opfuncid);
      elog(INFO,"OpResultType %d",op->opresulttype);
      elog(INFO,"OpCollid %d",op->opcollid);
      elog(INFO,"OpInputCollid %d",op->inputcollid);
      elog(INFO,"OpLocation %d",op->location);
      }
    Node   *arg1;
    Node   *arg2;
    Node   *other;
    Node *tablevar;
    int arg1_const_index=-1;
    int arg2_const_index=-1;
    //xpg_is_operator_supported(op->opno);
    // constraint variable is used in other places.
    // so make sure that this is set right
    constraint = true;
    if (xpg_is_operator_supported_new(op->opno))
       constraint = false;
    if(constraint)
      return false;
    if (list_length(op->args) != 2) {
      elog(INFO,"INValid arg");
      return false;	/* should not happen */
    }
    arg1 = (Node *)linitial(op->args);
    arg2 = (Node *)lsecond(op->args);
    if (IsTableVar(arg1)){
      //      if(IsTableVar(arg2))
      //	return false; // It is because of join
      other = arg2;
      tablevar = arg1;
      if(xpg_codegendebug) {
      elog(INFO,"table-Column id %d ",((Var *) (tablevar))->varattno);
      elog(INFO,"table-vartype %d ",((Var *) (tablevar))->vartype);
      }
      if(!IsTableVar(arg2)){
      if(xpg_codegendebug) {
	elog(INFO,"arg2-consttype %d ",((Const *) (other))->consttype);
      //int consttype= ((Const *) (other))->consttype;
	elog(INFO,"arg2-consttypemod %d ",((Const *) (other))->constlen);
	}
	Datum data = ((Const *) (other))->constvalue;
	arg1_const_index = xpg_fill_constantstream(((Const *) (other))->consttype,data);
	if(constraint){
         if(xpg_codegendebug)
	  elog(INFO,"Return constraints \n");
	  return false;
	}
      }
    }
    if (IsTableVar(arg2)) {
      //      if(IsTableVar(arg1))
      //	return false; // It is because of join
      other = arg1;
      tablevar =arg2;
      if(xpg_codegendebug) {
        elog(INFO,"table-Column id %d ",((Var *) (tablevar))->varattno);
        elog(INFO,"table-vartype %d ",((Var *) (tablevar))->vartype);
      }
      if(!IsTableVar(arg2)){
        if(xpg_codegendebug) {
	  elog(INFO,"arg2-consttype %d ",((Const *) (other))->consttype);
	  //int consttype= ((Const *) (other))->consttype;
	  elog(INFO,"arg2-consttypemod %d ",((Const *) (other))->constlen);
	  }
	Datum data = ((Const *) (other))->constvalue;
	arg2_const_index = xpg_fill_constantstream(((Const *) (other))->consttype,data);
	if(constraint) {
         if(xpg_codegendebug)
	  elog(INFO,"Return constrains \n");
	  return false;
	}
      }
    }
    /*    else {
      elog(INFO,"INValid args");
      return false;
    }
    */
    //    if(usedTemp>9)
    //      elog(ERROR,"Overflow of Temp Registers");
    if(xpg_codegendebug)
      elog(INFO,"TEMP %d is used",usedTemp);
    int constindex = -1;
    if(arg2_const_index>-1)
      constindex=arg2_const_index;
    else if(arg1_const_index>-1)
      constindex=arg1_const_index;
   
    if(arg1_const_index==-1 && arg2_const_index==-1) 
      addExprToVec(op->opno,VAR,((Var *) (arg1))->varattno,((Var *) (arg1))->vartype,VAR,((Var *) (arg2))->varattno,((Var *) (arg2))->vartype,TEMP,usedTemp,BOOL,varno);
    else
      //    addExprstring(op->opno,1,((Var *) (tablevar))->varattno,((Var *) (tablevar))->vartype,0,constindex,((Const *) (other))->consttype,2,usedTemp,0,qual_stream);
      addExprToVec(op->opno,VAR,((Var *) (tablevar))->varattno,((Var *) (tablevar))->vartype,CONST,constindex,((Const *) (other))->consttype,TEMP,usedTemp,BOOL,varno);
    temp[usedTemp]=true;
    usedTemp = usedTemp+1;
    //    elog(INFO,"TEMP %d is used",usedTemp);
    /*if(!temp0_used) {
      elog(INFO,"TEMP0 is used");
      temp0_used=true;
    }
    else if(!temp1_used){
      elog(INFO,"TEMP1 is used");
      temp1_used=true;
    }
    else {
      elog(INFO,"Wrong Assignment");
    }
    */
    if(xpg_codegendebug) 
      elog(INFO,"======END-EXPR======");
  }
  else
    if(xpg_codegendebug) 
      elog(INFO,"Invalid Type -OpExpr");
  return true;
}

/**
 *  Description: Checks if the operator is supported by XPG opcodes
 *  Called by: xpg_is_expr_supported
 */


bool
xpg_is_operator_supported(Oid opId)
{
  switch (opId)
    {
      //Less < Operator
    case Int4LessOperator:
    case TIDLessOperator:
    case Int8LessOperator:
    case 1095:
    case 1754:
    case 2345:
      if(xpg_codegendebug) 
        elog(INFO, "Less Operator");// %ld,obj->val.ival);
      return true;
    
      //Greater > Operator
    case 1756:
    case 2349:
    case 1097:
      if(xpg_codegendebug) 
        elog(INFO, "Greater Operator");// %ld,obj->val.ival);
      return true;

      //Greater <= Operator
    case 1096:
    case 1755:
    case 2346:
      if(xpg_codegendebug) 
        elog(INFO, "Less than equalto Operator");// %ld,obj->val.ival);
      return true;

      //Greater >= Operator
    case 1098:
    case 1757:
    case 2348:
      if(xpg_codegendebug) 
        elog(INFO, "Greater than equalto Operator");// %ld,obj->val.ival);
      return true;

    case 1054:
      if(xpg_codegendebug) 
        elog(INFO, "Equalto Operator");// %ld,obj->val.ival);
      return true;
    case 1209:
      elog(WARNING, "Unsupported operator \"like\"(expression match)");
      constraint = true;
      return false;
    default:
      elog(WARNING, "unrecognized operator Type ID: %d",
	   opId);
      constraint = true;
      return false;
  }
}

  void checkConstraint(int count,int type) {
    switch (type) {
    case CONST: 
      {
	if(count>=MAXCONST) {
	  constraint = true;
	  elog(WARNING,"Exceeds Constants register capacity(%d) of %d\n",MAXCONST,count);	  
	}
	break;  
      }
    case VAR:
      {
	if(count>=MAXVAR) {
	  constraint = true;
	  elog(WARNING,"Exceeds Var register capacity(%d) of %d\n",MAXVAR,count);	  
	}
	break;  
      }
    case TEMP:
      {
	if(count>=MAXTEMP) {
	  constraint = true;
	  elog(WARNING,"Exceeds Temp register capacity(%d) of %d\n",MAXTEMP,count);	  
	}
	break;  
      }
    case EXPR:
      {
	if(count>=MAXEXPR) {
	  constraint = true;
	  elog(WARNING,"Exceeds EXPR register capacity(%d) of %d\n",MAXEXPR,count);	  
	}
	else if(count==0){
	  constraint = true;
	  elog(WARNING,"Zero EXPRs to offload\n");	  
	}
	break;  
      }

    }
  }
  int xpg_fill_constantstream(Oid typeID, Datum data) 
{
   char buffer[100];
   //static int constidx = 0;
   Numeric num;
   switch (typeID)
    {
    case 1082://Date //DATEADT 
     {
      //elog(INFO, " Date(DATEADT): %d",DatumGetDateADT(data));
      //sprintf(buffer, "%d|",DatumGetDateADT(data));
      //elog(INFO,"buffer: %s",buffer);
      //strcat(const_stream,buffer);
      //elog(INFO,"idx: %d",constidx);
      //const_array[constidx] = DatumGetDateADT(data);
      long currConstData = DatumGetDateADT(data);
      sprintf(buffer,"%d|",currConstData);
      if(const_to_index.find(buffer)!=const_to_index.end()) {
        //elog(INFO, " const_to_indx.find(buffer) %d %d %d %s",const_to_index.find(buffer),const_to_index.end(), const_to_index.count(buffer), buffer);
        checkConstraint(constidx-1,CONST);
    	return const_to_index[buffer];
      } else {
      	const_to_index[buffer] = constidx;
        strcat(const_stream,buffer);
        currConsts.push_back(currConstData);  
      	constidx++;
      	checkConstraint(constidx-1,CONST);
      	return constidx-1;    
      }
      }
    case 1114://TimeStamp //DATEADT
      {
      //strcpy(buffer,converttimestamp_to_string(data));
      sprintf(buffer, "%s",converttimestamp_to_string(data));
      //elog(INFO,"TimeStamp buffer: %s",buffer);
      pg_tm_t tm;
      char *p;
      char year[5];
      char month[3];
      char day[3];
      p = strrchr(buffer,'-');
      strncpy(year,buffer,4);
      year[4]='\0';
      strncpy(month,p-2,2);
      strncpy(day,p+1,2);
      month[2]='\0';
      day[2]='\0';
      //elog(INFO,"year: %d",atoi(year));
      //elog(INFO,"month: %d",atoi(month));
      //elog(INFO,"day: %d",atoi(day));
      tm.year = atoi(year);
      tm.month = atoi(month);
      tm.day = atoi(day);
      //elog(INFO, "converted date: %d", PG_TM_2_DATE(tm));
      long currConstData = PG_TM_2_DATE_LOCAL(tm);
      sprintf(buffer, "%d|",PG_TM_2_DATE_LOCAL(tm));
      //elog(INFO,"TimeStamp buffer: %s",buffer);
      //const_array[constidx] = PG_TM_2_DATE(tm);
      //elog(INFO,"idx: %d",constidx);
      //elog(INFO,"const: %d at idx: %d",const_array[constidx],constidx);
      if(const_to_index.find(buffer)!=const_to_index.end()) {
        //elog(INFO, " const_to_indx.find(buffer) %d %d %d %s",const_to_index.find(buffer),const_to_index.end(), const_to_index.count(buffer), buffer);
        checkConstraint(constidx-1,CONST);
    	return const_to_index[buffer];
      } else {
        strcat(const_stream,buffer);
        currConsts.push_back(currConstData);  
      	const_to_index[buffer] = constidx;
      	constidx++;
      	checkConstraint(constidx-1,CONST);
      	return constidx-1;    
      }
      }
    case 1700: //Numeric
     {
      if(xpg_codegendebug)
        elog(INFO, "Numeric(DECIMAL) datatype");
      num = DatumGetNumeric(data);
      //uint16 n_header = num->choice.n_short.n_header;
      NumericDigit *digit = num->choice.n_short.n_data;
      int ndigit = NUMERIC_NDIGITS(num);
      if(xpg_codegendebug) 
        elog(INFO, "ndigit: %d", ndigit);
      int i =0;

     if(xpg_codegendebug) {
      for(;i < ndigit; i++) {
        elog(INFO, "Digit %d",digit[i]);
      }
      }
      std::string con_str = numeric_to_string(num);
      con_str.push_back(';');
      //elog(INFO, "Constr: %s", con_str.c_str());
      char *test;
      int num_const;
      test = getDec((char*)con_str.c_str(), num_const);
      if(xpg_codegendebug) 
        elog(INFO, "getDec output: %d", num_const);
      //const_array[constidx] = num_const;
      int postdecdig = (num->choice.n_short.n_header >> 7) & 0x003F;
      if(xpg_codegendebug) 
        elog(INFO, "postdecdig %d", postdecdig);
      long currConstData;
      if(xpg_fpgaoffload) {
      	if(postdecdig > 0) {
          sprintf(buffer, "%.4f|",num_const/(float)100);
          currConstData = num_const/(float)100;
        }
      	else {
          sprintf(buffer, "%d|",num_const/100);
          currConstData = num_const/100;
        }
      } else {
          sprintf(buffer, "%d|",num_const);
	/*if(postdecdig > 0) 
          sprintf(buffer, "%.4f|",num_const/(float)10000);
	else
          sprintf(buffer, "%d|",num_const/10000); */
      }
      if(const_to_index.find(buffer)!=const_to_index.end()) {
        //elog(INFO, " const_to_indx.find(buffer) %d %d %d %s",const_to_index.find(buffer),const_to_index.end(), const_to_index.count(buffer), buffer);
        checkConstraint(constidx-1,CONST);
    	return const_to_index[buffer];
      } else {
        strcat(const_stream,buffer);
        currConsts.push_back(currConstData);  
      	const_to_index[buffer] = constidx;
      	constidx++;
      	checkConstraint(constidx-1,CONST);
      	return constidx-1;    
      }
      }
    case 1042:
      elog(WARNING,"Unsupported Datatype \"String\"");
      constraint = true;
      return constidx;
    default:
      elog(WARNING, "unrecognized data Type ID: %d",
	   typeID);
      constraint = true;
      return constidx;
  }
}


void buildFilterStream (unsigned char *filterstream)
{
  //Preprocessing
  bool varWidth = false;
  for(int i=0;i<tableInfo.size();++i) {
    if(colid_to_index.count(tableInfo[i]->colid)>0)
      tableInfo[i]->sel = 1;
    if(tableInfo[i]->length<0 &&!varWidth)
      varWidth = true;
    if(tableInfo[i]->length>0 && varWidth)
      tableInfo[i]->align = 1;
    if(tableInfo[i]->type == 1700)
      tableInfo[i]->type = 2;
    else if (tableInfo[i]->type == 1082)
      tableInfo[i]->type = 1;
    else 
      tableInfo[i]->type = 0;
  }
  if(xpg_codegendebug) {
  for(int i=0;i<tableInfo.size();++i) 
    elog(INFO, "colid= %d,collen = %d coltype = %d colalign = %d sel = %d \n",tableInfo[i]->colid,tableInfo[i]->length,tableInfo[i]->type,tableInfo[i]->align,tableInfo[i]->sel);
  }
  //char *p_bufptr = (char*)malloc (NUM_BYTES);
  //filterstream = (char*)malloc (NUM_BYTES);
  //Setting up the buffer
  //memset (filterstream, 0, NUM_BYTES);
  int i=0;
  filterstream[i++] = 0xFF;
  filterstream[i++] = 0xFF;
  filterstream[i++] = (unsigned char)tableInfo.size();
  for (int a=0; a <tableInfo.size(); ++a) {
	  // HACK: the following if code should be removed
	  // once we figure out how to get the aggr cols.
	  /*if (a == 5) {
		 tableInfo[a]->sel = 1;
	  }
    */
    if(xpg_set_cols_agg.find(a)!=xpg_set_cols_agg.end())
        tableInfo[a]->sel = 1;
     filterstream[i++] = (char) tableInfo[a]->length;
     filterstream[i++] = (unsigned char) tableInfo[a]->type;
     filterstream[i++] = tableInfo[a]->align;
	 filterstream[i++] = tableInfo[a]->sel;

  }
  // first 4k is for filter, second is for sqleng
  if (i >= 4*1024) {
    elog(INFO,"Overflow filterBuf > 4k\n");
  }

}


void build_constream(char* constream) {
//char constream[4096];
 int i =0;
 char buffer[30];
 //int size = sizeof(const_array)/sizeof(const_array[0]);
 //elog(INFO,"Size: %d",constidx);
 //First copy header & QueryScan header
 //TODO::Once we get to support SCAN and SCANAGGR, toggle QueryScan accordingly
 sprintf(buffer,"%d",constidx);
 strcpy(constream,buffer);
 strcat(constream,"|");
 strcat(constream,const_stream);
/* for (i=0; i < constidx; i=i+1) {
  sprintf(buffer,"%d",const_array[i]);
    //elog(INFO,"const_array: %d, constidx: %d",const_array[i],constidx);
    strcat(constream,buffer);
    strcat(constream,"|");
 } */
 strcat(constream,"\n");
 if(xpg_codegendebug) {
 elog(INFO,"const_stream: %s",const_stream);
 elog(INFO,"constream: %s",constream);
 }
// return constream;
}

int
xpg_get_operator_enum(Oid opId)
{
  switch (opId)
    {
      //Less < Operator
    case Int4LessOperator:
    case TIDLessOperator:
    case Int8LessOperator:
    case 1095:
    case 1754:
    case 2345:
      //elog(INFO, "Less Operator ID: 4");
      return LT;
    
      //Greater > Operator
    case 1756:
    case 2349:
    case 1097:
      //elog(INFO, "Greater Operator ID: 3");
      return GT;

      //Greater <= Operator
    case 1096:
    case 1755:
    case 2346:
      //elog(INFO, "Less than equalto Operator ID: 6");
      return LE;

      //Greater >= Operator
    case 1098:
    case 1757:
    case 2348:
      //elog(INFO, "Greater than equalto Operator ID: 5");
      return GE;

    case 1054:
      //elog(INFO, "Equalto Operator ID: 7");
      return EQ;

    default:
      //elog(WARNING, "unrecognized operator Type ID: %d",
	//   opId);
      return (int) opId;
  }
}

int xpg_get_bool_enum(BoolExprType boolop)
{
  switch (boolop) {
    case BoolExprType::AND_EXPR:
      return OperatorType::AND;
    case BoolExprType::OR_EXPR:
      return OperatorType::OR;
    case BoolExprType::NOT_EXPR:
      return OperatorType::NOT;
    default:
      return (int)boolop;
  }
}

int
xpg_get_datatype_enum(Oid datatypeId)
{
  switch (datatypeId)
    {
    //case 1042://BPCHAR 
    //case 1082://Date //Long - 32 bits INT
    case 1082://Date //DATEADT 
      //elog(INFO, "Date(DATEADT) datatype ID: 2");
      return DATEADT;
    
    //case 1114://TimeStamp //Int64
    case 1114://TimeStamp //DATEADT
      //elog(INFO, " TimeStamp(DATEADT) datatype ID: 2");
      return DATEADT;

    case 1700: //Numeric
      //elog(INFO, "Numeric(DECIMAL) datatype ID : 1");
      return DECIMAL;

    default:
      //elog(WARNING, "unrecognized data Type ID: %d",
	//   datatypeId);
      return (int) datatypeId;
  }
}

void addTableColInfo(int colid,int length,int type,int align,int sel) {
  tableInfo.push_back(addTableInfo(colid,length,type,align,sel));
}


void addExprstring(int expr,int arg1_opType,int arg1_index,int arg1_datatype,
		  int arg2_opType,int arg2_index,int arg2_datatype,
		  int out_opType,int out_index,int out_datatype, char* qual_stream) {
		  
  char localstream[35];
  char buffer[30];
  currExprs.push_back(xpg_get_operator_enum_new(expr));
  sprintf(buffer,"%d|",xpg_get_operator_enum_new(expr));
  strcpy(localstream,buffer);
  currExprs.push_back(arg1_opType);
  sprintf(buffer,"%d|",arg1_opType);
  strcat(localstream,buffer); 
  currExprs.push_back(arg1_index);
  sprintf(buffer,"%d|",arg1_index);
  strcat(localstream,buffer); 
  currExprs.push_back(xpg_get_datatype_enum(arg1_datatype));
  sprintf(buffer,"%d|",xpg_get_datatype_enum(arg1_datatype));
  strcat(localstream,buffer); 
  currExprs.push_back(arg2_opType);
  sprintf(buffer,"%d|",arg2_opType);
  strcat(localstream,buffer); 
  currExprs.push_back(arg2_index);
  sprintf(buffer,"%d|",arg2_index);
  strcat(localstream,buffer); 
  currExprs.push_back(xpg_get_datatype_enum(arg2_datatype));
  sprintf(buffer,"%d|",xpg_get_datatype_enum(arg2_datatype));
  strcat(localstream,buffer); 
  currExprs.push_back(out_opType);
  sprintf(buffer,"%d|",out_opType);
  strcat(localstream,buffer); 
  currExprs.push_back(out_index);
  sprintf(buffer,"%d|",out_index);
  strcat(localstream,buffer); 
  currExprs.push_back(xpg_get_datatype_enum(out_datatype));
  sprintf(buffer,"%d|",xpg_get_datatype_enum(out_datatype));
  strcat(localstream,buffer); 
  //elog(INFO,"Localstream: %s,length:%ld",localstream,strlen(localstream));
  //elog(INFO,"qual_stream length: %d",strlen(qual_stream));
  memcpy(qual_stream+qual_length,localstream,strlen(localstream));
  //qual_length = qual_length+strlen(localstream);
  //elog(INFO,"qualstreamlength: %d qual+local: %d",strlen(qual_stream),qual_length+strlen(localstream));
  //elog(INFO,"qual+local: %d",qual_length+strlen(localstream));
  //qual_length = strlen(qual_stream);
  qual_length = qual_length+strlen(localstream);
  qual_stream[qual_length] = '\0';
  //elog(INFO,"qual_stream global: %s",qual_stream);
  exprcount = exprcount+1;
  //elog(INFO,"Exprcount: %d", exprcount);
  //return localstream;
}

void addExprToVec(int expr,int arg1_opType,int arg1_index,int arg1_datatype,
		  int arg2_opType,int arg2_index,int arg2_datatype,
		  int out_opType,int out_index,int out_datatype, int relid) {
  if(arg1_opType==VAR) {
    if(colid_to_index.count(arg1_index) == 0) {
    addColid(arg1_index);
    XpgVarAttrDetails num(arg1_index,relid,arg1_datatype);
    if(xpg_codegendebug)  {
      elog(INFO,"colId: %d,relid:%d,colType:%d",num.colId,num.relationId,num.colType);
      elog(INFO,"vec size: %d",xpg_get_num_attr());
      }
    varAttrDetailsVec.push_back(num);
    }
  }
  if(arg2_opType==VAR) {
    if(colid_to_index.count(arg2_index) == 0) {
      addColid(arg2_index);
      XpgVarAttrDetails num(arg2_index,relid,arg2_datatype);
      if(xpg_codegendebug) {
        elog(INFO,"colId: %d,relid:%d,colType:%d",num.colId,num.relationId,num.colType);
        elog(INFO,"vec size: %d",xpg_get_num_attr());
	}
      varAttrDetailsVec.push_back(num);
    }
  }
  
  expr_s.push_back(addExpr(expr,arg1_opType,arg1_index,arg1_datatype,
		  arg2_opType,arg2_index,arg2_datatype,
			  out_opType,out_index,out_datatype));
  checkConstraint(varAttrDetailsVec.size(),VAR);
  checkConstraint(expr_s.size(),EXPR);
}



void addExprVecToString() {
  mapColidToIndex();
  for (int i=0;i<expr_s.size();++i) {
      char localstream[35];
      char buffer[30];
      currExprs.push_back(xpg_get_operator_enum_new(expr_s[i]->expr));
      sprintf(buffer,"%d|",xpg_get_operator_enum_new(expr_s[i]->expr));
      strcpy(localstream,buffer);
      currExprs.push_back(expr_s[i]->arg1->operandType);
      sprintf(buffer,"%d|",expr_s[i]->arg1->operandType);
      strcat(localstream,buffer); 
      if(expr_s[i]->arg1->operandType==1) {
	      sprintf(buffer,"%d|",getIndex(expr_s[i]->arg1->index));
        currExprs.push_back(getIndex(expr_s[i]->arg1->index));
      }
      else {
      currExprs.push_back(expr_s[i]->arg1->index);
	      sprintf(buffer,"%d|",expr_s[i]->arg1->index);
      }
      strcat(localstream,buffer); 
      currExprs.push_back(xpg_get_datatype_enum(expr_s[i]->arg1->datatype));
      sprintf(buffer,"%d|",xpg_get_datatype_enum(expr_s[i]->arg1->datatype));
      strcat(localstream,buffer); 
      currExprs.push_back(expr_s[i]->arg2->operandType);
      sprintf(buffer,"%d|",expr_s[i]->arg2->operandType);
      strcat(localstream,buffer); 
      if(expr_s[i]->arg2->operandType==1) {
      currExprs.push_back(getIndex(expr_s[i]->arg2->index));
	      sprintf(buffer,"%d|",getIndex(expr_s[i]->arg2->index));
      }
      else {
      currExprs.push_back(expr_s[i]->arg2->index);
	      sprintf(buffer,"%d|",expr_s[i]->arg2->index);
      }
      strcat(localstream,buffer); 
      currExprs.push_back(xpg_get_datatype_enum(expr_s[i]->arg2->datatype));
      sprintf(buffer,"%d|",xpg_get_datatype_enum(expr_s[i]->arg2->datatype));
      strcat(localstream,buffer); 
      currExprs.push_back(expr_s[i]->result->operandType);
      sprintf(buffer,"%d|",expr_s[i]->result->operandType);
      strcat(localstream,buffer); 
      currExprs.push_back(expr_s[i]->result->index);
      sprintf(buffer,"%d|",expr_s[i]->result->index);
      strcat(localstream,buffer); 
      currExprs.push_back(xpg_get_datatype_enum(expr_s[i]->result->datatype));
      sprintf(buffer,"%d|",xpg_get_datatype_enum(expr_s[i]->result->datatype));
      strcat(localstream,buffer); 
      //elog(INFO,"Localstream: %s,length:%ld",localstream,strlen(localstream));
      //elog(INFO,"qual_stream length: %d",strlen(qual_stream));
      memcpy(qual_stream+qual_length,localstream,strlen(localstream));
      //qual_length = qual_length+strlen(localstream);
      //elog(INFO,"qualstreamlength: %d qual+local: %d",strlen(qual_stream),qual_length+strlen(localstream));
      //elog(INFO,"qual+local: %d",qual_length+strlen(localstream));
      //qual_length = strlen(qual_stream);
      qual_length = qual_length+strlen(localstream);
      qual_stream[qual_length] = '\0';
      //      elog(INFO,"qual_stream global: %s",qual_stream);
      exprcount = exprcount+1;
      //elog(INFO,"Exprcount: %d", exprcount);
  }
}

int addColid(int colid) {
  colid_to_index[colid]=0;
}

void mapColidToIndex(){
  int counter = 0;
  for (auto colid : colid_to_index){
    colid_to_index[colid.first]=counter;
    counter++;
  }
}

int getIndex(int colid) {
  if(colid_to_index.find(colid)!=colid_to_index.end())
    return colid_to_index[colid];
  else {
    int index = colid_to_index.size();
    colid_to_index[colid]=index;
    return index;
  }
}


}

XpgSetupStream setupstream;
char *getInt(char *ch, int& val, int sf, char& lastch) {
  val = 0;
  bool neg = false;
  if (*ch == '-') {
    neg = true;
    ch++;
  }
  int ds = 0; // # of decimals in fractions
  bool dotv = false;
  while(*ch != '|' && *ch != ';') {
    val = 10*val + (*ch++-'0');
    if (dotv)
      ds++;
    if (*ch == '.') {
      dotv = true;
      ch++;
    }
  }
  for (int i=ds; i<sf; i++)
    val = 10*val;
  lastch = *ch;
  ch++;
  val = neg?-val:val;
  return ch;
}

char *getDec(char *ch, int& val) {
  val = 0;
  
  int header;
  char lastch;
  ch = getInt(ch,header,0,lastch);

  // first 2 bits must be short
  if (!(header & 0x8000) || (header & 0x4000))
    printf("Error: Hdr X= %x or D= %d must be of SHORT type only\n", header, header);
  bool neg = false;
  if (header & 0x2000)
    neg = true;
  if (lastch == ';') // nothing follows header
    return ch;

  // In header [15:0] Bits [6:0] define no of digits before decimal
  int predecdig = header & 0x007F;
  if (predecdig == 0x007F)
    predecdig = -1; // 0 if all 1s in weight
  predecdig = predecdig+1;// each decimal bit is DEC_DIGITS wide

  while (predecdig > 0) {
    if (lastch == ';') // no more tokens if ended with semicolon
      break ;
    int len = DEC_DIGITS;
    int tval =0;
    ch = xpgnextTok(ch,tval,len,lastch);
    val = val +tval;
    predecdig--;
    // shifted by 10**DEC_DIGITS for every bit
    for (int i=0; i<DEC_DIGITS; i++)
      val = 10*val;
  }

  int postdecdig = (header>>7) & 0x003F;
  if (postdecdig >DEC_DIGITS) {
    printf("Error: Do not support post decimal %d digits Max allowed is %d\n",postdecdig,DEC_DIGITS);
  } else if (postdecdig >0 && lastch != ';') { // read next short for decimal digits
    int len = DEC_DIGITS;
    int tval =0;
    ch = xpgnextTok(ch,tval,len,lastch);
    val = val +tval;    
  }

  if (neg)
    val = -1*val;
  return ch;
}
char *xpgnextTok(char *ch, int& val, int& len, char& lastch) {
  val = 0;
  int clen = 0;
  while(*ch != '|' && *ch != ';') {
    if (*ch < '0' || *ch > '9') {
      printf("Error: %c in decimal digit must be 0-9\n",*ch++);
      continue;
    }
    val = 10*val + (*ch++-'0');
    clen++;
  }
  lastch = *ch;
  ch++; // skip pipe
  if (clen > len)
    printf("Error: Decimal len %d more than allowed max %d",clen,len);
  else
    len = clen;
  return ch;
}

bool build_xpgsetupstream(XpgSetupStream * setupstream,unsigned char *nativeSetupStream, char* constream, char* qual_stream, int exprcount,unsigned char scanType) {
 char buffer[50];
 std::string colstream;
 if(xpg_codegendebug) 
   elog(INFO,"length of conststream - %ld\n",strlen(constream));
 Size size = sizeof(constream)+sizeof(qual_stream);
 if (size > 4096) {
    return false;
  }
 if(xpg_fpgaoffload) {  
   //elog(INFO,"strlen(filterstream) %d\n",strlen(filterstream));
   //memcpy(setupstream->setup, filterstream, 4096);
   //memcpy(nativeSetupStream,filterstream, 4096);
   setupstream->length = 4096;
 }
   //nativeSetupStream += 4096;
  *nativeSetupStream++ = 0xFFu;

 if(xpg_codegendebug) 
   elog(INFO,"vec size: %d",xpg_get_num_attr());
 std::sort(varAttrDetailsVec.begin(), varAttrDetailsVec.end());
 int i;
 colstream.append(tostring(xpg_get_num_attr()));
 colstream.push_back('|');
 unsigned char ch = (xpg_get_num_attr()) & 0xFF;
 *nativeSetupStream++ = ch;

 for(i =0; i < xpg_get_num_attr(); i++) {
   if(xpg_codegendebug) 
     elog(INFO,"Vector[%d]: colid=%d,relid=%d,colType=%d\n",i,varAttrDetailsVec[i].colId,varAttrDetailsVec[i].relationId,varAttrDetailsVec[i].colType);
   colstream.append(tostring(xpg_get_datatype_enum(varAttrDetailsVec[i].colType)));
   ch = (xpg_get_datatype_enum(varAttrDetailsVec[i].colType)) & 0xFF;
   *nativeSetupStream++ = ch; 
   colstream.push_back('|');  
 }
   colstream.push_back('\n');

 //Set rest of the 4K to 0
 memset(setupstream->setup + 4096, 0, 4096);
 (setupstream->setup)[4096] = QDefs::PUSetupBlock;
 (setupstream->setup)[4097] = QDefs::QueryScan;
 setupstream->length += 2;

 memcpy(setupstream->setup+setupstream->length, constream, strlen(constream));
 setupstream->length += strlen(constream);
  if(xpg_codegendebug)
    elog(INFO,"setupstream after Constream %s",setupstream->setup+4098);
 
 //Add consts to NativeSetupStream
 *nativeSetupStream++ = currConsts.size();
  for(int k=0; k<currConsts.size(); ++k) {
    nativeSetupStream = confWriteLongStream(nativeSetupStream,currConsts[k]);
  }
  
  //Add Qualifiers to NativeSetupStream
  *nativeSetupStream++ = exprcount;
  for(int k=0; k<currExprs.size(); ++k) {
    nativeSetupStream = confWriteShortStream(nativeSetupStream,currExprs[k]);
  }
 if (scanType == KERNEL_MODE_SCANAGGR) {
 }
 else{
  //SCAN only - so add 0 for Aggregate entries
  *nativeSetupStream++ = 0x00u;
 }

 //if(xpg_codegendebug) 
 //  elog(INFO,"setupstream after Constream %s",setupstream->setup+4096);
 sprintf(buffer,"%d|",exprcount);
 memcpy(setupstream->setup+setupstream->length, buffer, strlen(buffer));
 setupstream->length += strlen(buffer);
 memcpy(setupstream->setup+setupstream->length, qual_stream, strlen(qual_stream));
 setupstream->length += strlen(qual_stream);
 strcpy(buffer,"\n");
 memcpy(setupstream->setup+setupstream->length, buffer, strlen(buffer));
 setupstream->length += strlen(buffer);
 //if(xpg_codegendebug) { 
 //  elog(INFO,"processing qualstream...");
 //  elog(INFO,"setupstream after Qualstream %s",setupstream->setup);
 //  }
 memcpy(setupstream->setup+setupstream->length, colstream.c_str(), colstream.length());
 setupstream->length += colstream.length();
// elog(INFO,"Colstream %s",colstream.c_str());
 if(xpg_codegendebug) 
   elog(INFO,"setupstream after Colstream %s",setupstream->setup+4098);

#if 0
  printf("filterstream from the host\n");
  int    j = 0;
  for (j = 3; j<4096; j++) {
     printf("j %d 0x%02x ", j,setupstream->setup[j]);
     if (((j+2) % 4 == 0) && j)
        printf("\n");
  }
  printf("\n");
  elog(INFO, "setupstream after filterstream %s", setupstream->setup + 4096)

#endif
  return true;
}

