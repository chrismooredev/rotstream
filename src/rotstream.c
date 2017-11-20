#define _GNU_SOURCE //Thanks https://github.com/lpeterse/haskell-socket/issues/24

#include "headers/liblogging.h"
#include "headers/librotstream.h"
#include "headers/libversion.h"
#include "headers/libargs.h"

bool           shouldTerminate;
int8_t  rotateAmount;
//struct timeval timeout_ref = {.tv_sec = 10, .tv_usec = 0};
//struct timeval timeout_ref = {.tv_sec = 0, .tv_usec = 1000000};
struct timeval timeout_ref = {.tv_sec = 1, .tv_usec = 0};

int main(int argc, char* argv[]) {
#ifdef DEBUG
	setbuf(stdout, NULL);
#endif
#ifdef __WINNT
	WSADATA wsaData;
	WSAStartup(0x0202, &wsaData); //Version 2.2 of Winsock.dll? // https://msdn.microsoft.com/en-us/library/windows/desktop/ms742213(v=vs.85).aspx
#endif
	struct arguments args;
	handleArguments(&argc, argv, &args);

	rotateAmount = args.rot; // Normalize the cipher, Negatives are preserved
	if(rotateAmount == 0){
		tnprintf("Warning: The RotateAmount is 0! This will act as an ordinary TCP Stream wrapper, without masking contents.");
	}
	tnprintf("Rotate Amount(nor): %d", rotateAmount);
	struct addrinfo *server = args.dst;
	struct addrinfo *listen = args.src;
	/*
	struct sockaddr_in {
		short	sin_family;
		u_short	sin_port;
		struct in_addr	sin_addr;
		char	sin_zero[8];
	};
	http://www.nightmare.com/medusa/async_sockets.html
	http://beej.us/guide/bgnet/output/html/multipage/advanced.html
	*/

#ifdef __WINNT
	SetConsoleCtrlHandler(handler_SIGINT, true);
#elif __linux
	struct sigaction sigintHandler;
	sigintHandler.sa_handler = handler_SIGINT;
	sigemptyset(&sigintHandler.sa_mask);
	sigintHandler.sa_flags = 0;
	sigaction(SIGINT, &sigintHandler, NULL);
#endif

	//TODO: Setup listening socket
	int listenSocketCount = 0;
	struct addrinfo* _tmp              = listen;
	while(_tmp != NULL){
		listenSocketCount++;
		_tmp = _tmp->ai_next;
	}
	tnprintf("Listen socket count: %d", listenSocketCount);
	Socket listenSockets[listenSocketCount];
	//int listenSocket = 
	getServerSocket(listen, listenSocketCount, listenSockets);

	//* Setup connection socket to Server
	//int masterServer = getServerSocket(beginning);

	//TODO: Start listening shit
	//* Setup `select()` data
	struct timeval   timeout = timeout_ref;
	struct fdlistenHead fd_list = {
		.count = listenSocketCount,
		.fds = listenSockets,
		.next = NULL};
	//struct fdlistHead fd_list = {.fd = listenSocket, .next = NULL};

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
	/*
	int select(
		int nfds,
		fd_set *readfds,
		fd_set *writefds,
        fd_set *exceptfds,
		struct timeval *timeout);
	*/

	struct fd_setcollection set = buildSets(&fd_list);
	struct fd_setcollection for_calc;
	for(int ready = 0; !shouldTerminate; timeout = timeout_ref, set = buildSets(&fd_list), ready = select(calcNfds(&fd_list, set), &set.read, &set.write, &set.except, &timeout)) {
		if(shouldTerminate)
			break;
		if(ready != 0)
			_printf("\n");
		if(ready == 0) {
			//tprintf("Waiting for connection...\n");
			_printf(".");
		} else if(ready == -1) {
			if(LAST_ERROR != EBADF) {
				ExitErrno(200, false);
			} else {
				//TODO: Remove bad FD
				tprintf("Must remove bad FD from socket\n");
			}
		} else {
			tnprintf("Handling %d connection%s...", ready, ready != 1 ? "s" : "");
			INCTAB() {
				for_calc = set;
				//TODO: bool accepted = acceptConnection(struct fdlistHead * list, fd_list read);
				for(int i = 0; i < fd_list.count; i++)
					if(FD_ISSET(fd_list.fds[i], &set.read)) {
						tprintf("Accepting... - ");
						socklen_t        addrlen = 1024;
						struct sockaddr* addr    = malloc(addrlen); //* free(addr)'d at `void RemFdPair(struct fdlistHead*, struct fdlist*)`
						int              clfd    = accept(fd_list.fds[i], addr, &addrlen);
						if(clfd == -1) {
							ExitErrno(27, false);
						}

						printSockaddr(addrlen, addr);

						int svfd = getRemoteConnection(server);

						AddFdPair(&fd_list, clfd, svfd, addr, addrlen);
						FD_CLR(fd_list.fds[i], &set.read);
					}
				for(struct fdlist* list = fd_list.next; list != NULL; list = list == NULL ? NULL : list->next) {
					tprintf("Processing %p for read\n", list);
					INCTAB() { list = processRead(&fd_list, list, &set, rotateAmount); }
					tprintf("Done reading. list = %p, list->next = %p\n", list, list == NULL ? NULL : list->next);
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

	if(shouldTerminate){
		tnprintf("Recieved termination signal... Exiting.");
	}

	#ifdef __WINNT
	WSACleanup();
	#endif

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