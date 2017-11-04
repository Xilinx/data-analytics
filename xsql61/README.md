Xilinx Text Table based TPCH Query 6 and 1
==========================================

This README file contains the following sections:
  1. OVERVIEW
  2. DESIGN FILE HIERARCHY  
  3. COMPILATION AND EXECUTION
  4. SUPPORT AND OTHER INFORMATION

## 1. OVERVIEW
This directory provides sample kernel implementation for Query 6 and Query 1 on TPCH data tables. It can be run on F1 instances to produce high quality TPCH results.
Insightful real time analytics is becoming increasingly compute intensive as the data keeps growing. This results into massive compute node scaling, which are expensive and, sometimes infeasible. Xilinx FPGA platform and solutions can offer significantly better acceleration for compute offload by enabling a field customizable hardware implementation. In this design, we show a way to create a massively parallel processing architecture offering 20-30x better processing efficiency than CPUs. This architecture can be extended to implement different types of applications.

### FPGA Kernel
The kernel consists of multiple processing units connected in a modified SIMD (Single Instruction Multiple Data) architecture. Multiple PUs are sent data using a memory scheduler distributing the DDR bursts to each PU in a round robin fashion. When all PUs are done processing, a final merge operation is performed, and the result is sent to host code.

### Host Code
Host code reads text based tables and sends blocks of data using the data mover library, xdbconn. It maintains a circular queue deep enough to utilize the device DDR fully, and keep processing units from starving. It also merges the result from FPGA kernels to create the final result. Using this architecture, host can scale and process very large data sets.

## 2. DESIGN FILE HIERARCHY

Files                 | Description
----------------------|----------------------------------------------------------------------------
src/sqlq6tpch.cpp     | TPCH Query 6 Kernel with top level HLS unit
src/sqlq6procunit.cpp | TPCH Query 6 Kernel for one HLS processing unit
src/sqlq1tpch.cpp     | TPCH Query 1 Kernel with top level HLS unit
src/sqlq1procunit.cpp | TPCH Query 1 Kernel for one HLS processing unit
src/sqlpageloop.cpp   | Kernel with loopback logic for testing the data flow
src/sqldate.cpp       | Sqldate library to handle SQL data formats directly into the FPGA logic
src/sqltbl.cpp        | Read SQL table, format as needed to send to FPGA device memory
src/sqlutils.cpp      | SQL Utility functions
src/sqlhost.cpp       | For spawning SQL query 
src/tbsqlutils.cpp    | Unit test bench for SQL utility library
src/q6main.cpp        | Main code to execute query 6
src/q1main.cpp        | Main code to execute query 1
src/main.cpp          | Host code with main functions and the args

AWS related Files            | Description
-----------------------------|---------------------------------------------------------------------
aws/sqlq1tpch_AWS_hw.xclbin  | Registered AFI image for Query 1 runs
aws/sqlq6tpch_AWS_hw.xclbin  | Registered AFI image for Query 6 runs
aws/samplelineitem.tbl       | Sample lineitem table


## 3. COMPILATION AND EXECUTION

### Compiling to create host code
Set up environment for Xilinx SDx release
```
$source <SDX_INSTALL_DIR>/settings64.sh
```
Make XDB connector library in ../xdbconn (see README in ../xdbconn on how to build libxdbconn)

Build host code for tpch
```
make all XPART=AWS
```
You can also build xclbin for different flows, including sw_emu, hw_emu and hw using make command.
```
make xbin FLOW=sw_emu QUERYNO=6
```

### Running example design on AWS F1

We have created example AFI for Query 6 and Query 1.

#### Query 6

To check query 6 AFI ID availability, please do the following -
```
aws ec2 describe-fpga-images --fpga-image-ids afi-0118cc79e6c12ee83
```
The xclbin has been stored into AFI S3 image, and is ready to be executed on your F1 instance. To execute Query 6, please run the following command -
```
sudo sh
source /opt/Xilinx/SDx/2017.1.rte/setup.sh
bin/xsqltpch -hw -kernel sqlq6tpch -xclbin aws/sqlq6tpch_AWS_hw.xclbin  -query 6 -db aws/samplelineitem.tbl -ntuples 256
```
It generates sdaccel result files showing kernel, DDR and IO performances. 

#### Query 1

