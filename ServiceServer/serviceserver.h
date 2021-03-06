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
#include <openssl/sha.h>
#include "../Common/ticket.h"
#include <sstream>
#include <string>
#include <array>
#include "../Common/network_object.h"
#include <map>
#include <openssl/aes.h>

enum class ServiceType
{
	TCP_ECHO = 1,
	TCP_TIME = 2,
	UDP_ECHO = 3,
	UDP_TIME = 4,
};

class ServiceServer : public NetworkObject
{
private:
	const int SERVICE_ID;
	const int PORT;
	const std::string ADDRESS;
	const int BROADCAST_PORT;
	const std::string BROADCAST_ADDRESS;
	std::string ClientAddress;
	bool opt;
	int addrlen;
	int broadcastAddrlen;
	int sd, max_sd;
	int mainSocket;
	int broadcastSocket;
	std::array<int, 10> clientSockets;

	struct sockaddr_in address;
	sockaddr_in broadcastAddress;
	unsigned char buffer[1024];
	fd_set readfds;
	
	std::map<std::string, long> timeouts;
	AES_KEY decryptionKey;

	std::string ToString(unsigned char*, int from, int to);

	void CreateMainSocket();
	void CreateBroadcastSocket();
	void InitClients();
	void BindMainSocket();
	void ListenMainSocket();

	bool AcceptNewConnection();
	bool RespondToConnectionAttempt(int& socket);
	bool UdpRequestProcessing(int sock);
	void SendMessage(int socket, const char* message) const;
	void SetNewSocket(int socket);
	bool AuthorizeClient(unsigned char *, std::string);

	void SendTcpEcho(int& socket);
	void SendTcpTime(int& socket);
	void SendUdpEcho(int& socket);
	void SendUdpTime(int& socket);

	std::string GetServerTime();
	
	bool ValidateTimeOut(std::string address, time_t timeout);
public:
	ServiceServer(int serviceID, int port);
	~ServiceServer();

	void BroadcastDisconnect();
	void Run();
	
};

#endif
