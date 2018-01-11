SRCDIR=src
OUTDIR=out

LINC=gcc
#LINC=/usr/bin/clang
WINC=x86_64-w64-mingw32-gcc
#WINC=/usr/bin/clang -target x86_64-pc-win32
#WINC=/usr/bin/clang -target x86_64-w64-windows-gnu
#WINC=/usr/bin/clang -target x86_64-w64-windows
#WINC=/usr/bin/clang -target x86_64-pc-windows-gnu

CFLAGS_BASE := -std=gnu99 -g -Wall -Wno-unknown-pragmas -Wno-comment -Wno-format-zero-length -DDEBUG=1
CFLAGS_BASE := $(CFLAGS_BASE) $$(./getBuildVersion.sh)
CFLAGS_BASE := $(CFLAGS_BASE) -Dlint #Suppress string constant in libargs3

.PHONY: all clean rotwin windows linux
.SECONDEXPANSION:

all: rotwin.exe rotlin
clean:
	rm -rf $(OUTDIR) rotlin rotwin.exe

rotwin.exe: CC = $(WINC)
rotwin.exe: PRE = Windows
rotwin.exe: CFLAGS_BASE := $(CFLAGS_BASE) -DPLATFORM=Windows

rotlin: CC = $(LINC)
rotlin: PRE = Linux
rotlin: CFLAGS_BASE := $(CFLAGS_BASE) -DPLATFORM=Linux

EXEMPT_C_FILES = $(SRCDIR)/test_getaddrinfo.c # List of files not used for compilation
SRC_C_FILES = $(filter-out $(EXEMPT_C_FILES),$(wildcard $(SRCDIR)/*.c $(SRCDIR)/*/*.c)) # *.c files in $SRCDIR/ and $SRCDIR/*/
OBJ_FILES = $(patsubst $(SRCDIR)/%.c,$(OUTDIR)/$$(PRE)/%.o,$(SRC_C_FILES)) # Change *.c to *.o for target and prerequisite resolution

define COMPILE_PLATFORM
$(OUTDIR)/$(1)/%.o: $(SRCDIR)/%.c
	@mkdir -p $$(@D)
	$$(CC) -c $$(CFLAGS_BASE) $$(CFLAGS) -o $$@ $$^
endef

$(eval $(call COMPILE_PLATFORM,Linux))
$(eval $(call COMPILE_PLATFORM,Windows))

rotwin: rotwin.exe
windows: rotwin.exe
rotwin.exe: $(OBJ_FILES)
	$(CC) $(CFLAGS_BASE) $(CFLAGS) $^ -o $@ -lws2_32

linux: rotlin
rotlin: $(OBJ_FILES)
	$(CC) $(CFLAGS_BASE) $(CFLAGS) $^ -o $@