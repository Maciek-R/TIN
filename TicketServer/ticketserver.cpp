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
		ClientAddress = Utils::ToString(buffer, 1, 5);
		
		std::cout << "Receive Broadcast Message from Client. Client Address is: "<<ClientAddress<<"\n";
		AnswerOnBroadcastMessage();
	}
	else if(buffer[0] == 2)
	{
		ClientAddress = Utils::ToString(buffer, 1, 5);

		std::string login = Utils::ToString(buffer, 5, 35);
		std::string password = Utils::ToString(buffer, 35, 55);
		std::string nameServer = Utils::ToString(buffer, 55, 56);
		std::string numerService = Utils::ToString(buffer, 56, 57); 

		std::cout << "Receive Request for Ticket from Client. Client Address is: "<<ClientAddress<<" Got " << bytesRead <<" bytes\n";
			std::cout <<"login: "<<login<<"\npassword: "<<password<<"\nnameServer: "<<nameServer<<"\nnumerService: "<<numerService<<"\n";

		AnswerOnRequestForTicket(AuthorizeClient(buffer), buffer[56]);
	
	}
	else{
		std::cout << "Unknown Message\n";
		return;
	}
}

bool TicketServer::AuthorizeClient(unsigned char * data)
{
	if (checkClientInDatabase(data))
		return true;
	else
		return false;
}

void TicketServer::AnswerOnBroadcastMessage()
{
	address.sin_addr.s_addr = inet_addr(ClientAddress.c_str());

	//std::string message = TICKET_SERVER_ADDRESS;

	//unsigned int lenAddr = message.length()+1;
	unsigned char message[4];
	Utils::LoadAddress(message, TICKET_SERVER_ADDRESS, 0);
	
	if(sendto(mainSocket, message, 4, 0, (struct sockaddr *) &address, sizeof(address)) != 4)
	{
		std::cerr << "Sending TicketServer Address Error";
	}	

	address.sin_addr.s_addr = INADDR_ANY;
}

void TicketServer::AnswerOnRequestForTicket(bool isConfirmed, unsigned char idService)
{
	address.sin_addr.s_addr = inet_addr(ClientAddress.c_str());
	loadServiceInfo(isConfirmed, idService);

	if(isConfirmed)
	{
		//unsigned char message[5] {1, 127, 0, 0, 1};	//service_address
		//unsigned int lenAddr = message.length()+1;


		if(sendto(mainSocket, serviceInfo, 14, 0, (struct sockaddr *) &address, sizeof(address)) != 14)
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
void TicketServer::loadServiceInfo(bool isConfirmed, unsigned char idService)
{
	unsigned char * mess;

	if(isConfirmed)
	{
		mess = new unsigned char[14];//potwierdzenie + adres + port
		mess[0] = 1;

		Utils::LoadAddress(mess, ClientAddress, 1);
		Utils::LoadAddress(mess, SERVICE_ADDRESS_1, 5);

		mess[9] = 8;	//port
		mess[10] = 8;
		mess[11] = 8;
		mess[12] = 9;

		mess[13] = idService; //id uslugi

		//czas waznosci

		//pole kontroli kryptograficznej
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

bool TicketServer::checkClientInDatabase(unsigned char * data)
{
		std::fstream file;
		file.open("Common/database", std::ios::in);

		if(!file.good())
		{
			std::cerr <<"Cannot find database" <<std::endl;
			return false;
		}

		std::string clientAddress = Utils::ToString(data, 1, 5);
		std::string clientLogin = Utils::ToStr(data, 5, 35);
		std::string clientPassword = Utils::ToStr(data, 35, 55);

		std::string addr;
		std::string login;
		std::string pass;

		bool f = false;
		while (getline(file, addr))//nie ma sprawdzania nazwy serwera oraz nazwy uslugi(baza tez ich nie uwzglednia)
		{
			getline(file, login);
			getline(file, pass);
			
			std::cout << addr << std::endl;
			std::cout << login << std::endl;
			std::cout << pass << std::endl;

			if(/*clientAddress == addr && */clientLogin == login /*&& clientPassword == pass*/)
			{
				f = true;
				break;
			}

			getline(file, addr);//;
		}

		file.close();

		if(f)
			std::cout << "Found client in database" <<std::endl;
		else
			std::cout << "Not Found Client in database" << std::endl;


		return f;

	//	std::cout << line <<std::endl;

}
