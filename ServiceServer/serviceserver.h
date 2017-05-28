#ifndef SERVICE_SERVER_H
#define SERVICE_SERVER_H

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

class ServiceServer
{
private:
	const int PORT;
	std::string ClientAddress;
	bool opt;
	int addrlen;
	int sd, max_sd;
	int mainSocket;
	std::array<int, 10> clientSockets;

	struct sockaddr_in address;
	unsigned char buffer[1024];	
	fd_set readfds;

	std::string ToString(unsigned char*, int from, int to);

	void CreateMainSocket();
	void InitClients();
	void BindMainSocket();
	void ListenMainSocket();

	void AcceptNewConnection();
	void SendMessage(int socket, const char* message) const;
	void SetNewSocket(int socket);
	bool AuthorizeClient(std::string);
public:
	ServiceServer();
	void Run();
	
};

#endif
