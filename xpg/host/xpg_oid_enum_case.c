/**********
Copyright 2017 Xilinx, Inc.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**********/

#include "xpg_oid_enum.h"
#include "xpgcodegen.h"

extern "C" {
#include "postgres.h"


	bool xpg_is_operator_supported_new(Oid opId)
	{
		switch (opId)
		{

			// !!, NOT tsquery
			case TSQUERY_NOT : //NOT tsquery
				elog(WARNING, "Unsupported Operator Type ID: 3682 \"tsquery_not\" (NOT tsquery ).");
				return false; 

			// @@, center of
			case CIRCLE_CENTER : //center of
				elog(WARNING, "Unsupported Operator Type ID: 1420 \"circle_center\" (center of ).");
				return false; 
			case TS_MATCH_TT : //text search match
				elog(WARNING, "Unsupported Operator Type ID: 3762 \"ts_match_tt\" (text search match ).");
				return false; 
			case TS_MATCH_TQ : //text search match
				elog(WARNING, "Unsupported Operator Type ID: 3763 \"ts_match_tq\" (text search match ).");
				return false; 
			case POLY_CENTER : //center of
				elog(WARNING, "Unsupported Operator Type ID: 971 \"poly_center\" (center of ).");
				return false; 
			case PATH_CENTER : //center of
				elog(WARNING, "Unsupported Operator Type ID: 970 \"path_center\" (center of ).");
				return false; 
			case TS_MATCH_VQ : //text search match
				elog(WARNING, "Unsupported Operator Type ID: 3636 \"ts_match_vq\" (text search match ).");
				return false; 
			case TS_MATCH_QV : //text search match
				elog(WARNING, "Unsupported Operator Type ID: 3637 \"ts_match_qv\" (text search match ).");
				return false; 
			case BOX_CENTER : //center of
				elog(WARNING, "Unsupported Operator Type ID: 513 \"box_center\" (center of ).");
				return false; 
			case LSEG_CENTER : //center of
				elog(WARNING, "Unsupported Operator Type ID: 969 \"lseg_center\" (center of ).");
				return false; 

			// ?|, vertically aligned
			case POINT_VERT : //vertically aligned
				elog(WARNING, "Unsupported Operator Type ID: 809 \"point_vert\" (vertically aligned ).");
				return false; 
			case LINE_VERTICAL : //vertical
				elog(WARNING, "Unsupported Operator Type ID: 1615 \"line_vertical\" (vertical ).");
				return false; 
			case LSEG_VERTICAL : //vertical
				elog(WARNING, "Unsupported Operator Type ID: 1529 \"lseg_vertical\" (vertical ).");
				return false; 
			case JSONB_EXISTS_ANY : //any key exists
				elog(WARNING, "Unsupported Operator Type ID: 3248 \"jsonb_exists_any\" (any key exists ).");
				return false; 

			// >=, greater than or equal
			case BITGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1789 \"bitge\" (greater than or equal ).");
				return false; 
			case MACADDR_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1225 \"macaddr_ge\" (greater than or equal ).");
				return false; 
			case MACADDR8_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3367 \"macaddr8_ge\" (greater than or equal ).");
				return false; 
			case FLOAT4GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 625 \"float4ge\" (greater than or equal ).");
				return false; 
			case TIMESTAMPTZ_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1325 \"timestamptz_ge\" (greater than or equal ).");
				return false; 
			case TEXT_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 667 \"text_ge\" (greater than or equal ).");
				return false; 
			case RELTIMEGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 571 \"reltimege\" (greater than or equal ).");
				return false; 
			case NETWORK_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1206 \"network_ge\" (greater than or equal ).");
				return false; 
			case NAMEGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 663 \"namege\" (greater than or equal ).");
				return false; 
			case FLOAT48GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1125 \"float48ge\" (greater than or equal ).");
				return false; 
			case TSVECTOR_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3631 \"tsvector_ge\" (greater than or equal ).");
				return false; 
			case INT48GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 82 \"int48ge\" (greater than or equal ).");
				return false; 
			case TSQUERY_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3678 \"tsquery_ge\" (greater than or equal ).");
				return false; 
			case INT82GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1873 \"int82ge\" (greater than or equal ).");
				return false; 
			case TIDGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2802 \"tidge\" (greater than or equal ).");
				return false; 
			case TIMESTAMPTZ_GE_TIMESTAMP : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2543 \"timestamptz_ge_timestamp\" (greater than or equal ).");
				return false; 
			case PATH_N_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 796 \"path_n_ge\" (greater than or equal ).");
				return false; 
			case INT4GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 525 \"int4ge\" (greater than or equal ).");
				return false; 
			case ARRAY_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1075 \"array_ge\" (greater than or equal ).");
				return false; 
			case BYTEAGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1960 \"byteage\" (greater than or equal ).");
				return false; 
			case DATE_GE_TIMESTAMP : //greater than or equal
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 2348 \"date_ge_timestamp\" (greater than or equal ).");
				return true; 
			case INT2GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 524 \"int2ge\" (greater than or equal ).");
				return false; 
			case INT42GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 543 \"int42ge\" (greater than or equal ).");
				return false; 
			case TIME_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1113 \"time_ge\" (greater than or equal ).");
				return false; 
			case BOOLGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1695 \"boolge\" (greater than or equal ).");
				return false; 
			case TIMESTAMPTZ_GE_DATE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2387 \"timestamptz_ge_date\" (greater than or equal ).");
				return false; 
			case CIRCLE_GE : //greater than or equal by area
				elog(WARNING, "Unsupported Operator Type ID: 1505 \"circle_ge\" (greater than or equal by area ).");
				return false; 
			case NUMERIC_GE : //greater than or equal
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 1757 \"numeric_ge\" (greater than or equal ).");
				return true; 
			case INTERVAL_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1335 \"interval_ge\" (greater than or equal ).");
				return false; 
			case JSONB_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3245 \"jsonb_ge\" (greater than or equal ).");
				return false; 
			case TINTERVALGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 816 \"tintervalge\" (greater than or equal ).");
				return false; 
			case BOX_GE : //greater than or equal by area
				elog(WARNING, "Unsupported Operator Type ID: 501 \"box_ge\" (greater than or equal by area ).");
				return false; 
			case CHARGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 634 \"charge\" (greater than or equal ).");
				return false; 
			case INT24GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 542 \"int24ge\" (greater than or equal ).");
				return false; 
			case OIDGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 612 \"oidge\" (greater than or equal ).");
				return false; 
			case FLOAT8GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 675 \"float8ge\" (greater than or equal ).");
				return false; 
			case DATE_GE : //greater than or equal
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 1098 \"date_ge\" (greater than or equal ).");
				return true; 
			case ABSTIMEGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 565 \"abstimege\" (greater than or equal ).");
				return false; 
			case FLOAT84GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1135 \"float84ge\" (greater than or equal ).");
				return false; 
			case LSEG_GE : //greater than or equal by length
				elog(WARNING, "Unsupported Operator Type ID: 1590 \"lseg_ge\" (greater than or equal by length ).");
				return false; 
			case INT28GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1867 \"int28ge\" (greater than or equal ).");
				return false; 
			case ENUM_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3521 \"enum_ge\" (greater than or equal ).");
				return false; 
			case UUID_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2977 \"uuid_ge\" (greater than or equal ).");
				return false; 
			case RANGE_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3886 \"range_ge\" (greater than or equal ).");
				return false; 
			case TIMETZ_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1555 \"timetz_ge\" (greater than or equal ).");
				return false; 
			case CASH_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 905 \"cash_ge\" (greater than or equal ).");
				return false; 
			case BPCHARGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1061 \"bpcharge\" (greater than or equal ).");
				return false; 
			case PG_LSN_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3227 \"pg_lsn_ge\" (greater than or equal ).");
				return false; 
			case RECORD_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2993 \"record_ge\" (greater than or equal ).");
				return false; 
			case VARBITGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1809 \"varbitge\" (greater than or equal ).");
				return false; 
			case TIMESTAMP_GE_TIMESTAMPTZ : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2537 \"timestamp_ge_timestamptz\" (greater than or equal ).");
				return false; 
			case INT8GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 415 \"int8ge\" (greater than or equal ).");
				return false; 
			case DATE_GE_TIMESTAMPTZ : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2361 \"date_ge_timestamptz\" (greater than or equal ).");
				return false; 
			case TIMESTAMP_GE_DATE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2374 \"timestamp_ge_date\" (greater than or equal ).");
				return false; 
			case OIDVECTORGE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 648 \"oidvectorge\" (greater than or equal ).");
				return false; 
			case TIMESTAMP_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2065 \"timestamp_ge\" (greater than or equal ).");
				return false; 
			case INT84GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 430 \"int84ge\" (greater than or equal ).");
				return false; 

			// >>, bitwise shift right
			case INT4SHR : //bitwise shift right
				elog(WARNING, "Unsupported Operator Type ID: 1885 \"int4shr\" (bitwise shift right ).");
				return false; 
			case INT8SHR : //bitwise shift right
				elog(WARNING, "Unsupported Operator Type ID: 1891 \"int8shr\" (bitwise shift right ).");
				return false; 
			case POINT_RIGHT : //is right of
				elog(WARNING, "Unsupported Operator Type ID: 508 \"point_right\" (is right of ).");
				return false; 
			case POLY_RIGHT : //is right of
				elog(WARNING, "Unsupported Operator Type ID: 488 \"poly_right\" (is right of ).");
				return false; 
			case NETWORK_SUP : //is supernet
				elog(WARNING, "Unsupported Operator Type ID: 933 \"network_sup\" (is supernet ).");
				return false; 
			case BOX_RIGHT : //is right of
				elog(WARNING, "Unsupported Operator Type ID: 496 \"box_right\" (is right of ).");
				return false; 
			case CIRCLE_RIGHT : //is right of
				elog(WARNING, "Unsupported Operator Type ID: 1509 \"circle_right\" (is right of ).");
				return false; 
			case BITSHIFTRIGHT : //bitwise shift right
				elog(WARNING, "Unsupported Operator Type ID: 1796 \"bitshiftright\" (bitwise shift right ).");
				return false; 
			case RANGE_AFTER : //is right of
				elog(WARNING, "Unsupported Operator Type ID: 3894 \"range_after\" (is right of ).");
				return false; 
			case INT2SHR : //bitwise shift right
				elog(WARNING, "Unsupported Operator Type ID: 1879 \"int2shr\" (bitwise shift right ).");
				return false; 

			// #<=, less than or equal by length
			case TINTERVALLENLE : //less than or equal by length
				elog(WARNING, "Unsupported Operator Type ID: 579 \"tintervallenle\" (less than or equal by length ).");
				return false; 

			// <>, not equal
			case DATE_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1094 \"date_ne\" (not equal ).");
				return false; 
			case FLOAT4NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 621 \"float4ne\" (not equal ).");
				return false; 
			case TIMESTAMPTZ_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1321 \"timestamptz_ne\" (not equal ).");
				return false; 
			case MACADDR_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1221 \"macaddr_ne\" (not equal ).");
				return false; 
			case FLOAT48NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1121 \"float48ne\" (not equal ).");
				return false; 
			case OIDNE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 608 \"oidne\" (not equal ).");
				return false; 
			case TSVECTOR_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 3630 \"tsvector_ne\" (not equal ).");
				return false; 
			case NETWORK_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1202 \"network_ne\" (not equal ).");
				return false; 
			case PG_LSN_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 3223 \"pg_lsn_ne\" (not equal ).");
				return false; 
			case ARRAY_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1071 \"array_ne\" (not equal ).");
				return false; 
			case TSQUERY_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 3677 \"tsquery_ne\" (not equal ).");
				return false; 
			case BOOLNE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 85 \"boolne\" (not equal ).");
				return false; 
			case RECORD_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 2989 \"record_ne\" (not equal ).");
				return false; 
			case TIMESTAMPTZ_NE_TIMESTAMP : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 2545 \"timestamptz_ne_timestamp\" (not equal ).");
				return false; 
			case BPCHARNE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1057 \"bpcharne\" (not equal ).");
				return false; 
			case LSEG_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1586 \"lseg_ne\" (not equal ).");
				return false; 
			case MACADDR8_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 3363 \"macaddr8_ne\" (not equal ).");
				return false; 
			case CIRCLE_NE : //not equal by area
				elog(WARNING, "Unsupported Operator Type ID: 1501 \"circle_ne\" (not equal by area ).");
				return false; 
			case INTERVAL_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1331 \"interval_ne\" (not equal ).");
				return false; 
			case TIDNE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 402 \"tidne\" (not equal ).");
				return false; 
			case DATE_NE_TIMESTAMPTZ : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 2363 \"date_ne_timestamptz\" (not equal ).");
				return false; 
			case TIMESTAMPTZ_NE_DATE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 2389 \"timestamptz_ne_date\" (not equal ).");
				return false; 
			case TINTERVALNE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 812 \"tintervalne\" (not equal ).");
				return false; 
			case BITNE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1785 \"bitne\" (not equal ).");
				return false; 
			case OIDVECTORNE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 644 \"oidvectorne\" (not equal ).");
				return false; 
			case CHARNE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 630 \"charne\" (not equal ).");
				return false; 
			case ABSTIMENE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 561 \"abstimene\" (not equal ).");
				return false; 
			case FLOAT84NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1131 \"float84ne\" (not equal ).");
				return false; 
			case POINT_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 713 \"point_ne\" (not equal ).");
				return false; 
			case FLOAT8NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 671 \"float8ne\" (not equal ).");
				return false; 
			case RELTIMENE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 567 \"reltimene\" (not equal ).");
				return false; 
			case INT28NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1863 \"int28ne\" (not equal ).");
				return false; 
			case BYTEANE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1956 \"byteane\" (not equal ).");
				return false; 
			case ENUM_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 3517 \"enum_ne\" (not equal ).");
				return false; 
			case INT82NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1869 \"int82ne\" (not equal ).");
				return false; 
			case UUID_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 2973 \"uuid_ne\" (not equal ).");
				return false; 
			case XIDNEQ : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 3315 \"xidneq\" (not equal ).");
				return false; 
			case TIMESTAMP_NE_DATE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 2376 \"timestamp_ne_date\" (not equal ).");
				return false; 
			case XIDNEQINT4 : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 3316 \"xidneqint4\" (not equal ).");
				return false; 
			case TIMETZ_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1551 \"timetz_ne\" (not equal ).");
				return false; 
			case VARBITNE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1805 \"varbitne\" (not equal ).");
				return false; 
			case CASH_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 901 \"cash_ne\" (not equal ).");
				return false; 
			case RANGE_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 3883 \"range_ne\" (not equal ).");
				return false; 
			case NUMERIC_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1753 \"numeric_ne\" (not equal ).");
				return false; 
			case JSONB_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 3241 \"jsonb_ne\" (not equal ).");
				return false; 
			case INT48NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 36 \"int48ne\" (not equal ).");
				return false; 
			case TIMESTAMP_NE_TIMESTAMPTZ : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 2539 \"timestamp_ne_timestamptz\" (not equal ).");
				return false; 
			case INT2NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 519 \"int2ne\" (not equal ).");
				return false; 
			case INT4NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 518 \"int4ne\" (not equal ).");
				return false; 
			case NAMENE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 643 \"namene\" (not equal ).");
				return false; 
			case INT84NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 417 \"int84ne\" (not equal ).");
				return false; 
			case INT8NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 411 \"int8ne\" (not equal ).");
				return false; 
			case TEXTNE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 531 \"textne\" (not equal ).");
				return false; 
			case TIMESTAMP_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 2061 \"timestamp_ne\" (not equal ).");
				return false; 
			case DATE_NE_TIMESTAMP : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 2350 \"date_ne_timestamp\" (not equal ).");
				return false; 
			case INT24NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 538 \"int24ne\" (not equal ).");
				return false; 
			case INT42NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 539 \"int42ne\" (not equal ).");
				return false; 
			case TIME_NE : //not equal
				elog(WARNING, "Unsupported Operator Type ID: 1109 \"time_ne\" (not equal ).");
				return false; 

			// ||, concatenate
			case TEXTANYCAT : //concatenate
				elog(WARNING, "Unsupported Operator Type ID: 2779 \"textanycat\" (concatenate ).");
				return false; 
			case ANYTEXTCAT : //concatenate
				elog(WARNING, "Unsupported Operator Type ID: 2780 \"anytextcat\" (concatenate ).");
				return false; 
			case TSQUERY_OR : //OR-concatenate
				elog(WARNING, "Unsupported Operator Type ID: 3681 \"tsquery_or\" (OR-concatenate ).");
				return false; 
			case BITCAT : //concatenate
				elog(WARNING, "Unsupported Operator Type ID: 1797 \"bitcat\" (concatenate ).");
				return false; 
			case TSVECTOR_CONCAT : //concatenate
				elog(WARNING, "Unsupported Operator Type ID: 3633 \"tsvector_concat\" (concatenate ).");
				return false; 
			case ARRAY_PREPEND : //prepend element onto front of array
				elog(WARNING, "Unsupported Operator Type ID: 374 \"array_prepend\" (prepend element onto front of array ).");
				return false; 
			case TEXTCAT : //concatenate
				elog(WARNING, "Unsupported Operator Type ID: 654 \"textcat\" (concatenate ).");
				return false; 
			case BYTEACAT : //concatenate
				elog(WARNING, "Unsupported Operator Type ID: 2018 \"byteacat\" (concatenate ).");
				return false; 
			case JSONB_CONCAT : //concatenate
				elog(WARNING, "Unsupported Operator Type ID: 3284 \"jsonb_concat\" (concatenate ).");
				return false; 
			case ARRAY_CAT : //concatenate
				elog(WARNING, "Unsupported Operator Type ID: 375 \"array_cat\" (concatenate ).");
				return false; 
			case ARRAY_APPEND : //append element onto end of array
				elog(WARNING, "Unsupported Operator Type ID: 349 \"array_append\" (append element onto end of array ).");
				return false; 

			// <=, less than or equal
			case BITLE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1788 \"bitle\" (less than or equal ).");
				return false; 
			case TIMESTAMPTZ_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1323 \"timestamptz_le\" (less than or equal ).");
				return false; 
			case MACADDR_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1223 \"macaddr_le\" (less than or equal ).");
				return false; 
			case FLOAT4LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 624 \"float4le\" (less than or equal ).");
				return false; 
			case NETWORK_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1204 \"network_le\" (less than or equal ).");
				return false; 
			case TEXT_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 665 \"text_le\" (less than or equal ).");
				return false; 
			case RELTIMELE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 570 \"reltimele\" (less than or equal ).");
				return false; 
			case FLOAT48LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1124 \"float48le\" (less than or equal ).");
				return false; 
			case DATE_LE : //less than or equal
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 1096 \"date_le\" (less than or equal ).");
				return true; 
			case NAMELE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 661 \"namele\" (less than or equal ).");
				return false; 
			case INT82LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1872 \"int82le\" (less than or equal ).");
				return false; 
			case INT48LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 80 \"int48le\" (less than or equal ).");
				return false; 
			case TIDLE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2801 \"tidle\" (less than or equal ).");
				return false; 
			case TSQUERY_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3675 \"tsquery_le\" (less than or equal ).");
				return false; 
			case PATH_N_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 795 \"path_n_le\" (less than or equal ).");
				return false; 
			case DATE_LE_TIMESTAMP : //less than or equal
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 2346 \"date_le_timestamp\" (less than or equal ).");
				return true; 
			case BPCHARLE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1059 \"bpcharle\" (less than or equal ).");
				return false; 
			case ARRAY_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1074 \"array_le\" (less than or equal ).");
				return false; 
			case INT24LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 540 \"int24le\" (less than or equal ).");
				return false; 
			case TIME_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1111 \"time_le\" (less than or equal ).");
				return false; 
			case BOOLLE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1694 \"boolle\" (less than or equal ).");
				return false; 
			case INT42LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 541 \"int42le\" (less than or equal ).");
				return false; 
			case TIMESTAMPTZ_LE_DATE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2385 \"timestamptz_le_date\" (less than or equal ).");
				return false; 
			case INT2LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 522 \"int2le\" (less than or equal ).");
				return false; 
			case INT4LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 523 \"int4le\" (less than or equal ).");
				return false; 
			case INT84LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 420 \"int84le\" (less than or equal ).");
				return false; 
			case NUMERIC_LE : //less than or equal
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 1755 \"numeric_le\" (less than or equal ).");
				return true; 
			case BOX_LE : //less than or equal by area
				elog(WARNING, "Unsupported Operator Type ID: 505 \"box_le\" (less than or equal by area ).");
				return false; 
			case TINTERVALLE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 815 \"tintervalle\" (less than or equal ).");
				return false; 
			case TSVECTOR_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3628 \"tsvector_le\" (less than or equal ).");
				return false; 
			case CHARLE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 632 \"charle\" (less than or equal ).");
				return false; 
			case CIRCLE_LE : //less than or equal by area
				elog(WARNING, "Unsupported Operator Type ID: 1504 \"circle_le\" (less than or equal by area ).");
				return false; 
			case OIDLE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 611 \"oidle\" (less than or equal ).");
				return false; 
			case ABSTIMELE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 564 \"abstimele\" (less than or equal ).");
				return false; 
			case LSEG_LE : //less than or equal by length
				elog(WARNING, "Unsupported Operator Type ID: 1588 \"lseg_le\" (less than or equal by length ).");
				return false; 
			case INTERVAL_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1333 \"interval_le\" (less than or equal ).");
				return false; 
			case FLOAT8LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 673 \"float8le\" (less than or equal ).");
				return false; 
			case JSONB_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3244 \"jsonb_le\" (less than or equal ).");
				return false; 
			case ENUM_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3520 \"enum_le\" (less than or equal ).");
				return false; 
			case INT28LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1866 \"int28le\" (less than or equal ).");
				return false; 
			case FLOAT84LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1134 \"float84le\" (less than or equal ).");
				return false; 
			case UUID_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2976 \"uuid_le\" (less than or equal ).");
				return false; 
			case BYTEALE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1958 \"byteale\" (less than or equal ).");
				return false; 
			case RANGE_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3885 \"range_le\" (less than or equal ).");
				return false; 
			case TIMETZ_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1553 \"timetz_le\" (less than or equal ).");
				return false; 
			case CASH_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 904 \"cash_le\" (less than or equal ).");
				return false; 
			case RECORD_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2992 \"record_le\" (less than or equal ).");
				return false; 
			case PG_LSN_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3226 \"pg_lsn_le\" (less than or equal ).");
				return false; 
			case VARBITLE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 1808 \"varbitle\" (less than or equal ).");
				return false; 
			case INT8LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 414 \"int8le\" (less than or equal ).");
				return false; 
			case DATE_LE_TIMESTAMPTZ : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2359 \"date_le_timestamptz\" (less than or equal ).");
				return false; 
			case OIDVECTORLE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 647 \"oidvectorle\" (less than or equal ).");
				return false; 
			case MACADDR8_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3365 \"macaddr8_le\" (less than or equal ).");
				return false; 
			case TIMESTAMPTZ_LE_TIMESTAMP : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2541 \"timestamptz_le_timestamp\" (less than or equal ).");
				return false; 
			case TIMESTAMP_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2063 \"timestamp_le\" (less than or equal ).");
				return false; 
			case TIMESTAMP_LE_DATE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2372 \"timestamp_le_date\" (less than or equal ).");
				return false; 
			case TIMESTAMP_LE_TIMESTAMPTZ : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2535 \"timestamp_le_timestamptz\" (less than or equal ).");
				return false; 

			// *>, greater than
			case RECORD_IMAGE_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 3191 \"record_image_gt\" (greater than ).");
				return false; 

			// ?-|, perpendicular
			case LSEG_PERP : //perpendicular
				elog(WARNING, "Unsupported Operator Type ID: 1527 \"lseg_perp\" (perpendicular ).");
				return false; 
			case LINE_PERP : //perpendicular
				elog(WARNING, "Unsupported Operator Type ID: 1613 \"line_perp\" (perpendicular ).");
				return false; 

			// -|-, is adjacent to
			case RANGE_ADJACENT : //is adjacent to
				elog(WARNING, "Unsupported Operator Type ID: 3897 \"range_adjacent\" (is adjacent to ).");
				return false; 

			// ^, exponentiation
			case DPOW : //exponentiation
				elog(WARNING, "Unsupported Operator Type ID: 965 \"dpow\" (exponentiation ).");
				return false; 
			case NUMERIC_POWER : //exponentiation
				elog(WARNING, "Unsupported Operator Type ID: 1038 \"numeric_power\" (exponentiation ).");
				return false; 

			// #>, get value from json with path elements
			case JSON_EXTRACT_PATH : //get value from json with path elements
				elog(WARNING, "Unsupported Operator Type ID: 3966 \"json_extract_path\" (get value from json with path elements ).");
				return false; 
			case TINTERVALLENGT : //greater than by length
				elog(WARNING, "Unsupported Operator Type ID: 578 \"tintervallengt\" (greater than by length ).");
				return false; 
			case JSONB_EXTRACT_PATH : //get value from jsonb with path elements
				elog(WARNING, "Unsupported Operator Type ID: 3213 \"jsonb_extract_path\" (get value from jsonb with path elements ).");
				return false; 

			// #=, equal by length
			case TINTERVALLENEQ : //equal by length
				elog(WARNING, "Unsupported Operator Type ID: 575 \"tintervalleneq\" (equal by length ).");
				return false; 

			// #<, less than by length
			case TINTERVALLENLT : //less than by length
				elog(WARNING, "Unsupported Operator Type ID: 577 \"tintervallenlt\" (less than by length ).");
				return false; 

			// #-, delete path
			case JSONB_DELETE_PATH : //delete path
				elog(WARNING, "Unsupported Operator Type ID: 3287 \"jsonb_delete_path\" (delete path ).");
				return false; 

			// !, factorial
			case NUMERIC_FAC : //factorial
				elog(WARNING, "Unsupported Operator Type ID: 388 \"numeric_fac\" (factorial ).");
				return false; 

			// #, number of points
			case PATH_NPOINTS : //number of points
				elog(WARNING, "Unsupported Operator Type ID: 797 \"path_npoints\" (number of points ).");
				return false; 
			case LSEG_INTERPT : //intersection point
				elog(WARNING, "Unsupported Operator Type ID: 1536 \"lseg_interpt\" (intersection point ).");
				return false; 
			case POLY_NPOINTS : //number of points
				elog(WARNING, "Unsupported Operator Type ID: 1521 \"poly_npoints\" (number of points ).");
				return false; 
			case BITXOR : //bitwise exclusive or
				elog(WARNING, "Unsupported Operator Type ID: 1793 \"bitxor\" (bitwise exclusive or ).");
				return false; 
			case BOX_INTERSECT : //box intersection
				elog(WARNING, "Unsupported Operator Type ID: 803 \"box_intersect\" (box intersection ).");
				return false; 
			case LINE_INTERPT : //intersection point
				elog(WARNING, "Unsupported Operator Type ID: 1617 \"line_interpt\" (intersection point ).");
				return false; 
			case INT8XOR : //bitwise exclusive or
				elog(WARNING, "Unsupported Operator Type ID: 1888 \"int8xor\" (bitwise exclusive or ).");
				return false; 
			case INT4XOR : //bitwise exclusive or
				elog(WARNING, "Unsupported Operator Type ID: 1882 \"int4xor\" (bitwise exclusive or ).");
				return false; 
			case INT2XOR : //bitwise exclusive or
				elog(WARNING, "Unsupported Operator Type ID: 1876 \"int2xor\" (bitwise exclusive or ).");
				return false; 

			// %, modulus
			case NUMERIC_MOD : //modulus
				elog(WARNING, "Unsupported Operator Type ID: 1762 \"numeric_mod\" (modulus ).");
				return false; 
			case INT4MOD : //modulus
				elog(WARNING, "Unsupported Operator Type ID: 530 \"int4mod\" (modulus ).");
				return false; 
			case INT2MOD : //modulus
				elog(WARNING, "Unsupported Operator Type ID: 529 \"int2mod\" (modulus ).");
				return false; 
			case INT8MOD : //modulus
				elog(WARNING, "Unsupported Operator Type ID: 439 \"int8mod\" (modulus ).");
				return false; 

			// ~~, matches LIKE expression
			case BYTEALIKE : //matches LIKE expression
				elog(WARNING, "Unsupported Operator Type ID: 2016 \"bytealike\" (matches LIKE expression ).");
				return false; 
			case NAMELIKE : //matches LIKE expression
				elog(WARNING, "Unsupported Operator Type ID: 1207 \"namelike\" (matches LIKE expression ).");
				return false; 
			case BPCHARLIKE : //matches LIKE expression
				elog(WARNING, "Unsupported Operator Type ID: 1211 \"bpcharlike\" (matches LIKE expression ).");
				return false; 
			case TEXTLIKE : //matches LIKE expression
				elog(WARNING, "Unsupported Operator Type ID: 1209 \"textlike\" (matches LIKE expression ).");
				return false; 

			// <<=, is subnet or equal
			case NETWORK_SUBEQ : //is subnet or equal
				elog(WARNING, "Unsupported Operator Type ID: 932 \"network_subeq\" (is subnet or equal ).");
				return false; 

			// &, bitwise and
			case INETAND : //bitwise and
				elog(WARNING, "Unsupported Operator Type ID: 2635 \"inetand\" (bitwise and ).");
				return false; 
			case INT2AND : //bitwise and
				elog(WARNING, "Unsupported Operator Type ID: 1874 \"int2and\" (bitwise and ).");
				return false; 
			case MACADDR8_AND : //bitwise and
				elog(WARNING, "Unsupported Operator Type ID: 3369 \"macaddr8_and\" (bitwise and ).");
				return false; 
			case MACADDR_AND : //bitwise and
				elog(WARNING, "Unsupported Operator Type ID: 3148 \"macaddr_and\" (bitwise and ).");
				return false; 
			case INT8AND : //bitwise and
				elog(WARNING, "Unsupported Operator Type ID: 1886 \"int8and\" (bitwise and ).");
				return false; 
			case BITAND : //bitwise and
				elog(WARNING, "Unsupported Operator Type ID: 1791 \"bitand\" (bitwise and ).");
				return false; 
			case INT4AND : //bitwise and
				elog(WARNING, "Unsupported Operator Type ID: 1880 \"int4and\" (bitwise and ).");
				return false; 

			// ?#, intersect
			case INTER_SL : //intersect
				elog(WARNING, "Unsupported Operator Type ID: 1537 \"inter_sl\" (intersect ).");
				return false; 
			case LSEG_INTERSECT : //intersect
				elog(WARNING, "Unsupported Operator Type ID: 1525 \"lseg_intersect\" (intersect ).");
				return false; 
			case LINE_INTERSECT : //intersect
				elog(WARNING, "Unsupported Operator Type ID: 1611 \"line_intersect\" (intersect ).");
				return false; 
			case PATH_INTER : //intersect
				elog(WARNING, "Unsupported Operator Type ID: 798 \"path_inter\" (intersect ).");
				return false; 
			case INTER_LB : //intersect
				elog(WARNING, "Unsupported Operator Type ID: 1539 \"inter_lb\" (intersect ).");
				return false; 
			case INTER_SB : //intersect
				elog(WARNING, "Unsupported Operator Type ID: 1538 \"inter_sb\" (intersect ).");
				return false; 

			// +, add
			case FLOAT8PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 591 \"float8pl\" (add ).");
				return false; 
			case TIMESTAMPTZ_PL_INTERVAL : //add
				elog(WARNING, "Unsupported Operator Type ID: 1327 \"timestamptz_pl_interval\" (add ).");
				return false; 
			case INETPL : //add
				elog(WARNING, "Unsupported Operator Type ID: 2637 \"inetpl\" (add ).");
				return false; 
			case INT4PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 551 \"int4pl\" (add ).");
				return false; 
			case INT2PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 550 \"int2pl\" (add ).");
				return false; 
			case INT42PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 553 \"int42pl\" (add ).");
				return false; 
			case INT24PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 552 \"int24pl\" (add ).");
				return false; 
			case FLOAT8UP : //unary plus
				elog(WARNING, "Unsupported Operator Type ID: 1920 \"float8up\" (unary plus ).");
				return false; 
			case NUMERIC_UPLUS : //unary plus
				elog(WARNING, "Unsupported Operator Type ID: 1921 \"numeric_uplus\" (unary plus ).");
				return false; 
			case INTERVAL_PL_TIMETZ : //add
				elog(WARNING, "Unsupported Operator Type ID: 2552 \"interval_pl_timetz\" (add ).");
				return false; 
			case BOX_ADD : //add point to box (translate)
				elog(WARNING, "Unsupported Operator Type ID: 804 \"box_add\" (add point to box (translate) ).");
				return false; 
			case INT8PL_INET : //add
				elog(WARNING, "Unsupported Operator Type ID: 2638 \"int8pl_inet\" (add ).");
				return false; 
			case INT48PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 692 \"int48pl\" (add ).");
				return false; 
			case POINT_ADD : //add points (translate)
				elog(WARNING, "Unsupported Operator Type ID: 731 \"point_add\" (add points (translate) ).");
				return false; 
			case DATE_PL_INTERVAL : //add
				elog(WARNING, "Unsupported Operator Type ID: 1076 \"date_pl_interval\" (add ).");
				return false; 
			case FLOAT48PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 1116 \"float48pl\" (add ).");
				return false; 
			case INT82PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 818 \"int82pl\" (add ).");
				return false; 
			case RANGE_UNION : //range union
				elog(WARNING, "Unsupported Operator Type ID: 3898 \"range_union\" (range union ).");
				return false; 
			case PATH_ADD : //concatenate
				elog(WARNING, "Unsupported Operator Type ID: 735 \"path_add\" (concatenate ).");
				return false; 
			case PATH_ADD_PT : //add (translate path)
				elog(WARNING, "Unsupported Operator Type ID: 736 \"path_add_pt\" (add (translate path) ).");
				return false; 
			case INT84PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 688 \"int84pl\" (add ).");
				return false; 
			case FLOAT4PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 586 \"float4pl\" (add ).");
				return false; 
			case DATE_PLI : //add
				elog(WARNING, "Unsupported Operator Type ID: 1100 \"date_pli\" (add ).");
				return false; 
			case TIMEPL : //add
				elog(WARNING, "Unsupported Operator Type ID: 581 \"timepl\" (add ).");
				return false; 
			case INTERVAL_PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 1337 \"interval_pl\" (add ).");
				return false; 
			case NUMERIC_ADD : //add
				elog(WARNING, "Unsupported Operator Type ID: 1758 \"numeric_add\" (add ).");
				return false; 
			case ACLINSERT : //add/update ACL item
				elog(WARNING, "Unsupported Operator Type ID: 966 \"aclinsert\" (add/update ACL item ).");
				return false; 
			case INTEGER_PL_DATE : //add
				elog(WARNING, "Unsupported Operator Type ID: 2555 \"integer_pl_date\" (add ).");
				return false; 
			case FLOAT4UP : //unary plus
				elog(WARNING, "Unsupported Operator Type ID: 1919 \"float4up\" (unary plus ).");
				return false; 
			case INT4UP : //unary plus
				elog(WARNING, "Unsupported Operator Type ID: 1918 \"int4up\" (unary plus ).");
				return false; 
			case FLOAT84PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 1126 \"float84pl\" (add ).");
				return false; 
			case INTERVAL_PL_TIMESTAMPTZ : //add
				elog(WARNING, "Unsupported Operator Type ID: 2554 \"interval_pl_timestamptz\" (add ).");
				return false; 
			case INT2UP : //unary plus
				elog(WARNING, "Unsupported Operator Type ID: 1917 \"int2up\" (unary plus ).");
				return false; 
			case INT8UP : //unary plus
				elog(WARNING, "Unsupported Operator Type ID: 1916 \"int8up\" (unary plus ).");
				return false; 
			case INTERVAL_PL_DATE : //add
				elog(WARNING, "Unsupported Operator Type ID: 2551 \"interval_pl_date\" (add ).");
				return false; 
			case INTERVAL_PL_TIME : //add
				elog(WARNING, "Unsupported Operator Type ID: 1849 \"interval_pl_time\" (add ).");
				return false; 
			case INTERVAL_PL_TIMESTAMP : //add
				elog(WARNING, "Unsupported Operator Type ID: 2553 \"interval_pl_timestamp\" (add ).");
				return false; 
			case CASH_PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 906 \"cash_pl\" (add ).");
				return false; 
			case TIMETZ_PL_INTERVAL : //add
				elog(WARNING, "Unsupported Operator Type ID: 1802 \"timetz_pl_interval\" (add ).");
				return false; 
			case TIME_PL_INTERVAL : //add
				elog(WARNING, "Unsupported Operator Type ID: 1800 \"time_pl_interval\" (add ).");
				return false; 
			case TIMETZDATE_PL : //convert time with time zone and date to timestamp with time zone
				elog(WARNING, "Unsupported Operator Type ID: 1366 \"timetzdate_pl\" (convert time with time zone and date to timestamp with time zone ).");
				return false; 
			case TIMEDATE_PL : //convert time and date to timestamp
				elog(WARNING, "Unsupported Operator Type ID: 1363 \"timedate_pl\" (convert time and date to timestamp ).");
				return false; 
			case DATETIME_PL : //convert date and time to timestamp
				elog(WARNING, "Unsupported Operator Type ID: 1360 \"datetime_pl\" (convert date and time to timestamp ).");
				return false; 
			case DATETIMETZ_PL : //convert date and time with time zone to timestamp with time zone
				elog(WARNING, "Unsupported Operator Type ID: 1361 \"datetimetz_pl\" (convert date and time with time zone to timestamp with time zone ).");
				return false; 
			case INT8PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 684 \"int8pl\" (add ).");
				return false; 
			case CIRCLE_ADD_PT : //add
				elog(WARNING, "Unsupported Operator Type ID: 1516 \"circle_add_pt\" (add ).");
				return false; 
			case TIMESTAMP_PL_INTERVAL : //add
				elog(WARNING, "Unsupported Operator Type ID: 2066 \"timestamp_pl_interval\" (add ).");
				return false; 
			case INT28PL : //add
				elog(WARNING, "Unsupported Operator Type ID: 822 \"int28pl\" (add ).");
				return false; 

			// *, multiply
			case FLOAT84MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 1129 \"float84mul\" (multiply ).");
				return false; 
			case INT4MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 514 \"int4mul\" (multiply ).");
				return false; 
			case RANGE_INTERSECT : //range intersection
				elog(WARNING, "Unsupported Operator Type ID: 3900 \"range_intersect\" (range intersection ).");
				return false; 
			case FLOAT8MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 594 \"float8mul\" (multiply ).");
				return false; 
			case INT28MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 824 \"int28mul\" (multiply ).");
				return false; 
			case FLT4_MUL_CASH : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 845 \"flt4_mul_cash\" (multiply ).");
				return false; 
			case FLOAT4MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 589 \"float4mul\" (multiply ).");
				return false; 
			case INT8_MUL_CASH : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 3349 \"int8_mul_cash\" (multiply ).");
				return false; 
			case PATH_MUL_PT : //multiply (rotate/scale path)
				elog(WARNING, "Unsupported Operator Type ID: 738 \"path_mul_pt\" (multiply (rotate/scale path) ).");
				return false; 
			case NUMERIC_MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 1760 \"numeric_mul\" (multiply ).");
				return false; 
			case INT84MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 690 \"int84mul\" (multiply ).");
				return false; 
			case INT8MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 686 \"int8mul\" (multiply ).");
				return false; 
			case CASH_MUL_FLT4 : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 843 \"cash_mul_flt4\" (multiply ).");
				return false; 
			case INT48MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 694 \"int48mul\" (multiply ).");
				return false; 
			case CIRCLE_MUL_PT : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 1518 \"circle_mul_pt\" (multiply ).");
				return false; 
			case INTERVAL_MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 1583 \"interval_mul\" (multiply ).");
				return false; 
			case CASH_MUL_FLT8 : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 908 \"cash_mul_flt8\" (multiply ).");
				return false; 
			case CASH_MUL_INT8 : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 3346 \"cash_mul_int8\" (multiply ).");
				return false; 
			case INT2MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 526 \"int2mul\" (multiply ).");
				return false; 
			case MUL_D_INTERVAL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 1584 \"mul_d_interval\" (multiply ).");
				return false; 
			case INT82MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 820 \"int82mul\" (multiply ).");
				return false; 
			case POINT_MUL : //multiply points (scale/rotate)
				elog(WARNING, "Unsupported Operator Type ID: 733 \"point_mul\" (multiply points (scale/rotate) ).");
				return false; 
			case INT2_MUL_CASH : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 918 \"int2_mul_cash\" (multiply ).");
				return false; 
			case INT24MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 544 \"int24mul\" (multiply ).");
				return false; 
			case INT42MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 545 \"int42mul\" (multiply ).");
				return false; 
			case FLOAT48MUL : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 1119 \"float48mul\" (multiply ).");
				return false; 
			case CASH_MUL_INT2 : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 914 \"cash_mul_int2\" (multiply ).");
				return false; 
			case INT4_MUL_CASH : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 917 \"int4_mul_cash\" (multiply ).");
				return false; 
			case FLT8_MUL_CASH : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 916 \"flt8_mul_cash\" (multiply ).");
				return false; 
			case BOX_MUL : //multiply box by point (scale)
				elog(WARNING, "Unsupported Operator Type ID: 806 \"box_mul\" (multiply box by point (scale) ).");
				return false; 
			case CASH_MUL_INT4 : //multiply
				elog(WARNING, "Unsupported Operator Type ID: 912 \"cash_mul_int4\" (multiply ).");
				return false; 

			// -, subtract
			case FLOAT8MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 592 \"float8mi\" (subtract ).");
				return false; 
			case INT2UM : //negate
				elog(WARNING, "Unsupported Operator Type ID: 559 \"int2um\" (negate ).");
				return false; 
			case INT4UM : //negate
				elog(WARNING, "Unsupported Operator Type ID: 558 \"int4um\" (negate ).");
				return false; 
			case INT4MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 555 \"int4mi\" (subtract ).");
				return false; 
			case INT2MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 554 \"int2mi\" (subtract ).");
				return false; 
			case INT42MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 557 \"int42mi\" (subtract ).");
				return false; 
			case INT24MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 556 \"int24mi\" (subtract ).");
				return false; 
			case FLOAT84MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1127 \"float84mi\" (subtract ).");
				return false; 
			case INETMI_INT8 : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 2639 \"inetmi_int8\" (subtract ).");
				return false; 
			case INTERVAL_MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1338 \"interval_mi\" (subtract ).");
				return false; 
			case INT48MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 693 \"int48mi\" (subtract ).");
				return false; 
			case DATE_MI_INTERVAL : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1077 \"date_mi_interval\" (subtract ).");
				return false; 
			case INT82MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 819 \"int82mi\" (subtract ).");
				return false; 
			case POINT_SUB : //subtract points (translate)
				elog(WARNING, "Unsupported Operator Type ID: 732 \"point_sub\" (subtract points (translate) ).");
				return false; 
			case TIME_MI_TIME : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1399 \"time_mi_time\" (subtract ).");
				return false; 
			case TIMESTAMPTZ_MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1328 \"timestamptz_mi\" (subtract ).");
				return false; 
			case NUMERIC_UMINUS : //negate
				elog(WARNING, "Unsupported Operator Type ID: 1751 \"numeric_uminus\" (negate ).");
				return false; 
			case FLOAT4MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 587 \"float4mi\" (subtract ).");
				return false; 
			case FLOAT4UM : //negate
				elog(WARNING, "Unsupported Operator Type ID: 584 \"float4um\" (negate ).");
				return false; 
			case FLOAT8UM : //negate
				elog(WARNING, "Unsupported Operator Type ID: 585 \"float8um\" (negate ).");
				return false; 
			case TIMEMI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 582 \"timemi\" (subtract ).");
				return false; 
			case FLOAT48MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1117 \"float48mi\" (subtract ).");
				return false; 
			case INETMI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 2640 \"inetmi\" (subtract ).");
				return false; 
			case RANGE_MINUS : //range difference
				elog(WARNING, "Unsupported Operator Type ID: 3899 \"range_minus\" (range difference ).");
				return false; 
			case INTERVAL_UM : //negate
				elog(WARNING, "Unsupported Operator Type ID: 1336 \"interval_um\" (negate ).");
				return false; 
			case DATE_MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1099 \"date_mi\" (subtract ).");
				return false; 
			case INT8UM : //negate
				elog(WARNING, "Unsupported Operator Type ID: 484 \"int8um\" (negate ).");
				return false; 
			case CASH_MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 907 \"cash_mi\" (subtract ).");
				return false; 
			case ACLREMOVE : //remove ACL item
				elog(WARNING, "Unsupported Operator Type ID: 967 \"aclremove\" (remove ACL item ).");
				return false; 
			case TIMETZ_MI_INTERVAL : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1803 \"timetz_mi_interval\" (subtract ).");
				return false; 
			case PG_LSN_MI : //minus
				elog(WARNING, "Unsupported Operator Type ID: 3228 \"pg_lsn_mi\" (minus ).");
				return false; 
			case TIME_MI_INTERVAL : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1801 \"time_mi_interval\" (subtract ).");
				return false; 
			case NUMERIC_SUB : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1759 \"numeric_sub\" (subtract ).");
				return false; 
			case TIMESTAMPTZ_MI_INTERVAL : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1329 \"timestamptz_mi_interval\" (subtract ).");
				return false; 
			case PATH_SUB_PT : //subtract (translate path)
				elog(WARNING, "Unsupported Operator Type ID: 737 \"path_sub_pt\" (subtract (translate path) ).");
				return false; 
			case TIMESTAMP_MI_INTERVAL : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 2068 \"timestamp_mi_interval\" (subtract ).");
				return false; 
			case INT84MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 689 \"int84mi\" (subtract ).");
				return false; 
			case BOX_SUB : //subtract point from box (translate)
				elog(WARNING, "Unsupported Operator Type ID: 805 \"box_sub\" (subtract point from box (translate) ).");
				return false; 
			case DATE_MII : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1101 \"date_mii\" (subtract ).");
				return false; 
			case INT8MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 685 \"int8mi\" (subtract ).");
				return false; 
			case CIRCLE_SUB_PT : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 1517 \"circle_sub_pt\" (subtract ).");
				return false; 
			case INT28MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 823 \"int28mi\" (subtract ).");
				return false; 
			case TIMESTAMP_MI : //subtract
				elog(WARNING, "Unsupported Operator Type ID: 2067 \"timestamp_mi\" (subtract ).");
				return false; 

			// /, divide
			case FLOAT84DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 1128 \"float84div\" (divide ).");
				return false; 
			case FLOAT8DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 593 \"float8div\" (divide ).");
				return false; 
			case CIRCLE_DIV_PT : //divide
				elog(WARNING, "Unsupported Operator Type ID: 1519 \"circle_div_pt\" (divide ).");
				return false; 
			case CASH_DIV_INT2 : //divide
				elog(WARNING, "Unsupported Operator Type ID: 915 \"cash_div_int2\" (divide ).");
				return false; 
			case CASH_DIV_CASH : //divide
				elog(WARNING, "Unsupported Operator Type ID: 3825 \"cash_div_cash\" (divide ).");
				return false; 
			case FLOAT4DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 588 \"float4div\" (divide ).");
				return false; 
			case INT28DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 825 \"int28div\" (divide ).");
				return false; 
			case CASH_DIV_FLT4 : //divide
				elog(WARNING, "Unsupported Operator Type ID: 844 \"cash_div_flt4\" (divide ).");
				return false; 
			case INT84DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 691 \"int84div\" (divide ).");
				return false; 
			case INT48DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 695 \"int48div\" (divide ).");
				return false; 
			case INT82DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 821 \"int82div\" (divide ).");
				return false; 
			case CASH_DIV_FLT8 : //divide
				elog(WARNING, "Unsupported Operator Type ID: 909 \"cash_div_flt8\" (divide ).");
				return false; 
			case CASH_DIV_INT8 : //divide
				elog(WARNING, "Unsupported Operator Type ID: 3347 \"cash_div_int8\" (divide ).");
				return false; 
			case INTERVAL_DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 1585 \"interval_div\" (divide ).");
				return false; 
			case INT2DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 527 \"int2div\" (divide ).");
				return false; 
			case INT24DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 546 \"int24div\" (divide ).");
				return false; 
			case INT42DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 547 \"int42div\" (divide ).");
				return false; 
			case PATH_DIV_PT : //divide (rotate/scale path)
				elog(WARNING, "Unsupported Operator Type ID: 739 \"path_div_pt\" (divide (rotate/scale path) ).");
				return false; 
			case FLOAT48DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 1118 \"float48div\" (divide ).");
				return false; 
			case INT8DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 687 \"int8div\" (divide ).");
				return false; 
			case NUMERIC_DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 1761 \"numeric_div\" (divide ).");
				return false; 
			case INT4DIV : //divide
				elog(WARNING, "Unsupported Operator Type ID: 528 \"int4div\" (divide ).");
				return false; 
			case BOX_DIV : //divide box by point (scale)
				elog(WARNING, "Unsupported Operator Type ID: 807 \"box_div\" (divide box by point (scale) ).");
				return false; 
			case CASH_DIV_INT4 : //divide
				elog(WARNING, "Unsupported Operator Type ID: 913 \"cash_div_int4\" (divide ).");
				return false; 
			case POINT_DIV : //divide points (scale/rotate)
				elog(WARNING, "Unsupported Operator Type ID: 734 \"point_div\" (divide points (scale/rotate) ).");
				return false; 

			// ~<~, less than
			case TEXT_PATTERN_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 2314 \"text_pattern_lt\" (less than ).");
				return false; 
			case BPCHAR_PATTERN_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 2326 \"bpchar_pattern_lt\" (less than ).");
				return false; 

			// ##, closest point to A on B
			case CLOSE_LB : //closest point to A on B
				elog(WARNING, "Unsupported Operator Type ID: 1568 \"close_lb\" (closest point to A on B ).");
				return false; 
			case CLOSE_LSEG : //closest point to A on B
				elog(WARNING, "Unsupported Operator Type ID: 1578 \"close_lseg\" (closest point to A on B ).");
				return false; 
			case CLOSE_PL : //closest point to A on B
				elog(WARNING, "Unsupported Operator Type ID: 1557 \"close_pl\" (closest point to A on B ).");
				return false; 
			case CLOSE_PB : //closest point to A on B
				elog(WARNING, "Unsupported Operator Type ID: 1559 \"close_pb\" (closest point to A on B ).");
				return false; 
			case CLOSE_PS : //closest point to A on B
				elog(WARNING, "Unsupported Operator Type ID: 1558 \"close_ps\" (closest point to A on B ).");
				return false; 
			case CLOSE_LS : //closest point to A on B
				elog(WARNING, "Unsupported Operator Type ID: 1577 \"close_ls\" (closest point to A on B ).");
				return false; 
			case CLOSE_SL : //closest point to A on B
				elog(WARNING, "Unsupported Operator Type ID: 1566 \"close_sl\" (closest point to A on B ).");
				return false; 
			case CLOSE_SB : //closest point to A on B
				elog(WARNING, "Unsupported Operator Type ID: 1567 \"close_sb\" (closest point to A on B ).");
				return false; 

			// >>=, is supernet or equal
			case NETWORK_SUPEQ : //is supernet or equal
				elog(WARNING, "Unsupported Operator Type ID: 934 \"network_supeq\" (is supernet or equal ).");
				return false; 

			// ~>=~, greater than or equal
			case TEXT_PATTERN_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2317 \"text_pattern_ge\" (greater than or equal ).");
				return false; 
			case BPCHAR_PATTERN_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2329 \"bpchar_pattern_ge\" (greater than or equal ).");
				return false; 

			// ||/, cube root
			case DCBRT : //cube root
				elog(WARNING, "Unsupported Operator Type ID: 597 \"dcbrt\" (cube root ).");
				return false; 

			// ?&, all keys exist
			case JSONB_EXISTS_ALL : //all keys exist
				elog(WARNING, "Unsupported Operator Type ID: 3249 \"jsonb_exists_all\" (all keys exist ).");
				return false; 

			// <->, distance between
			case DIST_LB : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 1524 \"dist_lb\" (distance between ).");
				return false; 
			case DIST_PPATH : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 618 \"dist_ppath\" (distance between ).");
				return false; 
			case LINE_DISTANCE : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 708 \"line_distance\" (distance between ).");
				return false; 
			case LSEG_DISTANCE : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 709 \"lseg_distance\" (distance between ).");
				return false; 
			case DIST_PL : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 613 \"dist_pl\" (distance between ).");
				return false; 
			case BOX_DISTANCE : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 706 \"box_distance\" (distance between ).");
				return false; 
			case PATH_DISTANCE : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 707 \"path_distance\" (distance between ).");
				return false; 
			case DIST_SL : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 616 \"dist_sl\" (distance between ).");
				return false; 
			case DIST_SB : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 617 \"dist_sb\" (distance between ).");
				return false; 
			case DIST_PS : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 614 \"dist_ps\" (distance between ).");
				return false; 
			case DIST_PB : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 615 \"dist_pb\" (distance between ).");
				return false; 
			case DIST_PPOLY : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 3276 \"dist_ppoly\" (distance between ).");
				return false; 
			case DIST_POLYP : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 3289 \"dist_polyp\" (distance between ).");
				return false; 
			case CIRCLE_DISTANCE : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 1520 \"circle_distance\" (distance between ).");
				return false; 
			case DIST_PC : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 1522 \"dist_pc\" (distance between ).");
				return false; 
			case DIST_CPOLY : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 1523 \"dist_cpoly\" (distance between ).");
				return false; 
			case POLY_DISTANCE : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 712 \"poly_distance\" (distance between ).");
				return false; 
			case DIST_CPOINT : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 3291 \"dist_cpoint\" (distance between ).");
				return false; 
			case POINT_DISTANCE : //distance between
				elog(WARNING, "Unsupported Operator Type ID: 517 \"point_distance\" (distance between ).");
				return false; 

			// ->>, get jsonb array element as text
			case JSONB_ARRAY_ELEMENT_TEXT : //get jsonb array element as text
				elog(WARNING, "Unsupported Operator Type ID: 3481 \"jsonb_array_element_text\" (get jsonb array element as text ).");
				return false; 
			case JSONB_OBJECT_FIELD_TEXT : //get jsonb object field as text
				elog(WARNING, "Unsupported Operator Type ID: 3477 \"jsonb_object_field_text\" (get jsonb object field as text ).");
				return false; 
			case JSON_ARRAY_ELEMENT_TEXT : //get json array element as text
				elog(WARNING, "Unsupported Operator Type ID: 3965 \"json_array_element_text\" (get json array element as text ).");
				return false; 
			case JSON_OBJECT_FIELD_TEXT : //get json object field as text
				elog(WARNING, "Unsupported Operator Type ID: 3963 \"json_object_field_text\" (get json object field as text ).");
				return false; 

			// <#>, convert to tinterval
			case MKTINTERVAL : //convert to tinterval
				elog(WARNING, "Unsupported Operator Type ID: 606 \"mktinterval\" (convert to tinterval ).");
				return false; 

			// #>=, greater than or equal by length
			case TINTERVALLENGE : //greater than or equal by length
				elog(WARNING, "Unsupported Operator Type ID: 580 \"tintervallenge\" (greater than or equal by length ).");
				return false; 

			// =, equal
			case OIDEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 607 \"oideq\" (equal ).");
				return false; 
			case FLOAT4EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 620 \"float4eq\" (equal ).");
				return false; 
			case MACADDR_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1220 \"macaddr_eq\" (equal ).");
				return false; 
			case FLOAT48EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1120 \"float48eq\" (equal ).");
				return false; 
			case XIDEQINT4 : //equal
				elog(WARNING, "Unsupported Operator Type ID: 353 \"xideqint4\" (equal ).");
				return false; 
			case NETWORK_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1201 \"network_eq\" (equal ).");
				return false; 
			case TIMESTAMPTZ_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1320 \"timestamptz_eq\" (equal ).");
				return false; 
			case ENUM_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 3516 \"enum_eq\" (equal ).");
				return false; 
			case UUID_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 2972 \"uuid_eq\" (equal ).");
				return false; 
			case ARRAY_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1070 \"array_eq\" (equal ).");
				return false; 
			case PG_LSN_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 3222 \"pg_lsn_eq\" (equal ).");
				return false; 
			case TSQUERY_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 3676 \"tsquery_eq\" (equal ).");
				return false; 
			case TIMESTAMPTZ_EQ_TIMESTAMP : //equal
				elog(WARNING, "Unsupported Operator Type ID: 2542 \"timestamptz_eq_timestamp\" (equal ).");
				return false; 
			case DATE_EQ_TIMESTAMP : //equal
				elog(WARNING, "Unsupported Operator Type ID: 2347 \"date_eq_timestamp\" (equal ).");
				return false; 
			case ACLITEMEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 974 \"aclitemeq\" (equal ).");
				return false; 
			case BPCHAREQ : //equal
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 1054 \"bpchareq\" (equal ).");
				return true; 
			case PATH_N_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 792 \"path_n_eq\" (equal ).");
				return false; 
			case LINE_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1616 \"line_eq\" (equal ).");
				return false; 
			case INT84EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 416 \"int84eq\" (equal ).");
				return false; 
			case MACADDR8_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 3362 \"macaddr8_eq\" (equal ).");
				return false; 
			case CIRCLE_EQ : //equal by area
				elog(WARNING, "Unsupported Operator Type ID: 1500 \"circle_eq\" (equal by area ).");
				return false; 
			case TIMESTAMPTZ_EQ_DATE : //equal
				elog(WARNING, "Unsupported Operator Type ID: 2386 \"timestamptz_eq_date\" (equal ).");
				return false; 
			case TINTERVALEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 811 \"tintervaleq\" (equal ).");
				return false; 
			case NUMERIC_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1752 \"numeric_eq\" (equal ).");
				return false; 
			case XIDEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 352 \"xideq\" (equal ).");
				return false; 
			case TSVECTOR_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 3629 \"tsvector_eq\" (equal ).");
				return false; 
			case DATE_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1093 \"date_eq\" (equal ).");
				return false; 
			case ABSTIMEEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 560 \"abstimeeq\" (equal ).");
				return false; 
			case BITEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1784 \"biteq\" (equal ).");
				return false; 
			case FLOAT84EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1130 \"float84eq\" (equal ).");
				return false; 
			case FLOAT8EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 670 \"float8eq\" (equal ).");
				return false; 
			case INTERVAL_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1330 \"interval_eq\" (equal ).");
				return false; 
			case RELTIMEEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 566 \"reltimeeq\" (equal ).");
				return false; 
			case BYTEAEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1955 \"byteaeq\" (equal ).");
				return false; 
			case TEXTEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 98 \"texteq\" (equal ).");
				return false; 
			case INT28EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1862 \"int28eq\" (equal ).");
				return false; 
			case CIDEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 385 \"cideq\" (equal ).");
				return false; 
			case TIMESTAMP_EQ_DATE : //equal
				elog(WARNING, "Unsupported Operator Type ID: 2373 \"timestamp_eq_date\" (equal ).");
				return false; 
			case TIDEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 387 \"tideq\" (equal ).");
				return false; 
			case BOOLEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 91 \"booleq\" (equal ).");
				return false; 
			case INT82EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1868 \"int82eq\" (equal ).");
				return false; 
			case NAMEEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 93 \"nameeq\" (equal ).");
				return false; 
			case CHAREQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 92 \"chareq\" (equal ).");
				return false; 
			case INT2EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 94 \"int2eq\" (equal ).");
				return false; 
			case INT4EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 96 \"int4eq\" (equal ).");
				return false; 
			case TIMETZ_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1550 \"timetz_eq\" (equal ).");
				return false; 
			case CASH_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 900 \"cash_eq\" (equal ).");
				return false; 
			case VARBITEQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1804 \"varbiteq\" (equal ).");
				return false; 
			case INT48EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 15 \"int48eq\" (equal ).");
				return false; 
			case RECORD_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 2988 \"record_eq\" (equal ).");
				return false; 
			case RANGE_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 3882 \"range_eq\" (equal ).");
				return false; 
			case JSONB_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 3240 \"jsonb_eq\" (equal ).");
				return false; 
			case BOX_EQ : //equal by area
				elog(WARNING, "Unsupported Operator Type ID: 503 \"box_eq\" (equal by area ).");
				return false; 
			case LSEG_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1535 \"lseg_eq\" (equal ).");
				return false; 
			case INT8EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 410 \"int8eq\" (equal ).");
				return false; 
			case INT24EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 532 \"int24eq\" (equal ).");
				return false; 
			case OIDVECTOREQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 649 \"oidvectoreq\" (equal ).");
				return false; 
			case TIMESTAMP_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 2060 \"timestamp_eq\" (equal ).");
				return false; 
			case DATE_EQ_TIMESTAMPTZ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 2360 \"date_eq_timestamptz\" (equal ).");
				return false; 
			case TIMESTAMP_EQ_TIMESTAMPTZ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 2536 \"timestamp_eq_timestamptz\" (equal ).");
				return false; 
			case TIME_EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 1108 \"time_eq\" (equal ).");
				return false; 
			case INT42EQ : //equal
				elog(WARNING, "Unsupported Operator Type ID: 533 \"int42eq\" (equal ).");
				return false; 

			// <, less than
			case TIMESTAMPTZ_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1322 \"timestamptz_lt\" (less than ).");
				return false; 
			case FLOAT4LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 622 \"float4lt\" (less than ).");
				return false; 
			case MACADDR_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1222 \"macaddr_lt\" (less than ).");
				return false; 
			case UUID_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 2974 \"uuid_lt\" (less than ).");
				return false; 
			case DATE_LT : //less than
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 1095 \"date_lt\" (less than ).");
				return true; 
			case TSVECTOR_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 3627 \"tsvector_lt\" (less than ).");
				return false; 
			case FLOAT48LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1122 \"float48lt\" (less than ).");
				return false; 
			case TEXT_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 664 \"text_lt\" (less than ).");
				return false; 
			case OIDLT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 609 \"oidlt\" (less than ).");
				return false; 
			case NETWORK_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1203 \"network_lt\" (less than ).");
				return false; 
			case NAMELT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 660 \"namelt\" (less than ).");
				return false; 
			case INT82LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1870 \"int82lt\" (less than ).");
				return false; 
			case DATE_LT_TIMESTAMPTZ : //less than
				elog(WARNING, "Unsupported Operator Type ID: 2358 \"date_lt_timestamptz\" (less than ).");
				return false; 
			case TSQUERY_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 3674 \"tsquery_lt\" (less than ).");
				return false; 
			case DATE_LT_TIMESTAMP : //less than
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 2345 \"date_lt_timestamp\" (less than ).");
				return true; 
			case TIMESTAMPTZ_LT_TIMESTAMP : //less than
				elog(WARNING, "Unsupported Operator Type ID: 2540 \"timestamptz_lt_timestamp\" (less than ).");
				return false; 
			case PATH_N_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 793 \"path_n_lt\" (less than ).");
				return false; 
			case MACADDR8_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 3364 \"macaddr8_lt\" (less than ).");
				return false; 
			case ARRAY_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1072 \"array_lt\" (less than ).");
				return false; 
			case LSEG_LT : //less than by length
				elog(WARNING, "Unsupported Operator Type ID: 1587 \"lseg_lt\" (less than by length ).");
				return false; 
			case CIRCLE_LT : //less than by area
				elog(WARNING, "Unsupported Operator Type ID: 1502 \"circle_lt\" (less than by area ).");
				return false; 
			case TIME_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1110 \"time_lt\" (less than ).");
				return false; 
			case INT24LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 534 \"int24lt\" (less than ).");
				return false; 
			case TIMESTAMPTZ_LT_DATE : //less than
				elog(WARNING, "Unsupported Operator Type ID: 2384 \"timestamptz_lt_date\" (less than ).");
				return false; 
			case BOX_LT : //less than by area
				elog(WARNING, "Unsupported Operator Type ID: 504 \"box_lt\" (less than by area ).");
				return false; 
			case NUMERIC_LT : //less than
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 1754 \"numeric_lt\" (less than ).");
				return true; 
			case RELTIMELT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 568 \"reltimelt\" (less than ).");
				return false; 
			case CHARLT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 631 \"charlt\" (less than ).");
				return false; 
			case RANGE_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 3884 \"range_lt\" (less than ).");
				return false; 
			case FLOAT84LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1132 \"float84lt\" (less than ).");
				return false; 
			case ABSTIMELT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 562 \"abstimelt\" (less than ).");
				return false; 
			case FLOAT8LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 672 \"float8lt\" (less than ).");
				return false; 
			case INTERVAL_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1332 \"interval_lt\" (less than ).");
				return false; 
			case BYTEALT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1957 \"bytealt\" (less than ).");
				return false; 
			case ENUM_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 3518 \"enum_lt\" (less than ).");
				return false; 
			case BITLT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1786 \"bitlt\" (less than ).");
				return false; 
			case TINTERVALLT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 813 \"tintervallt\" (less than ).");
				return false; 
			case INT2LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 95 \"int2lt\" (less than ).");
				return false; 
			case INT4LT : //less than
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 97 \"int4lt\" (less than ).");
				return true; 
			case CASH_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 902 \"cash_lt\" (less than ).");
				return false; 
			case VARBITLT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1806 \"varbitlt\" (less than ).");
				return false; 
			case TIMETZ_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1552 \"timetz_lt\" (less than ).");
				return false; 
			case BOOLLT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 58 \"boollt\" (less than ).");
				return false; 
			case RECORD_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 2990 \"record_lt\" (less than ).");
				return false; 
			case PG_LSN_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 3224 \"pg_lsn_lt\" (less than ).");
				return false; 
			case INT48LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 37 \"int48lt\" (less than ).");
				return false; 
			case BPCHARLT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1058 \"bpcharlt\" (less than ).");
				return false; 
			case INT42LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 535 \"int42lt\" (less than ).");
				return false; 
			case TIDLT : //less than
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 2799 \"tidlt\" (less than ).");
				return true; 
			case OIDVECTORLT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 645 \"oidvectorlt\" (less than ).");
				return false; 
			case INT8LT : //less than
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 412 \"int8lt\" (less than ).");
				return true; 
			case JSONB_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 3242 \"jsonb_lt\" (less than ).");
				return false; 
			case TIMESTAMP_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 2062 \"timestamp_lt\" (less than ).");
				return false; 
			case INT84LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 418 \"int84lt\" (less than ).");
				return false; 
			case TIMESTAMP_LT_DATE : //less than
				elog(WARNING, "Unsupported Operator Type ID: 2371 \"timestamp_lt_date\" (less than ).");
				return false; 
			case TIMESTAMP_LT_TIMESTAMPTZ : //less than
				elog(WARNING, "Unsupported Operator Type ID: 2534 \"timestamp_lt_timestamptz\" (less than ).");
				return false; 
			case INT28LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 1864 \"int28lt\" (less than ).");
				return false; 

			// ?, key exists
			case JSONB_EXISTS : //key exists
				elog(WARNING, "Unsupported Operator Type ID: 3247 \"jsonb_exists\" (key exists ).");
				return false; 

			// >, greater than
			case FLOAT4GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 623 \"float4gt\" (greater than ).");
				return false; 
			case MACADDR_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1224 \"macaddr_gt\" (greater than ).");
				return false; 
			case TIMESTAMPTZ_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1324 \"timestamptz_gt\" (greater than ).");
				return false; 
			case RECORD_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 2991 \"record_gt\" (greater than ).");
				return false; 
			case NETWORK_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1205 \"network_gt\" (greater than ).");
				return false; 
			case TEXT_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 666 \"text_gt\" (greater than ).");
				return false; 
			case FLOAT48GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1123 \"float48gt\" (greater than ).");
				return false; 
			case TSVECTOR_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 3632 \"tsvector_gt\" (greater than ).");
				return false; 
			case NAMEGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 662 \"namegt\" (greater than ).");
				return false; 
			case BITGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1787 \"bitgt\" (greater than ).");
				return false; 
			case DATE_GT : //greater than
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 1097 \"date_gt\" (greater than ).");
				return true; 
			case INT82GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1871 \"int82gt\" (greater than ).");
				return false; 
			case TIDGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 2800 \"tidgt\" (greater than ).");
				return false; 
			case ENUM_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 3519 \"enum_gt\" (greater than ).");
				return false; 
			case LSEG_GT : //greater than by length
				elog(WARNING, "Unsupported Operator Type ID: 1589 \"lseg_gt\" (greater than by length ).");
				return false; 
			case CIRCLE_GT : //greater than by area
				elog(WARNING, "Unsupported Operator Type ID: 1503 \"circle_gt\" (greater than by area ).");
				return false; 
			case MACADDR8_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 3366 \"macaddr8_gt\" (greater than ).");
				return false; 
			case DATE_GT_TIMESTAMP : //greater than
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 2349 \"date_gt_timestamp\" (greater than ).");
				return true; 
			case ARRAY_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1073 \"array_gt\" (greater than ).");
				return false; 
			case FLOAT84GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1133 \"float84gt\" (greater than ).");
				return false; 
			case TIME_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1112 \"time_gt\" (greater than ).");
				return false; 
			case INT2GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 520 \"int2gt\" (greater than ).");
				return false; 
			case INT4GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 521 \"int4gt\" (greater than ).");
				return false; 
			case JSONB_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 3243 \"jsonb_gt\" (greater than ).");
				return false; 
			case NUMERIC_GT : //greater than
				if (xpg_codegendebug)
					elog(INFO, "Supported Operator Type ID: 1756 \"numeric_gt\" (greater than ).");
				return true; 
			case BOX_GT : //greater than by area
				elog(WARNING, "Unsupported Operator Type ID: 502 \"box_gt\" (greater than by area ).");
				return false; 
			case DATE_GT_TIMESTAMPTZ : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 2362 \"date_gt_timestamptz\" (greater than ).");
				return false; 
			case OIDVECTORGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 646 \"oidvectorgt\" (greater than ).");
				return false; 
			case RELTIMEGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 569 \"reltimegt\" (greater than ).");
				return false; 
			case CHARGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 633 \"chargt\" (greater than ).");
				return false; 
			case FLOAT8GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 674 \"float8gt\" (greater than ).");
				return false; 
			case INTERVAL_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1334 \"interval_gt\" (greater than ).");
				return false; 
			case OIDGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 610 \"oidgt\" (greater than ).");
				return false; 
			case ABSTIMEGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 563 \"abstimegt\" (greater than ).");
				return false; 
			case TINTERVALGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 814 \"tintervalgt\" (greater than ).");
				return false; 
			case INT48GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 76 \"int48gt\" (greater than ).");
				return false; 
			case PATH_N_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 794 \"path_n_gt\" (greater than ).");
				return false; 
			case INT28GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1865 \"int28gt\" (greater than ).");
				return false; 
			case TIMESTAMPTZ_GT_DATE : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 2388 \"timestamptz_gt_date\" (greater than ).");
				return false; 
			case BYTEAGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1959 \"byteagt\" (greater than ).");
				return false; 
			case UUID_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 2975 \"uuid_gt\" (greater than ).");
				return false; 
			case VARBITGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1807 \"varbitgt\" (greater than ).");
				return false; 
			case CASH_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 903 \"cash_gt\" (greater than ).");
				return false; 
			case RANGE_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 3887 \"range_gt\" (greater than ).");
				return false; 
			case BOOLGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 59 \"boolgt\" (greater than ).");
				return false; 
			case TIMETZ_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1554 \"timetz_gt\" (greater than ).");
				return false; 
			case PG_LSN_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 3225 \"pg_lsn_gt\" (greater than ).");
				return false; 
			case BPCHARGT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 1060 \"bpchargt\" (greater than ).");
				return false; 
			case TIMESTAMPTZ_GT_TIMESTAMP : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 2544 \"timestamptz_gt_timestamp\" (greater than ).");
				return false; 
			case INT42GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 537 \"int42gt\" (greater than ).");
				return false; 
			case INT24GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 536 \"int24gt\" (greater than ).");
				return false; 
			case TIMESTAMP_GT_TIMESTAMPTZ : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 2538 \"timestamp_gt_timestamptz\" (greater than ).");
				return false; 
			case TSQUERY_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 3679 \"tsquery_gt\" (greater than ).");
				return false; 
			case INT8GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 413 \"int8gt\" (greater than ).");
				return false; 
			case TIMESTAMP_GT_DATE : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 2375 \"timestamp_gt_date\" (greater than ).");
				return false; 
			case TIMESTAMP_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 2064 \"timestamp_gt\" (greater than ).");
				return false; 
			case INT84GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 419 \"int84gt\" (greater than ).");
				return false; 

			// *<, less than
			case RECORD_IMAGE_LT : //less than
				elog(WARNING, "Unsupported Operator Type ID: 3190 \"record_image_lt\" (less than ).");
				return false; 

			// @, absolute value
			case FLOAT4ABS : //absolute value
				elog(WARNING, "Unsupported Operator Type ID: 590 \"float4abs\" (absolute value ).");
				return false; 
			case FLOAT8ABS : //absolute value
				elog(WARNING, "Unsupported Operator Type ID: 595 \"float8abs\" (absolute value ).");
				return false; 
			case INT4ABS : //absolute value
				elog(WARNING, "Unsupported Operator Type ID: 773 \"int4abs\" (absolute value ).");
				return false; 
			case INT8ABS : //absolute value
				elog(WARNING, "Unsupported Operator Type ID: 473 \"int8abs\" (absolute value ).");
				return false; 
			case NUMERIC_ABS : //absolute value
				elog(WARNING, "Unsupported Operator Type ID: 1763 \"numeric_abs\" (absolute value ).");
				return false; 
			case INT2ABS : //absolute value
				elog(WARNING, "Unsupported Operator Type ID: 682 \"int2abs\" (absolute value ).");
				return false; 

			// #<>, not equal by length
			case TINTERVALLENNE : //not equal by length
				elog(WARNING, "Unsupported Operator Type ID: 576 \"tintervallenne\" (not equal by length ).");
				return false; 

			// @-@, distance between endpoints
			case LSEG_LENGTH : //distance between endpoints
				elog(WARNING, "Unsupported Operator Type ID: 1591 \"lseg_length\" (distance between endpoints ).");
				return false; 
			case PATH_LENGTH : //sum of path segment lengths
				elog(WARNING, "Unsupported Operator Type ID: 799 \"path_length\" (sum of path segment lengths ).");
				return false; 

			// ~=, same as
			case POINT_EQ : //same as
				elog(WARNING, "Unsupported Operator Type ID: 510 \"point_eq\" (same as ).");
				return false; 
			case CIRCLE_SAME : //same as
				elog(WARNING, "Unsupported Operator Type ID: 1512 \"circle_same\" (same as ).");
				return false; 
			case TINTERVALSAME : //same as
				elog(WARNING, "Unsupported Operator Type ID: 572 \"tintervalsame\" (same as ).");
				return false; 
			case POLY_SAME : //same as
				elog(WARNING, "Unsupported Operator Type ID: 491 \"poly_same\" (same as ).");
				return false; 
			case BOX_SAME : //same as
				elog(WARNING, "Unsupported Operator Type ID: 499 \"box_same\" (same as ).");
				return false; 

			// #>>, get value from json as text with path elements
			case JSON_EXTRACT_PATH_TEXT : //get value from json as text with path elements
				elog(WARNING, "Unsupported Operator Type ID: 3967 \"json_extract_path_text\" (get value from json as text with path elements ).");
				return false; 
			case JSONB_EXTRACT_PATH_TEXT : //get value from jsonb as text with path elements
				elog(WARNING, "Unsupported Operator Type ID: 3206 \"jsonb_extract_path_text\" (get value from jsonb as text with path elements ).");
				return false; 

			// !~~*, does not match LIKE expression, case-insensitive
			case BPCHARICNLIKE : //does not match LIKE expression, case-insensitive
				elog(WARNING, "Unsupported Operator Type ID: 1630 \"bpcharicnlike\" (does not match LIKE expression, case-insensitive ).");
				return false; 
			case TEXTICNLIKE : //does not match LIKE expression, case-insensitive
				elog(WARNING, "Unsupported Operator Type ID: 1628 \"texticnlike\" (does not match LIKE expression, case-insensitive ).");
				return false; 
			case NAMEICNLIKE : //does not match LIKE expression, case-insensitive
				elog(WARNING, "Unsupported Operator Type ID: 1626 \"nameicnlike\" (does not match LIKE expression, case-insensitive ).");
				return false; 

			// |&>, overlaps or is above
			case POLY_OVERABOVE : //overlaps or is above
				elog(WARNING, "Unsupported Operator Type ID: 2576 \"poly_overabove\" (overlaps or is above ).");
				return false; 
			case CIRCLE_OVERABOVE : //overlaps or is above
				elog(WARNING, "Unsupported Operator Type ID: 2590 \"circle_overabove\" (overlaps or is above ).");
				return false; 
			case BOX_OVERABOVE : //overlaps or is above
				elog(WARNING, "Unsupported Operator Type ID: 2572 \"box_overabove\" (overlaps or is above ).");
				return false; 

			// &<, overlaps or is left of
			case BOX_OVERLEFT : //overlaps or is left of
				elog(WARNING, "Unsupported Operator Type ID: 494 \"box_overleft\" (overlaps or is left of ).");
				return false; 
			case POLY_OVERLEFT : //overlaps or is left of
				elog(WARNING, "Unsupported Operator Type ID: 486 \"poly_overleft\" (overlaps or is left of ).");
				return false; 
			case CIRCLE_OVERLEFT : //overlaps or is left of
				elog(WARNING, "Unsupported Operator Type ID: 1507 \"circle_overleft\" (overlaps or is left of ).");
				return false; 
			case RANGE_OVERLEFT : //overlaps or is left of
				elog(WARNING, "Unsupported Operator Type ID: 3895 \"range_overleft\" (overlaps or is left of ).");
				return false; 

			// &<|, overlaps or is below
			case POLY_OVERBELOW : //overlaps or is below
				elog(WARNING, "Unsupported Operator Type ID: 2575 \"poly_overbelow\" (overlaps or is below ).");
				return false; 
			case CIRCLE_OVERBELOW : //overlaps or is below
				elog(WARNING, "Unsupported Operator Type ID: 2589 \"circle_overbelow\" (overlaps or is below ).");
				return false; 
			case BOX_OVERBELOW : //overlaps or is below
				elog(WARNING, "Unsupported Operator Type ID: 2571 \"box_overbelow\" (overlaps or is below ).");
				return false; 

			// &>, overlaps or is right of
			case POLY_OVERRIGHT : //overlaps or is right of
				elog(WARNING, "Unsupported Operator Type ID: 487 \"poly_overright\" (overlaps or is right of ).");
				return false; 
			case BOX_OVERRIGHT : //overlaps or is right of
				elog(WARNING, "Unsupported Operator Type ID: 495 \"box_overright\" (overlaps or is right of ).");
				return false; 
			case CIRCLE_OVERRIGHT : //overlaps or is right of
				elog(WARNING, "Unsupported Operator Type ID: 1508 \"circle_overright\" (overlaps or is right of ).");
				return false; 
			case RANGE_OVERRIGHT : //overlaps or is right of
				elog(WARNING, "Unsupported Operator Type ID: 3896 \"range_overright\" (overlaps or is right of ).");
				return false; 

			// *=, identical
			case RECORD_IMAGE_EQ : //identical
				elog(WARNING, "Unsupported Operator Type ID: 3188 \"record_image_eq\" (identical ).");
				return false; 

			// ?-, horizontal
			case LINE_HORIZONTAL : //horizontal
				elog(WARNING, "Unsupported Operator Type ID: 1614 \"line_horizontal\" (horizontal ).");
				return false; 
			case POINT_HORIZ : //horizontally aligned
				elog(WARNING, "Unsupported Operator Type ID: 808 \"point_horiz\" (horizontally aligned ).");
				return false; 
			case LSEG_HORIZONTAL : //horizontal
				elog(WARNING, "Unsupported Operator Type ID: 1528 \"lseg_horizontal\" (horizontal ).");
				return false; 

			// &&, overlaps
			case BOX_OVERLAP : //overlaps
				elog(WARNING, "Unsupported Operator Type ID: 500 \"box_overlap\" (overlaps ).");
				return false; 
			case NETWORK_OVERLAP : //overlaps (is subnet or supernet)
				elog(WARNING, "Unsupported Operator Type ID: 3552 \"network_overlap\" (overlaps (is subnet or supernet) ).");
				return false; 
			case ARRAYOVERLAP : //overlaps
				elog(WARNING, "Unsupported Operator Type ID: 2750 \"arrayoverlap\" (overlaps ).");
				return false; 
			case POLY_OVERLAP : //overlaps
				elog(WARNING, "Unsupported Operator Type ID: 492 \"poly_overlap\" (overlaps ).");
				return false; 
			case TSQUERY_AND : //AND-concatenate
				elog(WARNING, "Unsupported Operator Type ID: 3680 \"tsquery_and\" (AND-concatenate ).");
				return false; 
			case RANGE_OVERLAPS : //overlaps
				elog(WARNING, "Unsupported Operator Type ID: 3888 \"range_overlaps\" (overlaps ).");
				return false; 
			case CIRCLE_OVERLAP : //overlaps
				elog(WARNING, "Unsupported Operator Type ID: 1513 \"circle_overlap\" (overlaps ).");
				return false; 
			case TINTERVALOV : //overlaps
				elog(WARNING, "Unsupported Operator Type ID: 574 \"tintervalov\" (overlaps ).");
				return false; 

			// |/, square root
			case DSQRT : //square root
				elog(WARNING, "Unsupported Operator Type ID: 596 \"dsqrt\" (square root ).");
				return false; 

			// <?>, is contained by
			case INTINTERVAL : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 583 \"intinterval\" (is contained by ).");
				return false; 

			// <<, contains
			case TINTERVALCT : //contains
				elog(WARNING, "Unsupported Operator Type ID: 573 \"tintervalct\" (contains ).");
				return false; 
			case BITSHIFTLEFT : //bitwise shift left
				elog(WARNING, "Unsupported Operator Type ID: 1795 \"bitshiftleft\" (bitwise shift left ).");
				return false; 
			case CIRCLE_LEFT : //is left of
				elog(WARNING, "Unsupported Operator Type ID: 1506 \"circle_left\" (is left of ).");
				return false; 
			case RANGE_BEFORE : //is left of
				elog(WARNING, "Unsupported Operator Type ID: 3893 \"range_before\" (is left of ).");
				return false; 
			case POINT_LEFT : //is left of
				elog(WARNING, "Unsupported Operator Type ID: 507 \"point_left\" (is left of ).");
				return false; 
			case POLY_LEFT : //is left of
				elog(WARNING, "Unsupported Operator Type ID: 485 \"poly_left\" (is left of ).");
				return false; 
			case INT4SHL : //bitwise shift left
				elog(WARNING, "Unsupported Operator Type ID: 1884 \"int4shl\" (bitwise shift left ).");
				return false; 
			case INT8SHL : //bitwise shift left
				elog(WARNING, "Unsupported Operator Type ID: 1890 \"int8shl\" (bitwise shift left ).");
				return false; 
			case NETWORK_SUB : //is subnet
				elog(WARNING, "Unsupported Operator Type ID: 931 \"network_sub\" (is subnet ).");
				return false; 
			case INT2SHL : //bitwise shift left
				elog(WARNING, "Unsupported Operator Type ID: 1878 \"int2shl\" (bitwise shift left ).");
				return false; 
			case BOX_LEFT : //is left of
				elog(WARNING, "Unsupported Operator Type ID: 493 \"box_left\" (is left of ).");
				return false; 

			// !~, does not match regular expression, case-sensitive
			case NAMEREGEXNE : //does not match regular expression, case-sensitive
				elog(WARNING, "Unsupported Operator Type ID: 640 \"nameregexne\" (does not match regular expression, case-sensitive ).");
				return false; 
			case TEXTREGEXNE : //does not match regular expression, case-sensitive
				elog(WARNING, "Unsupported Operator Type ID: 642 \"textregexne\" (does not match regular expression, case-sensitive ).");
				return false; 
			case BPCHARREGEXNE : //does not match regular expression, case-sensitive
				elog(WARNING, "Unsupported Operator Type ID: 1056 \"bpcharregexne\" (does not match regular expression, case-sensitive ).");
				return false; 

			// |>>, is above
			case CIRCLE_ABOVE : //is above
				elog(WARNING, "Unsupported Operator Type ID: 1514 \"circle_above\" (is above ).");
				return false; 
			case BOX_ABOVE : //is above
				elog(WARNING, "Unsupported Operator Type ID: 2573 \"box_above\" (is above ).");
				return false; 
			case POLY_ABOVE : //is above
				elog(WARNING, "Unsupported Operator Type ID: 2577 \"poly_above\" (is above ).");
				return false; 

			// ~>~, greater than
			case BPCHAR_PATTERN_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 2330 \"bpchar_pattern_gt\" (greater than ).");
				return false; 
			case TEXT_PATTERN_GT : //greater than
				elog(WARNING, "Unsupported Operator Type ID: 2318 \"text_pattern_gt\" (greater than ).");
				return false; 

			// >^, is above
			case POINT_ABOVE : //is above
				elog(WARNING, "Unsupported Operator Type ID: 506 \"point_above\" (is above ).");
				return false; 
			case BOX_ABOVE_EQ : //is above (allows touching)
				elog(WARNING, "Unsupported Operator Type ID: 800 \"box_above_eq\" (is above (allows touching) ).");
				return false; 

			// <<|, is below
			case CIRCLE_BELOW : //is below
				elog(WARNING, "Unsupported Operator Type ID: 1515 \"circle_below\" (is below ).");
				return false; 
			case POLY_BELOW : //is below
				elog(WARNING, "Unsupported Operator Type ID: 2574 \"poly_below\" (is below ).");
				return false; 
			case BOX_BELOW : //is below
				elog(WARNING, "Unsupported Operator Type ID: 2570 \"box_below\" (is below ).");
				return false; 

			// <^, is below (allows touching)
			case BOX_BELOW_EQ : //is below (allows touching)
				elog(WARNING, "Unsupported Operator Type ID: 801 \"box_below_eq\" (is below (allows touching) ).");
				return false; 
			case POINT_BELOW : //is below
				elog(WARNING, "Unsupported Operator Type ID: 509 \"point_below\" (is below ).");
				return false; 

			// ?||, parallel
			case LSEG_PARALLEL : //parallel
				elog(WARNING, "Unsupported Operator Type ID: 1526 \"lseg_parallel\" (parallel ).");
				return false; 
			case LINE_PARALLEL : //parallel
				elog(WARNING, "Unsupported Operator Type ID: 1612 \"line_parallel\" (parallel ).");
				return false; 

			// !~~, does not match LIKE expression
			case BPCHARNLIKE : //does not match LIKE expression
				elog(WARNING, "Unsupported Operator Type ID: 1212 \"bpcharnlike\" (does not match LIKE expression ).");
				return false; 
			case TEXTNLIKE : //does not match LIKE expression
				elog(WARNING, "Unsupported Operator Type ID: 1210 \"textnlike\" (does not match LIKE expression ).");
				return false; 
			case BYTEANLIKE : //does not match LIKE expression
				elog(WARNING, "Unsupported Operator Type ID: 2017 \"byteanlike\" (does not match LIKE expression ).");
				return false; 
			case NAMENLIKE : //does not match LIKE expression
				elog(WARNING, "Unsupported Operator Type ID: 1208 \"namenlike\" (does not match LIKE expression ).");
				return false; 

			// ~*, matches regular expression, case-insensitive
			case BPCHARICREGEXEQ : //matches regular expression, case-insensitive
				elog(WARNING, "Unsupported Operator Type ID: 1234 \"bpcharicregexeq\" (matches regular expression, case-insensitive ).");
				return false; 
			case NAMEICREGEXEQ : //matches regular expression, case-insensitive
				elog(WARNING, "Unsupported Operator Type ID: 1226 \"nameicregexeq\" (matches regular expression, case-insensitive ).");
				return false; 
			case TEXTICREGEXEQ : //matches regular expression, case-insensitive
				elog(WARNING, "Unsupported Operator Type ID: 1228 \"texticregexeq\" (matches regular expression, case-insensitive ).");
				return false; 

			// ->, get json array element
			case JSON_ARRAY_ELEMENT : //get json array element
				elog(WARNING, "Unsupported Operator Type ID: 3964 \"json_array_element\" (get json array element ).");
				return false; 
			case JSON_OBJECT_FIELD : //get json object field
				elog(WARNING, "Unsupported Operator Type ID: 3962 \"json_object_field\" (get json object field ).");
				return false; 
			case JSONB_OBJECT_FIELD : //get jsonb object field
				elog(WARNING, "Unsupported Operator Type ID: 3211 \"jsonb_object_field\" (get jsonb object field ).");
				return false; 
			case JSONB_ARRAY_ELEMENT : //get jsonb array element
				elog(WARNING, "Unsupported Operator Type ID: 3212 \"jsonb_array_element\" (get jsonb array element ).");
				return false; 

			// *>=, greater than or equal
			case RECORD_IMAGE_GE : //greater than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3193 \"record_image_ge\" (greater than or equal ).");
				return false; 

			// <@, point on line
			case ON_PL : //point on line
				elog(WARNING, "Unsupported Operator Type ID: 1546 \"on_pl\" (point on line ).");
				return false; 
			case ON_PS : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 1547 \"on_ps\" (is contained by ).");
				return false; 
			case BOX_CONTAINED : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 497 \"box_contained\" (is contained by ).");
				return false; 
			case PT_CONTAINED_POLY : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 756 \"pt_contained_poly\" (is contained by ).");
				return false; 
			case JSONB_CONTAINED : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 3250 \"jsonb_contained\" (is contained by ).");
				return false; 
			case PT_CONTAINED_CIRCLE : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 758 \"pt_contained_circle\" (is contained by ).");
				return false; 
			case TSQ_MCONTAINED : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 3694 \"tsq_mcontained\" (is contained by ).");
				return false; 
			case ON_SL : //lseg on line
				elog(WARNING, "Unsupported Operator Type ID: 1548 \"on_sl\" (lseg on line ).");
				return false; 
			case ON_SB : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 1549 \"on_sb\" (is contained by ).");
				return false; 
			case RANGE_CONTAINED_BY : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 3892 \"range_contained_by\" (is contained by ).");
				return false; 
			case ARRAYCONTAINED : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 2752 \"arraycontained\" (is contained by ).");
				return false; 
			case POLY_CONTAINED : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 489 \"poly_contained\" (is contained by ).");
				return false; 
			case ON_PB : //point inside box
				elog(WARNING, "Unsupported Operator Type ID: 511 \"on_pb\" (point inside box ).");
				return false; 
			case ELEM_CONTAINED_BY_RANGE : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 3891 \"elem_contained_by_range\" (is contained by ).");
				return false; 
			case ON_PPATH : //point within closed path, or point on open path
				elog(WARNING, "Unsupported Operator Type ID: 512 \"on_ppath\" (point within closed path, or point on open path ).");
				return false; 
			case CIRCLE_CONTAINED : //is contained by
				elog(WARNING, "Unsupported Operator Type ID: 1510 \"circle_contained\" (is contained by ).");
				return false; 

			// ~<=~, less than or equal
			case TEXT_PATTERN_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2315 \"text_pattern_le\" (less than or equal ).");
				return false; 
			case BPCHAR_PATTERN_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 2327 \"bpchar_pattern_le\" (less than or equal ).");
				return false; 

			// *<=, less than or equal
			case RECORD_IMAGE_LE : //less than or equal
				elog(WARNING, "Unsupported Operator Type ID: 3192 \"record_image_le\" (less than or equal ).");
				return false; 

			// ~~*, matches LIKE expression, case-insensitive
			case BPCHARICLIKE : //matches LIKE expression, case-insensitive
				elog(WARNING, "Unsupported Operator Type ID: 1629 \"bpchariclike\" (matches LIKE expression, case-insensitive ).");
				return false; 
			case TEXTICLIKE : //matches LIKE expression, case-insensitive
				elog(WARNING, "Unsupported Operator Type ID: 1627 \"texticlike\" (matches LIKE expression, case-insensitive ).");
				return false; 
			case NAMEICLIKE : //matches LIKE expression, case-insensitive
				elog(WARNING, "Unsupported Operator Type ID: 1625 \"nameiclike\" (matches LIKE expression, case-insensitive ).");
				return false; 

			// *<>, not identical
			case RECORD_IMAGE_NE : //not identical
				elog(WARNING, "Unsupported Operator Type ID: 3189 \"record_image_ne\" (not identical ).");
				return false; 

			// @>, contains
			case POLY_CONTAIN_PT : //contains
				elog(WARNING, "Unsupported Operator Type ID: 757 \"poly_contain_pt\" (contains ).");
				return false; 
			case PATH_CONTAIN_PT : //contains
				elog(WARNING, "Unsupported Operator Type ID: 755 \"path_contain_pt\" (contains ).");
				return false; 
			case POLY_CONTAIN : //contains
				elog(WARNING, "Unsupported Operator Type ID: 490 \"poly_contain\" (contains ).");
				return false; 
			case CIRCLE_CONTAIN_PT : //contains
				elog(WARNING, "Unsupported Operator Type ID: 759 \"circle_contain_pt\" (contains ).");
				return false; 
			case TSQ_MCONTAINS : //contains
				elog(WARNING, "Unsupported Operator Type ID: 3693 \"tsq_mcontains\" (contains ).");
				return false; 
			case ACLCONTAINS : //contains
				elog(WARNING, "Unsupported Operator Type ID: 968 \"aclcontains\" (contains ).");
				return false; 
			case JSONB_CONTAINS : //contains
				elog(WARNING, "Unsupported Operator Type ID: 3246 \"jsonb_contains\" (contains ).");
				return false; 
			case RANGE_CONTAINS_ELEM : //contains
				elog(WARNING, "Unsupported Operator Type ID: 3889 \"range_contains_elem\" (contains ).");
				return false; 
			case BOX_CONTAIN : //contains
				elog(WARNING, "Unsupported Operator Type ID: 498 \"box_contain\" (contains ).");
				return false; 
			case ARRAYCONTAINS : //contains
				elog(WARNING, "Unsupported Operator Type ID: 2751 \"arraycontains\" (contains ).");
				return false; 
			case RANGE_CONTAINS : //contains
				elog(WARNING, "Unsupported Operator Type ID: 3890 \"range_contains\" (contains ).");
				return false; 
			case CIRCLE_CONTAIN : //contains
				elog(WARNING, "Unsupported Operator Type ID: 1511 \"circle_contain\" (contains ).");
				return false; 
			case BOX_CONTAIN_PT : //contains
				elog(WARNING, "Unsupported Operator Type ID: 433 \"box_contain_pt\" (contains ).");
				return false; 

			// |, start of interval
			case TINTERVALSTART : //start of interval
				elog(WARNING, "Unsupported Operator Type ID: 1284 \"tintervalstart\" (start of interval ).");
				return false; 
			case INT2OR : //bitwise or
				elog(WARNING, "Unsupported Operator Type ID: 1875 \"int2or\" (bitwise or ).");
				return false; 
			case MACADDR_OR : //bitwise or
				elog(WARNING, "Unsupported Operator Type ID: 3149 \"macaddr_or\" (bitwise or ).");
				return false; 
			case INT8OR : //bitwise or
				elog(WARNING, "Unsupported Operator Type ID: 1887 \"int8or\" (bitwise or ).");
				return false; 
			case BITOR : //bitwise or
				elog(WARNING, "Unsupported Operator Type ID: 1792 \"bitor\" (bitwise or ).");
				return false; 
			case INETOR : //bitwise or
				elog(WARNING, "Unsupported Operator Type ID: 2636 \"inetor\" (bitwise or ).");
				return false; 
			case INT4OR : //bitwise or
				elog(WARNING, "Unsupported Operator Type ID: 1881 \"int4or\" (bitwise or ).");
				return false; 
			case MACADDR8_OR : //bitwise or
				elog(WARNING, "Unsupported Operator Type ID: 3370 \"macaddr8_or\" (bitwise or ).");
				return false; 

			// !~*, does not match regular expression, case-insensitive
			case NAMEICREGEXNE : //does not match regular expression, case-insensitive
				elog(WARNING, "Unsupported Operator Type ID: 1227 \"nameicregexne\" (does not match regular expression, case-insensitive ).");
				return false; 
			case BPCHARICREGEXNE : //does not match regular expression, case-insensitive
				elog(WARNING, "Unsupported Operator Type ID: 1235 \"bpcharicregexne\" (does not match regular expression, case-insensitive ).");
				return false; 
			case TEXTICREGEXNE : //does not match regular expression, case-insensitive
				elog(WARNING, "Unsupported Operator Type ID: 1229 \"texticregexne\" (does not match regular expression, case-insensitive ).");
				return false; 

			// ~, bitwise not
			case INETNOT : //bitwise not
				elog(WARNING, "Unsupported Operator Type ID: 2634 \"inetnot\" (bitwise not ).");
				return false; 
			case MACADDR8_NOT : //bitwise not
				elog(WARNING, "Unsupported Operator Type ID: 3368 \"macaddr8_not\" (bitwise not ).");
				return false; 
			case BPCHARREGEXEQ : //matches regular expression, case-sensitive
				elog(WARNING, "Unsupported Operator Type ID: 1055 \"bpcharregexeq\" (matches regular expression, case-sensitive ).");
				return false; 
			case NAMEREGEXEQ : //matches regular expression, case-sensitive
				elog(WARNING, "Unsupported Operator Type ID: 639 \"nameregexeq\" (matches regular expression, case-sensitive ).");
				return false; 
			case BITNOT : //bitwise not
				elog(WARNING, "Unsupported Operator Type ID: 1794 \"bitnot\" (bitwise not ).");
				return false; 
			case TEXTREGEXEQ : //matches regular expression, case-sensitive
				elog(WARNING, "Unsupported Operator Type ID: 641 \"textregexeq\" (matches regular expression, case-sensitive ).");
				return false; 
			case INT2NOT : //bitwise not
				elog(WARNING, "Unsupported Operator Type ID: 1877 \"int2not\" (bitwise not ).");
				return false; 
			case INT8NOT : //bitwise not
				elog(WARNING, "Unsupported Operator Type ID: 1889 \"int8not\" (bitwise not ).");
				return false; 
			case MACADDR_NOT : //bitwise not
				elog(WARNING, "Unsupported Operator Type ID: 3147 \"macaddr_not\" (bitwise not ).");
				return false; 
			case INT4NOT : //bitwise not
				elog(WARNING, "Unsupported Operator Type ID: 1883 \"int4not\" (bitwise not ).");
				return false; 
		default: 
		     return false; 
		}
	}
}
