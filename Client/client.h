#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define TRUE	1
#define FALSE	0


class Client
{
private:
	int mainSocket, valread;
	struct sockaddr_in address;
	char buffer[1024];

	void connectToServer();
	void readInitMessage();	
public:
	Client();
	void run();
	
};

#endif
