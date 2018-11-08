//----------------------------------------------------------------------------
// Testbench for xql6
//
// 2017-06-16 18:47:18  parik 
//----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>

// reads a page worth of tuples from file and returns for kernel
class Host {
public:
  Host (FILE *fp) : mp_fp (fp) 
  {
    mp_page = new unsigned char [PAGE_SZ];
    m_pgIdx = 0;
    m_pgBeg = 0; // copy new data from here. 0->m_bufStart has left-over data
    m_off = 0;
    m_pgEnd = PAGE_SZ; // could be less for last page
  }

  ~Host () {
    delete [] mp_page;
  }

  bool getNextPage (unsigned char* p_buf) {
    unsigned char *p_dstEnd = p_buf + PAGE_SZ;
    unsigned char *p_dst = p_buf;
    p_dst += 2;

    int nBytesRd = fread (mp_page+m_pgBeg, 1, PAGE_SZ-m_pgBeg, mp_fp);
    std::cout << " fread start " << m_pgBeg << " got " << nBytesRd << std::endl;
    if (nBytesRd == 0) {
      p_buf [0] = 0;
      p_buf [1] = 0;
      return false;
    }

    m_pgEnd = m_pgBeg + nBytesRd;
    int nTups = 0;

    m_pgIdx  = 0;
    int tupBeg = m_pgIdx, tupEnd = 0;
    bool eop = false;

    while (!eop && (m_pgIdx < m_pgEnd)) {
      tupBeg = m_pgIdx;
      m_off = 0;
      int f;
      for (f=0; f < nAttr && m_pgIdx < m_pgEnd; ++f) {
        unsigned int fLen = getFieldLen (f);
        if (!fLen) {
          std::cout << "Page end. tupBeg " << tupBeg << " bufIdx " << m_pgIdx << std::endl;
          eop = true;
          break;
        }
        //std::cout << "Tup [" << nTups << "] F[" << f << "] len " << fLen << " m_off " << m_off << std::endl;
        m_pgIdx += fLen;
        m_off += fLen;
      }

      int tupLen = m_pgIdx - tupBeg; 
      if (!eop && p_dst+tupLen < p_dstEnd) {
        memcpy (p_dst, &mp_page [tupBeg], tupLen);
        p_dst += tupLen;
        tupEnd = m_pgIdx;
        //std::cout << "Tup [" << nTups << "] size " << tupLen << " m_pgIdx " << m_pgIdx 
        //          << " tupBeg " << tupBeg << std::endl;
        ++ nTups;
      } else {
        eop = true;
      }
    }

    p_buf [0] = (unsigned char) (nTups & 0xFF);
    p_buf [1] = (unsigned char) (nTups >> 8 & 0xFF);
    //std::cout << "Host: nTups " << nTups << " leftOver " << PAGE_SZ-tupBeg << " bytes. tupBeg " 
    //  << tupBeg << " tupEnd " << tupEnd << std::endl;

    if (tupBeg >= tupEnd) {
      memcpy (mp_page, &mp_page [tupBeg], PAGE_SZ-tupBeg);
      std::cout << "Host: leftOver " << tupBeg << " to " << PAGE_SZ << std::endl;
      m_pgBeg = PAGE_SZ - tupBeg;
    } else {
      m_pgBeg = 0;
    }

    std::cout << "Host::getNextPage numTups " << nTups << std::endl;
    return true;
  }

  void getBlankPage (unsigned char* p_buf) {
    p_buf [0] = 0;
    p_buf [1] = 0;
  }

private:
  bool getBufByte (unsigned char* p_ch){
    if (m_pgIdx == m_pgEnd) {
      return false;
    }
    *p_ch = mp_page [m_pgIdx];
    ++ m_pgIdx;
    ++ m_off;
    return true;
  }

  unsigned int getFieldLen (unsigned int fIdx)
  {
    unsigned char ch;
    unsigned int fLen = 0;

    if (attrLens [fIdx] != -1) {
      fLen = attrLens [fIdx];

      // have to align?
      if (align [fIdx]) {
        int aoff = INTALIGN (m_off);
        if (aoff != m_off) {
          //printf ("\nalign fIdx %d off %d aoff %d\n", fIdx, m_off, aoff);
          for (int i=m_off; i< aoff; ++i) {
            if (! getBufByte (&ch)) { 
              return 0;
              //exit (1); 
            }
          }
        }
      }

      return fLen;
    }

    if (!getBufByte (&ch)) { 
      return 0;
      exit (1); 
    }

    if ( (ch & 0x03 == 0) || (ch & 0x03 == 2)) {
      printf ("getFieldLen <1G %02x\n", ch);
      std::cout << "Host: 1G fields not supported at " << fIdx << std::endl;
      exit (3);
    } else if (ch == 0x01) {
      std::cout << "Host: Toasted Ptrs not supported at F[" << fIdx << "] m_off " << m_off << " " << (int)ch << std::endl;
      exit (3);
    } else if (ch & 0x01) {
      fLen = ch >> 1;
      //std::cout << "Host: F[" << fIdx << "] lfld " << (int)ch << " len " << fLen << " m_off " << m_off << std::endl;
      return fLen - 1;
    }

    std::cout << "Host: Failed to determine field length at " << fIdx << " " << (int)ch << std::endl;
    exit (4);
  }

private:
  FILE *mp_fp;
  unsigned char *mp_page;
  int m_pgIdx; // cur byte being processed
  int m_pgBeg; // fread new data from here. 0->m_bufStart has left-over data
  int m_pgEnd; // end of data. could be less than PAGE_SZ for last page
  int m_off;  // offset within a tup
};

class DateMgr {
public:
  DateMgr () {}
  ~DateMgr () {}

  int date2j (int y, int m, int d) {
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
};


