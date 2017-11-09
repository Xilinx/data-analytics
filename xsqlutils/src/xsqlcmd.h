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

#ifndef XSQLUTILS_XSQLCMD_H
#define XSQLUTILS_XSQLCMD_H

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "xpgdate.h"

class XSqlCmd  {
 public:
  bool createSetupStream(char *setupstream);
  bool init(const char *qryFile);
  bool readLine();
  bool done() {
    return (mErr || mDone);
  }
  void print();
  bool err() {return mErr;}
  void readAll() {
    while (!readLine()) scanLine();
  }

  struct Ops {
    std::string op;
    bool ldop;
    std::string lReg;
    long lRegId;
    std::string rReg;
    long rRegId;
    std::string fReg;
    long fRegId;
    
    inline bool operator == (Ops const& rhs) const {
      bool eq = true;
      if (op.compare(rhs.op) != 0) eq = false;
      else if (lReg.compare(rhs.lReg) != 0) eq = false;
      else if (lRegId != rhs.lRegId)  eq = false;
      else if (rReg.compare(rhs.rReg) != 0) eq = false;
      else if (rRegId != rhs.rRegId)  eq = false;
      else if (fReg.compare(rhs.fReg) != 0) eq = false;
      else if (fRegId != rhs.fRegId)  eq = false;      
      else if (ldop != rhs.ldop ) eq = false;
      return eq;
    }
    
    inline size_t hashKey() const {
      return (op.length()+lReg.length()+lRegId+rReg.length()+rRegId+fReg.length()+fRegId);
    }
    
  };
  
  struct Hash {
    size_t operator() (const Ops& ops) const {
      return (ops.hashKey());
    }
  };
  
  struct Equal {
    bool operator() (const Ops & lhs, const Ops & rhs) const {
      return lhs == rhs;
    }
  };
  
  std::unordered_map<std::string, long>& getDefs() {return mDefs;}
  std::vector<Ops>& getOps() {return mOps; }
  std::unordered_map<std::string, std::string>& getKeys() {return mKeys;}

  bool getKeyVal(std::string& name, std::string& val);
  long toLong(std::string str);
  
 private:
  
  void tokenize(std::stringstream& ss, std::vector<std::string>& stk, char delim);  
  void insertDefs(std::string& kstr, long val);
  void insertKeys(std::string kstr, std::string val);
  void scanLine();
  
  char *mItem;
  bool mErr;
  bool mDone;
  FILE *mFqry;
  
  void insertCmd(std::string& op, std::string& lop, std::string& rop, std::string& reg);
  void splitRegId(std::string& str, std::string& reg, long& regId);
  long getDefVal(std::string& str);
  std::unordered_map<std::string, long> mDefs;
  std::unordered_map<std::string, std::string> mKeys; // for del and cols information
  std::vector<Ops> mOps;
  
};

#endif
