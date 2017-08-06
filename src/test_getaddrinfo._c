#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "headers/liblogging.h"

int main(int argc, char* argv[]){
	//struct in_addr  try4;
	//struct in_addr6 try6;
	struct addrinfo   hints;
	struct addrinfo* AddressInfo;
	memset(&hints, '\0', sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;
	int status = getaddrinfo(argv[1], argv[2], &hints, &AddressInfo);
	if(status == 0) {
		printAddrinfoList(AddressInfo);
		return 0;
	} else {
		char *des = NULL;
		const char *err = getEnumTriple(status, EAI_ERROR_VALUES, &des);
		printf("getaddrinfo Error: %s (%s)\n", des, err);
	}
	return status;
}