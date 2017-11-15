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
#ifndef XDBCONN_XDBUTILS_H
#define XDBCONN_XDBUTILS_H

#include <time.h>
#include <sys/time.h>
#include <chrono>

class XdbTimer {
public:
  void start() {
    mStart = std::chrono::high_resolution_clock::now(); 
  }
  void end() {
    mEnd = std::chrono::high_resolution_clock::now();
  }
  
  double duration()   {
    std::chrono::duration<double> elapsedSeconds = mEnd - mStart;
    return (elapsedSeconds.count());
  }

  XdbTimer() { }
  ~XdbTimer() { }  

private:
  // steady and system same across most implementaion, use high_resolution_clock (system_cloc)
  std::chrono::time_point<std::chrono::high_resolution_clock> mStart, mEnd;
  //std::chrono::time_point<std::chrono::system_clock> mSystemStart, mSystemEnd;
  //std::chrono::time_point<std::chrono::steady_clock> mSteadyStart, mSteadyEnd;  
};

#endif
