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
#include "main.h"
#include "sqlq1defs.h"
#include "sqlq6defs.h"

/// Sqlpageloop XDB offload code
void readSqlpageloopBufCopy(XdbConn *xdbConn, int queueNo, cl_event event, void *userData) {
  printf("+");fflush(stdout);  
  if (xdbConn == nullptr) {
    printf("Error: No xdbconn in read %d\n", queueNo);
    return;
  }  
  char *readBuf = xdbConn->getReadBuf(queueNo);
  if (readBuf == nullptr) {
    printf("Error: Read %d failed\n", queueNo);
    return;
  }
  char *writeBuf = xdbConn->getWriteBuf(queueNo);
  if (writeBuf == nullptr) {
    printf("Error: Write buf %d not there for comparison\n", queueNo);
    return;
  }
  if (userData != nullptr) {
    int *rowsProcessed = (int *) userData;
    (*rowsProcessed) += comparePages(readBuf,writeBuf,SqlQ6Defs::WriteBlockSize);
  }
  if (event != NULL)  { // in case of event syncs
    cl_int err = clSetUserEventStatus(event, CL_COMPLETE);
    xdbConn->getOclAccel().printError(err,"Read event generation failed");
  }
}

void writeSqlpageloopBufCopy(XdbConn *xdbConn, int queueNo, cl_event event, void *userData) {
  if (xdbConn == nullptr) {
    printf("Error: No xdbconn in Write %d\n", queueNo);
    return;
  } 
  char *writeBuf = xdbConn->getWriteBuf(queueNo);
  if (writeBuf == nullptr) {
    printf("Error: Write buf called on %d, but write buf pointer is null.\n", queueNo);
    return;
  }
  // nothing else to to do
  if (event != NULL)  { // in case of event syncs  
    cl_int err = clSetUserEventStatus(event, CL_COMPLETE);
    xdbConn->getOclAccel().printError(err,"Write event generation failed");
  }
}

int processSqlPageLoopback(XdbConn **xdbConns, int kernels, const char *tableName, SqlTbl& sqlHost) {

  sqlHost.rowBuf.clear();
  sqlHost.pageFields = 0xFFFF;
  sqlHost.maxPUBlockSize = SqlQ6Defs::WriteBlockSize;
  sqlHost.maxRowsInPUBlock =  SqlQ6Defs::WriteBlockSize; // no limit on max rows
  
  bool remainingPages = true;
  int rowsProcessed =0;
  int noOfPages=0;
  int curKernel = 0;  
  for (int i =0; i< kernels; i++) {    
    xdbConns[i]->setWriteBufCopyFuncPtr(writeSqlpageloopBufCopy);
    xdbConns[i]->setReadBufCopyFuncPtr(readSqlpageloopBufCopy);
  }
  XdbTimer timer;
  printf("INFO: Processing data blocks on FPGA device ");  
  timer.start();
  while (remainingPages) {
    char *inBuf = xdbConns[curKernel]->getWriteBuf();
    remainingPages = sqlHost.fetchNextPage(inBuf);
    //checkPage(tbBuffers.inbuf,SqlDefs::WriteBlockSize,false);
    xdbConns[curKernel]->enqueueCompute(&rowsProcessed);
    curKernel++;
    curKernel = curKernel%kernels; // wrap around when max kernels reached	      
    noOfPages++;
    printf(".");fflush(stdout);    
  }
  for (int i =0; i< kernels; i++)
    xdbConns[i]->finishCommands();
  timer.end();
  printf(" Done\n");fflush(stdout);  
  double secTime = timer.duration();
  double mbDataXfer = double(noOfPages)*double(SqlQ6Defs::WriteBlockSize)/double(1024*1024);
  printf("Total time: %.4lf sec. Total data: %.2lf MB. Efffective Rate: %.3f MB/s\n", secTime, mbDataXfer, mbDataXfer/secTime); 
  printf("Info: Loopback table %s processed %d rows %d pages\n",tableName,rowsProcessed, noOfPages);
  return EXIT_SUCCESS;
}

