//----------------------------------------------------------------------------
// Testbench for xql6. To be used with csim in HLS. Not needed for sw_emu in
// SDAccel. Standalone (not integrated with postgresql) test-bench. Uses a data
// file (binary data dump of lineitem table) as input.
//
// 2017-06-16 18:47:18  parik 
//----------------------------------------------------------------------------
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
//#include <cstdint>
#include <time.h> 
#include "q6_paged.h"
#include "q6_paged_tb.h"

extern void fpga_q6 (mywide_t inbuf[NUM_WORDS], long outbuf [NUM_RES_WORDS]);

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

bool fillBlock (Host& host, unsigned char* p_bufptr, int& nBlkPages, int& nBlkTups)
{
  bool bRealPages  = true;

  for (int i=0; i<NUM_PAGES; ++i) {
    int nTups;

    if (bRealPages) {
      bRealPages = host. getNextPage (p_bufptr);
    } else {
      host. getBlankPage (p_bufptr);
    }

    nTups = (p_bufptr [1] << 8) | p_bufptr[0]; 

    std::cout << "Kernel " << (bRealPages?"Real":"Blank") 
              << " Page [" << nBlkPages 
              << "] tups " << nTups << std::endl;

    ++ nBlkPages;
    nBlkTups += nTups;
    p_bufptr += PAGE_SZ;

  }
  return bRealPages;
}

int main (int argc, char* argv[])
{
#if 0
  // hard to use in HLS due to different cwd. Hardcoding data file instead.
  if (argc != 2) {
    std::cout << "Usage tup tupdata.dat" << std::endl;
    return 100;
  }

  //std::string fnme = "tupdata.dat";
  std::string fnme (argv [1]);
#endif
  std::string fnme ("/wrk/xsjhdnobkup4/parik/pgdb/hls/host/tupdata.dat");
  FILE *fp = fopen (fnme. c_str (), "rb");
  if (!fp) {
    std::cout << "Failed to open file " << fnme << std::endl;
    return 100;
  }
  Host host (fp);


  //long nTups = parseTuples ();
  //std::cout << "Parsed " << nTups << " tuples" << std::endl;

  char sTmp[1024];  
  struct timespec tBeg, tEnd;
  clock_gettime (CLOCK_REALTIME, &tBeg);

  unsigned char *p_buf = (unsigned char*)malloc (NUM_BYTES);
  unsigned char *p_bufptr = p_buf;

  int nMaxPages = 2;

  int nTotPages=0, nTotTups=0, nBlks = 0;
  long q6_sum = 0;

  bool bRealPages = true;
  while (bRealPages) {
    int nBlkPages=0, nBlkTups=0;
    long curSum=0;
    p_bufptr = p_buf;

    bRealPages = fillBlock (host, p_bufptr, nBlkPages, nBlkTups);

    fpga_q6 ( (mywide_t*)p_buf, &curSum);
    q6_sum += curSum;

    ++ nBlks;
    nTotPages += nBlkPages;
    nTotTups += nBlkTups;

    std::cout << "Kernel Blk [" << nBlkPages << "] anyBlank? " << (!bRealPages)
              << " curSum " << curSum << " sum " << q6_sum << std::endl;

    if (nTotPages >= nMaxPages) {
      std::cout << "Reached Max Pages " << nMaxPages << std::endl;
      break;
    }
  }

  std::cout << "Total: " << nTotPages << " pages " << nTotTups << " tups " 
            << nBlks << " blks " << std::endl;
  printf ("q6_paged result %ld.%d\n", q6_sum/(100*100), q6_sum%(100*100));

  clock_gettime (CLOCK_REALTIME, &tEnd);
  sprintf (sTmp, "\nq6 %ldns\n",
   (tEnd.tv_sec - tBeg.tv_sec) * 1000000000L + tEnd.tv_nsec - tBeg.tv_nsec);
  std::cout << sTmp << std::endl;

  fclose (fp);

  return 0;
}


