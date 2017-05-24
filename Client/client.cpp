#include "client.h"
#include <iostream>

Client::Client()
	: BROADCAST_PORT{8888}, BROADCAST_ADDRESS{"127.0.0.1"}, mainSocket{-1}, bytesRead{-1}, ticket{}
{
	
}

bool Client::GetServiceAddress()
{
	if(!InitBroadcastSocket())
		return false;

	if(!ConnectToTicketServer())
		return false;


//to sie przyda do polaczenia z serwerem uslugowym
	/*if(connect(mainSocket, (struct sockaddr *)&address, sizeof address) == -1)
	{
		std::cerr << "Failed to connect\n";
		return false;
	}*/

	return true;
	//ReadInitMessage();
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
bool Client::ConnectToTicketServer()
{
	unsigned int lenAddr = strlen(BROADCAST_ADDRESS.c_str());
	if(sendto(mainSocket, BROADCAST_ADDRESS.c_str(), lenAddr, 0, (struct sockaddr *) &address, sizeof(address)) != lenAddr)
	{
		std::cerr << "Error while sending broadcast message";
		return false;
	}

	int addrlen = sizeof(address);
	if(recvfrom(mainSocket, buffer, 1024, 0, (struct sockaddr *) &address, (socklen_t*)&addrlen) == -1)
	{
		std::cerr << "Error while receiving message from TicketServer";
		return false;
	}
	std::string data = buffer;
	return TranslateMessageFromTicketServer(data);
}
bool Client::TranslateMessageFromTicketServer(std::string data)
{
	if(data[0] == '1')
	{
		ServiceAddress = data.substr(1, data.length());
		std::cout << "Got Message from TicketServer. Service Address is: "<<ServiceAddress<<"\n";
		return true;
	}
	else if(data[0] == '0')
	{
		std::cout << "Got Message from TicketServer. You are not on VIP list!\n";
		return false;
	}
	else{
		std::cout << "Got Message from TicketServer. Unknown message!\n";
		return false;
	}
	
	
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
void Client::Run()
{	
	while(fgets(buffer, 51, stdin) != nullptr)
	{	
		if(write(mainSocket, buffer, sizeof buffer) == -1)
			std::cerr << "error while sending message\n";
	}	

	close(mainSocket);
}
bool Client::SendRequestForTicket()
{
	//TODO
	//create ticket object
	

	//buffer[0] = 'T';
	//buffer[1] = '\0';

	printf("Requesting for Ticket\n");
	std::string s = PrepareData();
	const char* data = s.c_str();
	if (write(mainSocket, data, strlen(data)) == -1)
	{
		std::cerr << "Error request Ticket\n";
		return false;
	}
	
	if((bytesRead = read(mainSocket, buffer, 1024)) == 0)
	{
		std::cerr << "Error receiving Ticket\n";
		return false;
	}
	else
	{
		buffer[bytesRead] = '\0';
		std::cerr << "Got Ticket: " << buffer << '\n';
		//TODO saving ticket
		//
		//
	}
	close(mainSocket);
	return true;
}
void Client::RunService(int numService)
{
	std::cout << "Service nr " << numService <<std::endl;
	//connect to Service
	//and run
}
std::string Client::PrepareData()
{
	std::string data;
	
	//
	data+="Maciek";		//login
	data+="qwerty";		//haslo, na razie jawnie
						//nazwa servera ?
						//nazwa uslugi ?
	return data;
}