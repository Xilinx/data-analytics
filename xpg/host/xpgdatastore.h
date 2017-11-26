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
#ifndef XPG_DATASTORE_H
#define XPG_DATASTORE_H

//  #include "postgres.h"

#include "xpgbolt.h"

extern "C" {
#include "access/htup.h"
#include "storage/off.h"
}

extern void 
xpg_init_datastore(void);

extern Size 
xpg_get_buffer_size(void);

extern Size
xpg_get_varstream_size(void);

extern bool 
xpg_fetch_next_block(FpgaTaskState * fpgaTaskState);

extern "C" bool 
xpg_insert_block(FpgaTaskState * fpgaTaskState, 
                 XpgBuffer * tupBuffer, 
                 Relation rel,
                 BlockNumber blknum,
                 OffsetNumber startOffsetNum,
                 BufferAccessStrategy strategy,
                 OffsetNumber * stopOffsetNum);

extern "C" bool
xpg_receive_results(FpgaTaskState * fpgaTaskState);

extern "C" void
xpg_populate_varstream(FpgaTaskState * fpgaTaskState);

extern "C" bool 
xpg_store_datum_to_varstream(Datum datum,
                             Oid attrType,
                             XpgVarStream * varstream);

extern "C" void
xpg_print_attr(HeapTuple tuple,
               FpgaTaskState * fpgaTaskState);

extern "C" void
xpg_print_datum(Datum datum, Oid attrType);

extern "C" void
xpg_populate_asciistream(FpgaTaskState * fpgaTaskState);

extern "C" bool
xpg_store_datum_to_asciistream(Datum datum,
                               Oid attrType,
                               XpgVarStream * varstream,
                               std::string &dataStr);

extern "C" int
xpg_fill_block(Relation rel,
               unsigned char * p_bufptr,
               FpgaTaskState * fpgaTaskState);

#endif 
