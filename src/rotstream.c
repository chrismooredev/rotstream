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

typedef struct sockaddr_in Address;
typedef struct in_addr     Addressv4;

int8_t  rotateAmount;
Address remote;
Address local;

void rotate(int8_t rotateBy, uint8_t* buf, size_t length) {
	for(size_t i = 0; i < length; i++) {
		buf[i] += rotateBy;
	}
}
struct in_addr ConvertIPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
	uint32_t asInt = htonl((a << 3 * 8) | (b << 2 * 8) | (c << 1 * 8) | (d << 0 * 8));
	return *(struct in_addr*) &asInt; // "Cast" type from uint32_t to in_addr struct
}
int parseAddress(char* address, char* port) {
	//struct in_addr  try4;
	//struct in_addr6 try6;
	//int getaddrinfo(const char *node, const char *service, \
		const struct addrinfo *hints, \
		struct addrinfo **res);
	//struct addrinfo  hints;
	struct addrinfo* AddressInfo;
	//memset(&hints, 0, sizeof(struct addrinfo));
	int status = 0;
	//int status = getaddrinfo(address, port, &hints, &AddressInfo);
	//int status = getaddrinfo(address, port, NULL, &AddressInfo);
	if(status == 0) {
		//success
	/*
	} else if(status == EAI_ADDRFAMILY) {
	} else if(status == EAI_AGAIN) {
	} else if(status == EAI_BADFLAGS) {
	} else if(status == EAI_FAIL) {
	} else if(status == EAI_FAMILY) {
	} else if(status == EAI_MEMORY) {
	} else if(status == EAI_NODATA) {
	} else if(status == EAI_NONAME) {
	} else if(status == EAI_SERVICE) {
	} else if(status == EAI_SOCKTYPE) {
	} else if(status == EAI_SYSTEM) {
		//check errno, system error*/
	} else {

	}

	return -1;
}

//int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)

#define asStr(token) #token
#define cpyStr(var, tok) case tok: var = asStr(tok)

void printAddrinfo(struct addrinfo *addr){
	char *s_fam;
	char *s_sock;
	switch(addr->ai_family){
		cpyStr(s_fam, AF_INET); break;
		cpyStr(s_fam, AF_INET6); break;
		//case AF_INET: s_fam = "IPv4"; break;
		//case AF_INET6: s_fam = "IPv6"; break;
		default: s_fam = "Unknown"; break;
	}
	switch(addr->ai_socktype){
		cpyStr(s_sock, SOCK_STREAM); break;
		cpyStr(s_sock, SOCK_DGRAM); break;
		cpyStr(s_sock, SOCK_RAW); break;
		//case SOCK_STREAM: s_sock = "TCP"; break;
		//case SOCK_DGRAM: s_sock = "UDP"; break;
		//case SOCK_RAW: s_sock = "Raw"; break;
		default: s_sock = "Unknown"; break;
	}
	switch(addr->ai_protocol){

	}

	printf("\tai_flags=0x%x\n", addr->ai_flags);
	printf("\tfamily=%s\n", s_fam);
	printf("\tsocktype=%s\n", s_sock);
	printf("\tprotocol=%d\n", addr->ai_protocol);
	printf("\taddrlen=%d\n", addr->ai_addrlen);
	switch(addr->ai_family){
		char buf[1024];
		char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
		char* rtn;
		int ret;
//int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
//                char *host, socklen_t hostlen,
//                char *serv, socklen_t servlen,
//                int flags);
		case AF_INET:
			//rtn = inet_ntoa(((struct sockaddr_in*) addr->ai_addr)->sin_addr);
			//printf("\t\tAddress= %s\n", rtn);
			//break;
		case AF_INET6:{
			int flags = 0
			//| NI_NAMEREQD //Return error if no hostname
			//| NI_DGRAM //Set default as UDP instead of TCP for service name resolution
			//| NI_NOFQDN //Return only hostname of FQDN
			| NI_NUMERICHOST //Force numerical form instead of textual
			| NI_NUMERICSERV //Force numberica service instead of textual
			;
			ret = getnameinfo(addr->ai_addr, addr->ai_addrlen, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), flags);
			if(ret == 0){
				printf("\tAddress=%s%s%s\n", hbuf, addr->ai_protocol ? ":" : "", addr->ai_protocol ? sbuf : "");
			} else {
				printf("\t\tThere was an error getting the human-readable format: %s\n", gai_strerror(ret));
			}
			break;
		}
		default:
			printf("\tUnrecognized address type.\n");
	}
	
	printf("\tcanonname=%s\n", addr->ai_canonname);
	/*
		struct addrinfo {
			int ai_flags;
			int ai_family;
			int ai_socktype;
			int ai_protocol;
			socklen_t       ai_addrlen;
			struct sockaddr *ai_addr;
			char            *ai_canonname;
			struct addrinfo *ai_next;
		}
	*/
}

size_t removeIndex(size_t index, size_t len, char** arr){
	if(index >= len){
		errno = ERANGE;
		return -1;
	}

	for(int x = index; x < len-1; x++){ //Shift all elements up one
		arr[x] = arr[x+1];
	}
	

	arr[len - 1] = NULL; //Set last element to NULL
	return len-1; //Return the new size
}
void printListHeader(char* header, size_t len, char** list){
	printf("%s: (%lu)\n", header, len);
	for(int i = 0; i < len; i++){
		printf("\t%d", i);
		printf(" (%p)", *list+i);
		printf("\t%s\n", list[i]);
	}
}

