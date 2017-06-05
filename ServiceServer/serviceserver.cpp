#include "serviceserver.h"
#include <vector>
#include <iostream>


ServiceServer::ServiceServer(int serviceID, int port)
	: SERVICE_ID{serviceID}, PORT{port}, ADDRESS{Utils::DetectIP(NetworkObject::interfaceType)}, BROADCAST_PORT{8888}, BROADCAST_ADDRESS{Utils::CalculateBroadCast(ADDRESS, "255.255.255.0")}, opt{true}, mainSocket{-1}
{
	InitClients();
	CreateMainSocket();
	BindMainSocket();
	ListenMainSocket();

	CreateBroadcastSocket();
}

ServiceServer::~ServiceServer()
{
	std::cout << "Destroyed\n";
	close(mainSocket);
	close(broadcastSocket);
}


void ServiceServer::CreateMainSocket()
{
	if((mainSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		std::cout << "Creating Server socket error\n";
		exit(EXIT_FAILURE);
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

void ServiceServer::Run()
{
	std::cout << "Waiting for connections\n";

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
			AcceptNewConnection();
		}

		for(std::size_t i = 0; i < clientSockets.size(); ++i)
		{
			switch(static_cast<ServiceType>(SERVICE_ID))
			{
			case ServiceType::TCP_ECHO:
				SendEcho(clientSockets[i]);
				break;
			case ServiceType::TCP_TIME:
				SendTime(clientSockets[i]);
				break;
			default:
				std::cout << "Unknown service request\n";
			}
		}
	}


}

void ServiceServer::SendTime(int& socket)
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

void ServiceServer::SendEcho(int& socket)
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

void ServiceServer::RespondToConnectionAttempt(int& socket)
{
	int bytesRead = read( socket , buffer, 1024);
	//Somebody disconnected , get his details and print
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

		Ticket ticket{buffer};

		//sockaddr_in tempAddress;
		//int tempAddrlen;
		getpeername(socket , (struct sockaddr*)&address , (socklen_t*)&addrlen);
		if(AuthorizeClient(buffer, inet_ntoa(address.sin_addr)))///*Tutaj walidacja*/ ticket.GenerateTicketInString().size() > 0)
		{

			std::cout << "Client ticket: " << ticket.GenerateTicketInString() << "\n";
			if(write(socket, "1\0", 4) == -1)
				std::cerr << "Cannot send a response\n";
			else
				std::cout << "Client authorized\n";
		}
		else
		{
			std::cout << "Client ticket: " << ticket.GenerateTicketInString() << "\n";
			if(write(socket, "0\0", 4) == -1)
				std::cerr << "Cannot send a response\n";
			else
				std::cout << "Client not authorized\n";
		}
	}
}

void ServiceServer::AcceptNewConnection()
{
	int newSocket;
	newSocket = accept(mainSocket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
	if(newSocket < 0)
	{
		std::cerr << "Accept Error\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "New Connection, socket fd is " << newSocket << "\n\tIP: " << inet_ntoa(address.sin_addr) << "\n\tPort: " << ntohs(address.sin_port) << "\n";


	RespondToConnectionAttempt(newSocket);
	SetNewSocket(newSocket);
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
	
	//sign
	for(unsigned int i = 0; i < 16 ; ++i)
	{
		if((int)newCheckSum[i] != ticket.GetCheckSum()[i])
		{
			std::cout << "Sign of ticket is incorrect!\n";
			return false;
		}
	}
	
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
	
	if(realAddress != ticket.GetServiceAddress())
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
			//std::cout << "Ticket is invalid! (timeout)\n";
			timeouts.erase(address);
			return false;
		}
	}
	
	return true;
}
