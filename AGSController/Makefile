UNAME := $(shell uname)



INCDIR = ../../Engine ../../Common
LIBDIR =
CC = gcc
CXX = g++
LIBS = -lm -lstdc++

ifeq ($(UNAME), Darwin)

LIBS    += -framework SDL2
SDL_LIB = -F/Library/Frameworks
SDL_INCLUDE = -F/Library/Frameworks -I/Library/Frameworks/SDL2.framework/Headers/

CFLAGS = -fPIC -fvisibility=hidden -O2 -g -Wall $(SDL_INCLUDE)
LDFLAGS = $(SDL_LIB)
TARGET = libagscontroller.dylib
CFLAGS += -DMAC_VERSION
else

SDL_LIB = `sdl2-config --libs`
SDL_INCLUDE = `sdl2-config --cflags`

CFLAGS = -fPIC -fvisibility=hidden -O2 -g -Wall $(SDL_INCLUDE)
LDFLAGS = $(SDL_LIB)
TARGET = libagscontroller.so
CFLAGS += -DLINUX_VERSION
endif

CXXFLAGS = $(CFLAGS)

include Makefile-objs


CFLAGS   := $(addprefix -I,$(INCDIR)) $(CFLAGS)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS)
ASFLAGS  := $(CFLAGS) $(ASFLAGS)


.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(CC) -shared -dynamiclib -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)

%.o: %.c
	@echo $@
	@$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	@echo $@
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	@rm -f $(TARGET) $(OBJS)
