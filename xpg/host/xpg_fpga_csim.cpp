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
extern "C" {
#include "postgres.h"
#include "date.h"
}
#include "xpg_fpga_defs.h"
#include "stdlib.h"
#include "stdio.h"
#include "xpg_fpga_csim.h"
#ifndef SQLENGINE_DECIMAL_H
#include "decimal.h"
#endif
#include "sqlcmds.h"

static   Decimal fl_compute_new = 0; //This stores the result of the direct compute, for verification purpose
static int LOGLIMIT = 0;

static int limit = 0;


/* 'process_var' generates varstream consisting of strings correspond to each column value involved in 6.sql
 */
bool process_var_file(char **varstream, std::string varfile) {
  std::string file_content;
  std::ifstream file(varfile.c_str());
  if (!file.good()) {
    printf("Error: Cannot read varfile %s.\n",varfile.c_str());
    return false;
  }

  std::string line;
  /*Get each line of sql6.var and append it to the end of string file_content
   */
  //int no_lines = 0;
  while (getline(file, line)) {
    file_content += line;
    //no_lines++;
    file_content += '\n';
  }
  size_t stream_len = file_content.length();
  *varstream = (char *)malloc(sizeof(char) * stream_len);
  /*Copy characters in string file_content to varstream
   */
  memcpy(*varstream, file_content.c_str(), stream_len);
  return true;
}

/*'process_const' formalize const variables and stores them to constream
 */
char* process_const_xpg(char *constream, bool keep_decimal) {
  string ss;
  int no_consts = 7;
  ss.append(tostring(no_consts));
  ss.push_back('|');  
  
  int absday;
  pg_tm_t tm;
  /*convert DATE 1994-01-01 to string  */
  tm.year = 1994;
  tm.month = 1;
  tm.day = 1;
  absday = PG_TM_2_DATE(tm);
  ss.append(tostring(absday));
  ss.push_back('|');

  //printf("absday(%d-%d-%d) = %d\n", tm.year, tm.month, tm.day, absday);
  /*convert DATE 1995-01-01 to string
   */
  tm.year = 1995;
  absday = PG_TM_2_DATE(tm);
  ss.append(tostring(absday));
  ss.push_back('|');

  if (keep_decimal)
    ss.append("0.0500");    
  else
    ss.append("500");
  ss.push_back('|');

  if (keep_decimal)
    ss.append("0.0700");    
  else
    ss.append("700");
  ss.push_back('|');  

  if (keep_decimal)
    ss.append("24");    
  else
    ss.append("240000");
  ss.push_back('|');  
  
  /*convert DATE 1998-08-15 to string  */
  tm.year = 1998;
  tm.month = 8;
  tm.day = 15;
  absday = PG_TM_2_DATE(tm);
  ss.append(tostring(absday));
  ss.push_back('|');

  if (keep_decimal)
    ss.append("1");    
  else
    ss.append("10000");
  ss.push_back('|');  

  ss.push_back('\n');
  
  //printf("absday(%d-%d-%d) = %d\n", tm.year, tm.month, tm.day, absday);
  //int ss_len = ss.length();
  printf("Added to conststream %s\n",ss.c_str());
  //*constream = (char *)malloc(ss_len);
  //memcpy(constream, ss.c_str(), ss_len);
  return (char*)ss.c_str();
}

const char* append_expr_xpg(char *setupstream, int (*expr) [10], int no_exprs) { 

  string ss;
  ss.append(tostring(no_exprs));
  ss.push_back('|');  
  for ( int i = 0; i < no_exprs; i++)  {
    for ( int j = 0; j < 10; j++)  {
      ss.append(tostring(expr[i][j]));
      ss.push_back('|');
    }
  }
  ss.push_back('\n');

  //int ss_len = ss.length();
  printf("Added to Setupstream %s\n",ss.c_str());
  //memcpy(setupstream, ss.c_str(), ss_len);
  return ss.c_str();
}

/* 'process_qualifier' generates qualstream consisting of enumeration value correspond to each filter condition in 6.sql
 */
