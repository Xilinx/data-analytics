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
#include "xdbconn.h"
#include <fstream>
#include <iostream>
#include <execinfo.h>

XdbConn *XdbConn::create(int queueDepth, int writeBlockSize, int readBlockSize) {
  XdbConn *xdbConn  = new XdbConn();
  xdbConn->mQueueDepth= queueDepth;
  xdbConn->mWriteBlockSize=writeBlockSize;
  xdbConn->mReadBlockSize=readBlockSize;
  for (int i=0; i<OpFlagEnd; i++)
    xdbConn->mOpFlags[i] = false;

//    xdbConn->mOpFlags[0] = true;
  
  xdbConn->mPlatformConnected = false;
  xdbConn->mBufferCreated = false;  
  xdbConn->mStatus = Normal;
  
  return xdbConn;
}

void XdbConn::remove(XdbConn *xdbConn) {
  if (xdbConn == nullptr)
    return;
  xdbConn->clear();
  delete xdbConn;
}

XdbConn::Status XdbConn::init(BufCopyFuncPtr writeBufCopyFuncPtr, BufCopyFuncPtr readBufCopyFuncPtr) {

  mFirstWriteInQueue = true;
  mFirstReadInQueue = true;
  mOclAccel.init();

  mWriteBufCopyFuncPtr=  writeBufCopyFuncPtr;
  mReadBufCopyFuncPtr = readBufCopyFuncPtr;
  
  mWriteClMem.resize(mQueueDepth);
  mReadClMem.resize(mQueueDepth);

  mWriteClMemMapd.resize(mQueueDepth); 
  mReadClMemMapd.resize(mQueueDepth);
  for (int i=0; i<mQueueDepth;i++) {
    mWriteClMemMapd[i] = nullptr;
    mReadClMemMapd[i] = nullptr;
  }
  
  mWriteBuf.resize(mQueueDepth); 
  mReadBuf.resize(mQueueDepth);
  for (int i=0; i<mQueueDepth;i++) {
    mWriteBuf[i] = nullptr;
    mReadBuf[i] = nullptr;
  }

  mWriteEvent.resize(mQueueDepth);
  mWriteMemReadAllocEvent.resize(mQueueDepth);
  mReadEvent.resize(mQueueDepth);
  mTaskEvent.resize(mQueueDepth);

  mCurrentQueue = 0;
  mPrevQueue = mQueueDepth-1;
  
  mWriteBufReadyEvent.resize(mQueueDepth);
  mReadBufProcessedEvent.resize(mQueueDepth);
  mUserData.resize(mQueueDepth);
  for (int i=0; i<mQueueDepth;i++) {
    mWriteBufReadyEvent[i] = NULL;
    mReadBufProcessedEvent[i] = NULL;
    mUserData[i] = NULL;
  }

  //Constant value used by copyreadbuf
  mReadCallBackData.resize(mQueueDepth);
  for (int i=0; i<mQueueDepth;i++) {
    mReadCallBackData[i].queueLoc = i;
    mReadCallBackData[i].xdbConn = nullptr;
    mReadCallBackData[i].userData = nullptr;    
  }

  return mStatus;
}

void XdbConn::clear() {

  // first finish all commands
  finishCommands();

  // Delete buffers as it was created
  if (!mBufferCreated) {
    printf("Warning: Calling clear without creating buffers successfully, no buffers to clear.\n");
  } else {
    if (mOpFlags[UseEnqueueBuffers] == false) {
      int err;
      for (int i =0; i < mQueueDepth; i++) {
	err = clEnqueueUnmapMemObject(mOclAccel.commands, mWriteClMem[i], mWriteClMemMapd[i], 0, NULL, NULL);
	if (err != CL_SUCCESS) {
	  printf("Error: Could not properly clear Enqueue unmap memobject failed for mWriteClMem[ %d ] for memcpy with msg %s\n",i,mOclAccel.getErrorString(err));    
	  mStatus = Error;
	}
      }

      for (int i =0; i < mQueueDepth; i++) {
	err = clEnqueueUnmapMemObject(mOclAccel.commands, mReadClMem[i], mReadClMemMapd[i], 0, NULL, NULL);
	if (err != CL_SUCCESS) {
	  printf("Error: Could not properly clear Enqueue unmap memobject failed for mReadClMem[ %d ] for memcpy with msg %s\n",i,mOclAccel.getErrorString(err));    
	  mStatus = Error;
	}
      }
    }

    //delete allocated buffers
    if (mOpFlags[UseEnqueueBuffers] || mOpFlags[DisableZeroCopyClMemMapd]) {
      for (int i =0; i<mQueueDepth; i++) {
	delete mWriteBuf[i];
	delete mReadBuf[i];    
      }
    }
  }

  /*todo currently clreleasememobject is not fully functional, could be related to user events needs debugging, release it later
  for (int i =0; i < mQueueDepth; i++) {  
    clReleaseMemObject(mWriteClMem[i]);
    clReleaseMemObject(mReadClMem[i]); 
    }*/

  mOclAccel.clear();
  mBufferCreated = false;
}

