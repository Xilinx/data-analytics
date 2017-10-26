Xilinx SQL Library Elements
======================

This README file contains the following sections:
  1. OVERVIEW
  2. DESIGN FILE HIERARCHY  
  3. COMPILATION AND EXECUTION

## 1. OVERVIEW
This library provides functions that can be used for some SQL library elements on FPGA.

## 2. DESIGN FILE HIERARCHY

Files               | Description
--------------------|----------------------------------------------------------------------------
src/sqldecimal.h    | Templated function library to create Sql Decimal using AP_FIXED of Xilinx HLS
src/sqldate.cpp     | Sqldate library to handle SQL data formats directly into the FPGA logic
src/sqlutils.cpp    | SQL Utility functions
src/sqldefs.h       | Defines used in SQL library implementations
src/tbsqlutils.cpp  | Unit test bench for SQL utility library

## 3. COMPILATION AND EXECUTION

### Compiling to create the sql library
Set up environment for Xilinx SDx release
```
$source <SDX_INSTALL_DIR>/settings64.sh
```
Make SQL library bin/libsql.a
```
$make lib
```
### Compiling to create the sql test
Build sql test and run a unit test
```
$make
$bin/tbsqlunits
```