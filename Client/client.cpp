#include "client.h"
#include <iostream>

Client::Client()
	: PORT{8888}, mainSocket{-1}, bytesRead{-1}, ticket{}
{
	//Never do that in costructor.
	if( (mainSocket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{
		std::cerr << "Creating socket error\n";
		exit(EXIT_FAILURE);
	}
	ConnectToServer();
}

void Client::ConnectToServer()
{
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons( PORT );

	if(connect(mainSocket, (struct sockaddr *)&address, sizeof address) == -1)
	{
		std::cerr << "Failed to connect\n";
		exit(EXIT_FAILURE);
	}

	ReadInitMessage();
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
void Client::SendRequestForTicket()
{
	buffer[0] = 'T';
	buffer[1] = '\0';

	printf("Requesting for Ticket\n");
	if (write(mainSocket, buffer, sizeof buffer) == -1)
	{
		std::cerr << "Error request Ticket\n";
	}
	
	if((bytesRead = read(mainSocket, buffer, 1024)) == 0)
	{
		std::cerr << "Error receiving Ticket\n";
	}
	else
	{
		buffer[bytesRead] = '\0';
		std::cerr << "Got Ticket: " << buffer << '\n';
	}
	close(mainSocket);
}
