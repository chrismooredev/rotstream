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
	int sock = -1;
	int bindres = -1;
	while(server != NULL){
		//int socket(int domain, int type, int protocol);
		sock = socket(server->ai_family, server->ai_socktype, SOCK_NONBLOCK);
		printf("hai\n");
		if(sock == -1){
			server = server->ai_next;
			continue;
		}
		bindres = bind(sock, server->ai_addr, server->ai_addrlen);
		if(sock == 0 && bindres == 0)
			break;
		server = server->ai_next;
	}
	if(sock == -1){
		ExitErrno(7, sock);
	} else if(bindres == -1){
		ExitErrno(8, bindres);
	}

	return sock;
}