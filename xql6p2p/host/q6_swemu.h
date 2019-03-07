//----------------------------------------------------------------------------
// xql6 defines for sw-emulation flow
//
// 2017-06-16 19:17:26  parik 
//----------------------------------------------------------------------------
#ifndef Q6_SWEMU_H_
#define Q6_SWEMU_H_

//#include <iostream>
//#include <string>
//#include <cstdlib>
//#include <cstdio>
//#include <cstring>
//#include "xql6.h"
#include <stdint.h>

#define ALIGNOF_INT 4

// alings the offset to the next nearest size boundary
//typedef unsigned int uintptr_t;
#define TYPEALIGN(ALIGNVAL,LEN)  \
          (((uintptr_t) (LEN) + ((ALIGNVAL) - 1)) & ~((uintptr_t) ((ALIGNVAL) - 1)))

#define INTALIGN(LEN)  TYPEALIGN(ALIGNOF_INT, (LEN))

#define WORD_SZ 8
#define PAGE_SZ 8192    // 8kb


#define BLK_SZ (2*1024*1024)
#define PAGES_PER_BLK  (BLK_SZ/PAGE_SZ)    // 256

#endif
