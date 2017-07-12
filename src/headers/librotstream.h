#ifndef _INCL_LIBROT
#define _INCL_LIBROT

#include <limits.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __linux
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#elif __WINNT
#include <winbase.h>
#include <wincon.h>
#include <winsock.h>
#include <winsock2.h>
#include <windows.h>
#endif

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

#define Exit(n, usererror)                                       \
	fprintf(stderr, "%s at " __FILE__ ":%d\n", (usererror ? "Exit Condition reached" : "Error"), __LINE__);     \
	exit(n);

#define CheckAndLogError(NAME, CHECKVAL)                         \
	if(CHECKVAL == -1 || CHECKVAL < 0) {                         \
		printf(#NAME " Error %d: %s\n", errno, strerror(errno)); \
		return errno;                                            \
	}

#define bool2str(x) ((x) ? "true" : "false")
#define asStr(token) #token
#define cpyStr(var, tok) case tok: var = asStr(tok)

void rotate(int8_t rotateBy, uint8_t* buf, size_t length);
struct in_addr ConvertIPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
size_t removeIndex(size_t index, size_t len, char** arr);
void printListHeader(char* header, size_t len, char** list);
void populateHints(struct addrinfo* hints, int* argc, char* argv[]);

#endif