const char* process_qualifier_xpg(char *qualstream, int query) {
  /*Represent the filter condition statements in 6.sql as the values enumerated in OperatorType, OperandType, VarNo, ConNo, TempNo
   */

  /* 'qualstream' represents the stream of filter conditions used in 6.sql.
   * The format of qualstream is as the following:
   * |Operator Type|Operand1 Type| Index | Data Type |Operand1 Type| Index | Data Type |Result Type| Index |
   * |-----int-----|-----int-----|--int--|----int----|-----int-----|--int--|----int----|----int----|--int--|
   * The 'Operator Type' is enumerated in enum type 'OperatorType' declared in common.h.
   * The 'Operand Type' is enumerated in enum type 'OperandType' declared in common.h.
   * The 'Index' is enumerated in enum type 'VarNo' and 'ConNo' declared in sql6_demo.cpp. 
   */
  int expr6[9][10] = {
    /* save the result of "l_shipdate_scan >= date '1994-01-01'" to TEMP0
     */
    {GE, VAR, l_shipdate_scan, DATEADT, CONST, exp1_con, DATEADT, TEMP, TEMP0, BOOL}, 

    /* save the result of "l_shipdate_scan < date '1994-01-01' + interval '1' year" to TEMP1
     */
    {LT, VAR, l_shipdate_scan, DATEADT, CONST, exp2_con, DATEADT, TEMP, TEMP1, BOOL}, 

    /* save the result of "l_shipdate_scan >= date '1994-01-01' 
     * and l_shipdate_scan < date '1994-01-1' + interval '1' year" to TEMP0
     */
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},

    /* Decompose "l_discount between .06 - 0.01 and .06 + 0.01" as
     * a. "l_discount >= .05" and
     * b. "l_discount <= .07"
     */
    /* save the result of "l_discount <.05" to TEMP1
     */
    {GE, VAR, l_discount_scan, DECIMAL, CONST, exp3_1_con, DECIMAL, TEMP, TEMP1, BOOL},
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},
    {LE, VAR, l_discount_scan, DECIMAL, CONST, exp3_2_con, DECIMAL, TEMP, TEMP1, BOOL},
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},
    /* save the result of "l_quantity < 24" to TEMP1
     */
    {LT, VAR, l_quantity_scan, DECIMAL, CONST, exp4_con, DECIMAL, TEMP, TEMP1, BOOL},
    /* save the result of the whole qualifier to TEMP0
     */
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL}
  };

  // l_shipdate_scan <= date '1998-12-01' - interval '108' day
  int expr1[1][10] = {
    /* save the result of "l_shipdate_scan >= date '1998-08-15'" to TEMP0   */
    {LE, VAR, l_shipdate_scan, DATEADT, CONST, exp5_con, DATEADT, TEMP, TEMP0, BOOL}
  };
  
  /*
  int stride = sizeof(int) * 10;
  *qualstream = (int *)malloc(sizeof(int) * 9 * 10);
  for ( int i = 0; i < 9; i++)
    {
      //todo memcpy(*qualstream + stride * i, expr[i], stride);
      memcpy(*qualstream + 10 * i, expr[i], stride);      
      }*/
  return append_expr_xpg(qualstream, expr6, 9);
}

/* 'process_aggregation' generates aggstream consisting of enumeration value corresponding to aggregation statement in 6.sql
 */
