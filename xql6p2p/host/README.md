Xilinx End-2-End PostgreSQL Acceleration for TPC-H Query-6 - Host Component
===========================================================================

This README file contains the following sections:
  1. OVERVIEW
  2. DESIGN FILE HIERARCHY  
  3. COMPILATION AND EXECUTION

## 1. OVERVIEW
The Host component serves as an intermediary between the Postgres server and the FPGA accelerator. It performs the following tasks:

1. Extend Postgres server using the C-UDF mechanism provided by Postgres
2. Manage the FPGA using the Xilinx OpenCL API. This includes identifying and starting up the OpenCL platform, data transfers to/from the FPGA and executing the kernel.
3. Use asyncronous OpenCL queues and events to hide the data transfer latency.
4. Extract raw tuples from Postgres data-structures and package them into Pages for the kernel
5. A software emulation interface to perform Query-6 scan+aggregate on the CPU

## 2. DESIGN FILE HIERARCHY

Files       | Description
------------|----------------------------------------------------------------------------
xql6.h      | Header for host code. Defines data sizes and state structures.
xql6.c      | Main entry point for the Postgres UDF. Performs Query-6 scan+agg on FPGA using native tuple data.
fpga\_q6.c  | Wrappers for OpenCL routines and memory buffer management.
q6\_swemu.c | Software emulation of Query-6. Operates on same same data as xql6.c, but performs scan+aggr on CPU
q6\_swemu.h | Header for software-emulation of Query-6
6.sql       | Query-6 from TPCH
xql6.sql    | Sql file to install C-UDf and create a new function in PostgreSQL.

## 3. COMPILATION AND EXECUTION
PostgreSQL 9.6 development libraries are needed to compile the Host component. On ubuntu derivatives, this is provided by the *postgresql-server-dev-9.6* package. After compilation, the shared library should be copied to a path that has been configured as a library dir for Postgres. The default path to this dir is */usr/lib/postgresql/9.6/lib/*.

```
cd host
make
sudo cp xql.so /usr/lib/postgresql/9.6/lib/.

P2P
Follow the same steps to run the P2P xql6 queries as the non-P2P except that
the query now takes a board Id as argument which should be 0 for single board
setups. For example:

select xql6('lineitem', 'fpga_q6.xclbin', 0);

which means run the query on Xilinx board with Id 0 - as returned by xbutil list command.


Multi-Board
The xql6 accelerated query now takes a third argument - the board Id - on which
the query should run. This Xilinx board Id should be one of the Ids as returned by
the 'xbutil list' command (formerly xbsak list command).

For example:

select xql6('lineitem', 'fpga_q6.xclbin', 0);

which means run the query on Xilinx board with Id 0.

```