XdbConn::Status XdbConn::connectPlatform(const char *targetDevice, const char *kernelName, const char *xclbin) {
  // Enable out of order exec mode always
  mOclAccel.commandQueueOrderProp = CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
  if (mOpFlags[EnableProfiling])
    mOclAccel.commandQueueOrderProp |= CL_QUEUE_PROFILING_ENABLE;
  
  OclAccel::Status status = mOclAccel.connectPlatform(targetDevice, kernelName,xclbin);
  if (status != OclAccel::Normal)
    mStatus = Error;
  mPlatformConnected = true;
  return mStatus;
}

XdbConn::Status XdbConn::createAllBufs() {

  cl_int err;
  //create N buffers in device
  for (int i =0; i<mQueueDepth; i++)   {
    mWriteClMem[i] = clCreateBuffer(mOclAccel.context,  CL_MEM_READ_ONLY, mWriteBlockSize, NULL, &err);
    if (err != CL_SUCCESS || !mWriteClMem[i]) {
	printf("Error: Failed to create buffer mWriteClMem[ %d ] with msg %s\n",i,mOclAccel.getErrorString(err));
	mStatus = Error;
	return mStatus;
    }
    
    mReadClMem[i] = clCreateBuffer(mOclAccel.context, CL_MEM_WRITE_ONLY, mReadBlockSize, NULL, &err);
    if (err != CL_SUCCESS || !mReadClMem[i]) {
	printf("Error: Failed to create buffer mReadClMem[ %d ] with msg %s\n",i,mOclAccel.getErrorString(err));
	mStatus = Error;
	return mStatus;
    }

  }
 
  // Now map clmemobjects for memcpy
  // get pointer mapped to device bufferfor writing, wait till mWriteEvent[qn] is complete
  if (mOpFlags[UseEnqueueBuffers] == false ) {
    for (int i =0; i<mQueueDepth; i++)   {  
      mWriteClMemMapd[i] = clEnqueueMapBuffer(mOclAccel.commands,mWriteClMem[i], CL_TRUE, CL_MAP_WRITE, 0, mWriteBlockSize, 0, NULL, NULL, &err);
      if (err != CL_SUCCESS) {
	printf("Error: Enqueue map buffer failed for mWriteClMem[ %d ] for memcpy with msg %s\n",i,mOclAccel.getErrorString(err));
	mStatus = Error;
	return mStatus;
      }
    }

    for (int i =0; i<mQueueDepth; i++)   {    
      mReadClMemMapd[i] = clEnqueueMapBuffer(mOclAccel.commands,mReadClMem[i], CL_TRUE, CL_MAP_READ, 0, mReadBlockSize, 0, NULL, NULL, &err);
      if (err != CL_SUCCESS) {
	printf("Error: Enqueue map buffer failed for mReadClMem[ %d ] for memcpy with msg %s\n",i,mOclAccel.getErrorString(err));
	mStatus = Error;
	return mStatus;
      }    
    }
  }

  for (int i =0; i<mQueueDepth; i++) {
    if (mOpFlags[UseEnqueueBuffers] || mOpFlags[DisableZeroCopyClMemMapd]) 
      mWriteBuf[i] = new char[mWriteBlockSize];
    else
      mWriteBuf[i] = (char *)mWriteClMemMapd[i];
  }
  for (int i =0; i<mQueueDepth; i++) {
    if (mOpFlags[UseEnqueueBuffers] || mOpFlags[DisableZeroCopyClMemMapd])     
      mReadBuf[i] = new char[mReadBlockSize];
    else
      mReadBuf[i] = (char *)mReadClMemMapd[i];    
  }
  
  mBufferCreated = true;
  return mStatus;
}