const char* process_aggregation_xpg(char *aggstream, int query) {
  /*Represent the aggregation statement in 6.sql as the values enumerated in OperatorType, OperandType, VarNo, ConNo, TempNo
   */

  /* 'aggstream' represents the stream of filter conditions used in 6.sql.
   * The format of aggstream is as the following:
   * |Operator Type|Operand1 Type| Index | Data Type |Operand1 Type| Index | Data Type |Result Type| Index |
   * |-----int-----|-----int-----|--int--|----int----|-----int-----|--int--|----int----|----int----|--int--|
   * The 'Operator Type' is enumerated in enum type 'OperatorType' declared in common.h.
   * The 'Operand Type' is enumerated in enum type 'OperandType' declared in common.h.
   * The 'Index' is enumerated in enum type 'VarNo' and 'ConNo' declared in sql6_demo.cpp. 
   */

  /*The aggregation statement "sum(l_extendedprice * l_discount) as revenue"
   * is decomposed into two basic expressions as the following:
   * a. temp2 = l_extendedprice * l_discount;
   * b. temp3 += temp2;
   */
  int expr6[2][10] = {
    {MUL, VAR, l_extendedprice, DECIMAL, VAR, l_discount, DECIMAL, TEMP, TEMP2, DECIMAL},
    {PLUS, TEMP, TEMP2, DECIMAL, TEMP, TEMP3, DECIMAL, TEMP, TEMP3, DECIMAL}
  };

  // sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge,
  int expr1[7][10] = {
    // sum(l_extendedprice) as sum_base_price TEMP1
    {PLUS, VAR, l_extendedprice, DECIMAL, TEMP, TEMP1, DECIMAL, TEMP, TEMP1, DECIMAL},
    // sum(l_quantity) as sum_qty TEMP2
    {PLUS, VAR, l_quantity, DECIMAL, TEMP, TEMP2, DECIMAL, TEMP, TEMP2, DECIMAL},
    // count(*) as count_order
    {PLUS, CONST, exp6_con, DECIMAL, TEMP, TEMP3, DECIMAL, TEMP, TEMP3, DECIMAL},    
    // sum(l_discount) as sum_disc,
    {PLUS, VAR, l_discount, DECIMAL, TEMP, TEMP4, DECIMAL, TEMP, TEMP4, DECIMAL},
    // sum(l_extendedprice * (1 - l_discount)) as sum_disc_price TEMP5
    {SUB, CONST, exp6_con, DECIMAL, VAR, l_discount, DECIMAL, TEMP, TEMP7, DECIMAL}, 
    {MUL, VAR, l_extendedprice, DECIMAL, TEMP, TEMP7, DECIMAL, TEMP, TEMP6, DECIMAL},
    {PLUS, TEMP, TEMP6, DECIMAL, TEMP, TEMP5, DECIMAL, TEMP, TEMP5, DECIMAL}
  };
  
  /*  int stride = sizeof(int) * 10;
  *aggstream = (int *)malloc(sizeof(int) * 2 * 10);
  for (int i = 0; i< 2; i++)
    {
      memcpy(*aggstream + stride * i, expr[i], stride);
      }*/

  return append_expr_xpg(aggstream, expr6, 2);
}

// Get Julian Absolute for the given data
int getPgAbsDay(int year, int month, int day) {
  pg_tm_t tm;
  tm.year = year;
  tm.month = month;
  tm.day = day;
  int absday = PG_TM_2_DATE(tm);
  return absday;
}

