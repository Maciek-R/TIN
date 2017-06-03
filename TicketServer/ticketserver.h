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
#include "../Common/ServiceDataBaseManager.h"
#include "../Common/network_object.h"

class TicketServer : public NetworkObject
{
private:
	ServiceDataBaseManager serviceDataBaseManager;
	const int PORT;
	const std::string SERVICE_ADDRESS_1;
	const std::string TICKET_SERVER_ADDRESS;
	const std::string BROADCAST_ADDRESS;
	const int BROADCAST_PORT;
	std::string ClientAddress;
	bool opt;
	int addrlen;
	int mainSocket;
	int broadcastSocket;

	struct sockaddr_in address;
	sockaddr_in broadcastAddress;
	int broadcastAddrlen;
	unsigned char buffer[1024];	
	fd_set readfds;

	unsigned char * serviceInfo;

	void CreateMainSocket();
	void BindMainSocket();
	bool InitBroadcastSocket();

	void SendMessage(int socket, const char* message) const;
	void GetBroadcastMessage();
	bool SendBroadcastMessage(int serviceID);
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