int populateHints(struct addrinfo *hints, int *argc, char* argv[]){
	*hints = (struct addrinfo) {0};
	hints->ai_family = AF_UNSPEC;
	for(int i = 0; i < *argc; i++){
		if(strcmp("-4", argv[i]) == 0){
			hints->ai_family = AF_INET;

			
		} else {
			hints->ai_family = AF_UNSPEC;
		}
	}

//FIX
	return NULL;
}

//bool force_ipv4 = false;
struct addrinfo addrinfo_hints = {0};

/*
http://www.nightmare.com/medusa/async_sockets.html
http://beej.us/guide/bgnet/output/html/multipage/advanced.html
*/

int main(int argc, char* argv[]) {
	if(argc < 4 + 1 || argc > 6 + 1) { //+1 for program name
		printf("%s [-4] <RotateAmount> <RemoteTargetPort> <RemoteTargetAddr> <LocalListenPort> [LocalListenAddr=0.0.0.0]\n", argv[0]);
		Exit(1, true);
	}

	printListHeader("Before index removal", argc, argv);
	argc = removeIndex(3, argc, argv);
	printListHeader("After index removal", argc, argv);

	size_t arg_num = 1;
	if(strcmp("-4", argv[arg_num]) == 0){
		//force_ipv4 = true;
		addrinfo_hints.ai_family = AF_INET;
		++arg_num;
	} else {
		addrinfo_hints.ai_family = AF_UNSPEC;
	}
	addrinfo_hints.ai_socktype = SOCK_STREAM;
	addrinfo_hints.ai_protocol = IPPROTO_TCP;
	addrinfo_hints.ai_flags = AI_CANONNAME;

	errno = 0;
	rotateAmount = strtol(argv[arg_num++], NULL, 10); //Convert cipher amount as string to long

	printf("Rotate Amount   : %d\n", rotateAmount);
	if(errno == ERANGE || !(-254 <= rotateAmount && rotateAmount <= 254)) {
		printf("The RotateAmount is out of range! Try putting it between -254 and 254.\n");
		Exit(2, true);
	}
	rotateAmount = rotateAmount % 255; // Normalize the cipher, Negatives are preserved

	printf("Rotate Amount(n): %d\n", rotateAmount);

	//printf("ArgNum: %d - %s\n", arg_num, argv[arg_num]);
	//uint16_t tmp_port = strtol(argv[arg_num++], NULL, 10);
	//addrinfo_hints.ai_

	struct addrinfo *beginning;
	struct addrinfo *addresses;
	int result = getaddrinfo(argv[arg_num+1], argv[arg_num], &addrinfo_hints, &beginning);
	addresses = beginning;

	printf("Getaddr %s\n", result == 0 ? "Successful" : "Unsuccessful");
	for(int i = 0; addresses != NULL; i++, addresses = addresses->ai_next){
		printf("Address Entry #%d\n", i);
		printAddrinfo(addresses);
	}

	freeaddrinfo(beginning);

	arg_num++;
	//remote.sin_port = tmp_port;

	char addr[1024];
	printf("%s", remote.sin_family == AF_INET ? "AF_INET" : "AF_INET6");
	//char* inet_ntop(int af, const void *src, char *dst, socklen_t size);
	printf("%s", inet_ntop(remote.sin_family, &remote.sin_addr, addr, sizeof(addr)));
	printf(":%d\n", remote.sin_port);

	//return 0;
/*
struct sockaddr_in {
	short	sin_family;
	u_short	sin_port;
	struct in_addr	sin_addr;
	char	sin_zero[8];
};
*/
#ifdef __WINNT
	WSADATA wsaData;
	WSAStartup(0x0202, &wsaData); //Version 2.2 of Winsock.dll? // https://msdn.microsoft.com/en-us/library/windows/desktop/ms742213(v=vs.85).aspx
#endif

	struct sockaddr_in binding;
	binding.sin_family = AF_INET;
	binding.sin_port   = htons(8080);
	binding.sin_addr   = ConvertIPv4(192, 168, 1, 64);

	struct sockaddr_in destination;
	destination.sin_family = AF_INET;
	destination.sin_port   = htons(8080);
	destination.sin_addr   = ConvertIPv4(192, 168, 1, 64);

	Socket initSock = socket(AF_INET, SOCK_STREAM /* | O_NONBLOCK*/, IPPROTO_TCP);
	CheckAndLogError("Socket Creation", initSock);

	int connectStatus = connect(initSock, (struct sockaddr*) &destination, sizeof(struct sockaddr_in));
	printf("Return = %d\n", connectStatus);
	CheckAndLogError("Socket Connection", connectStatus);

	puts("Connected!\n");

	bool    switchTo = true;
	uint8_t buffer[1024];
	size_t  readCount;
	do {
		if(switchTo) {
			switchTo  = false;
			readCount = read(STDIN_FILENO, buffer, sizeof(buffer));
			rotate(rotateAmount, buffer, readCount);
			send(initSock, buffer, readCount, 0);
		} else {
			switchTo  = true;
			readCount = read(initSock, buffer, sizeof(buffer));
			rotate(-rotateAmount, buffer, readCount);
			send(STDOUT_FILENO, buffer, readCount, 0);
		}
	} while(readCount > 1 || errno == EAGAIN);
	CheckAndLogError("Unexpected", readCount);

	return 0;
}