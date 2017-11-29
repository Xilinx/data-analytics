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
/* sql6_demo.cpp generates input of FPGA kernel interface
 * Fixes - 
 *    Fixed aggegration rules
 *    For now using text based headers, added const numbers, expr numbers, aggr nos inthe header
 * Bugs/enhancement
 *    Add number of fields in the header
 *    shipdate to be changed to decimal header format
 */
#include "sqlcmds.h"

using namespace std;
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
//#include "sqlcommon.h"


unsigned char confq6QualifierN = 9;
/* 'qualstream' represents the stream of filter conditions used in 6.sql.
 * The format of qualstream is as the following:
 * |Operator Type|Operand1 Type| Index | Data Type |Operand1 Type| Index | Data Type |Result Type| Index |
 * |-----int-----|-----int-----|--int--|----int----|-----int-----|--int--|----int----|----int----|--int--|
 * The 'Operator Type' is enumerated in enum type 'OperatorType' declared in common.h.
 * The 'Operand Type' is enumerated in enum type 'OperandType' declared in common.h.
 * The 'Index' is enumerated in enum type 'VarNo' and 'ConNo' declared in sql6_demo.cpp. 
 */
short confq6QualifierExpr[9][10] = {
  /* save the result of "l_shipdate >= date '1994-01-01'" to TEMP0
   */
  {GE, VAR, l_shipdate, DATEADT, CONST, exp1_con, DATEADT, TEMP, TEMP0, BOOL},
//
  /* save the result of "l_shipdate < date '1994-01-01' + interval '1' year" to TEMP1
   */
  {LT, VAR, l_shipdate, DATEADT, CONST, exp2_con, DATEADT, TEMP, TEMP1, BOOL},

  /* save the result of "l_shipdate >= date '1994-01-01' 
   * and l_shipdate < date '1994-01-1' + interval '1' year" to TEMP0
   */
  {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},

  /* Decompose "l_discount between .06 - 0.01 and .06 + 0.01" as
   * a. "l_discount >= .05" and
   * b. "l_discount <= .07"
   */
  /* save the result of "l_discount <.05" to TEMP1
   */
  {GE, VAR, l_discount, DECIMAL, CONST, exp3_1_con, DECIMAL, TEMP, TEMP1, BOOL},
  {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},
  {LE, VAR, l_discount, DECIMAL, CONST, exp3_2_con, DECIMAL, TEMP, TEMP1, BOOL},
  {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},
  /* save the result of "l_quantity < 24" to TEMP1
   */
  {LT, VAR, l_quantity, DECIMAL, CONST, exp4_con, DECIMAL, TEMP, TEMP1, BOOL},
  /* save the result of the whole qualifier to TEMP0
   */
  {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL}
};

// map l_shipdate->2; l_discount->1, l_quantity->unchg
short confq6QualifierExprScanOnly [9][10] = {
  /* save the result of "l_shipdate >= date '1994-01-01'" to TEMP0
   */
  {GE, VAR, 2, DATEADT, CONST, exp1_con, DATEADT, TEMP, TEMP0, BOOL},

  /* save the result of "l_shipdate < date '1994-01-01' + interval '1' year" to TEMP1
   */
  {LT, VAR, 2, DATEADT, CONST, exp2_con, DATEADT, TEMP, TEMP1, BOOL},

  /* save the result of "l_shipdate >= date '1994-01-01' 
   * and l_shipdate < date '1994-01-1' + interval '1' year" to TEMP0
   */
  {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},

  /* Decompose "l_discount between .06 - 0.01 and .06 + 0.01" as
   * a. "l_discount >= .05" and
   * b. "l_discount <= .07"
   */
  /* save the result of "l_discount <.05" to TEMP1
   */
  {GE, VAR, 1, DECIMAL, CONST, exp3_1_con, DECIMAL, TEMP, TEMP1, BOOL},
  {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},
  {LE, VAR, 1, DECIMAL, CONST, exp3_2_con, DECIMAL, TEMP, TEMP1, BOOL},
  {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},
  /* save the result of "l_quantity < 24" to TEMP1
   */
  {LT, VAR, l_quantity, DECIMAL, CONST, exp4_con, DECIMAL, TEMP, TEMP1, BOOL},
  /* save the result of the whole qualifier to TEMP0
   */
  {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL}
};



