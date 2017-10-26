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
#include "oclaccel.h"
#include "xdbconn.h"
#include "xdbutils.h"
#include <algorithm>
#include <thread>


void readBufCopy(XdbConn *xdbConn, int queueNo, cl_event event, void *userData) {
  if (xdbConn == nullptr) {
    printf("Error: No xdbconn in Read %d\n", queueNo);
  } else {
    char *readBuf = xdbConn->getReadBuf(queueNo);
    if (readBuf == nullptr) {
      printf("Error: Read %d failed\n", queueNo);
    } else {
      if (readBuf[queueNo] != char(0xFF)) { // same as -1
	printf("Error: Read %d %x failed\n", queueNo, readBuf[queueNo]);
      }
    }
  }
  if (event != NULL)  { // in case of event syncs
    cl_int err = clSetUserEventStatus(event, CL_COMPLETE);
    // print error of err is not CL_SUCCESS
    if (xdbConn) xdbConn->getOclAccel().printError(err,"Read event generation failed");
  }
  if (userData) {
    ((int*)userData)[queueNo]++;
  } 
}

void writeBufCopy(XdbConn *xdbConn, int queueNo, cl_event event, void *userData) {
  if (xdbConn == nullptr) {
    printf("Error: No xdbconn in Write Queue %d\n", queueNo);
  } else {
    char *writeBuf = xdbConn->getWriteBuf(queueNo);
    if (writeBuf == nullptr) {
      printf("Error: Write buf called on Queue %d, but write buf pointer is null.\n", queueNo);
    } else {
      int wrAddr = queueNo;
      writeBuf[wrAddr] = 0xFF; // same as -1 for char
    }
  }
  if (event != NULL)  { // in case of event syncs  
    cl_int err = clSetUserEventStatus(event, CL_COMPLETE);
    // print error of err is not CL_SUCCESS
    if (xdbConn) xdbConn->getOclAccel().printError(err,"Write event generation failed");
  }
}


void readBlkPuBuf(XdbConn *xdbConn, int queueNo, cl_event event, void *userData) {
  if (xdbConn == nullptr) {
    printf("Error: No xdbconn in Read %d\n", queueNo);
  } else {
    char *readBuf = xdbConn->getReadBuf(queueNo);
    if (readBuf == nullptr) {
      printf("Error: Read %d failed\n", queueNo);
    } else {
      // Assume only add ALU for now - tofo change it to handle other ops
      const int totalOps = readBuf[2]; 
      for (int i=0; i<totalOps; i++) {
	if (readBuf[2+8*(i+1)] != readBuf[3+8*(i+1)])
	  printf("Error: Read %d Op %d=Reqd: %d Actual: %d failed\n", readBuf[8*(i+1)],readBuf[1+8*(i+1)],readBuf[2+8*(i+1)],readBuf[3+8*(i+1)]);
      }
    }
  }
  if (event != NULL)  { // in case of event syncs
    cl_int err = clSetUserEventStatus(event, CL_COMPLETE);
    // print error of err is not CL_SUCCESS    
    if (xdbConn) xdbConn->getOclAccel().printError(err,"Read event generation failed");
  }
  if (userData) {
    ((int*)userData)[queueNo]++;
  } 
}

// to be used in writeblkpubuf, use as a static for now
static char opcode = 0x01;

