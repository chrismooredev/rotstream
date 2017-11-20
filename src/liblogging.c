

#include "headers/liblogging.h"

EnumTriple EAI_ERROR_VALUES[] = {
	MAKE_TRIPLE(EAI_BADFLAGS, "Invalid value for `ai_flags' field."),
	MAKE_TRIPLE(EAI_NONAME, "NAME or SERVICE is unknown."),
	MAKE_TRIPLE(EAI_AGAIN, "Temporary failure in name resolution."),
	MAKE_TRIPLE(EAI_FAIL, "Non-recoverable failure in name res."),
	MAKE_TRIPLE(EAI_FAMILY, "`ai_family' not supported."),
	MAKE_TRIPLE(EAI_SOCKTYPE, "`ai_socktype' not supported."),
	MAKE_TRIPLE(EAI_SERVICE, "SERVICE not supported for `ai_socktype'"),
	MAKE_TRIPLE(EAI_MEMORY, "Memory allocation failure."),
#ifdef __linux
	MAKE_TRIPLE(EAI_SYSTEM, "System error returned in `errno'."),
	MAKE_TRIPLE(EAI_OVERFLOW, "Argument buffer overflow."),
#endif
#ifdef _USE_GNU
	MAKE_TRIPLE(EAI_NODATA, "No address associated with NAME."),
	MAKE_TRIPLE(EAI_ADDRFAMILY, "Address family for NAME not supported."),
	MAKE_TRIPLE(EAI_INPROGRESS, "Processing request in progress."),
	MAKE_TRIPLE(EAI_CANCELED, "Request canceled."),
	MAKE_TRIPLE(EAI_NOTCANCELED, "Request not canceled."),
	MAKE_TRIPLE(EAI_ALLDONE, "All requests done."),
	MAKE_TRIPLE(EAI_INTR, "Interrupted by a signal."),
	MAKE_TRIPLE(EAI_IDN_ENCODE, "IDN encoding failed.")
#endif
};

// netdb.h:597 - Possible values for `ai_flags' field in `addrinfo' structure.
EnumTriple AI_FLAGS_VALUES[] = {
	MAKE_TRIPLE(AI_PASSIVE, "Socket address is intended for `bind'."),
	MAKE_TRIPLE(AI_CANONNAME, "Request for canonical name."),
	MAKE_TRIPLE(AI_NUMERICHOST, "Don't use name resolution."),
#ifdef __linux
	MAKE_TRIPLE(AI_V4MAPPED, "IPv4 mapped addresses are acceptable."),
	MAKE_TRIPLE(AI_ALL, "Return IPv4 mapped and IPv6 addresses."),
	MAKE_TRIPLE(AI_ADDRCONFIG, "Use configuration of this host to choose returned address type."),
	MAKE_TRIPLE(AI_NUMERICSERV, "Don't use name resolution."),
#endif
#ifdef _USE_GNU
	MAKE_TRIPLE(AI_IDN, "IDN encode input (assuming it is encoded in the current locale's character set) before looking it up."),
	MAKE_TRIPLE(AI_CANONIDN, "Translate canonical name from IDN format."),
	MAKE_TRIPLE(AI_IDN_ALLOW_UNASSIGNED, "Don't reject unassigned Unicode code points."),
	MAKE_TRIPLE(AI_IDN_USE_STD3_ASCII_RULES, "Validate strings according to STD3 rules."),
#endif
};

