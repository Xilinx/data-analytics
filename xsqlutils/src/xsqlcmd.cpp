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
#include <algorithm>
#include <cctype>
#include "xsqlcmd.h"


bool XSqlCmd::init(const char *qryFile) {
  mFqry = fopen(qryFile,"r");
  mErr = false;
  mDone = false;
  mItem = new char[256];
  mItem[0] = 0;
  if (NULL == mFqry) {
    std::cout << "Error: Cannot open file " << qryFile << std::endl;
    mErr = true;
  }
  return !mErr;
}
    
bool XSqlCmd::readLine() {
  if (mErr)
    return false;
  size_t len;
  if (getline(&mItem, &len, mFqry) == -1)
    mDone = true;
  return mDone;
}

void XSqlCmd::tokenize(std::stringstream& ss,
		      std::vector<std::string>& stk,
		      char delim) {
  std::string tmp;
  while (getline(ss,tmp,delim)) { // tokenizer with delim
    tmp.erase(std::remove_if(tmp.begin(),tmp.end(),[](unsigned char x){return std::isspace(x);}),tmp.end());
    stk.push_back(tmp);
  }
}

void XSqlCmd::splitRegId(std::string& str,
			std::string& reg,
			long& regId) {
  std::vector<std::string> stk;
  std::stringstream ss(str);
  tokenize(ss,stk,':');
  if (stk.size() < 2) {
    std::cout << "Error: Incorrect operands " << str << std::endl;
    mErr = true;
  } else {
    reg = stk[0];
    auto itr = mDefs.find(stk[1]);
    if (itr != mDefs.end())
      regId = itr->second;
    else 
      regId = toLong(stk[1]);
  }
}

bool XSqlCmd::getKeyVal(std::string& name, std::string& val) {
  auto itr = mKeys.find(name);
  if (itr != mKeys.end()) {
    val = itr->second;
    return true;
  }
  return false;
}

void XSqlCmd::insertDefs(std::string& kstr, long val) {
  auto itr = mDefs.find(kstr);
  if (itr == mDefs.end()) {
    mDefs[kstr] = val;
  } else {
    std::cout << "Error: redefining same value not allowed .." << mItem << std::endl;
  }
}

void XSqlCmd::insertKeys(std::string kstr, std::string val) {
  auto itr = mKeys.find(kstr);
  if (itr == mKeys.end()) {
    mKeys[kstr] = val;
  } else {
    std::cout << "Error: redefining same value not allowed .." << mItem << std::endl;
  }
}

void XSqlCmd::insertCmd(std::string& op, std::string& lop, std::string& rop, std::string& res) {
  Ops ops;
  ops.op = op;
  ops.ldop = false;
  splitRegId(lop,ops.lReg,ops.lRegId);
  splitRegId(rop,ops.rReg,ops.rRegId);
  splitRegId(res,ops.fReg,ops.fRegId);    
  mOps.push_back(ops);
}

long XSqlCmd::getDefVal(std::string& str ) {
  auto itr = mDefs.find(str);
  if (itr != mDefs.end())
    return itr->second;
  return (toLong(str));
}

long XSqlCmd::toLong(std::string str) {
  try {
    long lno = std::stol(str);
    return (lno);
  } catch (...) {
    std::cout << "Error: Invalid int string  " << str << std::endl;
  }
  return 0;
}

void XSqlCmd::scanLine() {
  if (mItem[0] == 0)
    return ;
  if (mItem[0] == '#') // Comments start with #
    return ;
  std::stringstream ss(mItem);
  std::vector<std::string> stk;
  tokenize(ss,stk,' ');
  if (stk.size() == 0)
    return;
  if (stk[0][0] == '#') return ;
  if (stk[0] == "def") {
    if (stk.size() != 3) {
      std::cout << "Incorrect def: " << mItem << std::endl;
      return;
    }
    std::stringstream ssd(stk[2]);
    std::vector<std::string> stkd;
    tokenize(ssd,stkd,'/');
    if (stkd.size() == 3) {
      XpgDate tm;
      tm.y = toLong(stkd[0]);
      tm.m = toLong(stkd[1]);
      tm.d =  toLong(stkd[2]);
      long absday = tm.toEpochDate();
      insertDefs(stk[1],absday);
    } else {
      insertDefs(stk[1],toLong(stk[2]));
    }
  } else if (stk[0] == "LD") {
    if (stk.size() != 3) {
      std::cout << "Error:  Incorrect op " << mItem << std::endl;
      mErr = true;
    } else { // a valid LD ops
      Ops ops;
      ops.op = stk[0];
      ops.fRegId = getDefVal(stk[2]);
      splitRegId(stk[1],ops.lReg,ops.lRegId);	  
      ops.ldop = true;
      mOps.push_back(ops);
    }
  } else if (stk.size() == 4) {
    insertCmd(stk[0],stk[1],stk[2],stk[3]);
  } else if (stk.size() == 2) { // Allow other definitions for cols, delimiters etc
    insertKeys(stk[0],stk[1]);
  } else {
    std::cout << "Incorrect OP:  " << mItem << std::endl;
    return;
  }
}

void XSqlCmd::print() {
  for (auto itr = mDefs.begin(); itr != mDefs.end(); itr++)
    std::cout << "def " << itr->first << " " << itr->second << std::endl;
  for (auto itr = mOps.begin(); itr != mOps.end(); itr++) {
    std::cout << itr->op << " " << itr->lReg << ":" << itr->lRegId << " ";
    if (itr->ldop) {
      std::cout << itr->fRegId << std::endl;      
    } else {
      std::cout << itr->rReg << ":" << itr->rRegId << " ";
      std::cout << itr->fReg << ":" << itr->fRegId << std::endl;
    }
  }
}
