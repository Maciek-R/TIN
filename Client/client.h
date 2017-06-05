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
#include <openssl/sha.h>

#include "../Common/ticket.h"
#include "../Common/utils.h"
#include "../Common/network_object.h"

#include <map>
#include <openssl/aes.h>

class Client : public NetworkObject
{
public:
	Client();
	~Client();
	bool GetServiceAddress();
	bool RunService(int);
	bool GetTicketServerAddress();
	bool GetTicket();
private:
	static InterfaceType interfaceType;

	const std::string CLIENT_ADDRESS;
	const int BROADCAST_PORT;
	const std::string BROADCAST_ADDRESS;
	std::map<int, std::string> serviceAddresses;
	std::map<int, int> servicePorts;
	std::string ticketServerAddress;

	int broadcastPermission;
	int mainSocket;
	int bytesRead;
	struct sockaddr_in address;
	unsigned char buffer[1024];
	std::map<int, Ticket> tickets;

	unsigned char clientInfo[57];
	AES_KEY decryptionKey;
	AES_KEY encryptionKey;

	void ReadInitMessage();

	bool InitBroadcastSocket();
	bool SendBroadcastMessage();
	bool ReceiveTicketServerAddress();

	bool ShowTicketToServer(int serviceID);

	bool InitSocketWithTicketServer();
	bool SendRequestForTicket();
	bool ReceiveTicket();

	bool SendTcpEcho();
	bool SendTcpTime();
	bool SendUdpEcho();

	void LoadClientInfo(int serviceID);
	
	void LoadUserDataFromConsole();
};

#endif