XdbConn::Status XdbConn::enqueueCompute(void *userData) {

  //  printf("SREE: In enqueueCompute\n");
  // mUserData used only if data copy is not disabled
  if (!mOpFlags[DisableUserDataCopy] )
    mUserData[mCurrentQueue] = userData;
  // Set the arguments to our compute kernel todo currently only two see if needs to be increased
    //printf("SREE: In BP 0\n");
  cl_int err = 0;
  err  = clSetKernelArg(mOclAccel.kernel, 0, sizeof(cl_mem), &(mWriteClMem[mCurrentQueue]));
    //printf("SREE: In BP 1\n");
  err |= clSetKernelArg(mOclAccel.kernel, 1, sizeof(cl_mem), &(mReadClMem[mCurrentQueue]));
  //if (dataFlag)
    //err |= clSetKernelArg(mOclAccel.kernel, 2, sizeof(dataFlags), &(dataFlags)); 
  if (mOclAccel.printError(err,"Failed to set kernel arguments") )  {
    mStatus = Error;
    return mStatus;
  }
    //printf("SREE: In BP 3\n");

  if (enqueueWriteBlock(userData) == Error)  return mStatus;
    //printf("SREE: In BP 4\n");

  // Enqueue next compute blocked by current write eW[i], returns event eT[i]

  if (mOpFlags[UseEnqueueBuffers]) {
    //printf("SREE: In BP 4.1\n");
    err = clEnqueueTask(mOclAccel.commands, mOclAccel.kernel, 1, &mWriteEvent[mCurrentQueue], &mTaskEvent[mCurrentQueue]);
  } else {
    //printf("SREE: In BP 4.2\n");
    cl_event taskBlockingEvents[2] = {mWriteEvent[mCurrentQueue],mWriteMemReadAllocEvent[mCurrentQueue]};
    err = clEnqueueTask(mOclAccel.commands, mOclAccel.kernel, 2, taskBlockingEvents, &mTaskEvent[mCurrentQueue]);    
  }
    //printf("SREE: In BP 5\n");
  if (mOclAccel.printError(err,"Failed to execute kernel") ) {
    mStatus = Error;
    return mStatus;
  } 
    //printf("SREE: In BP 6\n");
    
  if (enqueueReadBlock(userData) == Error)  return mStatus;
    //printf("SREE: In BP 7\n");
  
  // go to next queue, wrap around of max reached
  mPrevQueue = mCurrentQueue;
  mCurrentQueue++;
  if (mCurrentQueue == mQueueDepth) // reach the queue depth
    finishCommands();
  //printf("SREE: In BP 8\n");

  return mStatus;
}

// Call finishCommands to complete all the commands in the queue
// resets queuelength to 0
XdbConn::Status XdbConn::finishCommands() {
  if (mOclAccel.finishCommands() == OclAccel::Error)
    mStatus = Error;
  for (int rq=0; rq<mCurrentQueue;rq++) {

    if (mReadBufProcessedEvent[rq] != NULL) {
      clWaitForEvents(1, &(mReadBufProcessedEvent[rq]));
    }
  }
 /*if (mOpFlags[EnableProfiling]) {
    for (int qn =0; qn< mCurrentQueue; qn++)  {
      saveProfileData(mWriteEvent[qn], mWriteProfileData);
      saveProfileData(mReadEvent[qn], mReadProfileData);
      saveProfileData(mTaskEvent[qn], mTaskProfileData);	
    }
  }*/
  // 

  // Now reset all the internal values for command to restart

  mFirstWriteInQueue = true;
  mFirstReadInQueue = true;

  mPrevQueue = mQueueDepth-1;
  mCurrentQueue = 0;
 
  
  return mStatus;
}

