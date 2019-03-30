/*
 * Copyright (C) 2019, Xilinx Inc - All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include <CL/opencl.h>
#include <CL/cl_ext.h>
#include <string.h>
#include "oclaccel.h"
#include "CL/cl.h"
#include <iostream>
#include <fstream>
#include "omx.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <random>
#include <cmath>

#define BITS_PER_BYTE 8
#define MAX_HBM_BANKCOUNT 32
//FIXME: Index for the BANK where other data resides (in and out buffer)
#define IN_HBM_BANKCOUNT_IDX 8

using std::array;
//Allocator template to align buffer to Page boundary to 4K of type T for better data transfer
template <typename T> T* allocate(std::size_t num) {
  void* ptr = nullptr;
  if (posix_memalign(&ptr,4096,num*sizeof(T)))
    throw std::bad_alloc();
  return reinterpret_cast<T*>(ptr);
}

// streaming parameters
//static int queueDepth = 24;            // 24MB blocks for queuing the data
static int inBlockSize = 1024*1024;    // 1MB block sizes
static int outBlockSize = 2*1024*1024; // 2MB out block sizes

enum IndirectOps { Gather=0, Group=1, IndirectGather=2, FusedGather=3, SegmentedGather=4 } ;
static const unsigned int mNoDdrA = 16;

// DDR Sizes on host for openCL
static unsigned int mDdrSize = 1U<<29; //512MB A array size 
enum {
  rows = 1U << 26,    // 64M
  arrayu = 1U << 26,  // 64M
};

static const int totalBlocks = sizeof(unsigned int)*rows/inBlockSize;
#define TotalBlocks sizeof(unsigned int)*rows/inBlockSize

struct Row {
  unsigned int measure;
  unsigned int group;
};

static const int DBSize = arrayu * sizeof(struct Row); // database size

static struct Row *A;
static unsigned int *in;
static unsigned int *in_mapped;

static struct Row *out;
static struct Row *xilout;

static unsigned int seed;

static unsigned long diff(const struct timeval * newTime, const struct timeval * oldTime) {
  return (newTime->tv_sec - oldTime->tv_sec)*1000000 + (newTime->tv_usec - oldTime->tv_usec);
}

// Initialize all the data for data operations
void initData() {
  // Allocata all memory here, on stack causes issues
  A = new Row [arrayu];
  
  in = allocate<unsigned int>(rows);
  in_mapped = allocate<unsigned int>(rows);

  out = new Row [rows];
  xilout = allocate<Row>(rows);

  // Random fill indirection array A
  unsigned int i;
  for (i = 0; i < arrayu; i++) {
    A[i].measure = rand_r(&seed) % arrayu;
    A[i].group = rand_r(&seed);
  }

 
  // Random fill input

  for (i = 0; i < rows; i++){
    in[i] = rand_r(&seed) % (arrayu);
  }



}

/***
*	We define here
***/

// Returns true if string str contain all integers, otherwise false
bool is_digits(const std::string &str)
{
	return std::all_of(str.begin(), str.end(), ::isdigit);
}

void in_mapp (int sw_emu,int hbmNum, int p_ch) {
  if(sw_emu ==1) {
    for (int i = 0; i < rows; i++)
      in_mapped[i]=in[i];
  } else {
    // dbsize
    int dbsizebytes = rows*sizeof(struct Row);
    int totalDbIndx= rows;
    int perInterDbIndx=totalDbIndx/hbmNum;
    int hbmSize = 0x20000000;
    if (p_ch){
	hbmSize = hbmSize/2;
    }
    int hbmIndx= hbmSize/8;
    //mapping the address as actual address which Db store in HBM
    //DB will interleaved to 16 parts and sit across HBMs.
    for (int i = 0; i < rows; i++) {
      unsigned int flag=floor(in[i]/perInterDbIndx);

      in_mapped[i] = in[i] + flag*(hbmIndx-perInterDbIndx);
      if (flag >= hbmNum)
	printf("Error: lookup beyond max rows\n");
    }
  }
}

