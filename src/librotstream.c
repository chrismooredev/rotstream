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
		errno = ERANGE;
		return -1;
	}
	
	for(int x = index; x < len-1; x++) //Shift all elements up one
		arr[x] = arr[x+1];

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

void populateHints(struct addrinfo* hints, int* argc, char* argv[]) {
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
		printf("Arguments '-4' and '-6' cannot be used simultaneously.");
		Exit(5, true);
	}

	*hints = (struct addrinfo) {0};
	hints->ai_family = AF_UNSPEC;

	if(foundFour)
		hints->ai_family = AF_INET;
	else if(foundSix)
		hints->ai_family = AF_INET6;
	else
		hints->ai_family = AF_UNSPEC;
}

int getServerSocket(struct addrinfo* server){
	int sock    = -1;
	int bindres = -1;
	int lisres  = -1;
	while(server != NULL) {
		//int socket(int domain, int type, int protocol);
		sock = socket(server->ai_family, server->ai_socktype | SOCK_NONBLOCK, 0);
		
		int enable = 1; //https://stackoverflow.com/a/24194999/3439288
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
			perror("setsockopt(SO_REUSEADDR) failed");

		printf("hai\n");
		if(sock != -1){
			bindres = bind(sock, server->ai_addr, server->ai_addrlen);
			printf("chai\n");
			if(bindres != -1) {
				lisres = listen(sock, 10);
				printf("tea\n");
				if(sock != -1 && bindres == 0 && lisres == 0)
					break;
			}
		}
		if(sock == -1) {
			int errno_cache = errno;
			close(sock);
			sock = -1;
			errno = errno_cache;
		}
		sock   = -1;
		bindres = -1;
		lisres  = -1;
		server  = server->ai_next;
	}
	printf("Sock: %d, Bind: %d, Listen: %d\n", sock, bindres, lisres);
	if(sock == -1) {
		ExitErrno(7, "socket(2)");
	} else if(bindres == -1) {
		ExitErrno(8, "bind(2)");
	} else if(lisres == -1) {
		ExitErrno(9, "listen(2)");
	}

	return sock;
}
int getRemoteConnection(struct addrinfo* server){
	int sock    = -1;
	int conres = -1;
	int temperrno;
	while(server != NULL) {
		//int socket(int domain, int type, int protocol);
		sock = socket(server->ai_family, server->ai_socktype | SOCK_NONBLOCK , 0);
		//fcntl(sock, F_SETFL, O_NONBLOCK);
		printf("Created Socket. Creating Connection...\n");
		if(sock != -1 || sock == -1){
			errno = EOK;
			conres = connect(sock, server->ai_addr, server->ai_addrlen);
			printf("Checking result of connect()...\n");
			if(sock != -1 && (conres == 0 || errno == EINPROGRESS))
				break;
		}
		if(sock == -1) {
			int errno_cache = errno;
			close(sock);
		}
		sock   = -1;
		conres = -1;
		server  = server->ai_next;
	}
	printf("Sock: %d, Connect: %d\n", sock, conres);
	if(sock == -1) {
		ExitErrno(67, "socket(2)");
	} else if(conres == -1 && errno != EINPROGRESS) {
		ExitErrno(68, "connect(2)");
	}

	return sock;
}

struct fdlist* AddFdPair(struct fdlistHead *head, int client, int server, struct sockaddr *addr, socklen_t addrlen){
	struct fdlist* ent = calloc(1, sizeof(struct fdlist));

