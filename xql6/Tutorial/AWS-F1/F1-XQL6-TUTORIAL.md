# Data Analytics XQL6 Version 18.03.19 Tutorial

## Introduction
This tutorial introduces how to use XQL6, a PostgreSQL based FPGA acceleration UDF in AWS EC2 F1 Instance. 

The source code of XQL6 can be found at [Xilinx Data-Analytic Repo]

The instruction to launch an F1 instance can be found [here]

## Steps to Launch XQL6

Start by launching two terminals

### Terminal 1
1. Connect to F1 instance as centos user

2. Switch to superuser (root).
    ```
    $ sudo su -
    ```
       
3. Change to superuser's home directory
    ```
    $ cd ~
    ```
       
4. Enable Xilinx Runtime (XRT)
    ```
    $ source /opt/xilinx/xrt/setup.sh
    ```
       
5. Start Postgresql Server
    ```
    $ /home/centos/postgresql-9.6.2/install/bin/pg_ctl -D ./psql/ start
    ```

### Terminal 2

1. Connect to the same F1 instance as centos user

2. Change to centos's home directory
    ```
    $ cd ~
    ```

3. Connect to Postgresql server and tpch_1g database
    ```
    $ /home/centos/postgresql-9.6.2/install/bin/psql tpch_1g
    ```

4. Load xql6 UDF functions to execute query 6 on FPGA
    ```
    tpch_1g=# load '/home/centos/libs/xql6.so';
    LOAD
    tpch_1g=# CREATE OR REPLACE FUNCTION xql6(text, text) RETURNS cstring AS '/home/centos/libs/xql6.so', 'xql6'     LANGUAGE C STRICT;
    CREATE FUNCTION
    tpch_1g=# 
    ```
5. Run TPCH Query 6 using xql6 UDF
    ```
    tpch_1g=# select xql6('lineitem','swemu');
                              xql6                          
    --------------------------------------------------------
     xql6_run (sw) result 123141078.2283                   +
     Runtimes (us) Fill 688821, Comp 2363154. #Tups 6001215
    (1 row)
    ```
6. Turn on timing and rerun query 6 to see the runtime
    ```
    tpch_1g=# \timing
    Timing is on.
    tpch_1g=# select xql6('lineitem','/home/centos/awsxclbin/xql6.awsxclbin');
                                        xql6                                    
    ----------------------------------------------------------------------------
     xql6_run (hw) result 123141078.2283                                       +
     Runtimes (us) Fill 653924, W+Comp 7320, Rd 1050, Tot 662743. #Tups 6001215+
     
    (1 row)
    
    Time: 663.261 ms
    ```

7. Exit Postgresql
    ```
    tpch_1g=# \q
    ```
To get source code with README files for XQL6 implementation, please go to  [Xilinx Data-Analytic Repo] 

[here]: https://github.com/Xilinx/ML-Development-Stack-From-Xilinx/blob/master/launching_instance.md
[Xilinx Data-Analytic Repo]: https://github.com/Xilinx/data-analytics/tree/master/xql6



