#ifndef _INCL_LIBROT
#define _INCL_LIBROT

#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <error.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#ifdef __linux
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#elif __WINNT
#include <winbase.h>
#include <wincon.h>
#include <winsock.h>
#include <winsock2.h>
#include <windows.h>
#endif

#include "liblogging.h"

#ifdef __linux
#define CLOSE_SOCKET(FD) close(FD);
#elif __WINNT
#define CLOSE_SOCKET(FD) \
	closesocket(FD);     \
	WSACleanup();
#endif

#ifdef __linux
#define Socket int
#elif __WINNT
#define Socket SOCKET
#endif

#define Exit(n, usererror)                                                                                      \
	{ fprintf(stderr, "%s at " __FILE__ ":%d\n", (usererror ? "Exit Condition reached" : "Error"), __LINE__);     \
	exit(n); }
#define ExitErrno(n, usererror) {                                  \
	fprintf(stderr, "Errno: %d (%s)\n\t", errno, strerror(errno)); \
	Exit(n, usererror);                                            \
}

#define CheckAndLogError(NAME, CHECKVAL)                         \
	if(CHECKVAL == -1 || CHECKVAL < 0) {                         \
		printf(#NAME " Error %d: %s\n", errno, strerror(errno)); \
		return errno;                                            \
	}

#define bool2str(x) ((x) ? "true" : "false")
#define asStr(token) #token
#define cpyStr(var, tok) case tok: var = asStr(tok)

#undef max
#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct buffer1k {
	ssize_t length;
	size_t  startIndex;
	uint8_t buf[1024];
};
struct fd_setcollection {
	fd_set read;
	fd_set write;
	fd_set except;
};
struct fdlist {
	int				client;
	int				server;
	struct buffer1k clientBuf;
	struct buffer1k serverBuf;
	struct fdlist*  next;
};
struct fdlistHead {
	int listenSocket;
	struct fdlist* next;
};

void
rotate(int8_t rotateBy, uint8_t* buf, size_t length);
struct in_addr ConvertIPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
size_t removeIndex(size_t index, size_t len, char** arr);
void printListHeader(char* header, size_t len, char** list);
void populateHints(struct addrinfo* hints, int* argc, char* argv[]);

int getServerSocket(struct addrinfo* server);
int getRemoteConnection(struct addrinfo* server);

int calcNfds(struct fdlistHead* list);
struct fd_setcollection buildSets(struct fdlistHead* list);

struct fdlist* AddFdPair(struct fdlistHead* list, int client, int server);
void RemFdPair(struct fdlistHead* list, struct fdlist *element);

void normalizeBuf(struct buffer1k* buffer);
void readfromBuf(struct buffer1k* buffer, ssize_t amount);
#endif