unsigned char confq6AggregationN = 2;
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
short confq6AggregationExpr[2][10] = {
  {MUL, VAR, l_extendedprice, DECIMAL, VAR, l_discount, DECIMAL, TEMP, TEMP2, DECIMAL},
  {PLUS, TEMP, TEMP2, DECIMAL, TEMP, TEMP3, DECIMAL, TEMP, TEMP3, DECIMAL}
};

unsigned char* confq6Header (unsigned char* p_buf)
{
  *p_buf++ = 0xFFu;
  return p_buf;
}

// big endian
unsigned char* confWriteLong (unsigned char* p_buf, long data)
{
  for (int i=7; i >= 0; --i) {
    unsigned char ch = (data >> i * 8) & 0xFF;
    *p_buf++ = ch;
  }
  return p_buf;
}

unsigned char* confWriteShort (unsigned char* p_buf, short data)
{
  for (int i=1; i >= 0; --i) {
    unsigned char ch = (data >> i * 8) & 0xFF;
    *p_buf++ = ch;
  }
  return p_buf;
}

unsigned char* confWriteExpr (unsigned char* p_buf, short exprData [10])
{
  for (int i=0; i<10; ++i) {
    p_buf = confWriteShort (p_buf, exprData[i]);
  }
  return p_buf;
}

// all consts are 64 bit longs
unsigned char* confq6Consts (unsigned char* p_buf)
{
  *p_buf++ = 5;

  //int jd19940101 = -2191; //date2j (1994, 1, 1) - POSTGRES_EPOCH_JDATE;
  //int jd19950101 = -1826; //date2j (1995, 1, 1) - POSTGRES_EPOCH_JDATE;
  // numerics/decimals are multiplied by 100 in kernel. Other types arent
  long j19940101 = -2191;  //1994-01-01
  long j19950101 = -1826;  //1995-01-01
  long discLo = 5;     //0.05 * 100
  long discHi = 7;      //0.07 * 100
  long qtyHi = 2400;   //24 * 100

  p_buf = confWriteLong (p_buf, j19940101);
  p_buf = confWriteLong (p_buf, j19950101);
  p_buf = confWriteLong (p_buf, discLo);
  p_buf = confWriteLong (p_buf, discHi);
  p_buf = confWriteLong (p_buf, qtyHi);
  return p_buf;
}

unsigned char* confq6Qualifier (unsigned char* p_buf, unsigned char scanType)
{
  *p_buf++ = confq6QualifierN;
  for (int i=0; i < confq6QualifierN; ++i) {
    if (scanType == KERNEL_MODE_SCANAGGR)
      p_buf = confWriteExpr (p_buf, confq6QualifierExpr [i]);
    else
      p_buf = confWriteExpr (p_buf, confq6QualifierExprScanOnly [i]);
  }
  return p_buf;
}

unsigned char* confq6Aggregation (unsigned char* p_buf)
{
  *p_buf++ = confq6AggregationN;
  for (int i=0; i < confq6AggregationN; ++i) {
    p_buf = confWriteExpr (p_buf, confq6AggregationExpr [i]);
  }
  return p_buf;
}

unsigned char* confq6ColTypes (unsigned char* p_buf, unsigned char scanType)
{
  if (scanType == KERNEL_MODE_SCANONLY) {
    *p_buf++ = 0x03u;
    *p_buf++ = 0x01u; // l_quantity (long)
    *p_buf++ = 0x01u; // l_discount (long)
    *p_buf++ = 0x02u; // l_shipdate (int)
  } else if (scanType == KERNEL_MODE_SCANAGGR) {
    *p_buf++ = 0x04u;
    *p_buf++ = 0x01u; // l_quantity (long)
    *p_buf++ = 0x01u; // l_extendedprice (long)
    *p_buf++ = 0x01u; // l_discount (long)
    *p_buf++ = 0x02u; // l_shipdate (int)
  } else {
    std::cout << "ERROR: invalid scanType " << (short)scanType << std::endl;
  }
  return p_buf;
}

