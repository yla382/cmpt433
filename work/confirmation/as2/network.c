#include "network.h"

#define PORT 12345
#define MAX_PACKET_LEN 1024

static struct sockaddr_in sin;
static struct sockaddr_in sinRemote;
static unsigned int sin_len;
static int socketDescriptor;


/*
Initializing UDP sockets and set to listen to port 12345
intput: void
output: void
*/
void openConnection() {
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORT);

	socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
	bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
	sin_len = sizeof(sinRemote);
}


/*
Receives messages sent from client and returns number of characters in the message
input: *char, int
output: int
*/
int receiveRequest(char *request, int strLen) {
	//Receive packet. bytesRx is the number of characteres received
	int bytesRx = recvfrom(socketDescriptor, 
						   	request, 
						   	strLen - 1, 
						   	0, 
						   	(struct sockaddr *) &sinRemote, 
							&sin_len
						   );

	//remove last characters of the message because it will always be the newline
	request[bytesRx - 1] = 0;
	return bytesRx - 1;
}


/*
Sends messege to client. Messages are split into 1024 bytes
and sends them in order to prevent exceeding max packet size.
Returns number of packets sent.
input: *char, bool
output: int 
*/
int sendResponse(char *str, bool freeMem) {
	int packetSent = 0;
	char *strPointer = str;
	//char array that will be used copy message to packet
	char responsePacket[MAX_PACKET_LEN];
	memset(responsePacket, 0, MAX_PACKET_LEN);
	int counter = 0;
	while(*strPointer != 0) {
		//If responsePacket is full send message
		if (counter == MAX_PACKET_LEN) {
			sendto(socketDescriptor,
					responsePacket,
					strlen(responsePacket),
					0,
					(struct sockaddr *) &sinRemote, 
					sin_len
				  );

			//Clear contents of responsePacket
			memset(responsePacket, 0, MAX_PACKET_LEN);
			counter = 0;
			packetSent++;
		} else {
			//Add next character of message to responsePacket
			responsePacket[counter] = *strPointer;
			strPointer++;
			counter++;
		}
	}

	//Send remaining characters to client
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


	//De-allocate input string if specified (bool freeMem)
	if(freeMem) {
		free(str);
	}
	return packetSent;
}


/*
Close udp connection
input: void
output: void
*/
void closeConnection() {
	close(socketDescriptor);
}
