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

struct fdlist* AddFdPair(struct fdlistHead *head, struct fd_setcollection *fds, int client, int server){
	struct fdlist* ent = malloc(sizeof(struct fdlist));

	ent->client = client;
	ent->server = server;
	ent->next       = NULL;

	//TODO: Enable FDs for write-checking
	FD_SET(client, &(fds->read));
	FD_SET(server, &(fds->read));

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
void RemFdPair(struct fdlistHead* list, struct fd_setcollection* fds, struct fdlist *element){
	
}
int calcNfds(struct fdlistHead *list){
	struct fdlist* ls = list->next;
	int            max;
	for(max = list->listenSocket; ls != NULL; ls = ls->next)
		max = MAX(max, MAX(ls->client, ls->server));
	return max+1;
}