unsigned char* confq6 (unsigned char* p_buf, unsigned char scanType)
{
  p_buf = confq6Header (p_buf);
  p_buf = confq6ColTypes (p_buf, scanType);
  p_buf = confq6Consts (p_buf);
  p_buf = confq6Qualifier (p_buf, scanType);
  if (scanType == KERNEL_MODE_SCANAGGR) {
    p_buf = confq6Aggregation (p_buf);
  } else {
    *p_buf++ = 0x00u; // confq6AggregationN = 0
  }
  return p_buf;
}

//----------------------------------------------------------------------------
// for debug
const char* DataTypeNames [] = {
  "BOOL", "DECIMAL", "DATEADT", "STRING", "CHAR", "INT", "INT16",
  "INT64", "UINT16", "UINT32", "UINT64", "FLOAT", "DOUBLE" };

const char* OperatorTypeNames [] = {
  "AND", "OR", "NOT", "GT", "LT", "GE", "LE", "EQ", "NEQ", "PLUS", "SUB", "MUL", "DIV" };

const char* OperandTypeNames [] = {
  "CONST", "VAR", "TEMP" };

const char* TempNoNames [] {
  "TEMP0", "TEMP1", "TEMP2", "TEMP3", "TEMP4", "TEMP5", "TEMP6", "TEMP7" } ;

const char* VarNoNames [] = {
  "l_quantity", "l_extendedprice", "l_discount", "l_shipdate" };

const char* ConNoNames [] = {
  "exp1_con", "exp2_con", "exp3_1_con", "exp3_2_con", "exp4_con", "exp5_con", "exp6_con"
};

std::ostream& operator<< (std::ostream& os, const HostOperand& op)
{
  os << OperandTypeNames [op. operandtype] << ",";
  switch (op. operandtype) {
    case VAR: os << VarNoNames[op.opno]; break;
    case CONST: os << ConNoNames[op.opno]; break;
    case TEMP: os << TempNoNames[op.opno]; break;
    default: os << "BadOpNo " << op.opno;
  }
  os << "," << DataTypeNames [op.datatype];
  os << " ";
  return os;
}

std::ostream& operator<< (std::ostream& os, const HostExpr& ex)
{
  os << OperatorTypeNames [ex.operatortype] << ",";
  os << ex.operandleft << "," << ex.operandright << "," <<  ex.result << " ";
  return os;
}

void confDump (unsigned char* p_buf)
{
  for (int i=0; i < 4096; ++i) {
    if (i && i%4==0) printf (" ");
    if (i && i%32==0) printf ("\n");
    printf ("%02X", p_buf[i]);
  }
  std::cout << std::endl;
}

