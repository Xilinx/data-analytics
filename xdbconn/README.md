Xilinx Data Mover library
======================

This README file contains the following sections:
  1. OVERVIEW
  2. DESIGN FILE HIERARCHY  
  3. COMPILATION AND EXECUTION

## 1. OVERVIEW
This library provides an API to move data from host to Xilinx Device. It does so by using asynchronous event triggered actions to force data movement and kernel execution in correct order. It also maintains a circular queue in host, large enough to move enough data to host DDR. This provides a robust queue to FPGA device, so that the kernel does not have dead cycles waiting for data. This has been optimized to achieve high data rate transfers over PCIe bus. User has to copy the data into the pinned cl buffer, and then enqueue it for compute. When completed a registered call back function is invoked with the kernel result. Host code then must collect together the results to produce final results

## 2. DESIGN FILE HIERARCHY

Files               | Description
--------------------|----------------------------------------------------------------------------
src/oclaccel.cpp    | OpenCL API call to connect to Xilinx platforms
src/xdbconn.cpp     | Enqueue, Write, Read Merge based block-wise data mover
src/blkloop.cpp     | Test kernel to loop back block of data
src/blkpu.cpp       | Test kernel to test block processing unit data
src/xdbutils.h      | Sample utils libraries
src/main.cpp        | Test main code for loop back of blocks of data


## 3. COMPILATION AND EXECUTION

### Compiling to create the xdbcon library
Set up environment for Xilinx SDx release
```
$source <SDX_INSTALL_DIR>/settings64.sh
```
Make XDB connector library bin/libxdbconn.a
```
$make lib
```
### Compiling to create the xdbcon test
Build xdbconn test
```
$make  
```
To run a test, choose a target sw_emu or hw_emu or hw and build xclbin where
```
	sw_emu = software emulation
	hw_emu = hardware emulation
```
For sw_emu test
```
$make run_sw_emu
```
Make blkloop kernel to test loopback mode using xdbconn
```
$make KERNELS=1 KERNEL=blkloop XPART=KU3 RUNMODE=hw xbin
```
For board loopback test use xdbconn with the generated xclbin
```
$bin/xdbconn -kernel blkloop -xclbin blkloop_hw.xclbin
```
