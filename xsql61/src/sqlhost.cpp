/**********
Copyright (c) 2017, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

/*
 * SQL Host compilation requires Postgre or MySQL compilation. This  code is provided
 * as is. Main code can be modified to use sqlhost.h and sqlhost.cpp files. 
 * This was supported earlier, however it uses API based connectivity which is much
 * slower than native postgresql connectivity. This has been removed form the main
 * code and support is now provided for Postgres with its native storage formats.
 */
  
#include "sqlhost.h"
#include <my_global.h>
#include <mysql.h>

const char* SQLHost::tpchQueryText[22] = {
  // Query 1
"select \
	l_returnflag, \
	l_linestatus, \
	sum(l_quantity) as sum_qty, \
	sum(l_extendedprice) as sum_base_price, \
	sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, \
	sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, \
	avg(l_quantity) as avg_qty, \
	avg(l_extendedprice) as avg_price, \
	avg(l_discount) as avg_disc, \
	count(*) as count_order \
from \
	lineitem \
where \
	l_shipdate <= date '1998-12-01' - interval '108' day \
group by \
	l_returnflag, \
	l_linestatus \
order by \
	l_returnflag, \
	l_linestatus",
// Query 2
"select \
	s_acctbal, \
	s_name, \
	n_name, \
	p_partkey, \
	p_mfgr, \
	s_address, \
	s_phone, \
	s_comment \
from \
	part, \
	supplier, \
	partsupp, \
	nation, \
	region \
where \
	p_partkey = ps_partkey \
	and s_suppkey = ps_suppkey \
	and p_size = 30 \
	and p_type like '%STEEL' \
	and s_nationkey = n_nationkey \
	and n_regionkey = r_regionkey \
	and r_name = 'ASIA' \
	and ps_supplycost = ( \
		select \
			min(ps_supplycost) \
		from \
			partsupp, \
			supplier, \
			nation, \
			region \
		where \
			p_partkey = ps_partkey \
			and s_suppkey = ps_suppkey \
			and s_nationkey = n_nationkey \
			and n_regionkey = r_regionkey \
			and r_name = 'ASIA' \
	) \
order by \
	s_acctbal desc, \
	n_name, \
	s_name, \
	p_partkey \
limit 100",
// Query 3
"select \
	l_orderkey, \
	sum(l_extendedprice * (1 - l_discount)) as revenue, \
	o_orderdate, \
	o_shippriority \
from \
	customer, \
	orders, \
	lineitem \
where \
	c_mktsegment = 'AUTOMOBILE' \
	and c_custkey = o_custkey \
	and l_orderkey = o_orderkey \
	and o_orderdate < date '1995-03-13' \
	and l_shipdate > date '1995-03-13' \
group by \
	l_orderkey, \
	o_orderdate, \
	o_shippriority \
order by \
	revenue desc, \
	o_orderdate \
limit 10",
// Query 4
"select \
	o_orderpriority, \
	count(*) as order_count \
from \
	orders \
where \
	o_orderdate >= date '1995-01-01' \
	and o_orderdate < date '1995-01-01' + interval '3' month \
	and exists ( \
		select \
			* \
		from \
			lineitem \
		where \
			l_orderkey = o_orderkey \
			and l_commitdate < l_receiptdate \
	) \
group by \
	o_orderpriority \
order by \
	o_orderpriority",
// Query 5
"select \
	n_name, \
	sum(l_extendedprice * (1 - l_discount)) as revenue \
from \
	customer, \
	orders, \
	lineitem, \
	supplier, \
	nation, \
	region \
where \
	c_custkey = o_custkey \
	and l_orderkey = o_orderkey \
	and l_suppkey = s_suppkey \
	and c_nationkey = s_nationkey \
	and s_nationkey = n_nationkey \
	and n_regionkey = r_regionkey \
	and r_name = 'MIDDLE EAST' \
	and o_orderdate >= date '1994-01-01' \
	and o_orderdate < date '1994-01-01' + interval '1' year \
group by \
	n_name \
order by \
revenue desc",
// Query 6
"select \
	sum(l_extendedprice * l_discount) as revenue \
from \
	lineitem \
where \
	l_shipdate >= date '1994-01-01'\
        and l_shipdate < date '1994-01-01' + interval '1' year \
	and l_discount between 0.06 - 0.01 and 0.06 + 0.01 \
	and l_quantity < 24",
// Query 7
"select \
	supp_nation, \
	cust_nation, \
	l_year, \
	sum(volume) as revenue \
from \
	( \
		select \
			n1.n_name as supp_nation, \
			n2.n_name as cust_nation, \
			extract(year from l_shipdate) as l_year, \
			l_extendedprice * (1 - l_discount) as volume \
		from \
			supplier, \
			lineitem, \
			orders, \
			customer, \
			nation n1, \
			nation n2 \
		where \
			s_suppkey = l_suppkey \
			and o_orderkey = l_orderkey \
			and c_custkey = o_custkey \
			and s_nationkey = n1.n_nationkey \
			and c_nationkey = n2.n_nationkey \
			and ( \
				(n1.n_name = 'JAPAN' and n2.n_name = 'INDIA') \
				or (n1.n_name = 'INDIA' and n2.n_name = 'JAPAN') \
			) \
			and l_shipdate between date '1995-01-01' and date '1996-12-31' \
	) as shipping \
group by \
	supp_nation, \
	cust_nation, \
	l_year \
order by \
	supp_nation, \
	cust_nation, \
	l_year",
// Query 8
"select \
	o_year, \
	sum(case \
		when nation = 'INDIA' then volume \
		else 0 \
	end) / sum(volume) as mkt_share \
from \
	( \
		select \
			extract(year from o_orderdate) as o_year, \
			l_extendedprice * (1 - l_discount) as volume, \
			n2.n_name as nation \
		from \
			part, \
			supplier, \
			lineitem, \
			orders, \
			customer, \
			nation n1, \
			nation n2, \
			region \
		where \
			p_partkey = l_partkey \
			and s_suppkey = l_suppkey \
			and l_orderkey = o_orderkey \
			and o_custkey = c_custkey \
			and c_nationkey = n1.n_nationkey \
			and n1.n_regionkey = r_regionkey \
			and r_name = 'ASIA' \
			and s_nationkey = n2.n_nationkey \
			and o_orderdate between date '1995-01-01' and date '1996-12-31' \
			and p_type = 'SMALL PLATED COPPER' \
	) as all_nations \
group by \
	o_year \
order by \
	o_year",
// Query 9
"select \
	nation, \
	o_year, \
	sum(amount) as sum_profit \
from \
	( \
		select \
			n_name as nation, \
			extract(year from o_orderdate) as o_year, \
			l_extendedprice * (1 - l_discount) - ps_supplycost * l_quantity as amount \
		from \
			part, \
			supplier, \
			lineitem, \
			partsupp, \
			orders, \
			nation \
		where \
			s_suppkey = l_suppkey \
			and ps_suppkey = l_suppkey \
			and ps_partkey = l_partkey \
			and p_partkey = l_partkey \
			and o_orderkey = l_orderkey \
			and s_nationkey = n_nationkey \
			and p_name like '%dim%' \
	) as profit \
group by \
	nation, \
	o_year \
order by \
	nation, \
	o_year desc",
// Query 10
"select \
	c_custkey, \
	c_name, \
	sum(l_extendedprice * (1 - l_discount)) as revenue, \
	c_acctbal, \
	n_name, \
	c_address, \
	c_phone, \
	c_comment \
from \
	customer, \
	orders, \
	lineitem, \
	nation \
where \
	c_custkey = o_custkey \
	and l_orderkey = o_orderkey \
	and o_orderdate >= date '1993-08-01' \
	and o_orderdate < date '1993-08-01' + interval '3' month \
	and l_returnflag = 'R' \
	and c_nationkey = n_nationkey \
group by \
	c_custkey, \
	c_name, \
	c_acctbal, \
	c_phone, \
	n_name, \
	c_address, \
	c_comment \
order by \
	revenue desc \
limit 20",
// Query 11
"select \
	ps_partkey, \
	sum(ps_supplycost * ps_availqty) as value \
from \
	partsupp, \
	supplier, \
	nation \
where \
	ps_suppkey = s_suppkey \
	and s_nationkey = n_nationkey \
	and n_name = 'MOZAMBIQUE' \
group by \
	ps_partkey having \
		sum(ps_supplycost * ps_availqty) > ( \
			select \
				sum(ps_supplycost * ps_availqty) * 0.0001000000 \
			from \
				partsupp, \
				supplier, \
				nation \
			where \
				ps_suppkey = s_suppkey \
				and s_nationkey = n_nationkey \
				and n_name = 'MOZAMBIQUE' \
		) \
order by \
	value desc",
// Query 12
"select \
	l_shipmode, \
	sum(case \
		when o_orderpriority = '1-URGENT' \
			or o_orderpriority = '2-HIGH' \
			then 1 \
		else 0 \
	end) as high_line_count, \
	sum(case \
		when o_orderpriority <> '1-URGENT' \
			and o_orderpriority <> '2-HIGH' \
			then 1 \
		else 0 \
	end) as low_line_count \
from \
	orders, \
	lineitem \
where \
	o_orderkey = l_orderkey \
	and l_shipmode in ('RAIL', 'FOB') \
	and l_commitdate < l_receiptdate \
	and l_shipdate < l_commitdate \
	and l_receiptdate >= date '1997-01-01' \
	and l_receiptdate < date '1997-01-01' + interval '1' year \
group by \
	l_shipmode \
order by \
	l_shipmode",
// Query 13
"select \
	c_count, \
	count(*) as custdist \
from \
	( \
		select \
			c_custkey, \
			count(o_orderkey) as c_count \
		from \
			customer left outer join orders on \
				c_custkey = o_custkey \
				and o_comment not like '%pending%deposits%' \
		group by \
			c_custkey \
	) c_orders \
group by \
	c_count \
order by \
	custdist desc, \
	c_count desc",
// Query 14
"select \
	100.00 * sum(case \
		when p_type like 'PROMO%' \
			then l_extendedprice * (1 - l_discount) \
		else 0 \
	end) / sum(l_extendedprice * (1 - l_discount)) as promo_revenue \
from \
	lineitem, \
	part \
where \
	l_partkey = p_partkey \
	and l_shipdate >= date '1996-12-01' \
	and l_shipdate < date '1996-12-01' + interval '1' month",
// Query 15
"create view revenue0 (supplier_no, total_revenue) as \
	select \
		l_suppkey, \
		sum(l_extendedprice * (1 - l_discount)) \
	from \
		lineitem \
	where \
		l_shipdate >= date '1997-07-01' \
		and l_shipdate < date '1997-07-01' + interval '3' month \
	group by \
		l_suppkey; \
 \
 \
select \
	s_suppkey, \
	s_name, \
	s_address, \
	s_phone, \
	total_revenue \
from \
	supplier, \
	revenue0 \
where \
	s_suppkey = supplier_no \
	and total_revenue = ( \
		select \
			max(total_revenue) \
		from \
			revenue0 \
	) \
order by \
	s_suppkey; \
 \
drop view revenue0",
// Query 16
"select \
	p_brand, \
	p_type, \
	p_size, \
	count(distinct ps_suppkey) as supplier_cnt \
from \
	partsupp, \
	part \
where \
	p_partkey = ps_partkey \
	and p_brand <> 'Brand#34' \
	and p_type not like 'LARGE BRUSHED%' \
	and p_size in (48, 19, 12, 4, 41, 7, 21, 39) \
	and ps_suppkey not in ( \
		select \
			s_suppkey \
		from \
			supplier \
		where \
			s_comment like '%Customer%Complaints%' \
	) \
group by \
	p_brand, \
	p_type, \
	p_size \
order by \
	supplier_cnt desc, \
	p_brand, \
	p_type, \
	p_size",
// Query 17
"select \
	sum(l_extendedprice) / 7.0 as avg_yearly \
from \
	lineitem, \
	part \
where \
	p_partkey = l_partkey \
	and p_brand = 'Brand#44' \
	and p_container = 'WRAP PKG' \
	and l_quantity < ( \
		select \
			0.2 * avg(l_quantity) \
		from \
			lineitem \
		where \
			l_partkey = p_partkey \
	)",
// Query 18
"select \
	c_name, \
	c_custkey, \
	o_orderkey, \
	o_orderdate, \
	o_totalprice, \
	sum(l_quantity) \
from \
	customer, \
	orders, \
	lineitem \
where \
	o_orderkey in ( \
		select \
			l_orderkey \
		from \
			lineitem \
		group by \
			l_orderkey having \
				sum(l_quantity) > 314 \
	) \
	and c_custkey = o_custkey \
	and o_orderkey = l_orderkey \
group by \
	c_name, \
	c_custkey, \
	o_orderkey, \
	o_orderdate, \
	o_totalprice \
order by \
	o_totalprice desc, \
	o_orderdate \
limit 100",
// Query 19
"select \
	sum(l_extendedprice* (1 - l_discount)) as revenue \
from \
	lineitem, \
	part \
where \
	( \
		p_partkey = l_partkey \
		and p_brand = 'Brand#52' \
		and p_container in ('SM CASE', 'SM BOX', 'SM PACK', 'SM PKG') \
		and l_quantity >= 4 and l_quantity <= 4 + 10 \
		and p_size between 1 and 5 \
		and l_shipmode in ('AIR', 'AIR REG') \
		and l_shipinstruct = 'DELIVER IN PERSON' \
	) \
	or \
	( \
		p_partkey = l_partkey \
		and p_brand = 'Brand#11' \
		and p_container in ('MED BAG', 'MED BOX', 'MED PKG', 'MED PACK') \
		and l_quantity >= 18 and l_quantity <= 18 + 10 \
		and p_size between 1 and 10 \
		and l_shipmode in ('AIR', 'AIR REG') \
		and l_shipinstruct = 'DELIVER IN PERSON' \
	) \
	or \
	( \
		p_partkey = l_partkey \
		and p_brand = 'Brand#51' \
		and p_container in ('LG CASE', 'LG BOX', 'LG PACK', 'LG PKG') \
		and l_quantity >= 29 and l_quantity <= 29 + 10 \
		and p_size between 1 and 15 \
		and l_shipmode in ('AIR', 'AIR REG') \
		and l_shipinstruct = 'DELIVER IN PERSON' \
	)",
// Query 20
"select \
	s_name, \
	s_address \
from \
	supplier, \
	nation \
where \
	s_suppkey in ( \
		select \
			ps_suppkey \
		from \
			partsupp \
		where \
			ps_partkey in ( \
				select \
					p_partkey \
				from \
					part \
				where \
					p_name like 'green%' \
			) \
			and ps_availqty > ( \
				select \
					0.5 * sum(l_quantity) \
				from \
					lineitem \
				where \
					l_partkey = ps_partkey \
					and l_suppkey = ps_suppkey \
					and l_shipdate >= date '1993-01-01' \
					and l_shipdate < date '1993-01-01' + interval '1' year \
			) \
	) \
	and s_nationkey = n_nationkey \
	and n_name = 'ALGERIA' \
order by \
	s_name",
// Query 21
"select \
	s_name, \
	count(*) as numwait \
from \
	supplier, \
	lineitem l1, \
	orders, \
	nation \
where \
	s_suppkey = l1.l_suppkey \
	and o_orderkey = l1.l_orderkey \
	and o_orderstatus = 'F' \
	and l1.l_receiptdate > l1.l_commitdate \
	and exists ( \
		select \
			* \
		from \
			lineitem l2 \
		where \
			l2.l_orderkey = l1.l_orderkey \
			and l2.l_suppkey <> l1.l_suppkey \
	) \
	and not exists ( \
		select \
			* \
		from \
			lineitem l3 \
		where \
			l3.l_orderkey = l1.l_orderkey \
			and l3.l_suppkey <> l1.l_suppkey \
			and l3.l_receiptdate > l3.l_commitdate \
	) \
	and s_nationkey = n_nationkey \
	and n_name = 'EGYPT' \
group by \
	s_name \
order by \
	numwait desc, \
	s_name \
limit 100",
// Query 22
"select \
	cntrycode, \
	count(*) as numcust, \
	sum(c_acctbal) as totacctbal \
from \
	( \
		select \
			substring(c_phone from 1 for 2) as cntrycode, \
			c_acctbal \
		from \
			customer \
		where \
			substring(c_phone from 1 for 2) in \
				('20', '40', '22', '30', '39', '42', '21') \
			and c_acctbal > ( \
				select \
					avg(c_acctbal) \
				from \
					customer \
				where \
					c_acctbal > 0.00 \
					and substring(c_phone from 1 for 2) in \
						('20', '40', '22', '30', '39', '42', '21') \
			) \
			and not exists ( \
				select \
					* \
				from \
					orders \
				where \
					o_custkey = c_custkey \
			) \
	) as custsale \
group by \
	cntrycode \
order by \
	cntrycode" };

