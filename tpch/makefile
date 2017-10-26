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
# Set this linking with PostgreSQL or MyDataSQL using C-API. Default flow is using csv file.

SQLTBL=../../db/tpch-master/dbgen/1/lineitem.tbl

# only csv is supported. Make code/makefile change to support pgsql or mysql  see sqlhost.cpp
DBMODE=csv
ifeq (${DBMODE},pgsql)
   PGSERVER=xsjsda7
   DB_LIBS = /wrk/hdstaff/hare/sqldb
endif

CWD = ${PWD}
#supported flow: sw_emu, hw_emu, hw
FLOW=hw
# must be 1,6 or tablename for loopback
QUERYNO = 6
# possible kernels are sqlq6tpch sqlpageloop sqlq1tpch
ifeq (${QUERYNO},1)
   KERNEL = sqlq1tpch
else ifeq (${QUERYNO},6)
   KERNEL = sqlq6tpch
else
   KERNEL = sqlpageloop
endif
# XPART must be KU115 or KU3 or VU9P or AWS
XPART = AWS
# Kernels no of kernels
KERNELS = 1
KERNEL0 = ${KERNEL}0
KERNEL1 = ${KERNEL}1
KERNEL2 = ${KERNEL}2
KERNEL3 = ${KERNEL}3

#set DSA device based on the part chosen
ifeq (${XPART},KU3)
	XDEVICE=xilinx:adm-pcie-ku3:2ddr-xpr:4.0
else ifeq (${XPART},KU115)
	XDEVICE=xilinx:xil-accel-rd-ku115:4ddr-xpr:4.0
else ifeq (${XPART},VU9P)
	XDEVICE=xilinx:xil-accel-rd-vu9p:4ddr-xpr:4.1
else ifeq (${XPART},AWS)
	XDEVICE=xilinx:aws-vu9p-f1:4ddr-xpr-2pr:4.0
endif
# HOST_DEBUG compilation DBG or OPT
HOST_CCOPT = DBG

# Location for src, obj, bin, xclbin dires
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

HOST_SRCS = src/sqlutils.cpp src/sqldate.cpp src/sqlq6procunit.cpp src/sqlq6tpch.cpp src/sqlq1tpch.cpp src/sqlpageloop.cpp src/sqlq1procunit.cpp src/sqltbl.cpp 

# set to 0 if sqlunit test to be enabled
HOST_MAIN=1
ifeq (${HOST_MAIN},1)
	HOST_SRCS += src/q1main.cpp src/q6main.cpp src/main.cpp
	HOST_EXE = $(BINDIR)/sqltpch
else
	HOST_SRCS += src/tbsqlunits.cpp
	HOST_EXE = $(BINDIR)/tbsqlunits
endif
HOST_INCS = src/sqltbl.h src/sqlutils.h src/sqlq6defs.h src/sqlq1defs.h src/sqlq1procunit.h src/sqlq6procunit.cpp src/sqlpageloop.h src/sqldecimal.h src/sqldecimalutils.h src/sqldate.h
KERNEL_INCS = 

ifeq (${HOST_CCOPT},DBG)
	HOST_CCFLAGS = -g -DHLS_DEBUG
endif
ifeq (${HOST_CCOPT},OPT)
	HOST_CCFLAGS = -O3
endif

# HOST_CCFLAG to compile only for host code with Kernel0 def for HLS simulation -D__USE_XOPEN2K8
HOST_CCFLAGS +=  -DFPGA_DEVICE -DVIVADO_HLS_SIM -fPIC -std=c++0x -Wall -Wno-unknown-pragmas -Wno-unused-label -I${XILINX_SDX}/runtime/include/1_2 -I${XILINX_SDX}/Vivado_HLS/include  -I ../xdbconn/src -D SQLKERNEL_ID=0
HOST_LFLAGS = -lm -lpthread -lrt -ldl -L${XILINX_SDX}/runtime/lib/x86_64 -lxilinxopencl
HOST_LIBS = ../xdbconn/bin/libxdbconn.a

