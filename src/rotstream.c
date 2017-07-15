#include "headers/liblogging.h"
#include "headers/librotstream.h"

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
	struct timespec   timeout = {.tv_sec = 10, .tv_nsec = 0};
	struct fdlistHead fd_list = {.listenSocket = listenSocket, .next = NULL};

	//! FD_SET(int fd, fd_set *set);
	//! FD_CLR(int fd, fd_set *set);
	//! FD_ISSET(int fd, fd_set *set);
	//! FD_ZERO(fd_set *set);

	/*
	int pselect(
		int numfds,
		fd_set *readfds,
		fd_set *writefds,
		fd_set *exceptfds,
		const struct timespec *timeout,
		const sigset_t *sigmask);
	*/
	struct fd_setcollection set = buildSets(&fd_list);

	for(int ready = 0;;ready = pselect(calcNfds(&fd_list), &set.read, &set.write, &set.except, &timeout, NULL)) {
		if(ready == 0){
			tprintf("Waiting for connection...\n");
		} else if(ready == -1) {
			if(errno != EBADF) {
				ExitErrno(200, false);
			} else {
				//TODO: Remove bad FD
				tprintf("Must remove bad FD from socket\n");
			}
		} else {
			INCTAB(){
				if(FD_ISSET(fd_list.listenSocket, &set.read)){
					tprintf("Accepting... - ");
					socklen_t        addrlen = 1024;
					struct sockaddr *addr = malloc(addrlen);
					int clfd = accept4(fd_list.listenSocket, addr, &addrlen, SOCK_NONBLOCK);
					if(clfd == -1){
						ExitErrno(27, false);
					}

					printSockaddr(addrlen, addr);
					free(addr);

					int svfd = getRemoteConnection(server);
					AddFdPair(&fd_list, clfd, svfd);
				} else {
					for(struct fdlist *list = fd_list.next; list != NULL; list = list->next){
						tprintf("Processing %p for read\n", list);
						INCTAB(){
							if(FD_ISSET(list->client, &set.read)) {
								list->clientBuf.length = read(list->client, list->clientBuf.buf, sizeof(list->clientBuf.buf));
								//TODO: SHIT
								if(list->clientBuf.length == 0){ //* EOF
									INCTAB() { RemFdPair(&fd_list, list); }
								} else if(list->clientBuf.length == -1) { //* ERROR
									ExitErrno(90, false);
									list->clientBuf.length = 0;
								}
								tprintf("Got data from client. (%d bytes)\n", list->clientBuf.length);
							}
							if(FD_ISSET(list->server, &set.read)) {
								list->serverBuf.length = read(list->server, list->serverBuf.buf, sizeof(list->serverBuf.buf));
								if(list->serverBuf.length == 0){ //* EOF
									INCTAB(){ RemFdPair(&fd_list, list); }
								} else if(list->serverBuf.length == -1) { //* ERROR
									ExitErrno(91, false);
									list->serverBuf.length = 0;
								}
								tprintf("Got data from server. (%d bytes)\n", list->serverBuf.length);
							}
						}
					}
					for(struct fdlist *list = fd_list.next; list != NULL; list = list->next){
						tprintf("Processing %p for write\n", list);
						INCTAB(){
							if(FD_ISSET(list->client, &set.write)) {
								//list->clientBuf.length = read(list->client, list->clientBuf.buf, sizeof(list->clientBuf.buf));
								//TODO: SHIT
								tprintf("Can write to client. (%d bytes waiting)\n", list->serverBuf.length);
							}
							if(FD_ISSET(list->server, &set.write)) {
								tprintf("Can write to server. (%d bytes waiting)\n", list->clientBuf.length);
							}
						}
					}
					//tprintf("Should be handling %d sockets...\n", ready);
				}
			}
		}
		set = buildSets(&fd_list);
	}
	//TODO: Become a plugin-able netcat/rotate data
	//TODO: ???
	//TODO: Profit!

	freeaddrinfo(server);
	freeaddrinfo(listen);
	return 0;
}

/*

	articulate c'mon
	do the thing

	remove from list if EOF (read() == 0)
	add new connections to list
	figure out why select isn't blocking
	figure out difference in read/write
	kick self in foot
	just forward data for now, rotate can come later.
	only accept data that can be written? dunno

*/