PROGS	= texture
ALL:	$(PROGS)

SRCS	= texture.cpp vTexture.cpp
OBJS	= $(SRCS:.cpp=.o)

OD_TOOLCHAIN ?= /opt/FunKey-sdk-2.0.0/
CC := $(OD_TOOLCHAIN)bin/arm-funkey-linux-musleabihf-gcc
CXX := $(OD_TOOLCHAIN)bin/arm-funkey-linux-musleabihf-g++
LD := $(OD_TOOLCHAIN)bin/arm-funkey-linux-musleabihf-gcc
MARCH :=  -march=armv7-a+neon-vfpv4 -mtune=cortex-a7 -mfpu=neon-vfpv4 -Ofast -fno-PIC -fdata-sections -ffunction-sections -fsingle-precision-constant -fno-common -fno-builtin -DFUNKEY


CXXFLAGS =  $(shell /opt/FunKey-sdk-2.0.0/arm-funkey-linux-musleabihf/sysroot/usr/bin/sdl-config --cflags)
CXXFLAGS += $(MARCH)
CXXFLAGS += -D_THREAD_SAFE
#CXXFLAGS +=  -g -ggdb
#CXXFLAGS += -Ofast
CXXFLAGS += -O2
#CXXFLAGS +=  -g # -DDEBUG
LDFLAGS = $(shell /opt/FunKey-sdk-2.0.0/arm-funkey-linux-musleabihf/sysroot/usr/bin/sdl-config --libs) -Wl,--as-needed -Wl,--gc-sections -Wl,-O1,--sort-common -flto -s
LDFLAGS += $(MARCH)
#LDFLAGS = $(LIBS) `sdl-config --libs`
LDFLAGS += -lSDL_ttf -lSDL_image


.cpp.o:
	rm -f $*.o
	$(CXX) -c $(CXXFLAGS) $*.cpp

$(PROGS): $(OBJS)
	rm -f $@ *.opk
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)
	mksquashfs texture texture.png logo128a.bmp texture.funkey-s.desktop texture.opk

clean:
	rm -f $(PROGS) $(OBJS) *.opk core
