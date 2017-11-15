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
