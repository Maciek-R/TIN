#include "serviceserver.h"
#include <vector>
#include <iostream>


ServiceServer::ServiceServer(int serviceID, int port)
	: SERVICE_ID{serviceID}, PORT{port}, ADDRESS{Utils::DetectIP(NetworkObject::interfaceType)}, BROADCAST_PORT{8888}, BROADCAST_ADDRESS{Utils::CalculateBroadCast(ADDRESS, "255.255.255.0")}, opt{true}, mainSocket{-1}
{
	InitClients();
	if(SERVICE_ID == 1 || SERVICE_ID == 2)
	{
		CreateMainSocket();
		BindMainSocket();
		ListenMainSocket();
	}

	CreateBroadcastSocket();

	unsigned char keyText[] = "whateverwhatever\0";
	AES_set_decrypt_key(keyText, 128, &decryptionKey);
}

ServiceServer::~ServiceServer()
{
	std::cout << "Destroyed\n";
	close(mainSocket);
	close(broadcastSocket);
}


void ServiceServer::CreateMainSocket()
{
	if(SERVICE_ID == 1 || SERVICE_ID == 2)
	{
		if((mainSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			std::cout << "Creating Server socket error\n";
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		if((mainSocket = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
		{
			std::cout << "Creating Server socket error\n";
			exit(EXIT_FAILURE);
		}
	}

	int option = static_cast<int>(opt);
	if(setsockopt(mainSocket, SOL_SOCKET, SO_BROADCAST, &option, sizeof(option)) < 0)
	{
		std::cerr << "Setting sockopt error\n";
		exit(EXIT_FAILURE);
	}
}

void ServiceServer::InitClients()
{
	for(std::size_t i = 0; i < clientSockets.size(); ++i)
		clientSockets[i]=0;
}

void ServiceServer::BindMainSocket()
{
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);


	if(bind(mainSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cerr << "Binding Error\n";
		exit(EXIT_FAILURE);
	}
}

void ServiceServer::ListenMainSocket()
{	
	if (listen(mainSocket, 3) < 0)
	{
		std::cerr << "Listen Error\n";
		exit(EXIT_FAILURE);
	}
	
	addrlen = sizeof(address);
	std::cout << "Listening on port " << PORT << '\n';
}

bool ServiceServer::UdpRequestProcessing(int sock)
{
	addrlen = sizeof(address);

	unsigned char message[1024];
	int bytesRead = Utils::recvudp(message, sock, 100, address, addrlen);
	if(bytesRead > 0)
	{
		unsigned char decryptedTicket[1024];
		AES_decrypt(message, decryptedTicket, &decryptionKey);
		Ticket ticket{decryptedTicket};
		std::cout << "Ticket decrypted\n";

		std::cout << "Received message from the client: " << Ticket{decryptedTicket}.GenerateTicketInString() << "\n";
		unsigned char response[1];
		if(AuthorizeClient(decryptedTicket, inet_ntoa(address.sin_addr)))
		{
			response[0] = '1';
			std::cout << "Client authorized\n";
			Utils::sendudp(response, 2, address, sock);
			return true;
		}
		else
		{
			response[0] = '0';
			std::cout << "Client not authorized\n";
			Utils::sendudp(response, 2, address, sock);
			return false;
		}

	}
}

void ServiceServer::SendUdpEcho(int& socket)
{
	unsigned char message[1024];
	int bytesRead = Utils::recvudp(message, socket, 1024, address, addrlen);
	if(bytesRead > 0)
	{
		std::cout << "UDP echo: " << message << "\n";
		if(Utils::sendudp(message, 1024, address, socket) == -1)
			std::cout << "Sending UDP echo error\n";
		else
			std::cout << "Echo sent\n";
	}
	else
	{
		std::cout << "Invalid message\n";
	}
}

void ServiceServer::SendUdpTime(int& socket)
{
	unsigned char message[1024];

	int bytesRead = Utils::recvudp(message, socket, 1024, address, addrlen);
	if(bytesRead > 0)
	{
		std::string time = GetServerTime();

		for(int i = 0; i < time.size(); ++i)
		{
			message[i] = time[i];
		}
		message[time.size()] = '\0';

		if(Utils::sendudp(message, 1024, address, socket) == -1)
			std::cout << "Sending UDP time error\n";
		else
			std::cout << "Udp time sent\n";
	}
}

void ServiceServer::Run()
{
	std::cout << "Waiting for connections\n";

	if(SERVICE_ID == 3 || SERVICE_ID == 4)
	{
		int socket = Utils::udpsock(PORT, INADDR_ANY);
		std::cout << "Waiting for datagram on port: " << PORT << "\n";

		while(true)
		{
			if(UdpRequestProcessing(socket))
			{
				if(SERVICE_ID == 3)
				{
					SendUdpEcho(socket);
				}
				else
				{
					SendUdpTime(socket);
				}
			}
		}
	}
	else
	while(true)
	{
		FD_ZERO(&readfds);
		FD_SET(mainSocket, &readfds);
		
		max_sd = mainSocket;	
	
		for(std::size_t i = 0; i < clientSockets.size(); ++i)
		{
			sd = clientSockets[i];
			//if socket exists
			if(sd > 0)
				FD_SET(sd, &readfds);
			if(sd > max_sd)
				max_sd = sd;
		}

		int activity = select(max_sd+1, &readfds, nullptr, nullptr, nullptr);
	
		if((activity < 0) && (errno!=EINTR))
		{
			std::cout << activity << " " << errno << "\n";
			std::cout << "Error in select\n";
		}


		//new Connection
		if(FD_ISSET(mainSocket, &readfds))
		{
			if(!AcceptNewConnection())
				continue;
		}

		for(std::size_t i = 0; i < clientSockets.size(); ++i)
		{
			switch(static_cast<ServiceType>(SERVICE_ID))
			{
			case ServiceType::TCP_ECHO:
				SendTcpEcho(clientSockets[i]);
				break;
			case ServiceType::TCP_TIME:
				SendTcpTime(clientSockets[i]);
				break;
			default:
				std::cout << "Unknown service request\n";
			}
		}
	}


}

void ServiceServer::SendTcpTime(int& socket)
{
	if(FD_ISSET(socket, &readfds))
	{
		std::string time = GetServerTime();
		std::cout << time.c_str() << "\n";
		if( write(socket, time.c_str(), time.size()) == -1)
			std::cerr << "Error while sending time\n";
		else
			std::cerr << "Time successfuly sent\n";

		close(socket);
		socket = 0;
	}
}

void ServiceServer::SendTcpEcho(int& socket)
{
	if(FD_ISSET(socket, &readfds))
	{
	int bytesRead = read( socket , buffer, 1024);

	int i;
	char message[1024];
	for(i = 0; buffer[i]!=0; ++i)
	{
		std::cout << buffer[i];
		message[i] = buffer[i];
	}
	message[i] = '\0';
	std::cout << "\n";

	std::cout << "Message from Client nr " << ":\t" << buffer<<"\n";

	if(write(socket, message, strlen(message)) == -1)
		std::cerr << "Echo error\n";
	else
		std::cout << "Echo sent\n";
	close(socket);
	socket = 0;
	}
}

bool ServiceServer::RespondToConnectionAttempt(int& socket)
{
	int bytesRead = read( socket , buffer, 1024);
	if (bytesRead == 0)
	{
		getpeername(socket , (struct sockaddr*)&address , (socklen_t*)&addrlen);
		std::cout << "Host disconnected\n\tIP: " << inet_ntoa(address.sin_addr)<< "\n\tport: " <<  ntohs(address.sin_port) << '\n';

		close( socket );
		socket = 0;
	}
	//message
	else
	{
		buffer[bytesRead] = '\0';

		unsigned char decryptedTicket[1024];
		AES_decrypt(buffer, decryptedTicket, &decryptionKey);
		Ticket ticket{decryptedTicket};
		std::cout << "Ticket decrypted\n";

		getpeername(socket , (struct sockaddr*)&address , (socklen_t*)&addrlen);
		if(AuthorizeClient(decryptedTicket, inet_ntoa(address.sin_addr)))
		{

			std::cout << "Client ticket: " << ticket.GenerateTicketInString() << "\n";
			if(write(socket, "1\0", 4) == -1)
				std::cerr << "Cannot send a response\n";
			else
				std::cout << "Client authorized\n";
			return true;
		}
		else
		{
			std::cout << "Client ticket: " << ticket.GenerateTicketInString() << "\n";
			if(write(socket, "0\0", 4) == -1)
				std::cerr << "Cannot send a response\n";
			else
				std::cout << "Client not authorized\n";
			return false;
		}
	}
}

bool ServiceServer::AcceptNewConnection()
{
	int newSocket;
	newSocket = accept(mainSocket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
	if(newSocket < 0)
	{
		std::cerr << "Accept Error\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "New Connection, socket fd is " << newSocket << "\n\tIP: " << inet_ntoa(address.sin_addr) << "\n\tPort: " << ntohs(address.sin_port) << "\n";


	if(RespondToConnectionAttempt(newSocket))
	{
		SetNewSocket(newSocket);
		std::cout << "Connection accepted\n";
		return true;
	}
	else
	{
		std::cout << "Connection not accepted\n";
		return false;
	}

	return true;
}

void ServiceServer::SendMessage(int socket, const char * message) const
{
	if(send(socket, message, strlen(message), 0)!= strlen(message))
		std::cerr << "Error while sending message to client\n";
	else
		std::cout << "Message send successfully\n";
}
void ServiceServer::SetNewSocket(int socket)
{
	for(std::size_t i = 0; i < clientSockets.size(); ++i)
	{
		if(clientSockets[i]==0)
		{
			clientSockets[i] = socket;
			break;
		}
	}
}

bool ServiceServer::AuthorizeClient(unsigned char * data, std::string realAddress)
{
	Ticket ticket{data};
	std::string ticketAsString = ticket.GenerateTicketInString();
	
	unsigned char hash[16];
	unsigned char* newCheckSum = SHA1((unsigned char*)(ticketAsString.c_str()), ticketAsString.size(), hash);
	
	std::cout << "Got request for " << ticket.GetServiceId()  << ". Can handle " << SERVICE_ID << ". My port: " << PORT << "\n";
	//idservice
	if(SERVICE_ID != ticket.GetServiceId())
	{
		std::cout << "Service id is incorrect!\n";
		return false;
	}
	
	//timeout
	if(!ValidateTimeOut(ticket.GetClientAddress(), ticket.GetValidTime()))
	{
		std::cout << "Ticket is invalid! (timeout)\n";
		return false;
	}
	
	std::cout << realAddress << " " << ticket.GetClientAddress() << "\n";
	if(realAddress != ticket.GetClientAddress())
	{
		std::cout << "Addres of service is incorrect!\n";
		return false;
	}
	
	std::cout << "Authorization passed\n";
	return true;
}


void ServiceServer::CreateBroadcastSocket()
{
	if( (broadcastSocket = socket(AF_INET , SOCK_DGRAM , 0)) == -1)
	{
		std::cerr << "Creating socket error\n";
		exit(1);
	}
	int broadcastPermission = 1;
	if (setsockopt(broadcastSocket, SOL_SOCKET, SO_BROADCAST, (void *)&broadcastPermission, sizeof(broadcastPermission)) < 0)
	{
		std::cerr<<"setsockopt() failed\n";
		exit(1);
	}

	broadcastAddress.sin_family = AF_INET;
	broadcastAddress.sin_addr.s_addr = inet_addr(BROADCAST_ADDRESS.c_str());
	broadcastAddress.sin_port = htons(BROADCAST_PORT);

	unsigned char message[10] {static_cast<unsigned char>(3)};
	message[1] = SERVICE_ID;
	Utils::LoadAddress(message, ADDRESS, 2);
	Utils::InsertNumberToCharTable(message, PORT, 6, 10);

	for(int i = 0; i < 10; ++i)
	{
		std::cout << (int)message[i];
	}
	std::cout << "\n";
	if(sendto(broadcastSocket, message, 10, 0, (struct sockaddr *) &broadcastAddress, sizeof(broadcastAddress)) == -1)
	{
		std::cerr << "Error while sending broadcast message " << errno << "\n";
		exit(1);
	}

	close(broadcastSocket);
}

std::string ServiceServer::GetServerTime()
{
	time_t currentTime = time(0);
	struct tm * now = localtime( &currentTime );
	std::string serializedTime = "";
	serializedTime +=
		  (now->tm_hour < 10 ? "0" : "") + std::to_string(now->tm_hour) + ":"
		+ (now->tm_min < 10 ? "0" : "") + std::to_string(now->tm_min) + ":"
		+ (now->tm_sec < 10 ? "0" : "") + std::to_string(now->tm_sec) + " "
		+ (now->tm_mday < 10 ? "0" : "") + std::to_string(now->tm_mday) + "-"
		+ (now->tm_mon < 10 ? "0" : "") + std::to_string(now->tm_mon + 1) + "-"
		+ std::to_string(now->tm_year + 1900)
		+ "\n";
	return serializedTime;
}

bool ServiceServer::ValidateTimeOut(std::string address, time_t timeout)
{
	time_t currentTime = time(0);
	std::cout << "currentTime: " << currentTime << "\n";
	if(timeouts.count(address) == 0)
	{
		timeouts[address] = currentTime + timeout;
	}
	else
	{
		if(timeouts[address] < currentTime)
		{
			timeouts.erase(address);
			return false;
		}
	}
	
	return true;
}