ifeq (${DBMODE},pgsql)
	HOST_CCFLAGS +=  -I ${DB_LIBS}/mysql-connector-c-6.1.6-linux-glibc2.5-x86_64/include/ -I ${DB_LIBS}/pgsql/include/
	HOST_LFLAGS += -L${XILINX_SDX}/runtime/lib/x86_64 -L ${DB_LIBS}/pgsql/lib -lpq
	HOST_LIBS += ${DB_LIBS}/mysql-connector-c-6.1.6-linux-glibc2.5-x86_64/lib/libmysqlclient.a
endif

# sqlq6tpch kernel
ifeq (${KERNEL},sqlq6tpch)
        KERNEL0_SRCS = src/sqlq6procunit.cpp src/sqldate.cpp src/sqlq6tpch.cpp
	KERNEL0_INCS = src/sqldecimal.h src/sqldate.h src/sqldefs.h src/sqlq6defs.h src/sqlq6tpch.h
# sqlq1tpch kernel
else ifeq (${KERNEL},sqlq1tpch)
        KERNEL0_SRCS = src/sqlq1procunit.cpp src/sqldate.cpp src/sqlq1tpch.cpp
	KERNEL0_INCS = src/sqldecimal.h src/sqldate.h src/sqldefs.h src/sqlq1defs.h src/sqlq1tpch.h
# sqlpageloop kernel
else ifeq (${KERNEL},sqlpageloop)
	KERNEL0_SRCS = src/sqlpageloop.cpp
	KERNEL0_INCS = src/sqlpageloop.h
endif
KERNEL0_CLCC_OPT = -D SQLKERNEL_ID=0 -k ${KERNEL0}
KERNEL_CU_OPT = --nk ${KERNEL0}:1:${KERNEL0}
ifeq ($(KERNELS),2)
	BLD_KERNEL1 = 1
endif
ifeq ($(KERNELS),3)
	BLD_KERNEL1 = 1
	BLD_KERNEL2 = 1
endif
ifeq ($(KERNELS),4)
	BLD_KERNEL1 = 1
	BLD_KERNEL2 = 1
	BLD_KERNEL3 = 1
endif
ifeq ($(BLD_KERNEL1),1)
	KERNEL_CU_OPT += --nk ${KERNEL1}:1:${KERNEL1} 
	KERNEL1_SRCS = ${KERNEL0_SRCS}
	KERNEL1_CLCC_OPT = -D SQLKERNEL_ID=1 -k ${KERNEL1}
	KERNEL1_INCS = ${KERNEL0_INCS}
endif
ifeq ($(BLD_KERNEL2),1)
	KERNEL_CU_OPT += --nk ${KERNEL2}:1:${KERNEL2} 
	KERNEL2_SRCS = ${KERNEL0_SRCS}
	KERNEL2_CLCC_OPT = -D SQLKERNEL_ID=2 -k ${KERNEL2}
	KERNEL2_INCS = ${KERNEL0_INCS}
endif
ifeq ($(BLD_KERNEL3),1)
	KERNEL_CU_OPT += --nk ${KERNEL3}:1:${KERNEL3} 
	KERNEL3_SRCS = ${KERNEL0_SRCS}
	KERNEL3_CLCC_OPT = -D SQLKERNEL_ID=3 -k ${KERNEL3}
	KERNEL3_INCS = ${KERNEL0_INCS}
endif

ifeq ($(DEVICE_REPO),)
    DEVICE_REPO_OPT=
else
    DEVICE_REPO_OPT= --xp prop:solution.device_repo_paths=${DEVICE_REPO}
endif

KEEP_TEMP=1
KERNEL_DEBUG=1
CC = g++
CLCC = xocc
CLCC_OPT = --xdevice ${XDEVICE} ${DEVICE_REPO_OPT} --xp param:compiler.enableAutoPipelining=false -O0
# FLOW must be sw_emu, hw_emu or hw 
CLCC_OPT += -t ${FLOW}
XCLBIN = ${KERNEL}_${XPART}_${FLOW}.xclbin
KERNEL0_OBJ = ${KERNEL0}_${XPART}_${FLOW}.xo
# Build kernel1 only if 2 kernels are needed
ifeq (${BLD_KERNEL1},1)
	KERNEL1_OBJ = ${KERNEL1}_${XPART}_${FLOW}.xo
