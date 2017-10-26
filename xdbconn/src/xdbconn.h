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
#ifndef XDBCONN_XDBCONN_H
#define XDBCONN_XDBCONN_H

#include "oclaccel.h"
#include <vector>

/*
 * Write and read buffers are from host perspective. Write buffers are InBuffers, Read are Out buffers.
 * Read and write blocks can be of different sizes to allow for much shorter results
 * 
 * Circular queue of mQueueDepth is used to send blocks of data from host to device. User can set the queuedepth 
 */
class XdbConn {
public:

  enum Status {Normal=0, Error} ; // Add more as needed
  // By default, memmigratebuffer will be used, set useEnqueueBuffers to use enqueuebuffer mode
  // All opflags are set to false by default
  //
  // if clmemmapdbuffer is used, bufin and bufout are not allocated
  // Set UseEnqueueBuffers if want to use enqueue calls instead of default memmigrate calls
  // Set EnableProfiling to print profile text information
  // Set DisableZeroCopyClMemMapd to not use MemMapd buffer in memmigrate mode, uses extra buffer
  //                           if enqueue mode, cannot use ClMemmaps buffer so it is true
  // Set DisableUserDataCopy to disable any userdata copy. In this case readback functions are not called, and can be used to test measure throughput bandwidths
  //
  enum OpFlag { UseEnqueueBuffers=0, EnableProfiling, DisableZeroCopyClMemMapd, DisableUserDataCopy, OpFlagEnd};

  void setOpFlag(OpFlag opFlag) { mOpFlags[opFlag] = true; }
  bool getOpFlag(OpFlag opFlag) { return mOpFlags[opFlag]; }  
  void clearOpFlag(OpFlag opFlag) { mOpFlags[opFlag] = false; }

  typedef void (*BufCopyFuncPtr)(XdbConn *xdbconn, int queueLoc, cl_event event, void *userData); 
  void setWriteBufCopyFuncPtr(BufCopyFuncPtr fptr) { mWriteBufCopyFuncPtr = fptr; }
  BufCopyFuncPtr getWriteBufCopyFuncPtr() { return mWriteBufCopyFuncPtr ; }
  void setReadBufCopyFuncPtr(BufCopyFuncPtr fptr) { mReadBufCopyFuncPtr = fptr; }
  BufCopyFuncPtr getReadBufCopyFuncPtr() { return mReadBufCopyFuncPtr ; }
  
  static XdbConn *create(int queueDepth, int writeBlockSize, int readBlockSize);
  static void remove(XdbConn *xdbConn);
  Status init(BufCopyFuncPtr writeBufCopyFuncPtr=nullptr, BufCopyFuncPtr readBufCopyFuncPtr=nullptr);
  void clear();
  

  Status connectPlatform(const char *targetDevice, const char *kernelName, const char *xclbin);
  Status createAllBufs();
  Status enqueueCompute(void *userData=nullptr);
  Status enqueueReadBlock(void *userData);
  Status enqueueWriteBlock(void *userData);    

  Status finishCommands();

  // Debug statements
  static void eventStatus(cl_event event);
  
  bool getProfileData(bool printProfile=true);
  
  bool getWriteClMem(int queueLoc, cl_mem& clMem) {
    if (queueLoc >=mQueueDepth)
      return false;
    clMem = mWriteClMem[queueLoc];
  }

  bool getReadClMem(int queueLoc, cl_mem& clMem) {
    if (queueLoc >=mQueueDepth)
      return false;
    clMem = mReadClMem[queueLoc];
  }
  int getQueueDepth() { return mQueueDepth; }
  int getWriteBlockSize() { return mWriteBlockSize; }
  int getReadBlockSize() { return mReadBlockSize; }  

  char* getWriteBuf(int queueLoc) {
    if (queueLoc >=mQueueDepth)
      return nullptr;
    return mWriteBuf[queueLoc];
  }

  char *getWriteBuf() {
    return mWriteBuf[mCurrentQueue];
  }
  
  char* getReadBuf(int queueLoc) {
    if (queueLoc >=mQueueDepth)
      return nullptr;
    return mReadBuf[queueLoc];
  }

  char* getReadBuf() {
    return mReadBuf[mCurrentQueue];
  }
  
  void* getWriteClMemMapd(int queueLoc) {
    if (queueLoc >=mQueueDepth)
      return nullptr;
    return mWriteClMemMapd[queueLoc];
  }

  void* getReadClMemMapd(int queueLoc) {
    if (queueLoc >=mQueueDepth)
      return nullptr;
    return mReadClMemMapd[queueLoc];
  }
  OclAccel& getOclAccel() { return mOclAccel;  }

  int getCurrentQueuePosition() const { return mCurrentQueue; }
  int getPrevQueuePosition() const { return mPrevQueue; }  

  // set these variable to add a data flag to the kernel
  bool dataFlag;
  unsigned int dataFlags;
  
private:
  
  int mQueueDepth;
  int mWriteBlockSize;
  int mReadBlockSize;  
  
  std::vector<char*> mWriteBuf;
  std::vector<char*> mReadBuf;
  std::vector<cl_mem> mWriteClMem;
  std::vector<cl_mem> mReadClMem;
  std::vector<void*> mWriteClMemMapd;
  std::vector<void*> mReadClMemMapd;

  OclAccel mOclAccel;

  int mCurrentQueue;
  int mPrevQueue;
  std::vector<cl_event> mWriteEvent;
  std::vector<cl_event> mWriteMemReadAllocEvent;  
  std::vector<cl_event> mTaskEvent;
  std::vector<cl_event> mReadEvent;
  std::vector<void *> mUserData;
  
  Status mStatus;
  bool   mOpFlags[16]; // up to 16 flags
  bool mPlatformConnected;
  bool mBufferCreated;

  bool mFirstWriteInQueue;
  bool mFirstReadInQueue;
  std::vector<cl_event> mWriteBufReadyEvent;
  std::vector<cl_event> mReadBufProcessedEvent;
  BufCopyFuncPtr  mWriteBufCopyFuncPtr;
  BufCopyFuncPtr  mReadBufCopyFuncPtr;  
  
  static void copyReadBuf(cl_event event, cl_int eventCommandExecStatus, void *readCallBackData);

  struct ReadCallBackData {
    int queueLoc;
    XdbConn *xdbConn;
    void *userData;
  };
  std::vector<ReadCallBackData> mReadCallBackData;

  struct ProfileData {
    unsigned long long mQueuedTime;
    unsigned long long mSubmitTime;
    unsigned long long mStartTime;
    unsigned long long mEndTime;
  };
  std::vector<ProfileData> mReadProfileData;
  std::vector<ProfileData> mWriteProfileData;
  std::vector<ProfileData> mTaskProfileData;  
  bool saveProfileData(cl_event event, std::vector<ProfileData>& profileDataVec);
  void printProfileData(ProfileData profileData, const char* id, int qno, int blockSize);
  
  // Private construction only use create 
  XdbConn() : dataFlag(false) { }
  ~XdbConn() {   }

};

#endif