int cIndirect() {

  initData();
  struct timeval t0, t1;

  gettimeofday(&t0, 0);

  // Gather rows
  for (int i = 0; i < rows; i++) {
    out[i] = A[in[i]];
  }

  gettimeofday(&t1, 0);

  return 0;
}
// Wrap any OpenCL API calls that return error code(cl_int) with the below macros
// to quickly check for an error
#define OCL_CHECK(call)							\
  do {									\
    cl_int err = call;							\
    if (err != CL_SUCCESS) {						\
      printf("Error calling " #call ", error code is: %d\n", err);	\
      exit(EXIT_FAILURE);						\
    }									\
  } while (0);

// An event callback function that prints the operations performed by the OpenCL
// runtime.
void event_cb(cl_event event, cl_int cmd_status, void *data) {
  cl_command_type command;
  clGetEventInfo(event, CL_EVENT_COMMAND_TYPE, sizeof(cl_command_type),
                 &command, nullptr);
  cl_int status;
  clGetEventInfo(event, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int),
                 &status, nullptr);
  const char *command_str;
  const char *status_str;
  switch (command) {
  case CL_COMMAND_READ_BUFFER:
    command_str = "buffer read";
    break;
  case CL_COMMAND_WRITE_BUFFER:
    command_str = "buffer write";
    break;
  case CL_COMMAND_NDRANGE_KERNEL:
    command_str = "kernel";
    break;
  case CL_COMMAND_MAP_BUFFER:
    command_str = "kernel";
    break;
  case CL_COMMAND_COPY_BUFFER:
    command_str = "kernel";
    break;
  case CL_COMMAND_MIGRATE_MEM_OBJECTS:
    command_str = "buffer migrate";
    break;
  default:
    command_str = "unknown";
  }
  switch (status) {
  case CL_QUEUED:
    status_str = "Queued";
    break;
  case CL_SUBMITTED:
    status_str = "Submitted";
    break;
  case CL_RUNNING:
    status_str = "Executing";
    break;
  case CL_COMPLETE:
    status_str = "Completed";
    break;
  }
  printf("[%s]: %s %s\n", reinterpret_cast<char *>(data), status_str,
         command_str);
  fflush(stdout);
}

int isEqual(unsigned long long x1, unsigned long long x2) {
  if (x1 != x2) {
    printf("Error: Mismatch between  cpu=%llu & fpga=%llu \n", x1, x2);
    return 1;
  }
  return 0;
}

int isAEqual(int i, Row& r1, Row & r2) {
  if ((r1.measure != r2.measure) || (r1.group != r2.group)) {
    printf("Error: Row %d Mismatch between  cpu=%d %d & fpga=%u %u \n", i, r1.measure, r1.group, r2.measure, r2.group);
    for (int j=0; j<rows; j++) {
      Row tr = A[in[j]];
      if ((tr.measure == r2.measure) && (tr.group == r2.group)) {
	printf("Error: Row %d in A for in %d should have been %d for in %d\n", j, in[j], i, in[i]); break;
      }
    }
    return 1;
  }
  return 0;
}

void testMsg(const char *test, int& errCnt) {
  if (errCnt) {
    printf("Error: %s test failed.\n", test);
    errCnt = 0;
  } else {
    printf("*** %s passed\n", test);
  }
}

