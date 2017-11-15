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
  ops.opRegs = 3;
  splitRegId(lop,ops.lReg,ops.lRegId);
  splitRegId(rop,ops.rReg,ops.rRegId);
  splitRegId(res,ops.fReg,ops.fRegId);    
  mOps.push_back(ops);
}

void XSqlCmd::insertCmd(std::string& op, std::string& cop, std::string& lop, std::string& rop, std::string& res) {
  Ops ops;
  ops.op = op;
  ops.opRegs = 4;
  splitRegId(cop,ops.cReg,ops.cRegId);  
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
      std::cout << "Error: Incorrect def: " << mItem << std::endl;
      mErr = true;
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
      ops.opRegs = 1;
      mOps.push_back(ops);
    }
  } else if (stk.size() == 4) {
    insertCmd(stk[0],stk[1],stk[2],stk[3]);
  } else if (stk.size() == 5) {
    insertCmd(stk[0],stk[1],stk[2],stk[3],stk[4]);
  } else if (stk.size() == 2) { // Allow other definitions for cols, delimiters etc
    insertKeys(stk[0],stk[1]);
  } else {
    std::cout << "Error: Incorrect OP:  " << mItem << std::endl;
    mErr = true;    
    return;
  }
}

void XSqlCmd::print() {
  for (auto itr = mDefs.begin(); itr != mDefs.end(); itr++)
    std::cout << "def " << itr->first << " " << itr->second << std::endl;
  for (auto itr = mOps.begin(); itr != mOps.end(); itr++) {
    if (itr->opRegs == 1) {
      std::cout << itr->op << " " << itr->lReg << ":" << itr->lRegId << " ";
      std::cout << itr->fRegId << std::endl;
    } else if (itr->opRegs == 3) {
      std::cout << itr->op << " " << itr->lReg << ":" << itr->lRegId << " ";      
      std::cout << itr->rReg << ":" << itr->rRegId << " ";
      std::cout << itr->fReg << ":" << itr->fRegId << std::endl;
    } else if (itr->opRegs == 4) {
      std::cout << itr->op << " " ;
      std::cout << itr->cReg << ":" << itr->cRegId << " ";
      std::cout << itr->lReg << ":" << itr->lRegId << " ";      
      std::cout << itr->rReg << ":" << itr->rRegId << " ";
      std::cout << itr->fReg << ":" << itr->fRegId << std::endl;
    }
  }
}
