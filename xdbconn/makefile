ifndef XILINX_SDACCEL
ifdef XILINX_SDX
    XILINX_SDACCEL = $(XILINX_SDX)
else 
ifndef XILINX_OPENCL
    $(error XILILNX_SDX or XILINX_SDACCEL environment variable needs to be defined)
endif
endif
endif

XILINX_VIVADO_HLS = ${XILINX_SDACCEL}/Vivado_HLS/
#supported RUNMODE: sw_emu, hw_emu, hw - creates different xclbin for different modes
RUNMODE=sw_emu
#kernel language - either RTL or HLS
KRNL_LANG=hls
#Set target Xilinx device. Devices below are installed with SDAccel installation:
# XPART must be KU115 or KU3
XPART = KU3
ifeq (${XPART},KU3)
#	XDEVICE=xilinx:adm-pcie-ku3:2ddr-xpr:4.0
XDEVICE=xilinx:adm-pcie-ku3:2ddr-xpr:3.3
else ifeq (${XPART},KU115)
	XDEVICE=xilinx:xil-accel-rd-ku115:4ddr-xpr:4.0
else ifeq (${XPART},VU9P)
	XDEVICE=xilinx:aws-vu9p-f1:4ddr-xpr-2pr:4.0
endif
# HOST_DEBUG compilation DBG or OPT
HOST_CCOPT = DBG

# change these to set the proper directories where each files shoould be
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

# pick a kernel nam
KERNEL = blkloop
#KERNEL = blkpu
KERNEL_SRCS = src/${KERNEL}.cpp
KERNEL_DEFS = -k ${KERNEL}
KERNEL_INCS = src/${KERNEL}.h

KEEP_TEMP=
KERNEL_DEBUG=1
ifeq ($(DEVICE_REPO),)
    DEVICE_REPO_OPT=
else
    DEVICE_REPO_OPT= --xp prop:solution.device_repo_paths=${DEVICE_REPO}
endif

HOSTLIB_SRCS = src/oclaccel.cpp src/xdbconn.cpp
HOST_SRCS = ${HOSTLIB_SRCS}  src/main.cpp
HOST_INCS = src/oclaccel.h src/xdbconn.h src/xdbutils.h
HOST_EXE = $(BINDIR)/xdbconn
HOST_LIB = $(BINDIR)/libxdbconn.a

ifeq (${HOST_CCOPT},DBG)
	HOST_CFLAGS = -g -D HLS_DEBUG
endif
ifeq (${HOST_CCOPT},OPT)
	HOST_CFLAGS = -O3
endif
#HOST_CFLAGS += -D HW_EMU_DEBUG

HOST_CFLAGS += -DFPGA_DEVICE -fPIC -std=c++0x -Wall -Wno-unknown-pragmas -Wno-unused-label -I${XILINX_SDACCEL}/runtime/include/1_2 -I${XILINX_VIVADO_HLS}/include
HOST_LFLAGS = -lm -lpthread -lrt -ldl -L${XILINX_SDACCEL}/runtime/lib/x86_64 -lxilinxopencl
HOST_LIBS =

CC = g++
CLCC = xocc

RTL_CLCC_OPT = -l --xdevice ${XDEVICE} ${DEVICE_REPO_OPT} -o ${XCLBIN} -k ${KERNEL}_int -t ${RUNMODE} 
CLCC_OPT = --xdevice ${XDEVICE} ${DEVICE_REPO_OPT} -o ${XCLBIN} ${KERNEL_DEFS} -g
#CLCC_OPT += -D HW_EMU_DEBUG
# RUNMODE must be sw_emu, hw_emu or hw 
CLCC_OPT += -t ${RUNMODE}
XCLBIN = ${KERNEL}_${XPART}_${RUNMODE}.xclbin

HOST_ARGS += -dsa ${XDEVICE} -kernel ${KERNEL} -xclbin ${XCLBIN} 
ifeq (${RUNMODE},sw_emu)
    EMU_MODE = export XCL_EMULATION_MODE=true
    EMU_CONFIG = $(BINDIR)/emconfig.json
else ifeq (${RUNMODE},hw_emu)
    EMU_MODE = export XCL_EMULATION_MODE=true
    EMU_CONFIG = $(BINDIR)/emconfig.json
else ifeq (${RUNMODE},hw)
    EMU_MODE = echo "RUNMODE=hw"
    EMU_CONFIG = 
endif

ifeq (${KEEP_TEMP},1)
	CLCC_OPT += -s
endif
ifeq (${KERNEL_DEBUG},1)
#CLCC_OPT += -g
CLCC_OPT += --xp param:hw_em.debugLevel=GUI
endif

CLCC_OPT += --xp prop:solution.hls_pre_tcl=$(CURDIR)/hls_config.tcl

LIBOBJECTS := $(HOSTLIB_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
OBJECTS := $(HOST_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
.PHONY: all
all: host lib

run_sw_emu :
	make RUNMODE=sw_emu run

run_hw_emu :
	make RUNMODE=hw_emu run

run_hw :
	make RUNMODE=hw run
 
run : host xbin ${EMU_CONFIG}
	${EMU_MODE}; ${HOST_EXE} ${HOST_ARGS}

host: ${HOST_EXE}

lib: ${HOST_LIB}

xbin : ${XCLBIN}

$(BINDIR)/emconfig.json :
	emconfigutil --xdevice ${XDEVICE} ${DEVICE_REPO_OPT} --od $(BINDIR)

${HOST_LIB}: ${LIBOBJECTS}
	mkdir -p $(BINDIR)
	ar rvs $@ ${LIBOBJECTS}

${HOST_EXE}: ${OBJECTS}
	mkdir -p $(BINDIR)
	${CC} ${HOST_LFLAGS} ${OBJECTS} ${HOST_LIBS} -o $@

ifeq (${KRNL_LANG},rtl)
ifeq (${RUNMODE},sw_emu)
	echo "Error: sw_emu mode not supported for RTL kernels"
else
${XCLBIN}: rtl/*.v rtl/*.sv
	vivado -mode batch -source genxo.tcl -tclargs ${KERNEL} ${XPART}
	${CLCC} ${RTL_CLCC_OPT} ${KERNEL}_${XPART}_rtl.xo
endif
else
${XCLBIN}: ${KERNEL_SRCS} ${KERNEL_INCS}
	${CLCC} ${CLCC_OPT} ${KERNEL_SRCS}
endif

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp ${HOST_INCS}
	mkdir -p $(OBJDIR)
	$(CC) $(HOST_CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

clean:
	${RM} ${HOST_EXE} ${OBJECTS} ${HOST_LIB}

cleanall: clean
	rm -rf *.xo *.xclbin vivado* _xocc*.dir .Xil/ sdaccel_profile_summary.* $(BINDIR)/emconfig.json dltmp* ${BINDIR} ${OBJDIR}