void checkOp(const char *test, Row* r1, Row* r2, bool gather=false) {
  int errCnt = 0;
  for (int i=0; i<rows; i++) {
    if (gather)
      errCnt += isAEqual(i,A[in[i]],r2[i]);
    else
      errCnt += isAEqual(i, r1[i], r2[i]);
    if (errCnt >= 10) break;
  }
  testMsg(test,errCnt);  
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

cl_int createBuf(OclAccel& oclAccel, int blockSize, cl_mem& clbuf, char*& mapBuf, cl_mem_ext_ptr_t& bankmem) {
  cl_int clErr;
  clbuf = clCreateBuffer(oclAccel.context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, blockSize, &bankmem, &clErr);
  if (clErr != CL_SUCCESS || !clbuf) {
    printf("Error: Failed to create buffer with msg %s\n",oclAccel.getErrorString(clErr));
    return clErr;
  }
  // Pin to a char* pointer
  mapBuf = (char *)clEnqueueMapBuffer(oclAccel.commands, clbuf, CL_TRUE, CL_MAP_READ|CL_MAP_WRITE, 0, blockSize, 0, NULL, NULL, &clErr);
  if (clErr != CL_SUCCESS) {
    printf("Error: Enqueue map buffer failed for buf with msg %s\n",oclAccel.getErrorString(clErr));
    return clErr;
  }
  return clErr;
}

struct ReadCallBackData {
  int iterIdx;
  char *hostOutBuf;
  char *devOutBuf;
  cl_event event;
};

ReadCallBackData *readCallBackData=new ReadCallBackData[totalBlocks]; // Keep it as deep as execFpgaOp

void copyReadBuf(cl_event event, cl_int eventCommandExecStatus, void *rbRcbData) {
  if (eventCommandExecStatus != CL_COMPLETE) {
    printf("Error: copyReadBuf called when event is not completed and status is not CL_COMPLETE");
  }
  ReadCallBackData *rcbData = (ReadCallBackData *)rbRcbData;
  memcpy(rcbData->hostOutBuf, rcbData->devOutBuf, outBlockSize);
  Row *r = (Row *)rcbData->hostOutBuf;


   printf("+");
   fflush(stdout);  
  clSetUserEventStatus(rcbData->event, CL_COMPLETE);
}

void clearData() {
  // new array 
  delete [] A ;
  delete [] out;
  delete [] readCallBackData  ;
  // allocated using posix_memalign
  free(in);
  free(xilout);
}

inline void set_callback(cl_event event, const char *queue_name) {
  OCL_CHECK(clSetEventCallback(event, CL_COMPLETE, event_cb, (void *)queue_name));
}

void execFpgaOp(int ops,char* inBuf, char* outBuf, OclAccel& oclAccel, cl_mem mInClMem[2], char* mInBuf[2], cl_mem  mOutClMem[], char* mOutBuf[], cl_mem *mDdrClMem,  struct timeval& tStart, struct timeval& tEnd) {

  gettimeofday(&tStart,0);  

  // This pair of events will be used to track when a kernel is finished with
  // the input buffers. Once the kernel is finished processing the data, a new
  // set of elements will be written into the buffer.
  cl_event kernelEvents[totalBlocks];
  cl_event readEvents[totalBlocks];
  cl_event writeEvents[totalBlocks];

  int noOfOutBlockingEvents = 0;
  cl_event* outBlockingEventPtr = NULL;
  cl_event outBlockingEvents[3];

  for (size_t iterIdx = 0; iterIdx <totalBlocks ; iterIdx++) {
    // ping is the current buffer, pong is the other one
    int pingIdx = iterIdx % 2;
    cl_int clErr = 0;
    
    memcpy(mInBuf[pingIdx], inBuf+iterIdx*inBlockSize, inBlockSize);
    if (iterIdx == 0 || iterIdx == 1) {
      OCL_CHECK(clEnqueueMigrateMemObjects(oclAccel.commands, 1, &(mInClMem[pingIdx]),0, 0, NULL, &(writeEvents[iterIdx])));
    } else {
      OCL_CHECK(clEnqueueMigrateMemObjects(oclAccel.commands, 1, &(mInClMem[pingIdx]),0, 1, &(kernelEvents[iterIdx-2]), &writeEvents[iterIdx]));
      clWaitForEvents(1,&(kernelEvents[iterIdx-2]));       
    }

    // Set Kernel arguments
    ops &= PING_PONG_IN_MASK_OFF;
    ops &= PING_PONG_OUT_MASK_OFF;
    ops |= pingIdx?0x0003:0x0000;
    int out0Idx = pingIdx?0:iterIdx;
    int out1Idx = pingIdx?iterIdx:1;
    
    clErr  = clSetKernelArg(oclAccel.kernel, 0, sizeof(int), &ops);
    clErr  = clSetKernelArg(oclAccel.kernel, 1, sizeof(cl_mem), &(mInClMem[0]));
    clErr  = clSetKernelArg(oclAccel.kernel, 2, sizeof(cl_mem), &(mInClMem[1]));    
    clErr |= clSetKernelArg(oclAccel.kernel, 3, sizeof(cl_mem), &(mOutClMem[out0Idx]));
    clErr |= clSetKernelArg(oclAccel.kernel, 4, sizeof(cl_mem), &(mOutClMem[out1Idx]));    
    for (unsigned int i=0; i<mNoDdrA;i++)
      clErr |= clSetKernelArg(oclAccel.kernel, i+5, sizeof(cl_mem), &(mDdrClMem[0]));

    if (clErr != CL_SUCCESS) {
      printf("Error: Failed to set kernel arguments! %s\n", oclAccel.getErrorString(clErr));
    }

    // Do not enqueue task unless write to ping-buffer is done
    OCL_CHECK( clEnqueueTask(oclAccel.commands, oclAccel.kernel, 1, &(writeEvents[iterIdx]), &(kernelEvents[iterIdx])));

    // This operation only needs to wait for kernel call and readCallBackData[iterIdx-2]
    // wraparound readback buffer after 4 callbacks, ping-pong will block previous readbacks    
      noOfOutBlockingEvents = 1;
      outBlockingEventPtr = &(kernelEvents[iterIdx]);
      OCL_CHECK(clEnqueueMigrateMemObjects(oclAccel.commands, 1, &(mOutClMem[iterIdx]), CL_MIGRATE_MEM_OBJECT_HOST, noOfOutBlockingEvents, outBlockingEventPtr, &(readEvents[iterIdx])));

      
  }
  // Wait for all of the OpenCL operations to complete
  clFlush(oclAccel.commands);
  clFinish(oclAccel.commands);

  gettimeofday(&tEnd,0);
}


void execFpgaOp_cb(int ops,char* inBuf, char* outBuf, OclAccel& oclAccel, cl_mem mInClMem[2], char* mInBuf[2], cl_mem  mOutClMem[2], char* mOutBuf[2], cl_mem *mDdrClMem,  struct timeval& tStart, struct timeval& tEnd) {

  gettimeofday(&tStart,0);  

  // This pair of events will be used to track when a kernel is finished with
  // the input buffers. Once the kernel is finished processing the data, a new
  // set of elements will be written into the buffer.
  cl_event kernelEvents[totalBlocks];
  cl_event readEvents[totalBlocks];
  cl_event writeEvents[totalBlocks];

  int noOfKernelBlockingEvents = 0;
  cl_event* kernelBlockingEventPtr = NULL;
  cl_event kernelBlockingEvents[3];

  int noOfOutBlockingEvents = 0;
  cl_event* outBlockingEventPtr = NULL;
  cl_event outBlockingEvents[3];

  for (size_t iterIdx = 0; iterIdx <totalBlocks ; iterIdx++) {
    // ping is the current buffer, pong is the other one
    int pingIdx = iterIdx % 2;
    cl_int clErr = 0;
    
    memcpy(mInBuf[pingIdx], inBuf+iterIdx*inBlockSize, inBlockSize);
    if (iterIdx == 0 || iterIdx == 1) {
      OCL_CHECK(clEnqueueMigrateMemObjects(oclAccel.commands, 1, &(mInClMem[pingIdx]),0, 0, NULL, &(writeEvents[iterIdx])));
    } else {
      clWaitForEvents(1,&(kernelEvents[iterIdx-2])); 
      OCL_CHECK(clEnqueueMigrateMemObjects(oclAccel.commands, 1, &(mInClMem[pingIdx]),0, 1, &(kernelEvents[iterIdx-2]), &writeEvents[iterIdx]));
      //clWaitForEvents(1,&(kernelEvents[iterIdx-2]));       
    }

    // Set Kernel arguments
    ops &= PING_PONG_IN_MASK_OFF;
    ops &= PING_PONG_OUT_MASK_OFF;
    ops |= pingIdx?0x0003:0x0000;
    clErr  = clSetKernelArg(oclAccel.kernel, 0, sizeof(int), &ops);
    clErr  = clSetKernelArg(oclAccel.kernel, 1, sizeof(cl_mem), &(mInClMem[0]));
    clErr  = clSetKernelArg(oclAccel.kernel, 2, sizeof(cl_mem), &(mInClMem[1]));    
    clErr |= clSetKernelArg(oclAccel.kernel, 3, sizeof(cl_mem), &(mOutClMem[0]));
    clErr |= clSetKernelArg(oclAccel.kernel, 4, sizeof(cl_mem), &(mOutClMem[1]));    
    for (unsigned int i=0; i<mNoDdrA;i++)
      clErr |= clSetKernelArg(oclAccel.kernel, i+5, sizeof(cl_mem), &(mDdrClMem[0]));

    if (clErr != CL_SUCCESS) {
      printf("Error: Failed to set kernel arguments! %s\n", oclAccel.getErrorString(clErr));
    }

    // Do not enqueue task unless write to ping-buffer is done and previous ping read buffer done
     if(iterIdx == 0 || iterIdx == 1 ) {
      noOfKernelBlockingEvents=1;
      kernelBlockingEventPtr= &(writeEvents[iterIdx]);
     } else {
      noOfKernelBlockingEvents=2;
      kernelBlockingEvents[0]=writeEvents[iterIdx];
      kernelBlockingEvents[1]=readEvents[iterIdx-2];
      kernelBlockingEventPtr=kernelBlockingEvents;
      clWaitForEvents(1,&(readEvents[iterIdx-2])); 
     }

     OCL_CHECK( clEnqueueTask(oclAccel.commands, oclAccel.kernel, noOfKernelBlockingEvents,kernelBlockingEventPtr, &(kernelEvents[iterIdx])));


    // This operation only needs to wait for kernel call and readCallBackData[iterIdx-2]
    // wraparound readback buffer after 4 callbacks, ping-pong will block previous readbacks    
    if(iterIdx == 0 || iterIdx == 1 ) {
      noOfOutBlockingEvents = 1;
      outBlockingEventPtr = &(kernelEvents[iterIdx]);
    } else {
      noOfOutBlockingEvents = 2;
      outBlockingEvents[0] = kernelEvents[iterIdx];
      outBlockingEvents[1] = readCallBackData[iterIdx-2].event;
      outBlockingEventPtr = outBlockingEvents;
    }
    OCL_CHECK(clEnqueueMigrateMemObjects(oclAccel.commands, 1, &(mOutClMem[pingIdx]), CL_MIGRATE_MEM_OBJECT_HOST, noOfOutBlockingEvents, outBlockingEventPtr, &(readEvents[iterIdx])));
 //   set_callback(readEvents[iterIdx], "ooo_queue");
    
    readCallBackData[iterIdx].iterIdx = iterIdx;
    readCallBackData[iterIdx].hostOutBuf = outBuf+iterIdx*outBlockSize;
    readCallBackData[iterIdx].devOutBuf = mOutBuf[pingIdx];
    readCallBackData[iterIdx].event =clCreateUserEvent(oclAccel.context,NULL);
    OCL_CHECK( clSetEventCallback(readEvents[iterIdx],CL_COMPLETE,copyReadBuf,&(readCallBackData[iterIdx])));
  }
// Wait for all of the OpenCL operations to complete
  clFlush(oclAccel.commands);
  clFinish(oclAccel.commands);

  gettimeofday(&tEnd,0);
}

int main(int argc, char** argv) {

  // Run in CPU mode
  ArgParser input(argc, argv);
  std::string optValue("hw");
  // Run the software here..
  cIndirect();
  bool sw_emu = 0;
  if (input.getCmdOption("-mode",optValue)) {
    if (optValue.compare("cpu") == 0) { // if only CPU quit
      return 0;
    }
    if (optValue.compare("sw_emu") == 0) { // if only sw_emu
      sw_emu = 1;     
      mDdrSize = 1U<<29;      
    }

  }

/**
 *  pseudo channel function select
 *
 */
  int p_ch=1;
  if (input.getCmdOption("-pc",optValue)) {
    p_ch= atoi(optValue.c_str());
	if (p_ch != 0) p_ch = 1;
  }
	/* HBMBankCount argument */
	char HBMBankCount_str[1001] = "16";
	int HBMBankCount = stoi(std::string((const char *) HBMBankCount_str));
	bool HBMBankCount_valid = true;
	if (input.getCmdOption("-hbm",optValue)) {
		strcpy(HBMBankCount_str, optValue.c_str());
		HBMBankCount = stoi(std::string((const char *) HBMBankCount_str));
	}
	// contains digits
	HBMBankCount_valid = is_digits(HBMBankCount_str);
	// validate HBMBankCount input
	if (HBMBankCount_valid){
		// greater than min, less than max
		HBMBankCount_valid = (0 < HBMBankCount) && (HBMBankCount <= (MAX_HBM_BANKCOUNT));
		// power of two
		HBMBankCount_valid = HBMBankCount_valid && ((HBMBankCount & (HBMBankCount - 1)) == 0);
	
		// using BANK with IN/OUT
		long long int myDDRTest = (MAX_DDR_SIZE);
		if ((HBMBankCount > IN_HBM_BANKCOUNT_IDX)){
			myDDRTest = myDDRTest >> 1;
		}
		long long int mySizeTest = (((long long int) HBMBankCount) * myDDRTest);
		// Is there sufficient space in HBM for DB
		HBMBankCount_valid = HBMBankCount_valid && (mySizeTest >= DBSize);
	}
	if (!HBMBankCount_valid) {
		// error
		printf("Value %s: Invalid value for hbmbankcount!\n", HBMBankCount_str);
		return -1;
	}
/*
 * hbm count select
 */

  int hbmNum =HBMBankCount;


/*
 * enable PU in opmode
 */

int op = 0;
char stream[1001] = "16";
	int streamValue = stoi(std::string((const char *) stream));
	if (input.getCmdOption("-pu",optValue)) {
		strcpy(stream, optValue.c_str());
	}

#define streamBit(x) if (!strcmp(stream,VALUE_TO_STRING(x))){ \
		op |= STREAM_MASK_##x; \
		streamValue = x; \
	}
	streamBit(1)
	streamBit(2)
	streamBit(4)
	streamBit(8)
	streamBit(16)
	if ((op & STREAM_MASK) == 0x0) {
		// error
		printf("Value %s: Invalid value for stream!\n", stream);
		return -1;
	}

/**
 *  callback function select
 *
 */
  int callback=0;
  if (input.getCmdOption("-cb",optValue)) {
    callback= atoi(optValue.c_str());
  }

  in_mapp(sw_emu, hbmNum, p_ch);

  struct timeval tStart, tSetupDone, tGatherStart, tGatherEnd,tReadDataDone, tEnd;
  gettimeofday(&tStart,0);
  
  // Set device, kernel, xclbin names
  char deviceName[1001] = "xilinx:vcu1550:dynamic";
  if (input.getCmdOption("-dsa",optValue)) {
    strcpy(deviceName, optValue.c_str());
  }

  char kernelName[1001] = "omx";
  if (input.getCmdOption("-kernel",optValue)) {
    strcpy(kernelName, optValue.c_str());
  } 
  char xclbinName[1001] = "omx_VCU1525_hw.xclbin";
  if (input.getCmdOption("-xclbin",optValue)) {
    strcpy(xclbinName, optValue.c_str());
  } 

  bool run[5] = {true,true,true,true,true};
  if (input.getCmdOption("-run",optValue)) {
    for (int i=0; i<5; i++)
      run[i] = false;
    run[stoi(optValue)%3] = true;
  }
  printf("Starting omx dsa=%s xclbin=%s kernel=%s\n pu=%d hbmNum=%d callback=%d pseudochannel=%d \n", deviceName,kernelName,xclbinName,streamValue,hbmNum,callback,p_ch);

  cl_int clErr;

  // Connect to a valid Xilinx platform
  OclAccel oclAccel;
  oclAccel.init();
  oclAccel.connectPlatform(deviceName,kernelName,xclbinName);

  if (oclAccel.mStatus != OclAccel::Normal) {
    printf("Error: Could not successfully connect to a Xilinx platform.");
    return 0;
  }
  long int cl_max_mem; 
  long long int cl_global_mem;
  /*
    clGetDeviceInfo( oclAccel.deviceId, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(long int), &cl_max_mem, NULL);
    printf("CL_DEVICE_MAX_MEM_ALLOC_SIZE %x \n",cl_max_mem);
    clGetDeviceInfo( oclAccel.deviceId, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(long long int), &cl_global_mem, NULL);
    printf("CL_DEVICE_GLOBAL_MEM_SIZE %x \n",cl_global_mem);
  */
  int max_num_buffer = mNoDdrA;
  int num_buffer = hbmNum;
  char *mDdrBuf[max_num_buffer];
  cl_mem  mDdrClMem[max_num_buffer];
  cl_mem_ext_ptr_t mBankMem[max_num_buffer];
#define BANK_NAME(n) n | XCL_MEM_TOPOLOGY
  int banks[MAX_HBM_BANKCOUNT] = {
	BANK_NAME(0),
	BANK_NAME(1),
	BANK_NAME(2),
	BANK_NAME(3),
	BANK_NAME(4),
	BANK_NAME(5),
	BANK_NAME(6),
	BANK_NAME(7),
	BANK_NAME(8),
	BANK_NAME(9),
	BANK_NAME(10),
	BANK_NAME(11),
	BANK_NAME(12),
	BANK_NAME(13),
	BANK_NAME(14),
	BANK_NAME(15),
        BANK_NAME(16),
        BANK_NAME(17),
        BANK_NAME(18),
        BANK_NAME(19)
};
#undef BANK_NAME
  if(sw_emu==1) {
    mBankMem[0].flags = banks[0];
    mBankMem[0].obj = 0;
    mBankMem[0].param = 0;
    clErr = createBuf(oclAccel,mDdrSize, mDdrClMem[0], mDdrBuf[0], mBankMem[0]);
  } else {
    // mapping of buffer to bank
    for (int i = 0; i < hbmNum; i ++){
	if (p_ch){
    		mBankMem[i].flags = banks[i];
		//mBankMem[(2*i)+1].flags = banks[i];
	}
	else{
	    mBankMem[i].flags = banks[2*i];
	}
    }
    for (int i=0; i<num_buffer; i++) {
      mBankMem[i].obj = 0;
      mBankMem[i].param = 0;
    }
      
    int mPerDdrSize=mDdrSize/hbmNum;
    // pseudochannel: size of buffer
    for (int i=0; i<num_buffer; i++) {
      clErr = createBuf(oclAccel,mPerDdrSize, mDdrClMem[i], mDdrBuf[i], mBankMem[i]);
    }
  } 

  // create input buffer 
  cl_mem  mInClMem[2];
  cl_mem_ext_ptr_t inBufExt[2];
  inBufExt[0].flags = banks[16];
  inBufExt[1].flags = banks[17];
  char *mInBuf[2];
  for (int i =0 ; i< 2; i++) {
    inBufExt[i].obj = 0;
    inBufExt[i].param = 0;    
    clErr = createBuf(oclAccel, inBlockSize, mInClMem[i], mInBuf[i], inBufExt[i]);;
    if (clErr != CL_SUCCESS) {
      printf("Error: Failed to create buffer with msg %s\n",oclAccel.getErrorString(clErr));
    }
  }

  // create output buffer 
  // use as call back methods ; create ping pong buffers for outbuffer
  cl_mem  mOutClMem[totalBlocks];
  cl_mem_ext_ptr_t outBufExt[totalBlocks];
  char *mOutBuf[totalBlocks];

  if(callback==1) {
  outBufExt[0].flags = banks[18];
  outBufExt[1].flags = banks[19];
  for (int i =0 ; i< 2; i++) {
    outBufExt[i].obj = 0;
    outBufExt[i].param = 0;    
    clErr = createBuf(oclAccel, outBlockSize, mOutClMem[i], mOutBuf[i], outBufExt[i]);;
    if (clErr != CL_SUCCESS) {
      printf("Error: Failed to create buffer with msg %s\n",oclAccel.getErrorString(clErr));
    }
    }
  } else { //without using callback function create 256 outbufs. 
  for (int i =0 ; i< totalBlocks; i++) {
    outBufExt[i].obj = 0;
    outBufExt[i].param = 0;    
    outBufExt[i].flags = i%2?banks[19]:banks[18];
    clErr = createBuf(oclAccel, outBlockSize, mOutClMem[i], mOutBuf[i], outBufExt[i]);;
    if (clErr != CL_SUCCESS) {
      printf("Error: Failed to create buffer with msg %s\n",oclAccel.getErrorString(clErr));
    }
  }
  
 }
  // copy database A into each mapped DDR
  if(sw_emu ==1) {
    memcpy(mDdrBuf[0], A, sizeof(struct Row)*rows);
  } else {
    for(int i=0;i<num_buffer;i++) {
      memcpy(mDdrBuf[i], A+i*rows/num_buffer, sizeof(struct Row)*rows/num_buffer);
    }
  }    

  // Move data to the FPGA device DDR buffers
  cl_event ddrHost2XilEvent;
  if(sw_emu ==1) {
    clErr = clEnqueueMigrateMemObjects(oclAccel.commands, 1, mDdrClMem,0, 0, NULL, &ddrHost2XilEvent);
  } else {
    clErr = clEnqueueMigrateMemObjects(oclAccel.commands, num_buffer, mDdrClMem,0, 0, NULL, &ddrHost2XilEvent);
  }
 

  if (clErr != CL_SUCCESS) {
    printf("Error: Migrate map buffer failed for ddrbuf with msg %s\n",oclAccel.getErrorString(clErr));
    return 0;
  }    
  clWaitForEvents(1, &ddrHost2XilEvent);

  printf("Info: xclbin loaded, A set on device DDR, starting all ops on FPGA\n");
  gettimeofday(&tSetupDone,0);
  
  // Schedule gather first
  if (totalBlocks*inBlockSize != 4*rows || totalBlocks*outBlockSize != 8*rows) {
    printf("Error: In current implementation rows %d must be multiple of inBlockSize %d and rows %d must be multiple of outBlockSize %d\n",4*rows,inBlockSize,8*rows,outBlockSize);
  } else {
    if (run[Gather]) {
      printf("Info: Gather Started.. ");
      if(callback==1) {
        execFpgaOp_cb(op,(char*)(in_mapped),(char*) xilout,oclAccel,mInClMem,mInBuf,mOutClMem,mOutBuf,mDdrClMem, tGatherStart, tGatherEnd);
      } else {
        execFpgaOp(op,(char*)(in_mapped),(char*) xilout,oclAccel,mInClMem,mInBuf,mOutClMem,mOutBuf,mDdrClMem, tGatherStart, tGatherEnd);
      }
      fflush(stdout);      
      printf(" ..Done\n");
     if(callback==0)  {
       for (int i=0; i<totalBlocks; i++)
         memcpy(xilout+(i*outBlockSize/sizeof(Row)), mOutBuf[i], outBlockSize);
     }
      checkOp("Gather",out,xilout,true);      
    }
  }
  
  oclAccel.clear();
  gettimeofday(&tEnd,0);
  clearData();

  //printf("Setup: %lu, Gather: %lu, Indirect gather: %lu Segmented Gather: %lu Total: %lu \n",
  printf("Setup: %lu, Gather: %lu, Total: %lu \n",  
	 diff(&tSetupDone,&tStart), diff(&tGatherEnd,&tGatherStart),
         diff(&tEnd, &tStart));

}
