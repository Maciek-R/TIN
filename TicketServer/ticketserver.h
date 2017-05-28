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

#include "../Common/ticket.h"
#include <sstream>
#include <string>
#include <array>

class TicketServer
{
private:
	const int PORT;
	const std::string SERVICE_ADDRESS_1;
	const std::string TICKET_SERVER_ADDRESS;
	std::string ClientAddress;
	bool opt;
	int addrlen;
	int sd, max_sd;
	int mainSocket;
	std::array<int, 10> clientSockets;

	struct sockaddr_in address;
	unsigned char buffer[1024];	
	fd_set readfds;

	unsigned char * serviceInfo;

	std::string ToString(unsigned char*, int from, int to);

	void CreateMainSocket();
	void InitClients();
	void BindMainSocket();
	void ListenMainSocket();

	void AcceptNewConnection();
	void SendMessage(int socket, const char* message) const;
	void SetNewSocket(int socket);
	void GetBroadcastMessage();
	void AnswerOnBroadcastMessage();
	void AnswerOnRequestForTicket(bool);
	bool AuthorizeClient(std::string);
	void loadServiceInfo(bool);
public:
	TicketServer();
	void Run();
	
};

#endif
