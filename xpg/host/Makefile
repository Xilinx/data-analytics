#MODULE_big = xpgbolt
PG_CONFIG := pg_config
VPATH = /usr/lib/postgresql/9.6/lib/pgxs/src

###############################################################################
## Version
###############################################################################
XPG_VERSION=0.1
ifndef XILINX_SDACCEL
	ifdef XILINX_SDX
		XILINX_SDACCEL = $(XILINX_SDX)
	else
		# Default to 2016.3
		XILINX_SDACCEL = /proj/xbuilds/2016.4_sdx_daily_latest/installs/lin64/SDx/2016.4
	endif
endif
GPP = g++
CC = gcc
OBJDIR=obj
LDFLAGS = -shared

# HOST_CCFLAG to compile only for host code with Kernel0 def for HLS simulation -D__USE_XOPEN2K8
HOST_CCFLAGS +=  -DHW_EMU_DEBUG -D SQLKERNEL_ID=0 -g -DHLS_DEBUG -DFPGA_DEVICE -DVIVADO_HLS_SIM -fPIC -std=c++0x -Wall -Wno-unknown-pragmas -Wno-unused-label -I${XILINX_SDACCEL}/runtime/include/1_2 -I${XILINX_SDACCEL}/Vivado_HLS/include  -D SQLKERNEL_ID=0 -I/usr/include/mit-krb5 -I../../xdbconn/src -I../../xsqlutils/src -fPIC -pie -fno-omit-frame-pointer -fpic -I. -I./ -I/usr/include/postgresql/9.6/server -I/usr/include/postgresql/internal -D_FORTIFY_SOURCE=2 -D_GNU_SOURCE -I/usr/include/libxml2  -I/usr/include/tcl8.6 -I/usr/local/boost -DHW_INTEGRATION  #-- for building HW
HOST_LFLAGS = -lm -lpthread -lrt -ldl -L${XILINX_SDACCEL}/runtime/lib/x86_64 -lxilinxopencl -O3

# HW execution
HOST_LIBS = ../../xsqlutils/obj/sqldate.o  ../../xsqlutils/obj/sqlutils.o  ../../xdbconn/obj/xdbconn.o ../../xdbconn/obj/oclaccel.o  #-- for building HW

XPG_SRC = $(wildcard *.c)
XPG_OBJ = $(XPG_SRC:%.c=$(OBJDIR)/%.o)

CSIM_SRCS = $(wildcard *.cpp) #-- for building HW
CSIM_OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(CSIM_SRCS))
XPG_OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(XPG_SRC))

XPG_ALL_OBJS := $(CSIM_OBJS) $(XPG_OBJS)
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

LIB = $(OBJDIR)/xpgbolt.so

all: lib

lib: $(LIB)

$(LIB): $(XPG_ALL_OBJS)
		#$(GPP) $(HOST_CCFLAGS) $(XPG_ALL_OBJS) -shared -o $@ 
		$(GPP) $(HOST_CCFLAGS) $(XPG_ALL_OBJS) $(HOST_LIBS) $(HOST_LFLAGS) -shared -o $@  #-- HW

# We will keep the c and cpp compilation separate for sometime to chase any suprises that arise later.
$(OBJDIR)/%.o: %.cpp
	@echo "****** Compiling $@ *****"
	mkdir -p $(OBJDIR)
	$(GPP) $(HOST_CCFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(OBJDIR)/%.o: %.c
	@echo "****** Compiling $@ *****"
	mkdir -p $(OBJDIR)
	$(GPP) $(HOST_CCFLAGS) $(PGXS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

clean:
	rm -f $(XPG_ALL_OBJS) $(LIB)
	rm -rf $(OBJDIR)

#OBJS = main.o xpgscan.o xpgpreagg.o xpgcodegen.o xpgcsim_types.o xpgcsim.o

#ifdef USE_PGXS
  PG_CONFIG = pg_config
  PGXS := $(shell $(PG_CONFIG) --pgxs)
  include $(PGXS)
#endif
