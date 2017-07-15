#ifndef _INCL_LIBLOG
#define _INCL_LIBLOG

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//MinGW should provide in a windows build, should be in linux

#ifdef __linux
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#elif __WINNT
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <winbase.h>
#include <wincon.h>
#include <winsock.h>
#include <winerror.h>
#endif

#include "mp.h"

int tablevel;



#define MAKE_TUPLE(x) {.value = x, .name = #x}
#define MAKE_TRIPLE(x, y) {.value = x, .name = #x, .description = y}
#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))

#define INCTAB()                      \
	MPP_BEFORE(inctab_a, tablevel++;) \
	MPP_AFTER(inctab_b, tablevel--;)

/*
struct addrinfo {
	int ai_flags;
	int ai_family;
	int ai_socktype;
	int ai_protocol;
	socklen_t ai_addrlen;
	struct sockaddr* ai_addr;
	char*            ai_canonname;
	struct addrinfo* ai_next;
}
*/

#define LIST_ENTRIES(VAL, ENUM_SIZE, ENUM_VALS, CHECK_EXPR, PRNT_NUM)       \
	for(int i = 0; i < ENUM_SIZE; i++) {                                    \
		if((CHECK_EXPR)) {                                                  \
			tprintf("");                                                    \
			if(PRNT_NUM)                                                    \
				printf("%d - ", ENUM_VALS[i].value);                        \
			printf("%s\n",ENUM_VALS[i].name); \
		}                                                                   \
	}

typedef struct enumpair {
	int value;
	char* name;
} EnumTuple;
typedef struct enumtriple {
	int value;
	char* name;
	char* description;
} EnumTriple;
//#region lol
//{ // WITH* macros
#ifdef __USE_GNU
#define WITHGNU(x) x
#else
#define WITHGNU(x)
#endif

#ifdef __WINNT
#define WITHWIN(x) x
#else
#define WITHWIN(x)
#endif

#ifdef __linux
#define WITHLIN(x) x
#else
#define WITHLIN(x)
#endif
//}
//#endregion

EnumTriple EAI_ERROR_VALUES[8 WITHLIN(+2) WITHGNU(+8)];
EnumTriple AI_FLAGS_VALUES[3 WITHLIN(+4) WITHGNU(+4)];
EnumTuple AF_ENUM_VALUES[9 WITHLIN(+35) WITHWIN(+25)];
EnumTuple SOCK_ENUM_VALUES[5 WITHLIN(+2)];
EnumTuple PROTO_ENUM_VALUES[22 WITHLIN(+4)];

// Functions similar to printf however prefixes output with an amount of tabs specified by `tablevel`
void tprintf(const char* fmt, ...);
// Get the first value in the enum list, where `value` is equal to EnumValue
#define getEnumValue(value, enumValues) _getEnumValue(value, ARR_SIZE(enumValues), enumValues)
const char* _getEnumValue(int value, size_t enumSize, EnumTuple enumValues[]);
#define getEnumValueName(value, enumValues) _getEnumValueName(value, ARR_SIZE(enumValues), enumValues)
const char* _getEnumValueName(int value, size_t enumSize, EnumTriple enumValues[]);
#define getEnumTriple(value, enumValues, description) _getEnumTriple(value, ARR_SIZE(enumValues), enumValues, description)
const char* _getEnumTriple(int value, size_t enumSize, EnumTriple enumValues[], char** description);

void printAddrinfoList(struct addrinfo* addrinfo);
struct addrinfo* printAddrinfo(struct addrinfo* addressinfo);
void printSockaddr(int length, struct sockaddr* sockaddrinfo);

void listApplicableEntriesTriple(int value, size_t enumSize, EnumTriple enumValues[], bool print_num);
void listApplicableEntriesTuple(int value, size_t enumSize, EnumTuple enumValues[], bool print_num);
void listEquallingEntries(int value, size_t enumSize, EnumTuple enumValues[], bool print_num);

#endif