void printUsage(const char *msg=nullptr) {
  if (msg) printf("%s \n",msg);
  /*printf ("program [-hw|-cpu|-hls] [-kernels noofkernels] [-kernel kernel_name] [-xclbin xcbin_name] -query [query_no|table_name] -db [mysql|pgsql|tblfile] -sqlhost [sqlhostname] -sqlport [sqlportid] -schema [schemaname] -queue [memmigrate|enqueue] -profile [no|yes] -userdata [yes|no] -dsa [dsa_device_name] -qdepth queuedepth -ntuples ntuples\n");
  printf("Example usages: sqltpch -hls -query lineitem -db mysql -schema tpch -sqlhost xsjkgarlap50 -sqlportid 3306 \n        sqltpch -cpu -query 6 -db pgsql -schema sf40g -sqlhost xsjkgarlap50 -sqlport 5432\n     sqltpch -hw -kernels 1 -kernel sqlq6tpch -xclbin sqlq6tpch_hw.xclbin -query 6 -db pgsql -schema sf40g -sqlhost xsjkgarlap50 -sqlport 5432\n sqltpch -hw -query 6 -db lineitem.tbl -kernel sqlq6tpch -xclbin sqlq6tpch_KU115_hw.xclbin\n");  */
  printf ("sqltpch [-kernels noofkernels] [-kernel kernel_name] [-xclbin xcbin_name] -query [query_no|loopback] -queue [memmigrate|enqueue] -profile [no|yes] -userdata [yes|no] -dsa [dsa_device_name] -qdepth queuedepth -ntuples ntuples -delim [delimiter_char]\n");
  printf("Example usage: sqltpch -query 6 -db lineitem.tbl -kernel sqlq6tpch -xclbin sqlq6tpch_KU115_hw.xclbin\n");  
}

class ArgParser{
public:
  ArgParser (int &argc, char **argv){
    for (int i=1; i < argc; ++i)
      mTokens.push_back(std::string(argv[i]));
  }
  bool getCmdOption(const std::string option, std::string& value) const{
    std::vector<std::string>::const_iterator itr;
    itr =  std::find(this->mTokens.begin(), this->mTokens.end(), option);
    if (itr != this->mTokens.end() && ++itr != this->mTokens.end()){
      value = *itr;
      return true;
    }
    return false;
  }
private:
  std::vector <std::string> mTokens;
};

/*
 * top level main call
 */
