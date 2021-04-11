PROGS   = demo01
ALL:    $(PROGS)

SRCS    = demo01.cpp
OBJS    = $(SRCS:.cpp=.o)


OD_TOOLCHAIN ?= /opt/FunKey-sdk-2.0.0/
CC := $(OD_TOOLCHAIN)bin/arm-funkey-linux-musleabihf-gcc
CXX := $(OD_TOOLCHAIN)bin/arm-funkey-linux-musleabihf-g++
LD := $(OD_TOOLCHAIN)bin/arm-funkey-linux-musleabihf-gcc
MARCH :=  -march=armv7-a+neon-vfpv4 -mtune=cortex-a7 -mfpu=neon-vfpv4 -Ofast -fno-PIC -fdata-sections -ffunction-sections -fsingle-precision-constant -fno-common -fno-builtin -DFUNKEY


INCLUDES += -I../
SDL_CONFIG = $(shell /opt/FunKey-sdk-2.0.0/arm-funkey-linux-musleabihf/sysroot/usr/bin/sdl-config --cflags)
CFLAGS = -O2 -g $(INCLUDES) -static
CXXFLAGS = -O2 -g $(INCLUDES) $(SDL_CONFIG) $(MARCH)

SDL_LIBS = $(shell /opt/FunKey-sdk-2.0.0/arm-funkey-linux-musleabihf/sysroot/usr/bin/sdl-config --libs) -Wl,--as-needed -Wl,--gc-sections -Wl,-O1,--sort-common -flto -s
LIBS = $(SDL_LIBS) -lSDL_image
LDFLAGS = $(MARCH)

.c.o:
	$(CC) -c $(CFLAGS) $*.c -o $*.o

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $*.cpp -o $*.o

TARGET = demo01

ALL: $(TARGET)

$(TARGET): $(OBJS)
	rm -f $@ *.opk
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)
	mksquashfs demo01 demo01.png lava.png demo01.funkey-s.desktop demo01.opk

clean:
	rm -f $(OBJS) $(TARGET) core *.opk

