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
