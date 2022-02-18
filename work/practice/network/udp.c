#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_LEN 1024
#define PORT 12345


int main() {
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORT);

	printf("UDP socket example\n");
	int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
	bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

	struct sockaddr_in sinRemote;
	unsigned int sin_len = sizeof(sinRemote);
	char messageRx[MAX_LEN];
	while(true){
		int bytesRx = recvfrom(socketDescriptor, 
								messageRx, 
								MAX_LEN - 1, 
								0, 
								(struct sockaddr *) &sinRemote, 
								&sin_len);

		messageRx[bytesRx - 1] = 0;
		printf("Message received (%d bytes): '%s'\n", bytesRx, messageRx);


		if(messageRx[0] == '!') {
			char messageTx[MAX_LEN];
			sprintf(messageTx, "Exiting Program\n");
			sendto(socketDescriptor,
					messageTx,
					strlen(messageTx),
					0,
					(struct sockaddr *) &sinRemote, sin_len);
			break;
		}
	}


	close(socketDescriptor);
	return 0;
}