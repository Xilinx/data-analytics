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
#include "sqldate.h"



void SqlDate::SQLEVAL(toBin,SQLKERNEL_ID)(hls::stream<SqlDefs::RdDdrWord>& bufStream, SqlDefs::RdTracker& rdTracker) {
  char ch = SqlDefs::readChar(bufStream,rdTracker);
  year = ch-'0';
  ch = SqlDefs::readChar(bufStream,rdTracker);
  year = 10*year + (ch-'0');
  ch = SqlDefs::readChar(bufStream,rdTracker);
  year = 10*year + (ch-'0');
  ch = SqlDefs::readChar(bufStream,rdTracker);
  year = 10*year + (ch-'0');  

  ch = SqlDefs::readChar(bufStream,rdTracker);
#ifdef HLS_DEBUG
  if (ch !='-')
    printf("Error: Incorrect value %c in sql date\n",ch);
#endif
  
  ch = SqlDefs::readChar(bufStream,rdTracker);
  month = (ch-'0');
  ch = SqlDefs::readChar(bufStream,rdTracker);
  month = 10*month + (ch-'0');

  ch = SqlDefs::readChar(bufStream,rdTracker);
#ifdef HLS_DEBUG
  if (ch !='-')
    printf("Error: Incorrect value %c in sql date\n",ch);
#endif

  ch = SqlDefs::readChar(bufStream,rdTracker);
  day = (ch-'0');
  ch = SqlDefs::readChar(bufStream,rdTracker);
  day = 10*day + (ch-'0');
  
    
  ch = SqlDefs::readChar(bufStream,rdTracker);
#ifdef HLS_DEBUG
  if (ch !='|')
    printf("Error: Incorrect value %c in sql date\n",ch);
#endif
  
}

  
