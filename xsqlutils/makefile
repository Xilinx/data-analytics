#*******************************************************************************
#Vendor: Xilinx 
#Associated Filename: makefile
#Purpose: Makefile for SDAccel Compilation
#
#*******************************************************************************
#Copyright (C) 2015-2016 XILINX, Inc.
#
#This file contains confidential and proprietary information of Xilinx, Inc. and 
#is protected under U.S. and international copyright and other intellectual 
#property laws.
#
#DISCLAIMER
#This disclaimer is not a license and does not grant any rights to the materials 
#distributed herewith. Except as otherwise provided in a valid license issued to 
#you by Xilinx, and to the maximum extent permitted by applicable law: 
#(1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, AND XILINX 
#HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, 
#INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, OR 
#FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable (whether 
#in contract or tort, including negligence, or under any other theory of 
#liability) for any loss or damage of any kind or nature related to, arising under 
#or in connection with these materials, including for any direct, or any indirect, 
#special, incidental, or consequential loss or damage (including loss of data, 
#profits, goodwill, or any type of loss or damage suffered as a result of any 
#action brought by a third party) even if such damage or loss was reasonably 
#foreseeable or Xilinx had been advised of the possibility of the same.
#
#CRITICAL APPLICATIONS
#Xilinx products are not designed or intended to be fail-safe, or for use in any 
#application requiring fail-safe performance, such as life-support or safety 
#devices or systems, Class III medical devices, nuclear facilities, applications 
#related to the deployment of airbags, or any other applications that could lead 
#to death, personal injury, or severe property or environmental damage 
#(individually and collectively, "Critical Applications"). Customer assumes the 
#sole risk and liability of any use of Xilinx products in Critical Applications, 
#subject only to applicable laws and regulations governing limitations on product 
#liability. 
#
#THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT 
#ALL TIMES.
#
#*******************************************************************************
ifndef XILINX_SDX
all:
	@echo "Must define XILINX_SDX variable to a valid Xilinx SDx installation"
	exit 0
endif

CWD = ${PWD}
# HOST_DEBUG compilation DBG or OPT
HOST_CCOPT = DBG

# Location for src, obj, bin, xclbin dires
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

HOST_SRCS = src/sqlutils.cpp src/sqldate.cpp src/tbsqlunits.cpp
HOST_INCS = src/sqldefs.h src/sqlutils.h src/sqldecimal.h src/sqldecimalutils.h src/sqldate.h
HOST_EXE = $(BINDIR)/tbsqlunits

HOSTLIB_SRCS = src/sqlutils.cpp src/sqldate.cpp src/tbsqlunits.cpp
HOST_LIB = $(BINDIR)/libxsqlutils.a

ifeq (${HOST_CCOPT},DBG)
	HOST_CCFLAGS = -g -DHLS_DEBUG
endif
ifeq (${HOST_CCOPT},OPT)
	HOST_CCFLAGS = -O3
endif

# HOST_CCFLAG to compile only for host code with Kernel0 def for HLS simulation -D__USE_XOPEN2K8
HOST_CCFLAGS +=  -DFPGA_DEVICE -DVIVADO_HLS_SIM -fPIC -std=c++0x -Wall -Wno-unknown-pragmas -Wno-unused-label -I${XILINX_SDX}/runtime/include/1_2 -I${XILINX_SDX}/Vivado_HLS/include  -I ../xdbconn/src -D SQLKERNEL_ID=0
# Add these only for hw_emu debug
# HOST_CCFLAGS += -DHW_EMU_DEBUG
HOST_LFLAGS = -lm -lpthread -lrt -ldl -L${XILINX_SDX}/runtime/lib/x86_64 -lxilinxopencl
HOST_LIBS = ../xdbconn/bin/libxdbconn.a

# no kernel compilations here
CC = g++

OBJECTS := $(HOST_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
LIBOBJECTS := $(HOSTLIB_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

.PHONY: all

all: lib host

run : host 
	${HOST_EXE} ${HOST_ARGS}

host: ${HOST_EXE}

lib: ${HOST_LIB}

${HOST_LIB}: ${LIBOBJECTS}
	@echo "***** Compile library  *****"
	mkdir -p $(BINDIR)
	ar rvs $@ ${LIBOBJECTS}

${HOST_EXE}: ${OBJECTS}
	@echo "***** Compile host executable *****"
	mkdir -p $(BINDIR)
	${CC} ${HOST_LFLAGS} ${OBJECTS} ${HOST_LIBS} -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp ${HOST_INCS}
	@echo "***** Compiling $@ *****"
	mkdir -p $(OBJDIR)
	$(CC) $(HOST_CCFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

cleanall: clean 

clean:
	rm -rf ${HOST_EXE} ${OBJECTS} ${HOST_LIB} ${BINDIR} ${OBJDIR}
