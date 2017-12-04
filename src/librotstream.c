#include "headers/librotstream.h"

void rotate(int8_t rotateBy, uint8_t* buf, size_t length) {
	for(size_t i = 0; i < length; i++) {
		buf[i] += rotateBy;
	}
}
struct in_addr ConvertIPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
	uint32_t asInt = htonl((a << 3 * 8) | (b << 2 * 8) | (c << 1 * 8) | (d << 0 * 8));
	return *(struct in_addr*) &asInt; // "Cast" type from uint32_t to in_addr struct
}

size_t removeIndex(size_t index, size_t len, char** arr){
	if(index >= len){
		SET_LAST_ERROR(ERANGE);
		return -1;
	}
	
	for(int x = index; x < len-1; x++) //Shift all elements up one
		arr[x] = arr[x+1];

	arr[len - 1] = NULL; //Set last element to NULL
	return len-1; //Return the new size
}
void printListHeader(char* header, size_t len, char** list){
	tprintf("%s: (%"PRI_SIZET")\n", header, len);
	INCTAB() for(int i = 0; i < len; i++){
		tprintf("%d (%p)", i, *list+i);
		tprintf("%s\n", list[i]);
	}
}

void populateHints(struct addrinfo* hints, int* argc, char* argv[]) {
	assert(!"This function shouldn't be used.");
	bool foundFour = false;
	bool foundSix = false;
	
	for(int i = 0; i < *argc; i++) {
		if(strcmp("-4", argv[i]) == 0){
			foundFour = i;
			*argc     = removeIndex(i, *argc, argv);
		}
		if(strcmp("-6", argv[i]) == 0){
			foundSix = i;
			*argc     = removeIndex(i, *argc, argv);
		}
	}

	if(foundFour && foundSix){
		tprintf("Arguments '-4' and '-6' cannot be used simultaneously.");
		Exit(5, true);
	}

	memset(hints, 0, sizeof(struct addrinfo));
	// *hints = (struct addrinfo) {0};
	hints->ai_family = AF_UNSPEC;

	if(foundFour)
		hints->ai_family = AF_INET;
	else if(foundSix)
		hints->ai_family = AF_INET6;
	else
		hints->ai_family = AF_UNSPEC;
}

void getServerSocket(struct addrinfo* server, int sockcount, Socket* sockarr){
	//struct addrinfo* orig_server = server;
	int              count       = 0;

	while(server != NULL){
		//Socket *sock = &sockarr[count];
		Socket  sock = socket(server->ai_family, server->ai_socktype, 0);
		printErrIfErr(sock, LAST_ERROR);
		IFLOG(LOG_SOCKCRTE) tnprintf("Socket: Created");
		//TODO: EACCES EAFNOSUPPORT EINVAL EMFILE ENFILE ENOBUFS ENOMEM EPROTONOSUPPORT
		//TODO: WSAEMFILE WSAENOBUF
		assert(socketValid(sock));
		setSocketNonblocking(sock);

		INCTAB(){
			//Allows quick testing by enabling attaching to a used port
			int enable = 1; //https://stackoverflow.com/a/24194999/3439288
			if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &enable, sizeof(enable)) < 0)
				perror("setsockopt(SO_REUSEADDR) failed");

			if(server->ai_family == AF_INET6){
				int v6only = 1;
				if (setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (char*) &v6only, sizeof(v6only)) < 0)
					perror("setsockopt(IPV6_V6ONLY) failed");
			}

			int bindres = bind(sock, server->ai_addr, server->ai_addrlen);
			printErrIfErr(bindres, LAST_ERROR);
			IFLOG(LOG_SOCKCRTE) tnprintf("Socket: Bound");
			assert(bindres != -1); //Couldn't find any valid (probable) error cases for `bind'
			//TODO: EACCES EADDRINUSE EBADF EINVAL ENOTSOCK EADDRNOTAVAIL EFAULT ELOOP ENAMETOOLONG ENOENT ENOMEM ENOTDIR EROFS
			//TODO: WSAENETDOWN WSAENOBUFS

			int lisres = listen(sock, 10);
			printErrIfErr(lisres, LAST_ERROR);
			IFLOG(LOG_SOCKCRTE) tnprintf("Socket: Listening");
			assert(lisres != -1);
			//TODO: EADDRINUSE EOPNOTSUPP

			//if(socketValid(sock) && bindres == 0 && lisres == 0)
			//	break;
		}
		sockarr[count++] = sock;
		server = server->ai_next;
	}
}
Socket getRemoteConnection(struct addrinfo* server){
	Socket sock    = SOCKET_INVALID;
	int conres = -1;
	while(server != NULL) {
		//int socket(int domain, int type, int protocol);
		sock = socket(server->ai_family, server->ai_socktype, 0);

		IFLOG(LOG_SOCKCRTE) tnprintf("Created Socket. Creating Connection...");
		if(socketValid(sock)){
			setSocketNonblocking(sock);
			conres = connect(sock, server->ai_addr, server->ai_addrlen);

			if(socketValid(sock) && (conres == 0 || LAST_ERROR == EINPROGRESS || LAST_ERROR == EWOULDBLOCK)){
				IFLOG(LOG_SOCKCRTE) INCTAB() { tnprintf("connect() successful!"); }
				break;
			} else {
				int   err    = LAST_ERROR;
				char* errStr = getErrorMessage(err);
				tprintf("Checking result of connect()...  (%d) (Errno = %d - %s)\n", conres, conres != 0 ? err : 0, errStr);
				free(errStr);
			}
		} else {
			tprintf("Socket creation failed. Closing it.");
			CLOSE_SOCKET(sock);
		}
		sock   = SOCKET_INVALID;
		conres = -1;
		server  = server->ai_next;
	}
	IFLOG(LOG_SOCKCRTE) tprintf("Sock: %"PRI_SOCKT", Connect: %d\n", sock, conres);
	if(socketInvalid(sock)) {
		ExitErrno(67, "socket(2)");
	} else if(conres != 0 && LAST_ERROR != EINPROGRESS && LAST_ERROR != EWOULDBLOCK) {
		ExitErrno(68, "connect(2)");
	}

	return sock;
}