bool confq6Verify (unsigned char* p_buf, unsigned char scanType)
{
  confDump (p_buf);

  ConfBufStream confBufStream (p_buf);
  HostConfParser <ConfBufStream> prsr;

  HostReg puReg;
  HostExprs puExprs;

  if (! prsr. checkHeader (confBufStream)){
    std::cout << "confq6Verify: !Setup Page" << std::endl;
    return false;
  }
  std::cout << "confq6Verify: Setup Page " << std::endl;

  unsigned char nFiltCols = prsr. getFiltCols (confBufStream, puReg.ColTypeReg);
  //Dbg1 std::cout << "confq6Verify #FiltCols " << (short)nFiltCols << std::endl;
  for (int i=0; i < nFiltCols; ++i) 
    std::cout << "FiltCols[" << i << "] " << (short)puReg. ColTypeReg [i] << std::endl;

  unsigned char nConsts = prsr. getConsts (confBufStream, puReg);
  std::cout << "confq6Verify #Consts " << (short)nConsts << std::endl;
  for (int i=0; i<nConsts; ++i)
    std::cout << "Const[" << i << "] " << puReg. ConstReg [i] << std::endl;

  unsigned char nScans  = prsr. getExprs (confBufStream, puExprs.ScanExpr);
  puExprs.NoOfScanExprs = nScans;
  std::cout << "confq6Verify #Scans " << (short)nScans << std::endl;
  for (int i=0; i < nScans; ++i) 
    std::cout << "{" << puExprs.ScanExpr [i] << "}" << std::endl;

  unsigned char nAggrs  = prsr. getExprs (confBufStream, puExprs.AggrExpr);
  puExprs.NoOfAggrExprs = nAggrs;
  std::cout << "confq6Verify #Aggrs " << (short)nAggrs << std::endl;
  for (int i=0; i < nAggrs; ++i) 
    std::cout << "{" << puExprs.AggrExpr [i] << "}" << std::endl;

  prsr. skipToEnd (confBufStream);

  // TODO diff this puReg and the one from the kernel

  if (scanType == KERNEL_MODE_SCANONLY && nAggrs != 0) {
    std::cout << "ERROR: confq6Verify #Aggrs should be 0 for SCANONLY kernel" << std::endl;
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#define MAX_OPERAND_COUNT 50;

/*Three arguments required by opencl kernel interface include:
 * a. qualstream consisting of filter condition statements used to evaluate whether each tuple, i.e., row, meets specified condition 
 * b. varstream consisting of attributes involved in filter condition used in 6.sql
 * c. constream consisting of const variables involved in filter condition used in 6.sql
 */


/*typedef struct Expr{
  OperandType otype;
  Index index[MAX_OPERAND_COUNT];
  DataType dtype[MAX_OPERAND_COUNT];
  OperandType otype[MAX_OPERAND_COUNT];
  }Expr;*/

/*'process_csv' extracts all the attributes involved in 6.sql from 'lineitem.csv'
 * and stores the values to 'sql6.var' file.
 */
void
process_csv(string csvfile, string varfile)
{
  //const char *csv = "lineitem.csv";
  const char *csv = csvfile.c_str();
  //const char *res = "sql6.var";
  const char *res = varfile.c_str();
  FILE *fcsv = fopen(csv, "r");
  FILE *fres = fopen(res, "w");
  if (NULL == fcsv || NULL == fres)
    exit(EXIT_FAILURE); 

  /*The below array contains the index of attributes in lineitem.
   *  l_quantity = 5
   *  l_extendedprice = 6
   *  l_discount = 7
   *  l_shipdate = 11
   */
  int attidx[]={5, 6, 7, 11};
  ssize_t read;
  int i, j;
  size_t len = 0;
  char *item = NULL;
  const char *dem = "|";
  //todo int attnum = 3;
  pg_tm_t tm;
  int absday;
  string stnum;
  Numeric num;

  /*Read each line of lineitem.csv and formalize the values
   */
  while ( -1 != (read = getline(&item, &len, fcsv)))
    {
      i = j = 0;
      char *p = strtok(item, dem);
      i = 1;
      while (NULL != p)
	{
	  if ( i == attidx[j] )
	    {
	      switch (attidx[j])
		{
		case 5:
		  num = decimal_to_numeric(p);
		  stnum = numeric_to_string(num);
		  fprintf(fres, "%s;", stnum.c_str());
		  break;
		case 6:
		  num = decimal_to_numeric(p);
		  stnum = numeric_to_string(num);
		  fprintf(fres, "%s;", stnum.c_str());
		  break;
		case 7:
		  num = decimal_to_numeric(p);
		  stnum = numeric_to_string(num);
		  fprintf(fres, "%s;", stnum.c_str());
		  break;
		case 11:
		  /* 'make_date' converts string to pg_tm struct type variable;
		   * 'PG_TM_2_DATE(tm)' converts pg_tm variable to absolute day relative to POSTGRES_EPOCH_JDATE;
		   */
		  make_date(p, &tm);
		  absday = PG_TM_2_DATE(tm);												
		  fprintf(fres, "%d;\n", absday);
		  break;
		default:
		  break;
		}
	      j++;
	    }
	  i++;
	  p = strtok(NULL, dem);
	}
    }
}


/*'process_const' formalize const variables and stores them to constream
 */
int process_const(char *constream, bool keep_decimal) {
  string ss;
  int no_consts = 5;
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
    ss.append("5");
  ss.push_back('|');

  if (keep_decimal)
    ss.append("0.0700");    
  else
    ss.append("7");
  ss.push_back('|');  

  if (keep_decimal)
    ss.append("24");    
  else
    ss.append("2400");
  ss.push_back('|');  
  
  ss.push_back('\n');
  
  //printf("absday(%d-%d-%d) = %d\n", tm.year, tm.month, tm.day, absday);
  int ss_len = ss.length();
  //*constream = (char *)malloc(ss_len);
  memcpy(constream, ss.c_str(), ss_len);
  return ss_len;
}

/* 'process_var' generates varstream consisting of strings correspond to each column value involved in 6.sql
 */
bool process_var(char **varstream, string varfile) {
  string file_content;
  ifstream file(varfile.c_str());
  if (!file.good()) {
    printf("Error: Cannot read varfile %s.\n",varfile.c_str());
    return false;
  }

  string line;
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

int append_expr(char *setupstream, int (*expr) [10], int no_exprs) { 

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

  int ss_len = ss.length();
  memcpy(setupstream, ss.c_str(), ss_len);
  return ss_len;
}
int process_aggregation(char *aggstream, int query) {
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

  int ss_len;
  if (query == 1)
    ss_len = append_expr(aggstream, expr1, 7);
  else
    ss_len = append_expr(aggstream, expr6, 2);
  return ss_len;
  
}

/* 'process_var_stream_type' generates varstreamtype consisting of enumeration value correspond to each column type in var stream
 */
int process_var_stream_type(char *varstream_type, int query) {
  string ss;
  if(query == 6) {
    int no_cols = 3;
    ss.append(tostring(no_cols));
    ss.push_back('|');
    //DECIMAL is of type Enum 1
    //DATE is of type Enum 2
    //Query6 has DECIMAL/DECIMAL/DATE in sequence
    ss.push_back('1');
    ss.push_back('|');
    ss.push_back('1');
    ss.push_back('|');
    ss.push_back('2');
    ss.push_back('|');
    ss.push_back('\n');
  }
  
  //printf("absday(%d-%d-%d) = %d\n", tm.year, tm.month, tm.day, absday);
  int ss_len = ss.length();
  //*constream = (char *)malloc(ss_len);
  memcpy(varstream_type, ss.c_str(), ss_len);
  return ss_len;
}

int process_qualifier_scan(char *qualstream, int query) {
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

    /* Decompose "l_discount_scan between .06 - 0.01 and .06 + 0.01" as
     * a. "l_discount_scan >= .05" and
     * b. "l_discount_scan <= .07"
     */
    /* save the result of "l_discount_scan <.05" to TEMP1
     */
    {GE, VAR, l_discount_scan, DECIMAL, CONST, exp3_1_con, DECIMAL, TEMP, TEMP1, BOOL},
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},
    {LE, VAR, l_discount_scan, DECIMAL, CONST, exp3_2_con, DECIMAL, TEMP, TEMP1, BOOL},
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},
    /* save the result of "l_quantity_scan < 24" to TEMP1
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

  int ss_len;
  if (query == 1)
    ss_len = append_expr(qualstream, expr1, 1);
  else
    ss_len = append_expr(qualstream, expr6, 9);
  return ss_len;
  
}

int process_qualifier_scan_aggr(char *qualstream, int query) {
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
    /* save the result of "l_shipdate >= date '1994-01-01'" to TEMP0
     */
    {GE, VAR, l_shipdate, DATEADT, CONST, exp1_con, DATEADT, TEMP, TEMP0, BOOL}, 

    /* save the result of "l_shipdate < date '1994-01-01' + interval '1' year" to TEMP1
     */
    {LT, VAR, l_shipdate, DATEADT, CONST, exp2_con, DATEADT, TEMP, TEMP1, BOOL}, 

    /* save the result of "l_shipdate >= date '1994-01-01' 
     * and l_shipdate < date '1994-01-1' + interval '1' year" to TEMP0
     */
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},

    /* Decompose "l_discount between .06 - 0.01 and .06 + 0.01" as
     * a. "l_discount >= .05" and
     * b. "l_discount <= .07"
     */
    /* save the result of "l_discount <.05" to TEMP1
     */
    {GE, VAR, l_discount, DECIMAL, CONST, exp3_1_con, DECIMAL, TEMP, TEMP1, BOOL},
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},
    {LE, VAR, l_discount, DECIMAL, CONST, exp3_2_con, DECIMAL, TEMP, TEMP1, BOOL},
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},
    /* save the result of "l_quantity < 24" to TEMP1
     */
    {LT, VAR, l_quantity, DECIMAL, CONST, exp4_con, DECIMAL, TEMP, TEMP1, BOOL},
    /* save the result of the whole qualifier to TEMP0
     */
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL}
  };

  // l_shipdate <= date '1998-12-01' - interval '108' day
  int expr1[1][10] = {
    /* save the result of "l_shipdate >= date '1998-08-15'" to TEMP0   */
    {LE, VAR, l_shipdate, DATEADT, CONST, exp5_con, DATEADT, TEMP, TEMP0, BOOL}
  };
  
  /*
  int stride = sizeof(int) * 10;
  *qualstream = (int *)malloc(sizeof(int) * 9 * 10);
  for ( int i = 0; i < 9; i++)
    {
      //todo memcpy(*qualstream + stride * i, expr[i], stride);
      memcpy(*qualstream + 10 * i, expr[i], stride);      
      }*/

  int ss_len;
  if (query == 1)
    ss_len = append_expr(qualstream, expr1, 1);
  else
    ss_len = append_expr(qualstream, expr6, 9);
  return ss_len;
  
}

