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


#include "xpg_fpga_csim.h"
#include "xpg_fpga_defs.h"

#include "xpgbolt.h"
#include "xpgdatastore.h"
#include "stdlib.h"
#include "stdio.h"
#include "sqlcmds.h"
#ifdef HW_INTEGRATION
#include "xdbutils.h"
#include "xdbconn.h"
#include "oclaccel.h"
#include <CL/opencl.h>
#endif
#include <unistd.h>

char* xpg_setupstream = NULL;
char* xpg_datastream = NULL;
#ifdef HW_INTEGRATION
int kernels = 1;
XdbConn **p_xdbConns = new XdbConn*[kernels];
char *FpgaTargetDeviceName = "xilinx:adm-pcie-ku3:2ddr-xpr:3.3";
//char* kernelName = "scan0";
char* kernelName = "fpga_sql";
#endif

#ifdef HW_INTEGRATION
void xpg_init_fpga_defs(char * xclbinPath) //(int query, char * setupbuf_HW)
{
  //Initialize the xdbconn
  int readBlockSize = QDefs::ScanReadBlockSize;
  int writeBlockSize = QDefs::WriteBlockSize;
  //std::string xclBinName = "/home/visut/pgsql/scan_KU3_hw.xclbin";
  //std::string xclBinName = "/wrk/xsjhdnobkup1/sreesan/pgx/Proj/O/OPENCL_APPS_DEV/src/database/xql6/xpgFilter/fpga_q6_sw_emu.xclbin";
  //std::string xclBinName = "/home/yifanz/xclbin/fpga_q6_sw_emu.xclbin";
  //std::string xclBinName = "/home/visut/pgsql/fpga_q6_sw_emu.xclbin";
  //std::string xclBinName = "/wrk/xsjhdnobkup1/sreesan/pgx2/Proj/O/OPENCL_APPS_DEV/src/database/xpg_filter_scan_aggr/fpga_q6_sw_emu.xclbin";

  std::string xclBinName = std::string(xclbinPath);
  for (int i =0; i< kernels; i++)
    p_xdbConns[i] = XdbConn::create(XdbConnQueDepth,writeBlockSize,readBlockSize);
  std::cout << "Done creating xdbconn\n";
  for (int i =0; i< kernels; i++) {
    XdbConn::Status oclStatus = p_xdbConns[i]-> init();
    if (oclStatus == XdbConn::Error) {
      std::cout << "Error: Failed to initialize xdbconn for kernel " << i << std::endl;
      exit(1);
    }
  }
  std::cout << "Done Initializing xdbconn\n";

  for (int i =0; i< kernels; i++) {
    XdbConn::Status oclStatus = p_xdbConns[i]->connectPlatform (FpgaTargetDeviceName, kernelName, xclBinName.c_str());
    if (oclStatus == XdbConn::Error) {
      std::cout << "Error: Failed to get FPGA compute platform" << std::endl;
      exit(1);
    }

    oclStatus = p_xdbConns[i]->createAllBufs();
    if (oclStatus == XdbConn::Error) {
      std::cout << "Error: Failed to get CL Mem Buffers" << std::endl;
      exit(1);
    }
  }
  std::cout << "Done Connecting platform and createBufs\n";


  /* Init Buffers in xpg_buffer_container for every queue */

  //xpg_buffer_container.list = 
    //(XpgBufferSet*) palloc0(sizeof(XpgBufferSet) * XdbConnQueDepth);

  xpg_buffer_container.list = 
    (XpgBufferSet*) malloc(sizeof(XpgBufferSet) * XdbConnQueDepth);

  for (int qid = 0; qid < XdbConnQueDepth; qid++) {
    //std::cout << "Allocating buffer for Queue " << qid 
      //<< " size " << xpg_get_buffer_size() << std::endl;
    // This will be Mapped by device ptr later in xpg_fpga_defs
    xpg_buffer_container.list[qid].native_stream.blk = NULL;

    //xpg_buffer_container.list[qid].tup_buffer.buf = 
      //(char*) palloc0(sizeof(char) * xpg_get_buffer_size());

    xpg_buffer_container.list[qid].tup_buffer.buf = 
      (char*) malloc(sizeof(char) * xpg_get_buffer_size());

    xpg_buffer_container.list[qid].tup_buffer.length = 0;
    xpg_buffer_container.list[qid].tup_buffer.numTups = 0;

    xpg_buffer_container.list[qid].result_buffer.buf = 
      (char*) malloc(sizeof(char) * xpg_get_buffer_size());
    xpg_buffer_container.list[qid].result_buffer.length = 0;
    xpg_buffer_container.list[qid].result_buffer.numTups = 0;
    bool * bitResult = (bool*)xpg_buffer_container.list[qid].result_buffer.buf;
    //if(qid%50 == 0)
    //  memset(bitResult, 1, sizeof(char) * xpg_get_buffer_size());
    //else
    memset(bitResult, 0, sizeof(char) * xpg_get_buffer_size());
      
    xpg_buffer_container.list[qid].tuple_cache.cache = 
      (uintptr_t*) malloc(sizeof(uintptr_t) * xpg_get_buffer_size());
  }
  
  return;
}

void xpg_rel_fpga() 
{
  for (int i=0; i<kernels; i++) {
    if (p_xdbConns[i]->getOpFlag(XdbConn::EnableProfiling) )
      p_xdbConns[i]->getProfileData();
    XdbConn::remove(p_xdbConns[i]);
  }
}

#endif
