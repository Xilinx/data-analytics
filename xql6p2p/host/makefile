#XILINX_SDX=/proj/xbuilds/2017.1_sdx_1026_1/installs/lin64/SDx/2017.1/
#XILINX_SDX=/proj/xbuilds/2017.4_daily_latest/installs/lin64/SDx/2017.4/
#XILINX_SDX=/opt/Xilinx/SDx/2017.1.rte/
CC = gcc
PG_SERVER_INCS += -I $(shell pg_config --includedir) -I $(shell pg_config --includedir-server)
CFLAGS += -O3 -fpic $(PG_SERVER_INCS)  -I${XILINX_SDX}/runtime/include/1_2 -I /usr/include/x86_64-linux-gnu -D_GNU_SOURCE

LDFLAGS = -O3 -shared -L${XILINX_SDX}/runtime/lib/x86_64 -L${XILINX_SDX}/lib/lnx64.o  -lrt -lm -ldl -lxilinxopencl -lz

TARGET = xql6.so
SOURCES1 = xql6.c q6_swemu.c fpga_q6.c
HEADERS = xql6.h q6_swemu.h
OBJECTS1 = $(SOURCES1:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS1)
	$(CC) $(OBJECTS1) -o $(TARGET) $(LDFLAGS) 

clean:
	$(RM) $(OBJECTS1) $(TARGET)

# sudo make install
install: $(TARGET)
	cp $(TARGET) /usr/lib/postgresql/9.6/lib/.

