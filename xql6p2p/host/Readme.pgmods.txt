

Postgres Modifications and working with the modified postgres server:

xql6 P2P includes following modifications to the postgresql server:

1.
In P2P mode, application/FS/OS caches needs to be bypassed because the host
(x86) memory is no longer involved and the data movement happens directly
between the SSD and the FPGA DDR. For this:
postgres's mdopen() [src/backend/storage/smgr/md.c]
is modified which adds O_DIRECT to P2P table reads.

2.
Performance:

xql6 P2P defines a new function in postgres - mdread_bigd() - which reads the postgres table
data in big chunks (based on the arguments - xql6 P2P reads in 2MB chunks); the
original base postgresql mdread() reads only in 4KB chunks giving a low read BW from the SSD.

Following are the steps to build the postgres server with these modifications
and then working with it.

 
1. Download the sources
....
root@sushil-PowerEdge-T640:/tmp/p# wget https://ftp.postgresql.org/pub/source/v9.6.8/postgresql-9.6.8.tar.bz2
--2018-05-03 18:07:52--  https://ftp.postgresql.org/pub/source/v9.6.8/postgresql-9.6.8.tar.bz2
Resolving proxy (proxy)... 172.23.84.111
Connecting to proxy (proxy)|172.23.84.111|:8080... connected.
Proxy request sent, awaiting response... 200 OK
Length: 19528927 (19M) [application/x-bzip-compressed-tar]
Saving to: ‘postgresql-9.6.8.tar.bz2’

postgresql-9.6.8.tar.bz2  100%[=====================================>]  18.62M  2.15MB/s    in 18s     

2018-05-03 18:08:12 (1.01 MB/s) - ‘postgresql-9.6.8.tar.bz2’ saved [19528927/19528927]

root@sushil-PowerEdge-T640:/tmp/p# 
....


2. Apply the patch 'postgresql-9.6.8.patch':

....
root@sushil-PowerEdge-T640:/tmp/p# ls -l
total 19084
-rw-r--r-- 1 root root     9725 May  3 17:43 postgresql-9.6.8.patch
-rw-r--r-- 1 root root 19528927 Feb 27 04:01 postgresql-9.6.8.tar.bz2
root@sushil-PowerEdge-T640:/tmp/p# 
root@sushil-PowerEdge-T640:/tmp/p# 
root@sushil-PowerEdge-T640:/tmp/p# tar xfj postgresql-9.6.8.tar.bz2 
root@sushil-PowerEdge-T640:/tmp/p# 
root@sushil-PowerEdge-T640:/tmp/p# ls -l 
total 19088
drwxrwxrwx 6 1107 1107     4096 Feb 27 04:00 postgresql-9.6.8
-rw-r--r-- 1 root root     9725 May  3 17:43 postgresql-9.6.8.patch
-rw-r--r-- 1 root root 19528927 Feb 27 04:01 postgresql-9.6.8.tar.bz2
root@sushil-PowerEdge-T640:/tmp/p# 
root@sushil-PowerEdge-T640:/tmp/p# 
root@sushil-PowerEdge-T640:/tmp/p# patch -p0 < postgresql-9.6.8.patch 
patching file postgresql-9.6.8/src/backend/storage/smgr/md.c
patching file postgresql-9.6.8/src/include/storage/smgr.h
root@sushil-PowerEdge-T640:/tmp/p# 
....



3. Build postgresql server and client.

./configure; make; make install


This will create and populate the '/usr/local/pgsql/lib' directory for postgres,
and also install the postgres server - 'postgres' and postgresql client - 'psql'.
Copy these binaries into say /demo/postgresql/ and run them from there.



cp ./src/backend/postgres /demo/postgresql
cp ./src/bin/psql/psql /demo/postgresql



4: Start the postgresql server.


postgres@sushil-PowerEdge-T640:/demo/postgresql$ 
postgres@sushil-PowerEdge-T640:/demo/postgresql$ cd /demo/postgresql/
postgres@sushil-PowerEdge-T640:/demo/postgresql$ 
postgres@sushil-PowerEdge-T640:/demo/postgresql$ ./postgres -D <database dir path>
LOG:  database system was shut down at 2018-05-03 20:19:31 IST
LOG:  MultiXact member wraparound protections are now enabled
LOG:  database system is ready to accept connections
LOG:  autovacuum launcher started


Leave the postgres server running in that terminal in the foreground and
open new terminals to run the psql client.
Please remove the -D option to run the server in background.

