//----------------------------------------------------------------------------
// Host based emulation of q6. Operate on pages. xql6 will call this or fpga
// Page structure: nTups, {Tups}+, deadspace
// Exported fn 
//    long q6_swemu (unsigned char *p_blk);
//
// 2017-06-16 19:17:47 parik 
//----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include "q6_swemu.h"

//----------------------------------------------------------------------------
// externs
extern void hlog (const char* fmt, ...);

//----------------------------------------------------------------------------
//
static int nAttr = 16;
//                 0  1  2  3   4   5   6   7   8   9  10  11  12  13  14  15
static int attrLens [] = {8, 8, 8, 4, -1, -1, -1, -1, -1, -1,  4,  4,  4, -1, -1, -1};
static int align   [] = {0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0};


#define FIELD_SZ 16
#define DATE_SZ 4

static int off = 0;
static int woff=WORD_SZ, woffMax = WORD_SZ;

static unsigned char *p_buf = 0;
static int bufIdx = 0;

static unsigned char p_word [WORD_SZ];

int getWord ()
{
  if (bufIdx >= PAGE_SZ) return 0;

  int i;
  for (i=0; i<WORD_SZ; ++i)
    p_word [i] = p_buf [bufIdx++];

  return WORD_SZ;
}

void resetBuf ()
{
  bufIdx = 0;
  woff = WORD_SZ;
  woffMax = WORD_SZ;
}

int getByte (unsigned char* p_ch)
{
  if (woff == woffMax) {
    woffMax = getWord ();
    woff = 0;
  }
  if (woff == woffMax) {
    return 0;
  }

  *p_ch = p_word [woff];
  ++off; ++woff;
  //printf ("getByte 0x%02x\n", *p_ch);
  //std::cout << "getByte 0x" << (int)*p_ch << " " << std::hex << std::endl;
  return 1;
}

int getHeaderInt ()
{
  unsigned char ch1, ch2;
  if (!getByte (&ch1)) { exit (50); }
  if (!getByte (&ch2)) { exit (51); }
  int nTups = (ch2 << 8) | ch1; 
  return nTups;
}

int skipBytes (int fLen) 
{
  int nWordsToRead = (woff + fLen)/WORD_SZ;

  int i;
  for (i=0; i<nWordsToRead; ++i) {
    woffMax = getWord ();
  }

  off += fLen;
  woff = (woff + fLen) % WORD_SZ;
  return 1;
}

unsigned int getFieldLen (unsigned int fPtr)
{
  unsigned char ch;
  unsigned int fLen = 0;

  if (attrLens [fPtr] != -1) {
    fLen = attrLens [fPtr];

    // have to align?
    if (align [fPtr]) {
      int aoff = INTALIGN (off);
      if (aoff != off) {
        //printf ("\nalign fPtr %d off %d aoff %d\n", fPtr, off, aoff);
        int i;
        for (i=off; i< aoff; ++i) {
          if (! getByte (&ch)) { 
            return 0;
            //exit (1); 
          }
        }
      }
    }

    return fLen;
  }

  if (!getByte (&ch)) { 
    return 0;
    exit (1); 
  }

  if ( (ch & 0x03 == 0) || (ch & 0x03 == 2)) {
    //printf ("getFieldLen <1G %02x\n", ch);

    fLen = ch >> 2;
    int i;
    for (i=0; i < 3; ++i) {
      if (!getByte (&ch)) { exit (2); }
      unsigned int tmp = ch;
      fLen = fLen | tmp << i*8;
    }
    return fLen - 4;
  } else if (ch == 0x01) {
    //std::cout << "getFieldLen: Toasted Ptrs not supported at " << fPtr << std::endl;
    //exit (3);
  } else if (ch & 0x01) {
    fLen = ch >> 1;
    return fLen -1;
  } 

  //std::cout << "getFieldLen: Failed to determine field length at " << fPtr << std::endl;
  //exit (4);
}

int getField (unsigned int fLen, unsigned char fData[FIELD_SZ])
{
  unsigned char ch;
  int i;
  for (i=0; i < fLen; ++i) {
    if (!getByte (&ch)) { 
      //std::cout << "EOF" << std::endl;
      return 0;
      //exit (5); 
    }
    fData [i] = ch;
  }
  return 1;
}

long snumToLong3 (unsigned char fData[FIELD_SZ], int fsz)
{
  int bShort = (fData [1] & 0xC0) == 0x80;
  int bSign  = (fData [1] & 0x20);
  unsigned int scale = (fData [1] & 0x1F) << 1 + (fData [0] & 0x80);

  char weight = fData [0] & 0x7F;
  weight = weight | ((weight & 0x40) << 1);
  //printf ("snumToLong3 shrt %d, sign %d, scale %d, weight %d\n", bShort, bSign, scale, weight);

  long res=0;

  // 2 bytes for short-hdr. 2 bytes per btdig
  int nTotDigs  = (fsz - 2)/ 2;
  int nWhlDigs = (weight>=0)?weight+1:0;

  int nFracDigs = scale? ((scale/4)+1):0;
  int nZer0Digs = scale? 0:weight; 

  //int nDigs = nTotDigs - nFracDigs +1 + nZer0Digs;
  int nDigs = nWhlDigs + 1; // 1 Frac digit

  //printf ("snumToLong3 nTotDigs %d, nFracDigs %d, nZer0Digs %d, nDigs %d\n",
  //nTotDigs, nFracDigs, nZer0Digs, nDigs);

  int fPtr = 2;

  int i;
  for (i=nDigs-1; i>=0; --i, fPtr+=2) {
    short dig=0;
    if (fPtr < fsz) {
      dig = fData [fPtr+1] << 8  | fData [fPtr];
    }
    if (i==0) {
      res = res*100 + dig/100;
    } else {
      res = res*10000 + dig;
    }
  }

  if (bSign) return -1*res;
  else return res;
}