// Callback function - called when read is complete
void XdbConn::copyReadBuf(cl_event event, cl_int eventCommandExecStatus, void *readCallBackData) {
  if (eventCommandExecStatus != CL_COMPLETE) {
    printf("Error: copyReadBuf called when event is not completed and status is not CL_COMPLETE");
  }
  ReadCallBackData *rcbdata = (ReadCallBackData *) readCallBackData;
  if (rcbdata->xdbConn == nullptr) {
    printf("Error: copyReadBuf called with null xdbconn internal error.");
  } else {
    cl_event event = (rcbdata->xdbConn->mReadBufProcessedEvent)[rcbdata->queueLoc];
    if (rcbdata->xdbConn->mReadBufCopyFuncPtr) // if null nothing to copy    
      (*(rcbdata->xdbConn->mReadBufCopyFuncPtr))(rcbdata->xdbConn,rcbdata->queueLoc, event, rcbdata->userData);
    else // else mark the event complete todo see if runtime fix needed
      clSetUserEventStatus(event, CL_COMPLETE);    
  }
}

XdbConn::Status XdbConn::enqueueReadBlock(void *userData) {
  // Enqueue next read blocked by current compute eT[i], return event eR[i]
  cl_int err = CL_SUCCESS;

  int noOfBlockingEvents = 1;
  cl_event readBlockingEvents[2];
  cl_event* readBlockingEventPtr = &(mTaskEvent[mCurrentQueue]);

  // Use No User data copy to get maximum throughput performance
  if ( !mOpFlags[DisableUserDataCopy] && !mFirstReadInQueue) {
    noOfBlockingEvents = 2;
    readBlockingEvents[0] = mTaskEvent[mCurrentQueue];
    readBlockingEvents[1] = mReadBufProcessedEvent[mPrevQueue];
    readBlockingEventPtr = readBlockingEvents;
  }

  if (mFirstReadInQueue)
    mFirstReadInQueue = false;

  if (mOpFlags[UseEnqueueBuffers] == false ) {
    err = clEnqueueMigrateMemObjects(mOclAccel.commands, 1, &(mReadClMem[mCurrentQueue]),  CL_MIGRATE_MEM_OBJECT_HOST, noOfBlockingEvents, readBlockingEventPtr, &(mReadEvent[mCurrentQueue]));
  } else {
    err = clEnqueueReadBuffer( mOclAccel.commands, mReadClMem[mCurrentQueue], CL_FALSE, 0, mReadBlockSize, mReadBuf[mCurrentQueue], noOfBlockingEvents, readBlockingEventPtr , &(mReadEvent[mCurrentQueue]));
  }
  if (mOclAccel.printError(err,"Failed to read device outbuf") ) {
    mStatus = Error;
    return mStatus;
  }
  if ( !mOpFlags[DisableUserDataCopy]) {
    mReadBufProcessedEvent[mCurrentQueue] = clCreateUserEvent(mOclAccel.context, &err);
    mReadCallBackData[mCurrentQueue].userData = userData;
    mReadCallBackData[mCurrentQueue].xdbConn = this;    
    err = clSetEventCallback(mReadEvent[mCurrentQueue],CL_COMPLETE,copyReadBuf,&(mReadCallBackData[mCurrentQueue]));
    if (mOclAccel.printError(err,"Could not properly set callback event and function") ) {
      mStatus = Error;
      return mStatus; 
    }
  } // if no event sync - copy data after read is finished for the whole queue
  
  return mStatus;
}

