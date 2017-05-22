#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros


#include "../Common/ticket.h"

class Client
{
public:
	Client();
	void Run();
	void SendRequestForTicket();
private:
	const int PORT;
	int mainSocket;
	int bytesRead;
	struct sockaddr_in address;
	char buffer[1024];
	Ticket ticket;

	void ConnectToServer();
	void ReadInitMessage();
};

#endif
