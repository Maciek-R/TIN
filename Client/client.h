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
#include <sstream>
#include <istream>
#include "../Common/ticket.h"
#include "../Common/utils.h"

class Client
{
public:
	Client();
	void Run();
	bool GetServiceAddress();
	bool RunService(int);

	bool GetTicketServerAddress();
	bool GetTicket();
private:
	const std::string CLIENT_ADDRESS;
	const int BROADCAST_PORT;
	const std::string BROADCAST_ADDRESS;
	std::string ServiceAddress;
	int ServicePort;
	std::string TicketServerAddress;

	int broadcastPermission;
	int mainSocket;
	int bytesRead;
	struct sockaddr_in address;
	unsigned char buffer[1024];
	Ticket ticket;

	unsigned char clientInfo[57];

	void ReadInitMessage();

	bool InitBroadcastSocket();
	bool SendBroadcastMessage();
	bool ReceiveTicketServerAddress();

	bool InitSocketWithTicketServer();
	bool SendRequestForTicket();
	bool ReceiveTicket();

	bool sendTcpEcho();

	void loadClientInfo();
	
	void LoadUserDataFromConsole();
};

#endif