XdbConn::Status XdbConn::enqueueWriteBlock(void *userData) {

  cl_int err1 = CL_SUCCESS;
  cl_int err2 = CL_SUCCESS;

  int noOfBlockingEvents = 0;
  cl_event writeBlockingEvents[2];
  cl_event* writeBlockingEventPtr = NULL;

  int noOfReadAllocBlockingEvents = 0;
  cl_event* readAllocBlockingEventPtr = NULL;
  
  if (!mFirstWriteInQueue && !mOpFlags[UseEnqueueBuffers]) {
    noOfReadAllocBlockingEvents = 1;
    readAllocBlockingEventPtr = &(mWriteMemReadAllocEvent[mPrevQueue]);
  }
  
  // Use No User data copy to get maximum throughput performance  
  if ( !mOpFlags[DisableUserDataCopy] ) {
    // Create a writebuf event, this event is marked COMPLETE by app using copywritebuf func ptr 
    mWriteBufReadyEvent[mCurrentQueue] = clCreateUserEvent(mOclAccel.context, &err1);
    if (mOclAccel.printError(err1,"Could not properly create write user event")) {
      mStatus = Error;
      return mStatus;
    }
    if (mWriteBufCopyFuncPtr) {
      (*mWriteBufCopyFuncPtr)(this,mCurrentQueue,mWriteBufReadyEvent[mCurrentQueue],userData);
    } else {
      clSetUserEventStatus(mWriteBufReadyEvent[mCurrentQueue],CL_COMPLETE);
    }
    if (mFirstWriteInQueue) {
      noOfBlockingEvents = 1;
      writeBlockingEventPtr = &(mWriteBufReadyEvent[mCurrentQueue]);
    } else {
      noOfBlockingEvents = 2;
      writeBlockingEvents[0] = mWriteEvent[mPrevQueue];
      writeBlockingEvents[1] =  mWriteBufReadyEvent[mCurrentQueue];
      writeBlockingEventPtr = writeBlockingEvents;
    }
  } else {
    if (!mOpFlags[DisableUserDataCopy])
      if (mWriteBufCopyFuncPtr)      
	(*mWriteBufCopyFuncPtr)(this,mCurrentQueue,NULL,userData); // this func must be in same thread
    if (!mFirstWriteInQueue) {// if not first, add prev queue blocking event
      noOfBlockingEvents = 1;
      writeBlockingEventPtr = &(mWriteEvent[mPrevQueue]);
    }
  }
  if (mFirstWriteInQueue)
    mFirstWriteInQueue = false;
  
  if (mOpFlags[UseEnqueueBuffers] == false ) {      
    err1 = clEnqueueMigrateMemObjects(mOclAccel.commands, 1, &(mWriteClMem[mCurrentQueue]), 0,noOfBlockingEvents, writeBlockingEventPtr, &(mWriteEvent[mCurrentQueue]));
    err2 = clEnqueueMigrateMemObjects(mOclAccel.commands, 1, &(mReadClMem[mCurrentQueue]), CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, noOfReadAllocBlockingEvents,readAllocBlockingEventPtr, &(mWriteMemReadAllocEvent[mCurrentQueue]));
  } else {
    err1 = clEnqueueWriteBuffer(mOclAccel.commands, mWriteClMem[mCurrentQueue], CL_FALSE, 0, mWriteBlockSize, mWriteBuf[mCurrentQueue], noOfBlockingEvents, writeBlockingEventPtr, &(mWriteEvent[mCurrentQueue]));
  }
  if (mOclAccel.printError(err1,"Failed to write to source array inbuf") )
    mStatus = Error;
  if (mOclAccel.printError(err2,"Failed to write to source array inbuf") )
    mStatus = Error;    
  return mStatus;
}

