Data Analytics Accelerator Repository
====================================

Welcome to the Data Analytics repository. This repository contains the latest examples to get you started with application optimization targeting Xilinx PCIe FPGA acceleration boards. All examples are ready to be compiled and executed on boards, for example, the boards hosted in the Amazon F1 clouds. The repository information is provided as below

1. BIG DATA ACCELERATION
2. ACCELERATION MODULES
3. DIRECTORY STRUCTURE
4. OTHER INFORMATION
5. SUPPORT
6. REVISION HISTORY


## 1. BIG DATA ACCELERATION
This repository provides source code for big data acceleration on FPGA. In particular, this provides acceleration kernel for PostgreSQL RDBMS. The acceleration is in SQL Query and other typical big data operations.

## 2. ACCELERATION MODULES

This repository includes the following acceleration modules that can be run on F1

### xpg

It accelerates Postgres query by offloading scanning operations to Xilinx FPGA. The query plan is intercepted during the execution, and data is streamed to the FPGA for scan acceleration. The output relation is then registered with postgres database. The FPGA kernel parses native Postgres storage blocks in a massively parallel implementation of SQL Engines.

Refer to [xpg/README.md](https://github.com/Xilinx/data-analytics/blob/master/xpg/README.md) to learn more about this acceleration.

### xql6

It accelerates Query 6 integrated within Postgres as a UDF (User Defined Functions). It sends Postgres tables to FPGA. An array of Query 6 processors process this native data to produce Query 6 results. When comparing end-to-end solution on F1, it shows a performance improvement. Kernel level performance is an order of magnitude better than F1 CPU.

Refer to [xql6/README.md](https://github.com/Xilinx/data-analytics/blob/master/xql6/README.md) to learn more about this acceleration.

The following table summarizes PROJECTS and ACCELERATION MODULES associated with their respective supported release. The release support refers to the repository branch. The current supported release is 2018.2.

PROJECTS and ACCELERATION MODULES  | Release Support
-----------------------------------|------------------
XPG                                | 2018.2, 2017.1
XQL6                               | 2018.2, 2017.1
XSQL61                             | 2017.1
XDBCONN                            | 2017.1
XSQLUTILS                          | 2017.1

## 3. DIRECTORY STRUCTURE

All the latest files in the directories are present in Xilinx github at https://github.com/Xilinx/data-analytics

Each directory contains a makefile to build host code, library and Xilinx Kernel for target platforms. These are the directories and their functionalities.

Directory    | Description
-------------|----------------------------------------------------------------------------
postgresql   | Postgres internals (for reference)
xpg          | Postgres integrated TPCH Query Acceleration host code
xsql6        | Postgres integrated TPCH Query 6 Acceleration

## 4. OTHER INFORMATION

For more information check here:
[SDAccel User Guides](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2018_2/ug1023-sdaccel-user-guide.pdf)

## 5. SUPPORT
For questions and to get help on this project or your own projects, visit the [SDAccel Forums](https://forums.xilinx.com).

## 6. REVISION HISTORY

Date    | Readme Version | Revision Description
--------|----------------|-------------------------
OCT2017 | 1.0            | Initial Xilinx release
Mar2019 | 2.0            | Upgrade to SDx 2018.2 release


