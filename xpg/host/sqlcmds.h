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
#ifndef SQLENGINE_SQLCMDS_H
#define SQLENGINE_SQLCMDS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "date.h"
#include "sqldecimal.h"
#include "decimal.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "qdefs.h"
#include "xdbconn.h"

typedef enum{
  BOOL=0,
  DECIMAL,
  DATEADT,
  STRING,
  CHAR,
  INT,
  INT16,
  INT64,
  UINT16,
  UINT32,
  UINT64,
  FLOAT,
  DOUBLE
  } DataType;

/* Each enumerated value represents the name of common operators involved in
 * logic expressions and arithmetic expressions.
 */
typedef enum{
  AND=0,
  OR,
  NOT,
  GT,
  LT,
  GE,
  LE,
  EQ,
  NEQ,
  PLUS,
  SUB,
  MUL,
  DIV} OperatorType;

typedef enum{
  CONST=0,
  VAR,
  TEMP,
  EXPR} OperandType;

/* {TEMP0,TEMP1} corresponds to temporal results of filter conditions
 * and {TEMP2,TEMP3} corresponds to temporal results of aggregation statement.
 */
/* The kernel function 'process_scan' should allocate local variable to store the below temporal results.
 */
typedef enum{
  TEMP0=0, 
  TEMP1,
  TEMP2,
  TEMP3,
  TEMP4,
  TEMP5,
  TEMP6,
  TEMP7
} TempNo;

/*VarNo enumerates the sequence order of each attribute in varstream for 6.sql
 */
typedef enum{
  l_quantity,		//correspond to attribute 'l_quantity' in lineitem table
  l_extendedprice,//correspond to attribute 'l_extendedprice' in lineitem table
  l_discount,		//correspond to attribute 'l_discount' in lineitem table
  l_shipdate		//correspond to attribute 'l_shipdate' in lineitem table

} VarNo;

/*VarNo_Scan enumerates the sequence order of each attribute in varstream for 6.sql for scan filter only
 */
typedef enum{
  l_quantity_scan,		//correspond to attribute 'l_quantity' in lineitem table
  l_discount_scan,		//correspond to attribute 'l_discount' in lineitem table
  l_shipdate_scan		//correspond to attribute 'l_shipdate' in lineitem table

} VarNo_Scan;

/*ConNo enumerates the sequence order of each const variable in constream for 6.sql
 */
typedef enum{
  exp1_con,  //corresponding to '1994-01-1'
  exp2_con,  //corresponding to '1994-01-01' + interval '1' year
  exp3_1_con,//corresponding to '.06 - 0.01' 
  exp3_2_con,//corresponding to '.06 - 0.01' 
  exp4_con,  //corresponding to '24'
  exp5_con,   //corresponding to '1998-12-01' - interval '108' days
  exp6_con   //corresponding to '1'  
  
}ConNo;

const unsigned char KERNEL_MODE_SCANONLY = 0x00u;
const unsigned char KERNEL_MODE_SCANAGGR = 0x01u;
#define CONF_BYTES 4096

// Maximum number of registers on chip for each Operand Type
static const int MAXCONSTS = 9;
static const int MAXVARS = 9;
static const int MAXTEMPS = 9;
// 16 bits long int, maximum 5 decimal bits max int: 64576
typedef SqlDecimal<5,0,16> SqlInt16b;
// 4 bits of decimal need only 14 bits for Ali notation, for now using all 16bits of short
// sql decimal with 15 predecimal, 4 post decimal bits Use this to hold all float numbers
typedef SqlDecimal<15,4,80> SqlDec;

struct Reg {
  SqlDec ConstReg[MAXCONSTS];
  SqlInt16b ColTypeReg[MAXVARS];
  SqlDec TempReg[MAXTEMPS];
  SqlDec VarReg[MAXVARS];
};

// todo make it more efficient by reducing bits
struct Operand {
  SqlInt16b    operandtype;  // VAR CONST or TEMP
  SqlInt16b    opno;          // operandnumber to be picked from the array
  SqlInt16b    datatype;        // DECIMAL DATEADT or BOOL
    
  SqlDec SQLEVAL(getData,SQLKERNEL_ID)(Reg& reg);
  void SQLEVAL(setData,SQLKERNEL_ID)(SqlDec& val, Reg& reg);
  void SQLEVAL(setOps,SQLKERNEL_ID)(hls::stream<SqlDefs::RdDdrWord>& bufStream, SqlDefs::RdTracker&  rdTracker);  
};

struct Expr_x {
  SqlInt16b operatortype; // NOT GT etc
  Operand   operandleft;
  Operand   operandright;
  Operand   result;

  void SQLEVAL(setOps,SQLKERNEL_ID)(hls::stream<SqlDefs::RdDdrWord>& bufStream, SqlDefs::RdTracker&  rdTracker);
  SqlDec SQLEVAL(exec,SQLKERNEL_ID)(Reg& reg);
};

const int MAXSCANS = 20;

struct Exprs {
  SqlInt16b NoOfScanExprs;
  Expr_x ScanExpr[MAXSCANS];
};

static Reg reg;
static Exprs exprs;


//const int MAXCONSTS = 9;
//const int MAXVARS = 9;
//const int MAXTEMPS = 9;
//const int MAXSCANS = 20;
const int MAXAGGRS = 20;
// 0:end_of_cols, 1:Long, 2:Int
const int MAX_COL_TYPES = 2;

const unsigned char HDR_CONFPAGE = 0xFFu;
const unsigned char HDR_SCAN = 0x00u;
const unsigned char HDR_AGGR = 0x01u;

