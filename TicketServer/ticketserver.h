#ifndef SERVER_H
#define SERVER_H

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

#include "ticket.h"

#define TRUE	1
#define FALSE	0

#define MAX_CLIENTS	10
#define PORT 		8888

class TicketServer
{
private:
	int opt = TRUE;
	int addrlen;
	int sd, max_sd;
	char *message = "SERVER\n";
	int mainSocket;
	int clientSockets[MAX_CLIENTS];

	struct sockaddr_in address;
	char buffer[1024];	
	fd_set readfds;


	void createMainSocket();
	void initClients();
	void bindMainSocket();
	void listenMainSocket();
	void setMaxConnections();	

	void AcceptNewConnection();
	void SendMessage(int socket, char * message);
	void SetNewSocket(int socket);
public:
	TicketServer();
	void run();
	
};

#endif
