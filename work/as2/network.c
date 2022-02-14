#include "network.h"

#define PORT 12345
#define MAX_PACKET_LEN 1024

static struct sockaddr_in sin;
static struct sockaddr_in sinRemote;
static unsigned int sin_len;
static int socketDescriptor;

void openConnection() {
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORT);

	socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
	bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
	sin_len = sizeof(sinRemote);
}

int receiveRequest(char *request, int strLen) {
	int bytesRx = recvfrom(socketDescriptor, 
						   	request, 
						   	strLen - 1, 
						   	0, 
						   	(struct sockaddr *) &sinRemote, 
							&sin_len
						   );

	request[bytesRx - 1] = 0;
	return bytesRx - 1;
}

int sendResponse(char *str) {
	int packetSent = 0;

	char *strPointer = str;
	char responsePacket[MAX_PACKET_LEN];
	memset(responsePacket, 0, MAX_PACKET_LEN);
	int counter = 0;
	while(*strPointer != 0) {
		if (counter == MAX_PACKET_LEN) {
			sendto(socketDescriptor,
					responsePacket,
					strlen(responsePacket),
					0,
					(struct sockaddr *) &sinRemote, 
					sin_len
				  );
			memset(responsePacket, 0, MAX_PACKET_LEN);
			counter = 0;
			packetSent++;
		} else {
			responsePacket[counter] = *strPointer;
			strPointer++;
			counter++;
		}
	}

	if(strlen(responsePacket) > 0) {
		sendto(socketDescriptor,
					responsePacket,
					strlen(responsePacket),
					0,
					(struct sockaddr *) &sinRemote, 
					sin_len
				  );
		memset(responsePacket, 0, MAX_PACKET_LEN);
		packetSent++;
	}

	return packetSent;
}

void closeConnection() {
	close(socketDescriptor);
}
