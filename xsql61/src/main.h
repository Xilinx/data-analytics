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
#ifndef XSQL61_MAIN_H
#define XSQL61_MAIN_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include "sqltbl.h"
#include "sqldecimal.h"
#include "sqlpageloop.h"
#include "sqlq6tpch.h"
#include "sqlq1tpch.h"
#include "sqlutils.h"
#include "xdbutils.h"
#include "xdbconn.h"
#include "oclaccel.h"

void writeSqlQuery1BufCopy(XdbConn *xdbConn, int queueNo, cl_event event, void *userData);
int processQuery1(XdbConn **xdbConns, int kernels, SqlTbl& sqlHost);
int processQuery6(XdbConn **xdbConns, int kernels, SqlTbl& sqlHost);

#endif
