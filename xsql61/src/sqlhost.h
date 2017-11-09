/**********
Copyright (c) 2017, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

/*
 * SQL Host compilation requires Postgre or MySQL compilation. This  code is provided
 * as is. Main code can be modified to use sqlhost.h and sqlhost.cpp files. 
 * This was supported earlier, however it uses API based connectivity which is much
 * slower than native postgresql connectivity. This has been removed form the main
 * code and support is now provided for Postgres with its native storage formats.
 */
  
#ifndef XSQL61_SQLHOST_H
#define XSQL61_SQLHOST_H

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