// Profiling logic 
bool XdbConn::getProfileData(bool printProfile) {
  if (mOpFlags[EnableProfiling] == false ) {
    printf("Info: No prfoile inforamtion available. Please enable profiling flag for profile data.\n");
    return false;
  }
  if (mOpFlags[UseEnqueueBuffers]) {
    printf("Info: Event profiling information with enqueuebufferusage can be inaccurate. Use memmigrate calls instead.\n");
    return false;
  }

  if ( finishCommands() == Error) {
    printf("Error: Could not complete all the enqueued commandsbefore profiling.a\n");    
  }

  unsigned long long nsTotalWriteDuration = 0, nsTotalReadDuration=0,nsTotalTaskDuration=0;
  unsigned int totalWrites= mWriteProfileData.size();
  if ((totalWrites != mReadProfileData.size()) || (totalWrites != mTaskProfileData.size())) {
    std::cout << "Error: Total writes "<< totalWrites << ", reads "<< mReadProfileData.size() << " and tasks "<< mTaskProfileData.size() << " must be same" << std::endl;
    return false;
  }
  
  if (printProfile) 
    std::cout << "PROFILE: ID  1  LABEL XDB MNK 4096 4096 4096" << std::endl;
  for (unsigned int i=0; i<totalWrites; i++) {
    nsTotalWriteDuration += (mWriteProfileData[i].mEndTime - mWriteProfileData[i].mStartTime);
    nsTotalReadDuration += (mReadProfileData[i].mEndTime - mReadProfileData[i].mStartTime);
    unsigned long long taskDuration = (mTaskProfileData[i].mEndTime - mTaskProfileData[i].mStartTime);
    nsTotalTaskDuration += taskDuration;
    if (printProfile) {
      std::cout<<"KERNEL k0 time " << taskDuration << " ns start " << mTaskProfileData[i].mStartTime << " end " << mTaskProfileData[i].mEndTime << std::endl;
      printProfileData(mWriteProfileData[i], "Write-0-ma", i , mWriteBlockSize);
      printProfileData(mReadProfileData[i], "Cread-0", i , mReadBlockSize);
      printProfileData(mTaskProfileData[i], "Kernel-0", i , mWriteBlockSize);
    }
  }
  
  double dmbytes = double(mWriteBlockSize)*double(totalWrites) / double(1024*1024);
  double dsduration = double(nsTotalWriteDuration)/double(1000000000);
  double wrdmbpersec = dmbytes/dsduration;

  dmbytes = double(mReadBlockSize)*double(totalWrites) / double(1024*1024);    
  dsduration = double(nsTotalReadDuration)/double(1000000000);
  double rddmbpersec = dmbytes/dsduration;
  
  double dmsduration = double(nsTotalTaskDuration)/double(1000000);

  printf("Appoximate Effective PCIe Bandwidth:  Write= %.3lf MB/sec Read= %.3lf MB/sec Total Kernel Time=%3lf ms\n", wrdmbpersec, rddmbpersec, dmsduration);
  
  return true;
}

bool XdbConn::saveProfileData(cl_event event, std::vector<ProfileData>& profileDataVec) {
  if (event == NULL)
    return false;
  ProfileData profileData;
  cl_int err = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_QUEUED, sizeof(unsigned long long), ((void *)(&(profileData.mQueuedTime))), NULL);
  err = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_SUBMIT, sizeof(unsigned long long), ((void *)(&(profileData.mSubmitTime))), NULL);
  err = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(unsigned long long), ((void *)(&(profileData.mStartTime))), NULL);  
  err = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END,    sizeof(unsigned long long), ((void *)(&(profileData.mEndTime))),NULL);
  if (err != CL_SUCCESS) {
    printf("Error: %d = %s Failed to run clGetEventProfilingInfo \n", err, mOclAccel.getErrorString(err));
    return false;
  }
  profileDataVec.push_back(profileData);
  return true;
}

void XdbConn::printProfileData(ProfileData profileData, const char* id, int qno, int blockSize) {
  std::cout << "PROFILE: " << profileData.mQueuedTime << " " << (qno+1) << " " << id << "-queued  " << blockSize << std::endl;
  std::cout << "PROFILE: " << profileData.mSubmitTime << " " << (qno+1) << " " << id << "-submit  " << blockSize << std::endl;
  std::cout << "PROFILE: " << profileData.mStartTime << " " << (qno+1) << " " << id << "-start  " << blockSize << std::endl;
  std::cout << "PROFILE: " << profileData.mEndTime << " " << (qno+1) << " " << id << "-end  " << blockSize << std::endl;
}
    
void XdbConn::eventStatus(cl_event event) {
  if(!event) {
    printf("Null Event\n");
    return;
  }

  int rdy;
  clGetEventInfo(event, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), &rdy, NULL);
  switch(rdy) {
  case CL_COMPLETE: {
    printf("CL_COMPLETE\n");
    break;
  }
  case CL_QUEUED: {
    printf("CL_QUEUED\n");
    break;
  }
  case CL_SUBMITTED: {
    printf("CL_SUBMITTED\n");
    break;
  }
  case CL_RUNNING: {
    printf("CL_RUNNING\n");
    break;
  }
  default: {
    printf("Error Event\n");
    break;
  }
  }

}
