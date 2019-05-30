# Copyright (C) 2019, Xilinx Inc - All rights reserved
#
# Licensed under the Apache License, Version 2.0 (the "License"). You may
# not use this file except in compliance with the License. A copy of the
# License is located at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.

ifndef XILINX_SDX
  $(error Must define XILINX_SDX variable to a valid Xilinx SDx installation)
endif

ifndef USER_SCRIPT_PATH
  $(error USER_SCRIPT_PATH variable is not set)
endif

CWD = ${PWD}
#supported flow: sw_emu, hw_emu, hw
FLOW=hw
# possible runmode are - cpu, sw_emu, hw_emu, hw
RUNMODE=hw
# possible kernels are scanaggr
KERNEL = omx
XPART = U280
# Kernels no of kernels
KERNELS = 1
KERNEL0 = ${KERNEL}
KERNEL1 = ${KERNEL}1
# must be 1 or 6
#
HOSTQUERYARG = 6

#set DSA device based on the part chosen
ifeq (${XPART},U280)
    XDEVICE=xilinx_u280-es1_xdma_201830_1.xpfm
else ifeq (${XPART},VU9P)
	XDEVICE=xilinx:xil-accel-rd-vu9p:4ddr-xpr:4.1
else ifeq (${XPART},AWS)
	XDEVICE=xilinx:aws-vu9p-f1:4ddr-xpr-2pr:4.0
endif
# HOST_DEBUG compilation DBG or OPT
HOST_CCOPT = DBG
HOST_CFLAGS+=-DTARGET_DEVICE=\"${XDEVICE}\"
#NDDR=4ch or 2ch can choose whether is 4 channel or 2channel design.
NDDR = 16
#choose to run simulation with smaller data size 
SMALL_SIZE = 0

# Location for src, obj, bin, xclbin dires
SRCDIR   = src
KNLDIR   = kernel
OBJDIR   = obj
BINDIR   = bin

HOST_EXE = $(BINDIR)/omx
HOST_SRCS += ${SRCDIR}/oclaccel.cpp ${SRCDIR}/main.cpp 
HOST_INCS = ${SRCDIR}/oclaccel.h
HOSTLIB_SRCS = 
HOST_LIB = $(BINDIR)/libomx.a


KERNEL_INCS = 

ifeq (${HOST_CCOPT},DBG)
	HOST_CCFLAGS = -g
endif
ifeq (${HOST_CCOPT},OPT)
	HOST_CCFLAGS = -O3
endif

# HOST_CCFLAG to compile only for host code with Kernel0 def for HLS simulation -D__USE_XOPEN2K8
#HOST_CCFLAGS +=  -DFPGA_DEVICE -DVIVADO_HLS_SIM -fPIC -std=c++0x -Wall -Wno-unknown-pragmas -Wno-unused-label -I${XILINX_SDX}/runtime/include/1_2 -I./kernel -I${XILINX_SDX}/Vivado_HLS/include
HOST_CCFLAGS +=  -DFPGA_DEVICE -DVIVADO_HLS_SIM -fPIC -std=c++0x -Wall -Wno-unknown-pragmas -Wno-unused-label -I${XILINX_XRT}/include/ -L${XILINX_XRT}/lib -I./kernel -I${XILINX_SDX}/Vivado_HLS/include

HOST_CCFLAGS += -DDSA64 -DHAL2

ifeq (${SMALL_SIZE},1)
HOST_CCFLAGS +=  -DSMALL_SIZE
endif

# Add these only for hw_emu debug
#HOST_CCFLAGS += -DHW_EMU_DEBUG 
#HOST_LFLAGS = -lm -lpthread -lrt -ldl -L${XILINX_SDX}/runtime/lib/x86_64 -lxilinxopencl -g
HOST_LFLAGS = -lm -lpthread -lrt -ldl -I${XILINX_XRT}/include -L/usr/lib/x86_64-linux-gnu   -L${XILINX_XRT}/lib -lxilinxopencl -lxrt_core -g

HOST_LIBS = ${HOST_LIB} 