#define MONTHS_PER_YEAR 12
#define POSTGRES_EPOCH_JDATE   2451545 /* == date2j(2000, 1, 1) */
int date2j (int y, int m, int d)     
{
   int julian, century;

   if (m > 2) {
     m += 1;
     y += 4800;
   }
   else {
     m += 13;
     y += 4799;
   }

   century = y / 100;
   julian = y * 365 - 32167;
   julian += y / 4 - century + century / 4;
   julian += 7834 * m / 256 + d;

   return julian;
}

void j2date (int jd, int* year, int* month, int* day)
{
    unsigned int julian;
    unsigned int quad;
    unsigned int extra;
    int         y;

    julian = jd;
    julian += 32044;
    quad = julian / 146097;
    extra = (julian - quad * 146097) * 4 + 3;
    julian += 60 + quad * 3 + extra / 146097;
    quad = julian / 1461;
    julian -= quad * 1461;
    y = julian * 4 / 1461;
    julian = ((y != 0) ? ((julian + 305) % 365) : ((julian + 306) % 366))
        + 123;
    y += quad * 4;
    *year = y - 4800;
    quad = julian * 2141 / 65536;
    *day = julian - 7834 * quad / 256;
    *month = (quad + 10) % MONTHS_PER_YEAR + 1;

    return;
}

int getNumTups ()
{
  unsigned char ch1, ch2;
  getByte (&ch1);
  getByte (&ch2);
  int nTups = ch2 << 8 | ch1;
  return nTups;
}

int getJdate (unsigned char fData[FIELD_SZ])
{
  int j=0;
  int i;
  for (i=DATE_SZ-1; i>=0; --i) {
    j = j<<8 | fData [i];
  }
  return j;
}

long q6_paged ()
{
  resetBuf ();

  int nTups = getHeaderInt ();
  //std::cout << "q6_paged start. #tups " << nTups << std::endl;

  long sum = 0;
  long matches=0;
  int jd19940101 = date2j (1994, 1, 1) - POSTGRES_EPOCH_JDATE;
  int jd19950101 = date2j (1995, 1, 1) - POSTGRES_EPOCH_JDATE;
  long discountLow = 5, discountHi = 7, quantityHi = 2400; // *100
  //std::cout << "q6 jd19940101 " << jd19940101 << " jd19950101 " << jd19950101
  // << " discLo " << discountLow << " discHi " << discountHi 
  // << " qtyHi " << quantityHi << std::endl;

  unsigned char fData[FIELD_SZ];

  int tup;
  for (tup=0; tup < nTups; ++ tup) {
    off = 0;

    int shipDate = 0;
    long discount = 0;
    long quantity = 0;
    long extPrice = 0;
    int eof=0;

    unsigned int fPtr;
    for (fPtr=0; fPtr < nAttr; ++fPtr) {

      unsigned int fLen = getFieldLen (fPtr);

      if (!(fPtr ==4 || fPtr == 5 || fPtr == 6 || fPtr == 10)) {
        if (! skipBytes (fLen)) {
          //std::cout << "EOF at fPtr " << fPtr << " nTups " << nTups << std::endl;
          eof = 1;
          break;
        }
        continue;
      }

      //std::cout << "Tup [" << nTups << "] Field [" << fPtr << "] Len " << fLen << std::endl;
      if (! getField (fLen, fData)) {
        //std::cout << "EOF at fPtr " << fPtr << " nTups " << nTups << std::endl;
        eof=1;
        break;
      }

      if (fPtr==4) {
        quantity = snumToLong3 (fData, fLen);
      } else if (fPtr==5) {
        extPrice = snumToLong3 (fData, fLen);
      } else if (fPtr==6) {
        discount = snumToLong3 (fData, fLen);
      } else if (fPtr==10) {
        shipDate = getJdate (fData);
      } else {
       continue;
      } 
    }
    if (eof) break;

    if (shipDate >= jd19940101 && shipDate < jd19950101 &&
        discount >= discountLow && discount <= discountHi &&
        quantity < quantityHi) {
      sum += extPrice * discount;
      ++matches;

      //std::cout << "Matched. sum " << sum << std::endl;
      //std::cout << "Tup [" << nTups << "] " << quantity <<"|" << extPrice 
      //  << "|" << discount << "|" << shipDate << std::endl;
    }
  }
  //printf ("q6_paged %d tups, %ld matches, result %ld.%ld\n", nTups, matches, sum/(100*100), sum%(100*100));
  //return sum/(100*100);
  return sum;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------


long q6_swemu (unsigned char *p_blk)
{
  p_buf = (unsigned char*) malloc (PAGE_SZ); 
  long q6_sum = 0;
  long pg_sum = 0;

  int page;
  for (page=0; page < PAGES_PER_BLK; ++page) {
    memcpy (p_buf, p_blk, PAGE_SZ);
    p_blk += PAGE_SZ;
    hlog ("q6_paged page %d q6_sum %ld\n", page, q6_sum);

    pg_sum = q6_paged ();
    q6_sum += pg_sum;
    hlog ("q6_paged page %d pg_sum %ld q6_sum %ld\n", page, pg_sum, q6_sum);
  }

  free (p_buf);
  p_buf = 0;

  return q6_sum;
}
