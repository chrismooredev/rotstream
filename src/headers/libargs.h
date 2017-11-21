#ifndef _INCL_LIBARG
#define _INCL_LIBARG

#include "librotstream.h"
#include "liblogging.h"
#include "../libs/argtable3.h"

#define WELL_KNOWN_PORTS_CAP 1024

#define assif(x) assert(x); if(x)

//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define free_exit(at, code)              \
	do {                                 \
		arg_freetable(at, ARR_SIZE(at)); \
		_Pragma("GCC diagnostic push"); \
		_Pragma("GCC diagnostic ignored \"-Wdeprecated\""); \
		exit(code); \
		_Pragma("GCC diagnostic pop"); \
	} while(0)
//#pragma GCC diagnostic pop


typedef void* ArgTable;

enum IPvEnum {
	IPvBoth,
	IPv4,
	IPv6
};
struct IPvMatrix {
	enum IPvEnum dst;
	enum IPvEnum src;
};
struct argumentsRaw {
	bool silent;
	size_t           loglvlsCount;
	const char**      loglvls;
	struct IPvMatrix ip;
	bool        rotSet;
	int         rot;
	const char* dstIp;
	const char* dstPort;
	const char* srcIp;
	const char* srcPort;
};
struct arguments {
	//bool silent; //Output debug or not
	enum LOGGING_FLAGS logflags;
	int8_t             rot; //Amount to rotate
	struct addrinfo* dst; //IP to forward to. Must hold only one
	struct addrinfo* src; //IP to listen in. May hold more than one.
};

void   handleArguments(int* argc, char* argv[], struct arguments* args);
void   printHelp(const char* binaryName, ArgTable argTable, bool justBasic);
void printVersion(const char* binaryName);
void printCopyright(const char* binaryName);
void   printArgsRaw(struct argumentsRaw* raw);
int    getAddressTargets(const char* name, const char* service, enum IPvEnum ipv, struct addrinfo** result, int ai_flags);
void   processArgs(struct argumentsRaw* raw, struct arguments* args);
int8_t normalizeRot(bool rotSet, int rawArg, struct addrinfo* dstAddrinfo);

#endif