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