// Get next int based on format like 222|
char *nextInt(char *ch, int& val, int sf, char& lastch) {
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

char *nextInt(char *ch, Int32& val, int sf, char& lastch) {
  int v;
  ch = nextInt(ch,v,sf,lastch);
  val = v;
  return ch;
}
// Get next int based on format like 222|
// this is used in ProcessCScanNew
char *nextIntScan(char *ch, int& val, int sf, char& lastch) {
   val = 0;
   bool neg = false;
   if (*ch == '-') {
      neg = true;
      ch++;
   }
   int ds = 0; // # of decimals in fractions
   bool dotv = false;
   while (*ch != '|' && *ch != ';') {
      val = 10 * val + (*ch++ - '0');
      if (dotv)
         ds++;
      if (*ch == '.') {
         dotv = true;
         ch++;
      }
   }
   for (int i = ds; i<sf; i++)
      val = 10 * val;
   lastch = *ch;
   ch++;
   val = neg ? -val : val;
   return ch;
}

char * nextVarInt(char *ch, int & val)
{
  val = 0;
  int * varint = (int*)ch;

  val = *varint;

  varint++;

  if (limit<LOGLIMIT)
    elog(INFO, "date is %d", val);

  return (char*)varint;
}

char *nextIntScan(char *ch, Int32& val, int sf, char& lastch) 
{
   int v;
   ch = nextIntScan(ch, v, sf, lastch);
   val = v;
   return ch;
}

char * nextVarInt(char *ch, Int32& val)
{
  int v;
  ch = nextVarInt(ch, v);
  val = v;

  return ch;
}

// Get next val token based on format like 222|
char *nextTok(char *ch, int& val, int& len, char& lastch) {
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
// Get next val token based on format like 222|
char *nextTokScan(char *ch, int& val, int& len, char& lastch) {
   val = 0;
   int clen = 0;
   while (*ch != '|' && *ch != ';') {
      if (*ch < '0' || *ch > '9') {
         printf("Error: %c in decimal digit must be 0-9\n", *ch++);
         continue;
      }
      val = 10 * val + (*ch++ - '0');
      clen++;
   }
   lastch = *ch;
   ch++; // skip pipe
   if (clen > len)
      printf("Error: Decimal len %d more than allowed max %d", clen, len);
   else
      len = clen;
   return ch;
}
// Traverse through the string to set val as next decimal character
// If a token ends with semicolon ; the decimal has been fully specified 
char *nextDec(char *ch, int& val) {
  val = 0;
  
  int header;
  char lastch;
  ch = nextInt(ch,header,0,lastch);

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
    ch = nextTok(ch,tval,len,lastch);
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
    ch = nextTok(ch,tval,len,lastch);
    val = val +tval;    
  }

  if (neg)
    val = -1*val;
  return ch;
}

char *nextDec(char *ch, Int32& val) {
  int v;
  ch = nextDec(ch,v);
  val = v;
  return ch;
}
// This has to be rewritten for new scan to get 16 bits shorts.
char *nextNumeric(char *ch, unsigned short&val)
{
   val = 0;

   int header;
   char lastch;
   ch = nextIntScan(ch, header, 0, lastch);
   //if (limit < 4) {
   //   //unsigned short p = (ch[1] << 8) | ch[2];
   //   printf("first 16 bits %d\n", header);
   //}

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
   predecdig = predecdig + 1;// each decimal bit is DEC_DIGITS wide

   while (predecdig > 0) {
      if (lastch == ';') // no more tokens if ended with semicolon
         break;
      int len = DEC_DIGITS;
      int tval = 0;
      ch = nextTokScan(ch, tval, len, lastch);
      val = val + tval;
      predecdig--;
      // shifted by 10**DEC_DIGITS for every bit
      for (int i = 0; i<DEC_DIGITS; i++)
         val = 10 * val;
   }

   int postdecdig = (header >> 7) & 0x003F;
   if (postdecdig >DEC_DIGITS) {
      printf("Error: Do not support post decimal %d digits Max allowed is %d\n", postdecdig, DEC_DIGITS);
   }
   else if (postdecdig >0 && lastch != ';') { // read next short for decimal digits
      int len = DEC_DIGITS;
      int tval = 0;
      ch = nextTokScan(ch, tval, len, lastch);
      val = val + tval;
   }

   if (neg)
      val = -1 * val;
   return ch;
}

char *nextNumeric(char *ch, Int32& val)
{
   unsigned short v;
   ch = nextNumeric(ch, v);
   val = (Int32)v;
   return ch;
}
char * nextVarDec(char *ch, int & val)
{
  bool print = false;
  limit++;

  if (limit < LOGLIMIT)
    print = true;

  val = 0;
  
  unsigned short * dec = (unsigned short*) ch;

  Size size = *dec;
  if (print)
    elog(INFO, "size is %d", size);

  dec++;

  unsigned short header = *dec;
  if (print)
    elog(INFO, "header is %d", header);
  dec++;

  // first two bits must be short
  //
  if (!(header & 0x8000) || (header & 0x4000))
    printf("Error: Hdr X= %x or D= %d must be of SHORT type only\n", header, header);

  bool neg = false;

  if (header & 0x2000)
    neg = true;


  int predecdig = header & 0x007F;
  int postdecdig = ((header >> 7) & 0x003F);

  if (print)
    elog(INFO, "predec: %d", predecdig);
  if (print)
    elog(INFO, "postdec: %d", postdecdig);


  if (predecdig == 0x007F)
    predecdig = -1;
  predecdig = predecdig+1;

  if (print)
    elog(INFO, "predec: %d", predecdig);


  postdecdig = postdecdig / DEC_DIGITS + ((postdecdig % DEC_DIGITS == 0) ? 0 : 1 );
  if (print)
    elog(INFO, "postdec: %d", postdecdig);

  if (size < (predecdig + postdecdig)*2)
    postdecdig = 0;

  int cutoff = -1;
  if (size < predecdig * 2)
    cutoff = predecdig - (size/2);

  if (size == 0)
    predecdig = 0;

  if (header == 0) {
    predecdig = 0;
    postdecdig = 0;
  }

  
  while (predecdig > 0) {
    unsigned short numericdigit = 0;
    if (predecdig > cutoff) {
      numericdigit = *dec;
      dec++;
    }
    if (print)
      elog(INFO, "  numeric %d", numericdigit);
    val = 10000 * val + numericdigit;
    predecdig--;
  }

  
  bool stop = false;

  val = 10000 * val;
  while(postdecdig > 0) {
    unsigned short numericdigit = *dec;
    if (print)
      elog(INFO, "  numeric %d", numericdigit);
    if (!stop) {
      val = val + numericdigit;
      stop = true;
    }
    postdecdig--;
    dec++;
  }


  if (neg)
    val = -1*val;

  if (print)
    elog(INFO, "final val: %d", val);

  return (char*)dec;

}

char *nextVarDec(char *ch, Int32& val)
{
  int v;
  ch = nextVarDec(ch, v);
  val = v;
  return ch;
}


// todo: all types are decimal convert shupdate to decimal as well
// These are static pointers to three possible Operands

// Maximum number of registers on chip for each Operand Type
static Decimal   constOperand[MAXCONSTS];
static Decimal   tempOperand[MAXTEMPS];
static Decimal   varOperand[MAXVARS];

// OperandC holds the operand type of operation
struct OperandC {
  OperandType type;  // VAR CONST or TEMP
  DataType datatype;        // DECIMAL DATEADT or BOOL
  Int32 opno;          // operandnumber to be picked from the array
    
  Decimal getData() {
    Decimal val = 0;
    if (type == CONST)
      val = constOperand[opno];
    else if (type == VAR)
      val = varOperand[opno];
    else if (type == TEMP)
      val = tempOperand[opno];
    else
      printf("Error: Not a valid opernd type\n");
    return val;
  }
  void setData(Decimal val) {
    if (type == CONST)
      constOperand[opno] = val;
    else if (type == VAR)
      varOperand[opno] = val;
    else if (type == TEMP)
      tempOperand[opno] = val;
    else
      printf("Error: Not a valid opernd type\n");
  }

  char *setOps(char *ch) {
    int val;
    char lastch;
    ch = nextInt(ch,val,0,lastch);
    type = OperandType(val);
    ch = nextInt(ch,val,0,lastch);
    opno = val;
    ch = nextInt(ch,val,0,lastch);
    datatype = DataType(val);
    return ch;
  }
  
};

// ExprC performs an operation on left, right and result operands
struct ExprC {
  OperatorType operatortype; // NOT GT etc
  OperandC operandleft;
  OperandC operandright;
  OperandC result;

  char *setOps(char *ch) {
    int val;
    char lastch;
    ch = nextInt(ch,val,0,lastch);
    operatortype = OperatorType(val);
    ch = operandleft.setOps(ch);
    ch = operandright.setOps(ch);
    ch = result.setOps(ch);    
    return ch;
  }
  
  void exec() {
    //if (operandleft.type != operandright.type)
    //printf("Error: In expr both operand %d and %d must have the same type\n",operandleft.type,operandright.type);
    // convert left and right operands
    Decimal lval = operandleft.getData();
    Decimal rval = operandright.getData();
    Decimal res = 0;
    switch(operatortype) {
    case AND:
      res = lval & rval;
      break;
    case OR:
      res = lval & rval;	
      break;
    case NOT:
      res[0] = ~lval[0];
      break;
    case GT:
      if (lval > rval)
	res = 1;
      else
	res = 0;
      break;
    case LT:
      if (lval < rval)
	res = 1;
      else
	res = 0;	
      break;
    case GE:
      if (lval >= rval)
	res = 1;
      else
	res = 0;	
      break;
    case LE:
      if (lval <= rval)
	res = 1;
      else
	res = 0;	
      break;
    case EQ:
      if (lval == rval)
	res = 1;
      else
	res = 0;	
      break;
    case NEQ:
      if (lval != rval)
	res = 1;
      else
	res = 0;	
      break;
    case PLUS:
      res = lval+rval;
      break;
    case SUB:
      res = lval-rval;
      break;
    case MUL:
      res = lval*rval;
      break;
    case DIV:
      res = lval/rval;
      printf("Error: Divide opttype not supported.\n");	
      break;
    default:
      printf("Error: Invalid opttype.\n");
      break;
    }
    //if (res.uitofp(to_long()<0)
    //printf("Error: %d=%d op %d negative number as an op result\n",res.to_int(),lval.to_int(),rval.to_int());
    result.setData(res);
  }
};


void processCscan(char *setupbuf, char *varstream, int ntuples, char *result, int pcielanes,
		int deviceslrs, int query, bool verifyCompute) 
{

  // expression and aggregation registers
  ExprC exprs[MAXSCANS];
  
  // Constants
  int absday94_01_jan = getPgAbsDay(1994,1,1);
  int absday95_01_jan = getPgAbsDay(1995,1,1);
  
  int absday98_15_aug = getPgAbsDay(1998,8,15);

  // read constants first
  char *ch = setupbuf;

  //if (*ch != char(QDefs::PUSetupBlock)) {
  //  printf("Incorrect setup data block %c\n", *ch);
  //}
  
  ch++;
  int total_consts = 0;
  char lastch;
  ch = nextInt(ch,total_consts,0,lastch);
  if (total_consts < 0 || total_consts > MAXCONSTS) {
    printf("Error: Required const %d exceeds allowed maximum constants %d\n" ,total_consts, MAXCONSTS);
  }
  for (int i=0;i<total_consts;i++) {
    int val;
    ch = nextInt(ch,val,0,lastch);
    constOperand[i] = val;
  }
  if (*ch != '\n')
    printf("Error: no newline after cont %c\n",*ch);
  ch++;
  
  // now read the scan expressions
  int total_exprs = 0;
  ch = nextInt(ch,total_exprs,0,lastch);
  if (total_exprs < 0 || total_exprs > MAXSCANS) {
    printf("Error: Required exprs %d exceeds allowed maximum exprs %d\n" ,total_exprs, MAXSCANS);
  }

  for (int i=0;i<total_exprs;i++) {
    ch = exprs[i].setOps(ch);
  }
  if (*ch != '\n')
    printf("Error: no newline after exprs %c\n",*ch);
  ch++;

  // now read the num columns/ column types
  int cols[MAXCONSTS];
  int total_cols = 0;
  ch = nextInt(ch,total_cols,0,lastch);
  if (total_cols < 0 || total_cols > MAXSCANS) {
    printf("Error: Required num columns %d exceeds allowed maximum columns %d\n" ,total_cols, MAXSCANS);
  }
  for (int i=0;i<total_cols;i++) {
    int val;
    ch = nextInt(ch,val,0,lastch);
    cols[i] = val;
  }
  if (*ch != '\n')
    printf("Error: no newline after cols %c\n",*ch);
  ch++;

  Int32 fl_quantity=0;
  //  Int32 fl_extended_price =0; 
  Int32 fl_discount =0;       
  DateAdt fl_shipdate;          
  Decimal fl_revenue = 0;   // Needs float to hold the
  Int32 colIntVal=0;
  DateAdt colDateVal;          

  long setupbufsize = (ch-setupbuf);
  
  int totalRows = 0;
  int scannedRows = 0;  
  
  ch = varstream;
  // Reset all temp registers to 0
  for (int i=0; i< MAXTEMPS; i++) 
    tempOperand[i] = 0;

  Decimal fl_compute = 0; //This stores the result of the direct compute, for verification purpose

  // Now operate the expressions on the given opcodes
  for (int i=0; i<ntuples;i++) {
    // todo change it to use variable count from headers;
    //      needs shipdate changed to decimal as well
    /*ch = nextDec(ch,fl_quantity);    
    //    ch = nextDec(ch,fl_extended_price);
    ch = nextDec(ch,fl_discount);
    ch = nextInt(ch,fl_shipdate,0,lastch);
    varOperand[l_quantity_scan] = Decimal(fl_quantity);
    //    varOperand[l_extendedprice] = Decimal(fl_extended_price);
    varOperand[l_discount_scan] = Decimal(fl_discount);
    varOperand[l_shipdate_scan] = Decimal(fl_shipdate);
    // skip the newline
    ch++;
    totalRows++;*/
    for(int j = 0; j < total_cols; ++j) {
      if (cols[j] == 1) {
      // if coltype is Int32, call nextDec
        ch = nextDec(ch, colIntVal);
        varOperand[j] = Decimal(colIntVal);
      } else if (cols[j] == 2) {
      // if coltype is Date, call nextInt
        ch = nextInt(ch,colDateVal,0,lastch);
        varOperand[j] = Decimal(colDateVal);
      } else {
        printf("Error: Unknown column type, check if setupbuf colstream is correctly handled" );
      }
    }
    ch++;
    totalRows++;
    
    if(query == 6 && verifyCompute) {
     if ((fl_shipdate >= absday94_01_jan && fl_shipdate < absday95_01_jan) &&
    		(fl_discount >= 500 && fl_discount <= 700) &&
    		fl_quantity < 240000) {
    	      scannedRows++;
	      //    fl_compute = fl_compute + fl_extended_price *fl_discount;
      }
    } else if(query == 1 && verifyCompute) {
      if(fl_shipdate <= absday98_15_aug) {
    	scannedRows++;
	//    fl_compute = fl_compute + (10000 - fl_discount)*fl_extended_price;
      }
    }

    // first scan todo change shipdate to decimal, add # of exprs in setup header
    // scan result is alwways in tempOperand[TEMP0]
    for (int j=0; j<total_exprs; j++)
      exprs[j].exec();
    // Aggregate only if row to be scanned
    if (tempOperand[TEMP0] != 0) {
      scannedRows++;
      result[i] = true;
    }
    else {
      result[i] = false;
    }
  }
  //printf("Result: %s - %d\n",result,result);

  // printf("Direct: Rows=%d Revenue = %.4lf\n",scanned_rows,float(fl_revenue)/100000000.00);
  if (totalRows > 0 ) {
    printf("Info: processed %d total rows, filtered to %d rows , %.2f of total rows\n", totalRows, scannedRows, float(scannedRows)/float(totalRows));
  }
}

void processCscanNew(char *setupbuf, char *varstream, int ntuples, 
   char *result, int pcielanes,
   int deviceslrs, int query, bool verifyCompute) 
{

  bool * results = (bool*)result;
   
   //printf("setupbuf %s\n", setupbuf);
   //printf("varstream %s\n", varstream);
   //elog(INFO, "ntuples %d\n", ntuples);
   //return;
   // expression and aggregation registers
   //printf("varstream %d\n", varstream);

   ExprC exprs[MAXSCANS];

   // Constants
   int absday94_01_jan = getPgAbsDay(1995, 1, 1);
   int absday95_01_jan = getPgAbsDay(1996, 1, 1);

   int absday98_15_aug = getPgAbsDay(1998, 8, 15);

   // read constants first
   char *ch = setupbuf;
   //if (*ch++ != QDefs::PUSetupBlock) {
   //  printf("Incorrect setup data block\n");
   //}
   ch++;
   int total_consts = 0;
   char lastch;
   ch = nextInt(ch, total_consts, 0, lastch);
   if (total_consts < 0 || total_consts > MAXCONSTS) {
      printf("Error: Required const %d exceeds allowed maximum constants %d\n", total_consts, MAXCONSTS);
   }
   for (int i = 0; i<total_consts; i++) {
      int val;
      ch = nextInt(ch, val, 0, lastch);
      constOperand[i] = val;
   }
   if (*ch != '\n')
      printf("Error: no newline after cont %c\n", *ch);
   ch++;

   // now read the scan expressions
   int total_exprs = 0;
   ch = nextInt(ch, total_exprs, 0, lastch);
   if (total_exprs < 0 || total_exprs >MAXSCANS) {
      printf("Error: Required exprs %d exceeds allowed maximum exprs %d\n", total_exprs, MAXSCANS);
   }

   for (int i = 0; i<total_exprs; i++) {
      ch = exprs[i].setOps(ch);
   }
   if (*ch != '\n')
      printf("Error: no newline after exprs %c\n", *ch);
   ch++;

   // now read the num columns/ column types
   int cols[MAXCONSTS];
   int total_cols = 0;
   ch = nextInt(ch,total_cols,0,lastch);
   if (total_cols < 0 || total_cols > MAXSCANS) {
     printf("Error: Required num columns %d exceeds allowed maximum columns %d\n" ,total_cols, MAXSCANS);
   }
   for (int i=0;i<total_cols;i++) {
     int val;
     ch = nextInt(ch,val,0,lastch);
     cols[i] = val;
   }
   if (*ch != '\n')
     printf("Error: no newline after cols %c\n",*ch);
   ch++;

   Int32 fl_quantity = 0;
   Int32 fl_extended_price = 0;
   Int32 fl_discount = 0;
   DateAdt fl_shipdate;
   Decimal fl_revenue = 0;   // Needs float to hold the

   Int32 colIntVal=0;
   DateAdt colDateVal;  

   long setupbufsize = (ch - setupbuf);

   int totalRows = 0;
   int scannedRows = 0;

   ch = varstream;
   // Reset all temp registers to 0
   for (int i = 0; i< MAXTEMPS; i++)
      tempOperand[i] = 0;

   //Decimal fl_compute = 0; //This stores the result of the direct compute, for verification purpose
   
   //for (int i = 0; i < ntuples; i++) {
   //   result[i] = true;
   //}
   //return;
   // Now operate the expressions on the given opcodes
   for (int i = 0; i<ntuples; i++) {
     //  todo    needs shipdate changed to decimal as well
     for(int j = 0; j < total_cols; ++j) {
       if (cols[j] == 1) {
         // if coltype is Int32, call nextVarDec
         ch = nextVarDec(ch, colIntVal);
         varOperand[j] = Decimal(colIntVal);
       } else if (cols[j] == 2) {
         // if coltype is Date, call nextVarInt
         ch = nextVarInt(ch,colDateVal);
         varOperand[j] = Decimal(colDateVal);
       } else {
         printf("Error: Unknown column type, check if setupbuf colstream is correctly handled" );
       }
     }
     totalRows++;


      if (query == 6 && verifyCompute) {
         if ((fl_shipdate >= absday94_01_jan && fl_shipdate < absday95_01_jan) &&
            (fl_discount >= 800 && fl_discount <= 1000) &&
            fl_quantity < 240000) {
            scannedRows++;
	    fl_compute_new = fl_compute_new + fl_extended_price * fl_discount;
         } 
      }
      else if (query == 1 && verifyCompute) {
         if (fl_shipdate <= absday98_15_aug) {
            scannedRows++;
	    fl_compute_new = fl_compute_new + (10000 - fl_discount)*fl_extended_price;
         }
      }

      // first scan todo change shipdate to decimal, add # of exprs in setup header
      // scan result is alwways in tempOperand[TEMP0]
      for (int j = 0; j<total_exprs; j++)
         exprs[j].exec();
      // Aggregate only if row to be scanned
      if (tempOperand[TEMP0] != 0) {
         scannedRows++;
         results[i] = true;
      }
      else {
         results[i] = false;
      }

      //if (limit < LOGLIMIT)
      //if (results[i] == true)
      //elog(INFO, "cscan: result id %d is %d of %d ", i, results[i], ntuples);
   }

 //           elog(INFO, "agg value - %ld", fl_compute_new.to_long());
   // printf("Direct: Rows=%d Revenue = %.4lf\n",scanned_rows,float(fl_revenue)/100000000.00);
   printf("Info: For query processed %d total rows, filtered to %d rows , %.2f of total rows\n", totalRows, scannedRows, float(scannedRows) / float(totalRows));
}


