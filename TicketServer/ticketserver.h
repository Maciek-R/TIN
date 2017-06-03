#ifndef TICKET_SERVER_H
#define TICKET_SERVER_H

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
#include "../Common/utils.h"
#include <sstream>
#include <string>
#include <array>
#include <fstream>
#include <assert.h>

class TicketServer
{
private:
	const int PORT;
	const std::string SERVICE_ADDRESS_1;
	const std::string TICKET_SERVER_ADDRESS;
	std::string ClientAddress;
	bool opt;
	int addrlen;
	int mainSocket;

	struct sockaddr_in address;
	unsigned char buffer[1024];	
	fd_set readfds;

	unsigned char * serviceInfo;

	void CreateMainSocket();
	void BindMainSocket();

	void SendMessage(int socket, const char* message) const;
	void GetBroadcastMessage();
	void AnswerOnBroadcastMessage();
	void AnswerOnRequestForTicket(bool, unsigned char);
	bool AuthorizeClient(unsigned char *);
	void loadServiceInfo(bool, unsigned char);

	bool checkClientInDatabase(unsigned char *);
public:
	TicketServer();	
	~TicketServer();
	void Run();
};

#endif
