Xilinx End-2-End PostgreSQL Acceleration for TPC-H Query-6 - Kernel Component
===========================================================================

This README file contains the following sections:
  1. OVERVIEW
  2. DESIGN FILE HIERARCHY  
  3. COMPILATION AND EXECUTION

## 1. OVERVIEW
HLS kernel hardcoded for lineitem table and sql Query-6, from the TPCH benchmarks. Task-parallel, SPMD architecture. This example uses 32 processing units (*PUs*). Parses native postgresql data format tuples, performs column filtering, scan and aggregate for SQL Query-6.

Operates on pages. Page structure: {nTups, {Tups}+, deadspace}. Each page is
given to a PU and is self-contained. Many pages make up a Block. The sizes are
configurable and should reflect on host-side as well.

## 2. DESIGN FILE HIERARCHY
The code is made available in two seperate directories, one for the host component and the other for the FPGA kernel.

Files              | Description
-------------------|----------------------------------------------------------------------------
q6\_paged.h        | Header for kernel
q6\_paged.cpp      | Kernel functions
q6\_paged\_tb.h    | Header for vivado\_hls csim Test bench
q6\_paged\_tb.cpp  | Sources for csim Test bench


## 3. COMPILATION AND EXECUTION
```
cd kernel
mkdir run1; cd run1; cp ../makefile .
make xbin FLOW=hw XDEVICE=xilinx:adm-pcie-ku3:2ddr-xpr:3.3
```
For AWS F1, copy the kernel binary (\*.xclbin), to your F1 instance and follow the directions to get an AFI.


