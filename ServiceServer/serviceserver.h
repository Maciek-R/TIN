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
#include "../Common/network_object.h"

class ServiceServer : public NetworkObject
{
private:
	const int SERVICE_ID;
	const int PORT;
	const std::string ADDRESS;
	const int LISTENING_PORT;
	std::string ClientAddress;
	bool opt;
	int addrlen;
	int sd, max_sd;
	int mainSocket;
	int listeningSocket;
	std::array<int, 10> clientSockets;

	struct sockaddr_in address;
	sockaddr_in listeningAddress;
	unsigned char buffer[1024];
	fd_set readfds;

	std::string ToString(unsigned char*, int from, int to);

	void CreateMainSocket();
	void CreateListeningSocket();
	void InitClients();
	void BindMainSocket();
	void BindListeningSocket();
	void ListenMainSocket();

	void AcceptNewConnection();
	void SendMessage(int socket, const char* message) const;
	void SetNewSocket(int socket);
	bool AuthorizeClient(unsigned char *);
public:
	ServiceServer(int serviceID, int port);
	~ServiceServer();
	void Run();
	
};

#endif