	tprintf("Adding pair: Client=%d and Server=%d\n", client, server);

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
struct fdlist* RemFdPair(struct fdlistHead* list, struct fdlist *element){
	tprintf("Removing pair (Client=%d, Server=%d) at %p.\n", element->client.fd, element->server.fd, element);
	struct fdlist *last;
	INCTAB(){
		close(element->client.fd);
		close(element->server.fd);
		free(element->client.sockaddr); //* malloc(addr)'d at `void RemFdPair(struct fdlistHead*, struct fdlist*)`
		//free(element->server.sockaddr); //Closed when freeaddrinfo is called

		tprintf("Next element is at %p.\n", element->next);

		if(list->next == element) {
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
			tprintf("LastElement: %p\n", last);
		}
		tprintf("Last element was %p\n", last);

		free(element); //TODO: Free it
	}
	return last;
}
int calcNfds(struct fdlistHead *list, struct fd_setcollection col) {
	struct fdlist* ls = list->next;
	int            max;
	for(max = list->fd; ls != NULL; ls = ls->next){
		bool cl = FD_ISSET(ls->client.fd, &col.read) || FD_ISSET(ls->client.fd, &col.write);
		bool sv = FD_ISSET(ls->server.fd, &col.read) || FD_ISSET(ls->server.fd, &col.write);;
		max = MAX(max, MAX(cl ? ls->client.fd : 0, sv ? ls->server.fd : 0));
	}
	return max+1;
}
struct fd_setcollection buildSets(struct fdlistHead* list) {
	#define isValidFd(fd) (fcntl(fd, F_GETFD) != -1 || errno != EBADF)
	#define skipLogPair(el) do { \
		if(!isValidFd(el->client.fd)){ \
			printf("Client FD %d from element %p is not valid!\n", el->client.fd, el); \
			continue; \
		} \
		if(!isValidFd(el->server.fd)){ \
			printf("Server FD %d from element %p is not valid!\n", el->server.fd, el); \
			continue; \
		} \
	} while(false)
    struct fd_setcollection sets;
    FD_ZERO(&sets.read);
	FD_ZERO(&sets.write);
	FD_ZERO(&sets.except);

	//* FD_SET(fd, set) FD_ISSET(fd, set) FD_ZERO(set) FD_CLR(fd, set)
	//* Add main socket to listen for new connections
	FD_SET(list->fd, &sets.read);

	//* Cycle through, if buffer has no data, add to sets.read
	for(struct fdlist *elem = list->next; elem != NULL; elem = elem->next){
		skipLogPair(elem);
		addToSetIf(elem->client.buf.length == 0, elem->client.fd, &sets.read);
		addToSetIf(elem->server.buf.length == 0, elem->server.fd, &sets.read);
	}

