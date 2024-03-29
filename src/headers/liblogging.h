#ifndef _INCL_LIBLOG
#define _INCL_LIBLOG

#include "libversion.h"
#include "../libs/mp.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>

//MinGW should provide in a windows build, should be in linux

#ifdef BUILD_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//#endif
#elif defined(BUILD_WIN32)
/*#ifdef _WINSOCKAPI_
#warning somehow _WINSOCKAPI_ is defined
#endif
#define _WINSOCKAPI_ someshit*/
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <winbase.h>
#include <wincon.h>
#include <winerror.h>
#endif

int tablevel;
enum LOGGING_FLAGS loglevel;

#define MAKE_TUPLE(x) {.value = x, .name = #x}
#define MAKE_TRIPLE(x, y) {.value = x, .name = #x, .description = y}
#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))

#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

#define INCTAB()                      \
	MPP_BEFORE(inctab_a, tablevel++;) \
	MPP_AFTER(inctab_b, tablevel--;)

#define IFLOG(lvl)        \
	if(!((lvl & loglevel) != 0)) \
		; \
	else
#define IFLOGF(lvl, force) \
	if(!((lvl & loglevel) != 0 || (force))) \
		; \
	else

#define dbg if(DEBUG)

#define printErrIfErr(x, lstErrMacro)                \
	do {                                             \
		if(x == -1) { \
		char* errStr = getErrorMessage(lstErrMacro); \
		_printf("%s", errStr); \
		free(errStr); \
		} \
	} while(0)

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
				_printf("%d - ", ENUM_VALS[i].value);                        \
			_printf("%s\n",ENUM_VALS[i].name); \
		}                                                                   \
	}

#ifdef _WIN32
#  ifdef _WIN64
#    define PRI_SIZET PRIu64
#    define PRI_SSIZET PRId64
#    define PRI_SOCKT PRIu64
#    define PRI_SOCKLENT PRIu64
#  else
#    define PRI_SIZET PRIu32
#    define PRI_SSIZET PRId32
#    define PRI_SOCKT PRIu32
#    define PRI_SOCKLENT PRIu32
#  endif
#else
#  define PRI_SIZET "zu"
#  define PRI_SSIZET "zd"
#  define PRI_SOCKT "d"
#  define PRI_SOCKLENT "u"
#endif

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
#ifdef _USE_GNU
#define WITHGNU(x) x
#else
#define WITHGNU(x)
#endif

#ifdef BUILD_WIN32
#define WITHWIN(x) x
#else
#define WITHWIN(x)
#endif

#ifdef BUILD_LINUX
#define WITHLIN(x) x
#else
#define WITHLIN(x)
#endif
//}
//#endregion

enum LOGGING_FLAGS {
	LOG_SILENT   = 0b00000000,
	LOG_RAWARGS  = 0b00000001,
	LOG_PROCARGS = 0b00000010,

	LOG_GAI_DST  = 0b00000100,
	LOG_GAI_SRC  = 0b00001000,
	LOG_DNS      = LOG_GAI_DST | LOG_GAI_SRC,
	LOG_SOCKCRTE = 0b00010000,

	LOG_ARGS       = 0b00100000 | LOG_DNS,
	LOG_CALCHAND   = 0b01000000,
	LOG_PROC_READ  = 0b10000000,
	LOG_PROC_WRITE = 0b00000001 << 8,
	LOG_CONNHAND   = 0b00000010 << 8,

	LOG_DEFAULT = LOG_SOCKCRTE | LOG_PROC_READ | LOG_PROC_WRITE | LOG_CONNHAND | LOG_CALCHAND
};
EnumTuple LOGGING_FLAGS_VALUES[13];

EnumTriple EAI_ERROR_VALUES[8 WITHLIN(+2) WITHGNU(+8)];
EnumTriple AI_FLAGS_VALUES[3 WITHLIN(+4) WITHGNU(+4)];
EnumTuple AF_ENUM_VALUES[9 WITHLIN(+35) WITHWIN(+25)];
EnumTuple SOCK_ENUM_VALUES[5 WITHLIN(+2)];
EnumTuple PROTO_ENUM_VALUES[12 WITHLIN(+14)];

enum RETURN_CODES {
	NO_ERRORS = 0,
	NO_ERROR_HELPV = 1,
	ERROR_SYNTAX = 2,
	ERROR_DNS = 3,
	ERROR_OTHER = 100
};

int tvfprintf(FILE *stream, const char *fmt, va_list args) __attribute__ ((format (printf, 2, 0))); // Functions similar to printf however prefixes output with an amount of tabs specified by `tablevel`
int tfprintf(FILE *stream, const char *fmt, ...) __attribute__ ((format (printf, 2, 3))); // Functions similar to printf however prefixes output with an amount of tabs specified by `tablevel`
int tnprintf(const char* fmt, ...) __attribute__ ((format (printf, 1, 2))); // Identical to tprintf, but adds a newline to the end
int tprintf(const char* fmt, ...) __attribute__((format (printf, 1, 2))); // Functions similar to printf however prefixes output with an amount of tabs specified by `tablevel`
//Use what is functionally equilavent to printf without triggering compiler warnings.
#define _printf(format, ...) fprintf(stdout, format, ##__VA_ARGS__)

int printf(const char *format, ...) __attribute__ ((deprecated));
//void exit(int __status) __attribute__ ((deprecated));

// Get the first value in the enum list, where `value` is equal to EnumValue
#define getEnumValue(value, enumValues) _getEnumValue(value, ARR_SIZE(enumValues), enumValues)
const char* _getEnumValue(int value, size_t enumSize, EnumTuple enumValues[]);
#define getEnumValueName(value, enumValues) _getEnumValueName(value, ARR_SIZE(enumValues), enumValues)
const char* _getEnumValueName(int value, size_t enumSize, EnumTriple enumValues[]);
#define getEnumTriple(value, enumValues, description) _getEnumTriple(value, ARR_SIZE(enumValues), enumValues, description)
const char* _getEnumTriple(int value, size_t enumSize, EnumTriple enumValues[], char** description);
char *getErrorMessage(int error);

void printAddrinfoList(struct addrinfo *addrinfo);
struct addrinfo* printAddrinfo(struct addrinfo* addressinfo);
void printSockaddr(int length, struct sockaddr* sockaddrinfo);

void listApplicableEntriesTriple(int value, size_t enumSize, EnumTriple enumValues[], bool print_num);
void listApplicableEntriesTuple(int value, size_t enumSize, EnumTuple enumValues[], bool print_num);
void listEquallingEntries(int value, size_t enumSize, EnumTuple enumValues[], bool print_num);

#endif
