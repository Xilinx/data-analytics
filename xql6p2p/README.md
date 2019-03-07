Xilinx End-2-End PostgreSQL Acceleration for TPC-H Query-6
Peer-to-Peer (P2P) support
==========================================================

This README file contains the following sections:
  1. OVERVIEW
  2. DIRECTORY STRUCTURE
  3. SOFTWARE AND SYSTEM REQUIREMENTS

## 1. OVERVIEW
Please see the host/README.md for P2P support in the host component
and kernel/README.md for kernel details.


## 2. DESIGN FILE HIERARCHY
The code is made available in two seperate directories, one for the host component and the other for the FPGA kernel.

Files           | Description
----------------|----------------------------------------------------------------------------
host/           | Source files for Host component
kernel/         | Source files for FPGA Kernel (HLS) component
Binaries/       | The binary for target host systems, and the xclbin for target devices
Tutorial/       | Tutorials for executing on specific host systems
README.md       | Readme file

## 3. SOFTWARE AND SYSTEM REQUIREMENTS

|         Board       |                 Device Name               |    Software Version |
|---------------------|-------------------------------------------|-------------------------------------|
|Xilinx Alveo U200   | xilinx_xdma_201830_2  |    2018.3 |

