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
/* xpg_oid_enum.h*/

#ifndef XPG_OID_OPERATOR_H
#define XPG_OID_OPERATOR_H

typedef enum {

 // !!, NOT tsquery
  TSQUERY_NOT = 3682, //NOT tsquery

 // @@, center of
  CIRCLE_CENTER = 1420, //center of
  TS_MATCH_TT = 3762, //text search match
  TS_MATCH_TQ = 3763, //text search match
  POLY_CENTER = 971, //center of
  PATH_CENTER = 970, //center of
  TS_MATCH_VQ = 3636, //text search match
  TS_MATCH_QV = 3637, //text search match
  BOX_CENTER = 513, //center of
  LSEG_CENTER = 969, //center of

 // ?|, vertically aligned
  POINT_VERT = 809, //vertically aligned
  LINE_VERTICAL = 1615, //vertical
  LSEG_VERTICAL = 1529, //vertical
  JSONB_EXISTS_ANY = 3248, //any key exists

 // >=, greater than or equal
  BITGE = 1789, //greater than or equal
  MACADDR_GE = 1225, //greater than or equal
  MACADDR8_GE = 3367, //greater than or equal
  FLOAT4GE = 625, //greater than or equal
  TIMESTAMPTZ_GE = 1325, //greater than or equal
  TEXT_GE = 667, //greater than or equal
  RELTIMEGE = 571, //greater than or equal
  NETWORK_GE = 1206, //greater than or equal
  NAMEGE = 663, //greater than or equal
  FLOAT48GE = 1125, //greater than or equal
  TSVECTOR_GE = 3631, //greater than or equal
  INT48GE = 82, //greater than or equal
  TSQUERY_GE = 3678, //greater than or equal
  INT82GE = 1873, //greater than or equal
  TIDGE = 2802, //greater than or equal
  TIMESTAMPTZ_GE_TIMESTAMP = 2543, //greater than or equal
  PATH_N_GE = 796, //greater than or equal
  INT4GE = 525, //greater than or equal
  ARRAY_GE = 1075, //greater than or equal
  BYTEAGE = 1960, //greater than or equal
  DATE_GE_TIMESTAMP = 2348, //greater than or equal
  INT2GE = 524, //greater than or equal
  INT42GE = 543, //greater than or equal
  TIME_GE = 1113, //greater than or equal
  BOOLGE = 1695, //greater than or equal
  TIMESTAMPTZ_GE_DATE = 2387, //greater than or equal
  CIRCLE_GE = 1505, //greater than or equal by area
  NUMERIC_GE = 1757, //greater than or equal
  INTERVAL_GE = 1335, //greater than or equal
  JSONB_GE = 3245, //greater than or equal
  TINTERVALGE = 816, //greater than or equal
  BOX_GE = 501, //greater than or equal by area
  CHARGE = 634, //greater than or equal
  INT24GE = 542, //greater than or equal
  OIDGE = 612, //greater than or equal
  FLOAT8GE = 675, //greater than or equal
  DATE_GE = 1098, //greater than or equal
  ABSTIMEGE = 565, //greater than or equal
  FLOAT84GE = 1135, //greater than or equal
  LSEG_GE = 1590, //greater than or equal by length
  INT28GE = 1867, //greater than or equal
  ENUM_GE = 3521, //greater than or equal
  UUID_GE = 2977, //greater than or equal
  RANGE_GE = 3886, //greater than or equal
  TIMETZ_GE = 1555, //greater than or equal
  CASH_GE = 905, //greater than or equal
  BPCHARGE = 1061, //greater than or equal
  PG_LSN_GE = 3227, //greater than or equal
  RECORD_GE = 2993, //greater than or equal
  VARBITGE = 1809, //greater than or equal
  TIMESTAMP_GE_TIMESTAMPTZ = 2537, //greater than or equal
  INT8GE = 415, //greater than or equal
  DATE_GE_TIMESTAMPTZ = 2361, //greater than or equal
  TIMESTAMP_GE_DATE = 2374, //greater than or equal
  OIDVECTORGE = 648, //greater than or equal
  TIMESTAMP_GE = 2065, //greater than or equal
  INT84GE = 430, //greater than or equal

 // >>, bitwise shift right
  INT4SHR = 1885, //bitwise shift right
  INT8SHR = 1891, //bitwise shift right
  POINT_RIGHT = 508, //is right of
  POLY_RIGHT = 488, //is right of
  NETWORK_SUP = 933, //is supernet
  BOX_RIGHT = 496, //is right of
  CIRCLE_RIGHT = 1509, //is right of
  BITSHIFTRIGHT = 1796, //bitwise shift right
  RANGE_AFTER = 3894, //is right of
  INT2SHR = 1879, //bitwise shift right

 // #<=, less than or equal by length
  TINTERVALLENLE = 579, //less than or equal by length

 // <>, not equal
  DATE_NE = 1094, //not equal
  FLOAT4NE = 621, //not equal
  TIMESTAMPTZ_NE = 1321, //not equal
  MACADDR_NE = 1221, //not equal
  FLOAT48NE = 1121, //not equal
  OIDNE = 608, //not equal
  TSVECTOR_NE = 3630, //not equal
  NETWORK_NE = 1202, //not equal
  PG_LSN_NE = 3223, //not equal
  ARRAY_NE = 1071, //not equal
  TSQUERY_NE = 3677, //not equal
  BOOLNE = 85, //not equal
  RECORD_NE = 2989, //not equal
  TIMESTAMPTZ_NE_TIMESTAMP = 2545, //not equal
  BPCHARNE = 1057, //not equal
  LSEG_NE = 1586, //not equal
  MACADDR8_NE = 3363, //not equal
  CIRCLE_NE = 1501, //not equal by area
  INTERVAL_NE = 1331, //not equal
  TIDNE = 402, //not equal
  DATE_NE_TIMESTAMPTZ = 2363, //not equal
  TIMESTAMPTZ_NE_DATE = 2389, //not equal
  TINTERVALNE = 812, //not equal
  BITNE = 1785, //not equal
  OIDVECTORNE = 644, //not equal
  CHARNE = 630, //not equal
  ABSTIMENE = 561, //not equal
  FLOAT84NE = 1131, //not equal
  POINT_NE = 713, //not equal
  FLOAT8NE = 671, //not equal
  RELTIMENE = 567, //not equal
  INT28NE = 1863, //not equal
  BYTEANE = 1956, //not equal
  ENUM_NE = 3517, //not equal
  INT82NE = 1869, //not equal
  UUID_NE = 2973, //not equal
  XIDNEQ = 3315, //not equal
  TIMESTAMP_NE_DATE = 2376, //not equal
  XIDNEQINT4 = 3316, //not equal
  TIMETZ_NE = 1551, //not equal
  VARBITNE = 1805, //not equal
  CASH_NE = 901, //not equal
  RANGE_NE = 3883, //not equal
  NUMERIC_NE = 1753, //not equal
  JSONB_NE = 3241, //not equal
  INT48NE = 36, //not equal
  TIMESTAMP_NE_TIMESTAMPTZ = 2539, //not equal
  INT2NE = 519, //not equal
  INT4NE = 518, //not equal
  NAMENE = 643, //not equal
  INT84NE = 417, //not equal
  INT8NE = 411, //not equal
  TEXTNE = 531, //not equal
  TIMESTAMP_NE = 2061, //not equal
  DATE_NE_TIMESTAMP = 2350, //not equal
  INT24NE = 538, //not equal
  INT42NE = 539, //not equal
  TIME_NE = 1109, //not equal

 // ||, concatenate
  TEXTANYCAT = 2779, //concatenate
  ANYTEXTCAT = 2780, //concatenate
  TSQUERY_OR = 3681, //OR-concatenate
  BITCAT = 1797, //concatenate
  TSVECTOR_CONCAT = 3633, //concatenate
  ARRAY_PREPEND = 374, //prepend element onto front of array
  TEXTCAT = 654, //concatenate
  BYTEACAT = 2018, //concatenate
  JSONB_CONCAT = 3284, //concatenate
  ARRAY_CAT = 375, //concatenate
  ARRAY_APPEND = 349, //append element onto end of array

 // <=, less than or equal
  BITLE = 1788, //less than or equal
  TIMESTAMPTZ_LE = 1323, //less than or equal
  MACADDR_LE = 1223, //less than or equal
  FLOAT4LE = 624, //less than or equal
  NETWORK_LE = 1204, //less than or equal
  TEXT_LE = 665, //less than or equal
  RELTIMELE = 570, //less than or equal
  FLOAT48LE = 1124, //less than or equal
  DATE_LE = 1096, //less than or equal
  NAMELE = 661, //less than or equal
  INT82LE = 1872, //less than or equal
  INT48LE = 80, //less than or equal
  TIDLE = 2801, //less than or equal
  TSQUERY_LE = 3675, //less than or equal
  PATH_N_LE = 795, //less than or equal
  DATE_LE_TIMESTAMP = 2346, //less than or equal
  BPCHARLE = 1059, //less than or equal
  ARRAY_LE = 1074, //less than or equal
  INT24LE = 540, //less than or equal
  TIME_LE = 1111, //less than or equal
  BOOLLE = 1694, //less than or equal
  INT42LE = 541, //less than or equal
  TIMESTAMPTZ_LE_DATE = 2385, //less than or equal
  INT2LE = 522, //less than or equal
  INT4LE = 523, //less than or equal
  INT84LE = 420, //less than or equal
  NUMERIC_LE = 1755, //less than or equal
  BOX_LE = 505, //less than or equal by area
  TINTERVALLE = 815, //less than or equal
  TSVECTOR_LE = 3628, //less than or equal
  CHARLE = 632, //less than or equal
  CIRCLE_LE = 1504, //less than or equal by area
  OIDLE = 611, //less than or equal
  ABSTIMELE = 564, //less than or equal
  LSEG_LE = 1588, //less than or equal by length
  INTERVAL_LE = 1333, //less than or equal
  FLOAT8LE = 673, //less than or equal
  JSONB_LE = 3244, //less than or equal
  ENUM_LE = 3520, //less than or equal
  INT28LE = 1866, //less than or equal
  FLOAT84LE = 1134, //less than or equal
  UUID_LE = 2976, //less than or equal
  BYTEALE = 1958, //less than or equal
  RANGE_LE = 3885, //less than or equal
  TIMETZ_LE = 1553, //less than or equal
  CASH_LE = 904, //less than or equal
  RECORD_LE = 2992, //less than or equal
  PG_LSN_LE = 3226, //less than or equal
  VARBITLE = 1808, //less than or equal
  INT8LE = 414, //less than or equal
  DATE_LE_TIMESTAMPTZ = 2359, //less than or equal
  OIDVECTORLE = 647, //less than or equal
  MACADDR8_LE = 3365, //less than or equal
  TIMESTAMPTZ_LE_TIMESTAMP = 2541, //less than or equal
  TIMESTAMP_LE = 2063, //less than or equal
  TIMESTAMP_LE_DATE = 2372, //less than or equal
  TIMESTAMP_LE_TIMESTAMPTZ = 2535, //less than or equal

 // *>, greater than
  RECORD_IMAGE_GT = 3191, //greater than

 // ?-|, perpendicular
  LSEG_PERP = 1527, //perpendicular
  LINE_PERP = 1613, //perpendicular

 // -|-, is adjacent to
  RANGE_ADJACENT = 3897, //is adjacent to

 // ^, exponentiation
  DPOW = 965, //exponentiation
  NUMERIC_POWER = 1038, //exponentiation

 // #>, get value from json with path elements
  JSON_EXTRACT_PATH = 3966, //get value from json with path elements
  TINTERVALLENGT = 578, //greater than by length
  JSONB_EXTRACT_PATH = 3213, //get value from jsonb with path elements

 // #=, equal by length
  TINTERVALLENEQ = 575, //equal by length

 // #<, less than by length
  TINTERVALLENLT = 577, //less than by length

 // #-, delete path
  JSONB_DELETE_PATH = 3287, //delete path

 // !, factorial
  NUMERIC_FAC = 388, //factorial

 // #, number of points
  PATH_NPOINTS = 797, //number of points
  LSEG_INTERPT = 1536, //intersection point
  POLY_NPOINTS = 1521, //number of points
  BITXOR = 1793, //bitwise exclusive or
  BOX_INTERSECT = 803, //box intersection
  LINE_INTERPT = 1617, //intersection point
  INT8XOR = 1888, //bitwise exclusive or
  INT4XOR = 1882, //bitwise exclusive or
  INT2XOR = 1876, //bitwise exclusive or

 // %, modulus
  NUMERIC_MOD = 1762, //modulus
  INT4MOD = 530, //modulus
  INT2MOD = 529, //modulus
  INT8MOD = 439, //modulus

 // ~~, matches LIKE expression
  BYTEALIKE = 2016, //matches LIKE expression
  NAMELIKE = 1207, //matches LIKE expression
  BPCHARLIKE = 1211, //matches LIKE expression
  TEXTLIKE = 1209, //matches LIKE expression

 // <<=, is subnet or equal
  NETWORK_SUBEQ = 932, //is subnet or equal

 // &, bitwise and
  INETAND = 2635, //bitwise and
  INT2AND = 1874, //bitwise and
  MACADDR8_AND = 3369, //bitwise and
  MACADDR_AND = 3148, //bitwise and
  INT8AND = 1886, //bitwise and
  BITAND = 1791, //bitwise and
  INT4AND = 1880, //bitwise and

 // ?#, intersect
  INTER_SL = 1537, //intersect
  LSEG_INTERSECT = 1525, //intersect
  LINE_INTERSECT = 1611, //intersect
  PATH_INTER = 798, //intersect
  INTER_LB = 1539, //intersect
  INTER_SB = 1538, //intersect

 // +, add
  FLOAT8PL = 591, //add
  TIMESTAMPTZ_PL_INTERVAL = 1327, //add
  INETPL = 2637, //add
  INT4PL = 551, //add
  INT2PL = 550, //add
  INT42PL = 553, //add
  INT24PL = 552, //add
  FLOAT8UP = 1920, //unary plus
  NUMERIC_UPLUS = 1921, //unary plus
  INTERVAL_PL_TIMETZ = 2552, //add
  BOX_ADD = 804, //add point to box (translate)
  INT8PL_INET = 2638, //add
  INT48PL = 692, //add
  POINT_ADD = 731, //add points (translate)
  DATE_PL_INTERVAL = 1076, //add
  FLOAT48PL = 1116, //add
  INT82PL = 818, //add
  RANGE_UNION = 3898, //range union
  PATH_ADD = 735, //concatenate
  PATH_ADD_PT = 736, //add (translate path)
  INT84PL = 688, //add
  FLOAT4PL = 586, //add
  DATE_PLI = 1100, //add
  TIMEPL = 581, //add
  INTERVAL_PL = 1337, //add
  NUMERIC_ADD = 1758, //add
  ACLINSERT = 966, //add/update ACL item
  INTEGER_PL_DATE = 2555, //add
  FLOAT4UP = 1919, //unary plus
  INT4UP = 1918, //unary plus
  FLOAT84PL = 1126, //add
  INTERVAL_PL_TIMESTAMPTZ = 2554, //add
  INT2UP = 1917, //unary plus
  INT8UP = 1916, //unary plus
  INTERVAL_PL_DATE = 2551, //add
  INTERVAL_PL_TIME = 1849, //add
  INTERVAL_PL_TIMESTAMP = 2553, //add
  CASH_PL = 906, //add
  TIMETZ_PL_INTERVAL = 1802, //add
  TIME_PL_INTERVAL = 1800, //add
  TIMETZDATE_PL = 1366, //convert time with time zone and date to timestamp with time zone
  TIMEDATE_PL = 1363, //convert time and date to timestamp
  DATETIME_PL = 1360, //convert date and time to timestamp
  DATETIMETZ_PL = 1361, //convert date and time with time zone to timestamp with time zone
  INT8PL = 684, //add
  CIRCLE_ADD_PT = 1516, //add
  TIMESTAMP_PL_INTERVAL = 2066, //add
  INT28PL = 822, //add

 // *, multiply
  FLOAT84MUL = 1129, //multiply
  INT4MUL = 514, //multiply
  RANGE_INTERSECT = 3900, //range intersection
  FLOAT8MUL = 594, //multiply
  INT28MUL = 824, //multiply
  FLT4_MUL_CASH = 845, //multiply
  FLOAT4MUL = 589, //multiply
  INT8_MUL_CASH = 3349, //multiply
  PATH_MUL_PT = 738, //multiply (rotate/scale path)
  NUMERIC_MUL = 1760, //multiply
  INT84MUL = 690, //multiply
  INT8MUL = 686, //multiply
  CASH_MUL_FLT4 = 843, //multiply
  INT48MUL = 694, //multiply
  CIRCLE_MUL_PT = 1518, //multiply
  INTERVAL_MUL = 1583, //multiply
  CASH_MUL_FLT8 = 908, //multiply
  CASH_MUL_INT8 = 3346, //multiply
  INT2MUL = 526, //multiply
  MUL_D_INTERVAL = 1584, //multiply
  INT82MUL = 820, //multiply
  POINT_MUL = 733, //multiply points (scale/rotate)
  INT2_MUL_CASH = 918, //multiply
  INT24MUL = 544, //multiply
  INT42MUL = 545, //multiply
  FLOAT48MUL = 1119, //multiply
  CASH_MUL_INT2 = 914, //multiply
  INT4_MUL_CASH = 917, //multiply
  FLT8_MUL_CASH = 916, //multiply
  BOX_MUL = 806, //multiply box by point (scale)
  CASH_MUL_INT4 = 912, //multiply

 // -, subtract
  FLOAT8MI = 592, //subtract
  INT2UM = 559, //negate
  INT4UM = 558, //negate
  INT4MI = 555, //subtract
  INT2MI = 554, //subtract
  INT42MI = 557, //subtract
  INT24MI = 556, //subtract
  FLOAT84MI = 1127, //subtract
  INETMI_INT8 = 2639, //subtract
  INTERVAL_MI = 1338, //subtract
  INT48MI = 693, //subtract
  DATE_MI_INTERVAL = 1077, //subtract
  INT82MI = 819, //subtract
  POINT_SUB = 732, //subtract points (translate)
  TIME_MI_TIME = 1399, //subtract
  TIMESTAMPTZ_MI = 1328, //subtract
  NUMERIC_UMINUS = 1751, //negate
  FLOAT4MI = 587, //subtract
  FLOAT4UM = 584, //negate
  FLOAT8UM = 585, //negate
  TIMEMI = 582, //subtract
  FLOAT48MI = 1117, //subtract
  INETMI = 2640, //subtract
  RANGE_MINUS = 3899, //range difference
  INTERVAL_UM = 1336, //negate
  DATE_MI = 1099, //subtract
  INT8UM = 484, //negate
  CASH_MI = 907, //subtract
  ACLREMOVE = 967, //remove ACL item
  TIMETZ_MI_INTERVAL = 1803, //subtract
  PG_LSN_MI = 3228, //minus
  TIME_MI_INTERVAL = 1801, //subtract
  NUMERIC_SUB = 1759, //subtract
  TIMESTAMPTZ_MI_INTERVAL = 1329, //subtract
  PATH_SUB_PT = 737, //subtract (translate path)
  TIMESTAMP_MI_INTERVAL = 2068, //subtract
  INT84MI = 689, //subtract
  BOX_SUB = 805, //subtract point from box (translate)
  DATE_MII = 1101, //subtract
  INT8MI = 685, //subtract
  CIRCLE_SUB_PT = 1517, //subtract
  INT28MI = 823, //subtract
  TIMESTAMP_MI = 2067, //subtract

 // /, divide
  FLOAT84DIV = 1128, //divide
  FLOAT8DIV = 593, //divide
  CIRCLE_DIV_PT = 1519, //divide
  CASH_DIV_INT2 = 915, //divide
  CASH_DIV_CASH = 3825, //divide
  FLOAT4DIV = 588, //divide
  INT28DIV = 825, //divide
  CASH_DIV_FLT4 = 844, //divide
  INT84DIV = 691, //divide
  INT48DIV = 695, //divide
  INT82DIV = 821, //divide
  CASH_DIV_FLT8 = 909, //divide
  CASH_DIV_INT8 = 3347, //divide
  INTERVAL_DIV = 1585, //divide
  INT2DIV = 527, //divide
  INT24DIV = 546, //divide
  INT42DIV = 547, //divide
  PATH_DIV_PT = 739, //divide (rotate/scale path)
  FLOAT48DIV = 1118, //divide
  INT8DIV = 687, //divide
  NUMERIC_DIV = 1761, //divide
  INT4DIV = 528, //divide
  BOX_DIV = 807, //divide box by point (scale)
  CASH_DIV_INT4 = 913, //divide
  POINT_DIV = 734, //divide points (scale/rotate)

 // ~<~, less than
  TEXT_PATTERN_LT = 2314, //less than
  BPCHAR_PATTERN_LT = 2326, //less than

 // ##, closest point to A on B
  CLOSE_LB = 1568, //closest point to A on B
  CLOSE_LSEG = 1578, //closest point to A on B
  CLOSE_PL = 1557, //closest point to A on B
  CLOSE_PB = 1559, //closest point to A on B
  CLOSE_PS = 1558, //closest point to A on B
  CLOSE_LS = 1577, //closest point to A on B
  CLOSE_SL = 1566, //closest point to A on B
  CLOSE_SB = 1567, //closest point to A on B

 // >>=, is supernet or equal
  NETWORK_SUPEQ = 934, //is supernet or equal

 // ~>=~, greater than or equal
  TEXT_PATTERN_GE = 2317, //greater than or equal
  BPCHAR_PATTERN_GE = 2329, //greater than or equal

 // ||/, cube root
  DCBRT = 597, //cube root

 // ?&, all keys exist
  JSONB_EXISTS_ALL = 3249, //all keys exist

 // <->, distance between
  DIST_LB = 1524, //distance between
  DIST_PPATH = 618, //distance between
  LINE_DISTANCE = 708, //distance between
  LSEG_DISTANCE = 709, //distance between
  DIST_PL = 613, //distance between
  BOX_DISTANCE = 706, //distance between
  PATH_DISTANCE = 707, //distance between
  DIST_SL = 616, //distance between
  DIST_SB = 617, //distance between
  DIST_PS = 614, //distance between
  DIST_PB = 615, //distance between
  DIST_PPOLY = 3276, //distance between
  DIST_POLYP = 3289, //distance between
  CIRCLE_DISTANCE = 1520, //distance between
  DIST_PC = 1522, //distance between
  DIST_CPOLY = 1523, //distance between
  POLY_DISTANCE = 712, //distance between
  DIST_CPOINT = 3291, //distance between
  POINT_DISTANCE = 517, //distance between

 // ->>, get jsonb array element as text
  JSONB_ARRAY_ELEMENT_TEXT = 3481, //get jsonb array element as text
  JSONB_OBJECT_FIELD_TEXT = 3477, //get jsonb object field as text
  JSON_ARRAY_ELEMENT_TEXT = 3965, //get json array element as text
  JSON_OBJECT_FIELD_TEXT = 3963, //get json object field as text

 // <#>, convert to tinterval
  MKTINTERVAL = 606, //convert to tinterval

 // #>=, greater than or equal by length
  TINTERVALLENGE = 580, //greater than or equal by length

 // =, equal
  OIDEQ = 607, //equal
  FLOAT4EQ = 620, //equal
  MACADDR_EQ = 1220, //equal
  FLOAT48EQ = 1120, //equal
  XIDEQINT4 = 353, //equal
  NETWORK_EQ = 1201, //equal
  TIMESTAMPTZ_EQ = 1320, //equal
  ENUM_EQ = 3516, //equal
  UUID_EQ = 2972, //equal
  ARRAY_EQ = 1070, //equal
  PG_LSN_EQ = 3222, //equal
  TSQUERY_EQ = 3676, //equal
  TIMESTAMPTZ_EQ_TIMESTAMP = 2542, //equal
  DATE_EQ_TIMESTAMP = 2347, //equal
  ACLITEMEQ = 974, //equal
  BPCHAREQ = 1054, //equal
  PATH_N_EQ = 792, //equal
  LINE_EQ = 1616, //equal
  INT84EQ = 416, //equal
  MACADDR8_EQ = 3362, //equal
  CIRCLE_EQ = 1500, //equal by area
  TIMESTAMPTZ_EQ_DATE = 2386, //equal
  TINTERVALEQ = 811, //equal
  NUMERIC_EQ = 1752, //equal
  XIDEQ = 352, //equal
  TSVECTOR_EQ = 3629, //equal
  DATE_EQ = 1093, //equal
  ABSTIMEEQ = 560, //equal
  BITEQ = 1784, //equal
  FLOAT84EQ = 1130, //equal
  FLOAT8EQ = 670, //equal
  INTERVAL_EQ = 1330, //equal
  RELTIMEEQ = 566, //equal
  BYTEAEQ = 1955, //equal
  TEXTEQ = 98, //equal
  INT28EQ = 1862, //equal
  CIDEQ = 385, //equal
  TIMESTAMP_EQ_DATE = 2373, //equal
  TIDEQ = 387, //equal
  BOOLEQ = 91, //equal
  INT82EQ = 1868, //equal
  NAMEEQ = 93, //equal
  CHAREQ = 92, //equal
  INT2EQ = 94, //equal
  INT4EQ = 96, //equal
  TIMETZ_EQ = 1550, //equal
  CASH_EQ = 900, //equal
  VARBITEQ = 1804, //equal
  INT48EQ = 15, //equal
  RECORD_EQ = 2988, //equal
  RANGE_EQ = 3882, //equal
  JSONB_EQ = 3240, //equal
  BOX_EQ = 503, //equal by area
  LSEG_EQ = 1535, //equal
  INT8EQ = 410, //equal
  INT24EQ = 532, //equal
  OIDVECTOREQ = 649, //equal
  TIMESTAMP_EQ = 2060, //equal
  DATE_EQ_TIMESTAMPTZ = 2360, //equal
  TIMESTAMP_EQ_TIMESTAMPTZ = 2536, //equal
  TIME_EQ = 1108, //equal
  INT42EQ = 533, //equal

 // <, less than
  TIMESTAMPTZ_LT = 1322, //less than
  FLOAT4LT = 622, //less than
  MACADDR_LT = 1222, //less than
  UUID_LT = 2974, //less than
  DATE_LT = 1095, //less than
  TSVECTOR_LT = 3627, //less than
  FLOAT48LT = 1122, //less than
  TEXT_LT = 664, //less than
  OIDLT = 609, //less than
  NETWORK_LT = 1203, //less than
  NAMELT = 660, //less than
  INT82LT = 1870, //less than
  DATE_LT_TIMESTAMPTZ = 2358, //less than
  TSQUERY_LT = 3674, //less than
  DATE_LT_TIMESTAMP = 2345, //less than
  TIMESTAMPTZ_LT_TIMESTAMP = 2540, //less than
  PATH_N_LT = 793, //less than
  MACADDR8_LT = 3364, //less than
  ARRAY_LT = 1072, //less than
  LSEG_LT = 1587, //less than by length
  CIRCLE_LT = 1502, //less than by area
  TIME_LT = 1110, //less than
  INT24LT = 534, //less than
  TIMESTAMPTZ_LT_DATE = 2384, //less than
  BOX_LT = 504, //less than by area
  NUMERIC_LT = 1754, //less than
  RELTIMELT = 568, //less than
  CHARLT = 631, //less than
  RANGE_LT = 3884, //less than
  FLOAT84LT = 1132, //less than
  ABSTIMELT = 562, //less than
  FLOAT8LT = 672, //less than
  INTERVAL_LT = 1332, //less than
  BYTEALT = 1957, //less than
  ENUM_LT = 3518, //less than
  BITLT = 1786, //less than
  TINTERVALLT = 813, //less than
  INT2LT = 95, //less than
  INT4LT = 97, //less than
  CASH_LT = 902, //less than
  VARBITLT = 1806, //less than
  TIMETZ_LT = 1552, //less than
  BOOLLT = 58, //less than
  RECORD_LT = 2990, //less than
  PG_LSN_LT = 3224, //less than
  INT48LT = 37, //less than
  BPCHARLT = 1058, //less than
  INT42LT = 535, //less than
  TIDLT = 2799, //less than
  OIDVECTORLT = 645, //less than
  INT8LT = 412, //less than
  JSONB_LT = 3242, //less than
  TIMESTAMP_LT = 2062, //less than
  INT84LT = 418, //less than
  TIMESTAMP_LT_DATE = 2371, //less than
  TIMESTAMP_LT_TIMESTAMPTZ = 2534, //less than
  INT28LT = 1864, //less than

 // ?, key exists
  JSONB_EXISTS = 3247, //key exists

 // >, greater than
  FLOAT4GT = 623, //greater than
  MACADDR_GT = 1224, //greater than
  TIMESTAMPTZ_GT = 1324, //greater than
  RECORD_GT = 2991, //greater than
  NETWORK_GT = 1205, //greater than
  TEXT_GT = 666, //greater than
  FLOAT48GT = 1123, //greater than
  TSVECTOR_GT = 3632, //greater than
  NAMEGT = 662, //greater than
  BITGT = 1787, //greater than
  DATE_GT = 1097, //greater than
  INT82GT = 1871, //greater than
  TIDGT = 2800, //greater than
  ENUM_GT = 3519, //greater than
  LSEG_GT = 1589, //greater than by length
  CIRCLE_GT = 1503, //greater than by area
  MACADDR8_GT = 3366, //greater than
  DATE_GT_TIMESTAMP = 2349, //greater than
  ARRAY_GT = 1073, //greater than
  FLOAT84GT = 1133, //greater than
  TIME_GT = 1112, //greater than
  INT2GT = 520, //greater than
  INT4GT = 521, //greater than
  JSONB_GT = 3243, //greater than
  NUMERIC_GT = 1756, //greater than
  BOX_GT = 502, //greater than by area
  DATE_GT_TIMESTAMPTZ = 2362, //greater than
  OIDVECTORGT = 646, //greater than
  RELTIMEGT = 569, //greater than
  CHARGT = 633, //greater than
  FLOAT8GT = 674, //greater than
  INTERVAL_GT = 1334, //greater than
  OIDGT = 610, //greater than
  ABSTIMEGT = 563, //greater than
  TINTERVALGT = 814, //greater than
  INT48GT = 76, //greater than
  PATH_N_GT = 794, //greater than
  INT28GT = 1865, //greater than
  TIMESTAMPTZ_GT_DATE = 2388, //greater than
  BYTEAGT = 1959, //greater than
  UUID_GT = 2975, //greater than
  VARBITGT = 1807, //greater than
  CASH_GT = 903, //greater than
  RANGE_GT = 3887, //greater than
  BOOLGT = 59, //greater than
  TIMETZ_GT = 1554, //greater than
  PG_LSN_GT = 3225, //greater than
  BPCHARGT = 1060, //greater than
  TIMESTAMPTZ_GT_TIMESTAMP = 2544, //greater than
  INT42GT = 537, //greater than
  INT24GT = 536, //greater than
  TIMESTAMP_GT_TIMESTAMPTZ = 2538, //greater than
  TSQUERY_GT = 3679, //greater than
  INT8GT = 413, //greater than
  TIMESTAMP_GT_DATE = 2375, //greater than
  TIMESTAMP_GT = 2064, //greater than
  INT84GT = 419, //greater than

 // *<, less than
  RECORD_IMAGE_LT = 3190, //less than

 // @, absolute value
  FLOAT4ABS = 590, //absolute value
  FLOAT8ABS = 595, //absolute value
  INT4ABS = 773, //absolute value
  INT8ABS = 473, //absolute value
  NUMERIC_ABS = 1763, //absolute value
  INT2ABS = 682, //absolute value

 // #<>, not equal by length
  TINTERVALLENNE = 576, //not equal by length

 // @-@, distance between endpoints
  LSEG_LENGTH = 1591, //distance between endpoints
  PATH_LENGTH = 799, //sum of path segment lengths

 // ~=, same as
  POINT_EQ = 510, //same as
  CIRCLE_SAME = 1512, //same as
  TINTERVALSAME = 572, //same as
  POLY_SAME = 491, //same as
  BOX_SAME = 499, //same as

 // #>>, get value from json as text with path elements
  JSON_EXTRACT_PATH_TEXT = 3967, //get value from json as text with path elements
  JSONB_EXTRACT_PATH_TEXT = 3206, //get value from jsonb as text with path elements

 // !~~*, does not match LIKE expression, case-insensitive
  BPCHARICNLIKE = 1630, //does not match LIKE expression, case-insensitive
  TEXTICNLIKE = 1628, //does not match LIKE expression, case-insensitive
  NAMEICNLIKE = 1626, //does not match LIKE expression, case-insensitive

 // |&>, overlaps or is above
  POLY_OVERABOVE = 2576, //overlaps or is above
  CIRCLE_OVERABOVE = 2590, //overlaps or is above
  BOX_OVERABOVE = 2572, //overlaps or is above

 // &<, overlaps or is left of
  BOX_OVERLEFT = 494, //overlaps or is left of
  POLY_OVERLEFT = 486, //overlaps or is left of
  CIRCLE_OVERLEFT = 1507, //overlaps or is left of
  RANGE_OVERLEFT = 3895, //overlaps or is left of

 // &<|, overlaps or is below
  POLY_OVERBELOW = 2575, //overlaps or is below
  CIRCLE_OVERBELOW = 2589, //overlaps or is below
  BOX_OVERBELOW = 2571, //overlaps or is below

 // &>, overlaps or is right of
  POLY_OVERRIGHT = 487, //overlaps or is right of
  BOX_OVERRIGHT = 495, //overlaps or is right of
  CIRCLE_OVERRIGHT = 1508, //overlaps or is right of
  RANGE_OVERRIGHT = 3896, //overlaps or is right of

 // *=, identical
  RECORD_IMAGE_EQ = 3188, //identical

 // ?-, horizontal
  LINE_HORIZONTAL = 1614, //horizontal
  POINT_HORIZ = 808, //horizontally aligned
  LSEG_HORIZONTAL = 1528, //horizontal

 // &&, overlaps
  BOX_OVERLAP = 500, //overlaps
  NETWORK_OVERLAP = 3552, //overlaps (is subnet or supernet)
  ARRAYOVERLAP = 2750, //overlaps
  POLY_OVERLAP = 492, //overlaps
  TSQUERY_AND = 3680, //AND-concatenate
  RANGE_OVERLAPS = 3888, //overlaps
  CIRCLE_OVERLAP = 1513, //overlaps
  TINTERVALOV = 574, //overlaps

 // |/, square root
  DSQRT = 596, //square root

 // <?>, is contained by
  INTINTERVAL = 583, //is contained by

 // <<, contains
  TINTERVALCT = 573, //contains
  BITSHIFTLEFT = 1795, //bitwise shift left
  CIRCLE_LEFT = 1506, //is left of
  RANGE_BEFORE = 3893, //is left of
  POINT_LEFT = 507, //is left of
  POLY_LEFT = 485, //is left of
  INT4SHL = 1884, //bitwise shift left
  INT8SHL = 1890, //bitwise shift left
  NETWORK_SUB = 931, //is subnet
  INT2SHL = 1878, //bitwise shift left
  BOX_LEFT = 493, //is left of

 // !~, does not match regular expression, case-sensitive
  NAMEREGEXNE = 640, //does not match regular expression, case-sensitive
  TEXTREGEXNE = 642, //does not match regular expression, case-sensitive
  BPCHARREGEXNE = 1056, //does not match regular expression, case-sensitive

 // |>>, is above
  CIRCLE_ABOVE = 1514, //is above
  BOX_ABOVE = 2573, //is above
  POLY_ABOVE = 2577, //is above

 // ~>~, greater than
  BPCHAR_PATTERN_GT = 2330, //greater than
  TEXT_PATTERN_GT = 2318, //greater than

 // >^, is above
  POINT_ABOVE = 506, //is above
  BOX_ABOVE_EQ = 800, //is above (allows touching)

 // <<|, is below
  CIRCLE_BELOW = 1515, //is below
  POLY_BELOW = 2574, //is below
  BOX_BELOW = 2570, //is below

 // <^, is below (allows touching)
  BOX_BELOW_EQ = 801, //is below (allows touching)
  POINT_BELOW = 509, //is below

 // ?||, parallel
  LSEG_PARALLEL = 1526, //parallel
  LINE_PARALLEL = 1612, //parallel

 // !~~, does not match LIKE expression
  BPCHARNLIKE = 1212, //does not match LIKE expression
  TEXTNLIKE = 1210, //does not match LIKE expression
  BYTEANLIKE = 2017, //does not match LIKE expression
  NAMENLIKE = 1208, //does not match LIKE expression

 // ~*, matches regular expression, case-insensitive
  BPCHARICREGEXEQ = 1234, //matches regular expression, case-insensitive
  NAMEICREGEXEQ = 1226, //matches regular expression, case-insensitive
  TEXTICREGEXEQ = 1228, //matches regular expression, case-insensitive

 // ->, get json array element
  JSON_ARRAY_ELEMENT = 3964, //get json array element
  JSON_OBJECT_FIELD = 3962, //get json object field
  JSONB_OBJECT_FIELD = 3211, //get jsonb object field
  JSONB_ARRAY_ELEMENT = 3212, //get jsonb array element

 // *>=, greater than or equal
  RECORD_IMAGE_GE = 3193, //greater than or equal

 // <@, point on line
  ON_PL = 1546, //point on line
  ON_PS = 1547, //is contained by
  BOX_CONTAINED = 497, //is contained by
  PT_CONTAINED_POLY = 756, //is contained by
  JSONB_CONTAINED = 3250, //is contained by
  PT_CONTAINED_CIRCLE = 758, //is contained by
  TSQ_MCONTAINED = 3694, //is contained by
  ON_SL = 1548, //lseg on line
  ON_SB = 1549, //is contained by
  RANGE_CONTAINED_BY = 3892, //is contained by
  ARRAYCONTAINED = 2752, //is contained by
  POLY_CONTAINED = 489, //is contained by
  ON_PB = 511, //point inside box
  ELEM_CONTAINED_BY_RANGE = 3891, //is contained by
  ON_PPATH = 512, //point within closed path, or point on open path
  CIRCLE_CONTAINED = 1510, //is contained by

 // ~<=~, less than or equal
  TEXT_PATTERN_LE = 2315, //less than or equal
  BPCHAR_PATTERN_LE = 2327, //less than or equal

 // *<=, less than or equal
  RECORD_IMAGE_LE = 3192, //less than or equal

 // ~~*, matches LIKE expression, case-insensitive
  BPCHARICLIKE = 1629, //matches LIKE expression, case-insensitive
  TEXTICLIKE = 1627, //matches LIKE expression, case-insensitive
  NAMEICLIKE = 1625, //matches LIKE expression, case-insensitive

 // *<>, not identical
  RECORD_IMAGE_NE = 3189, //not identical

 // @>, contains
  POLY_CONTAIN_PT = 757, //contains
  PATH_CONTAIN_PT = 755, //contains
  POLY_CONTAIN = 490, //contains
  CIRCLE_CONTAIN_PT = 759, //contains
  TSQ_MCONTAINS = 3693, //contains
  ACLCONTAINS = 968, //contains
  JSONB_CONTAINS = 3246, //contains
  RANGE_CONTAINS_ELEM = 3889, //contains
  BOX_CONTAIN = 498, //contains
  ARRAYCONTAINS = 2751, //contains
  RANGE_CONTAINS = 3890, //contains
  CIRCLE_CONTAIN = 1511, //contains
  BOX_CONTAIN_PT = 433, //contains

 // |, start of interval
  TINTERVALSTART = 1284, //start of interval
  INT2OR = 1875, //bitwise or
  MACADDR_OR = 3149, //bitwise or
  INT8OR = 1887, //bitwise or
  BITOR = 1792, //bitwise or
  INETOR = 2636, //bitwise or
  INT4OR = 1881, //bitwise or
  MACADDR8_OR = 3370, //bitwise or

 // !~*, does not match regular expression, case-insensitive
  NAMEICREGEXNE = 1227, //does not match regular expression, case-insensitive
  BPCHARICREGEXNE = 1235, //does not match regular expression, case-insensitive
  TEXTICREGEXNE = 1229, //does not match regular expression, case-insensitive

 // ~, bitwise not
  INETNOT = 2634, //bitwise not
  MACADDR8_NOT = 3368, //bitwise not
  BPCHARREGEXEQ = 1055, //matches regular expression, case-sensitive
  NAMEREGEXEQ = 639, //matches regular expression, case-sensitive
  BITNOT = 1794, //bitwise not
  TEXTREGEXEQ = 641, //matches regular expression, case-sensitive
  INT2NOT = 1877, //bitwise not
  INT8NOT = 1889, //bitwise not
  MACADDR_NOT = 3147, //bitwise not
  INT4NOT = 1883, //bitwise not
} OID_ENUM_TYPE;

#endif  /* XPG_OID_OPERATOR_H */
