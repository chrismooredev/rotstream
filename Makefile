LINC=gcc
WINC=x86_64-w64-mingw32-gcc

CFLAGS_BASE=-Wall -Wno-unknown-pragmas -std=gnu99 -g

#ifdef CFLAGS
#	CFLAGS:=$(CFLAGS_BASE) $(CFLAGS)
#else
#	CFLAGS:=$(CFLAGS_BASE)
#endif

#CFLAGS=-Wall -Wno-unknown-pragmas -g -lcrypto $(ENVCFLAGS)
#CFLAGS+=
windows:
	$(WINC) -o rotwin.exe src/rotstream.c -lws2_32 $(CFLAGS_BASE) $(CFLAGS)
linux:
	$(LINC) -o rotlin src/rotstream.c $(CFLAGS_BASE) $(CFLAGS)
test_getaddrinfo:
	$(LINC) -o test_getaddrinfo src/rotstream.c src/liblogging.c $(CFLAGS_BASE) $(CFLAGS)
all: windows linux
clean:
	rm bin/*
getstatuse:
	CFLAGS:=$(CFLAGS) -Werror
	$(MAKE) $1
