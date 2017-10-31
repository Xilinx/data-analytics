/*******************************************************************************
Vendor: Xilinx
Revision History:
Oct 25, 2016: initial release
*******************************************************************************
Copyright (C) 2016 XILINX, Inc.

This file contains confidential and proprietary information of Xilinx, Inc. and
is protected under U.S. and international copyright and other intellectual
property laws.

DISCLAIMER
This disclaimer is not a license and does not grant any rights to the materials
distributed herewith. Except as otherwise provided in a valid license issued to
you by Xilinx, and to the maximum extent permitted by applicable law:
(1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, AND XILINX
HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY,
INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, OR
FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable (whether
in contract or tort, including negligence, or under any other theory of
liability) for any loss or damage of any kind or nature related to, arising under
or in connection with these materials, including for any direct, or any indirect,
special, incidental, or consequential loss or damage (including loss of data,
profits, goodwill, or any type of loss or damage suffered as a result of any
action brought by a third party) even if such damage or loss was reasonably
foreseeable or Xilinx had been advised of the possibility of the same.

CRITICAL APPLICATIONS
Xilinx products are not designed or intended to be fail-safe, or for use in any
application requiring fail-safe performance, such as life-support or safety
devices or systems, Class III medical devices, nuclear facilities, applications
related to the deployment of airbags, or any other applications that could lead
to death, personal injury, or severe property or environmental damage
(individually and collectively, "Critical Applications"). Customer assumes the
sole risk and liability of any use of Xilinx products in Critical Applications,
subject only to applicable laws and regulations governing limitations on product
liability.

THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT
ALL TIMES.

*******************************************************************************/

/*
 * SQL Host compilation requires Postgre or MySQL compilation. This  code is provided
 * as is. Main code can be modified to use sqlhost.h and sqlhost.cpp files. 
 * This was supported earlier, however it uses API based connectivity which is much
 * slower than native postgresql connectivity. This has been removed form the main
 * code and support is now provided for Postgres with its native storage formats.
 */
  
#ifndef TPCH_SQLHOST_H
#define TPCH_SQLHOST_H

#include <mysql.h>
#include "libpq-fe.h"
#include "sqldefs.h"
#include <vector>
#include <string>

struct SQLHost {
  enum SqlDbType {PGSQL=0,MySQL=1,CSV};
  SqlDbType sqldb;
  void setSqlDb(const char *db);
  void setSqlDb(SqlDbType sdb) { sqldb = sdb; }
  
  int connect();
  int connectMySQL();
  int connectPGSQL();
  int connectCSV();
  
  void close();
  
  int query(const char *queryText, bool printResult=true);

  int printQueryResult(bool freeResult=true);
  int printQueryResultMySQL(bool freeResult);
  int printQueryResultPGSQL(bool freeResult);


  // For now support up to 32 fields, expand later as needed
  bool fetchNextPage(char *bufRam);
  bool fetchNextPageMySQL(char *bufRam);
  bool fetchNextPagePGSQL(char *bufRam);
  bool fetchNextPageCSV(char *bufRam);
  bool copyRowToBuf(char *bufRam, unsigned int& curBuf);

  MYSQL *mysqlCon;
  MYSQL_RES *mysqlRes;
  
  PGconn   *pgsqlCon;
  PGresult *pgsqlRes;
  int pgsqlResRow;

  // CSV related options
  std::string csvFileName;
  char *csvFile;
  int ntuples;
  int processedTuples;
  char *curVarCh;
  
  static const char* tpchQueryText[22];
  int queryTpch(int queryNo);
  
  std::string sqlhostname;
  std::string schemaName;
  int sqlportid;
  std::vector<char> rowBuf;
  unsigned int pageFields;

  unsigned int maxPUBlockSize;
  unsigned int maxRowsInPUBlock;
  
};

#endif
