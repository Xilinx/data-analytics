# Data Analytics XSQL61 Tutorial

## Introduction
This tutorial runs TPCH query 6 and query 1 directly on text table file of database on an FPGA acceleration kernel in AWS EC2 F1 Instance. 

The source code of xsql61 can be found at [Xilinx Data-Analytic Repo]

More information on this code can be found at [Text File Query 6 and 1 README]

The instruction on launching an F1 instance can be found [here]

### Terminal
1. Connect to F1 instance

The xclbin has been stored into AFI S3 image, and is ready to be executed on your F1 instance. To execute Query 6 and query 1 of TPCH on lineitem table, please follow these commands.

2. Navigate to /home/centos/xsql61_demo
    ```
    $ cd /home/centos/xsql61_demo
    $ ls
    afi		    sdaccel_profile_summary.csv   sqlq1tpch.awsxclbin  xsqltpch
    samplelineitem.tbl  sdaccel_profile_summary.html  sqlq6tpch.awsxclbin
    ```
3. Set AWS environments for F1 execution

   ```
   $ sudo sh
   $ source /opt/Xilinx/SDx/2017.1.rte/setup.sh
   ```
4. Run Query 6 on lineitem on F1 FPGA
   ```
   $ ./xsqltpch  -kernel sqlq6tpch -xclbin sqlq6tpch.awsxclbin -query 6 -db lineitem.tbl -ntuples 6001215
   ......
   INFO: Total time: 1.6815 sec. Total data: 184.00 MB. Efffective Rate: 109.427 MB/s
   INFO: For query 6 processed 92 pages
   INFO: Query6 result = 123141078.23  Hex = 000011EB5CD604B|
   All Complete!!
   ```
5. Run Query 1 on lineitem on F1 FPGA
   ```
   $ ./xsqltpch  -kernel sqlq1tpch -xclbin sqlq1tpch.awsxclbin -query 1 -db lineitem.tbl -ntuples 6001215
   ......
   Total time: 1.8058 sec. Total data: 734.00 MB. Efffective Rate: 406.479 MB/s
   INFO: For query 1 processed 367 pages
   Query1 result = 
   l_returnflag | l_linestatus | sum_qty | sum_base_price | sum_disc_price | sum_charge | avg_qty | avg_price | avg_disc | count_order | 
   A|F|37734107.00 | 56586554400.73 | 53758257134.87 | 55909065222.83 | 25.52|38273.13|0.050|1478493|
   N|F|991417.00 | 1487504710.38 | 1413082168.05 | 1469649223.19 | 25.52|38284.47|0.050|38854|
   N|O|73533166.00 | 110287596362.18 | 104774847005.94 | 108969626230.36 | 25.50|38249.00|0.050|2883411|
   R|F|37719753.00 | 56568041380.90 | 53741292684.60 | 55889619119.83 | 25.51|38250.85|0.050|1478870|
   All Complete!!  
   ```

[here]: https://github.com/Xilinx/ML-Development-Stack-From-Xilinx/blob/master/launching_instance.md
[Xilinx Data-Analytic Repo]: https://github.com/Xilinx/data-analytics/tree/master/xpg/host
[Text File Query 6 and 1 README]: https://github.com/Xilinx/data-analytics/blob/master/xsql61/README.md