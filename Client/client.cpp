#include "client.h"
#include <iostream>
#include <limits>
#include <assert.h>


Client::Client()
	: CLIENT_ADDRESS{Utils::DetectIP(NetworkObject::interfaceType)}, BROADCAST_PORT{8888}, BROADCAST_ADDRESS{Utils::CalculateBroadCast(CLIENT_ADDRESS, "255.255.255.0")}, mainSocket{-1}, bytesRead{-1}, ticket{}
{
		std::cout << CLIENT_ADDRESS << "\n";
		std::cout << BROADCAST_ADDRESS << "\n";
}

Client::~Client()
{
	close(mainSocket);
}

bool Client::GetTicketServerAddress()
{
	if(!InitBroadcastSocket() || !SendBroadcastMessage() || !ReceiveTicketServerAddress())
		return false;

	return true;
}

bool Client::GetTicket()
{
	if(!InitSocketWithTicketServer() || !SendRequestForTicket() || !ReceiveTicket())
		return false;

	return true;
}


bool Client::InitBroadcastSocket()
{
	if( (mainSocket = socket(AF_INET , SOCK_DGRAM , 0)) == -1) 
	{
		std::cerr << "Creating socket error\n";
		return false;
	}
	broadcastPermission = 1;
	if (setsockopt(mainSocket, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
	{
		std::cerr<<"setsockopt() failed";
		return false;
	}
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(BROADCAST_ADDRESS.c_str());
	address.sin_port = htons( BROADCAST_PORT );

	return true;
}

bool Client::SendBroadcastMessage()
{
	unsigned char message[5] {1};
	Utils::LoadAddress(message, CLIENT_ADDRESS, 1);
	if(sendto(mainSocket, message, 5, 0, (struct sockaddr *) &address, sizeof(address)) != 5)
	{
		std::cerr << "Error while sending broadcast message";
		return false;
	}
	return true;
}

bool Client::ReceiveTicketServerAddress()
{
	int addrlen = sizeof(address);
	if(recvfrom(mainSocket, buffer, 1024, 0, (struct sockaddr *) &address, (socklen_t*)&addrlen) == -1)
	{
		std::cerr << "Error while receiving message from TicketServer";
		return false;
	}
	
	ticketServerAddress = Utils::ToString(buffer, 0, 4);
	std::cout << "TicketServer address is: "<<ticketServerAddress <<"\n";
	close(mainSocket);
	return true;
}
bool Client::InitSocketWithTicketServer()
{
	if( (mainSocket = socket(AF_INET , SOCK_DGRAM , 0)) == -1) 
	{
		std::cerr << "Creating socket error\n";
		return false;
	}
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ticketServerAddress.c_str());
	address.sin_port = htons( BROADCAST_PORT );

	return true;
}
bool Client::SendRequestForTicket()
{
	if(sendto(mainSocket, clientInfo, 57, 0, (struct sockaddr *) &address, sizeof(address)) != 57)
	{
		std::cerr << "Error while sending request for ticket\n";
		return false;
	}

	return true;
}

bool Client::ReceiveTicket()
{
	int addrlen = sizeof(address);
	int bytesRead;
	if((bytesRead = recvfrom(mainSocket, buffer, 1024, 0, (struct sockaddr *) &address, (socklen_t*)&addrlen)) < 0)
	{
		std::cerr << "Error while receiving message from TicketServer";
		return false;
	}

	if(buffer[0] == 1)
	{
		ticket = Ticket{buffer+1};
		std::cout << "Ticket " << ticket.GenerateTicketInString() << "\n";

		serviceAddress = Utils::ToString(buffer, 5, 9);
		servicePort = Utils::ToInt(buffer, 9, 13);

		std::cout << "Got Message from TicketServer. Service Address is: "<<serviceAddress<<" Service Port: "<<servicePort
					<<"time: " << buffer[14] << buffer[15]<<"\n";
		
		return true;
	}
	else if (buffer[0]==0)
	{
		std::cout << "Got Message from TicketServer. You are not authorized!\n";
		return false;
	}
	else
	{
		std::cout << "Unknown message\n";
		return false;
	}
	close(mainSocket);
}

void Client::ReadInitMessage()
{
	if((bytesRead = read(mainSocket, buffer, 1024))==0)
	{
		std::cerr << "Receiving Message Error\n";
	}
	else
	{
		buffer[bytesRead] = '\0';
		std::cout << "Got " << bytesRead << " bytes\n";
		std::cout << "From Server: " << buffer << '\n';
	}
}

bool Client::RunService(int numService)
{
	assert(numService >= 1 && numService <= 4);
	LoadClientInfo(numService);
	assert(GetTicketServerAddress());
	assert(GetTicket());
	switch(numService)
	{
		case 1: SendTcpEcho(); break;
		case 2: SendTcpTime(); break;
		case 3: break;
		case 4: break;
	}

}

void Client::ShowTicketToServer()
{
	if( (mainSocket = socket(AF_INET , SOCK_STREAM , 0)) == -1)
	{
		std::cerr << "Creating socket error\n";
		std::terminate();
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(serviceAddress.c_str());
	address.sin_port = htons( servicePort );

	if(connect(mainSocket, (struct sockaddr *)&address, sizeof address)==-1)
	{
		std::cerr<<"Connecting to ServiceServer failed\n";
		std::terminate();
	}

	char message[1024];
	int size = 45;
	unsigned char * buff = ticket.Serialize();

	for(int i=0; i<size; ++i)
		message[i] = buff[i];

	message[size ] = '\0';

	std::cout << size  << "\n";

	if(write(mainSocket, message, size +1) == -1)
	{
		std::cerr << "Sending Ticket to ServiceServer Error\n";
		std::terminate();
	}
	else
	{
		std::cout << "Ticket sent\n";
	}

	int bytesRead;
	if((bytesRead = read(mainSocket, message, 1024))==0)
	{
		std::cerr << "Authorization Result Error\n";
		std::terminate();
	}
	else
	{
		message[bytesRead] = '\0';
		std::cout << "Authorization verdict: " << message << "\n";
	}
}

bool Client::SendTcpEcho()
{
	ShowTicketToServer();
	char message[1024];
	std::cout <<"Type Message: ";

	std::string info;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::getline(std::cin, info);

	if(info.size() > 0)
	{
		if(write(mainSocket, info.c_str(), info.size() +1) == -1)
		{
			std::cerr << "Sending Ticket to ServiceServer Error\n";
			std::terminate();
		}
		else
		{
			std::cout << "Echo message sent\n";
		}
	}
	else
	{
		std::cerr << "Empty message!\n";
		return false;
	}

	if((bytesRead = read(mainSocket, message, 1024))==0)
	{
		std::cerr << "Cannot Receive Echo\n";
		std::terminate();
	}
	else
	{
		message[bytesRead] = '\0';
		std::cout << "Echo: " << message << "\n";
	}

	return true;
}
bool Client::SendTcpTime()
{
	if( (mainSocket = socket(AF_INET , SOCK_STREAM , 0)) == -1) 
	{
		std::cerr << "Creating socket error\n";
		return false;
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(serviceAddress.c_str());
	address.sin_port = htons( servicePort );

	if(connect(mainSocket, (struct sockaddr *)&address, sizeof address)==-1)
	{
		std::cerr<<"Connecting to ServiceServer failed\n";
		return false;
	}

	std::string message = "TimeRequest";
	if(write(mainSocket, message.c_str(), message.size()) == -1)
	{
		std::cerr << "Sending Echo Message to ServiceServer Error\n";
		return false;
	}

	int bytesRead;
	char response[1024];
	if((bytesRead = read(mainSocket, response, 1024)) == 0)
	{
		std::cerr << "Receiving Echo Message from ServiceServer Error\n";
		return false;		
	}
	else
	{
		std::cout << bytesRead << "\n";
		response[bytesRead] = '\0';
		std::cout << "Server time: ";
		for(int i =0 ; i < bytesRead; ++i)
		{
			std::cout << response[i];
		}
		std::cout <<"\n";
	}
	return true;
}
void Client::LoadClientInfo(int serviceID)
{
	std::cout << "Loaded\n";
	clientInfo[0] = 2;	//ticket request

	Utils::LoadAddress(clientInfo, CLIENT_ADDRESS, 1);
	LoadUserDataFromConsole();

	clientInfo[55] = 0;//It was supposed to be server name but it was idiotic idea
	clientInfo[56] = serviceID;
}

void Client::LoadUserDataFromConsole()
{
	std::string name, password;
	std::cout << "Username: ";
	std::cin >> name;
	std::cout << "Password: ";
	std::cin >> password;
	
	unsigned char hash[20];
	
	unsigned char* resultSHA = SHA1(reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), hash);
	
	Utils::InsertStringToCharTable(clientInfo, name, 5, 34);
	Utils::InsertStringToCharTable(clientInfo, Utils::ToStr(resultSHA,0,20), 35, 54);
}
