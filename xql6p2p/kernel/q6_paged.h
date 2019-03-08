//----------------------------------------------------------------------------
// xql6 kernel.
//
// 2017-06-16 18:48:27 parik 
//----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <hls_stream.h>

// define for csim in HLS
//#define HLS_SIM

const int g_dbgLvl = 0;
#define Dbg0 if(g_dbgLvl)
#define Dbg1 if(g_dbgLvl > 1)
#define Dbg2 if(g_dbgLvl > 2)

//----------------------------------------------------------------------------

// Schema info TPCH lineitem table. len=-1 => variable length
const int nAttr = 16;
const int attrLens [] = {8, 8, 8, 4, -1, -1, -1, -1, -1, -1,  4,  4,  4, -1, -1, -1};
const bool align   [] = {0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0};

#define NMRC_FIELD_SZ 16

#define ALIGNOF_INT 4

// aligns the offset to the next nearest size boundary
// TODO add align code for other types as well.
//typedef unsigned int uintptr_t;
#define TYPEALIGN(ALIGNVAL,LEN)  \
          (((unsigned int) (LEN) + ((ALIGNVAL) - 1)) & ~((unsigned int) ((ALIGNVAL) - 1)))

#define INTALIGN(LEN)  TYPEALIGN(ALIGNOF_INT, (LEN))

#define MONTHS_PER_YEAR 12
#define POSTGRES_EPOCH_JDATE   2451545 /* == date2j(2000, 1, 1) */

//----------------------------------------------------------------------------
// TPCH Q6 scan and aggregate criteria

//int jd19940101 = -2191; //date2j (1994, 1, 1) - POSTGRES_EPOCH_JDATE;
#define jd19940101 -2191
//int jd19950101 = -1826; //date2j (1995, 1, 1) - POSTGRES_EPOCH_JDATE;
#define jd19950101 -1826
//long discountLow = 5, discountHi = 7, quantityHi = 2400; // *100
#define discountLow 5
#define discountHi 7
#define quantityHi 2400

//----------------------------------------------------------------------------
// Kernel capacity and memory access

#define WORD_SZ 16
#define PAGE_SZ 8192                          // 8KB
#define WORDS_PER_PAGE (PAGE_SZ/WORD_SZ)      // 512

#define NUM_BYTES (2*1024*1024)               // 2Mb
#define NUM_WORDS (NUM_BYTES/WORD_SZ)         // 131072 words
#define NUM_PAGES (NUM_WORDS/WORDS_PER_PAGE)  // 256 pages

#define NUM_PU 32
#define PAGES_PER_PU (NUM_PAGES/NUM_PU)       // 1 pages/pu

#define NUM_RES_WORDS 1

#define DATE_SZ 4

typedef struct __wide {
    unsigned char data [WORD_SZ];
} mywide_t;

typedef struct __fdata {
  unsigned char data [NMRC_FIELD_SZ];
} fdata_t;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
 * Tuple parser. Unpacks Postgres native tuple format. Refer to Postgres
 * sources for more info.
 */
class TupParser {
private:
  int off;    // offset in tuple
  int pgOff;  // offset in a page

public:
#if 0
  TupParser ()
    : off (0), pgOff (2)
  {}
#else
  TupParser ()
    : off (0), pgOff (0)
  {}
#endif

  ~TupParser () {}

  void resetBuf () {
    //pgOff = 2;    // ignore the header bytes
    pgOff = 0;    // ignore the header bytes
  }

  void startTup () {
    off = 0;
  }

  unsigned char getByte (hls::stream <unsigned char>& stream) {
#ifdef HLS_SIM
    if (pgOff >= PAGE_SZ) {
      std::cout << "getByte OOB " << pgOff << std::endl;
    }
#endif
    ++ pgOff;
    return stream. read ();
  }

  int getHeaderInt (hls::stream <unsigned char>& stream) {
    unsigned char ch1, ch2;
    ch1 = getByte (stream);
    ch2 = getByte (stream);
    //if (!b1 || !b2) std::cout << "getHeaderInt failed b1 " << b1 << " b2 " << b2 << std::endl;
    int nTups = (ch2 << 8) | ch1; 
    return nTups;
  }

  unsigned int getFieldLen (unsigned int fPtr, hls::stream <unsigned char>& stream) {
    unsigned char ch;
    unsigned int fLen = 0;

    if (attrLens [fPtr] != -1) {
      fLen = attrLens [fPtr];

      // have to align?
      if (align [fPtr]) {
        int aoff = INTALIGN (off);
        if (aoff != off) {
          Dbg0 printf ("\nalign fPtr %d off %d aoff %d\n", fPtr, off, aoff);
          for (int i=off; i< aoff; ++i) {
//#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=0 max=1 avg=0
            ch =  getByte (stream);
            ++ off;
            //if (!b1) std::cout << "getByte failed at align" << std::endl;
          }
        }
      }

      return fLen;
    }

    ch = getByte (stream);
    ++ off;
    // no toast, no long field
    if (ch & 0x01) {
      fLen = ch >> 1;
      return fLen -1;
    } else {
      std::cout << "getFieldLen: Failed to determine field length at " << fPtr << std::endl;
      exit (10);
      return 0;
    }
  }

  // read more than 1 byte from rdBuf TODO
  bool getField (unsigned char fData[NMRC_FIELD_SZ], unsigned int fLen, hls::stream <unsigned char>& stream ) {
#pragma HLS ARRAY_PARTITION variable=fData complete dim=0
    unsigned char ch;
    for (int i=0; i < fLen; ++i) {
//#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=4 max=16 avg=8
      ch = getByte (stream);
      //if (!b1) std::cout << "getField failed at i=" << i << std::endl;
      fData [i] = ch;
    }
    off += fLen;
    return true;
  }

  bool skipBytes (int fLen, hls::stream <unsigned char>& stream) {
    off += fLen;
    for (int i=0; i<fLen; ++i) {
#pragma HLS LOOP_TRIPCOUNT min=4 max=16 avg=8
      unsigned char ch;
      ch = getByte (stream);
    }
    return true;
  }

  void skipToPageEnd (hls::stream <unsigned char>& stream) {
    //m_rdBuf. skipToPageEnd (stream);
    for (int i = pgOff; i < PAGE_SZ; ++i) {
#pragma HLS LOOP_TRIPCOUNT min=4 max=16 avg=8
      unsigned char ch;
      ch = getByte (stream);
    }
  }
};