EnumTuple AF_ENUM_VALUES[] = {
#ifdef __linux
	MAKE_TUPLE(AF_ALG),
	MAKE_TUPLE(AF_ASH),
	MAKE_TUPLE(AF_ATMPVC),
	MAKE_TUPLE(AF_ATMSVC),
	MAKE_TUPLE(AF_AX25),
	MAKE_TUPLE(AF_BLUETOOTH),
	MAKE_TUPLE(AF_BRIDGE),
	MAKE_TUPLE(AF_CAIF),
	MAKE_TUPLE(AF_CAN),
	MAKE_TUPLE(AF_ECONET),
	MAKE_TUPLE(AF_FILE),
	MAKE_TUPLE(AF_IB),
	MAKE_TUPLE(AF_IEEE802154),
	MAKE_TUPLE(AF_ISDN),
	MAKE_TUPLE(AF_IUCV),
	MAKE_TUPLE(AF_KEY),
	MAKE_TUPLE(AF_LLC),
	MAKE_TUPLE(AF_LOCAL),
	MAKE_TUPLE(AF_MPLS),
	MAKE_TUPLE(AF_NETBEUI),
	MAKE_TUPLE(AF_NETLINK),
	MAKE_TUPLE(AF_NETROM),
	MAKE_TUPLE(AF_NFC),
	MAKE_TUPLE(AF_PACKET),
	MAKE_TUPLE(AF_PHONET),
	MAKE_TUPLE(AF_PPPOX),
	MAKE_TUPLE(AF_RDS),
	MAKE_TUPLE(AF_ROSE),
	MAKE_TUPLE(AF_ROUTE),
	MAKE_TUPLE(AF_RXRPC),
	MAKE_TUPLE(AF_SECURITY),
	MAKE_TUPLE(AF_TIPC),
	MAKE_TUPLE(AF_VSOCK),
	MAKE_TUPLE(AF_WANPIPE),
	MAKE_TUPLE(AF_X25),
#elif __WINNT
	MAKE_TUPLE(AF_12844),
	MAKE_TUPLE(AF_ATM),
	MAKE_TUPLE(AF_BAN),
	MAKE_TUPLE(AF_BTH),
	MAKE_TUPLE(AF_CCITT),
	MAKE_TUPLE(AF_CHAOS),
	MAKE_TUPLE(AF_CLUSTER),
	MAKE_TUPLE(AF_DATAKIT),
	MAKE_TUPLE(AF_DLI),
	MAKE_TUPLE(AF_ECMA),
	MAKE_TUPLE(AF_FIREFOX),
	MAKE_TUPLE(AF_HYLINK),
	MAKE_TUPLE(AF_ICLFXBM),
	MAKE_TUPLE(AF_IMPLINK),
	MAKE_TUPLE(AF_ISO),
	MAKE_TUPLE(AF_LAT),
	MAKE_TUPLE(AF_NETBIOS),
	MAKE_TUPLE(AF_NETDES),
	MAKE_TUPLE(AF_NS),
	MAKE_TUPLE(AF_OSI),
	MAKE_TUPLE(AF_PUP),
	MAKE_TUPLE(AF_TCNMESSAGE),
	MAKE_TUPLE(AF_TCNPROCESS),
	MAKE_TUPLE(AF_UNKNOWN1),
	MAKE_TUPLE(AF_VOICEVIEW),
#endif
	MAKE_TUPLE(AF_APPLETALK),
	MAKE_TUPLE(AF_INET),
	MAKE_TUPLE(AF_INET6),
	MAKE_TUPLE(AF_IPX),
	MAKE_TUPLE(AF_IRDA),
	MAKE_TUPLE(AF_MAX),
	MAKE_TUPLE(AF_SNA),
	MAKE_TUPLE(AF_UNIX),
	MAKE_TUPLE(AF_UNSPEC),
};
EnumTuple SOCK_ENUM_VALUES[] = {
#ifdef __linux
	MAKE_TUPLE(SOCK_DCCP),
	MAKE_TUPLE(SOCK_PACKET),
#endif
	MAKE_TUPLE(SOCK_STREAM),
	MAKE_TUPLE(SOCK_DGRAM),
	MAKE_TUPLE(SOCK_RAW),
	MAKE_TUPLE(SOCK_RDM),
	MAKE_TUPLE(SOCK_SEQPACKET)
};
  
