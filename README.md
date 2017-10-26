Data analytics Acceerator Repository
===========================

Welcome to the Data Analytics repository. This repository contains the latest examples to get you started with application optimization targeting Xilinx PCIe FPGA acceleration boards. All examples are ready to be compiled and executed on boards hosted in the Amazon F1 clouds. The repository information is provided as below

1. BIG DATA ACCELERATION
2. ACCELERATION MODULES
3. DIRECTORY STRUCTURE
7. OTHER INFORMATION
8. SUPPORT
9. REVISION HISTORY


### 1. BIG DATA ACCELERATION
This repository provides source code for big data acceleration on FPGA. In particular, this provides acceleration kernel for PostgreSQL RDBMS and Text files. The acceleration is in SQL Query and other typical big data operations.

### 2. ACCELERATION MODULES

This repository includes the following acceleration modules that can be run on F1
Directory    | Description
-------------|----------------------------------------------------------------------------
             | Postgres integrated TPCH Query 6 Acceleration
tpch         | Text file table TPCH Query 6 and 1 Acceleration

### 4. DIRECTORY STRUCTURE
Each directory contains a makefile to build host code, library and Xilinx Kernel for target platforms. These are the directories and their functionalities.

Directory    | Description
-------------|----------------------------------------------------------------------------
xdbconn      | An API structure to move blocks of data from host to device using Xilinx SDx
sql          | Sql library with functions to handle Sql data on FPGA
tpch         | Host code and Xilinx FPGA Kernel code to run Query 6 and Query 1 on FPGA


## 7. OTHER INFORMATION

For more information check here:
[SDAccel User Guides][]

## 8. SUPPORT
For questions and to get help on this project or your own projects, visit the [SDAccel Forums][].

## 9. REVISION HISTORY

Date    | Readme Version | Revision Description
--------|----------------|-------------------------
OCT2017 | 1.0            | Initial Xilinx release