void SQLHost::setSqlDb(const char *db) {
  sqldb = CSV;
  if (strcmp(db,"pgsql") == 0)
    sqldb = PGSQL;
  else if (strcmp(db,"mysql") == 0)
    sqldb = MySQL;
  else
    csvFileName = db;
};

int SQLHost::connect() {
  int status = EXIT_SUCCESS;
  if (sqldb == MySQL)
    status = connectMySQL();
  else if (sqldb == PGSQL)
    status = connectPGSQL();
  else
    status = connectCSV();
      
  /*  query("show tables");
  query("SELECT * FROM customer LIMIT 10");
  query("SELECT COUNT(*) FROM customer");
  query("SELECT COUNT(*) FROM lineitem");
  query("SELECT COUNT(*) FROM nation");
  query("SELECT COUNT(*) FROM orders");
  query("SELECT COUNT(*) FROM part");
  query("SELECT COUNT(*) FROM partsupp");
  query("SELECT COUNT(*) FROM region");
  query("SELECT COUNT(*) FROM supplier;"); */
  return status;
  
}

int SQLHost::connectCSV() {
  // Load csv file to memory
  csvFile= NULL;
  const char * filename = csvFileName.c_str();
  size_t size = 0;
  FILE *f = fopen(filename, "rb");
  if (f == NULL) {
    fprintf(stderr,"Cannot open file: %s\n", filename);
    return EXIT_FAILURE;
  } 
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);
  csvFile = (char *)malloc(size+1);
  if (size != fread(csvFile, sizeof(char), size, f)) {
    fprintf(stderr,"Cannot read file: %s\n", filename);
    free(csvFile);
    return EXIT_FAILURE;    
  }
  fclose(f);
  csvFile[size] = 0;
  curVarCh = csvFile;
  processedTuples = 0;
  
  return EXIT_SUCCESS;
}