struct fdlist* AddFdPair(struct fdlistenHead *head, int client, int server, struct sockaddr *addr, socklen_t addrlen){
	struct fdlist* ent = calloc(1, sizeof(struct fdlist));

	tprintf("Adding pair: Client=%d and Server=%d\n", client, server);

	setSocketNoTcpDelay(client);
	setSocketNoTcpDelay(server);

	ent->client.fd = client;
	ent->server.fd = server;

	ent->client.descriptString = "client";
	ent->server.descriptString = "server";

	ent->client.sockaddr = addr;
	ent->client.sockaddrlen = addrlen;

	ent->next = NULL;

	if(head->next == NULL)
		head->next      = ent;
	else {
		struct fdlist* list = head->next;
		struct fdlist* prev = list;
		for(; list != NULL; prev = list, list = list->next);
		prev->next = ent;
	}

	return ent;
}
struct fdlist* RemFdPair(struct fdlistenHead* list, struct fdlist *element){
	tprintf("Removing pair (Client=%"PRI_SOCKT", Server=%"PRI_SOCKT") at %p.\n", element->client.fd, element->server.fd, element);
	struct fdlist *last;
	INCTAB(){
		CLOSE_SOCKET(element->client.fd);
		CLOSE_SOCKET(element->server.fd);
		free(element->client.sockaddr); // * malloc(addr)'d at `void RemFdPair(struct fdlistHead*, struct fdlist*)`
		//free(element->server.sockaddr); //Closed when freeaddrinfo is called

		tprintf("Next element is at %p.\n", element->next);

		if(list->next == element) {
			//tprintf("First element. element->next = %p\n", element->next);
			list->next = element->next;
			last       = NULL;
		} else {
			last = list->next;
			for(struct fdlist *elem = list->next; elem != NULL; elem = elem->next){
				tprintf("Current Element: %p, Last Element: %p (Searching for %p)\n", elem, last, element);
				if(elem == element){
					tprintf("Putting next of %p as %p\n", last, elem->next);
					last->next = elem->next; //Works if element->next is NULL or not
					tprintf("last: %p, last->next: %p\n", last, last->next);
					break;
				}
				last = elem;
			}
			//tprintf("LastElement: %p\n", last);
		}
		//tprintf("Last element was %p\n", last);

		free(element); //TODO: Free it
	}
	return last;
}
int calcNfds(struct fdlistenHead *list, struct fd_setcollection col) {
	int            max = 0; //Highest file descriptor
	for(int i = 0; i < list->count; i++)
		max = MAX(max, list->fds[i]);
	for(struct fdlist* ls = list->next; ls != NULL; ls = ls->next) {
		bool cl = FD_ISSET(ls->client.fd, &col.read) || FD_ISSET(ls->client.fd, &col.write);
		bool sv = FD_ISSET(ls->server.fd, &col.read) || FD_ISSET(ls->server.fd, &col.write);
		max = MAX(max, MAX(cl ? ls->client.fd : 0, sv ? ls->server.fd : 0));
	}
	return max+1;
}
struct fd_setcollection buildSets(struct fdlistenHead* list) {
	#define skipLogPair(el) do { \
		if(!isValidFd(el->client.fd)){ \
			tprintf("Client FD %"PRI_SOCKT" from element %p is not valid!\n", el->client.fd, el); \
			continue; \
		} \
		if(!isValidFd(el->server.fd)){ \
			tprintf("Server FD %"PRI_SOCKT" from element %p is not valid!\n", el->server.fd, el); \
			continue; \
		} \
	} while(false)
    struct fd_setcollection sets;
    FD_ZERO(&sets.read);
	FD_ZERO(&sets.write);
	FD_ZERO(&sets.except);

	// * FD_SET(fd, set) FD_ISSET(fd, set) FD_ZERO(set) FD_CLR(fd, set)
	// * Add main socket to listen for new connections
	for(int i = 0; i < list->count; i++)
		FD_SET(list->fds[i], &sets.read);

	// * Cycle through, if buffer has no data, add to sets.read
	for(struct fdlist *elem = list->next; elem != NULL; elem = elem->next){
		skipLogPair(elem);
		addToSetIf(elem->client.buf.length == 0, elem->client.fd, &sets.read);
		addToSetIf(elem->server.buf.length == 0, elem->server.fd, &sets.read);
	}

	// * Cycle through, if buffer has data, add to sets.write
	for(struct fdlist *elem = list->next; elem != NULL; elem = elem->next){
		normalizeBuf(&(elem->client.buf));
		normalizeBuf(&(elem->server.buf));
		skipLogPair(elem);

		//TODO: Eliminate this section of code, it hasn't seemed to have errored in ages
		socklen_t	intsize = sizeof(int);
		int	client_err      = 0;
		int	server_err      = 0;
		int	client_res      = getsockopt(elem->client.fd, SOL_SOCKET, SO_ERROR, (char*) &client_err, &intsize);
		int	client_errno    = LAST_ERROR;
		int	server_res      = getsockopt(elem->server.fd, SOL_SOCKET, SO_ERROR, (char*) &server_err, &intsize);
		int	server_errno    = LAST_ERROR;

		// * The sockets are waiting for data on the other stream, so check the other one
		addToSetIf(elem->server.buf.length != 0, elem->client.fd, &sets.write);
		addToSetIf(elem->client.buf.length != 0, elem->server.fd, &sets.write);

		if(client_res == -1 || server_res == -1){
			char *cen = getErrorMessage(client_errno);
			char *sen = getErrorMessage(server_errno);
			tprintf("###########Client_res: %s, Server_res: %s\n", cen, sen);
			free(cen);
			free(sen);
		}

		if(FD_ISSET(elem->client.fd, &sets.write)){
			if(client_err != 0){
				char* errStr = getErrorMessage(client_err);
				tprintf("Warning: socket %"PRI_SOCKT" set to write even though it has error %d (%s)\n", elem->client.fd, client_err, errStr);
				free(errStr);
			}
		}
		if(FD_ISSET(elem->server.fd, &sets.write)){
			if(server_err != 0){
				char* errStr = getErrorMessage(server_err);
				tprintf("Warning: socket %"PRI_SOCKT" set to write even though it has error %d (%s)\n", elem->server.fd, server_err, errStr);
				free(errStr);
			}
		}
	}

	return sets;
}
void normalizeBuf(struct buffer1k *buffer){
	size_t len = buffer->length - buffer->startIndex;
	//tprintf("Buf %p, Src %p, totalLen %d (Len %d, Ind %d)\n", buffer->buf, buffer->buf + buffer->startIndex, len, buffer->length, buffer->startIndex);
	if(buffer->length == 0) {
		buffer->startIndex = 0;
	} else if(buffer->startIndex != 0) {
		memmove(buffer->buf, buffer->buf + buffer->startIndex, len);
		buffer->length     = buffer->length - buffer->startIndex;
		buffer->startIndex = 0;
	}
}
void readfromBuf(struct buffer1k *buffer, ssize_t amount){
	assert((amount >= 0));
	assert((buffer->startIndex + amount <= buffer->length));

	if(amount == buffer->length){
		buffer->length = 0;
		buffer->startIndex = 0;
	} else {
		buffer->startIndex += amount;

		if(buffer->startIndex == buffer->length - 1){
			buffer->length = 0;
			buffer->startIndex = 0;
		}
		normalizeBuf(buffer);
	}
}