void writeBlkPuBuf(XdbConn *xdbConn, int queueNo, cl_event event, void *userData) {
  if (xdbConn == nullptr) {
    printf("Error: No xdbconn in Write %d\n", queueNo);
  } else {
    char *writeBuf = xdbConn->getWriteBuf(queueNo);
    if (writeBuf == nullptr) {
      printf("Error: Write buf called on %d, but write buf pointer is null.\n", queueNo);
    } else {
      const char totalOps = 16; // 16 operations
      writeBuf[0] = 0;
      writeBuf[1] = 0;
      writeBuf[2] = totalOps;
      writeBuf[3] = 0;
      for (int i=0; i<totalOps; i++) {
	char a = 2*i;
	char b = 2*i+1;
	writeBuf[8*(i+1)] = a;
	writeBuf[8*(i+1)+1] = b;
	if (opcode == 0x01)
	  writeBuf[8*(i+1)+2] = b+a;
	else if (opcode == 0x02)
	  writeBuf[8*(i+1)+2] = b-a;
	else if (opcode == 0x03)
	  writeBuf[8*(i+1)+2] = b*a;
	else
	  writeBuf[8*(i+1)+2] = 0;	  
	for (int j=3;j<7;j++)
	  writeBuf[8*(i+1)+j] = 0;
      }
    }
  }
  if (event != NULL)  { // in case of event syncs  
    cl_int err = clSetUserEventStatus(event, CL_COMPLETE);
    if (xdbConn) xdbConn->getOclAccel().printError(err,"Write event generation failed");
  }
}

