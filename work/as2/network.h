#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#ifndef NETWORK_H
#define NETWORK_H

void openConnection();
int receiveRequest(char *request, int strLen);
int sendResponse(char *str);
void closeConnection();

#endif