# kernel sources
KERNEL0_SRCS = ${KNLDIR}/gather.cpp ${KNLDIR}/omx.cpp
KERNEL0_INCS = ${KNLDIR}/omx.h
KERNEL0_CLCC_OPT = -D SQLKERNEL_ID=0 -k ${KERNEL0}
ifeq ($(KERNELS),1)
	KERNEL_CU_OPT = --nk ${KERNEL0}:1:${KERNEL0}
	KERNEL1_SRCS = 
	KERNEL1_CLCC_OPT = 
	KERNEL1_INCS = 
else
	KERNEL_CU_OPT = --nk ${KERNEL0}:1:${KERNEL0} --nk ${KERNEL1}:1:${KERNEL1} 
	KERNEL1_SRCS = ${KERNEL0_SRCS}
	KERNEL1_CLCC_OPT = -D SQLKERNEL_ID=1 -k ${KERNEL1}
	KERNEL1_INCS = ${KERNEL0_INCS}
endif


PROFILE = 0
KEEP_TEMP=1
KERNEL_DEBUG=1
CC = g++
CLCC = xocc
CLCC_OPT =-t ${FLOW} -s --platform ${XDEVICE}  --xp param:compiler.enableAutoPipelining=false -g  -O0


CLCC_OPT +=--sp omx.inbuf0:HBM[16] \
	   --sp omx.inbuf1:HBM[17] \
	   --sp omx.outbuf0:HBM[18] \
	   --sp omx.outbuf1:HBM[19] \
	   --sp omx.ddr0:HBM[0:15] \
           --sp omx.ddr1:HBM[0:15] \
           --sp omx.ddr2:HBM[0:15] \
           --sp omx.ddr3:HBM[0:15] \
           --sp omx.ddr4:HBM[0:15] \
           --sp omx.ddr5:HBM[0:15] \
           --sp omx.ddr6:HBM[0:15] \
           --sp omx.ddr7:HBM[0:15] \
	   --sp omx.ddr8:HBM[0:15] \
           --sp omx.ddr9:HBM[0:15] \
           --sp omx.ddr10:HBM[0:15] \
           --sp omx.ddr11:HBM[0:15] \
           --sp omx.ddr12:HBM[0:15] \
           --sp omx.ddr13:HBM[0:15] \
           --sp omx.ddr14:HBM[0:15] \
           --sp omx.ddr15:HBM[0:15]



CLCC_OPT  += --xp param:compiler.userPostSysLinkTcl=${USER_SCRIPT_PATH}/user_tcl_file.tcl




ifeq (${SMALL_SIZE},1)
	CLCC_OPT += -D SMALL_SIZE
endif


# Use only for debug
#CLCC_OPT += -DHW_EMU_DEBUG -g --xp param:hw_em.enableProtocolChecker=true
# FLOW must be sw_emu, hw_emu or hw 
XCLBIN = ${KERNEL}_${XPART}_${FLOW}_${NDDR}.xclbin
KERNEL0_OBJ = ${KERNEL0}_${XPART}_${FLOW}_${NDDR}.xo
# Build kernel1 only if 2 kernels are needed
ifeq (${KERNELS},2)
	KERNEL1_OBJ = ${KERNEL1}_${XPART}_${FLOW}.xo
endif

CLCC_OPT += --include ${CWD}/kernel --include ${CWD}/src --include ${CWD}/../xsqlutils/src
#CLCC_OPT += --xp prop:solution.hls_pre_tcl=$(CURDIR)/hls_config.tcl

CLCC_LINK_OPT = ${KERNEL_CU_OPT}
# Set constraint file
#CONSTRAINTFILE=$(CURDIR)/data/${KERNEL}_${KERNELS}kernel.xdc
CONSTRAINTFILE=

