Data Analytics Accelerator Testbench Data
============================================

This file details the installation of the TPC-H tables in PostgreSQL.

1. PREREQUISITES
2. INSTALLATION

## 1. PREREQUISITES
PostgreSQL must be installed. A server must be running and a client must be connected.

## 2. INSTALLATION

### Uncompress the tables from the tar.gz file.
```
% tar xzfv tables-0.1g.tar.gz
```

### Open a PostgreSQL client, connect to the appropriate database.

### In the client shell, set the path where the tar.gz file was uncompressed.
```
tpch_0.1g=# set myPath '/wrk/xilinx/tpch0.1g'
```

### Build the tpch tables.
```
tpch_0.1g=# \i /wrk/xilinx/tpch0.1g/tpch-build-db.sql
```
