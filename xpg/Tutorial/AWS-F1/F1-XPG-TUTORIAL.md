# Data Analytics XPGBolt Version 18.03.19 Tutorial

## Introduction
This tutorial introduces how to launch and use XPGBolt, a PostgreSQL based FPGA accelerating library in AWS EC2 F1 Instance. 

The instruction to launch an F1 instance can be found [here]

XPG Amazon AWS AMI can be found at [AWS Data Analytics Acceleration Stack using Postgres] page

The source code of XPG is available upon request to Xilinx customer support.

## Steps to Launch XPGBolt

Start by launching two terminals. The purpose for the first terminal is to launch the database server, and the second terminal is used to connect to the database server and run queries.

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

4. turn off pager
    ```
    tpch_1g=#
    tpch_1g=# \pset pager off
    Pager usage is off.
    ```

5. The 1GB TPC-H dataset contains multiple tables, to view the list of all tables, run the command below
    ```
    tpch_1g=# \dt
              List of relations
     Schema |   Name   | Type  | Owner   
    --------+----------+-------+--------
     public | customer | table | centos
     public | lineitem | table | centos
     public | nation   | table | centos
     public | orders   | table | centos
     public | part     | table | centos
     public | partsupp | table | centos
     public | region   | table | centos
     public | supplier | table | centos
    (8 rows)

    tpch_1g=# \d+;
                                List of relations
     Schema |          Name          |   Type   | Owner  |    Size    | Description 
    --------+------------------------+----------+--------+------------+-------------
     public | customer               | table    | centos | 29 MB      | 
     public | customer_c_custkey_seq | sequence | centos | 8192 bytes | 
     public | lineitem               | table    | centos | 950 MB     | 
     public | nation                 | table    | centos | 8192 bytes | 
     public | nation_n_nationkey_seq | sequence | centos | 8192 bytes | 
     public | orders                 | table    | centos | 216 MB     | 
     public | orders_o_orderkey_seq  | sequence | centos | 8192 bytes | 
     public | part                   | table    | centos | 32 MB      | 
     public | part_p_partkey_seq     | sequence | centos | 8192 bytes | 
     public | partsupp               | table    | centos | 143 MB     | 
     public | region                 | table    | centos | 8192 bytes | 
     public | region_r_regionkey_seq | sequence | centos | 8192 bytes | 
     public | supplier               | table    | centos | 1848 kB    | 
     public | supplier_s_suppkey_seq | sequence | centos | 8192 bytes | 
    (14 rows)
    ```
6. Load XPGBolt library at runtime. This step will load the .so file and load FPGA bitstream
    ```
    tpch_1g=# load '/home/centos/libs/xpgbolt.so';
    LOAD
    tpch_1g=# CREATE OR REPLACE FUNCTION init_fpga(text) RETURNS void AS '/home/centos/libs/xpgbolt.so', 'init_fpga' LANGUAGE C STRICT;
    CREATE FUNCTION
    tpch_1g=# CREATE OR REPLACE FUNCTION rel_fpga() RETURNS void AS '/home/centos/libs/xpgbolt.so', 'release_fpga' LANGUAGE C STRICT;
    CREATE FUNCTION
    tpch_1g=# set xpgbolt.fpga_offload=on;
    SET
    tpch_1g=# select init_fpga('/home/centos/awsxclbin/fpga_sql_hw.awsxclbin');
     init_fpga 
    -----------
 
    (1 row)
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
        123141078.2283
        (1 row)
    ```
    FPGA kernel performance data is stored in `/var/lib/postgresql/9.6/main/sdaccel_profile_summary.html`
    Please open it in a browser to view.

8. Turn on timing and rerun query 6 to see the runtime
    ```
    tpch_1g=# \timing on
    tpch_1g=# \i queries/6.sql
        revenue     
    ----------------
     123141078.2283
    (1 row)

    Time: 1227.636 ms
    tpch_1g=# \timing off
    ```

11. Run TPCH Query 12. Query 12 is a more complex query involving `lineitem` table and `orders` table.
    We are also offloading the scan-filtering portion or the query
    ```
    tpch_1g=# \i queries/12.sql
     l_shipmode | high_line_count | low_line_count 
    ------------+-----------------+----------------
     AIR        |            6290 |           9263
    (2 rows)
    ```

12. Turn on timing and rerun query 12
    ```
    tpch_1g=# \timing on
    tpch_1g=# \i queries/12.sql
     l_shipmode | high_line_count | low_line_count 
    ------------+-----------------+----------------
     AIR        |            6290 |           9263
    (1 row)

    Time: 1621.538 ms
    tpch_1g=# \timing off
    ```

13. Exit Postgresql
    ```
    tpch_1g=# \q
    ```
    
[here]: https://github.com/Xilinx/SDAccel_Examples/wiki/Create,-configure-and-test-an-AWS-F1-instance
[AWS Data Analytics Acceleration Stack using Postgres]: https://aws.amazon.com/marketplace/pp/B07BVSZL51
[TPC-H Homepage]: http://www.tpc.org/tpch/