int main(int argc, char** argv) {

  char targetDeviceName[1001]  = "xilinx:aws-vu9p-f1:4ddr-xpr-2pr:4.0"; // default dsa override with -dsa option
  char kernelName[1001] = "";
  char xclbinName[1001] = "";
  int queueDepth = 256; // default queue depth

  ArgParser input(argc, argv);
  std::string optValue;
  if (input.getCmdOption("-qdepth",optValue)) {
    queueDepth = atoi(optValue.c_str());
    if (queueDepth <= 0) {
      printUsage("Incorrect qdepth arg");
      return EXIT_SUCCESS;
    }    
  }

  char sqlDb[101] = "lineitem.tbl";
  if (input.getCmdOption("-db",optValue)) {
    strcpy(sqlDb,optValue.c_str());
  }
  
  /* Removed for now, use native postgres connection. 
  if (input.getCmdOption("-db",optValue)) {
    if (optValue.compare("mysql") == 0)
      strcpy(sqlDb,"mysql");
    else if (optValue.compare("pgsql") == 0)
      strcpy(sqlDb,"pgsql");
    else 
      strcpy(sqlDb,optValue.c_str());
  }

  std::string sqlHostName;
  if (!input.getCmdOption("-sqlhost",sqlHostName)) {
    if (strcmp(sqlDb,"pgsql") == 0 || strcmp(sqlDb,"mysql") == 0 ) {
      printUsage("must provide -sqlhost sqlhostname for the sqldb");
      return EXIT_SUCCESS;
    }
  }

  int sqlPortId = 5432;  
  if (input.getCmdOption("-sqlport",optValue)) {
    sqlPortId = atoi(optValue.c_str());
  } 
  
  std::string schemaName;
  if (!input.getCmdOption("-schema",schemaName)) {
    if (strcmp(sqlDb,"pgsql") == 0 || strcmp(sqlDb,"mysql") == 0 ) {
      printUsage("must provide -schema schemaName for the sqldb");
      return EXIT_SUCCESS;
    }
  }
  */
  int ntuples = 6001215;
  if (input.getCmdOption("-ntuples",optValue))
    ntuples = atoi(optValue.c_str());

  int kernels = 1;
  if (input.getCmdOption("-kernels",optValue)) {
    kernels = atoi(optValue.c_str());
  }
  if (kernels <= 0) {
    printUsage("No of kernels must be positive integer");
    return EXIT_SUCCESS;
  }
  if (input.getCmdOption("-kernel",optValue)) {
    strcpy(kernelName,optValue.c_str());
  } else {
    printUsage("must provide -kernel kernelName");
    return EXIT_SUCCESS;
  }
  if (input.getCmdOption("-xclbin",optValue)) {
    strcpy(xclbinName,optValue.c_str());
  } else {
    printUsage("must provide -xclbin xclbinName");
    return EXIT_SUCCESS;
  }
  if (input.getCmdOption("-dsa",optValue)) {
    strcpy(targetDeviceName,optValue.c_str());
  }

  char delim = '|';
  if (input.getCmdOption("-delim", optValue)) {
    delim = optValue[0];
  }
  // for tbl sqldb is the tablename
  char tableName[1001] = "";
  strcpy(tableName,sqlDb);    
  int queryNo = -1;
  if (input.getCmdOption("-query", optValue)) {
    if (optValue.compare("loopback") == 0){ // just do loopback on db
    } else {
      queryNo = optValue[0]-'0';
    }
  } else {
    printUsage("must provide -query [query_no|table_name]");
    return EXIT_SUCCESS;
  }

  // Only query 6 and 1 are supported for now
  if (queryNo > 0 && (queryNo != 6 && queryNo != 1)) {
    printf("Error: Currenlty in HLS and FPGA TPCH query 6 and Query 1 are supported. Query %d not implemented yet.\n",queryNo);
    return EXIT_FAILURE;
  }
  
  int writeBlockSize = SqlQ6Defs::WriteBlockSize;
  int readBlockSize = SqlQ6Defs::WriteBlockSize;
  if (queryNo == 1) {
    writeBlockSize = SqlQ1Defs::WriteBlockSize;
    readBlockSize = SqlQ1Defs::ReadBlockSize;
  } else if (queryNo == 6) {
    writeBlockSize = SqlQ6Defs::WriteBlockSize;
    readBlockSize = SqlQ6Defs::ReadBlockSize;
  }

  
  XdbConn **xdbConns= new XdbConn*[kernels];
  for (int i =0; i< kernels; i++)
    xdbConns[i] = XdbConn::create(queueDepth,writeBlockSize,readBlockSize);
  
  if (input.getCmdOption("-queue",optValue)) {
    if (optValue.compare("enqueue") == 0)
      for (int i =0; i< kernels; i++)
	xdbConns[i]->setOpFlag(XdbConn::UseEnqueueBuffers);      
  }
  
  if (input.getCmdOption("-profile",optValue)) {
    if (optValue.compare("yes") == 0)
      for (int i =0; i< kernels; i++)
	xdbConns[i]->setOpFlag(XdbConn::EnableProfiling);
  }

  if (input.getCmdOption("-userdata",optValue)) {
    if (optValue.compare("no") == 0)
      for (int i =0; i< kernels; i++)
	xdbConns[i]->setOpFlag(XdbConn::DisableUserDataCopy);
  }

  //printf("INFO: opmode=%d db=%s sqlhost=%s sqlport=%d schema=%s NoOfKernels=%d Kernel=%s Xclbin=%s queueDepth=%d WriteBlockSize=%d ReadBlockSize=%d  EnqueueBuffers=%d profile=%d disable_userdata=%d dsa=%s ntuples=%d\n", tpchRunMode, sqlDb, sqlHostName.c_str(), sqlPortId, schemaName.c_str(), kernels, kernelName,xclbinName, queueDepth, writeBlockSize, readBlockSize, xdbConns[0]->getOpFlag(XdbConn::UseEnqueueBuffers),xdbConns[0]->getOpFlag(XdbConn::EnableProfiling), xdbConns[0]->getOpFlag(XdbConn::DisableUserDataCopy),targetDeviceName,ntuples);
  printf("INFO: db=%s NoOfKernels=%d Kernel=%s Xclbin=%s queueDepth=%d WriteBlockSize=%d ReadBlockSize=%d  EnqueueBuffers=%d profile=%d disable_userdata=%d dsa=%s ntuples=%d\n", sqlDb, kernels, kernelName,xclbinName, queueDepth, writeBlockSize, readBlockSize, xdbConns[0]->getOpFlag(XdbConn::UseEnqueueBuffers),xdbConns[0]->getOpFlag(XdbConn::EnableProfiling), xdbConns[0]->getOpFlag(XdbConn::DisableUserDataCopy),targetDeviceName,ntuples);  

  int status=EXIT_SUCCESS;

  /*   removed database connectivity 
  // connect to sql host
  SQLHost sqlHost;
  sqlHost.setSqlDb(sqlDb);
  sqlHost.schemaName = schemaName;
  sqlHost.sqlhostname = sqlHostName;
  sqlHost.sqlportid = sqlPortId;
  sqlHost.ntuples = ntuples;
  
  if (sqlHost.connect() != EXIT_SUCCESS) {
    return EXIT_FAILURE;
    }*/

  // Read the tbl file into memory
  SqlTbl sqlHost;
  printf("INFO: Loading tbl file %s into memory\n",sqlDb);
  sqlHost.setSqlDb(sqlDb,delim);
  sqlHost.ntuples = ntuples;
  if (sqlHost.readTbl() != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }

  printf("INFO: Connecting the platform and loading the kernels\n");
  // First init all the xdbconns
  for (int i =0; i< kernels; i++) {
    XdbConn::Status oclStatus = xdbConns[i]->init(); // set callback funcs later
    if (oclStatus == XdbConn::Error) {
      printf("Error: Failed to get initialize xdbconn\n");
      return EXIT_FAILURE;
    }
  }

  // Now load the kernels
  for (int i =0; i< kernels; i++) {    
    std::string knlName(kernelName);
    knlName += std::to_string(i);
    XdbConn::Status oclStatus = xdbConns[i]->connectPlatform(targetDeviceName,knlName.c_str(),xclbinName);
    if (oclStatus == XdbConn::Error) {
      printf("Error: Failed to get FPGA compute platform\n");
      return EXIT_FAILURE;
    }
    oclStatus = xdbConns[i]->createAllBufs();
    if (oclStatus == XdbConn::Error) {
      printf("Error: Failed to get CL Mem Buffers\n");
      return EXIT_FAILURE;
    }
  }

  if (queryNo == 6) {
    status = processQuery6(xdbConns, kernels, sqlHost);
  } else if (queryNo == 1) {
    status = processQuery1(xdbConns, kernels, sqlHost);    
  } else { // loopback table table_name
    status =  processSqlPageLoopback(xdbConns, kernels, tableName, sqlHost);
  }

  for (int i=0; i<kernels; i++) {
    if (xdbConns[i]->getOpFlag(XdbConn::EnableProfiling) )
      xdbConns[i]->getProfileData();
    XdbConn::remove(xdbConns[i]);
  }
  
  printf("All Complete!!\n");
  
  return status;
}
