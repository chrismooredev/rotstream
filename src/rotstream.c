#include "headers/librotstream.h"
#include "headers/liblogging.h"

int8_t  rotateAmount;



int main(int argc, char* argv[]) {
	if(argc < 4 + 1 || argc > 6 + 1) { //+1 for program name
		printf("%s [-4] <RotateAmount> <RemoteTargetPort> <RemoteTargetAddr> <LocalListenPort> [LocalListenAddr=0.0.0.0/::]\n", argv[0]);
		Exit(1, true);
	}

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

	struct addrinfo *server;
	struct addrinfo *listen;
	int result = getaddrinfo(argv[arg_num+1], argv[arg_num], &addrinfo_hints, &server);
	
	arg_num += 2;
	addrinfo_hints.ai_flags |= AI_PASSIVE;
	int result2 = getaddrinfo(arg_num+1 <= argc ? argv[arg_num+1] : NULL, argv[arg_num], &addrinfo_hints, &listen);

	if(result != 0){
		printf("Result != 0\n");
	}
	if(result2 != 0){
		printf("Result != 0\n");
	}

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

/*
http://www.nightmare.com/medusa/async_sockets.html
http://beej.us/guide/bgnet/output/html/multipage/advanced.html
*/

	

	//TODO: Setup listening socket
	int listenSocket = getServerSocket(listen);

	//* Setup connection socket to Server
	//int masterServer = getServerSocket(beginning);

	//TODO: Start listening shit
	//* Setup `select()` data
	struct timespec timeout = { .tv_sec = 10, .tv_nsec = 0};
	fd_set sockets;
	int count;

	//! FD_SET(int fd, fd_set *set);
	//! FD_CLR(int fd, fd_set *set);
	//! FD_ISSET(int fd, fd_set *set);
	//! FD_ZERO(fd_set *set);
	FD_ZERO(&sockets);
	#define addfd(fd, list)
	FD_SET(listenSocket, &sockets);
	/*
	int pselect(
		int numfds,
		fd_set *readfds,
		fd_set *writefds,
		fd_set *exceptfds,
		const struct timespec *timeout,
		const sigset_t *sigmask);
	*/
	int a = pselect(/*wat*/ 0, NULL, NULL, NULL, *timeout, NULL)
	//TODO: Become a plugin-able netcat/rotate data
	//TODO: ???
	//TODO: Profit!

	freeaddrinfo(server);
	freeaddrinfo(listen);
	return 0;
}

struct fdlist {
	int            fd;
	struct fdlist* pair;
	struct fdlist* next;
};

void AddFdPair(struct fdlist* list, struct fd_set* fds, int fd1, fd2)
void AddFd(struct fdlist* list, struct fd_set* fds, int fd) {
	
}
int calcNfds(struct fdlist *list){

}