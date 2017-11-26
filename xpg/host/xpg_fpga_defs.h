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
#ifndef XPGCSIM_TYPES_H
#define XPGCSIM_TYPES_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>

#ifdef HW_INTEGRATION
#include "xdbutils.h"
#include "xdbconn.h"
#include "oclaccel.h"
#endif
/*
 * operation (scan/aggregate etc) and data (column data) streams to send to FPGA
 */
//xpg_datastream - stream of "columns" of interest 
extern "C" char *xpg_datastream;
//xpg_setupstream - stream of constants & operands
extern "C" char *xpg_setupstream;

//xdbconn for FPGA opencl interaction

#ifdef HW_INTEGRATION
extern "C" XdbConn **p_xdbConns; //xdbconn
extern "C" int kernels;
extern "C" int xdbconnQueDepth; // = 1;
extern "C" char *FpgaTargetDeviceName; // = "xilinx:adm-pcie-ku3:2ddr-xpr:3.3";
#endif


//Init FPGA defs/constants
void xpg_init_fpga_defs(char * xclbinPath);
void xpg_init_fpga_defs_HW(int query, char * setupbuf_HW);

void xpg_rel_fpga(); 

#endif
