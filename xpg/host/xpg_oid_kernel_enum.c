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
#include "sqlcmds.h"

extern "C" {
#include "postgres.h"

	int xpg_get_operator_enum_new(Oid opId)
	{
		switch (opId)
		{

			// !!, NOT tsquery
			case TSQUERY_NOT : // ID: 3682, NOT tsquery
				return (int) opId ; 

			// @@, center of
			case CIRCLE_CENTER : // ID: 1420, center of
				return (int) opId ; 
			case TS_MATCH_TT : // ID: 3762, text search match
				return (int) opId ; 
			case TS_MATCH_TQ : // ID: 3763, text search match
				return (int) opId ; 
			case POLY_CENTER : // ID: 971, center of
				return (int) opId ; 
			case PATH_CENTER : // ID: 970, center of
				return (int) opId ; 
			case TS_MATCH_VQ : // ID: 3636, text search match
				return (int) opId ; 
			case TS_MATCH_QV : // ID: 3637, text search match
				return (int) opId ; 
			case BOX_CENTER : // ID: 513, center of
				return (int) opId ; 
			case LSEG_CENTER : // ID: 969, center of
				return (int) opId ; 

			// ?|, vertically aligned
			case POINT_VERT : // ID: 809, vertically aligned
				return (int) opId ; 
			case LINE_VERTICAL : // ID: 1615, vertical
				return (int) opId ; 
			case LSEG_VERTICAL : // ID: 1529, vertical
				return (int) opId ; 
			case JSONB_EXISTS_ANY : // ID: 3248, any key exists
				return (int) opId ; 

			// >=, greater than or equal
			case BITGE : // ID: 1789, greater than or equal
				return (int) opId ; 
			case MACADDR_GE : // ID: 1225, greater than or equal
				return (int) opId ; 
			case MACADDR8_GE : // ID: 3367, greater than or equal
				return (int) opId ; 
			case FLOAT4GE : // ID: 625, greater than or equal
				return (int) opId ; 
			case TIMESTAMPTZ_GE : // ID: 1325, greater than or equal
				return (int) opId ; 
			case TEXT_GE : // ID: 667, greater than or equal
				return (int) opId ; 
			case RELTIMEGE : // ID: 571, greater than or equal
				return (int) opId ; 
			case NETWORK_GE : // ID: 1206, greater than or equal
				return (int) opId ; 
			case NAMEGE : // ID: 663, greater than or equal
				return (int) opId ; 
			case FLOAT48GE : // ID: 1125, greater than or equal
				return (int) opId ; 
			case TSVECTOR_GE : // ID: 3631, greater than or equal
				return (int) opId ; 
			case INT48GE : // ID: 82, greater than or equal
				return (int) opId ; 
			case TSQUERY_GE : // ID: 3678, greater than or equal
				return (int) opId ; 
			case INT82GE : // ID: 1873, greater than or equal
				return (int) opId ; 
			case TIDGE : // ID: 2802, greater than or equal
				return (int) opId ; 
			case TIMESTAMPTZ_GE_TIMESTAMP : // ID: 2543, greater than or equal
				return (int) opId ; 
			case PATH_N_GE : // ID: 796, greater than or equal
				return (int) opId ; 
			case INT4GE : // ID: 525, greater than or equal
				return (int) opId ; 
			case ARRAY_GE : // ID: 1075, greater than or equal
				return (int) opId ; 
			case BYTEAGE : // ID: 1960, greater than or equal
				return (int) opId ; 
			case DATE_GE_TIMESTAMP : // ID: 2348, greater than or equal
				return GE;
			case INT2GE : // ID: 524, greater than or equal
				return (int) opId ; 
			case INT42GE : // ID: 543, greater than or equal
				return (int) opId ; 
			case TIME_GE : // ID: 1113, greater than or equal
				return (int) opId ; 
			case BOOLGE : // ID: 1695, greater than or equal
				return (int) opId ; 
			case TIMESTAMPTZ_GE_DATE : // ID: 2387, greater than or equal
				return (int) opId ; 
			case CIRCLE_GE : // ID: 1505, greater than or equal by area
				return (int) opId ; 
			case NUMERIC_GE : // ID: 1757, greater than or equal
				return GE;
			case INTERVAL_GE : // ID: 1335, greater than or equal
				return (int) opId ; 
			case JSONB_GE : // ID: 3245, greater than or equal
				return (int) opId ; 
			case TINTERVALGE : // ID: 816, greater than or equal
				return (int) opId ; 
			case BOX_GE : // ID: 501, greater than or equal by area
				return (int) opId ; 
			case CHARGE : // ID: 634, greater than or equal
				return (int) opId ; 
			case INT24GE : // ID: 542, greater than or equal
				return (int) opId ; 
			case OIDGE : // ID: 612, greater than or equal
				return (int) opId ; 
			case FLOAT8GE : // ID: 675, greater than or equal
				return (int) opId ; 
			case DATE_GE : // ID: 1098, greater than or equal
				return GE;
			case ABSTIMEGE : // ID: 565, greater than or equal
				return (int) opId ; 
			case FLOAT84GE : // ID: 1135, greater than or equal
				return (int) opId ; 
			case LSEG_GE : // ID: 1590, greater than or equal by length
				return (int) opId ; 
			case INT28GE : // ID: 1867, greater than or equal
				return (int) opId ; 
			case ENUM_GE : // ID: 3521, greater than or equal
				return (int) opId ; 
			case UUID_GE : // ID: 2977, greater than or equal
				return (int) opId ; 
			case RANGE_GE : // ID: 3886, greater than or equal
				return (int) opId ; 
			case TIMETZ_GE : // ID: 1555, greater than or equal
				return (int) opId ; 
			case CASH_GE : // ID: 905, greater than or equal
				return (int) opId ; 
			case BPCHARGE : // ID: 1061, greater than or equal
				return (int) opId ; 
			case PG_LSN_GE : // ID: 3227, greater than or equal
				return (int) opId ; 
			case RECORD_GE : // ID: 2993, greater than or equal
				return (int) opId ; 
			case VARBITGE : // ID: 1809, greater than or equal
				return (int) opId ; 
			case TIMESTAMP_GE_TIMESTAMPTZ : // ID: 2537, greater than or equal
				return (int) opId ; 
			case INT8GE : // ID: 415, greater than or equal
				return (int) opId ; 
			case DATE_GE_TIMESTAMPTZ : // ID: 2361, greater than or equal
				return (int) opId ; 
			case TIMESTAMP_GE_DATE : // ID: 2374, greater than or equal
				return (int) opId ; 
			case OIDVECTORGE : // ID: 648, greater than or equal
				return (int) opId ; 
			case TIMESTAMP_GE : // ID: 2065, greater than or equal
				return (int) opId ; 
			case INT84GE : // ID: 430, greater than or equal
				return (int) opId ; 

			// >>, bitwise shift right
			case INT4SHR : // ID: 1885, bitwise shift right
				return (int) opId ; 
			case INT8SHR : // ID: 1891, bitwise shift right
				return (int) opId ; 
			case POINT_RIGHT : // ID: 508, is right of
				return (int) opId ; 
			case POLY_RIGHT : // ID: 488, is right of
				return (int) opId ; 
			case NETWORK_SUP : // ID: 933, is supernet
				return (int) opId ; 
			case BOX_RIGHT : // ID: 496, is right of
				return (int) opId ; 
			case CIRCLE_RIGHT : // ID: 1509, is right of
				return (int) opId ; 
			case BITSHIFTRIGHT : // ID: 1796, bitwise shift right
				return (int) opId ; 
			case RANGE_AFTER : // ID: 3894, is right of
				return (int) opId ; 
			case INT2SHR : // ID: 1879, bitwise shift right
				return (int) opId ; 

			// #<=, less than or equal by length
			case TINTERVALLENLE : // ID: 579, less than or equal by length
				return (int) opId ; 

			// <>, not equal
			case DATE_NE : // ID: 1094, not equal
				return (int) opId ; 
			case FLOAT4NE : // ID: 621, not equal
				return (int) opId ; 
			case TIMESTAMPTZ_NE : // ID: 1321, not equal
				return (int) opId ; 
			case MACADDR_NE : // ID: 1221, not equal
				return (int) opId ; 
			case FLOAT48NE : // ID: 1121, not equal
				return (int) opId ; 
			case OIDNE : // ID: 608, not equal
				return (int) opId ; 
			case TSVECTOR_NE : // ID: 3630, not equal
				return (int) opId ; 
			case NETWORK_NE : // ID: 1202, not equal
				return (int) opId ; 
			case PG_LSN_NE : // ID: 3223, not equal
				return (int) opId ; 
			case ARRAY_NE : // ID: 1071, not equal
				return (int) opId ; 
			case TSQUERY_NE : // ID: 3677, not equal
				return (int) opId ; 
			case BOOLNE : // ID: 85, not equal
				return (int) opId ; 
			case RECORD_NE : // ID: 2989, not equal
				return (int) opId ; 
			case TIMESTAMPTZ_NE_TIMESTAMP : // ID: 2545, not equal
				return (int) opId ; 
			case BPCHARNE : // ID: 1057, not equal
				return (int) opId ; 
			case LSEG_NE : // ID: 1586, not equal
				return (int) opId ; 
			case MACADDR8_NE : // ID: 3363, not equal
				return (int) opId ; 
			case CIRCLE_NE : // ID: 1501, not equal by area
				return (int) opId ; 
			case INTERVAL_NE : // ID: 1331, not equal
				return (int) opId ; 
			case TIDNE : // ID: 402, not equal
				return (int) opId ; 
			case DATE_NE_TIMESTAMPTZ : // ID: 2363, not equal
				return (int) opId ; 
			case TIMESTAMPTZ_NE_DATE : // ID: 2389, not equal
				return (int) opId ; 
			case TINTERVALNE : // ID: 812, not equal
				return (int) opId ; 
			case BITNE : // ID: 1785, not equal
				return (int) opId ; 
			case OIDVECTORNE : // ID: 644, not equal
				return (int) opId ; 
			case CHARNE : // ID: 630, not equal
				return (int) opId ; 
			case ABSTIMENE : // ID: 561, not equal
				return (int) opId ; 
			case FLOAT84NE : // ID: 1131, not equal
				return (int) opId ; 
			case POINT_NE : // ID: 713, not equal
				return (int) opId ; 
			case FLOAT8NE : // ID: 671, not equal
				return (int) opId ; 
			case RELTIMENE : // ID: 567, not equal
				return (int) opId ; 
			case INT28NE : // ID: 1863, not equal
				return (int) opId ; 
			case BYTEANE : // ID: 1956, not equal
				return (int) opId ; 
			case ENUM_NE : // ID: 3517, not equal
				return (int) opId ; 
			case INT82NE : // ID: 1869, not equal
				return (int) opId ; 
			case UUID_NE : // ID: 2973, not equal
				return (int) opId ; 
			case XIDNEQ : // ID: 3315, not equal
				return (int) opId ; 
			case TIMESTAMP_NE_DATE : // ID: 2376, not equal
				return (int) opId ; 
			case XIDNEQINT4 : // ID: 3316, not equal
				return (int) opId ; 
			case TIMETZ_NE : // ID: 1551, not equal
				return (int) opId ; 
			case VARBITNE : // ID: 1805, not equal
				return (int) opId ; 
			case CASH_NE : // ID: 901, not equal
				return (int) opId ; 
			case RANGE_NE : // ID: 3883, not equal
				return (int) opId ; 
			case NUMERIC_NE : // ID: 1753, not equal
				return (int) opId ; 
			case JSONB_NE : // ID: 3241, not equal
				return (int) opId ; 
			case INT48NE : // ID: 36, not equal
				return (int) opId ; 
			case TIMESTAMP_NE_TIMESTAMPTZ : // ID: 2539, not equal
				return (int) opId ; 
			case INT2NE : // ID: 519, not equal
				return (int) opId ; 
			case INT4NE : // ID: 518, not equal
				return (int) opId ; 
			case NAMENE : // ID: 643, not equal
				return (int) opId ; 
			case INT84NE : // ID: 417, not equal
				return (int) opId ; 
			case INT8NE : // ID: 411, not equal
				return (int) opId ; 
			case TEXTNE : // ID: 531, not equal
				return (int) opId ; 
			case TIMESTAMP_NE : // ID: 2061, not equal
				return (int) opId ; 
			case DATE_NE_TIMESTAMP : // ID: 2350, not equal
				return (int) opId ; 
			case INT24NE : // ID: 538, not equal
				return (int) opId ; 
			case INT42NE : // ID: 539, not equal
				return (int) opId ; 
			case TIME_NE : // ID: 1109, not equal
				return (int) opId ; 

			// ||, concatenate
			case TEXTANYCAT : // ID: 2779, concatenate
				return (int) opId ; 
			case ANYTEXTCAT : // ID: 2780, concatenate
				return (int) opId ; 
			case TSQUERY_OR : // ID: 3681, OR-concatenate
				return (int) opId ; 
			case BITCAT : // ID: 1797, concatenate
				return (int) opId ; 
			case TSVECTOR_CONCAT : // ID: 3633, concatenate
				return (int) opId ; 
			case ARRAY_PREPEND : // ID: 374, prepend element onto front of array
				return (int) opId ; 
			case TEXTCAT : // ID: 654, concatenate
				return (int) opId ; 
			case BYTEACAT : // ID: 2018, concatenate
				return (int) opId ; 
			case JSONB_CONCAT : // ID: 3284, concatenate
				return (int) opId ; 
			case ARRAY_CAT : // ID: 375, concatenate
				return (int) opId ; 
			case ARRAY_APPEND : // ID: 349, append element onto end of array
				return (int) opId ; 

			// <=, less than or equal
			case BITLE : // ID: 1788, less than or equal
				return (int) opId ; 
			case TIMESTAMPTZ_LE : // ID: 1323, less than or equal
				return (int) opId ; 
			case MACADDR_LE : // ID: 1223, less than or equal
				return (int) opId ; 
			case FLOAT4LE : // ID: 624, less than or equal
				return (int) opId ; 
			case NETWORK_LE : // ID: 1204, less than or equal
				return (int) opId ; 
			case TEXT_LE : // ID: 665, less than or equal
				return (int) opId ; 
			case RELTIMELE : // ID: 570, less than or equal
				return (int) opId ; 
			case FLOAT48LE : // ID: 1124, less than or equal
				return (int) opId ; 
			case DATE_LE : // ID: 1096, less than or equal
				return LE;
			case NAMELE : // ID: 661, less than or equal
				return (int) opId ; 
			case INT82LE : // ID: 1872, less than or equal
				return (int) opId ; 
			case INT48LE : // ID: 80, less than or equal
				return (int) opId ; 
			case TIDLE : // ID: 2801, less than or equal
				return (int) opId ; 
			case TSQUERY_LE : // ID: 3675, less than or equal
				return (int) opId ; 
			case PATH_N_LE : // ID: 795, less than or equal
				return (int) opId ; 
			case DATE_LE_TIMESTAMP : // ID: 2346, less than or equal
				return LE;
			case BPCHARLE : // ID: 1059, less than or equal
				return (int) opId ; 
			case ARRAY_LE : // ID: 1074, less than or equal
				return (int) opId ; 
			case INT24LE : // ID: 540, less than or equal
				return (int) opId ; 
			case TIME_LE : // ID: 1111, less than or equal
				return (int) opId ; 
			case BOOLLE : // ID: 1694, less than or equal
				return (int) opId ; 
			case INT42LE : // ID: 541, less than or equal
				return (int) opId ; 
			case TIMESTAMPTZ_LE_DATE : // ID: 2385, less than or equal
				return (int) opId ; 
			case INT2LE : // ID: 522, less than or equal
				return (int) opId ; 
			case INT4LE : // ID: 523, less than or equal
				return (int) opId ; 
			case INT84LE : // ID: 420, less than or equal
				return (int) opId ; 
			case NUMERIC_LE : // ID: 1755, less than or equal
				return LE;
			case BOX_LE : // ID: 505, less than or equal by area
				return (int) opId ; 
			case TINTERVALLE : // ID: 815, less than or equal
				return (int) opId ; 
			case TSVECTOR_LE : // ID: 3628, less than or equal
				return (int) opId ; 
			case CHARLE : // ID: 632, less than or equal
				return (int) opId ; 
			case CIRCLE_LE : // ID: 1504, less than or equal by area
				return (int) opId ; 
			case OIDLE : // ID: 611, less than or equal
				return (int) opId ; 
			case ABSTIMELE : // ID: 564, less than or equal
				return (int) opId ; 
			case LSEG_LE : // ID: 1588, less than or equal by length
				return (int) opId ; 
			case INTERVAL_LE : // ID: 1333, less than or equal
				return (int) opId ; 
			case FLOAT8LE : // ID: 673, less than or equal
				return (int) opId ; 
			case JSONB_LE : // ID: 3244, less than or equal
				return (int) opId ; 
			case ENUM_LE : // ID: 3520, less than or equal
				return (int) opId ; 
			case INT28LE : // ID: 1866, less than or equal
				return (int) opId ; 
			case FLOAT84LE : // ID: 1134, less than or equal
				return (int) opId ; 
			case UUID_LE : // ID: 2976, less than or equal
				return (int) opId ; 
			case BYTEALE : // ID: 1958, less than or equal
				return (int) opId ; 
			case RANGE_LE : // ID: 3885, less than or equal
				return (int) opId ; 
			case TIMETZ_LE : // ID: 1553, less than or equal
				return (int) opId ; 
			case CASH_LE : // ID: 904, less than or equal
				return (int) opId ; 
			case RECORD_LE : // ID: 2992, less than or equal
				return (int) opId ; 
			case PG_LSN_LE : // ID: 3226, less than or equal
				return (int) opId ; 
			case VARBITLE : // ID: 1808, less than or equal
				return (int) opId ; 
			case INT8LE : // ID: 414, less than or equal
				return (int) opId ; 
			case DATE_LE_TIMESTAMPTZ : // ID: 2359, less than or equal
				return (int) opId ; 
			case OIDVECTORLE : // ID: 647, less than or equal
				return (int) opId ; 
			case MACADDR8_LE : // ID: 3365, less than or equal
				return (int) opId ; 
			case TIMESTAMPTZ_LE_TIMESTAMP : // ID: 2541, less than or equal
				return (int) opId ; 
			case TIMESTAMP_LE : // ID: 2063, less than or equal
				return (int) opId ; 
			case TIMESTAMP_LE_DATE : // ID: 2372, less than or equal
				return (int) opId ; 
			case TIMESTAMP_LE_TIMESTAMPTZ : // ID: 2535, less than or equal
				return (int) opId ; 

			// *>, greater than
			case RECORD_IMAGE_GT : // ID: 3191, greater than
				return (int) opId ; 

			// ?-|, perpendicular
			case LSEG_PERP : // ID: 1527, perpendicular
				return (int) opId ; 
			case LINE_PERP : // ID: 1613, perpendicular
				return (int) opId ; 

			// -|-, is adjacent to
			case RANGE_ADJACENT : // ID: 3897, is adjacent to
				return (int) opId ; 

			// ^, exponentiation
			case DPOW : // ID: 965, exponentiation
				return (int) opId ; 
			case NUMERIC_POWER : // ID: 1038, exponentiation
				return (int) opId ; 

			// #>, get value from json with path elements
			case JSON_EXTRACT_PATH : // ID: 3966, get value from json with path elements
				return (int) opId ; 
			case TINTERVALLENGT : // ID: 578, greater than by length
				return (int) opId ; 
			case JSONB_EXTRACT_PATH : // ID: 3213, get value from jsonb with path elements
				return (int) opId ; 

			// #=, equal by length
			case TINTERVALLENEQ : // ID: 575, equal by length
				return (int) opId ; 

			// #<, less than by length
			case TINTERVALLENLT : // ID: 577, less than by length
				return (int) opId ; 

			// #-, delete path
			case JSONB_DELETE_PATH : // ID: 3287, delete path
				return (int) opId ; 

			// !, factorial
			case NUMERIC_FAC : // ID: 388, factorial
				return (int) opId ; 

			// #, number of points
			case PATH_NPOINTS : // ID: 797, number of points
				return (int) opId ; 
			case LSEG_INTERPT : // ID: 1536, intersection point
				return (int) opId ; 
			case POLY_NPOINTS : // ID: 1521, number of points
				return (int) opId ; 
			case BITXOR : // ID: 1793, bitwise exclusive or
				return (int) opId ; 
			case BOX_INTERSECT : // ID: 803, box intersection
				return (int) opId ; 
			case LINE_INTERPT : // ID: 1617, intersection point
				return (int) opId ; 
			case INT8XOR : // ID: 1888, bitwise exclusive or
				return (int) opId ; 
			case INT4XOR : // ID: 1882, bitwise exclusive or
				return (int) opId ; 
			case INT2XOR : // ID: 1876, bitwise exclusive or
				return (int) opId ; 

			// %, modulus
			case NUMERIC_MOD : // ID: 1762, modulus
				return (int) opId ; 
			case INT4MOD : // ID: 530, modulus
				return (int) opId ; 
			case INT2MOD : // ID: 529, modulus
				return (int) opId ; 
			case INT8MOD : // ID: 439, modulus
				return (int) opId ; 

			// ~~, matches LIKE expression
			case BYTEALIKE : // ID: 2016, matches LIKE expression
				return (int) opId ; 
			case NAMELIKE : // ID: 1207, matches LIKE expression
				return (int) opId ; 
			case BPCHARLIKE : // ID: 1211, matches LIKE expression
				return (int) opId ; 
			case TEXTLIKE : // ID: 1209, matches LIKE expression
				return (int) opId ; 

			// <<=, is subnet or equal
			case NETWORK_SUBEQ : // ID: 932, is subnet or equal
				return (int) opId ; 

			// &, bitwise and
			case INETAND : // ID: 2635, bitwise and
				return (int) opId ; 
			case INT2AND : // ID: 1874, bitwise and
				return (int) opId ; 
			case MACADDR8_AND : // ID: 3369, bitwise and
				return (int) opId ; 
			case MACADDR_AND : // ID: 3148, bitwise and
				return (int) opId ; 
			case INT8AND : // ID: 1886, bitwise and
				return (int) opId ; 
			case BITAND : // ID: 1791, bitwise and
				return (int) opId ; 
			case INT4AND : // ID: 1880, bitwise and
				return (int) opId ; 

			// ?#, intersect
			case INTER_SL : // ID: 1537, intersect
				return (int) opId ; 
			case LSEG_INTERSECT : // ID: 1525, intersect
				return (int) opId ; 
			case LINE_INTERSECT : // ID: 1611, intersect
				return (int) opId ; 
			case PATH_INTER : // ID: 798, intersect
				return (int) opId ; 
			case INTER_LB : // ID: 1539, intersect
				return (int) opId ; 
			case INTER_SB : // ID: 1538, intersect
				return (int) opId ; 

			// +, add
			case FLOAT8PL : // ID: 591, add
				return (int) opId ; 
			case TIMESTAMPTZ_PL_INTERVAL : // ID: 1327, add
				return (int) opId ; 
			case INETPL : // ID: 2637, add
				return (int) opId ; 
			case INT4PL : // ID: 551, add
				return (int) opId ; 
			case INT2PL : // ID: 550, add
				return (int) opId ; 
			case INT42PL : // ID: 553, add
				return (int) opId ; 
			case INT24PL : // ID: 552, add
				return (int) opId ; 
			case FLOAT8UP : // ID: 1920, unary plus
				return (int) opId ; 
			case NUMERIC_UPLUS : // ID: 1921, unary plus
				return (int) opId ; 
			case INTERVAL_PL_TIMETZ : // ID: 2552, add
				return (int) opId ; 
			case BOX_ADD : // ID: 804, add point to box (translate)
				return (int) opId ; 
			case INT8PL_INET : // ID: 2638, add
				return (int) opId ; 
			case INT48PL : // ID: 692, add
				return (int) opId ; 
			case POINT_ADD : // ID: 731, add points (translate)
				return (int) opId ; 
			case DATE_PL_INTERVAL : // ID: 1076, add
				return (int) opId ; 
			case FLOAT48PL : // ID: 1116, add
				return (int) opId ; 
			case INT82PL : // ID: 818, add
				return (int) opId ; 
			case RANGE_UNION : // ID: 3898, range union
				return (int) opId ; 
			case PATH_ADD : // ID: 735, concatenate
				return (int) opId ; 
			case PATH_ADD_PT : // ID: 736, add (translate path)
				return (int) opId ; 
			case INT84PL : // ID: 688, add
				return (int) opId ; 
			case FLOAT4PL : // ID: 586, add
				return (int) opId ; 
			case DATE_PLI : // ID: 1100, add
				return (int) opId ; 
			case TIMEPL : // ID: 581, add
				return (int) opId ; 
			case INTERVAL_PL : // ID: 1337, add
				return (int) opId ; 
			case NUMERIC_ADD : // ID: 1758, add
				return (int) opId ; 
			case ACLINSERT : // ID: 966, add/update ACL item
				return (int) opId ; 
			case INTEGER_PL_DATE : // ID: 2555, add
				return (int) opId ; 
			case FLOAT4UP : // ID: 1919, unary plus
				return (int) opId ; 
			case INT4UP : // ID: 1918, unary plus
				return (int) opId ; 
			case FLOAT84PL : // ID: 1126, add
				return (int) opId ; 
			case INTERVAL_PL_TIMESTAMPTZ : // ID: 2554, add
				return (int) opId ; 
			case INT2UP : // ID: 1917, unary plus
				return (int) opId ; 
			case INT8UP : // ID: 1916, unary plus
				return (int) opId ; 
			case INTERVAL_PL_DATE : // ID: 2551, add
				return (int) opId ; 
			case INTERVAL_PL_TIME : // ID: 1849, add
				return (int) opId ; 
			case INTERVAL_PL_TIMESTAMP : // ID: 2553, add
				return (int) opId ; 
			case CASH_PL : // ID: 906, add
				return (int) opId ; 
			case TIMETZ_PL_INTERVAL : // ID: 1802, add
				return (int) opId ; 
			case TIME_PL_INTERVAL : // ID: 1800, add
				return (int) opId ; 
			case TIMETZDATE_PL : // ID: 1366, convert time with time zone and date to timestamp with time zone
				return (int) opId ; 
			case TIMEDATE_PL : // ID: 1363, convert time and date to timestamp
				return (int) opId ; 
			case DATETIME_PL : // ID: 1360, convert date and time to timestamp
				return (int) opId ; 
			case DATETIMETZ_PL : // ID: 1361, convert date and time with time zone to timestamp with time zone
				return (int) opId ; 
			case INT8PL : // ID: 684, add
				return (int) opId ; 
			case CIRCLE_ADD_PT : // ID: 1516, add
				return (int) opId ; 
			case TIMESTAMP_PL_INTERVAL : // ID: 2066, add
				return (int) opId ; 
			case INT28PL : // ID: 822, add
				return (int) opId ; 

			// *, multiply
			case FLOAT84MUL : // ID: 1129, multiply
				return (int) opId ; 
			case INT4MUL : // ID: 514, multiply
				return (int) opId ; 
			case RANGE_INTERSECT : // ID: 3900, range intersection
				return (int) opId ; 
			case FLOAT8MUL : // ID: 594, multiply
				return (int) opId ; 
			case INT28MUL : // ID: 824, multiply
				return (int) opId ; 
			case FLT4_MUL_CASH : // ID: 845, multiply
				return (int) opId ; 
			case FLOAT4MUL : // ID: 589, multiply
				return (int) opId ; 
			case INT8_MUL_CASH : // ID: 3349, multiply
				return (int) opId ; 
			case PATH_MUL_PT : // ID: 738, multiply (rotate/scale path)
				return (int) opId ; 
			case NUMERIC_MUL : // ID: 1760, multiply
				return (int) opId ; 
			case INT84MUL : // ID: 690, multiply
				return (int) opId ; 
			case INT8MUL : // ID: 686, multiply
				return (int) opId ; 
			case CASH_MUL_FLT4 : // ID: 843, multiply
				return (int) opId ; 
			case INT48MUL : // ID: 694, multiply
				return (int) opId ; 
			case CIRCLE_MUL_PT : // ID: 1518, multiply
				return (int) opId ; 
			case INTERVAL_MUL : // ID: 1583, multiply
				return (int) opId ; 
			case CASH_MUL_FLT8 : // ID: 908, multiply
				return (int) opId ; 
			case CASH_MUL_INT8 : // ID: 3346, multiply
				return (int) opId ; 
			case INT2MUL : // ID: 526, multiply
				return (int) opId ; 
			case MUL_D_INTERVAL : // ID: 1584, multiply
				return (int) opId ; 
			case INT82MUL : // ID: 820, multiply
				return (int) opId ; 
			case POINT_MUL : // ID: 733, multiply points (scale/rotate)
				return (int) opId ; 
			case INT2_MUL_CASH : // ID: 918, multiply
				return (int) opId ; 
			case INT24MUL : // ID: 544, multiply
				return (int) opId ; 
			case INT42MUL : // ID: 545, multiply
				return (int) opId ; 
			case FLOAT48MUL : // ID: 1119, multiply
				return (int) opId ; 
			case CASH_MUL_INT2 : // ID: 914, multiply
				return (int) opId ; 
			case INT4_MUL_CASH : // ID: 917, multiply
				return (int) opId ; 
			case FLT8_MUL_CASH : // ID: 916, multiply
				return (int) opId ; 
			case BOX_MUL : // ID: 806, multiply box by point (scale)
				return (int) opId ; 
			case CASH_MUL_INT4 : // ID: 912, multiply
				return (int) opId ; 

			// -, subtract
			case FLOAT8MI : // ID: 592, subtract
				return (int) opId ; 
			case INT2UM : // ID: 559, negate
				return (int) opId ; 
			case INT4UM : // ID: 558, negate
				return (int) opId ; 
			case INT4MI : // ID: 555, subtract
				return (int) opId ; 
			case INT2MI : // ID: 554, subtract
				return (int) opId ; 
			case INT42MI : // ID: 557, subtract
				return (int) opId ; 
			case INT24MI : // ID: 556, subtract
				return (int) opId ; 
			case FLOAT84MI : // ID: 1127, subtract
				return (int) opId ; 
			case INETMI_INT8 : // ID: 2639, subtract
				return (int) opId ; 
			case INTERVAL_MI : // ID: 1338, subtract
				return (int) opId ; 
			case INT48MI : // ID: 693, subtract
				return (int) opId ; 
			case DATE_MI_INTERVAL : // ID: 1077, subtract
				return (int) opId ; 
			case INT82MI : // ID: 819, subtract
				return (int) opId ; 
			case POINT_SUB : // ID: 732, subtract points (translate)
				return (int) opId ; 
			case TIME_MI_TIME : // ID: 1399, subtract
				return (int) opId ; 
			case TIMESTAMPTZ_MI : // ID: 1328, subtract
				return (int) opId ; 
			case NUMERIC_UMINUS : // ID: 1751, negate
				return (int) opId ; 
			case FLOAT4MI : // ID: 587, subtract
				return (int) opId ; 
			case FLOAT4UM : // ID: 584, negate
				return (int) opId ; 
			case FLOAT8UM : // ID: 585, negate
				return (int) opId ; 
			case TIMEMI : // ID: 582, subtract
				return (int) opId ; 
			case FLOAT48MI : // ID: 1117, subtract
				return (int) opId ; 
			case INETMI : // ID: 2640, subtract
				return (int) opId ; 
			case RANGE_MINUS : // ID: 3899, range difference
				return (int) opId ; 
			case INTERVAL_UM : // ID: 1336, negate
				return (int) opId ; 
			case DATE_MI : // ID: 1099, subtract
				return (int) opId ; 
			case INT8UM : // ID: 484, negate
				return (int) opId ; 
			case CASH_MI : // ID: 907, subtract
				return (int) opId ; 
			case ACLREMOVE : // ID: 967, remove ACL item
				return (int) opId ; 
			case TIMETZ_MI_INTERVAL : // ID: 1803, subtract
				return (int) opId ; 
			case PG_LSN_MI : // ID: 3228, minus
				return (int) opId ; 
			case TIME_MI_INTERVAL : // ID: 1801, subtract
				return (int) opId ; 
			case NUMERIC_SUB : // ID: 1759, subtract
				return (int) opId ; 
			case TIMESTAMPTZ_MI_INTERVAL : // ID: 1329, subtract
				return (int) opId ; 
			case PATH_SUB_PT : // ID: 737, subtract (translate path)
				return (int) opId ; 
			case TIMESTAMP_MI_INTERVAL : // ID: 2068, subtract
				return (int) opId ; 
			case INT84MI : // ID: 689, subtract
				return (int) opId ; 
			case BOX_SUB : // ID: 805, subtract point from box (translate)
				return (int) opId ; 
			case DATE_MII : // ID: 1101, subtract
				return (int) opId ; 
			case INT8MI : // ID: 685, subtract
				return (int) opId ; 
			case CIRCLE_SUB_PT : // ID: 1517, subtract
				return (int) opId ; 
			case INT28MI : // ID: 823, subtract
				return (int) opId ; 
			case TIMESTAMP_MI : // ID: 2067, subtract
				return (int) opId ; 

			// /, divide
			case FLOAT84DIV : // ID: 1128, divide
				return (int) opId ; 
			case FLOAT8DIV : // ID: 593, divide
				return (int) opId ; 
			case CIRCLE_DIV_PT : // ID: 1519, divide
				return (int) opId ; 
			case CASH_DIV_INT2 : // ID: 915, divide
				return (int) opId ; 
			case CASH_DIV_CASH : // ID: 3825, divide
				return (int) opId ; 
			case FLOAT4DIV : // ID: 588, divide
				return (int) opId ; 
			case INT28DIV : // ID: 825, divide
				return (int) opId ; 
			case CASH_DIV_FLT4 : // ID: 844, divide
				return (int) opId ; 
			case INT84DIV : // ID: 691, divide
				return (int) opId ; 
			case INT48DIV : // ID: 695, divide
				return (int) opId ; 
			case INT82DIV : // ID: 821, divide
				return (int) opId ; 
			case CASH_DIV_FLT8 : // ID: 909, divide
				return (int) opId ; 
			case CASH_DIV_INT8 : // ID: 3347, divide
				return (int) opId ; 
			case INTERVAL_DIV : // ID: 1585, divide
				return (int) opId ; 
			case INT2DIV : // ID: 527, divide
				return (int) opId ; 
			case INT24DIV : // ID: 546, divide
				return (int) opId ; 
			case INT42DIV : // ID: 547, divide
				return (int) opId ; 
			case PATH_DIV_PT : // ID: 739, divide (rotate/scale path)
				return (int) opId ; 
			case FLOAT48DIV : // ID: 1118, divide
				return (int) opId ; 
			case INT8DIV : // ID: 687, divide
				return (int) opId ; 
			case NUMERIC_DIV : // ID: 1761, divide
				return (int) opId ; 
			case INT4DIV : // ID: 528, divide
				return (int) opId ; 
			case BOX_DIV : // ID: 807, divide box by point (scale)
				return (int) opId ; 
			case CASH_DIV_INT4 : // ID: 913, divide
				return (int) opId ; 
			case POINT_DIV : // ID: 734, divide points (scale/rotate)
				return (int) opId ; 

			// ~<~, less than
			case TEXT_PATTERN_LT : // ID: 2314, less than
				return (int) opId ; 
			case BPCHAR_PATTERN_LT : // ID: 2326, less than
				return (int) opId ; 

			// ##, closest point to A on B
			case CLOSE_LB : // ID: 1568, closest point to A on B
				return (int) opId ; 
			case CLOSE_LSEG : // ID: 1578, closest point to A on B
				return (int) opId ; 
			case CLOSE_PL : // ID: 1557, closest point to A on B
				return (int) opId ; 
			case CLOSE_PB : // ID: 1559, closest point to A on B
				return (int) opId ; 
			case CLOSE_PS : // ID: 1558, closest point to A on B
				return (int) opId ; 
			case CLOSE_LS : // ID: 1577, closest point to A on B
				return (int) opId ; 
			case CLOSE_SL : // ID: 1566, closest point to A on B
				return (int) opId ; 
			case CLOSE_SB : // ID: 1567, closest point to A on B
				return (int) opId ; 

			// >>=, is supernet or equal
			case NETWORK_SUPEQ : // ID: 934, is supernet or equal
				return (int) opId ; 

			// ~>=~, greater than or equal
			case TEXT_PATTERN_GE : // ID: 2317, greater than or equal
				return (int) opId ; 
			case BPCHAR_PATTERN_GE : // ID: 2329, greater than or equal
				return (int) opId ; 

			// ||/, cube root
			case DCBRT : // ID: 597, cube root
				return (int) opId ; 

			// ?&, all keys exist
			case JSONB_EXISTS_ALL : // ID: 3249, all keys exist
				return (int) opId ; 

			// <->, distance between
			case DIST_LB : // ID: 1524, distance between
				return (int) opId ; 
			case DIST_PPATH : // ID: 618, distance between
				return (int) opId ; 
			case LINE_DISTANCE : // ID: 708, distance between
				return (int) opId ; 
			case LSEG_DISTANCE : // ID: 709, distance between
				return (int) opId ; 
			case DIST_PL : // ID: 613, distance between
				return (int) opId ; 
			case BOX_DISTANCE : // ID: 706, distance between
				return (int) opId ; 
			case PATH_DISTANCE : // ID: 707, distance between
				return (int) opId ; 
			case DIST_SL : // ID: 616, distance between
				return (int) opId ; 
			case DIST_SB : // ID: 617, distance between
				return (int) opId ; 
			case DIST_PS : // ID: 614, distance between
				return (int) opId ; 
			case DIST_PB : // ID: 615, distance between
				return (int) opId ; 
			case DIST_PPOLY : // ID: 3276, distance between
				return (int) opId ; 
			case DIST_POLYP : // ID: 3289, distance between
				return (int) opId ; 
			case CIRCLE_DISTANCE : // ID: 1520, distance between
				return (int) opId ; 
			case DIST_PC : // ID: 1522, distance between
				return (int) opId ; 
			case DIST_CPOLY : // ID: 1523, distance between
				return (int) opId ; 
			case POLY_DISTANCE : // ID: 712, distance between
				return (int) opId ; 
			case DIST_CPOINT : // ID: 3291, distance between
				return (int) opId ; 
			case POINT_DISTANCE : // ID: 517, distance between
				return (int) opId ; 

			// ->>, get jsonb array element as text
			case JSONB_ARRAY_ELEMENT_TEXT : // ID: 3481, get jsonb array element as text
				return (int) opId ; 
			case JSONB_OBJECT_FIELD_TEXT : // ID: 3477, get jsonb object field as text
				return (int) opId ; 
			case JSON_ARRAY_ELEMENT_TEXT : // ID: 3965, get json array element as text
				return (int) opId ; 
			case JSON_OBJECT_FIELD_TEXT : // ID: 3963, get json object field as text
				return (int) opId ; 

			// <#>, convert to tinterval
			case MKTINTERVAL : // ID: 606, convert to tinterval
				return (int) opId ; 

			// #>=, greater than or equal by length
			case TINTERVALLENGE : // ID: 580, greater than or equal by length
				return (int) opId ; 

			// =, equal
			case OIDEQ : // ID: 607, equal
				return (int) opId ; 
			case FLOAT4EQ : // ID: 620, equal
				return (int) opId ; 
			case MACADDR_EQ : // ID: 1220, equal
				return (int) opId ; 
			case FLOAT48EQ : // ID: 1120, equal
				return (int) opId ; 
			case XIDEQINT4 : // ID: 353, equal
				return (int) opId ; 
			case NETWORK_EQ : // ID: 1201, equal
				return (int) opId ; 
			case TIMESTAMPTZ_EQ : // ID: 1320, equal
				return (int) opId ; 
			case ENUM_EQ : // ID: 3516, equal
				return (int) opId ; 
			case UUID_EQ : // ID: 2972, equal
				return (int) opId ; 
			case ARRAY_EQ : // ID: 1070, equal
				return (int) opId ; 
			case PG_LSN_EQ : // ID: 3222, equal
				return (int) opId ; 
			case TSQUERY_EQ : // ID: 3676, equal
				return (int) opId ; 
			case TIMESTAMPTZ_EQ_TIMESTAMP : // ID: 2542, equal
				return (int) opId ; 
			case DATE_EQ_TIMESTAMP : // ID: 2347, equal
				return (int) opId ; 
			case ACLITEMEQ : // ID: 974, equal
				return (int) opId ; 
			case BPCHAREQ : // ID: 1054, equal
				return EQ;
			case PATH_N_EQ : // ID: 792, equal
				return (int) opId ; 
			case LINE_EQ : // ID: 1616, equal
				return (int) opId ; 
			case INT84EQ : // ID: 416, equal
				return (int) opId ; 
			case MACADDR8_EQ : // ID: 3362, equal
				return (int) opId ; 
			case CIRCLE_EQ : // ID: 1500, equal by area
				return (int) opId ; 
			case TIMESTAMPTZ_EQ_DATE : // ID: 2386, equal
				return (int) opId ; 
			case TINTERVALEQ : // ID: 811, equal
				return (int) opId ; 
			case NUMERIC_EQ : // ID: 1752, equal
				return (int) opId ; 
			case XIDEQ : // ID: 352, equal
				return (int) opId ; 
			case TSVECTOR_EQ : // ID: 3629, equal
				return (int) opId ; 
			case DATE_EQ : // ID: 1093, equal
				return (int) opId ; 
			case ABSTIMEEQ : // ID: 560, equal
				return (int) opId ; 
			case BITEQ : // ID: 1784, equal
				return (int) opId ; 
			case FLOAT84EQ : // ID: 1130, equal
				return (int) opId ; 
			case FLOAT8EQ : // ID: 670, equal
				return (int) opId ; 
			case INTERVAL_EQ : // ID: 1330, equal
				return (int) opId ; 
			case RELTIMEEQ : // ID: 566, equal
				return (int) opId ; 
			case BYTEAEQ : // ID: 1955, equal
				return (int) opId ; 
			case TEXTEQ : // ID: 98, equal
				return (int) opId ; 
			case INT28EQ : // ID: 1862, equal
				return (int) opId ; 
			case CIDEQ : // ID: 385, equal
				return (int) opId ; 
			case TIMESTAMP_EQ_DATE : // ID: 2373, equal
				return (int) opId ; 
			case TIDEQ : // ID: 387, equal
				return (int) opId ; 
			case BOOLEQ : // ID: 91, equal
				return (int) opId ; 
			case INT82EQ : // ID: 1868, equal
				return (int) opId ; 
			case NAMEEQ : // ID: 93, equal
				return (int) opId ; 
			case CHAREQ : // ID: 92, equal
				return (int) opId ; 
			case INT2EQ : // ID: 94, equal
				return (int) opId ; 
			case INT4EQ : // ID: 96, equal
				return (int) opId ; 
			case TIMETZ_EQ : // ID: 1550, equal
				return (int) opId ; 
			case CASH_EQ : // ID: 900, equal
				return (int) opId ; 
			case VARBITEQ : // ID: 1804, equal
				return (int) opId ; 
			case INT48EQ : // ID: 15, equal
				return (int) opId ; 
			case RECORD_EQ : // ID: 2988, equal
				return (int) opId ; 
			case RANGE_EQ : // ID: 3882, equal
				return (int) opId ; 
			case JSONB_EQ : // ID: 3240, equal
				return (int) opId ; 
			case BOX_EQ : // ID: 503, equal by area
				return (int) opId ; 
			case LSEG_EQ : // ID: 1535, equal
				return (int) opId ; 
			case INT8EQ : // ID: 410, equal
				return (int) opId ; 
			case INT24EQ : // ID: 532, equal
				return (int) opId ; 
			case OIDVECTOREQ : // ID: 649, equal
				return (int) opId ; 
			case TIMESTAMP_EQ : // ID: 2060, equal
				return (int) opId ; 
			case DATE_EQ_TIMESTAMPTZ : // ID: 2360, equal
				return (int) opId ; 
			case TIMESTAMP_EQ_TIMESTAMPTZ : // ID: 2536, equal
				return (int) opId ; 
			case TIME_EQ : // ID: 1108, equal
				return (int) opId ; 
			case INT42EQ : // ID: 533, equal
				return (int) opId ; 

			// <, less than
			case TIMESTAMPTZ_LT : // ID: 1322, less than
				return (int) opId ; 
			case FLOAT4LT : // ID: 622, less than
				return (int) opId ; 
			case MACADDR_LT : // ID: 1222, less than
				return (int) opId ; 
			case UUID_LT : // ID: 2974, less than
				return (int) opId ; 
			case DATE_LT : // ID: 1095, less than
				return LT;
			case TSVECTOR_LT : // ID: 3627, less than
				return (int) opId ; 
			case FLOAT48LT : // ID: 1122, less than
				return (int) opId ; 
			case TEXT_LT : // ID: 664, less than
				return (int) opId ; 
			case OIDLT : // ID: 609, less than
				return (int) opId ; 
			case NETWORK_LT : // ID: 1203, less than
				return (int) opId ; 
			case NAMELT : // ID: 660, less than
				return (int) opId ; 
			case INT82LT : // ID: 1870, less than
				return (int) opId ; 
			case DATE_LT_TIMESTAMPTZ : // ID: 2358, less than
				return (int) opId ; 
			case TSQUERY_LT : // ID: 3674, less than
				return (int) opId ; 
			case DATE_LT_TIMESTAMP : // ID: 2345, less than
				return LT;
			case TIMESTAMPTZ_LT_TIMESTAMP : // ID: 2540, less than
				return (int) opId ; 
			case PATH_N_LT : // ID: 793, less than
				return (int) opId ; 
			case MACADDR8_LT : // ID: 3364, less than
				return (int) opId ; 
			case ARRAY_LT : // ID: 1072, less than
				return (int) opId ; 
			case LSEG_LT : // ID: 1587, less than by length
				return (int) opId ; 
			case CIRCLE_LT : // ID: 1502, less than by area
				return (int) opId ; 
			case TIME_LT : // ID: 1110, less than
				return (int) opId ; 
			case INT24LT : // ID: 534, less than
				return (int) opId ; 
			case TIMESTAMPTZ_LT_DATE : // ID: 2384, less than
				return (int) opId ; 
			case BOX_LT : // ID: 504, less than by area
				return (int) opId ; 
			case NUMERIC_LT : // ID: 1754, less than
				return LT;
			case RELTIMELT : // ID: 568, less than
				return (int) opId ; 
			case CHARLT : // ID: 631, less than
				return (int) opId ; 
			case RANGE_LT : // ID: 3884, less than
				return (int) opId ; 
			case FLOAT84LT : // ID: 1132, less than
				return (int) opId ; 
			case ABSTIMELT : // ID: 562, less than
				return (int) opId ; 
			case FLOAT8LT : // ID: 672, less than
				return (int) opId ; 
			case INTERVAL_LT : // ID: 1332, less than
				return (int) opId ; 
			case BYTEALT : // ID: 1957, less than
				return (int) opId ; 
			case ENUM_LT : // ID: 3518, less than
				return (int) opId ; 
			case BITLT : // ID: 1786, less than
				return (int) opId ; 
			case TINTERVALLT : // ID: 813, less than
				return (int) opId ; 
			case INT2LT : // ID: 95, less than
				return (int) opId ; 
			case INT4LT : // ID: 97, less than
				return LT;
			case CASH_LT : // ID: 902, less than
				return (int) opId ; 
			case VARBITLT : // ID: 1806, less than
				return (int) opId ; 
			case TIMETZ_LT : // ID: 1552, less than
				return (int) opId ; 
			case BOOLLT : // ID: 58, less than
				return (int) opId ; 
			case RECORD_LT : // ID: 2990, less than
				return (int) opId ; 
			case PG_LSN_LT : // ID: 3224, less than
				return (int) opId ; 
			case INT48LT : // ID: 37, less than
				return (int) opId ; 
			case BPCHARLT : // ID: 1058, less than
				return (int) opId ; 
			case INT42LT : // ID: 535, less than
				return (int) opId ; 
			case TIDLT : // ID: 2799, less than
				return LT;
			case OIDVECTORLT : // ID: 645, less than
				return (int) opId ; 
			case INT8LT : // ID: 412, less than
				return LT;
			case JSONB_LT : // ID: 3242, less than
				return (int) opId ; 
			case TIMESTAMP_LT : // ID: 2062, less than
				return (int) opId ; 
			case INT84LT : // ID: 418, less than
				return (int) opId ; 
			case TIMESTAMP_LT_DATE : // ID: 2371, less than
				return (int) opId ; 
			case TIMESTAMP_LT_TIMESTAMPTZ : // ID: 2534, less than
				return (int) opId ; 
			case INT28LT : // ID: 1864, less than
				return (int) opId ; 

			// ?, key exists
			case JSONB_EXISTS : // ID: 3247, key exists
				return (int) opId ; 

			// >, greater than
			case FLOAT4GT : // ID: 623, greater than
				return (int) opId ; 
			case MACADDR_GT : // ID: 1224, greater than
				return (int) opId ; 
			case TIMESTAMPTZ_GT : // ID: 1324, greater than
				return (int) opId ; 
			case RECORD_GT : // ID: 2991, greater than
				return (int) opId ; 
			case NETWORK_GT : // ID: 1205, greater than
				return (int) opId ; 
			case TEXT_GT : // ID: 666, greater than
				return (int) opId ; 
			case FLOAT48GT : // ID: 1123, greater than
				return (int) opId ; 
			case TSVECTOR_GT : // ID: 3632, greater than
				return (int) opId ; 
			case NAMEGT : // ID: 662, greater than
				return (int) opId ; 
			case BITGT : // ID: 1787, greater than
				return (int) opId ; 
			case DATE_GT : // ID: 1097, greater than
				return GT;
			case INT82GT : // ID: 1871, greater than
				return (int) opId ; 
			case TIDGT : // ID: 2800, greater than
				return (int) opId ; 
			case ENUM_GT : // ID: 3519, greater than
				return (int) opId ; 
			case LSEG_GT : // ID: 1589, greater than by length
				return (int) opId ; 
			case CIRCLE_GT : // ID: 1503, greater than by area
				return (int) opId ; 
			case MACADDR8_GT : // ID: 3366, greater than
				return (int) opId ; 
			case DATE_GT_TIMESTAMP : // ID: 2349, greater than
				return GT;
			case ARRAY_GT : // ID: 1073, greater than
				return (int) opId ; 
			case FLOAT84GT : // ID: 1133, greater than
				return (int) opId ; 
			case TIME_GT : // ID: 1112, greater than
				return (int) opId ; 
			case INT2GT : // ID: 520, greater than
				return (int) opId ; 
			case INT4GT : // ID: 521, greater than
				return (int) opId ; 
			case JSONB_GT : // ID: 3243, greater than
				return (int) opId ; 
			case NUMERIC_GT : // ID: 1756, greater than
				return GT;
			case BOX_GT : // ID: 502, greater than by area
				return (int) opId ; 
			case DATE_GT_TIMESTAMPTZ : // ID: 2362, greater than
				return (int) opId ; 
			case OIDVECTORGT : // ID: 646, greater than
				return (int) opId ; 
			case RELTIMEGT : // ID: 569, greater than
				return (int) opId ; 
			case CHARGT : // ID: 633, greater than
				return (int) opId ; 
			case FLOAT8GT : // ID: 674, greater than
				return (int) opId ; 
			case INTERVAL_GT : // ID: 1334, greater than
				return (int) opId ; 
			case OIDGT : // ID: 610, greater than
				return (int) opId ; 
			case ABSTIMEGT : // ID: 563, greater than
				return (int) opId ; 
			case TINTERVALGT : // ID: 814, greater than
				return (int) opId ; 
			case INT48GT : // ID: 76, greater than
				return (int) opId ; 
			case PATH_N_GT : // ID: 794, greater than
				return (int) opId ; 
			case INT28GT : // ID: 1865, greater than
				return (int) opId ; 
			case TIMESTAMPTZ_GT_DATE : // ID: 2388, greater than
				return (int) opId ; 
			case BYTEAGT : // ID: 1959, greater than
				return (int) opId ; 
			case UUID_GT : // ID: 2975, greater than
				return (int) opId ; 
			case VARBITGT : // ID: 1807, greater than
				return (int) opId ; 
			case CASH_GT : // ID: 903, greater than
				return (int) opId ; 
			case RANGE_GT : // ID: 3887, greater than
				return (int) opId ; 
			case BOOLGT : // ID: 59, greater than
				return (int) opId ; 
			case TIMETZ_GT : // ID: 1554, greater than
				return (int) opId ; 
			case PG_LSN_GT : // ID: 3225, greater than
				return (int) opId ; 
			case BPCHARGT : // ID: 1060, greater than
				return (int) opId ; 
			case TIMESTAMPTZ_GT_TIMESTAMP : // ID: 2544, greater than
				return (int) opId ; 
			case INT42GT : // ID: 537, greater than
				return (int) opId ; 
			case INT24GT : // ID: 536, greater than
				return (int) opId ; 
			case TIMESTAMP_GT_TIMESTAMPTZ : // ID: 2538, greater than
				return (int) opId ; 
			case TSQUERY_GT : // ID: 3679, greater than
				return (int) opId ; 
			case INT8GT : // ID: 413, greater than
				return (int) opId ; 
			case TIMESTAMP_GT_DATE : // ID: 2375, greater than
				return (int) opId ; 
			case TIMESTAMP_GT : // ID: 2064, greater than
				return (int) opId ; 
			case INT84GT : // ID: 419, greater than
				return (int) opId ; 

			// *<, less than
			case RECORD_IMAGE_LT : // ID: 3190, less than
				return (int) opId ; 

			// @, absolute value
			case FLOAT4ABS : // ID: 590, absolute value
				return (int) opId ; 
			case FLOAT8ABS : // ID: 595, absolute value
				return (int) opId ; 
			case INT4ABS : // ID: 773, absolute value
				return (int) opId ; 
			case INT8ABS : // ID: 473, absolute value
				return (int) opId ; 
			case NUMERIC_ABS : // ID: 1763, absolute value
				return (int) opId ; 
			case INT2ABS : // ID: 682, absolute value
				return (int) opId ; 

			// #<>, not equal by length
			case TINTERVALLENNE : // ID: 576, not equal by length
				return (int) opId ; 

			// @-@, distance between endpoints
			case LSEG_LENGTH : // ID: 1591, distance between endpoints
				return (int) opId ; 
			case PATH_LENGTH : // ID: 799, sum of path segment lengths
				return (int) opId ; 

			// ~=, same as
			case POINT_EQ : // ID: 510, same as
				return (int) opId ; 
			case CIRCLE_SAME : // ID: 1512, same as
				return (int) opId ; 
			case TINTERVALSAME : // ID: 572, same as
				return (int) opId ; 
			case POLY_SAME : // ID: 491, same as
				return (int) opId ; 
			case BOX_SAME : // ID: 499, same as
				return (int) opId ; 

			// #>>, get value from json as text with path elements
			case JSON_EXTRACT_PATH_TEXT : // ID: 3967, get value from json as text with path elements
				return (int) opId ; 
			case JSONB_EXTRACT_PATH_TEXT : // ID: 3206, get value from jsonb as text with path elements
				return (int) opId ; 

			// !~~*, does not match LIKE expression, case-insensitive
			case BPCHARICNLIKE : // ID: 1630, does not match LIKE expression, case-insensitive
				return (int) opId ; 
			case TEXTICNLIKE : // ID: 1628, does not match LIKE expression, case-insensitive
				return (int) opId ; 
			case NAMEICNLIKE : // ID: 1626, does not match LIKE expression, case-insensitive
				return (int) opId ; 

			// |&>, overlaps or is above
			case POLY_OVERABOVE : // ID: 2576, overlaps or is above
				return (int) opId ; 
			case CIRCLE_OVERABOVE : // ID: 2590, overlaps or is above
				return (int) opId ; 
			case BOX_OVERABOVE : // ID: 2572, overlaps or is above
				return (int) opId ; 

			// &<, overlaps or is left of
			case BOX_OVERLEFT : // ID: 494, overlaps or is left of
				return (int) opId ; 
			case POLY_OVERLEFT : // ID: 486, overlaps or is left of
				return (int) opId ; 
			case CIRCLE_OVERLEFT : // ID: 1507, overlaps or is left of
				return (int) opId ; 
			case RANGE_OVERLEFT : // ID: 3895, overlaps or is left of
				return (int) opId ; 

			// &<|, overlaps or is below
			case POLY_OVERBELOW : // ID: 2575, overlaps or is below
				return (int) opId ; 
			case CIRCLE_OVERBELOW : // ID: 2589, overlaps or is below
				return (int) opId ; 
			case BOX_OVERBELOW : // ID: 2571, overlaps or is below
				return (int) opId ; 

			// &>, overlaps or is right of
			case POLY_OVERRIGHT : // ID: 487, overlaps or is right of
				return (int) opId ; 
			case BOX_OVERRIGHT : // ID: 495, overlaps or is right of
				return (int) opId ; 
			case CIRCLE_OVERRIGHT : // ID: 1508, overlaps or is right of
				return (int) opId ; 
			case RANGE_OVERRIGHT : // ID: 3896, overlaps or is right of
				return (int) opId ; 

			// *=, identical
			case RECORD_IMAGE_EQ : // ID: 3188, identical
				return (int) opId ; 

			// ?-, horizontal
			case LINE_HORIZONTAL : // ID: 1614, horizontal
				return (int) opId ; 
			case POINT_HORIZ : // ID: 808, horizontally aligned
				return (int) opId ; 
			case LSEG_HORIZONTAL : // ID: 1528, horizontal
				return (int) opId ; 

			// &&, overlaps
			case BOX_OVERLAP : // ID: 500, overlaps
				return (int) opId ; 
			case NETWORK_OVERLAP : // ID: 3552, overlaps (is subnet or supernet)
				return (int) opId ; 
			case ARRAYOVERLAP : // ID: 2750, overlaps
				return (int) opId ; 
			case POLY_OVERLAP : // ID: 492, overlaps
				return (int) opId ; 
			case TSQUERY_AND : // ID: 3680, AND-concatenate
				return (int) opId ; 
			case RANGE_OVERLAPS : // ID: 3888, overlaps
				return (int) opId ; 
			case CIRCLE_OVERLAP : // ID: 1513, overlaps
				return (int) opId ; 
			case TINTERVALOV : // ID: 574, overlaps
				return (int) opId ; 

			// |/, square root
			case DSQRT : // ID: 596, square root
				return (int) opId ; 

			// <?>, is contained by
			case INTINTERVAL : // ID: 583, is contained by
				return (int) opId ; 

			// <<, contains
			case TINTERVALCT : // ID: 573, contains
				return (int) opId ; 
			case BITSHIFTLEFT : // ID: 1795, bitwise shift left
				return (int) opId ; 
			case CIRCLE_LEFT : // ID: 1506, is left of
				return (int) opId ; 
			case RANGE_BEFORE : // ID: 3893, is left of
				return (int) opId ; 
			case POINT_LEFT : // ID: 507, is left of
				return (int) opId ; 
			case POLY_LEFT : // ID: 485, is left of
				return (int) opId ; 
			case INT4SHL : // ID: 1884, bitwise shift left
				return (int) opId ; 
			case INT8SHL : // ID: 1890, bitwise shift left
				return (int) opId ; 
			case NETWORK_SUB : // ID: 931, is subnet
				return (int) opId ; 
			case INT2SHL : // ID: 1878, bitwise shift left
				return (int) opId ; 
			case BOX_LEFT : // ID: 493, is left of
				return (int) opId ; 

			// !~, does not match regular expression, case-sensitive
			case NAMEREGEXNE : // ID: 640, does not match regular expression, case-sensitive
				return (int) opId ; 
			case TEXTREGEXNE : // ID: 642, does not match regular expression, case-sensitive
				return (int) opId ; 
			case BPCHARREGEXNE : // ID: 1056, does not match regular expression, case-sensitive
				return (int) opId ; 

			// |>>, is above
			case CIRCLE_ABOVE : // ID: 1514, is above
				return (int) opId ; 
			case BOX_ABOVE : // ID: 2573, is above
				return (int) opId ; 
			case POLY_ABOVE : // ID: 2577, is above
				return (int) opId ; 

			// ~>~, greater than
			case BPCHAR_PATTERN_GT : // ID: 2330, greater than
				return (int) opId ; 
			case TEXT_PATTERN_GT : // ID: 2318, greater than
				return (int) opId ; 

			// >^, is above
			case POINT_ABOVE : // ID: 506, is above
				return (int) opId ; 
			case BOX_ABOVE_EQ : // ID: 800, is above (allows touching)
				return (int) opId ; 

			// <<|, is below
			case CIRCLE_BELOW : // ID: 1515, is below
				return (int) opId ; 
			case POLY_BELOW : // ID: 2574, is below
				return (int) opId ; 
			case BOX_BELOW : // ID: 2570, is below
				return (int) opId ; 

			// <^, is below (allows touching)
			case BOX_BELOW_EQ : // ID: 801, is below (allows touching)
				return (int) opId ; 
			case POINT_BELOW : // ID: 509, is below
				return (int) opId ; 

			// ?||, parallel
			case LSEG_PARALLEL : // ID: 1526, parallel
				return (int) opId ; 
			case LINE_PARALLEL : // ID: 1612, parallel
				return (int) opId ; 

			// !~~, does not match LIKE expression
			case BPCHARNLIKE : // ID: 1212, does not match LIKE expression
				return (int) opId ; 
			case TEXTNLIKE : // ID: 1210, does not match LIKE expression
				return (int) opId ; 
			case BYTEANLIKE : // ID: 2017, does not match LIKE expression
				return (int) opId ; 
			case NAMENLIKE : // ID: 1208, does not match LIKE expression
				return (int) opId ; 

			// ~*, matches regular expression, case-insensitive
			case BPCHARICREGEXEQ : // ID: 1234, matches regular expression, case-insensitive
				return (int) opId ; 
			case NAMEICREGEXEQ : // ID: 1226, matches regular expression, case-insensitive
				return (int) opId ; 
			case TEXTICREGEXEQ : // ID: 1228, matches regular expression, case-insensitive
				return (int) opId ; 

			// ->, get json array element
			case JSON_ARRAY_ELEMENT : // ID: 3964, get json array element
				return (int) opId ; 
			case JSON_OBJECT_FIELD : // ID: 3962, get json object field
				return (int) opId ; 
			case JSONB_OBJECT_FIELD : // ID: 3211, get jsonb object field
				return (int) opId ; 
			case JSONB_ARRAY_ELEMENT : // ID: 3212, get jsonb array element
				return (int) opId ; 

			// *>=, greater than or equal
			case RECORD_IMAGE_GE : // ID: 3193, greater than or equal
				return (int) opId ; 

			// <@, point on line
			case ON_PL : // ID: 1546, point on line
				return (int) opId ; 
			case ON_PS : // ID: 1547, is contained by
				return (int) opId ; 
			case BOX_CONTAINED : // ID: 497, is contained by
				return (int) opId ; 
			case PT_CONTAINED_POLY : // ID: 756, is contained by
				return (int) opId ; 
			case JSONB_CONTAINED : // ID: 3250, is contained by
				return (int) opId ; 
			case PT_CONTAINED_CIRCLE : // ID: 758, is contained by
				return (int) opId ; 
			case TSQ_MCONTAINED : // ID: 3694, is contained by
				return (int) opId ; 
			case ON_SL : // ID: 1548, lseg on line
				return (int) opId ; 
			case ON_SB : // ID: 1549, is contained by
				return (int) opId ; 
			case RANGE_CONTAINED_BY : // ID: 3892, is contained by
				return (int) opId ; 
			case ARRAYCONTAINED : // ID: 2752, is contained by
				return (int) opId ; 
			case POLY_CONTAINED : // ID: 489, is contained by
				return (int) opId ; 
			case ON_PB : // ID: 511, point inside box
				return (int) opId ; 
			case ELEM_CONTAINED_BY_RANGE : // ID: 3891, is contained by
				return (int) opId ; 
			case ON_PPATH : // ID: 512, point within closed path, or point on open path
				return (int) opId ; 
			case CIRCLE_CONTAINED : // ID: 1510, is contained by
				return (int) opId ; 

			// ~<=~, less than or equal
			case TEXT_PATTERN_LE : // ID: 2315, less than or equal
				return (int) opId ; 
			case BPCHAR_PATTERN_LE : // ID: 2327, less than or equal
				return (int) opId ; 

			// *<=, less than or equal
			case RECORD_IMAGE_LE : // ID: 3192, less than or equal
				return (int) opId ; 

			// ~~*, matches LIKE expression, case-insensitive
			case BPCHARICLIKE : // ID: 1629, matches LIKE expression, case-insensitive
				return (int) opId ; 
			case TEXTICLIKE : // ID: 1627, matches LIKE expression, case-insensitive
				return (int) opId ; 
			case NAMEICLIKE : // ID: 1625, matches LIKE expression, case-insensitive
				return (int) opId ; 

			// *<>, not identical
			case RECORD_IMAGE_NE : // ID: 3189, not identical
				return (int) opId ; 

			// @>, contains
			case POLY_CONTAIN_PT : // ID: 757, contains
				return (int) opId ; 
			case PATH_CONTAIN_PT : // ID: 755, contains
				return (int) opId ; 
			case POLY_CONTAIN : // ID: 490, contains
				return (int) opId ; 
			case CIRCLE_CONTAIN_PT : // ID: 759, contains
				return (int) opId ; 
			case TSQ_MCONTAINS : // ID: 3693, contains
				return (int) opId ; 
			case ACLCONTAINS : // ID: 968, contains
				return (int) opId ; 
			case JSONB_CONTAINS : // ID: 3246, contains
				return (int) opId ; 
			case RANGE_CONTAINS_ELEM : // ID: 3889, contains
				return (int) opId ; 
			case BOX_CONTAIN : // ID: 498, contains
				return (int) opId ; 
			case ARRAYCONTAINS : // ID: 2751, contains
				return (int) opId ; 
			case RANGE_CONTAINS : // ID: 3890, contains
				return (int) opId ; 
			case CIRCLE_CONTAIN : // ID: 1511, contains
				return (int) opId ; 
			case BOX_CONTAIN_PT : // ID: 433, contains
				return (int) opId ; 

			// |, start of interval
			case TINTERVALSTART : // ID: 1284, start of interval
				return (int) opId ; 
			case INT2OR : // ID: 1875, bitwise or
				return (int) opId ; 
			case MACADDR_OR : // ID: 3149, bitwise or
				return (int) opId ; 
			case INT8OR : // ID: 1887, bitwise or
				return (int) opId ; 
			case BITOR : // ID: 1792, bitwise or
				return (int) opId ; 
			case INETOR : // ID: 2636, bitwise or
				return (int) opId ; 
			case INT4OR : // ID: 1881, bitwise or
				return (int) opId ; 
			case MACADDR8_OR : // ID: 3370, bitwise or
				return (int) opId ; 

			// !~*, does not match regular expression, case-insensitive
			case NAMEICREGEXNE : // ID: 1227, does not match regular expression, case-insensitive
				return (int) opId ; 
			case BPCHARICREGEXNE : // ID: 1235, does not match regular expression, case-insensitive
				return (int) opId ; 
			case TEXTICREGEXNE : // ID: 1229, does not match regular expression, case-insensitive
				return (int) opId ; 

			// ~, bitwise not
			case INETNOT : // ID: 2634, bitwise not
				return (int) opId ; 
			case MACADDR8_NOT : // ID: 3368, bitwise not
				return (int) opId ; 
			case BPCHARREGEXEQ : // ID: 1055, matches regular expression, case-sensitive
				return (int) opId ; 
			case NAMEREGEXEQ : // ID: 639, matches regular expression, case-sensitive
				return (int) opId ; 
			case BITNOT : // ID: 1794, bitwise not
				return (int) opId ; 
			case TEXTREGEXEQ : // ID: 641, matches regular expression, case-sensitive
				return (int) opId ; 
			case INT2NOT : // ID: 1877, bitwise not
				return (int) opId ; 
			case INT8NOT : // ID: 1889, bitwise not
				return (int) opId ; 
			case MACADDR_NOT : // ID: 3147, bitwise not
				return (int) opId ; 
			case INT4NOT : // ID: 1883, bitwise not
				return (int) opId ; 
		default: 
		     return (int) opId; 
		}
	}
}
