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

HOST_SRCS = src/sqlutils.cpp src/sqldate.cpp src/xpgdate.cpp src/xsqlcmd.cpp src/tbsqlunits.cpp
HOST_INCS = src/sqldefs.h src/sqlutils.h src/sqldecimal.h src/sqldecimalutils.h src/xsqlcmd.h src/sqldate.h src/xpgdate.h
HOST_EXE = $(BINDIR)/tbsqlunits

HOSTLIB_SRCS = src/sqlutils.cpp src/sqldate.cpp src/xpgdate.cpp src/xsqlcmd.cpp
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