	//* Cycle through, if buffer has data, add to sets.write
	for(struct fdlist *elem = list->next; elem != NULL; elem = elem->next){
		normalizeBuf(&(elem->client.buf));
		normalizeBuf(&(elem->server.buf));
		skipLogPair(elem);

		socklen_t	intsize = sizeof(int);
		int	client_err      = 0;
		int	server_err      = 0;
		int	client_res      = getsockopt(elem->client.fd, SOL_SOCKET, SO_ERROR, &client_err, &intsize);
		int	client_errno    = errno;
		int	server_res      = getsockopt(elem->server.fd, SOL_SOCKET, SO_ERROR, &server_err, &intsize);
		int	server_errno    = errno;

		//* The sockets are waiting for data on the other stream, so check the other one
		addToSetIf(elem->server.buf.length != 0, elem->client.fd, &sets.write);
		addToSetIf(elem->client.buf.length != 0, elem->server.fd, &sets.write);

		if(client_res == -1 || server_res == -1){
			tprintf("Client_res: %s, Server_res: %s\n", strerror(client_errno), strerror(server_errno));
		}

		if(FD_ISSET(elem->client.fd, &sets.write)){
			if(client_err != 0){
				tprintf("Warning: socket %d set to write even though it has error %d (%s)\n", elem->client.fd, client_err, strerror(client_err));
			}
		}		if(FD_ISSET(elem->server.fd, &sets.write)){
			if(server_err != 0){
				tprintf("Warning: socket %d set to write even though it has error %d (%s)\n", elem->server.fd, server_err, strerror(server_err));
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


void processRead(struct fdlistHead* head, struct fdlist* list, struct fd_setcollection* set, int8_t rotateAmount){
	struct fdelem *connection = &list->client;
	while(connection != NULL){

		if(FD_ISSET(connection->fd, &set->read)) {
			assert(connection->buf.length == 0);

			connection->buf.length = read(connection->fd, connection->buf.buf, sizeof(connection->buf.buf));
			//TODO: SHIT
			if(connection->buf.length == 0){ //* EOF
				INCTAB() {
					tprintf("Recieved zero bytes: EOF\n");
					FD_CLR(connection->fd, &set->read);
					FD_CLR(connection->fd, &set->write);
					FD_CLR(getOpposite(list, connection)->fd, &set->read);
					FD_CLR(getOpposite(list, connection)->fd, &set->write);
					list = RemFdPair(head, list);
					//connection = NULL; //End loop
				}
				//tablevel--;
				break;
			} else if(connection->buf.length == -1) { //* ERROR
				ExitErrno(90, false);
				connection->buf.length = 0;
			} else {
				tprintf("Got data from %s. (%d bytes)\n", connection->descriptString, connection->buf.length);
				rotate(rotateAmount, connection->buf.buf, connection->buf.length);
				FD_CLR(connection->fd, &set->read);
			}
		}

		connection = connection == &list->client ? &list->server : NULL;
	}
}
void processWrite(struct fdlist* list, fd_set* writeset){
	//int fd = list->client;
	struct fdelem *conn = &list->client;
	
	while(conn != NULL) {
		struct fdelem *opp = getOpposite(list, conn);

		if(FD_ISSET(opp->fd, writeset)) {
			//ssize_t write(int fd, const void *buf, size_t count);
			tprintf("Write to %d (%s) (Opp buffer contains Len:%zd Ind:%zu)\n", opp->fd, opp->descriptString, conn->buf.length, conn->buf.startIndex);
			ssize_t written = write(opp->fd, conn->buf.buf + conn->buf.startIndex, conn->buf.length - conn->buf.startIndex);
			if(written == -1 && errno == EINPROGRESS) {
				tprintf("Had an error that no-one cares about. (EINPROGRESS)");
			} else if(written == -1) {
				ExitErrno(101, false);
			} else {
				tprintf("Written to %s: %d bytes\n", opp->descriptString, written);
				readfromBuf(&conn->buf, written);
				FD_CLR(opp->fd, writeset);
			}
		}
		conn = conn == &list->client ? &list->server : NULL;
	}
}
int calcHandled(struct fdlistHead *head, struct fd_setcollection actedOn, struct fd_setcollection fromSelect, char*** metadata){
	bool log = *metadata != NULL;

	int removed = FD_ISSET(head->fd, &fromSelect.read) && !FD_ISSET(head->fd, &actedOn.read) ? 1 : 0;
	//if fromSelect && !fromActedOn
	for(struct fdlist* list = head->next; list != NULL; list = list->next) {
		struct fdelem *conn = &list->client;
		while((conn = conn == &list->client ? &list->server : NULL) != NULL) { //First section is ran, then result of conditional is used for loop
			if(FD_ISSET(conn->fd, &fromSelect.except))
				printf("File Descriptor %d (%s) was in the `except` list.\n", conn->fd, conn->descriptString);
			
			removed += FD_ISSET(conn->fd, &fromSelect.read) && !FD_ISSET(conn->fd, &actedOn.read) ? 1 : 0;
			removed += FD_ISSET(conn->fd, &fromSelect.write) && !FD_ISSET(conn->fd, &actedOn.write) ? 1 : 0;
			removed += FD_ISSET(conn->fd, &fromSelect.except) && !FD_ISSET(conn->fd, &actedOn.except) ? 1 : 0;
		}
	}

	if(log){
		tprintf("Creating list of stuff\n");
		*metadata = calloc(removed + 1, sizeof(char*));
		int r = 0;

		tprintf("Allocated data\n");
		for(struct fdlist* list = head->next; list != NULL; list = list->next) {
			struct fdelem *conn = &list->client;
			while((conn = conn == &list->client ? &list->server : NULL) != NULL) { //First section is ran, then result of conditional is used for loop
				char buf[1024];
				
				#define lol(fd, origSet, newSet, cat) \
					if(FD_ISSET(fd, &(origSet.cat)) && !FD_ISSET(fd, &(newSet.cat))) { \
					sprintf(buf, "File Descriptor %d cleared from %s set.", fd, #cat); \
					(*metadata)[r++] = strdup(buf); }

					// *((*metadata) + 1)

				lol(conn->fd, fromSelect, actedOn, read);
				lol(conn->fd, fromSelect, actedOn, write);
				lol(conn->fd, fromSelect, actedOn, except);
			}
		}
	}

	tprintf("Done creating stuff\n");

	return removed;
}