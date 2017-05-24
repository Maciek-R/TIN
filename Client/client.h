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

#include <string>
#include "../Common/ticket.h"

class Client
{
public:
	Client();
	void Run();
	bool SendRequestForTicket();
	bool GetServiceAddress();
	void RunService(int);
private:
	const int BROADCAST_PORT;
	const std::string BROADCAST_ADDRESS;
	std::string ServiceAddress;
	int broadcastPermission;
	int mainSocket;
	int bytesRead;
	struct sockaddr_in address;
	char buffer[1024];
	Ticket ticket;

	
	void ReadInitMessage();
	std::string PrepareData();
	bool ConnectToTicketServer();
	bool InitBroadcastSocket();
	bool TranslateMessageFromTicketServer(std::string);
};

#endif
