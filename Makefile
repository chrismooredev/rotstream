LINC=gcc
WINC=x86_64-w64-mingw32-gcc

CFLAGS_BASE= -std=gnu99 -g -Wall -Wno-unknown-pragmas -Wno-comment 

#ifdef CFLAGS
#	CFLAGS:=$(CFLAGS_BASE) $(CFLAGS)
#else
#	CFLAGS:=$(CFLAGS_BASE)
#endif

#CFLAGS=-Wall -Wno-unknown-pragmas -g -lcrypto $(ENVCFLAGS)
#CFLAGS+=

#Force action even if target is 'up-to-date'
.PHONY: all windows linux test_getaddrinfo clean getstatuse

windows:
	$(WINC) -o rotwin.exe src/rotstream.c src/librotstream.c src/liblogging.c -lws2_32 $(CFLAGS_BASE) $(CFLAGS)
linux:
	$(LINC) -o rotlin src/rotstream.c src/librotstream.c src/liblogging.c $(CFLAGS_BASE) $(CFLAGS)
test_getaddrinfo:
	$(LINC) -o test_getaddrinfo src/test_getaddrinfo.c src/liblogging.c $(CFLAGS_BASE) $(CFLAGS)
all: windows linux
clean:
	rm bin/*
getstatuse:
	CFLAGS:=$(CFLAGS) -Werror
	$(MAKE) $1
