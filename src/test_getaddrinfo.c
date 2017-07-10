/*#include <errno.h>
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
*/
#include "liblogging.h"

int main(int argc, char* argv[]){
	//struct in_addr  try4;
	//struct in_addr6 try6;
	struct addrinfo   hints;
	struct addrinfo* AddressInfo;
	memset(&hints, '\0', sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;
	int status = getaddrinfo(argv[1], argv[2], &hints, &AddressInfo);
	if(status == 0) {
		//success
		struct addrinfo* next = AddressInfo;
		int              count = 1;
		do {
			printf("(*%p) %d: \n", next, count++);
			//tablevel++;
			INCTAB(){
				next = addrinfoToString(next);
			}
			//tablevel--;
		} while(next != NULL);
		return 0;
	} else {
		const char *err = getEnumValueName(status, ARR_SIZE(EAI_ERROR_VALUES), EAI_ERROR_VALUES);
		printf("getaddrinfo Error: %s\n", err);
	}
	return status;
}