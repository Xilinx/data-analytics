Xilinx End-2-End PostgreSQL Accelerator (Host)
======================

This README file contains the following sections:
  1. OVERVIEW
  2. SOFTWARE REQUIREMENTS

## 1. OVERVIEW
This library is precompiled for target system using GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-36).

## 2. SOFTWARE REQUIREMENTS

### Required Shared library:
* libpthread.so.0
* librt.so.1
* libdl.so.2
* libxilinxopencl.so
* libstdc++.so.62
* libm.so.62
* libgcc_s.so.12
* libc.so.6

### Library Compatibility:
 1. Version: 1  File: libgcc_s.so.1  Cnt: 1
   ⋅⋅* Name: GCC_3.0  Flags: none  Version: 12
 2. Version: 1  File: libpthread.so.0  Cnt: 1
   ⋅⋅* Name: GLIBC_2.2.5  Flags: none  Version: 10
 3. Version: 1  File: libstdc++.so.6  Cnt: 7
   ⋅⋅* Name: GLIBCXX_3.4.18  Flags: none  Version: 9
   ⋅⋅* Name: GLIBCXX_3.4.11  Flags: none  Version: 8
   ⋅⋅* Name: GLIBCXX_3.4.14  Flags: none  Version: 7
   ⋅⋅* Name: GLIBCXX_3.4.5  Flags: none  Version: 6
   ⋅⋅* Name: CXXABI_1.3  Flags: none  Version: 5
   ⋅⋅* Name: GLIBCXX_3.4.19  Flags: none  Version: 4
   ⋅⋅* Name: GLIBCXX_3.4  Flags: none  Version: 3
 4. Version: 1  File: libc.so.6  Cnt: 2
   ⋅⋅* Name: GLIBC_2.14  Flags: none  Version: 11
   ⋅⋅* Name: GLIBC_2.2.5  Flags: none  Version: 2
