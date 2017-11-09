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
#ifndef XDBCONN_OCLACCEL_H
#define XDBCONN_OCLACCEL_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unordered_map>
#include <CL/opencl.h>
#include <mutex>

class OclAccel {
 public:
  
  enum Status {NotInitialized=0, Normal, Error};

  void init();

  Status connectPlatform(const char *deviceName, const char* kernelName, const char *xclb);
  void clear();
  Status finishCommands();
  
  bool printError(cl_int err, const char *errString);
  const char *getErrorString(cl_int code);
  
  cl_command_queue commands;           // compute command queue
  // 0 if in order, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE for out of order queue
  // CL_QUEUE_PROFILING_ENABLE if profiling needed
  cl_command_queue_properties commandQueueOrderProp;
  cl_kernel                   kernel;              // compute kernel
  
  static cl_context       context;             // compute context
  static cl_program       program;             // compute program
  static cl_platform_id   platformId;          // platform id  
  static cl_device_id     deviceId;            // compute device id
  static std::string      xclbin;
  static std::string      targetDeviceName;
  static std::mutex       resourceMutex;  
  static Status           connectStatus;
  static int              noOfQueues;

  Status                   mStatus;
  
 private:
  static int loadFileToMemory(const char *filename, char **result);
  void printClLog();
  Status createContext();
  Status createProgram(const char *xclb);
  std::unordered_map<cl_int, const char*> oclErrorCodes;
  
};

#endif