#todo clean vivado props and params
ifeq (${FLOW},hw)
	# allow for  up to 100ps on mmcm and system clocks
	CLCC_OPT += --xp param:compiler.worstNegativeSlack=-0.1

	# limit control net fanout
	# set SLR level pblocks for kernels
	#XP_VIVADO_PROPS =--xp vivado_prop:run.impl_1.STEPS.OPT_DESIGN.TCL.PRE=$(CONSTRAINTFILE)
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
ifeq ($(PROFILE),1)
	CLCC_OPT += --profile_kernel stall:all:all:all
	CLCC_LINK_OPT += --profile_kernel data:all:all:all:all --profile_kernel exec:all:all:all --profile_kernel stall:all:all:all
endif
endif

ifeq (${RUNMODE},cpu)
    HOST_ARGS += -cpu
else ifeq (${RUNMODE},sw_emu)
    HOST_ARGS += -hw -kernel ${KERNEL} -xclbin ${XCLBIN} 
    CLCC_OPT += -D VIVADO_HLS_SIM
    EMU_MODE = export XCL_EMULATION_MODE=true
    EMU_CONFIG = $(BINDIR)/emconfig.json
else ifeq (${RUNMODE},hw_emu)
    HOST_ARGS += -hw -kernel ${KERNEL} -xclbin ${XCLBIN} 
    CLCC_OPT += -D VIVADO_HLS_SIM
    EMU_MODE = export XCL_EMULATION_MODE=true
    EMU_CONFIG = $(BINDIR)/emconfig.json
else ifeq (${RUNMODE},hw)
    HOST_ARGS += -hw -kernel ${KERNEL} -xclbin ${XCLBIN} 
    EMU_MODE = echo "RUNMODE=hw"
    EMU_CONFIG = 
endif

#HOST_ARGS += -query ${HOSTQUERYARG} -var test/s1sf.var -ntuples 6000 -eventsync yes
HOST_ARGS += -dsa ${XDEVICE}

OBJECTS := $(HOST_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
LIBOBJECTS := $(HOSTLIB_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

.PHONY: all

all: host lib

run_cpu_int : host
	${HOST_EXE} ${HOST_ARGS}

run_cpu :
	make RUNMODE=cpu run_cpu_int

run_hls_int : host
	${HOST_EXE} ${HOST_ARGS}

run_hls :
	make RUNMODE=hls run_hls_int

run_sw_emu :
	make RUNMODE=sw_emu FLOW=sw_emu run

run_hw_emu :
	make RUNMODE=hw_emu FLOW=hw_emu run

run_hw :
	make RUNMODE=hw FLOW=hw run

run : host xbin ${EMU_CONFIG} ${KERNEL_INCS}
	${EMU_MODE}; 
	${HOST_EXE} ${HOST_ARGS}


host: ${HOST_EXE}

lib: ${HOST_LIB}

xbin : ${XCLBIN}

${HOST_LIB}: ${LIBOBJECTS}
	mkdir -p $(BINDIR)
	ar rvs $@ ${LIBOBJECTS}

$(BINDIR)/emconfig.json :
	emconfigutil --platform ${XDEVICE} --od $(BINDIR)

${HOST_EXE}: ${OBJECTS} ${HOST_LIBS}
	@echo "***** Compile host executable *****"
	mkdir -p $(BINDIR)
	${CC} ${HOST_LFLAGS} ${OBJECTS} ${HOST_LIBS} -o $@

${XCLBIN}: ${KERNEL0_OBJ} ${KERNEL1_OBJ}
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

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp ${HOST_INCS}
	@echo "***** Compiling $@ *****"
	mkdir -p $(OBJDIR)
	$(CC) $(HOST_CCFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(LIBOBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp ${HOST_INCS}
	@echo "***** Compiling $@ *****"
	mkdir -p $(OBJDIR)
	$(CC) $(HOST_CCFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

cleanall: clean cleanxbin

clean:
	${RM} ${HOST_EXE} ${OBJECTS} ${HOST_LIB} ${LIBOBJECTS}

cleanxbin:
	${RM} -rf ${XCLDIR}/*.xclbin _x/ .Xil/ *.xclbin *.xo sdaccel_profile_summary.*  dltmp* ${BINDIR} ${OBJDIR}
