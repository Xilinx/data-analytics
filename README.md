Data Analytics Accelerator Repository
====================================

Welcome to the Data Analytics repository. This repository contains the latest examples to get you started with application optimization targeting Xilinx PCIe FPGA acceleration boards. All examples are ready to be compiled and executed on boards hosted in the Amazon F1 clouds. The repository information is provided as below

1. BIG DATA ACCELERATION
2. ACCELERATION MODULES
3. DIRECTORY STRUCTURE
4. OTHER INFORMATION
5. SUPPORT
6. REVISION HISTORY


## 1. BIG DATA ACCELERATION
This repository provides source code for big data acceleration on FPGA. In particular, this provides acceleration kernel for PostgreSQL RDBMS and Text files. The acceleration is in SQL Query and other typical big data operations.

## 2. ACCELERATION MODULES

This repository includes the following acceleration modules that can be run on F1

### xql6

It accelerates Query 6 in a postgre integrated module. Refer to xql6/README.md to learn more about the acceleration.

### xsql61

It shows an accelerated Query 6 and 1 implementation. Refer to [xsql6/README.md](xsql6/README.md) ( directory to run the design on an F1 instance.


## 3. DIRECTORY STRUCTURE
Each directory contains a makefile to build host code, library and Xilinx Kernel for target platforms. These are the directories and their functionalities.

Directory    | Description
-------------|----------------------------------------------------------------------------
xsql6        | Postgres integrated TPCH Query 6 Acceleration
xsql61       | Host code and Xilinx FPGA Kernel code to run Query 6 and Query 1 on FPGA
xdbconn      | An API structure to move blocks of data from host to device using Xilinx SDx
xsqlutils    | Sql library with functions to handle Sql data on FPGA

All the latest files in the directories are all present in the github at https://github.com/Xilinx/data-analytics

## 4. OTHER INFORMATION

For more information check here:
[SDAccel User Guides](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2017_1/ug1023-sdaccel-user-guide.pdf)

## 5. SUPPORT
For questions and to get help on this project or your own projects, visit the [SDAccel Forums][].

## 6. REVISION HISTORY

Date    | Readme Version | Revision Description
--------|----------------|-------------------------
OCT2017 | 1.0            | Initial Xilinx release


