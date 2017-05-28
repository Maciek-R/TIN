#include "client.h"
#include <iostream>

Client::Client()
	: BROADCAST_PORT{8888}, BROADCAST_ADDRESS{"127.0.0.1"}, MY_ADDRESS{"127.0.0.1"}, mainSocket{-1}, bytesRead{-1}, ticket{}
{
		loadClientInfo();
}

bool Client::GetTicketServerAddress()
{
	if(!InitBroadcastSocket())
		return false;

	if(!SendBroadcastMessage())
		return false;

	if(!ReceiveTicketServerAddress())
		return false;

	

	//if(!ConnectToTicketServer())
	//	return false;


//to sie przyda do polaczenia z serwerem uslugowym
	/*if(connect(mainSocket, (struct sockaddr *)&address, sizeof address) == -1)
	{
		std::cerr << "Failed to connect\n";
		return false;
	}*/

	return true;
	//ReadInitMessage();
}
bool Client::GetTicket()
{
	if(!InitSocketWithTicketServer())
		return false;

	if(!SendRequestForTicket())
		return false;
	
	if(!ReceiveTicket())
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
	//std::string message = "1" + MY_ADDRESS;
	//unsigned int lenAddr = message.length()+1;
	unsigned char message[5] {1, 127, 0, 0, 1};
	if(sendto(mainSocket, message, 5, 0, (struct sockaddr *) &address, sizeof(address)) != 5)
	{
		std::cerr << "Error while sending broadcast message";
		return false;
	}
	/*if(sendto(mainSocket, message.c_str(), lenAddr, 0, (struct sockaddr *) &address, sizeof(address)) != lenAddr)
	{
		std::cerr << "Error while sending broadcast message";
		return false;
	}*/
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
	
	TicketServerAddress = ToString(buffer, 0, 4);
	std::cout << "TicketServer address is: "<<TicketServerAddress <<"\n";
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
	address.sin_addr.s_addr = inet_addr(TicketServerAddress.c_str());
	address.sin_port = htons( BROADCAST_PORT );

	return true;
}
bool Client::SendRequestForTicket()
{
	//unsigned char message[5] {2, 127, 0, 0, 1, };	
	if(sendto(mainSocket, clientInfo, 57, 0, (struct sockaddr *) &address, sizeof(address)) != 57)
	{
		std::cerr << "Error while sending request for ticket\n";
		return false;
	}

	/*std::string message = "2" + MY_ADDRESS;
	unsigned int lenAddr = message.length()+1;
	if(sendto(mainSocket, message.c_str(), lenAddr, 0, (struct sockaddr *) &address, sizeof(address)) != lenAddr)
	{
		std::cerr << "Error while sending request for ticket\n";
		return false;
	}*/

	return true;
}

bool Client::ReceiveTicket()
{
	int addrlen = sizeof(address);
	int bytesRead;
	if((bytesRead = recvfrom(mainSocket, buffer, 1024, 0, (struct sockaddr *) &address, (socklen_t*)&addrlen)) == -1)
	{
		std::cerr << "Error while receiving message from TicketServer";
		return false;
	}

	if(buffer[0] == 1)
	{
		ServiceAddress = ToString(buffer, 1, 5);
		ServicePort = ToInt(buffer, 5, 9);

		std::cout << "Got Message from TicketServer. Service Address is: "<<ServiceAddress<<" Service Port: "<<ServicePort<<"\n";
		return true;
	}
	else if (buffer[0]==0)
	{
		std::cout << "Got Message from TicketServer. You are not on VIP list!\n";
		return false;
	}
	else{
		std::cout << "Unknown message\n";
		return false;
	}
close(mainSocket);
	
/*
	std::string data;
	if(bytesRead == 1)
		data = ToString(buffer, 0, 1);
	
	else if(bytesRead == 4)
		data = ToString(buffer, 0, 4);
	else
		std::cout << "Error\n";

	close(mainSocket);
	return TranslateMessageFromTicketServer(data);*/
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
/*	while(fgets(buffer, 51, stdin) != nullptr)
	{	
		if(write(mainSocket, buffer, sizeof buffer) == -1)
			std::cerr << "error while sending message\n";
	}	

	close(mainSocket);*/
}
/*bool Client::SendRequestForTicket()
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
}*/
bool Client::RunService(int numService)
{
	switch(numService)
	{
		case 1: sendTcpEcho(); break;
		case 2: break;
		case 3: break;
		case 4: break;
	}
	

}
bool Client::sendTcpEcho()
{
	if( (mainSocket = socket(AF_INET , SOCK_STREAM , 0)) == -1) 
	{
		std::cerr << "Creating socket error\n";
		return false;
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ServiceAddress.c_str());
	address.sin_port = htons( ServicePort );

	if(connect(mainSocket, (struct sockaddr *)&address, sizeof address)==-1)
	{
		std::cerr<<"Connecting to ServiceServer failed\n";
		return false;
	}

	char message[1024];
	std::cout <<"Type Message: ";
	if(fgets(message, 1024, stdin) != NULL)
	{	
		message[strlen(message)-1] = '\0';
		if(write(mainSocket, message, strlen(message)) == -1)
		{
			std::cerr << "Sending Echo Message to ServiceServer Error\n";
			return false;
		}
	}

	int bytesRead;
	if((bytesRead= read(mainSocket, message, 1024))==0)
	{
		std::cerr << "Receiving Echo Message from ServiceServer Error\n";
		return false;		
	}
	else{
		message[bytesRead] = '\0';
		printf("Message from ServiceServer: %s\n", message);
	}
	return true;
}
std::string Client::ToString(unsigned char * buff, int from, int to)
{
	std::stringstream ss;

	for(size_t i=from; i<to-1; ++i)
	{
		ss << (int) buff[i];
		ss << ".";
	}
	ss << (int) buff[to-1];
	
	return ss.str();
}
int Client::ToInt(unsigned char * buff, int from, int to)
{
	std::stringstream ss;

	for(size_t i=from; i<to; ++i)
		ss << (int) buff[i];
	
	int a;
	ss >> a;

	return a;
}
void Client::loadClientInfo()
{
	unsigned char * mess = new unsigned char[57];	// na razie na zywca (57, bo 4+30+20+1+1 -> dokumentacja)
	mess[0] = 2;	//zadanie o bilet

	mess[1] = 127;//adres
	mess[2] = 0;
	mess[3] = 0;
	mess[4] = 1;

	mess[5] = 'm';	//login
	mess[6] = 'a';
	mess[7] = 'c';
	mess[8] = 'i';
	mess[9] = 'e';
	mess[10] = 'k';	
	mess[11] = 0;

	mess[35] = 'a';				//skrot hasla
	mess[36] = 'b';
	mess[37] = 'c';
	mess[38] = 0;

								//na razie wstawiam tu 1 1, ale to sie bedzie zmieniac
	mess[55] = 1;				//nazwa serwera
	mess[56] = 1;				//nazwa uslugi

	clientInfo = mess;
}
