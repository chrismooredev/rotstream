#include "headers/libargs.h"

void handleArguments(int *argc, char* argv[], struct arguments* args) {
	char* binName = argv[0];

	struct arg_lit* silent     = arg_lit0("s", "silent", "disable streaming output; only output errors on stderr");
	struct arg_lit* help       = arg_lit0("h", "help", "print this help and exit");
	struct arg_lit* version    = arg_lit0("v", "version", "print version information and exit");
	struct arg_str* loglvls    = arg_strn("l", "log-flag", "LOG_FLAG", 0, 100, "toggles the specified logging flag in the application. Use --silent to disable configurable logging.");

	struct arg_lit* d4         = arg_lit0(NULL, "d4", "Forces IPv4 name resolution on the destination host.");
	struct arg_lit* d6         = arg_lit0(NULL, "d6", "Forces IPv6 name resolution on the destination host.");
	struct arg_lit* s4         = arg_lit0(NULL, "s4", "Forces IPv4 name resolution on the source host.");
	struct arg_lit* s6         = arg_lit0(NULL, "s6", "Forces IPv6 name resolution on the source host.");
	
	struct arg_int* rotAmount  = arg_int0("r", "rotate", "<n>", "The amount to rotate the stream by.");
	struct arg_str* dstPort    = arg_str1(NULL, NULL, "dstPort", "The destination port to forward to");
	struct arg_str* dstIp      = arg_str1(NULL, NULL, "dstIP", "The destination IP to forward to");
	struct arg_str* srcPort    = arg_str1(NULL, NULL, "srcPort", "The source port to listen on");
	struct arg_str* srcIp      = arg_str0(NULL, NULL, "srcIP", "The source IP to listen on");
	//struct arg_str* strings    = arg_strn(NULL, NULL, "STRING", 0, *argc + 2, NULL);
	struct arg_end* end        = arg_end(20);

	ArgTable           argtable[] = {silent, help, version, loglvls, d4, d6, s4, s6, rotAmount, dstPort, dstIp, srcPort, srcIp, end};
	int nerrors = arg_parse(*argc, argv, argtable);

	if(help->count > 0) { //Print help and exit, if found
		printVersion(binName);
		printHelp(binName, argtable, false);
		_printf("\n");
		printCopyright(binName);
		free_exit(argtable, NO_ERROR_HELPV);
	}
	if(version->count > 0) { //Print version info and exit, if found
		printVersion(binName);
		free_exit(argtable, NO_ERROR_HELPV);
	}
	if(nerrors > 0) { //Print argument errors and exit, if errors found
		arg_print_errors(stdout, end, binName);
		printHelp(binName, argtable, true);
		free_exit(argtable, ERROR_SYNTAX);
	}

	struct argumentsRaw ar;
	ar.silent  = false;
	ar.loglvlsCount = 0;
	ar.loglvls = NULL;
	ar.ip      = (struct IPvMatrix) {0};
	ar.rotSet  = false;
	ar.rot     = 0;
	ar.dstPort = NULL;
	ar.dstIp   = NULL;
	ar.srcPort = NULL;
	ar.srcIp   = NULL;

	if(silent->count > 0) {
		ar.silent = true;
	}
	{
		bool matchErrors = false;
		for(int i = 0, foundMatch = false; i < loglvls->count; i++, foundMatch = false){
			const char* str = loglvls->sval[i];
			for(int o = 0; o < ARR_SIZE(LOGGING_FLAGS_VALUES); o++) {
				if(strcmp(LOGGING_FLAGS_VALUES[o].name, str) == 0){
					foundMatch = true;
					break;
				}
			}
			if(!foundMatch){
				//TODO: implement as error in argument parser, for proper error mechanism checking
				tnprintf("Unknown logging flag `%s'!", str);
				matchErrors = true;
			}
		}
		if(matchErrors){
			printHelp(binName, argtable, true);
			free_exit(argtable, ERROR_SYNTAX);
		}
	}
	ar.loglvlsCount = loglvls->count;
	ar.loglvls      = loglvls->sval;
	{
		if(d4->count == 1 && d6->count == 1) {
			tnprintf("The -d4 and -d6 options are mutually exclusive.");
			printHelp(binName, argtable, true);
			free_exit(argtable, ERROR_SYNTAX);
		} else if(d4->count == 1) {
			ar.ip.dst = IPv4;
		} else if(d6->count == 1) {
			ar.ip.dst = IPv6;
		} else {
			ar.ip.dst = IPvBoth;
		}
		if(s4->count == 1 && s6->count == 1) {
			tnprintf("The -s4 and -s6 options are mutually exclusive.");
			printHelp(binName, argtable, true);
			free_exit(argtable, ERROR_SYNTAX);
		} else if(s4->count == 1) {
			ar.ip.src = IPv4;
		} else if(s6->count == 1) {
			ar.ip.src = IPv6;
		} else {
			ar.ip.src = IPvBoth;
		}
	}
	if(rotAmount->count == 1) {
		ar.rot = *(rotAmount->ival);
		ar.rotSet = true;
	}
	assif(dstPort->count == 1) {
		ar.dstPort = *(dstPort->sval);
	}
	assif(dstIp->count == 1) {
		ar.dstIp = *(dstIp->sval);
	}
	assif(srcPort->count == 1) {
		ar.srcPort = *(srcPort->sval);
	}
	if(srcIp->count == 1) {
		ar.srcIp = *(srcIp->sval);
	}

	IFLOG(LOG_RAWARGS) INCTAB() printArgsRaw(&ar);
	arg_freetable(argtable, ARR_SIZE(argtable));
	tnprintf("Processing arguments...");
	INCTAB() processArgs(&ar, args);

	//tnprintf("Exiting since testing libargs");
	//free_exit(argtable, ERROR_OTHER);
	arg_freetable(argtable, ARR_SIZE(argtable));
}