EnumTuple PROTO_ENUM_VALUES[] = {
#ifdef __linux
	MAKE_TUPLE(IPPROTO_IPIP),
	MAKE_TUPLE(IPPROTO_EGP),
	MAKE_TUPLE(IPPROTO_TP),
	MAKE_TUPLE(IPPROTO_DCCP),
	MAKE_TUPLE(IPPROTO_RSVP),
	MAKE_TUPLE(IPPROTO_GRE),
	MAKE_TUPLE(IPPROTO_MTP),
	MAKE_TUPLE(IPPROTO_BEETPH),
	MAKE_TUPLE(IPPROTO_ENCAP),
	MAKE_TUPLE(IPPROTO_PIM),
	MAKE_TUPLE(IPPROTO_COMP),
	MAKE_TUPLE(IPPROTO_SCTP),
	MAKE_TUPLE(IPPROTO_UDPLITE),
	MAKE_TUPLE(IPPROTO_MPLS),
#endif
	MAKE_TUPLE(IPPROTO_IP),
	MAKE_TUPLE(IPPROTO_ICMP),
	MAKE_TUPLE(IPPROTO_IGMP),
	MAKE_TUPLE(IPPROTO_TCP),
	MAKE_TUPLE(IPPROTO_PUP),
	MAKE_TUPLE(IPPROTO_UDP),
	MAKE_TUPLE(IPPROTO_IDP),
	MAKE_TUPLE(IPPROTO_IPV6),
	MAKE_TUPLE(IPPROTO_ESP),
	MAKE_TUPLE(IPPROTO_AH),
	MAKE_TUPLE(IPPROTO_RAW),
	MAKE_TUPLE(IPPROTO_MAX),
};

int tvfprintf(FILE* stream, const char *fmt, va_list args){
	//assert(tablevel >= 0);
	assert(fmt != NULL);
	int tblvl    = tablevel < 0 ? 0 : tablevel;
	int totalLen = strlen(fmt) + 1 + tblvl;
	//register void* sp asm("sp");
	//_printf("Creating VLA on stack space now... (sp=%p)\n", sp);
	char hi[totalLen];
	//_printf("Created  VLA on stack space...     (sp=%p)\n", sp);
	//_printf("Creating memory zone size=%d...", totalLen);
	//char* hi = malloc(totalLen); //Create buffer big enough for string, null, and tabs
	//_printf(" Done! Addr=%p\n", hi);
	//if(hi == -1) {
	//	_printf("malloc errored!\n");
	//}
	strcpy(hi+tblvl, fmt); //copy over fmt string after where the tabs go, including \0
	memset(hi, '\t', tblvl); //set tab character preceding it
	//_printf("Calling vprintf\n");
	//_printf("Stream=%d, &Str=%p, strlen=%lu\n", *stream, hi, strlen(hi));
	return vfprintf(stream, hi, args); //pass to regular _printf
	//_printf("Done with tvprintf\n");
	//free(hi); //free memory obtained with malloc
}
int tfprintf(FILE* stream, const char *fmt, ...){
	va_list args;
	va_start(args, fmt);
	int rtn = tvfprintf(stream, fmt, args);
	va_end(args); //dealloc args?
	return rtn;
}
int tnprintf(const char *fmt, ...) {
	assert(fmt != NULL);
	va_list args;
	va_start(args, fmt);

	char* nl       = WITHWIN("\r\n") WITHLIN("\n");
	int   totalLen = strlen(fmt) + strlen(nl) + 1; //strlength + \0 + newline
	char hi[totalLen];
	strcpy(hi, fmt); //copy over fmt string after where the tabs go, including \0
	strcpy(hi + strlen(fmt), nl);
	hi[totalLen - 1] = '\0';
	int rtn = tvfprintf(stdout, hi, args); //pass to regular _printf
	va_end(args); //dealloc args?
	return rtn;
}
int tprintf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int rtn = tvfprintf(stdout, fmt, args);
	va_end(args); //dealloc args?
	return rtn;
}

const char* _getEnumValue(int value, size_t enumSize, EnumTuple enumValues[]){
	for(int i = 0; i < enumSize; i++) {
		if(value == enumValues[i].value)
			return enumValues[i].name;
	}
	return "Unknown Value";
}
const char* _getEnumValueName(int value, size_t enumSize, EnumTriple enumValues[]){
	for(int i = 0; i < enumSize; i++) {
		if(value == enumValues[i].value)
			return enumValues[i].name;
	}
	return "Unknown Value";
}
const char* _getEnumTriple(int value, size_t enumSize, EnumTriple enumValues[], char** description){
	for(int i = 0; i < enumSize; i++) {
		if(value == enumValues[i].value){
			(*description) = enumValues[i].description;
			return enumValues[i].name;
		}
	}
	return "Unknown Value";
}
char* getErrorMessage(int error){
#ifdef __linux
	return strdup(strerror(error));
#elif __WINNT
	char *s = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
				NULL, error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)&s, 0, NULL);
	*strchr(s, '\n') = '\0';
	*strchr(s, '\r') = '\0';
	return s;
