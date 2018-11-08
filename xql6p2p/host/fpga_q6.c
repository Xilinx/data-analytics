//----------------------------------------------------------------------------
// OCL wrappers for xql6.
//
// 2017-06-16 19:18:04 parik
//----------------------------------------------------------------------------
#include <stdio.h>
#include <CL/opencl.h>
#include <unistd.h>

#include "xql6.h"

//----------------------------------------------------------------------------
extern void hlog (const char* fmt, ...);
extern int g_devid;

//----------------------------------------------------------------------------

int open_fpga (cl_context *context, cl_command_queue *commands, 
               cl_program *program, cl_kernel *kernel, const char *xclbin);
int close_fpga (cl_context *context, cl_command_queue *commands, 
               cl_program *program, cl_kernel *kernel);

//----------------------------------------------------------------------------

static cl_context gcontext;                 // compute context
static cl_command_queue gcommands;          // compute command queue
static cl_program gprogram;                 // compute program
static cl_kernel gkernel;                   // compute kernel

// device memory. NPIPES buffers are created to allow multiple overlapping
// operations to hide transfer latencies.
#if 1
#define NPIPES 10
#else
#define NPIPES 20
#endif
static cl_mem devInp [NPIPES];
static cl_mem devOut [NPIPES];

// events for write, ndrangekernel and read
cl_event wrEvent [NPIPES];
cl_event ndEvent [NPIPES];
cl_event rdEvent [NPIPES];

// host pointers to access device memory
unsigned char* pInp[NPIPES];
unsigned char* pOut[NPIPES];

//----------------------------------------------------------------------------

/*
 * \brief Creates OpenCL kernel in FPGA, allocate device memory buffers
 * and map to host access pointers
 * \param xclbin Name of xclbin file from SDAccel
 * \return Standard C return codes for success/failure
 */
