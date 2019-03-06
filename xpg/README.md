Xilinx End-2-End PostgreSQL Accelerator
======================

This README file contains the following sections:
  1. OVERVIEW
  2. DIRECTORY STRUCTURE
  3. SOFTWARE AND SYSTEM REQUIREMENTS
  4. EXECUTION

## 1. OVERVIEW
This library provides full end-to-end support for Postgres Acceleration for Scan using postgres native hooks. The library interacts with Native Postgres hooks, decides whether scan can be offloaded to FPGA, and take care of offloading scan operation to FPGA and sends the Scan result back to Postgres server for rest of the operations in the query. There are three main phases FPGA Offload Feasibility, FPGA Codegen, Data Mover, FPGA Offload, and Data Receiver.

### 1.1 FPGA Offload Feasibility
This is the first phase where decision is made to make a FPGA offload call. The decision is made based on Kernel resource feasibility, Kernel operator support, Kernel Data Type support, and Cost function. Kernel implemention has some limits set to number of expressions that can be offloaded, depth of expression tree, the type of operations, and supported datatypes. If all the checks are satisfied, then FPGA offload call will be active. Currently only Scan operation is supported.
Currently cost function is set to "0" wich means the offload happens by default if rest of the checks are met. In future, we plan to expand the cost function to decide the offload feasibility compared to CPU runtime through some rough clock cycle estimations.

### 1.2 FPGA Codegen
Once FPGA offload Feasibility is checked, codegen is called to generate a stream of native bits to teach FPGA on the current Scan operation. Codegen stream has the constants, expressions, qualifiers, columns to process, and operator/operand data encoded.

### 1.3 Data Mover
Data Mover sends the raw Tuple data from Postgres for the current Table to FPGA. Columns to process are properly tagged based on the data from FPGA codegen.

### 1.4 FPGA Offload
FPGA codegen stream (from Codegen) and Data Stream (from Data Mover) are sent to FPGA for the Scan operation. This uses xdbconn - Xilinx Data Mover library to move the data to FPGA, calls Enqueue compute, and receives the data back from FPGA. Please refer to xdbconn in data-analytics for more details. Also details on kernel can be found in xpg/kernel section

### 1.5 Data Receiver
Post FPGA offload call, the data is received from FPGA and native tuples are constructed back for the rows that satisfy the Scan operation. The native tuples based table is then sent to Postgres for subsequent Aggregate/hash and other operations that are part of the query.

## 2. DIRECTORY STRUCTURE

Directory    | Description
-------------|----------------------------------------------------------------------------
Binaries/    | The binary for target host systems, and the xclbin for target devices
Tutorial/    | Tutorials for executing on specific host systems
README.md       | Readme file

## 3. SOFTWARE AND SYSTEM REQUIREMENTS

|         Board       |                 Device Name               |    Software Version |
|---------------------|-------------------------------------------|-------------------------------------|
|AWS F1 Custom Board  | xilinx_aws-vu9p-f1-04261818_dynamic_5_0   |    Xilinx SDx 2018.2 (AMV v1.5.x)|
|Xilinx Alveo U200   | xilinx_xdma_201820_1  |    2018.2_XDF |

## 4. EXECUTION

### Setting Xilinx SDx release environment
Set up environment for Xilinx SDx release
```
$source <SDX_INSTALL_DIR>/settings64.sh
```

### Connect to PostgreSQL client and load XPG library
Connect to PostgreSQL client
```
psql -p <port> <database name> 
```
In PostgreSQL shell, load the xpgbolt library
```
LOAD '<full path to xpgbolt.so'> 
```
Create UDF for load & release FPGAs
```
CREATE OR REPLACE FUNCTION init_fpga(text) RETURNS void AS '<path to xpgbolt.so>', 'init_fpga' LANGUAGE C STRICT;
CREATE OR REPLACE FUNCTION rel_fpga() RETURNS void AS '<path to xpgbolt.so>', 'release_fpga' LANGUAGE C STRICT; 
```
Init FPGA - Loads the bitstream. Please refer to xpg/kernel for building xclbin
```
select init_fpga('<full path to xclbin file>');
```
Set FPGA offload on - This will offload the queries to FPGA

```
set xpgbolt.fpga_offload=on;
```
Once the above sequence of commands are done, then the subsequent queries will be offloaded to FPGA
