//----------------------------------------------------------------------------
// xql6: UDF package for sql6
//
// 2017-06-16 19:13:37  parik 
//----------------------------------------------------------------------------
#ifndef _XQL6_H__
#define _XQL6_H__
#include <stdbool.h>

typedef struct Xql6FillState {
  int blkNum;
  int nBlks;
  int linNum;  // init to FirstOffsetNumber
} Xql6FillState; 

// TODO common this across hls sources
#define XQL_WORD_SZ 16
#define XQL_PAGE_SZ 8192                              // 8KB
#define XQL6_BLK_SZ (2*1024*1024)

#define XQL_WORDS_PER_PAGE (XQL_PAGE_SZ/XQL_WORD_SZ)  // 512

#define XQL6_PAGES_PER_BLK  (XQL6_BLK_SZ/XQL_PAGE_SZ) // 256

#define XQL_NUM_WORDS (XQL_BLK_SZ/XQL_WORD_SZ)        // 131072 words

#define XQL_NUM_RES_WORDS 1

typedef struct __wide {
    unsigned char data [XQL_WORD_SZ];
} mywide_t;




#endif
