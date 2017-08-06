#define _GNU_SOURCE //Thanks https://github.com/lpeterse/haskell-socket/issues/24

#include "headers/liblogging.h"
#include "headers/librotstream.h"

int8_t  rotateAmount;

int main(int argc, char* argv[]) {
	if(argc < 4 + 1 || argc > 6 + 1) { //+1 for program name
		tprintf("%s [-4] <RotateAmount> <RemoteTargetPort> <RemoteTargetAddr> <LocalListenPort> [LocalListenAddr=0.0.0.0/::]\n", argv[0]);
		Exit(1, true);
	}

	setbuf(stdout, NULL);

	size_t arg_num = 1;
	struct addrinfo addrinfo_hints = {0};
	populateHints(&addrinfo_hints, &argc, argv);

	addrinfo_hints.ai_socktype = SOCK_STREAM;
	addrinfo_hints.ai_protocol = IPPROTO_TCP;
	addrinfo_hints.ai_flags    = 0; //// AI_CANONNAME;

	errno = 0;
	rotateAmount = strtol(argv[arg_num++], NULL, 10); //Convert cipher amount as string to long
	int rotErrno = errno;

	tprintf("Rotate Amount   : %d\n", rotateAmount);
	if(rotErrno == ERANGE || !(-254 <= rotateAmount && rotateAmount <= 254)) {
		tprintf("The RotateAmount is out of range! Try putting it between -254 and 254.\n");
		Exit(2, true);
	}
	rotateAmount = rotateAmount % 255; // Normalize the cipher, Negatives are preserved

	tprintf("Rotate Amount(n): %d\n", rotateAmount);

	struct addrinfo *server;
	struct addrinfo *listen;
	int result = getaddrinfo(argv[arg_num+1], argv[arg_num], &addrinfo_hints, &server);
	
	arg_num += 2;
	addrinfo_hints.ai_flags |= AI_PASSIVE;
	int result2 = getaddrinfo(arg_num+1 <= argc ? argv[arg_num+1] : NULL, argv[arg_num], &addrinfo_hints, &listen);

	if(result != 0){
		tprintf("Result != 0\n");
	}
	if(result2 != 0){
		tprintf("Result2 != 0\n");
	}

	arg_num++;

	tprintf("sizeof(struct fdlistHead) = %lu, sizeof(struct fdlist) = %lu, sizeof(struct fdelem) = %lu\n", sizeof(struct fdlistHead), sizeof(struct fdlist), sizeof(struct fdelem));

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
	struct fdlistHead fd_list = {.fd = listenSocket, .next = NULL};

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
	struct fd_setcollection for_calc;
	for(int ready = 0;;set = buildSets(&fd_list), ready = pselect(calcNfds(&fd_list, set), &set.read, &set.write, &set.except, &timeout, NULL)) {
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
			tprintf("Handling %d connection%s...\n", ready, ready != 1 ? "s" : "");
			INCTAB() {
				for_calc = set;
				//TODO: bool accepted = acceptConnection(struct fdlistHead * list, fd_list read);
				if(FD_ISSET(fd_list.fd, &set.read)) {
					tprintf("Accepting... - ");
					socklen_t        addrlen = 1024;
					struct sockaddr *addr = malloc(addrlen); //* free(addr)'d at `void RemFdPair(struct fdlistHead*, struct fdlist*)`
					int clfd = accept4(fd_list.fd, addr, &addrlen, SOCK_NONBLOCK);
					if(clfd == -1) {
						ExitErrno(27, false);
					}

					printSockaddr(addrlen, addr);

					int svfd = getRemoteConnection(server);

					AddFdPair(&fd_list, clfd, svfd, addr, addrlen);
					FD_CLR(fd_list.fd, &set.read);
				}
				for(struct fdlist* list = fd_list.next; list != NULL; list = list == NULL ? NULL : list->next) {
					tprintf("Processing %p for read\n", list);
					INCTAB() { processRead(&fd_list, list, &set, rotateAmount); }
				}
				for(struct fdlist *list = fd_list.next; list != NULL; list = list->next){
					tprintf("Processing %p for write\n", list);
					INCTAB(){ processWrite(list, &(set.write)); }
				}

				char** metadata;
				int handledCount = calcHandled(&fd_list, set, for_calc, &metadata);
				tprintf("Handled %d/%d sockets.\n", handledCount, ready);
				INCTAB(){
					for(int i = 0; i < handledCount; i++){
						tprintf("%s\n", metadata[i]);
					}
				}
			}
		}
		//set = buildSets(&fd_list);
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