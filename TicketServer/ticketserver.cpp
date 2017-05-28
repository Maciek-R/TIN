#include "ticketserver.h"
#include <vector>
#include <iostream>

TicketServer::TicketServer()
	: PORT{8888}, SERVICE_ADDRESS_1{"127.0.0.1"}, TICKET_SERVER_ADDRESS{"127.0.0.1"}, opt{true}, serviceInfo{nullptr}
{
	CreateMainSocket();
	BindMainSocket();
}
void TicketServer::CreateMainSocket()
{
	if((mainSocket = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
	{
		std::cout << "Creating Server socket error\n";
		exit(EXIT_FAILURE);
	}
	//setting to accept broadcast
	int option = static_cast<int>(opt);
	if(setsockopt(mainSocket, SOL_SOCKET, SO_BROADCAST, &option, sizeof(option)) < 0)
	{
		std::cerr << "Setting sockopt error\n";
		exit(EXIT_FAILURE);
	}

}
void TicketServer::BindMainSocket()
{
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);	


	if(bind(mainSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cerr << "Binding Error\n";
		exit(EXIT_FAILURE);
	}
	addrlen = sizeof(address);
	std::cout << "Listening on port " << PORT << '\n';
}

void TicketServer::Run()
{
	std::cout << "Waiting for connections\n";

	while(true)
	{
		GetBroadcastMessage();
	}
}

void TicketServer::SendMessage(int socket, const char * message) const
{
	if(send(socket, message, strlen(message), 0)!= strlen(message))
		std::cerr << "Error while sending message to client\n";
	else
		std::cout << "Message send successfully\n";
}

void TicketServer::GetBroadcastMessage()
{
	int bytesRead = recvfrom(mainSocket, buffer, 1024, 0, (struct sockaddr *) &address, (socklen_t*)&addrlen);

	if(buffer[0] == 1 )
	{
		ClientAddress = ToString(buffer, 1, 5);
		
		std::cout << "Receive Broadcast Message from Client. Client Address is: "<<ClientAddress<<"\n";
		AnswerOnBroadcastMessage();
	}
	else if(buffer[0] == 2)
	{
		ClientAddress = ToString(buffer, 1, 5);

		std::string login = ToString(buffer, 5, 35);
		std::string password = ToString(buffer, 35, 55);
		std::string nameServer = ToString(buffer, 55, 56);
		std::string numerService = ToString(buffer, 56, 57); 

		std::cout << "Receive Request for Ticket from Client. Client Address is: "<<ClientAddress<<" Got " << bytesRead <<" bytes\n";
			std::cout <<"login: "<<login<<"\npassword: "<<password<<"\nnameServer: "<<nameServer<<"\nnumerService: "<<numerService<<"\n";

		if(AuthorizeClient(ClientAddress))
			AnswerOnRequestForTicket(true);
		else
			AnswerOnRequestForTicket(false);
	}
	else{
		std::cout << "Unknown Message\n";
		return;
	}
	/*std::string data = buffer;
	if(data[0] == '1')
	{
		ClientAddress = data.substr(1, data.length());
		
		AnswerOnBroadcastMessage();
	}
	else if(data[0] == '2')
	{
		ClientAddress = data.substr(1, data.length());
		std::cout << "Receive Request for Ticket from Client. Client Address is: "<<ClientAddress<<"\n";

		if(AuthorizeClient(data.substr(1, data.length())))
			AnswerOnRequestForTicket(true);
		else
			AnswerOnRequestForTicket(false);
	}
	else{
		std::cout << "Unknown Message\n";
		return;
	}*/

	//std::cout << "Got message from address: "<<buffer<<"\n";
	//std::string data = buffer;

	
}

bool TicketServer::AuthorizeClient(std::string data)
{
	//tutaj bedzie autoryzacja klienta na podstawie jego danych i ewentualne wyslanie mu biletu

	return true;
}

void TicketServer::AnswerOnBroadcastMessage()
{
	address.sin_addr.s_addr = inet_addr(ClientAddress.c_str());

	//std::string message = TICKET_SERVER_ADDRESS;

	//unsigned int lenAddr = message.length()+1;
	unsigned char message[4] {127, 0, 0, 1};		//ticketServerAddress
	if(sendto(mainSocket, message, 4, 0, (struct sockaddr *) &address, sizeof(address)) != 4)
	{
		std::cerr << "Sending TicketServer Address Error";
	}	

	address.sin_addr.s_addr = INADDR_ANY;
}
void TicketServer::AnswerOnRequestForTicket(bool isConfirmed)
{
	address.sin_addr.s_addr = inet_addr(ClientAddress.c_str());
	loadServiceInfo(isConfirmed);

	if(isConfirmed)
	{
		//unsigned char message[5] {1, 127, 0, 0, 1};	//service_address
		//unsigned int lenAddr = message.length()+1;


		if(sendto(mainSocket, serviceInfo, 9, 0, (struct sockaddr *) &address, sizeof(address)) != 9)
		{
			std::cerr << "Sending ServiceAddress Error";
		}	
	}
	else
	{
		//unsigned int lenAddr = message.length()+1;
		//unsigned char message[1] {0};
		if(sendto(mainSocket, serviceInfo, 1, 0, (struct sockaddr *) &address, sizeof(address)) != 1)
		{
			std::cerr << "Error";
		}	
	}

	address.sin_addr.s_addr = INADDR_ANY;
}
std::string TicketServer::ToString(unsigned char * buff, int from, int to)
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
void TicketServer::loadServiceInfo(bool isConfirmed)
{
	unsigned char * mess;

	if(isConfirmed)
	{
		mess = new unsigned char[9];//potwierdzenie + adres + port
		mess[0] = 1;

		mess[1] = 127;// Service Address
		mess[2] = 0;
		mess[3] = 0;
		mess[4] = 1;

		mess[5] = 8;	//port
		mess[6] = 8;
		mess[7] = 8;
		mess[8] = 9;
	}
	else
	{
		mess = new unsigned char[1];//odmowa
		mess[0] = 0;
	}

	if(serviceInfo!=nullptr)
		delete serviceInfo;

	serviceInfo = mess;
}