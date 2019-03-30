/**********

Copyright 2019 Xilinx, Inc.

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
#include "oclaccel.h"
#include <cstdio>

//extern char **environ;
cl_context       OclAccel::context;             // compute context
cl_program       OclAccel::program;             // compute program
cl_platform_id   OclAccel::platformId;          // platform id  
cl_device_id     OclAccel::deviceId;            // compute device id
std::string      OclAccel::xclbin;
std::string      OclAccel::targetDeviceName;
std::mutex       OclAccel::resourceMutex;
OclAccel::Status OclAccel::connectStatus = NotInitialized;
int              OclAccel::noOfQueues = 0;
std::unordered_map<cl_int, const char*> OclAccel::oclErrorCodes;
int OclAccel::loadFileToMemory(const char *filename, char **result) { 
  size_t size = 0;
  FILE *f = fopen(filename, "rb");
  if (f == NULL) { 
    *result = NULL;
    return -1; // -1 means file opening fail 
  } 
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);
  *result = (char *)malloc(size+1);
  if (size != fread(*result, sizeof(char), size, f)) { 
    //printf ("SREE: calling free on result\n");
    fflush(stdout);
    free(*result);
    return -2; // -2 means file reading fail 
  } 
  fclose(f);
  (*result)[size] = 0;
  return size;
}

void OclAccel::printClLog() {
  // Determine the size of the log
  size_t log_size;
  clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

  // Allocate memory for the log
  char *log = (char *) malloc(log_size);

  // Get the log
  clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

  // Print the log
  printf("%s\n", log);
}

void OclAccel::init() {

  // From oclError
  oclErrorCodes.insert(std::make_pair(CL_SUCCESS, "CL_SUCCESS"));
  oclErrorCodes.insert(std::make_pair(CL_DEVICE_NOT_FOUND, "CL_DEVICE_NOT_FOUND"));
  oclErrorCodes.insert(std::make_pair(CL_DEVICE_NOT_AVAILABLE, "CL_DEVICE_NOT_AVAILABLE"));
  oclErrorCodes.insert(std::make_pair(CL_COMPILER_NOT_AVAILABLE, "CL_COMPILER_NOT_AVAILABLE"));
  oclErrorCodes.insert(std::make_pair(CL_MEM_OBJECT_ALLOCATION_FAILURE, "CL_MEM_OBJECT_ALLOCATION_FAILURE"));
  oclErrorCodes.insert(std::make_pair(CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES"));
  oclErrorCodes.insert(std::make_pair(CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY"));
  oclErrorCodes.insert(std::make_pair(CL_PROFILING_INFO_NOT_AVAILABLE, "CL_PROFILING_INFO_NOT_AVAILABLE"));
  oclErrorCodes.insert(std::make_pair(CL_MEM_COPY_OVERLAP, "CL_MEM_COPY_OVERLAP"));
  oclErrorCodes.insert(std::make_pair(CL_IMAGE_FORMAT_MISMATCH, "CL_IMAGE_FORMAT_MISMATCH"));
  oclErrorCodes.insert(std::make_pair(CL_IMAGE_FORMAT_NOT_SUPPORTED, "CL_IMAGE_FORMAT_NOT_SUPPORTED"));
  oclErrorCodes.insert(std::make_pair(CL_BUILD_PROGRAM_FAILURE, "CL_BUILD_PROGRAM_FAILURE"));
  oclErrorCodes.insert(std::make_pair(CL_MAP_FAILURE, "CL_MAP_FAILURE"));
  oclErrorCodes.insert(std::make_pair(CL_MISALIGNED_SUB_BUFFER_OFFSET, "CL_MISALIGNED_SUB_BUFFER_OFFSET"));
  oclErrorCodes.insert(std::make_pair(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST, "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_W"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_VALUE, "CL_INVALID_VALUE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_DEVICE_TYPE, "CL_INVALID_DEVICE_TYPE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_PLATFORM, "CL_INVALID_PLATFORM"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_DEVICE, "CL_INVALID_DEVICE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_CONTEXT, "CL_INVALID_CONTEXT"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_QUEUE_PROPERTIES, "CL_INVALID_QUEUE_PROPERTIES"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_COMMAND_QUEUE, "CL_INVALID_COMMAND_QUEUE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_HOST_PTR, "CL_INVALID_HOST_PTR"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_MEM_OBJECT, "CL_INVALID_MEM_OBJECT"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR, "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_IMAGE_SIZE, "CL_INVALID_IMAGE_SIZE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_SAMPLER, "CL_INVALID_SAMPLER"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_BINARY, "CL_INVALID_BINARY"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_BUILD_OPTIONS, "CL_INVALID_BUILD_OPTIONS"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_PROGRAM, "CL_INVALID_PROGRAM"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_PROGRAM_EXECUTABLE, "CL_INVALID_PROGRAM_EXECUTABLE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_KERNEL_NAME, "CL_INVALID_KERNEL_NAME"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_KERNEL_DEFINITION, "CL_INVALID_KERNEL_DEFINITION"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_KERNEL, "CL_INVALID_KERNEL"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_ARG_INDEX, "CL_INVALID_ARG_INDEX"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_ARG_VALUE, "CL_INVALID_ARG_VALUE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_ARG_SIZE, "CL_INVALID_ARG_SIZE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_KERNEL_ARGS, "CL_INVALID_KERNEL_ARGS"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_WORK_DIMENSION, "CL_INVALID_WORK_DIMENSION"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_WORK_GROUP_SIZE, "CL_INVALID_WORK_GROUP_SIZE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_WORK_ITEM_SIZE, "CL_INVALID_WORK_ITEM_SIZE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_GLOBAL_OFFSET, "CL_INVALID_GLOBAL_OFFSET"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_EVENT_WAIT_LIST, "CL_INVALID_EVENT_WAIT_LIST"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_EVENT, "CL_INVALID_EVENT"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_OPERATION, "CL_INVALID_OPERATION"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_GL_OBJECT, "CL_INVALID_GL_OBJECT"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_BUFFER_SIZE, "CL_INVALID_BUFFER_SIZE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_MIP_LEVEL, "CL_INVALID_MIP_LEVEL"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_GLOBAL_WORK_SIZE, "CL_INVALID_GLOBAL_WORK_SIZE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_PROPERTY, "CL_INVALID_PROPERTY"));

  // there are repeated errors, no harm but unnecessary remvoe it later
  // run-time and JIT compiler errors
  oclErrorCodes.insert(std::make_pair(CL_SUCCESS,"CL_SUCCESS"));
  oclErrorCodes.insert(std::make_pair(CL_DEVICE_NOT_FOUND,"CL_DEVICE_NOT_FOUND"));
  oclErrorCodes.insert(std::make_pair(CL_DEVICE_NOT_AVAILABLE,"CL_DEVICE_NOT_AVAILABLE"));
  oclErrorCodes.insert(std::make_pair(CL_COMPILER_NOT_AVAILABLE,"CL_COMPILER_NOT_AVAILABLE"));
  oclErrorCodes.insert(std::make_pair(CL_MEM_OBJECT_ALLOCATION_FAILURE,"CL_MEM_OBJECT_ALLOCATION_FAILURE"));
  oclErrorCodes.insert(std::make_pair(CL_OUT_OF_RESOURCES,"CL_OUT_OF_RESOURCES"));
  oclErrorCodes.insert(std::make_pair(CL_OUT_OF_HOST_MEMORY,"CL_OUT_OF_HOST_MEMORY"));
  oclErrorCodes.insert(std::make_pair(CL_PROFILING_INFO_NOT_AVAILABLE,"CL_PROFILING_INFO_NOT_AVAILABLE"));
  oclErrorCodes.insert(std::make_pair(CL_MEM_COPY_OVERLAP,"CL_MEM_COPY_OVERLAP"));
  oclErrorCodes.insert(std::make_pair(CL_IMAGE_FORMAT_MISMATCH,"CL_IMAGE_FORMAT_MISMATCH"));
  oclErrorCodes.insert(std::make_pair(CL_IMAGE_FORMAT_NOT_SUPPORTED,"CL_IMAGE_FORMAT_NOT_SUPPORTED"));
  oclErrorCodes.insert(std::make_pair(CL_BUILD_PROGRAM_FAILURE,"CL_BUILD_PROGRAM_FAILURE"));
  oclErrorCodes.insert(std::make_pair(CL_MAP_FAILURE,"CL_MAP_FAILURE"));
  oclErrorCodes.insert(std::make_pair(CL_MISALIGNED_SUB_BUFFER_OFFSET,"CL_MISALIGNED_SUB_BUFFER_OFFSET"));
  oclErrorCodes.insert(std::make_pair(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST,"CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST"));
  oclErrorCodes.insert(std::make_pair(CL_COMPILE_PROGRAM_FAILURE,"CL_COMPILE_PROGRAM_FAILURE"));
  oclErrorCodes.insert(std::make_pair(CL_LINKER_NOT_AVAILABLE,"CL_LINKER_NOT_AVAILABLE"));
  oclErrorCodes.insert(std::make_pair(CL_LINK_PROGRAM_FAILURE,"CL_LINK_PROGRAM_FAILURE"));
  oclErrorCodes.insert(std::make_pair(CL_DEVICE_PARTITION_FAILED,"CL_DEVICE_PARTITION_FAILED"));
  oclErrorCodes.insert(std::make_pair(CL_KERNEL_ARG_INFO_NOT_AVAILABLE,"CL_KERNEL_ARG_INFO_NOT_AVAILABLE"));

  // compile-time errors
  oclErrorCodes.insert(std::make_pair(CL_INVALID_VALUE,"CL_INVALID_VALUE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_DEVICE_TYPE,"CL_INVALID_DEVICE_TYPE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_PLATFORM,"CL_INVALID_PLATFORM"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_DEVICE,"CL_INVALID_DEVICE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_CONTEXT,"CL_INVALID_CONTEXT"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_QUEUE_PROPERTIES,"CL_INVALID_QUEUE_PROPERTIES"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_COMMAND_QUEUE,"CL_INVALID_COMMAND_QUEUE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_HOST_PTR,"CL_INVALID_HOST_PTR"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_MEM_OBJECT,"CL_INVALID_MEM_OBJECT"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR,"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_IMAGE_SIZE,"CL_INVALID_IMAGE_SIZE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_SAMPLER,"CL_INVALID_SAMPLER"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_BINARY,"CL_INVALID_BINARY"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_BUILD_OPTIONS,"CL_INVALID_BUILD_OPTIONS"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_PROGRAM,"CL_INVALID_PROGRAM"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_PROGRAM_EXECUTABLE,"CL_INVALID_PROGRAM_EXECUTABLE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_KERNEL_NAME,"CL_INVALID_KERNEL_NAME"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_KERNEL_DEFINITION,"CL_INVALID_KERNEL_DEFINITION"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_KERNEL,"CL_INVALID_KERNEL"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_ARG_INDEX,"CL_INVALID_ARG_INDEX"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_ARG_VALUE,"CL_INVALID_ARG_VALUE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_ARG_SIZE,"CL_INVALID_ARG_SIZE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_KERNEL_ARGS,"CL_INVALID_KERNEL_ARGS"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_WORK_DIMENSION,"CL_INVALID_WORK_DIMENSION"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_WORK_GROUP_SIZE,"CL_INVALID_WORK_GROUP_SIZE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_WORK_ITEM_SIZE,"CL_INVALID_WORK_ITEM_SIZE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_GLOBAL_OFFSET,"CL_INVALID_GLOBAL_OFFSET"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_EVENT_WAIT_LIST,"CL_INVALID_EVENT_WAIT_LIST"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_EVENT,"CL_INVALID_EVENT"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_OPERATION,"CL_INVALID_OPERATION"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_GL_OBJECT,"CL_INVALID_GL_OBJECT"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_BUFFER_SIZE,"CL_INVALID_BUFFER_SIZE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_MIP_LEVEL,"CL_INVALID_MIP_LEVEL"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_GLOBAL_WORK_SIZE,"CL_INVALID_GLOBAL_WORK_SIZE"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_PROPERTY,"CL_INVALID_PROPERTY"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_IMAGE_DESCRIPTOR,"CL_INVALID_IMAGE_DESCRIPTOR"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_COMPILER_OPTIONS,"CL_INVALID_COMPILER_OPTIONS"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_LINKER_OPTIONS,"CL_INVALID_LINKER_OPTIONS"));
  oclErrorCodes.insert(std::make_pair(CL_INVALID_DEVICE_PARTITION_COUNT,"CL_INVALID_DEVICE_PARTITION_COUNT"));

  // extension errors
  oclErrorCodes.insert(std::make_pair(CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR,"CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR"));
  oclErrorCodes.insert(std::make_pair(CL_PLATFORM_NOT_FOUND_KHR,"CL_PLATFORM_NOT_FOUND_KHR"));

}

const char *OclAccel::getErrorString(cl_int code) {
  auto iter = oclErrorCodes.find(code);
  if (iter == oclErrorCodes.end())
    return "UNKNOWN ERROR";
  return iter->second;
}
			 

bool OclAccel::printError(cl_int err, const char *errStr) {
  if (err != CL_SUCCESS) {
    printf("ERROR: %s CL error code %d=%s \n",errStr,err,getErrorString(err));
    return true;
  }
  return false;
}

// Connect to a compute device
// find all devices and then select the target device, a xilinx device here..
OclAccel::Status OclAccel::connectPlatform(const char *deviceName, const char* kernelName, const char *xclb)  {
  std::lock_guard<std::mutex> guard(resourceMutex);
  mStatus = Normal;
  if (connectStatus == NotInitialized) {
    targetDeviceName = deviceName;
    createContext();
    if (mStatus == Error) {
      printf("ERROR: Failed to create context for the given device\n");
      connectStatus = Error;
      return mStatus;
    }
    createProgram(xclb);
    if (mStatus == Error) {
      printf("ERROR: Failed to create program for the given device\n");
      connectStatus = Error;      
      return mStatus;
    }
    connectStatus = Normal;
  }
  if (connectStatus == Error) {
    mStatus = Error;
    printf("ERROR: Failed to create context and program for the given device\n");    
  }

  cl_int err;
  // Create the compute kernel in the program we wish to run
  kernel = clCreateKernel(program, kernelName, &err);
  if (!kernel || err != CL_SUCCESS) {
    printError(err,"Failed to create compute kernel.");
    mStatus = Error;
    return mStatus;
  }
  
  // Create a command commands
  //
  //commands = clCreateCommandQueue(context, deviceId, commandQueueOrderProp, &err);
// commands = clCreateCommandQueue(context, deviceId, CL_QUEUE_PROFILING_ENABLE, &err);
commands = clCreateCommandQueue(context, deviceId, CL_QUEUE_PROFILING_ENABLE|CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);

  if (!commands || (err != CL_SUCCESS)) {
    printError(err,"Failed to create command queue");
    mStatus = Error;
    return mStatus;
  }
  
  noOfQueues++;
  return mStatus;
}

OclAccel::Status OclAccel::createContext() {
  cl_platform_id platforms[16];       // platform id
  cl_uint platform_count;
  char cl_platform_vendor[1001];
  cl_int err;

  // Get all platforms and then select Xilinx platform
  err = clGetPlatformIDs(16, platforms, &platform_count);
  if (printError(err,"Failed to find an OpenCL platform")) {
    mStatus = Error;
    return mStatus;
  }
  printf("INFO: Found %d platforms\n", platform_count);

  // Find Xilinx Plaftorm
  int platform_found = 0;
  for (unsigned int iplat=0; iplat<platform_count; iplat++) {
    err = clGetPlatformInfo(platforms[iplat], CL_PLATFORM_VENDOR, 1000, (void *)cl_platform_vendor,NULL);
    if (printError(err,"clGetPlatformInfo(CL_PLATFORM_VENDOR) failed.")) {
      mStatus = Error;
      return mStatus;
    }
    if (strcmp(cl_platform_vendor, "Xilinx") == 0) {
      printf("INFO: Selected platform %d from %s\n", iplat, cl_platform_vendor);
      platformId = platforms[iplat];
      platform_found = 1;
    }
  }
  if (!platform_found) {
    printError(err,"Platform Xilinx not found.");
    mStatus = Error;
    return mStatus;
  }

  // Get device ID 
  cl_device_id devices[16];  // compute device id 
  cl_uint device_count;
  //unsigned int device_found = 0;
  //char cl_device_name[1001];
  
  err = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ACCELERATOR,
		       16, devices, &device_count);
  if (printError(err,"Failed to create a device group.")) {
    mStatus = Error;
    return mStatus;
  }
/*int i = 1;
  char *s = *environ;

  for (; s; i++) {
    printf("%s\n", s);
    s = *(environ+i);
  }*/

  //iterate all devices to select the target device. 
  /*for (unsigned i=0; i<device_count; i++) {
    err = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 1024, cl_device_name, 0);
    if (err != CL_SUCCESS) {
      printf("ERROR: Failed to get device name for device %d!\n", i);
      printError(err,"Failed to get a device name.");
      mStatus = Error;
      return mStatus;
    }

    printf("CL_DEVICE_NAME %s\n", cl_device_name);
    if(targetDeviceName.compare(cl_device_name) == 0) {
      deviceId = devices[i];
      device_found = 1;
      printf("INFO: Selected %s as the target device\n", cl_device_name);
    }
  }
    
  if (!device_found) {
    printf("ERROR: Target device %s not found. Exit.\n", targetDeviceName.c_str());
    mStatus = Error;
    return mStatus;
  }*/

  err = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ACCELERATOR,
		       1, &deviceId, NULL);
  if (printError(err, "Failed to create a device group")) {
    mStatus = Error;
    return mStatus;
  }

  // Create a compute context 
  //
  context = clCreateContext(0, 1, &deviceId, NULL, NULL, &err);
  if (!context || (err != CL_SUCCESS)) {
    printError(err,"Failed to create a compute context");
    mStatus = Error;
    return mStatus;
  }

  return mStatus;
}

