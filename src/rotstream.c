#include "headers/librotstream.h"
#include "headers/liblogging.h"

int8_t  rotateAmount;

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
	struct addrinfo addrinfo_hints = {0};
	populateHints(&addrinfo_hints, &argc, argv);

	addrinfo_hints.ai_socktype = SOCK_STREAM;
	addrinfo_hints.ai_protocol = IPPROTO_TCP;
	addrinfo_hints.ai_flags    = 0; //// AI_CANONNAME;

	errno = 0;
	rotateAmount = strtol(argv[arg_num++], NULL, 10); //Convert cipher amount as string to long
	int rotErrno = errno;

	printf("Rotate Amount   : %d\n", rotateAmount);
	if(rotErrno == ERANGE || !(-254 <= rotateAmount && rotateAmount <= 254)) {
		printf("The RotateAmount is out of range! Try putting it between -254 and 254.\n");
		Exit(2, true);
	}
	rotateAmount = rotateAmount % 255; // Normalize the cipher, Negatives are preserved

	printf("Rotate Amount(n): %d\n", rotateAmount);

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

	//TODO: Setup connection socket to Server
	//TODO: Setup listening socket
	//TODO: Start listening shit
	//TODO: Become a plugin-able netcat/rotate data
	//TODO: ???
	//TODO: Profit!

	return 0;
}