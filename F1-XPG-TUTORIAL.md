# Data Analytics XPGBolt Version 18.03.19 Tutorial

## Introduction
This tutorial introduces how to launch and use XPGBolt, a PostgreSQL based FPGA accelerating library in AWS EC2 F1 Instance. 

The instruction to launch an F1 instance can be found [here]

XPG Amazon AWS AMI can be found at [AWS Data Analytics Acceleration Stack using Postgres] page

The source code of XPG is available upon request to Xilinx customer support.

## Steps to Launch XPGBolt

Start by launching two terminals. The purpose for the first terminal is to launch the database server, and the second terminal is used to connect to the database server and run queries.

### Terminal 1
1. Connect to F1 instance
2. Navigate to /home/centos/xpg_demo
    ```
    $ cd /home/centos/xpg_demo
    $ ls
    Dockerfile  afi  postgres_f2  run_root.sh
    ```
3. The demo will run within a docker container. The purpose of using docker is to create an environment isolated from the main disk.
   Start by launching docker service
    ```
    $ sudo service docker start
    Redirecting to /bin/systemctl start docker.service
    ```
   To launch a docker container, run `./run_root.sh xpg_demo_container work_space`
    ```
    $ ./run_root.sh xpg_demo_container work_space
    root@0f077c7d68a7:/#
    ```
   If there is an error message indicating container name conflict, there are two options available:
   
   a. Remove old container and relaunch the new container
   
       ```
       $ sudo docker stop xpg_demo_container
       xpg_demo_container
       $ sudo docker rm xpg_demo_container
       xpg_demo_container
       $ ./run_root.sh xpg_demo_container work_space
       root@0f077c7d68a7:/#
       ```
       
   b. Make sure the existing container is active, and directly connect to the existing container
   
       ```
       $ sudo docker start xpg_demo_container
       xpg_demo_container
       $ sudo docker exec -it xpg_demo_container /bin/bash
       root@0f077c7d68a7:/# 
       ```
       
4. Now you are inside docker container and ready to launch PostgreSQL
    ```
    $ cd /home/work_space/demo
    $ ls
    loadfpga.sql  postgres_f2  queries  run_pg.sh
    $ ./run_pg.sh
    2018-01-23 22:38:46.530 UTC [21] LOG:  database system was shut down at 2018-01-08 21:45:59 UTC
    2018-01-23 22:38:46.538 UTC [21] LOG:  MultiXact member wraparound protections are now enabled
    2018-01-23 22:38:46.540 UTC [25] LOG:  autovacuum launcher started
    2018-01-23 22:38:46.541 UTC [20] LOG:  database system is ready to accept connections
    ```
    At this time the PostgreSQL server is launched and listening to user connections. 
    The launched database is preloaded with TPC-H 1GB dataset for demo purposes. To learn more about TPC-H benchmark, please visit [TPC-H Homepage].

### Terminal 2
1. Connect to the same F1 instance
2. Connect to the same docker instance that was created in Terminal 1
    ```
    $ sudo docker exec -it xpg_demo_container /bin/bash
    root@0f077c7d68a7:/# 
    ```
    Now you are inside docker container
3. Navigate to /home/work_space/demo
    ```
    $ cd /home/work_space/demo
    $ ls
    loadfpga.sql  postgres_f2  queries  run_pg.sh
    ```
4. Change user to `postgres`
    ```
    $ su postgres
    postgres@0f077c7d68a7:
    ```
5. Run `psql tpch_1g` to connect to 1GB TPC-H database. After logging in, turn off pager
    ```
    $ psql tpch_1g
    psql (9.6.3, server 9.6.2)
    Type "help" for help.

    tpch_1g=#
    tpch_1g=# \pset pager off
    Pager usage is off.
    ``` 
   The 1GB TPC-H dataset contains multiple tables, to view the list of all tables, run the command below
    ```
    tpch_1g=# \dt
              List of relations
     Schema |   Name   | Type  |  Owner   
    --------+----------+-------+----------
     public | customer | table | postgres
     public | lineitem | table | postgres
     public | nation   | table | postgres
     public | orders   | table | postgres
     public | part     | table | postgres
     public | partsupp | table | postgres
     public | region   | table | postgres
     public | supplier | table | postgres
    (8 rows)

    tpch_1g=# \d+;
                                List of relations
     Schema |          Name          |   Type   |  Owner   |    Size    | Description 
    --------+------------------------+----------+----------+------------+-------------
     public | customer               | table    | postgres | 29 MB      | 
     public | customer_c_custkey_seq | sequence | postgres | 8192 bytes | 
     public | lineitem               | table    | postgres | 950 MB     | 
     public | nation                 | table    | postgres | 8192 bytes | 
     public | nation_n_nationkey_seq | sequence | postgres | 8192 bytes | 
     public | orders                 | table    | postgres | 216 MB     | 
     public | orders_o_orderkey_seq  | sequence | postgres | 8192 bytes | 
     public | part                   | table    | postgres | 32 MB      | 
     public | part_p_partkey_seq     | sequence | postgres | 8192 bytes | 
     public | partsupp               | table    | postgres | 143 MB     | 
     public | region                 | table    | postgres | 8192 bytes | 
     public | region_r_regionkey_seq | sequence | postgres | 8192 bytes | 
     public | supplier               | table    | postgres | 1848 kB    | 
     public | supplier_s_suppkey_seq | sequence | postgres | 8192 bytes | 
    (14 rows)
    ```
6. Load XPGBolt library at runtime. This step will load the .so file and load FPGA bitstream
    ```
    tpch_1g=# \i loadfpga.sql
    LOAD
    CREATE FUNCTION
    CREATE FUNCTION
    SET
    tpch_1g=# 
    ```
7. Run TPCH Query 6. Query 6 is a Scan-Aggregation query working on `lineitem` table. 

   To get the number of tuples in this table
    ```
    tpch_1g=# select count( * ) from lineitem;
      count  
    ---------
     6001215
    (1 row)
    ```

   In the current demo we are offloading the Scanfiltering part of this query.
    ```
    tpch_1g=# \i queries/6.sql
        revenue    
        ---------------
        45040342.8530
        (1 row)
    ```
    FPGA kernel performance data is stored in `/var/lib/postgresql/9.6/main/sdaccel_profile_summary.html`
    Please open it in a browser to view.

8. Turn on timing and rerun query 6 to see the runtime
    ```
    tpch_1g=# \timing on
    tpch_1g=# \i queries/6.sql
    Time: 1086.218 ms
    tpch_1g=# \timing off
    ```
11. Run TPCH Query 12. Query 12 is a more complex query involving `lineitem` table and `orders` table.
    We are also offloading the scan-filtering portion or the query
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
    
[here]: https://github.com/Xilinx/SDAccel_Examples/wiki/Create,-configure-and-test-an-AWS-F1-instance
[AWS Data Analytics Acceleration Stack using Postgres]: https://aws.amazon.com/marketplace/pp/B07BVSZL51
[TPC-H Homepage]: http://www.tpc.org/tpch/