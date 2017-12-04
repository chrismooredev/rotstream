LINC=gcc
#LINC=/usr/bin/clang
WINC=x86_64-w64-mingw32-gcc
#WINC=/usr/bin/clang -target x86_64-pc-win32
#WINC=/usr/bin/clang -target x86_64-w64-windows-gnu
#WINC=/usr/bin/clang -target x86_64-w64-windows
#WINC=/usr/bin/clang -target x86_64-pc-windows-gnu

CFLAGS_BASE=-c -std=gnu99 -g -Wall -Wno-unknown-pragmas -Wno-comment -Wno-format-zero-length -DDEBUG=1

CFLAGS_BASE := $(CFLAGS_BASE) -DVERSION_MAJOR=0
CFLAGS_BASE := $(CFLAGS_BASE) -DVERSION_MINOR=2
CFLAGS_BASE := $(CFLAGS_BASE) -Dlint #Suppress string constant in libargs3
BUILD_DIR=out

#Force action even if target is 'up-to-date'
#.PHONY: all windows linux test_getaddrinfo clean getstatuse

define_compiler:
ifeq "$(MAKECMDGOALS)" "windows"
COM = $(WINC)
PRE = Windows
else ifeq "$(MAKECMDGOALS)" "linux"
COM = $(LINC)
PRE = Linux
endif
CFLAGS_BASE := $(CFLAGS_BASE) -DPLATFORM=$(PRE)

compile: define_compiler
	mkdir -p $(BUILD_DIR)/$(PRE)/libs #Creates full heiarchy
	$(COM) $(CFLAGS_BASE) $(CFLAGS) -o $(BUILD_DIR)/$(PRE)/libs/argtable3.o src/libs/argtable3.c
	$(COM) $(CFLAGS_BASE) $(CFLAGS) -o $(BUILD_DIR)/$(PRE)/rotstream.o src/rotstream.c
	$(COM) $(CFLAGS_BASE) $(CFLAGS) -o $(BUILD_DIR)/$(PRE)/librotstream.o src/librotstream.c
	$(COM) $(CFLAGS_BASE) $(CFLAGS) -o $(BUILD_DIR)/$(PRE)/liblogging.o src/liblogging.c
	$(COM) $(CFLAGS_BASE) $(CFLAGS) -o $(BUILD_DIR)/$(PRE)/libargs.o src/libargs.c

rotwin: windows
rotwin.exe: windows
windows: compile
	$(COM) $(BUILD_DIR)/$(PRE)/*.o $(BUILD_DIR)/$(PRE)/libs/*.o -o rotwin.exe -lws2_32

rotlin: linux
linux: compile
	$(COM) $(BUILD_DIR)/$(PRE)/*.o $(BUILD_DIR)/$(PRE)/libs/*.o -o rotlin

all: windows linux

clean:
	rm -rf $(BUILD_DIR) rotlin rotwin.exe