// create a program with the given xclbin
OclAccel::Status OclAccel::createProgram(const char *xclb) {

  xclbin = xclb;
  cl_int err;
  /*int i = 1;
  char *s = *environ;

  for (; s; i++) {
    printf("%s\n", s);
    s = *(environ+i);
    }*/
  
  // Create Program Objects
  //
  // Load binary from disk
  unsigned char *kernelbinary;
  printf("INFO: Loading %s\n", xclbin.c_str());
  int n_i = loadFileToMemory(xclbin.c_str(), (char **) &kernelbinary);
  if (n_i < 0) {
    printf("ERROR: Failed to load kernel from xclbin: %s\n", xclbin.c_str());
    mStatus = Error;
    return mStatus;
  }
  size_t n = n_i;
  int status;
  // Create the compute program from offline
  program = clCreateProgramWithBinary(context, 1, &deviceId, &n,
				      (const unsigned char **) &kernelbinary, &status, &err);
  if ((!program) || (err!=CL_SUCCESS)) {
    printError(err,"Failed to create compute program from binary");
    mStatus = Error;
    return mStatus;
  }

  // Build the program executable
  //
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS) {
    size_t len;
    char buffer[2048];
    clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    printError(err,"Failed to build program executable ");
    mStatus = Error;
    return mStatus;
  }
  //printClLog(program,deviceId);
  
  return mStatus;

}

void OclAccel::clear() {
  std::lock_guard<std::mutex> guard(resourceMutex);
  finishCommands();
  clReleaseCommandQueue(commands);
  clReleaseKernel(kernel);
  noOfQueues--;
  if (noOfQueues == 0) { // Last command queue
    clReleaseProgram(program);
    clReleaseContext(context);
  }
  mStatus = Normal;
}

OclAccel::Status OclAccel::finishCommands() {
  cl_int err = clFinish(commands);
  if (printError(err,"Failed to complete commands") )
    mStatus = Error;
  return mStatus;
}