endif
ifeq (${BLD_KERNEL2},1)
	KERNEL2_OBJ = ${KERNEL2}_${XPART}_${FLOW}.xo
endif
ifeq (${BLD_KERNEL3},1)
	KERNEL3_OBJ = ${KERNEL3}_${XPART}_${FLOW}.xo
endif

ifeq (${KEEP_TEMP},1)
	CLCC_OPT += -s 
endif
ifeq (${KERNEL_DEBUG},1)
	CLCC_OPT += -g
endif
CLCC_OPT += --include ${CWD}/src
CLCC_OPT += --xp prop:solution.hls_pre_tcl=$(CURDIR)/hls_config.tcl

CLCC_LINK_OPT = ${KERNEL_CU_OPT}

# Set constraint file
CONSTRAINTFILE=$(CURDIR)/data/${KERNEL}_${KERNELS}kernel.xdc

#todo clean vivado props and params
ifeq (${XPART},KU115)
# ifeq (${BLD_KERNEL1},1) todo
ifeq (${FLOW},hw)
	# allow for  up to 100ps on mmcm and system clocks
	CLCC_OPT += --xp param:compiler.worstNegativeSlack=-0.1

	# limit control net fanout
	# set SLR level pblocks for kernels
	XP_VIVADO_PROPS =--xp vivado_prop:run.impl_1.STEPS.OPT_DESIGN.TCL.PRE=$(CONSTRAINTFILE)
	# disable brampoweropt in opt_design
	XP_VIVADO_PROPS +=--xp vivado_prop:run.impl_1.STEPS.OPT_DESIGN.ARGS.DIRECTIVE=NoBramPowerOpt


	# enable explore directive
	XP_VIVADO_PROPS +=--xp vivado_prop:run.impl_1.STEPS.PLACE_DESIGN.ARGS.DIRECTIVE=Explore
	XP_VIVADO_PROPS +=--xp vivado_prop:run.impl_1.STEPS.PHYS_OPT_DESIGN.IS_ENABLED=true
	XP_VIVADO_PROPS +=--xp vivado_prop:run.impl_1.STEPS.PHYS_OPT_DESIGN.ARGS.DIRECTIVE=Explore
	XP_VIVADO_PROPS +=--xp vivado_prop:run.impl_1.STEPS.ROUTE_DESIGN.ARGS.DIRECTIVE=Explore


	# verbose messages in place and route
	XP_VIVADO_PARAMS =--xp vivado_param:project.writeIntermediateCheckpoints=1
	XP_VIVADO_PARAMS+=--xp vivado_param:place.hardVerbose=469538
	XP_VIVADO_PARAMS+=--xp vivado_param:place.oldMsgVerbose=1
	XP_VIVADO_PARAMS+=--xp vivado_param:route.flowDbg=1
	XP_VIVADO_PARAMS+=--xp vivado_param:route.timingDbg=1

	CLCC_LINK_OPT += $(XP_VIVADO_PARAMS) $(XP_VIVADO_PROPS)
endif
endif

ifeq (${FLOW},sw_emu)
    HOST_ARGS += -hw -kernel ${KERNEL} -xclbin ${XCLBIN} 
    CLCC_OPT += -D VIVADO_HLS_SIM
    EMU_MODE = export XCL_EMULATION_MODE=true
    EMU_CONFIG = $(BINDIR)/emconfig.json
else ifeq (${FLOW},hw_emu)
    HOST_ARGS += -hw -kernel ${KERNEL} -xclbin ${XCLBIN} 
    CLCC_OPT += -D VIVADO_HLS_SIM
    EMU_MODE = export XCL_EMULATION_MODE=true
    EMU_CONFIG = $(BINDIR)/emconfig.json
