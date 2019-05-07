Xilinx Database Acceleration
==========================================

This README file contains the following sections:
  1. OVERVIEW
  2. DESIGN FILE HIERARCHY  
  3. COMPILATION AND EXECUTION

## 1. OVERVIEW
This directory provides database functional implementation for gather using interleaving and streaming. 
  -  Interleaving: Data A is equally interleaved across # HBM . 
  -  Streaming:    Data in/out is being streamed while excuting kernel operations.   


### FPGA Kernel
The kernel is for random access to database across HBMs. by setting opmode argument in host code, you can select to run the required operation. when each operation is done, the output will write back data out to DDR. Host code can check the result.

### Host Code
Host code distribute data(A) equally across HBMs and  block streaming data in/out  while executing kernel from host code. use ping-pong buffer architecture, you can transfer data to and from FPGA simultaneously and also perform computations during the data transfers. Performing all three of these operations at the same time allows you to keep the FPGA busy and take full advantage of all of the hardware on your system.

## 2. DESIGN FILE HIERARCHY

Files                     | Description
--------------------------|----------------------------------------------------------------------------
makefile                  | make file
src/main.cpp              | Host code with main function 
src/oclaccel.cpp          | For host code
kernel/omx.cpp            | Kernel top level
kernel/gather.cpp         | Kernel sub-function Gather



## 3. COMPILATION AND EXECUTION
### Before Compiling

In makefile, in current release(2018.3), you need to update the user_tcl_file.tcl path to the full path in your working environment intead of relative path: 
 Line need to update in makefile is as follows:
   --xp param:compiler.userPostSysLinkTcl=[full_path]/scripts/user_tcl_file.tcl


Set up environment for Xilinx SDx release
```
$source <SDX_INSTALL_DIR>/settings64.sh
```

### Compiling to create host code
Build host code for omx
```
make host
```
You can also build xclbin for different flows, including sw_emu, hw_emu and hw using make command.
```
make FLOW=hw xbin
```

### Running example design

After you have compiled host code and xclbin, you can run designs using command line like below:
  -  sw emulation: #pu=16 #hbm=8 

./omx -xclbin omx_U280_sw.xclbin -mode sw_emu -pu 16 -hbm 8 

  -  hw: #pu=16 #hbm=4 

./omx -xclbin omx_U280_hw.xclbin  -pu 16 -hbm 4 


### Command line option description
Argument |   Value            | Description               | Default
---------|-----------------   |---------------------------|-------------------------------------------------
-pu      | 16/8/4/2/1         |  Select #PU               |  16
-hbm     | 16/8/4/2/1         |  HBM counts               |  16
-mode    | sw_emu/cpu/other   |  Select mode              |  other
-cb      | 1/0                |  Enable callback function |  0 (disabled callback function) 
-pc      | 1/0                |  Enable Pseudo-Channel    |  1



## 4.SOFTWARE AND SYSTEM REQUIREMENTS
Board	                              | Device Name             	| Software Version
--------------------------------------|---------------------------------|-----------------------
Alveo U280                            | xilinx_u280-es1_xdma_201830_1   | SDAccel 2018.3








