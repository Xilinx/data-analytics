# Data Analytics XQL6 Version 18.03.19 Tutorial

## Introduction
This tutorial introduces how to use XQL6, a PostgreSQL based FPGA acceleration UDF in AWS EC2 F1 Instance. 

The source code of XQL6 can be found at [Xilinx Data-Analytic Repo]

The instruction to launch an F1 instance can be found [here]

## Steps to Launch XQL6

Start by launching two terminals

### Terminal 1
1. Connect to F1 instance
2. Navigate to /home/centos/xql6_demo
    ```
    $ cd /home/centos/xql6_demo
    $ ls
    Dockerfile  afi  host postgres_f2  run_root.sh
    ```
3. Run `./run_root.sh xql6_demo_container xql6_work`
    ```
    $ ./run_root.sh xql6_demo_container xql6_work
    root@0f077c7d68a7:/# 
    ```
4. Now you are inside docker container, navigate to /home/xql6_work/demo
    ```
    $ cd /home/xql6_work/demo
    $ ls
    postgres_f2  run_pg.sh  xql6.awsxclbin  xql6.so  xql6.sql
    ```
5. Run `run_pg.sh`
    ```
    $ ./run_pg.sh
    2018-01-23 22:38:46.530 UTC [21] LOG:  database system was shut down at 2018-01-08 21:45:59 UTC
    2018-01-23 22:38:46.538 UTC [21] LOG:  MultiXact member wraparound protections are now enabled
    2018-01-23 22:38:46.540 UTC [25] LOG:  autovacuum launcher started
    2018-01-23 22:38:46.541 UTC [20] LOG:  database system is ready to accept connections
    ```

### Terminal 2
1. Connect to the same F1 instance
2. Run `sudo docker exec -it xql6_demo_container /bin/bash`
    ```
    $ sudo docker exec -it xql6_demo_container /bin/bash
    root@0f077c7d68a7:/# 
    ```
3. Navigate to /home/xql6_work/demo
    ```
    $ cd /home/xql6_work/demo
    $ ls
    postgres_f2  run_pg.sh  xql6.awsxclbin  xql6.so  xql6.sql
    ```
4. Now you are inside docker container
5. Run `su postgres` to change user to `postgres`
    ```
    $ su postgres
    postgres@0f077c7d68a7:
    ```
6. Now you are inside PostgreSQL console 
7. Run `psql tpch_1g` to connect to 1GB TPCH database
    ```
    $ psql tpch_1g
    psql (9.6.3, server 9.6.2)
    Type "help" for help.

    tpch_1g=# 
    ``` 
8. Load xql6 UDF functions to execute query 6 on FPGA
    ```
    tpch_1g=# \i xql6.sql
    CREATE FUNCTION
    tpch_1g=# 
    ```
9. Run TPCH Query 6 using xql6 UDF
    ```
    tpch_1g=# select xql6('lineitem','swemu');
                          xql6                          
--------------------------------------------------------
 xql6_run (sw) result 123141078.2283                   +
 Runtimes (us) Fill 618284, Comp 2320124. #Tups 6001215
(1 row)
    ```
10. Turn on timing and rerun query 6 to see the runtime
    ```
    tpch_1g=# \timing
    tpch_1g=# select xql6('lineitem', '/home/centos/xql6_demo/afi/fpga_q6.awsxclbin');
                                    xql6                                     
-----------------------------------------------------------------------------
 xql6 init (fpga config) 7035247 us                                         +
 xql6_run (hw) result 123141078.2283                                        +
 Runtimes (us) Fill 626897, W+Comp 16064, Rd 1097, Tot 644128. #Tups 6001215+ 
(1 row)    
    ```
To get source code with README files for XQL6 implementation, please go to  [Xilinx Data-Analytic Repo] 

[here]: https://github.com/Xilinx/ML-Development-Stack-From-Xilinx/blob/master/launching_instance.md
[Xilinx Data-Analytic Repo]: https://github.com/Xilinx/data-analytics/tree/master/xql6