void  enqueueThread(XdbConn *xdbConn, int blockCount, int blockSize, int queue, const char *targetDeviceName, char *kernelName, char *xclbinName) {
  XdbConn::Status oclStatus = xdbConn->init();
  if (oclStatus == XdbConn::Error) {
    printf("Error: Failed to get initialize xdbconn\n");
    return ;
  }
  
  oclStatus = xdbConn->connectPlatform(targetDeviceName,kernelName,xclbinName);
  if (oclStatus == XdbConn::Error) {
    printf("Error: Failed to get FPGA compute platform\n");
    return ;
  }
  
  oclStatus = xdbConn->createAllBufs();
  if (oclStatus == XdbConn::Error) {
    printf("Error: Failed to get CL Mem Buffers\n");
    return ;
  }

  XdbTimer timer;
  timer.start();
  for (int curDataBlock = 0; curDataBlock<blockCount; curDataBlock++) {
    xdbConn->enqueueCompute(nullptr);
  }
  xdbConn->finishCommands();
  timer.end();

  if (xdbConn->getOpFlag(XdbConn::EnableProfiling) )
      xdbConn->getProfileData();
  xdbConn->clear();
  
  double timeDuration = timer.duration();
  double mbDataXfer = double(blockCount)*double(blockSize)/double(1024*1024);
  printf("Queue Thread %d Total time: %.4lf sec. Total data: %.2lf MB. Efffective Rate: %.3f MB/s\n", queue, timeDuration, mbDataXfer, mbDataXfer/timeDuration);
  return;
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

void printUsage(const char *msg=nullptr) {
  if (msg) printf("%s \n",msg);
  printf("Block size -bsize must be matched with the Kernel definition - PU size * Pu count with Burst paramenters.\n");
  printf("Usage: \"program -kernel kernel_name -xclbin xclbin_name -queue [enqueue|memmigrate] -profile [yes|no] -userdata [yes|no] -usethread [yes|no] -eventsync [no|yes] -dsa [dsa_device_name] -bsize KernelBlockSize -qdepth queuedepth -bcount numberOfBlocks -qcount numberOfQueues\"\n"); 
}

// This design sends data and recieves back the same data. It checks the xdbconn algorithm to
// send and receive loopback data from FPGA platform
int main(int argc, char** argv) {
  
  const unsigned int KernelBlockSize=2097152;
  
  char targetDeviceName[1001]  = "xilinx:xil-accel-rd-ku115:4ddr-xpr:4.0";// "xilinx:adm-pcie-ku3:2ddr-xpr:3.3"
  char kernelName[1001];
  char xclbinName[1001];
  int blockSize = KernelBlockSize; //todo later  see how it can be handled across kernel and host program as arg
  int queueDepth = 512; // default queue depth
  int blockCount = 2048; // default block count
  int queueCount = 1;    // number of queued threads

  ArgParser input(argc, argv);
  std::string optValue;
  if (input.getCmdOption("-qdepth",optValue)) {
    queueDepth = atoi(optValue.c_str());
    if (queueDepth <= 0) {
      printUsage("Incorrect qdepth arg");
      return EXIT_SUCCESS;
    }    
  }
  if (input.getCmdOption("-bcount",optValue)) {
    blockCount = atoi(optValue.c_str());
    if (blockCount <= 0) {
      printUsage("Incorrect bcount arg");
      return EXIT_SUCCESS;
    }    
  }
  if (input.getCmdOption("-bsize",optValue)) {
    blockSize = atoi(optValue.c_str());
    if (blockSize <= 0) {
      printUsage("Incorrect bsize arg");
      return EXIT_SUCCESS;
    }    
  }

  if (input.getCmdOption("-qcount",optValue)) {
    queueCount = atoi(optValue.c_str());
    if (queueCount <= 0) {
      printUsage("Incorrect qcount arg");
      return EXIT_SUCCESS;
    }    
  }
  
  XdbConn **xdbConn = new XdbConn*[queueCount];
  for (int i =0; i< queueCount; i++) 
    xdbConn[i] = XdbConn::create(queueDepth,blockSize,blockSize);
  
  if (input.getCmdOption("-kernel",optValue)) {
    strcpy(kernelName,optValue.c_str());
  } else {
    printUsage("must provide -kernel kernelName");
    return EXIT_SUCCESS;
  }
  enum {Loopback=0,BlkPu} kernelType;
  kernelType = Loopback;
  if (optValue.compare("blkpu") == 0)
    kernelType = BlkPu;

  if (input.getCmdOption("-dsa",optValue)) {
    strcpy(targetDeviceName,optValue.c_str());
  }

  if (input.getCmdOption("-xclbin",optValue)) {
    strcpy(xclbinName,optValue.c_str());
  } else {
    printUsage("must provide -xclbin xclbinName");
    return EXIT_SUCCESS;
  }

  if (input.getCmdOption("-queue",optValue)) {
    if (optValue.compare("enqueue") == 0)
      for (int i =0; i< queueCount; i++)
	xdbConn[i]->setOpFlag(XdbConn::UseEnqueueBuffers);      
  }
  
  if (input.getCmdOption("-profile",optValue)) {
    if (optValue.compare("yes") == 0)
      for (int i =0; i< queueCount; i++)
	xdbConn[i]->setOpFlag(XdbConn::EnableProfiling);
  }

  if (input.getCmdOption("-userdata",optValue)) {
    if (optValue.compare("no") == 0)
      for (int i =0; i< queueCount; i++)
	xdbConn[i]->setOpFlag(XdbConn::DisableUserDataCopy);
  }

  bool usethread = false;
  if (input.getCmdOption("-usethread",optValue)) {
    if (optValue.compare("yes") == 0)
      usethread = true;
  }

  // opcode has the following meaning
  // Asel [1:0] 00 = A, 01 = B, 10 = C, 11=>gnd
  // Bsel [1:0] 00 = B, 01 = A, 10 = C, 11=>gnd
  // Opcode : 0000 - means it is a data block
  //          0001 = A+B
  //          0010 = B-A
  //          0011 = A*B
  if (input.getCmdOption("-opcode",optValue)) {
    opcode = char(stoi(optValue));
  }
  
  printf("Kernel=%s Xclbin=%s queueDepth=%d BlockSize=%d NumberOfBlocks=%d EnqueueBuffers=%d profile=%d disable_userdata=%d usethread=%d NumberOfQueues=%d\n",kernelName,xclbinName, queueDepth,blockSize, blockCount, xdbConn[0]->getOpFlag(XdbConn::UseEnqueueBuffers),xdbConn[0]->getOpFlag(XdbConn::EnableProfiling), xdbConn[0]->getOpFlag(XdbConn::DisableUserDataCopy), usethread,queueCount);

  // spawn all the threads
  if (usethread) {
    std::thread **tdbThreads = new std::thread* [queueCount];
    for (int i=0; i<queueCount; i++) {
      tdbThreads[i] = new std::thread(enqueueThread, xdbConn[i], blockCount, blockSize,i,targetDeviceName,kernelName,xclbinName);
    }

    // synchronize all the threads
    for (int i=0; i<queueCount; i++) {
      tdbThreads[i]->join();
    }

    for (int i=0; i<queueCount; i++) {
      delete tdbThreads[i];
    }
    delete [] tdbThreads;

  } else {  // remain the main program process, no thread to spawn

    XdbConn::Status oclStatus = xdbConn[0]->init(); 
    if (oclStatus == XdbConn::Error) {
      printf("Error: Failed to get initialize xdbconn\n");
      return EXIT_SUCCESS;
    }
  
    oclStatus = xdbConn[0]->connectPlatform(targetDeviceName,kernelName,xclbinName);
    if (oclStatus == XdbConn::Error) {
      printf("Error: Failed to get FPGA compute platform\n");
      return EXIT_SUCCESS;
    }
  
    oclStatus = xdbConn[0]->createAllBufs();
    if (oclStatus == XdbConn::Error) {
      printf("Error: Failed to get CL Mem Buffers\n");
      return EXIT_SUCCESS;
    }

    // Set the command and data registers
    if (kernelType == BlkPu) {
      int queueNo = 0;
      char *writeBuf = xdbConn[0]->getWriteBuf(queueNo);
      if (writeBuf == nullptr) {
	printf("Error: Write buf called on %d, but write buf pointer is null.\n", queueNo);
	return EXIT_SUCCESS;
      }
      writeBuf[0] = opcode;
      writeBuf[1] = 0;
      writeBuf[2] = 0;
      writeBuf[3] = 0;
      xdbConn[0]->enqueueCompute(nullptr);
      xdbConn[0]->finishCommands();
    }

    int *userData;
    userData = new int[queueDepth];
    for (int i=0; i<queueDepth; i++)
      userData[i] = 0;
    
    // Now register readback function
    if (kernelType == BlkPu) {
      for (int i=0; i<queueCount; i++) {
	xdbConn[i]->setWriteBufCopyFuncPtr(writeBlkPuBuf);
	xdbConn[i]->setReadBufCopyFuncPtr(readBlkPuBuf);
      }
    } else {
      for (int i=0; i<queueCount; i++) {
	xdbConn[i]->setWriteBufCopyFuncPtr(writeBufCopy);
	xdbConn[i]->setReadBufCopyFuncPtr(readBufCopy);
      }
    }
    
    XdbTimer timer;
    timer.start();
    for (int curDataBlock = 0; curDataBlock<blockCount; curDataBlock++) {
      xdbConn[0]->enqueueCompute(userData);
    }
    xdbConn[0]->finishCommands();    
    timer.end();

    if (xdbConn[0]->getOpFlag(XdbConn::EnableProfiling) )
      xdbConn[0]->getProfileData();
    xdbConn[0]->clear();

    int queueRounds = blockCount/queueDepth;
    int lastQueueDepth = blockCount%queueDepth;
    
    for (int i=0; i<queueDepth;i++) {
      int expQueueCount = queueRounds;
      if (i < lastQueueDepth)
	expQueueCount++;
      if (userData[i] != expQueueCount)
	printf("Error: Incorrect count %d, expected %d call on queue position %d.\n", userData[i],expQueueCount,i);
    }
    delete [] userData;
    
    double timeDuration = timer.duration();
    double mbDataXfer = double(blockCount)*double(blockSize)/double(1024*1024);
    printf("Total time: %.4lf sec. Total data: %.2lf MB. Efffective Rate: %.3f MB/s\n", timeDuration, mbDataXfer, mbDataXfer/timeDuration);
  }
  
  printf("All Complete!!\n");
  
  return EXIT_SUCCESS;
  
}