void printHelp(const char* binaryName, ArgTable argTable, bool justBasic) {
	tprintf("%s", binaryName);
	const char* default_fmt = "  %-20s %s\n";
	char newfmt[tablevel + strlen(default_fmt) + 1];
	memset(newfmt, '\t', tablevel);
	memcpy(newfmt + tablevel, default_fmt, strlen(default_fmt));
	newfmt[sizeof(newfmt)] = '\0';
	arg_print_syntax(stdout, argTable, "\n");
	//arg_print_glossary_gnu(stdout, argTable);
	//arg_print_glossary(stdout, argTable, "  %-20s %s\n");
	arg_print_glossary(stdout, argTable, newfmt);

	if(!justBasic){
		tnprintf("");
		tnprintf("If rotAmount is omitted, it is assumed as `-5` if the destPort is within the well-known port range (1-1024), otherwise defaults to `5`");
		tnprintf("It should be noted that a rotAmount of `0` should act as a transparent TCP forwarder.");
		tnprintf("");
		tnprintf("This program includes a few standard return codes, indicating various conditions:");
		INCTAB() {
			tnprintf("0\t- Ran and exited without errors.");
			tnprintf("1\t- Ran Help/Version and exited without errors.");
			tnprintf("2\t- Syntax error from command line");
			tnprintf("3\t- IP/Hostname lookup error");
			tnprintf("etc\t- Other error");
		}
	}
}
void printVersion(const char* binaryName){
	#ifndef PLATFORM
		#warning Unknown PLATFORM build!
		#define PLATFORM "unknown-os"
	#endif
	//#define PLATFORM lolz
	tnprintf("%s: Version %d.%d " STRINGIZE(VERSION_BUILD) " (" STRINGIZE(PLATFORM) " %s build)", binaryName, VERSION_MAJOR, VERSION_MINOR, (DEBUG ? "debug" : "release"));
}
void printCopyright(const char* binaryName){
	tnprintf("All rights reserved.");
	tnprintf("There is NO WARRANTY, to the extent permitted by law.");
	tnprintf("Written by TheWhoAreYouPerson <thewhoareyouperson+rotstream@gmail.com>");
}
void processArgs(struct argumentsRaw* raw, struct arguments* args){
	//struct arguments argsImmediate;
	//struct arguments* args = &argsImmediate;
	if(raw->silent)
		args->logflags = LOG_SILENT;
	else
		args->logflags = LOG_DEFAULT;

	{
		//Data already validated above
		for(int i = 0; i < raw->loglvlsCount; i++){
			const char* str = raw->loglvls[i];
			for(int o = 0; o < ARR_SIZE(LOGGING_FLAGS_VALUES); o++) {
				if(strcmp(LOGGING_FLAGS_VALUES[o].name, str) == 0){
					args->logflags ^= LOGGING_FLAGS_VALUES[o].value;
					//tnprintf("XORd %s!", LOGGING_FLAGS_VALUES[o].name);
					break;
				}
			}
		}
		//TODO: Find a way to refactor this out without breaking libargs logging
		loglevel = args->logflags;
	}
	{
		int dstResult;
		IFLOG(LOG_GAI_DST) tnprintf("Destination:");
		INCTAB() {
			dstResult = getAddressTargets(raw->dstIp, raw->dstPort, raw->ip.dst, &args->dst, 0);
		}
		if(dstResult != 0){
			char* errStr = getErrorMessage(dstResult);
			tnprintf("getaddrinfo(%s, %s) failed: (%d) %s", raw->dstIp, raw->dstPort, dstResult, errStr);
			tnprintf("Exiting.");
			free(errStr);
			exit(ERROR_DNS);
		} else {
			IFLOG(LOG_GAI_DST) printAddrinfoList(args->dst);
		}

		int srcResult;
		IFLOG(LOG_GAI_SRC) tnprintf("Source:");
		INCTAB() {
			srcResult = getAddressTargets(raw->srcIp, raw->srcPort, raw->ip.src, &args->src, AI_PASSIVE);
		}
		if(srcResult != 0){
			char* errStr = getErrorMessage(srcResult);
			tnprintf("getaddrinfo(%s, %s) failed: (%d) %s", raw->srcIp, raw->srcPort, srcResult, errStr);
			tnprintf("Exiting.");
			free(errStr);
			exit(ERROR_DNS);
		} else {
			IFLOG(LOG_GAI_SRC) printAddrinfoList(args->src);
		}
	}

	args->rot = normalizeRot(raw->rotSet, raw->rot, args->dst);
}
int getAddressTargets(const char* name, const char* service, enum IPvEnum ipv, struct addrinfo **result, int ai_flags) {
	struct addrinfo addrinfo_hints = {0};

	assert(ipv == IPvBoth || ipv == IPv4 || ipv == IPv6);
	addrinfo_hints.ai_family   = ipv == IPvBoth ? AF_UNSPEC : ipv == IPv4 ? AF_INET : AF_INET6;
	addrinfo_hints.ai_socktype = SOCK_STREAM;
	addrinfo_hints.ai_protocol = IPPROTO_TCP;
	addrinfo_hints.ai_flags    = ai_flags;

	return getaddrinfo(name, service, &addrinfo_hints, result);
}

void printArgsRaw(struct argumentsRaw* raw){
	tnprintf("Silent: %s", bool2str(raw->silent));
	tnprintf("Custom rotation: %s", bool2str(raw->rotSet));
	if(raw->rotSet) tnprintf("Rotation Count: %d", raw->rot);
	tnprintf("Destination IP: %s", raw->dstIp);
	tnprintf("Destination Port: %s", raw->dstPort);
	tnprintf("Source IP: %s", raw->srcIp);
	tnprintf("Source Port: %s", raw->srcPort);
}

int8_t normalizeRot(bool rotSet, int rawArg, struct addrinfo* dstAddrinfo) {
	uint16_t dstPort  = ntohs(((struct sockaddr_in*) dstAddrinfo->ai_addr)->sin_port);
	if(rotSet && rawArg != rawArg % 256){
		tnprintf("Warning: Your rotation amount is over 255/under -255, that isn't ideal...");
	}
	return rotSet ? rawArg % 256 : (dstPort < WELL_KNOWN_PORTS_CAP ? -5 : 5);
}