//PGSQL Host calls
int SQLHost::connectPGSQL() {
  char buff[256];
  sprintf(buff, "dbname=postgres host=%s port=%d user=postgres password=xilinx",
	  sqlhostname.c_str(), sqlportid);
  //std::stringstream bufStr;
  //bufStr << "dbname=postgres host="<< sqlhostname << " port=" << sqlportid << " user=postgres password=xilinx";
  //const char *buff = bufStr.str().c_str();
  pgsqlCon = PQconnectdb(buff);
  
  if( PQstatus(pgsqlCon) != CONNECTION_OK ) {
    fprintf(stderr,"Connection failed: %s\n", PQerrorMessage(pgsqlCon) );
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

// MySQL host calls
int SQLHost::connectMySQL() {
  mysqlCon = mysql_init(NULL);
  if (mysqlCon == NULL)   {
    fprintf(stderr, "%s\n", mysql_error(mysqlCon));
    return EXIT_FAILURE;
  }
  unsigned int conn_timeout=1;
  // make user id and password argument if needed for protection
  mysql_options(mysqlCon, MYSQL_OPT_CONNECT_TIMEOUT, &conn_timeout);
  if (mysql_real_connect(mysqlCon, sqlhostname.c_str(), "root", "xilinx",   
  			 NULL, sqlportid, NULL, 0) == NULL) {  
    fprintf(stderr, "%s\n", mysql_error(mysqlCon));
    mysql_close(mysqlCon);
    return EXIT_FAILURE;
  }
  if (mysql_select_db(mysqlCon,schemaName.c_str())) {
    fprintf(stderr, "%s\n", mysql_error(mysqlCon));
    mysql_close(mysqlCon);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int SQLHost::printQueryResult(bool freeResult) {
  if (sqldb == PGSQL) {
    return (printQueryResultPGSQL(freeResult)); 
  } else {
    return (printQueryResultMySQL(freeResult));
  }
}

int SQLHost::printQueryResultPGSQL(bool freeResult) {
  /*  // Query should have fetched the rows
      pgsqlRes = PQexec(pgsqlCon, "FETCH ALL in my_portal" ); */
  if (!pgsqlRes) {
    printf("Null result\n");
    return 0;
  }

  int nFields = PQnfields(pgsqlRes);
  // Print out the field names
  for(int i=0; i<nFields; i++ )
    printf("%s | ", PQfname(pgsqlRes, i) );
  printf("\n");
  
  // Print out the rows
  while (pgsqlRes) {
    ExecStatusType status =  PQresultStatus(pgsqlRes);
    if (status == PGRES_TUPLES_OK) {
      PQclear(pgsqlRes);
      pgsqlRes = PQgetResult(pgsqlCon);
      continue;
    }
    if (status != PGRES_SINGLE_TUPLE) {
      printf("ERROR, queryresult failed: %s", PQerrorMessage(pgsqlCon) );
      PQclear(pgsqlRes);
      pgsqlRes = PQgetResult(pgsqlCon);      
      continue;
    }
    for(int i = 0; i < nFields; i++)   {
      char *pg_val = PQgetvalue(pgsqlRes,0,i);
      printf("%s | ",pg_val);
    }
    printf("\n");
    PQclear(pgsqlRes);
    pgsqlRes = PQgetResult(pgsqlCon);
  }

  return EXIT_SUCCESS;
}

int SQLHost::printQueryResultMySQL(bool freeResult) {
  // query succeeded, process any data returned by it  
  unsigned int num_fields;
  unsigned int num_rows;
  //printf("Query completed %s\n", queryText);
  if (mysqlRes) {  // there are rows  
    num_fields = mysql_num_fields(mysqlRes);
    // print fields first
    MYSQL_FIELD *fields;
    unsigned int i;
    fields = mysql_fetch_fields(mysqlRes);
    for(i = 0; i < num_fields; i++) {
      printf("[ %s ] ", fields[i].name);
    }
    printf("\n");
    // retrieve rows, then call mysql_free_result(mysqlRes)
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(mysqlRes))) {
      unsigned long *lengths = mysql_fetch_lengths(mysqlRes);
      for(i = 0; i < num_fields; i++)   {
	printf("[%.*s] ", (int) lengths[i],row[i] ? row[i] : "NULL");
      }
      printf("\n");
    }
  } else { // mysql_store_result() returned nothing; should it have?
    if(mysql_field_count(mysqlCon) == 0) {
      // query does not return data
      // (it was not a SELECT)
      num_rows = mysql_affected_rows(mysqlCon);
      printf("No result returned affected rows %d",num_rows);
    } else { // mysql_store_result() should have returned data
      fprintf(stderr, "Error: No result from last query %s\n", mysql_error(mysqlCon));
      return EXIT_FAILURE;      
    }
  }
  if (freeResult)
    mysql_free_result(mysqlRes); 
  return EXIT_SUCCESS;
}


int SQLHost::query(const char *queryText, bool printResult) {
  if (sqldb == PGSQL) {
    //std::stringstream sqlbufStr;
    //sqlbufStr << "BEGIN; SET SCHEMA '"<< schemaName <<"'; DECLARE my_portal CURSOR FOR " << queryText ;
    //const char *sqlbuf = sqlbufStr.str().c_str();
    char sqlbuf[2048];
    //sprintf(sqlbuf, "BEGIN; SET SCHEMA '%s'; DECLARE my_portal CURSOR FOR %s", schemaName.c_str(),queryText);
    //pgsqlRes = PQexec(pgsqlCon, sqlbuf);
    sprintf(sqlbuf, "SET SCHEMA '%s';", schemaName.c_str());
    pgsqlRes = PQexec(pgsqlCon, sqlbuf);
    if( PQresultStatus(pgsqlRes) != PGRES_COMMAND_OK )  {
      printf("Error setting schema %s!: %s\n", sqlbuf, PQerrorMessage(pgsqlCon) );
      PQclear(pgsqlRes);
      return EXIT_FAILURE;
    }
    PQclear(pgsqlRes);
    //queryText = "SELECT * FROM lineitem;";
    sprintf(sqlbuf, "%s;", queryText);    
    if( !PQsendQuery(pgsqlCon, sqlbuf)) {
      printf("Error executing sending SQL %s!: %s\n", sqlbuf, PQerrorMessage(pgsqlCon) );
      return EXIT_FAILURE;
    }
    if (!PQsetSingleRowMode(pgsqlCon)) { // 0 return means that it failed
      printf("ERROR, set single mode Failed: %s\n", PQerrorMessage(pgsqlCon) );
      return EXIT_FAILURE;
    }
    pgsqlRes = PQgetResult(pgsqlCon);
    //printf("DEBUG: query: %s\n", queryText);
    //printf("DEBUG: queryresult rows: %d\n", PQntuples(pgsqlRes));
    ExecStatusType status =  PQresultStatus(pgsqlRes);    
    if( status != PGRES_TUPLES_OK && status != PGRES_SINGLE_TUPLE) {
      printf("ERROR, queryresult failed: %s\n", PQerrorMessage(pgsqlCon) );
      PQclear(pgsqlRes);
      return EXIT_FAILURE;
    }
    
    //pgsqlRes = PQexec(pgsqlCon, "FETCH ALL in my_portal" );
    //pgsqlResRow = 0;
    //if( PQresultStatus(pgsqlRes) != PGRES_TUPLES_OK ) {
    //printf("ERROR, Fetch All Failed: %s", PQerrorMessage(pgsqlCon) );
    //PQclear(pgsqlRes);
    //return EXIT_FAILURE;
    //}
    if (printResult) // if done  prinitng for pgsql fetchnextpage will not work
      return printQueryResult();
    
  } else if (sqldb == MySQL) {
    if (mysql_query(mysqlCon, queryText)) {
      fprintf(stderr, "%s\n", mysql_error(mysqlCon));
      return EXIT_FAILURE;
    }
    mysqlRes = mysql_store_result(mysqlCon);
    if (!mysqlRes) {
      fprintf(stderr, "Error: cannot get results %s\n", mysql_error(mysqlCon));      
      return EXIT_FAILURE;
    }    
    if (printResult) // if done  prinitng for pgsql fetchnextpage will not work
      return printQueryResult();
  }
  return EXIT_SUCCESS;  
}

int SQLHost::queryTpch(int queryNo) {
  if (queryNo <1 || queryNo > 22) {
    fprintf(stderr, "%d not  valid query between 1 and 22\n", queryNo );
    return EXIT_FAILURE;    
  }
  int status = query(tpchQueryText[queryNo-1]);
  return status;
}

void SQLHost::close() {
  if (sqldb == PGSQL) {
    PQfinish(pgsqlCon);
    pgsqlCon = NULL;
  } else {
    mysql_close(mysqlCon);
    mysqlCon = NULL;
  } 
}

//  char rowBuf[globals::max_row_size];
// Use 0, 1,2,3 to store page length
// return false if no more pages to be fetced

bool SQLHost::fetchNextPage(char *bufRam)  {
  if (sqldb == PGSQL) {
    return (fetchNextPagePGSQL(bufRam));
  } else if (sqldb == MySQL) {
    return (fetchNextPageMySQL(bufRam));
  } else {
    return (fetchNextPageCSV(bufRam));    
  }
}

// $$$$ todo
bool SQLHost::fetchNextPageCSV(char *bufRam) {
  
  unsigned int noOfRows = 0;
  unsigned int curBuf = 4;

  bool rowsRemaining = false;

  int curField = 0;

  while(processedTuples < ntuples) {
    char *lastVarCh = curVarCh;

    while (true) {
      if (*curVarCh == '|') {
	if (0x1<<curField & pageFields)      
	  bufRam[curBuf++] = *curVarCh;
	curField++;
	curVarCh++;
	continue;
      }

      if (0x1<<curField & pageFields)      
	bufRam[curBuf++] = *curVarCh;
      curVarCh++;
      
      if (*curVarCh == '\n') {
	//bufRam[curBuf++] = '|';
	bufRam[curBuf++] = *curVarCh++;
	curField = 0;
	break;
      }
      if (curBuf >= (maxPUBlockSize-1)) { // Need min of 2 chars to accomodate next row
	curVarCh = lastVarCh;
	rowsRemaining = true;
	break;
      }
    }
    
    if (rowsRemaining)
      break;
    processedTuples++;
    noOfRows++;
    if (noOfRows >= maxRowsInPUBlock) {
      lastVarCh = curVarCh;
      rowsRemaining = true;
      break;
    }
  }

  bufRam[0] = 0x00; // populated by the number of publocks in qmain.cpp
  bufRam[1]= (noOfRows>>16 & 0xFF);    
  bufRam[2]= (noOfRows>>8 & 0xFF);  
  bufRam[3]= (noOfRows & 0xFF);
  
  return rowsRemaining;
}

bool SQLHost::fetchNextPagePGSQL(char *bufRam) {
  if (!pgsqlRes)
    return false;
 
  //printf("DEBUG: SQLHost::fetchNextPagePGSQL\n");   
  unsigned int noOfRows = 0;
  unsigned int curBuf = 4;
  bool remainingRows = false;
  if (!rowBuf.empty() ) { // Need to to first put last row into current page
    if (copyRowToBuf(bufRam,curBuf) == false) {
      return remainingRows;
    }
    noOfRows++;    
    //printf("DEBUG: rowBuf not empty: noOfRows=%d\n", noOfRows);
  }
  unsigned int numFields = PQnfields(pgsqlRes);
  while (pgsqlRes) {
    ExecStatusType status =  PQresultStatus(pgsqlRes);
    if (status == PGRES_TUPLES_OK) {
      PQclear(pgsqlRes);
      pgsqlRes = PQgetResult(pgsqlCon);
      continue;
    }
    if (status != PGRES_SINGLE_TUPLE) {
      printf("ERROR, queryresult failed: %s", PQerrorMessage(pgsqlCon) );
      PQclear(pgsqlRes);
      pgsqlRes = PQgetResult(pgsqlCon);      
      continue;
    }
    for(unsigned int i = 0; i < numFields; i++)   {
      char *pg_val = PQgetvalue(pgsqlRes,0,i);
      //printf("DEBUG: %d pg_val=%s\n", i, pg_val);
      if (pg_val && (0x1<<i & pageFields)) {
	while(*pg_val) {
	  //printf("%c",*pg_val);
	  rowBuf.push_back(*pg_val++);
	}
	//printf("|");
	while (isspace(rowBuf.back()))
	  rowBuf.pop_back();
	rowBuf.push_back('|');
      }
    }
    //printf("\n");
    rowBuf.push_back('\n');
    // break loop if max size reached
    if (noOfRows == maxRowsInPUBlock) {
      remainingRows = true;      
      //ywu: need to clear the pgsqlRes for the last row in a page as it's
      //already popped out
      PQclear(pgsqlRes);
      pgsqlRes = PQgetResult(pgsqlCon);
      //printf("DEBUG: noOfRows == maxRowsInPUBlock %d\n", maxRowsInPUBlock);
      break;
    }    
    // Add it to the buffer, else add it next time, get next result
    if (copyRowToBuf(bufRam,curBuf)) {
      noOfRows++;
      //printf("DEBUG: copyRowToBuf noOfRows=%d\n", noOfRows);
      rowBuf.clear();
    } else {
      remainingRows = true;
      PQclear(pgsqlRes);
      pgsqlRes = PQgetResult(pgsqlCon);
      break;
    }
    PQclear(pgsqlRes);
    pgsqlRes = PQgetResult(pgsqlCon);
  }
  bufRam[0] = 0x0;
  bufRam[1]= (noOfRows>>16 & 0xFF);    
  bufRam[2]= (noOfRows>>8 & 0xFF);  
  bufRam[3]= (noOfRows & 0xFF);
  
  //  *((unsigned int *)bufRam ) = noOfRows;
  if (!remainingRows) {
    PQclear(pgsqlRes);
  }
  return remainingRows;
}

bool SQLHost::fetchNextPageMySQL(char *bufRam) {
  if (!mysqlRes) // No result provided
    return false;
  unsigned int noOfRows = 0;
  unsigned int curBuf = 4;
  bool remainingRows = false;
  if (!rowBuf.empty()) { // Need to to first put last row into current page
    if (copyRowToBuf(bufRam,curBuf) == false) {
      return remainingRows;
    }
    noOfRows++;    
  }
  unsigned int num_fields = mysql_num_fields(mysqlRes);
  //MYSQL_FIELD *fields = mysql_fetch_fields(mysqlRes);
  /*for(i = 0; i < num_fields; i++) {
    printf("Field %u is %s\n", i, fields[i].name);
    } */ 
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(mysqlRes))) {
    unsigned long *lengths = mysql_fetch_lengths(mysqlRes);
    for(unsigned int i = 0; i < num_fields; i++)   {
      if (row[i] && (0x1<<i & pageFields)) {
	for (unsigned int j=0; j< lengths[i]; j++) {
	  //printf("%c",row[i][j]);
	  rowBuf.push_back(row[i][j]);
	}
	//printf("|");
	while (isspace(rowBuf.back()))
	  rowBuf.pop_back();
	rowBuf.push_back('|');
      }
    }
    //printf("\n");
    rowBuf.push_back('\n');
    // break loop if max size reached
    if (noOfRows == maxRowsInPUBlock) {
      remainingRows = true;      
      break;
    }    
    // Add it to the buffer
    if (copyRowToBuf(bufRam,curBuf)) {
      noOfRows++;
    } else {
      remainingRows = true;
      break;
    }
  }
  *((unsigned int *)bufRam ) = noOfRows;
  return remainingRows;
}

bool SQLHost::copyRowToBuf(char *bufRam, unsigned int& curBuf) {
  if (rowBuf.size() >= maxPUBlockSize) {
    printf("Error: Row size %ld greater than max Block Size %d. Cannot trnsfer over PCIe link. Please increase block size in the kernel.\n", rowBuf.size(), maxPUBlockSize);
    return false;
  }    

  // goes to next page
  if ((curBuf + rowBuf.size()) >= maxPUBlockSize) {    
    return false;
  }
  for (unsigned int cur_rowBuf = 0; cur_rowBuf<rowBuf.size(); cur_rowBuf++)  {
    //printf("%c",rowBuf[cur_rowBuf]);
    bufRam[curBuf++] = rowBuf[cur_rowBuf];
  }
  rowBuf.clear();
  return true;
}