struct fdlist* processRead(struct fdlistenHead* head, struct fdlist* list, struct fd_setcollection* set, int8_t rotateAmount){
	struct fdelem *connection = &list->client;
	while(list != NULL && connection != NULL){
		IFLOG(LOG_PROC_READ) tprintf("Handing %s...\n", connection->descriptString);
		if (FD_ISSET(connection->fd, &set->read)) {
			assert(connection->buf.length == 0); //Shouldn't be reading with data still in the buffer

			connection->buf.length = recv(connection->fd, (char*) connection->buf.buf, sizeof(connection->buf.buf), 0);
			
			if(connection->buf.length == 0 || (connection->buf.length == -1 && (LAST_ERROR == ECONNRESET))){ //* EOF
				INCTAB() {
					IFLOG(LOG_PROC_READ)
						if(connection->buf.length == 0)
							tnprintf("Recieved zero bytes: EOF");
						else if(LAST_ERROR == ECONNRESET)
							tnprintf("Recieved ECONNRESET Error. Removing connection pair.");
					FD_CLR(connection->fd, &set->read);
					FD_CLR(connection->fd, &set->write);
					FD_CLR(getOpposite(list, connection)->fd, &set->read);
					FD_CLR(getOpposite(list, connection)->fd, &set->write);
					IFLOG(LOG_PROC_READ) tprintf("Calling RemFdPair(%p, %p)\n", head, list);
					list = RemFdPair(head, list);
					IFLOG(LOG_PROC_READ) tprintf("After RemFdPair: list = %p\n", list);
				}
				//break;
			} else if(connection->buf.length == -1) { //* ERROR
				connection->buf.length = 0;
				ExitErrno(90, false);
			} else {
				IFLOG(LOG_PROC_READ) tprintf("Got data from %s. (%"PRI_SSIZET" bytes)\n", connection->descriptString, connection->buf.length);
				rotate(rotateAmount, connection->buf.buf, connection->buf.length);
				FD_CLR(connection->fd, &set->read);
			}
		}
		//tprintf("connection = %p, list = %p\n", connection, list);

		if(list == NULL)
			connection = NULL;
		else
			connection = connection == &list->client ? &list->server : NULL;
	}
	//tprintf("Reached end of processRead\n");
	return list;
}
void processWrite(struct fdlist* list, fd_set* writeset){
	//int fd = list->client;
	struct fdelem *conn = &list->client;
	
	while(conn != NULL) {
		struct fdelem *opp = getOpposite(list, conn);

		if(FD_ISSET(opp->fd, writeset)) {
			//ssize_t write(int fd, const void *buf, size_t count);
			IFLOG(LOG_PROC_WRITE) tnprintf("Write to %"PRI_SOCKT" (%s) (Opp buffer contains Len:%" PRI_SSIZET " Ind:%" PRI_SIZET ")", opp->fd, opp->descriptString, conn->buf.length, conn->buf.startIndex);
			//written = send(sockfd, buf, len, flags)
			ssize_t written = send(opp->fd, (char*) (conn->buf.buf + conn->buf.startIndex), conn->buf.length - conn->buf.startIndex, 0);

			if(written == -1 && LAST_ERROR == EINPROGRESS) {
				IFLOG(LOG_PROC_WRITE) tnprintf("Had an error that no-one cares about. (EINPROGRESS)");
			} else if(written == -1) {
				ExitErrno(101, false);
			} else {
				IFLOG(LOG_PROC_WRITE) tnprintf("Written to %s: %"PRI_SIZET" bytes", opp->descriptString, written);
				readfromBuf(&conn->buf, written);
				FD_CLR(opp->fd, writeset);
			}
		}
		conn = conn == &list->client ? &list->server : NULL;
	}
}
int calcHandled(struct fdlistenHead *head, struct fd_setcollection actedOn, struct fd_setcollection fromSelect, char*** metadata){
	IFLOG(LOG_CALCHAND) tnprintf("Creating list of stuff for calcHandler errors");
	bool log = *metadata != NULL;
	int  removed = 0;
	INCTAB() {
		for(int i = 0; i < head->count; i++){
			removed += FD_ISSET(head->fds[i], &fromSelect.read) && !FD_ISSET(head->fds[i], &actedOn.read) ? 1 : 0;
		}

		//if fromSelect && !fromActedOn
		for(struct fdlist* list = head->next; list != NULL; list = list->next) {
			struct fdelem *conn = &list->client;
			while((conn = conn == &list->client ? &list->server : NULL) != NULL) { //First section is ran, then result of conditional is used for loop
				if(FD_ISSET(conn->fd, &fromSelect.except))
					tfprintf(stderr, "File Descriptor %"PRI_SOCKT" (%s) was in the `except` list.\n", conn->fd, conn->descriptString);
				
				removed += FD_ISSET(conn->fd, &fromSelect.read) && !FD_ISSET(conn->fd, &actedOn.read) ? 1 : 0;
				removed += FD_ISSET(conn->fd, &fromSelect.write) && !FD_ISSET(conn->fd, &actedOn.write) ? 1 : 0;
				removed += FD_ISSET(conn->fd, &fromSelect.except) && !FD_ISSET(conn->fd, &actedOn.except) ? 1 : 0;
			}
		}

		if(log){
			*metadata = calloc(removed + 1, sizeof(char*));
			int r = 0; //metadata index
			IFLOG(LOG_CALCHAND) tprintf("Allocated data\n");
			for(struct fdlist* list = head->next; list != NULL; list = list->next) {
				struct fdelem *conn = &list->client;
				while((conn = conn == &list->client ? &list->server : NULL) != NULL) { //First section is ran, then result of conditional is used for loop
					char buf[1024];
					
					#define lol(fd, origSet, newSet, cat) \
						if(FD_ISSET(fd, &(origSet.cat)) && !FD_ISSET(fd, &(newSet.cat))) { \
						sprintf(buf, "File Descriptor %"PRI_SOCKT" cleared from %s set.", fd, #cat); \
						(*metadata)[r++] = strdup(buf); }

					lol(conn->fd, fromSelect, actedOn, read);
					lol(conn->fd, fromSelect, actedOn, write);
					lol(conn->fd, fromSelect, actedOn, except);
				}
			}
		}
		IFLOG(LOG_CALCHAND) tprintf("Done creating stuff\n");
	}
	return removed;
}
bool setSocketNonblocking(Socket sock){
#ifdef BUILD_LINUX
	int flags = fcntl(sock, F_GETFL, 0);
	assert(flags != -1);
	flags |= O_NONBLOCK;
	int rtn = fcntl(sock, F_SETFL, flags);
	return rtn == 0;
#elif defined(BUILD_WIN32)
	u_long argp = true;
	int rtn = ioctlsocket(sock, FIONBIO, &argp);
	return rtn == 0;
#else
#error Unsupported Compiler Target
#endif
}
bool setSocketNoTcpDelay(Socket sock) {
	int flag = 1;
	//int setsockopt(sock, level, optname, optval, optlen)
	int res = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag));
	return res == 0;
}

/*
	Ctrl-C - Terminates application by setting (_terminate = true)
*/
#ifdef BUILD_LINUX
void handler_SIGINT(int s) {
	shouldTerminate = true;
}
#elif defined(BUILD_WIN32)
BOOL handler_SIGINT(DWORD dwCtrlType) {
	switch(dwCtrlType){
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
			shouldTerminate = true;
			return true;
		default:
			return false;
	}
}
#endif