else ifeq (${FLOW},hw)
    HOST_ARGS += -hw -kernel ${KERNEL} -xclbin ${XCLBIN} 
    EMU_MODE = echo "FLOW=hw"
    EMU_CONFIG = 
endif

HOST_ARGS += -query ${QUERYNO} -dsa ${XDEVICE}
ifeq (${DBMODE},pgsql)
    HOST_SRCS += src/sqlhost.cpp
    HOST_INCS += src/sqlhost.cpp
    HOST_ARGS += -db pgsql -schema sf1g -sqlhost ${PGSERVER} -sqlport 5432
else
    HOST_ARGS += -db ${SQLTBL}
endif

OBJECTS := $(HOST_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

.PHONY: all

all: host

run_cpu_int : host
	${HOST_EXE} ${HOST_ARGS}

run_cpu :
	make FLOW=cpu run_cpu_int

run_hls_int : host
	${HOST_EXE} ${HOST_ARGS}

run_hls :
	make FLOW=hls run_hls_int

run_sw_emu :
	make FLOW=sw_emu FLOW=sw_emu run

run_hw_emu :
	make FLOW=hw_emu FLOW=hw_emu run

run_hw :
	make FLOW=hw FLOW=hw run

run : host xbin ${EMU_CONFIG} ${KERNEL_INCS}
	${EMU_MODE}; ${HOST_EXE} ${HOST_ARGS}

host: ${HOST_EXE}

xbin : ${XCLBIN}

$(BINDIR)/emconfig.json :
	emconfigutil --xdevice ${XDEVICE} ${DEVICE_REPO_OPT} --od $(BINDIR)

${HOST_EXE}: ${OBJECTS}
	@echo "***** Compile host executable *****"
	mkdir -p $(BINDIR)
	${CC} ${HOST_LFLAGS} ${OBJECTS} ${HOST_LIBS} -o $@

${XCLBIN}: ${KERNEL0_OBJ} ${KERNEL1_OBJ} ${KERNEL2_OBJ} ${KERNEL3_OBJ}
	@echo "************* Compile XCLBIN ${XCLBIN} *************"
	@echo "FLOW = ${FLOW}"
	@echo "KERNELS = ${KERNELS}"
	@echo "Using constraint file $(CONSTRAINTFILE)"
	${CLCC} -l ${CLCC_OPT} ${CLCC_LINK_OPT} $^ -o $@

${KERNEL0_OBJ} : ${KERNEL0_SRCS} ${KERNEL0_INCS}
	@echo "***** Compiling $@ *****"
	${CLCC} -c ${CLCC_OPT} ${KERNEL0_CLCC_OPT} -o $@ ${KERNEL0_SRCS}

${KERNEL1_OBJ} : ${KERNEL1_SRCS} ${KERNEL1_INCS}
	@echo "***** Compiling $@ *****"
	${CLCC} -c ${CLCC_OPT} ${KERNEL1_CLCC_OPT} -o $@ ${KERNEL1_SRCS}

${KERNEL2_OBJ} : ${KERNEL2_SRCS} ${KERNEL2_INCS}
	@echo "***** Compiling $@ *****"
	${CLCC} -c ${CLCC_OPT} ${KERNEL2_CLCC_OPT} -o $@ ${KERNEL2_SRCS}

${KERNEL3_OBJ} : ${KERNEL3_SRCS} ${KERNEL3_INCS}
	@echo "***** Compiling $@ *****"
	${CLCC} -c ${CLCC_OPT} ${KERNEL3_CLCC_OPT} -o $@ ${KERNEL3_SRCS}

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp ${HOST_INCS}
	@echo "***** Compiling $@ *****"
	mkdir -p $(OBJDIR)
	$(CC) $(HOST_CCFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

cleanall: clean cleanxbin

clean:
	${RM} ${HOST_EXE} ${OBJECTS}

cleanxbin:
	${RM} -r ${XCLDIR}/*.xclbin _xocc*.dir .Xil/ *.xclbin *.xo sdaccel_profile_summary.* $(BINDIR)/emconfig.json