#else
#error Unsupported Compiler Target
#endif
}
void printAddrinfoList(struct addrinfo *addrinfo){
	struct addrinfo* next = addrinfo;
	int              count = 1;
	do {
		tnprintf("Addrinfo #%d (0x%p): ", count++, next);
		INCTAB(){
			next = printAddrinfo(next);
		}
	} while(next != NULL);
}

struct addrinfo* printAddrinfo(struct addrinfo* addressinfo){
	struct addrinfo info = *addressinfo;
	struct addrinfo *addr = addressinfo;
	tprintf("ai_flags: %d\n", info.ai_flags);
	INCTAB() {
		listApplicableEntriesTriple(info.ai_flags, ARR_SIZE(AI_FLAGS_VALUES), AI_FLAGS_VALUES, true);
	}
	tprintf("ai_family:    %d (%s)\n", info.ai_family, getEnumValue(info.ai_family, AF_ENUM_VALUES));
	tprintf("ai_socktype:  %d (%s)\n", addr->ai_socktype, getEnumValue(info.ai_socktype, SOCK_ENUM_VALUES));
	tprintf("ai_protocol:  %d (%s)\n", addr->ai_protocol, getEnumValue(info.ai_protocol, PROTO_ENUM_VALUES));
	tprintf("ai_addrlen:   %d\n", info.ai_addrlen);
	//tprintf("ai_addr: *%p\n", info.ai_addr);
	INCTAB() {
		printSockaddr(info.ai_addrlen, info.ai_addr);
	}
	tprintf("ai_canonname: %s\n", info.ai_canonname);
	//tprintf("ai_next: *%p\n", info.ai_next);

	return info.ai_next;
}

void printSockaddr(int length, struct sockaddr* sockaddrinfo){
	char hostname[NI_MAXHOST], hostnumb[NI_MAXHOST];
	char servname[NI_MAXSERV], servnumb[NI_MAXSERV];

	/*
	int getnameinfo(const struct sockaddr *sa, socklen_t salen,
	char *host, socklen_t hostlen,
	char *serv, socklen_t servlen, int flags);
	*/

	int res_name = getnameinfo(sockaddrinfo, length, hostname, NI_MAXHOST, servname, NI_MAXSERV, 0);
	int res_numb = getnameinfo(sockaddrinfo, length, hostnumb, NI_MAXHOST, servnumb, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

	if(res_name != 0 || res_numb != 0){
		tprintf("Error with result: (%d) %s\n", errno, strerror(errno));
		return;
	}

	bool hostSame = strcmp(hostname, hostnumb) == 0;
	bool servSame = strcmp(servname, servnumb) == 0;
	tprintf("");
	if(hostSame)
		_printf("[");
	_printf("%s", hostname);
	if(hostSame)
		_printf("]");

	_printf(":%s", servname);

	if(!hostSame || !servSame){
		_printf(" (");
		if(!hostSame){
			if(!servSame && sockaddrinfo->sa_family == AF_INET6)
				_printf("[");
			_printf("%s", hostnumb);
		}
		if(!servSame){
			if(!hostSame && sockaddrinfo->sa_family == AF_INET6)
				_printf("]");
			_printf(":%s", servnumb);
		}
		_printf(")");
	}
	_printf("\n");
}


void listApplicableEntriesTriple(int value, size_t enumSize, EnumTriple enumValues[], bool print_num){
	LIST_ENTRIES(value, enumSize, enumValues, (value & enumValues[i].value) != 0, print_num);
}
void listApplicableEntriesTuple(int value, size_t enumSize, EnumTuple enumValues[], bool print_num){
	LIST_ENTRIES(value, enumSize, enumValues, (value & enumValues[i].value) != 0, print_num);
}
void listEquallingEntries(int value, size_t enumSize, EnumTuple enumValues[], bool print_num){
	LIST_ENTRIES(value, enumSize, enumValues, value == enumValues[i].value, print_num);
}