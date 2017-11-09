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
