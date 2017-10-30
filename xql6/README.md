Xilinx End-2-End PostgreSQL Acceleration for TPC-H Query-6
==========================================================

This README file contains the following sections:
  1. OVERVIEW
  2. DESIGN FILE HIERARCHY  
  3. COMPILATION AND EXECUTION

## 1. OVERVIEW
XQL6 is an FPGA accelerated implementation of Query-6 from the TPC-H benchmarks for PostgreSQL 9.6. It has a host component which extends Postgres via a C-UDF (C - User Defined Function), and an FPGA component that accelerates scan + aggregate of Query-6. This project is presented as a technology demo for end-to-end database acceleration.

### 1.1 FPGA Kernel
The kernel consists of multiple processing units (**PU**s) arranged as an SPMD (Single Program, Multiple Data) architecture. Each PU operates on a *Page* of data from the host. Pages have a fixed size of 8K and have the following structure: {\*nTuples\*, {Tuple}\*, deadspace}. The Tuples are in native PostgreSQL format, with their headers removed. Please refer to PostgreSQL sources for more information about the Tuple format. 

Multiple pages make up a *Block* of data. This example uses Blocks of size 2MB. Each kernel invocation is performed on a Block. The host tranfers the Block over the PCIe bus onto the on-board memory of the FPGA accelerator. The kernel assigns Pages from the Block to each PU in a round-robin manner. The number of PUs, Page and Block sizes are configurable in the code. 

The PU unpacks the tuples, extracts the relevant columns for Query-6, converts the data to native C formats (int, long, etc), and performs the scan + aggregate operation. The aggregates from all PUs are eventually combined to a single aggregate value, which is then written back to the on-board DDR. The host reads this result back and returns it to PostgreSQL. The kernel employs task level parallelism both within and across the PUs to maximize the throughput.

### 1.2 Host Component
The host code compiles to a .so that can be loaded into a running Postgres server as a C UDF. The UDF manages the FPGA through Xilinx OpenCL API, and the data movement from Postgres to the FPGA. Data transfers are overlapped with kernel compute to reduce latencies. 

The UDF requests native storage pages from PostgreSQL, extracts the tuples and repackages them into a new page structure for FPGA offload. Native PostgreSQL pages contain a lot of meta-data which is not necessary for OLAP style scan-aggregate operations. Removing this meta-data reduces the amount of data that has to be transferred over the PCIe bus to the FPGA accelerator. On the other hand, unpacking the PostgreSQL tuples and performing column filtering can be very CPU intensive and is preferably done on the FPGA. This is an example of the tradeoffs involved in FPGA acceleration.

## 2. DESIGN FILE HIERARCHY
The code is made available in two seperate directories, one for the host component and the other for the FPGA kernel.

Files     | Description
----------|----------------------------------------------------------------------------
host/     | Source files for Host component
kernel/   | Source files for FPGA Kernel (HLS) component
README.md | Readme file

## 3. COMPILATION AND EXECUTION
PostgreSQL 9.6 server loaded with TPC-H data, is required to execute the Query-6 UDF. To compile host-code, PostgreSQL 9.6 development libraries are needed. Please refer to your operating system instructions to install these libraries.

```
myhost$ psql -d tpch1G
psql (9.6.2)
Type "help" for help.

tpch1G=# \dt
         List of relations
 Schema |   Name   | Type  | Owner
--------+----------+-------+-------
 public | customer | table | parik
 public | lineitem | table | parik
 public | nation   | table | parik
 public | orders   | table | parik
 public | part     | table | parik
 public | partsupp | table | parik
 public | region   | table | parik
 public | supplier | table | parik
(8 rows)                                         
```

### 3.1 Compile Host Code
```
cd host
make
sudo cp xql.so /usr/lib/postgresql/9.6/lib/.
```
### 3.2 Compile FPGA Kernel
```
cd kernel
mkdir run1; cd run1; cp ../makefile .
make xbin
```
For Amazon F1, copy the kernel binary (\*.xclbin), to your F1 instance and follow the directions to get an AFI.

### 3.3 PostgreSQL Configuration
Add LD\_LIBRARY\_PATH and XILINX\_SDX to /etc/postgresql/9.6/main/environment. This is to ensure that the C-UDF can see the Xilinx shared libraries during execution from within the PostgreSQL environment.

```
XILINX\_SDX='/proj/xbuilds/2017.1\_sdx\_1026\_1/installs/lin64/SDx/2017.1'
LD\_LIBRARY\_PATH='$XILINX\_SDX/lib/lnx64.o:$XILINX\_SDX/runtime/lib/x86\_64'
```

### 3.3 Execution
Create the C-UDF by running the xql6.sql file. The UDF can then be executed either in software-emulation mode or in FPGA acceleration mode. 

```
Usage:
  select xql6 (relationname, command);
    command = { xclbin-name, swemu, stop }
      xclbin-name : Load the xclbin onto fpga and run scan+aggr on relation. FPGA
                    remains open until 'stop' is called.
      swemu       : Run software emulation based scan+aggr 
      stop        : Release fpga kernel
```

```
myhost$ psql -d tpch1G
tpch1G# \i xql6.sql;

tpch1G# select xql6('lineitem', 'swemu');
  xql6\_run result 1231410782283 2929985381 ns+

tpch1G# select xql6('lineitem', 'full/path/to/fpga\_q6.awsxclbin')
  xql6\_run result 1231410782283 507591046 ns+

tpch1G# select xql6('lineitem', 'stop');
```
