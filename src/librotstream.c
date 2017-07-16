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
	while(server != NULL) {
		//int socket(int domain, int type, int protocol);
		sock = socket(server->ai_family, server->ai_socktype | SOCK_NONBLOCK , 0);
		//fcntl(sock, F_SETFL, O_NONBLOCK);
		printf("hai\n");
		if(sock != -1 || sock == -1){
			conres = connect(sock, server->ai_addr, server->ai_addrlen);
			printf("chai\n");
			if(sock != -1 && (conres == 0 || errno == EINPROGRESS))
				break;
		}
		if(sock == -1) {
			int errno_cache = errno;
			close(sock);
			sock = -1;
			errno = errno_cache;
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
void RemFdPair(struct fdlistHead* list, struct fdlist *element){
	tprintf("Removing pair (Client=%d, Server=%d) at %p.\n", element->client.fd, element->server.fd, element);

	close(element->client.fd);
	close(element->server.fd);
	free(element->client.sockaddr); //* malloc(addr)'d at `void RemFdPair(struct fdlistHead*, struct fdlist*)`
	//free(element->server.sockaddr); //Closed when freeaddrinfo is called

	if(list->next == element) {
		list->next = element->next;
	} else {
		for(struct fdlist *elem = list->next, *last = elem; elem != NULL; last = elem, elem = elem->next){
			if(elem == element){
				last->next = element->next; //Works if element->next is NULL or not
			}
		}
	}
	free(element);
}
int calcNfds(struct fdlistHead *list, struct fd_setcollection col) {
	struct fdlist* ls = list->next;
	int            max;
	for(max = list->listenSocket; ls != NULL; ls = ls->next){
		bool cl = FD_ISSET(ls->client.fd, &col.read) || FD_ISSET(ls->client.fd, &col.write);
		bool sv = FD_ISSET(ls->server.fd, &col.read) || FD_ISSET(ls->server.fd, &col.write);;
		max = MAX(max, MAX(cl ? ls->client.fd : 0, sv ? ls->server.fd : 0));
	}
	return max+1;
}
struct fd_setcollection buildSets(struct fdlistHead* list) {
	struct fd_setcollection sets;
	FD_ZERO(&sets.read);
	FD_ZERO(&sets.write);
	FD_ZERO(&sets.except);

	//* FD_SET(fd, set) FD_ISSET(fd, set) FD_ZERO(set) FD_CLR(fd, set)
	//* Add main socket to listen for new connections
	FD_SET(list->listenSocket, &sets.read);

	//* Cycle through, if buffer has no data, add to sets.read
	for(struct fdlist *elem = list->next; elem != NULL; elem = elem->next){
		addToSetIf(elem->client.buf.length == 0, elem->client.fd, &sets.read);
		addToSetIf(elem->server.buf.length == 0, elem->server.fd, &sets.read);
	}

	//* Cycle through, if buffer has data, add to sets.write
	for(struct fdlist *elem = list->next; elem != NULL; elem = elem->next){
		normalizeBuf(&(elem->client.buf));
		normalizeBuf(&(elem->server.buf));

		socklen_t intsize    = sizeof(int);
		int client_err = 0;
		int server_err = 0;
		int client_res = getsockopt(elem->client.fd, SOL_SOCKET, SO_ERROR, &client_err, &intsize);
		int       client_errno = errno;
		int       server_res = getsockopt(elem->server.fd, SOL_SOCKET, SO_ERROR, &server_err, &intsize);
		int       server_errno = errno;

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
void processWrite(struct fdlist* list, fd_set* writeset){
	//int fd = list->client;
	struct fdelem *conn = &list->client;
	#define getOpposite(list, elem) (&list->client == elem ? &list->server : &list->client)

	while(conn != NULL) {
		struct fdelem *opp = getOpposite(list, conn);
		if(FD_ISSET(opp->fd, writeset)) {
			//ssize_t write(int fd, const void *buf, size_t count);
			printf("Write to %d (%s) (Opp buffer contains Len:%zd Ind:%zu)\n", opp->fd, opp->descriptString, conn->buf.length, conn->buf.startIndex);
			ssize_t written = write(opp->fd, conn->buf.buf + conn->buf.startIndex, conn->buf.length - conn->buf.startIndex) - 1;
			if(written == -1 && errno == EINPROGRESS) {
				printf("Had an error that no-one cares about.");
			} else if(written == -1) {
				ExitErrno(101, false);
			} else {
				tprintf("Written to %s: %d bytes\n", opp->descriptString, written);
				readfromBuf(&conn->buf, written);
			}
			FD_CLR(opp->fd, writeset);
		}
		conn = conn == &list->client ? &list->server : NULL;
	}
}