struct HostReg {
  unsigned char ColTypeReg[MAXVARS];  // cast filtOut to this type
  short ConstReg[MAXCONSTS];
  short TempReg[MAXTEMPS];
  short VarReg[MAXVARS];
};

struct HostOperand {
  short operandtype;  // VAR CONST or TEMP
  short opno;          // operandnumber to be picked from the array
  short datatype;
};

struct HostExpr {
  short operatortype; // NOT GT etc
  HostOperand   operandleft;
  HostOperand   operandright;
  HostOperand   result;
};

struct HostExprs {
  short NoOfScanExprs;
  HostExpr ScanExpr[MAXSCANS];
  short NoOfAggrExprs;
  HostExpr AggrExpr[MAXAGGRS];
};


class ConfBufStream {
public:
  ConfBufStream (unsigned char* p_buf) : mp_buf (p_buf) {}
  ~ConfBufStream () {}

  unsigned char read () {
    return *mp_buf++;
  }
private:
  unsigned char* mp_buf;
};

template<typename StreamT>
class HostConfParser {
private:
  short m_off;

public:
  HostConfParser () : m_off (0) {}

  ~HostConfParser () {}

  void skipToEnd (StreamT& stream) {
    for ( ; m_off < CONF_BYTES; ++m_off) {
      stream. read ();
    }
  }

  unsigned char getByte (StreamT& stream) {
    ++ m_off;
    return stream. read ();
  }

  bool checkHeader (StreamT& stream) {
    m_off = 0;
    unsigned char hdr = getByte (stream);
    return hdr == 0xFFu;
  }

  // big endian: 0xAABB => stream[t] = 0xAA, stream[t+1] = 0xBB
  // ltl endian: 0xAABB => stream[t] = 0xBB, stream[t+1] = 0xAA
  // big endian is preferred because >> is arithmetic shift
  short getShort (StreamT& stream) {
    short ret = 0;
    for (unsigned char gsInd=0; gsInd<2; ++gsInd) {
      unsigned char ch = getByte (stream);
      ret = ret << 8 | ch;
    }
    return ret;
  }

  long getLong (StreamT& stream) {
    long ret=0;
    for (unsigned char glInd=0; glInd<8; ++glInd) {
      unsigned char ch = getByte (stream);
      ret = ret << 8 | ch;
    }
    return ret;
  }

  unsigned char getFiltCols (StreamT& stream, 
                             unsigned char ColTypeReg[MAXVARS]) {
    unsigned char nFiltCols = getByte (stream);
    if (nFiltCols > MAXVARS) {
      std::cout << "ERROR: too many filt-cols" << (short)nFiltCols << std::endl;
    }
    for (unsigned char fIdx = 0; fIdx < nFiltCols; ++fIdx) {
      unsigned char colType = getByte (stream);
      if (colType > MAX_COL_TYPES) {
        std::cout << "ERROR: invalid col-type " << (short)colType 
                  << " at " << (short)fIdx << std::endl;
        colType = 1;
      }
      ColTypeReg [fIdx] = colType; 
    }
    return nFiltCols;
  }

  void getOperand (StreamT& stream, HostOperand& operand) {
    operand. operandtype = getShort (stream);
    operand. opno = getShort (stream);
    operand. datatype = getShort (stream);
  }

  void getExpr (StreamT& stream, HostExpr& expr) {
    expr. operatortype = getShort (stream);
    getOperand (stream, expr.operandleft);
    getOperand (stream, expr.operandright);
    getOperand (stream, expr.result);
  }

  unsigned char getExprs (StreamT& stream, HostExpr exprs[MAXSCANS]) {
    unsigned char nExprs = getByte (stream);
    if (nExprs > MAXSCANS) {
      std::cout << "ERROR: too many exprs " << nExprs << std::endl;
    }
    for (unsigned char exprIdx=0; exprIdx<nExprs; ++exprIdx) {
      getExpr (stream, exprs [exprIdx]);
    }
    return nExprs;
  }

  unsigned char getConsts (StreamT& stream, HostReg& reg) {
    unsigned char nConsts = getByte (stream);
    if (nConsts > MAXCONSTS) {
      std::cout << "ERROR: too many consts " << nConsts << std::endl;
    }
    for (unsigned char constIdx=0; constIdx<nConsts; ++constIdx) {
      reg. ConstReg [constIdx] = getLong (stream);
    }
    return nConsts;
  }

};


typedef ap_int<70>   Decimal;
typedef ap_int<32>   DateAdt;
typedef ap_int<1>    Bool;
typedef ap_int<32>   Int32;

enum  TpchRunMode {HW, CPU_RUN, HLS_RUN};
int processQuery6(XdbConn **xdbConns, int kernels, TpchRunMode tpchRunMode, char *varstream, int ntuples);
// C model to process the data
void processCscan(char *setupbuf, char *varstream, int ntuples, char *result, int pcielanes, int deviceslrs,int query, bool verifyCompute);
 
void process_csv(string csvfile, string varfile);
bool process_var(char **varstream, string varfile);
int process_const(char *constream, bool keep_decimal);
int process_qualifier(char *qualstream, int query);
int process_var_stream_type(char *varstream_type, int query);
int process_aggregation(char *aggstream, int query);
int process_qualifier_scan(char *qualstream, int query);
int process_qualifier_scan_aggr(char *qualstream, int query);
unsigned char* confWriteLong (unsigned char* p_buf, long data);
unsigned char* confWriteShort (unsigned char* p_buf, short data);
void confDump (unsigned char* p_buf);
#endif