To check query 1 AFI ID availability, please do the following -
```
aws ec2 describe-fpga-images --fpga-image-ids afi-052c4dd898e6ce3bf
```
The xclbin has been stored into AFI S3 image, and is ready to be executed on your F1 instance. To execute Query 1, please run the following command -
```
sudo sh
source /opt/Xilinx/SDx/2017.1.rte/setup.sh
bin/xsqltpch -hw -kernel sqlq1tpch -xclbin aws/sqlq1tpch_AWS_hw.xclbin -query 1 -db aws/samplelineitem.tbl -ntuples 256
```
It generates sdaccel result files showing kernel, DDR and IO performances.

You can also run this on TPCH 1GB table that produces result same as the one produced by PostgreSQL integrated query. You can get the 1gb_lineitem.tbl in Amazon Data-Analytics F1 AMI and run the design there by replacing -db argument.

### Running TPCH Query tests using sw and hw flows

Run TPCH query with the lineitem table csv file
```
$make QUERYNO=6 DBMODE=csv run_sw_emu
```
To run TPCH query with the lineitem table csv file for hardware emulation
```
$make QUERYNO=6 DBMODE=csv run_hw_emu
```

## 4. SUPPORT AND OTHER INFORMATION

There is more information available below to learn more about different flows on Amazon F1.

### Amazon AWS F1 Information

You can learn more about AWS F1 instance hardware at [Amazon F1 hardware](https://aws.amazon.com/ec2/instance-types/f1) AWS also provides an [Official AWS github](https://github.com/aws/aws-fpga) where information is provided on how to build and execute for Xilinx FPGA devices. [AWS FPGA Forums](https://forums.aws.amazon.com/forum.jspa?forumID=243&start=0) has discussions related to F1 development, and please feel free to participate.

### Creating FPGA binaries
This design comes with a xilinx binary for AWS that can be run directly on F1 instance. It also has the xclbin that has been created by Xilinx tools. This 

### Amazon AWS F1 Information and Support

You can learn more about AWS F1 instance hardware at [Amazon F1 hardware](https://aws.amazon.com/ec2/instance-types/f1) site. AWS also provides an [Official AWS github](https://github.com/aws/aws-fpga), where information is provided on how to build and execute for Xilinx FPGA devices. [AWS FPGA Forums](https://forums.aws.amazon.com/forum.jspa?forumID=243&start=0) has discussions related to F1 development, and please feel free to participate.

### SDAccel Information and Support

This example uses SDAccel tools to generate F1 acceleration designs. 
For more information check here: [SDAccel User Guides](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2017_1/ug1023-sdaccel-user-guide.pdf)

For questions and to get help on SDAccel visit the [SDAccel Forums](https://forums.xilinx.com/t5/SDAccel/bd-p/SDx).

### AWS SDK Flow

This design uses AWS SDK (Software Development Kit) Flow. Click here on [AWS SDK](https://github.com/aws/aws-fpga/tree/master/sdk) to learn about the required setup to build the binaries.

This example comes with a pre-built xclbin for Xilinx F1 instance. Please follow README from github xsql61 files to create a new xclbin. 

### AWS AFI Image

This example comes with a pre-built AWS AFI image to run the sample design.

You can follow the [AWS AFI generation](https://github.com/aws/aws-fpga/tree/master/hdk/cl/examples) steps to create an AFI from Xilinx binaries. 

The command to generate an AFI image for the given binary looks like the following command. Pleas change the names and keys to run for your specific keys and binary names
```
$SDACCEL_DIR/tools/create_sdaccel_afi.sh -xclbin=sqlq1tpch_AWS_hw.xclbin -o=sqlq1tpch_AWS_hw.xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0 -s3_bucket=aws-xlnx-f1-developer -s3_dcp_key=user@example.com/f1-dcp-folder/ -s3_logs_key=user@example.com/f1-logs/
```

Please ensure that the fpga image has a valid Amazon id. The instructions on these steps exist on AWS AFI generation page.

```
aws ec2 describe-fpga-images --fpga-image-ids afi-your-afi-id
```

Please look at the options provided for bin/xsqltpch to run it in different modes. You can feed in larger lineitem.tbl generated by TPCH dbgen. This code has been checked with up to 100GB of data size, and it scales with the data design for TBytes of data sizes as well.