/* 'process_qualifier' generates qualstream consisting of enumeration value correspond to each filter condition in 6.sql
 */
int process_qualifier(char *qualstream, int query) {
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
    /* save the result of "l_shipdate >= date '1994-01-01'" to TEMP0
     */
    {GE, VAR, l_shipdate, DATEADT, CONST, exp1_con, DATEADT, TEMP, TEMP0, BOOL}, 

    /* save the result of "l_shipdate < date '1994-01-01' + interval '1' year" to TEMP1
     */
    {LT, VAR, l_shipdate, DATEADT, CONST, exp2_con, DATEADT, TEMP, TEMP1, BOOL}, 

    /* save the result of "l_shipdate >= date '1994-01-01' 
     * and l_shipdate < date '1994-01-1' + interval '1' year" to TEMP0
     */
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},

    /* Decompose "l_discount between .06 - 0.01 and .06 + 0.01" as
     * a. "l_discount >= .05" and
     * b. "l_discount <= .07"
     */
    /* save the result of "l_discount <.05" to TEMP1
     */
    {GE, VAR, l_discount, DECIMAL, CONST, exp3_1_con, DECIMAL, TEMP, TEMP1, BOOL},
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},
    {LE, VAR, l_discount, DECIMAL, CONST, exp3_2_con, DECIMAL, TEMP, TEMP1, BOOL},
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL},
    /* save the result of "l_quantity < 24" to TEMP1
     */
    {LT, VAR, l_quantity, DECIMAL, CONST, exp4_con, DECIMAL, TEMP, TEMP1, BOOL},
    /* save the result of the whole qualifier to TEMP0
     */
    {AND, TEMP, TEMP0, BOOL, TEMP, TEMP1, BOOL, TEMP, TEMP0, BOOL}
  };

  // l_shipdate <= date '1998-12-01' - interval '108' day
  int expr1[1][10] = {
    /* save the result of "l_shipdate >= date '1998-08-15'" to TEMP0   */
    {LE, VAR, l_shipdate, DATEADT, CONST, exp5_con, DATEADT, TEMP, TEMP0, BOOL}
  };
  
  /*
  int stride = sizeof(int) * 10;
  *qualstream = (int *)malloc(sizeof(int) * 9 * 10);
  for ( int i = 0; i < 9; i++)
    {
      //todo memcpy(*qualstream + stride * i, expr[i], stride);
      memcpy(*qualstream + 10 * i, expr[i], stride);      
      }*/

  int ss_len;
  if (query == 1)
    ss_len = append_expr(qualstream, expr1, 1);
  else
    ss_len = append_expr(qualstream, expr6, 9);
  return ss_len;
  
}

