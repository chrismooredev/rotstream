LINC=gcc
#LINC=clang
WINC=x86_64-w64-mingw32-gcc

CFLAGS_BASE= -std=gnu99 -g -Wall -Wno-unknown-pragmas -Wno-comment -DDEBUG=1

CFLAGS_BASE := $(CFLAGS_BASE) -DVERSION_MAJOR=0
CFLAGS_BASE := $(CFLAGS_BASE) -DVERSION_MINOR=2
BUILD_DIR= out

#Force action even if target is 'up-to-date'
#.PHONY: all windows linux test_getaddrinfo clean getstatuse

define_compiler:
ifeq "$(MAKECMDGOALS)" "windows"
COM = $(WINC)
PRE = windows
else ifeq "$(MAKECMDGOALS)" "linux"
COM = $(LINC)
PRE = linux
endif

compile: define_compiler
	mkdir -p $(BUILD_DIR)
	$(COM) $(CFLAGS_BASE) $(CFLAGS) -c -o $(BUILD_DIR)/$(PRE)_rotstream.o src/rotstream.c
	$(COM) $(CFLAGS_BASE) $(CFLAGS) -c -o $(BUILD_DIR)/$(PRE)_librotstream.o src/librotstream.c
	$(COM) $(CFLAGS_BASE) $(CFLAGS) -c -o $(BUILD_DIR)/$(PRE)_liblogging.o src/liblogging.c

windows: compile
	$(COM) $(BUILD_DIR)/$(PRE)_* -o rotwin.exe -lws2_32

linux: compile
	$(COM) $(BUILD_DIR)/$(PRE)_* -o rotlin

all: windows linux

clean:
	rm -rf out rotlin rotwin.exe
