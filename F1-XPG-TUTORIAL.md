# Data Analytics XPGBolt Tutorial

## Introduction
This tutorial introduces how to launch and use XPGBolt, a PostgreSQL based FPGA accelerating library in AWS EC2 F1 Instance. 

The source code of XPGBolt can be found at [Xilinx Data-Analytic Repo]

The instruction to launch an F1 instance can be found [here]

## Steps to Launch XPGBolt

Start by launching two terminals

### Terminal 1
1. Connect to F1 instance
2. Navigate to /home/centos/xpg_demo
    ```
    $ cd /home/centos/xpg_demo
    $ ls
    Dockerfile  afi  postgres_f2  run_root.sh
    ```
3. Run `./run_root.sh xpg_demo_container work_space`
    ```
    $ ./run_root.sh xpg_demo_container work_space
    root@0f077c7d68a7:/# 
    ```
4. Now you are inside docker container, navigate to /home/work_space/demo
    ```
    $ cd /home/work_space/demo
    $ ls
    loadfpga.sql  postgres_f2  queries  run_pg.sh
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
2. Run `sudo docker exec –it xpg_demo_container /bin/bash`
    ```
    $ sudo docker exec –it xpg_demo_container /bin/bash
    root@0f077c7d68a7:/# 
    ```
3. Navigate to /home/work_space/demo
    ```
    $ cd /home/work_space/demo
    $ ls
    loadfpga.sql  postgres_f2  queries  run_pg.sh
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
8. Load XPGBolt library at runtime. This step will load the .so file and load FPGA bitstream
    ```
    tpch_1g=# \i loadfpga.sql
    LOAD
    CREATE FUNCTION
    CREATE FUNCTION
    SET
    tpch_1g=# 
    ```
9. Run TPCH Query 6
    ```
    tpch_1g=# \i queries/6.sql
        revenue    
        ---------------
        45040342.8530
        (1 row)
    ```
10. Turn on timing and rerun query 6 to see the runtime
    ```
    tpch_1g=# \timing on
    tpch_1g=# \i queries/6.sql
    Time: 1086.218 ms
    tpch_1g=# \timing off
    ```
11. Run TPCH Query 12
    ```
    tpch_1g=# \i queries/12.sql
     l_shipmode | high_line_count | low_line_count 
    ------------+-----------------+----------------
     MAIL       |            6252 |           9376
     SHIP       |            6262 |           9439
    (2 rows)
    ```

12. Turn on timing and rerun query 12
    ```
    tpch_1g=# \timing on
    tpch_1g=# \i queries/12.sql
    Time: 1203.786 ms
    tpch_1g=# \timing off
    ```
    
[here]: https://github.com/Xilinx/ML-Development-Stack-From-Xilinx/blob/master/launching_instance.md
[Xilinx Data-Analytic Repo]: https://github.com/Xilinx/data-analytics/tree/master/xpg/host



