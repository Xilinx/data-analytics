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