int start_fpga (const char *xclbin) {

  cl_mem_ext_ptr_t input_buffer_ext;

  if (open_fpga (&gcontext, &gcommands, &gprogram, &gkernel, xclbin) != EXIT_SUCCESS) {
    hlog ("ERROR: FPGA Open failed\n");
    return EXIT_FAILURE;
  }

  hlog ("fpga_q6 clCreateBuffer\n");
  int i;
  for (i=0; i<NPIPES; ++i) {
    // alloc device memory buffers
    input_buffer_ext.flags = XCL_MEM_EXT_P2P_BUFFER;
    input_buffer_ext.obj = NULL;
    input_buffer_ext.param = 0;
    devInp[i] = clCreateBuffer (gcontext, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, XQL6_BLK_SZ, &input_buffer_ext, NULL);
    devOut[i] = clCreateBuffer (gcontext, CL_MEM_WRITE_ONLY, sizeof(long) * XQL_NUM_RES_WORDS, NULL, NULL);

    if (!devInp[i] || !devOut[i]) {
      printf("ERROR: Failed to allocate device memory Buf %d\n", i);
      return EXIT_FAILURE;
    }

    // map device buffers to host access pointers
    pInp[i] = (unsigned char*) clEnqueueMapBuffer (gcommands, devInp[i], CL_TRUE, CL_MAP_WRITE, 0,
            XQL6_BLK_SZ, 0, NULL, NULL, NULL);
    pOut[i] = (unsigned char*) clEnqueueMapBuffer (gcommands, devOut[i], CL_TRUE, CL_MAP_READ, 0,
            sizeof(long) * XQL_NUM_RES_WORDS, 0, NULL, NULL, NULL);

    if (!pInp[i] || !pOut[i]) {
      hlog ("ERROR: Failed to allocate host memory for Buf %d\n", i);
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

/*
 * \brief Close OpenCL FPGA context and release all buffers
 */
int end_fpga() {
  int i;
  for (i=0; i<NPIPES; ++i) {
    int err;
    err = clEnqueueUnmapMemObject (gcommands, devInp[i], pInp[i], 0, NULL, NULL);
    err = clEnqueueUnmapMemObject (gcommands, devOut[i], pOut[i], 0, NULL, NULL);
    clReleaseMemObject (devInp[i]);
    clReleaseMemObject (devOut[i]);
    devInp[i] = 0; devOut[i] = 0; pInp[i]=0; pOut[i]=0;
  }

  if(close_fpga(&gcontext, &gcommands, &gprogram, &gkernel) != EXIT_SUCCESS) {
    hlog("ERROR: FPGA Close failed\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/*
 *
 */
int fpga_q6_enq_compute (int bufId)
{
  if (0) hlog ("fpga_q6_enq_compute bufId %d start...\n", bufId);

  int err = clEnqueueMigrateMemObjects (gcommands, 1, &devInp[bufId], 0, 0, NULL, &wrEvent[bufId]);
  if (err != CL_SUCCESS) {
    hlog("ERROR: Failed to write to devInp[%d]. Status %d!\n", bufId, err);
    return EXIT_FAILURE;
  }
  if (0) hlog ("fpga_q6_enq_compute clEnqueueWriteBuffer\n");

  err  = clSetKernelArg(gkernel, 0, sizeof(cl_mem), &devInp[bufId]);
  err |= clSetKernelArg(gkernel, 1, sizeof(cl_mem), &devOut[bufId]);
  if (err != CL_SUCCESS) {
    hlog("ERROR: Failed to set kernel arguments! %d\n", err);
    return EXIT_FAILURE;
  }
  if (0) hlog ("fpga_q6_enq_compute clSetKernelArg\n");

  size_t global[1], local[1];
  global[0] = 1; local[0] = 1;

  err = clEnqueueNDRangeKernel (gcommands, gkernel, 1, NULL, (size_t*)&global, (size_t*)&local,
                                0, NULL, &ndEvent[bufId]);

  if (err) {
    hlog("ERROR: Failed to execute kernel! %d\n", err);
    return EXIT_FAILURE;
  }
  if (0) hlog ("fpga_q6_enq_compute clEnqueueNDRangeKernel\n");

  err = clEnqueueMigrateMemObjects(gcommands, 1, &devOut[bufId], 
          CL_MIGRATE_MEM_OBJECT_HOST, 1, &ndEvent[bufId], &rdEvent[bufId]);
  if (err) {
    printf("ERROR: Failed to clEnqueueMigrateMemObjects to host! %d\n", err);
    return EXIT_FAILURE;
  }

  clRetainEvent (rdEvent[bufId]);

  clReleaseEvent (ndEvent [bufId]);
  clReleaseEvent (rdEvent [bufId]);

  if (0) hlog ("fpga_q6_enq_compute done\n");
  return EXIT_SUCCESS;
}

static int load_file_to_memory(const char *filename, char **result)
{ 
  int size = 0;
  FILE *f = fopen(filename, "rb");
  if (f == NULL) 
  { 
    *result = NULL;
    return -1; // -1 means file opening fail 
  } 
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);
  *result = (char *)malloc(size+1);
  if (size != fread(*result, sizeof(char), size, f)) 
  { 
    free(*result);
    return -2; // -2 means file reading fail 
  } 
  fclose(f);
  (*result)[size] = 0;
  return size;
}

int open_fpga (cl_context *context, cl_command_queue *commands, cl_program *program, 
               cl_kernel *kernel, const char *xclbin) {
  cl_platform_id platform_id;         // platform id
  cl_device_id device_id;             // compute device id 
  cl_device_id * devices;

  char cl_platform_vendor[1001];
  char cl_platform_name[1001];

  // Load binary from disk
  unsigned char *kernelbinary;

  hlog("loading %s\n", xclbin);
  int n_i = load_file_to_memory(xclbin, (char **) &kernelbinary);
  if (n_i < 0) {
    hlog("failed to load kernel from xclbin: %s\n", xclbin);
    hlog("ERROR: Test failed\n");
    return EXIT_FAILURE;
  }

  // Connect to first platform
  //
  int err = clGetPlatformIDs(1,&platform_id,NULL);
  if (err != CL_SUCCESS)
  {
    hlog("ERROR: Failed to find an OpenCL platform %d!\n", err);
    hlog("ERROR: Test failed\n");
    return EXIT_FAILURE;
  }
  err = clGetPlatformInfo(platform_id,CL_PLATFORM_VENDOR,1000,(void *)cl_platform_vendor,NULL);
  if (err != CL_SUCCESS)
  {
    hlog("ERROR: clGetPlatformInfo(CL_PLATFORM_VENDOR) failed %d!\n", err);
    hlog("ERROR: Test failed\n");
    return EXIT_FAILURE;
  }
  hlog("CL_PLATFORM_VENDOR %s\n",cl_platform_vendor);
  err = clGetPlatformInfo(platform_id,CL_PLATFORM_NAME,1000,(void *)cl_platform_name,NULL);
  if (err != CL_SUCCESS)
  {
    hlog("ERROR: clGetPlatformInfo(CL_PLATFORM_NAME) failed!\n");
    hlog("ERROR: Test failed\n");
    return EXIT_FAILURE;
  }
  hlog("CL_PLATFORM_NAME %s\n",cl_platform_name);
  
  // Connect to a compute device
  //
  int fpga = 0;
//#if defined (FLOW_ZYNQ_HLS_BITSTREAM) || defined(FLOW_HLS_CSIM) || defined(FLOW_HLS_COSIM)
  fpga = 1;

  cl_uint num_devices = 0;
  err = clGetDeviceIDs(platform_id, fpga ? CL_DEVICE_TYPE_ACCELERATOR : CL_DEVICE_TYPE_CPU,
                       0, NULL, &num_devices);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to create a device group!\n");
    hlog("Error: Failed to create a device group!\n");
    printf("Test failed\n");
    return EXIT_FAILURE;
  }
  hlog("num %d devices\n", num_devices);
  if (g_devid >= num_devices) {
    hlog("ERROR: device out of range; device id %d: num devices %d\n", g_devid, num_devices);
    hlog("ERROR: Test failed\n");
    return EXIT_FAILURE;
  }
  devices = (cl_device_id *)malloc(num_devices*sizeof(cl_device_id));

  err = clGetDeviceIDs(platform_id, fpga ? CL_DEVICE_TYPE_ACCELERATOR : CL_DEVICE_TYPE_CPU,
                       num_devices, devices, NULL);
  if (err != CL_SUCCESS)
  {
    printf("ERROR: Failed to create a device group!\n");
    return EXIT_FAILURE;
  }

  hlog("Got %d devices\n", num_devices);
  device_id = devices[g_devid];
  hlog("assigned device %d: %d devices\n", g_devid, num_devices);

  // Create a compute context 
  //
  *context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
  if (!*context)
  {
    hlog("ERROR: Failed to create a compute context!\n");
    hlog("ERROR: Test failed\n");
    return EXIT_FAILURE;
  }

  // Create a command commands
  //
  *commands = clCreateCommandQueue(*context, device_id, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
  if (!*commands)
  {
    hlog("ERROR: Failed to create a command commands!\n");
    hlog("ERROR: code %i\n",err);
    hlog("ERROR: Test failed\n");
    return EXIT_FAILURE;
  }

  int status;

  size_t n = n_i;
  // Create the compute program from offline
  *program = clCreateProgramWithBinary(*context, 1, &device_id, &n,
                                      (const unsigned char **) &kernelbinary, &status, &err);
  if ((!program) || (err!=CL_SUCCESS)) {
    hlog("ERROR: Failed to create compute program from binary %d!\n", err);
    hlog("ERROR: Test failed\n");
    return EXIT_FAILURE;
  }
  hlog ("clCreateProgramWithBinary successful\n");

  // Build the program executable
  //
  err = clBuildProgram(*program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS)
  {
    size_t len;
    char buffer[2048];

    hlog("ERROR: Failed to build program executable!\n");
    clGetProgramBuildInfo(*program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    hlog("%s\n", buffer);
    hlog("ERROR: Test failed\n");
    return EXIT_FAILURE;
  }
  hlog ("clBuildProgram successful\n");

  // Create the compute kernel in the program we wish to run
  //
  *kernel = clCreateKernel(*program, "fpga_q6", &err);
  if (!*kernel || err != CL_SUCCESS)
  {
    hlog("ERROR: Failed to create compute kernel!\n");
    hlog("ERROR: Test failed\n");
    return EXIT_FAILURE;
  }

  hlog ("open_fpga successful\n");
  return EXIT_SUCCESS;
}

int close_fpga (cl_context *context, cl_command_queue *commands, cl_program *program, cl_kernel *kernel) {
  clReleaseProgram(*program);
  clReleaseKernel(*kernel);
  clReleaseCommandQueue(*commands);
  clReleaseContext(*context);

  return EXIT_SUCCESS;
}
