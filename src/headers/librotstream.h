#ifndef _INCL_LIBROT
#define _INCL_LIBROT

#include "libversion.h"
#include "liblogging.h"
#include "../libs/argtable3.h"

#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <error.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#ifdef BUILD_LINUX
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#elif defined(BUILD_WIN32)
//#include <winsock.h>
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
#include <wincon.h>
#endif

#ifdef BUILD_LINUX
#define CLOSE_SOCKET(FD) close(FD);
#elif defined(BUILD_WIN32)
#define CLOSE_SOCKET(FD) \
	closesocket(FD);
#else
#error Unsupported Compiler Target
#endif

#ifdef BUILD_LINUX
#define Socket int
#elif defined(BUILD_WIN32)
#define Socket SOCKET
#else
#error Unsupported Compiler Target
#endif

#ifdef BUILD_LINUX
#define SOCKET_INVALID -1
#elif defined(BUILD_WIN32)
#define SOCKET_INVALID INVALID_SOCKET
#else
#error Unsupported Compiler Target
#endif

#define socketInvalid(x) ((x) == SOCKET_INVALID)
#define socketValid(x) ((x) != SOCKET_INVALID)

#ifdef BUILD_LINUX
#define LAST_ERROR errno
#elif defined(BUILD_WIN32)
#define LAST_ERROR WSAGetLastError()
#else
#error Unsupported Compiler Target
#endif
#ifdef BUILD_LINUX
#define LAST_SYS_ERROR errno
#elif defined(BUILD_WIN32)
#define LAST_SYS_ERROR GetLastError()
#else
#error Unsupported Compiler Target
#endif

#ifdef BUILD_LINUX
#define SET_LAST_ERROR(x) do { errno = (x); } while(0)
#elif defined(BUILD_WIN32)
#define SET_LAST_ERROR(x) WSASetLastError(x)
#else
#error Unsupported Compiler Target
#endif


#ifdef BUILD_LINUX
#define isValidFd(fd) (!(fcntl(fd, F_GETFD) == -1 && errno == EBADF))
#elif defined(BUILD_WIN32)
#define isValidFd(fd) (!(getsockopt(fd, SOL_SOCKET, SO_TYPE, NULL, NULL) && WSAGetLastError() == WSAENOTSOCK))
#else
#error Unsupported Compiler Target
#endif

#ifdef BUILD_WIN32
#undef EWOULDBLOCK
#undef EINPROGRESS
#undef ECONNRESET
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EINPROGRESS WSAEINPROGRESS
#define ECONNRESET WSAECONNRESET
#endif

#define Exit(n, usererror)                                                                                      \
	do { fprintf(stderr, "%s in %s@" __FILE__ ":%d\n", (usererror ? "Exit Condition reached" : "Error"), __func__, __LINE__);     \
	exit(n); } while(false)
#define ExitErrno(n, usererror)                                     \
	do                                                              \
	{                                                               \
		char *err_str = getErrorMessage(LAST_ERROR);                \
		fprintf(stderr, "Errno: %d (%s)\n\t", LAST_ERROR, err_str); \
		/* //TODO free(err_str);  SEGFAULTS */                      \
		Exit(n, usererror);                                         \
	} while (false)

#define CheckAndLogError(NAME, CHECKVAL)                         \
do { \
	if(CHECKVAL == -1 || CHECKVAL < 0) {                         \
		fprintf(stderr, #NAME " Error %d: %s\n", LAST_ERROR, strerror(errno)); \
		return LAST_ERROR;                                            \
	} while(false)

#define bool2str(x) ((x) ? "true" : "false")
#define asStr(token) #token
#define cpyStr(var, tok) case tok: var = asStr(tok)

#undef max
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define getOpposite(list, elem) (&list->client == elem ? &list->server : &list->client)

#define addToSetIf(cond, fd, set) \
	do {                          \
		if((cond))                \
			FD_SET((fd), (set));  \
	} while(0)

struct buffer1k {
	ssize_t length;
	size_t  startIndex;
	uint8_t buf[16*1024]; //16KiB buffer
};
struct fd_setcollection {
	fd_set read;
	fd_set write;
	fd_set except;
};
struct fdelem {
	Socket fd;
	char*            descriptString;
	struct buffer1k  buf;
	struct sockaddr* sockaddr;
	int              sockaddrlen;
};

/*
union fdlist_u {
	struct fdlist list;
	struct {
		struct fdelem elems[2];
		struct fdlist* next;
	};
}; */
struct fdlist {
	struct fdelem  client;
	struct fdelem  server;
	struct fdlist* next;
};
struct fdlistenHead {
	size_t count;
	Socket* fds;
	struct fdlist* next;
};
struct fdlistHead {
	Socket fd;
	struct fdlist* next;
};

extern bool shouldTerminate;

void rotate(int8_t rotateBy, uint8_t* buf, size_t length);
struct in_addr ConvertIPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
size_t removeIndex(size_t index, size_t len, char** arr);
void printListHeader(char* header, size_t len, char** list);
void populateHints(struct addrinfo* hints, int* argc, char* argv[]);

void getServerSocket(struct addrinfo* server, int count, Socket* sockarr);
Socket getRemoteConnection(struct addrinfo* server);

int calcNfds(struct fdlistenHead* list, struct fd_setcollection col);
struct fd_setcollection buildSets(struct fdlistenHead* list);

struct fdlist* AddFdPair(struct fdlistenHead* list, int client, int server, struct sockaddr *addr, socklen_t addrlen);
struct fdlist* RemFdPair(struct fdlistenHead* list, struct fdlist *element);

void normalizeBuf(struct buffer1k* buffer);
void readfromBuf(struct buffer1k* buffer, ssize_t amount);

struct fdlist* processRead(struct fdlistenHead* head, struct fdlist* list, struct fd_setcollection* collection, int8_t rotateAmount);
void processWrite(struct fdlist* list, fd_set* write);
int calcHandled(struct fdlistenHead* list, struct fd_setcollection actedOn, struct fd_setcollection fromSelect, char*** metadata);
bool setSocketNonblocking(Socket sock);
bool setSocketNoTcpDelay(Socket sock);

/*
	Ctrl-C - Terminates application by setting (_terminate = true)
*/
#ifdef BUILD_LINUX
void handler_SIGINT(int s);
#elif defined(BUILD_WIN32)
BOOL handler_SIGINT(DWORD dwCtrlType);
#endif

#ifdef DEBUG
#ifdef BUILD_LINUX
#include <execinfo.h>
#define enableStacktrace() //do { signal(SIGSEGV, errHandler); } while(0)
void errHandler(int signalno);
#endif
#endif
#ifndef enableStacktrace
#define enableStacktrace()
#endif
#endif
