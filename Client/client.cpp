#include "client.h"
#include <iostream>

Client::Client()
	: BROADCAST_PORT{8888}, BROADCAST_ADDRESS{"127.0.0.255"}, CLIENT_ADDRESS{"127.0.0.1"}, mainSocket{-1}, bytesRead{-1}, ticket{}
{
		LoadClientInfo();
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
	if((bytesRead = recvfrom(mainSocket, buffer, 1024, 0, (struct sockaddr *) &address, (socklen_t*)&addrlen)) < 0)
	{
		std::cerr << "Error while receiving message from TicketServer";
		return false;
	}

	if(buffer[0] == 1)
	{
		ticket = Ticket{};
		ticket.SetClientAddress(Utils::ToString(buffer, 1, 5));
		ticket.SetServiceAddress(Utils::ToString(buffer, 5, 9));
		ticket.SetServicePort(Utils::ToInt(buffer, 9, 13));
		ticket.SetServiceId(buffer[13]);

		serviceAddress = Utils::ToString(buffer, 5, 9);
		servicePort = Utils::ToInt(buffer, 9, 13);

		std::cout << "Got Message from TicketServer. Service Address is: "<<serviceAddress<<" Service Port: "<<servicePort<<"\n";
		return true;
	}
	else if (buffer[0]==0)
	{
		std::cout << "Got Message from TicketServer. You are not on VIP list!\n";
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
	switch(numService)
	{
		case 1: SendTcpEcho(); break;
		case 2: SendTcpTime(); break;
		case 3: break;
		case 4: break;
	}
	

}
bool Client::SendTcpEcho()
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
	char message[1024];
	std::cout <<"Type Message: ";
	std::string info = "Maciek Ty spierdolino!\0";

	std::cin.ignore();
	std::getline(std::cin, info);
	std::cout << "Info " << info << "\n";
	if(info.size() > 1)
	{
		int size;
		unsigned char * buff = ticket.GetAsBuffor(size);
		
		for(int i=0; i<size; ++i)// trzeba bedzie to poprawic na ladniej
			message[i] = buff[i];
		for(int i=size; i<size + strlen(info.c_str()); ++i)
			message[i] = info[i-size];
		
		message[size + strlen(info.c_str())] = '\0';

		std::cout << size + strlen(info.c_str()) << "\n";

		if(write(mainSocket, message, size + strlen(info.c_str())+1) == -1)
		{
			std::cerr << "Sending Echo Message to ServiceServer Error\n";
			return false;
		}
	}
	else
	{
		std::cerr << "Empty message!\n";
		return false;
	}

	int bytesRead;
	if((bytesRead = read(mainSocket, message, 1024))==0)
	{
		std::cerr << "Receiving Echo Message from ServiceServer Error\n";
		return false;		
	}
	else
	{
		message[bytesRead] = '\0';
		std::cout << message << "\n";
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
void Client::LoadClientInfo()
{
	//unsigned char * mess = new unsigned char[57];	// na razie na zywca (57, bo 4+30+20+1+1 -> dokumentacja)
	clientInfo[0] = 2;	//zadanie o bilet

	Utils::LoadAddress(clientInfo, CLIENT_ADDRESS, 1);
	LoadUserDataFromConsole();
	
								//na razie wstawiam tu 1 1, ale to sie bedzie zmieniac
	clientInfo[55] = 1;				//nazwa serwera
	clientInfo[56] = 1;				//nazwa uslugi (1 2 3 4) (tcpecho tcpczas udpecho udpczas)
}

void Client::LoadUserDataFromConsole()
{
	std::string name, password;
	std::cout << "Username: ";
	std::cin >> name;
	std::cout << "Password: ";
	std::cin >> password;
	
	unsigned char hash[20];


	SHA1(reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), hash);
	
	Utils::InsertStringToCharTable(clientInfo, name, 5, 34);
	std::cout << "haslo: " << hash << "\n";

	Utils::InsertStringToCharTable(clientInfo, Utils::ToStr(hash,0,19), 35, 54);
}
