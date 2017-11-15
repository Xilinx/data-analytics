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
    // 0 is not valid, 1 for LD, 2 none so far, 3 for comparisons , 4 for MULT/ADD    
    int opRegs; 
    std::string lReg;
    long lRegId;
    std::string rReg;
    long rRegId;
    std::string fReg;
    long fRegId;
    std::string cReg;
    long cRegId;
    
    inline bool operator == (Ops const& rhs) const {
      bool eq = true;
      if (op.compare(rhs.op) != 0) eq = false;
      else if (lReg.compare(rhs.lReg) != 0) eq = false;
      else if (lRegId != rhs.lRegId)  eq = false;
      else if (rReg.compare(rhs.rReg) != 0) eq = false;
      else if (rRegId != rhs.rRegId)  eq = false;
      else if (fReg.compare(rhs.fReg) != 0) eq = false;
      else if (fRegId != rhs.fRegId)  eq = false;
      else if (cReg.compare(rhs.cReg) != 0) eq = false;
      else if (cRegId != rhs.cRegId)  eq = false;            
      else if (opRegs != rhs.opRegs ) eq = false;
      return eq;
    }
    
    inline size_t hashKey() const {
      return (op.length()+lReg.length()+lRegId+rReg.length()+rRegId+fReg.length()+fRegId+cReg.length()+cRegId);
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
  void insertCmd(std::string& op, std::string& cop, std::string& lop, std::string& rop, std::string& res);
  void splitRegId(std::string& str, std::string& reg, long& regId);
  long getDefVal(std::string& str);
  std::unordered_map<std::string, long> mDefs;
  std::unordered_map<std::string, std::string> mKeys; // for del and cols information
  std::vector<Ops> mOps;
  
};

#endif
