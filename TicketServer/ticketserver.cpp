#include "ticketserver.h"
#include <vector>
#include <iostream>

TicketServer::TicketServer()
	: serviceDataBaseManager{"serversdatabase"},PORT{8888}, SERVICE_ADDRESS_1{"127.0.0.1"}, TICKET_SERVER_ADDRESS{Utils::DetectIP(NetworkObject::interfaceType)},
	  BROADCAST_PORT{8886}, BROADCAST_ADDRESS{Utils::CalculateBroadCast(TICKET_SERVER_ADDRESS, "255.255.255.0")},
	  serviceServersDetails{{1, std::vector<std::pair<int, std::string>>{}}, {2, std::vector<std::pair<int, std::string>>{}}, {3, std::vector<std::pair<int, std::string>>{}}, {4, std::vector<std::pair<int, std::string>>{}}},
	  opt{true}, serviceInfo{nullptr}, mainSocket{-1}, broadcastSocket{-1}
{
	CreateMainSocket();
	BindMainSocket();
	//InitBroadcastSocket(1);
}

TicketServer::~TicketServer()
{
	close(mainSocket);
	close(broadcastSocket);
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
	assert(bytesRead > 0);

	switch(static_cast<RequestType>(buffer[0]))
	{
		case RequestType::ADDRESS:
		{
			assert(bytesRead == 5);
			ClientAddress = Utils::ToString(buffer, 1, 5);
			std::cout << "Receive Broadcast Message from Client. Client Address is: "<<ClientAddress<<"\n";
			AnswerOnBroadcastMessage();
			break;
		}
		case RequestType::TICKET:
		{
			assert(bytesRead == 57);

			ClientAddress = Utils::ToString(buffer, 1, 5);

			std::string login = Utils::ToString(buffer, 5, 35);
			std::string password = Utils::ToString(buffer, 35, 55);
			std::string nameServer = Utils::ToString(buffer, 55, 56);
			std::string numerService = Utils::ToString(buffer, 56, 57);

			std::cout << "Receive Request for Ticket from Client. Client Address is: "<<ClientAddress<<" Got " << bytesRead <<" bytes\n";
				std::cout <<"login: "<<login<<"\npassword: "<<password<<"\nnameServer: "<<nameServer<<"\nnumerService: "<<numerService<<"\n";

			AnswerOnRequestForTicket(AuthorizeClient(buffer), buffer[56]);
			break;
		}
		case RequestType::REGISTER_SERVICE:
		{
			assert(bytesRead == 10);

			int serviceID = buffer[1];
			std::pair<int, std::string> serviceDetails{Utils::ToInt(buffer, 6, 10), Utils::ToString(buffer,2,6)};
			std::cout << serviceID << " " << serviceDetails.first << " " << serviceDetails.second << "\n";

			serviceServersDetails[serviceID].push_back(serviceDetails);
			break;
		}
		default:
		{
			std::cout << (int)buffer[0] << "\n";
			std::cout << "Unknown Message\n";
		}
	}
}

bool TicketServer::AuthorizeClient(unsigned char * data)
{
	return CheckClientInDatabase(data);
}

void TicketServer::AnswerOnBroadcastMessage()
{
	std::cout << ClientAddress;
	address.sin_addr.s_addr = inet_addr(ClientAddress.c_str());

	unsigned char message[4];
	Utils::LoadAddress(message, TICKET_SERVER_ADDRESS, 0);
	
	if(sendto(mainSocket, message, 4, 0, (struct sockaddr *) &address, sizeof(address)) != 4)
	{
		std::cerr << "Sending TicketServer Address Error";
	}	

	address.sin_addr.s_addr = INADDR_ANY;
}

void TicketServer::AnswerOnRequestForTicket(bool isClientAuthorized, unsigned char idService)
{
	std::cout << "Sending ticket\n";
	address.sin_addr.s_addr = inet_addr(ClientAddress.c_str());
	LoadServiceInfo(isClientAuthorized, idService);
	std::cout << "Loaded info\n";

	if(isClientAuthorized)
	{
		if(sendto(mainSocket, serviceInfo, 46, 0, (struct sockaddr *) &address, sizeof(address)) != 46)
		{
			std::cerr << "Sending ServiceAddress Error\n";
		}	
	}
	else
	{
		if(sendto(mainSocket, serviceInfo, 1, 0, (struct sockaddr *) &address, sizeof(address)) != 1)
		{
			std::cerr << "Not authorized\n";

		}	
	}

	address.sin_addr.s_addr = INADDR_ANY;
}

Ticket TicketServer::CreateTicket(unsigned char serviceId, std::pair <int, std::string> details)
{
	Ticket ticket;
	
	ticket.SetClientAddress(ClientAddress);
	ticket.SetServiceAddress(details.second);
	ticket.SetServicePort(details.first);
	ticket.SetServiceId(serviceId);
	ticket.SetValidTime(83); //dafuq!!!? przy wartosci 10, cos dziwnego sie dzieje zapisuje jako 0 1
	ticket.GenerateCheckSum();
	
	return ticket;
}

void TicketServer::LoadServiceInfo(bool isClientAuthorized, unsigned char idService)
{
	assert(idService >= 1 && idService <= 4);
	unsigned char * mess;
	assert(serviceServersDetails[idService].size() > 0);

	if(isClientAuthorized)
	{
		mess = new unsigned char[46];//potwierdzenie + adres + port

		mess[0] = 1;

		std::cout << serviceServersDetails[idService].size() << "\n";
		std::pair<int, std::string> details = serviceServersDetails[idService][rand()%serviceServersDetails[idService].size()];
		
		Ticket createdTicket = CreateTicket(idService, details);
		unsigned char* ticketInChar = createdTicket.Serialize();
		
		for(size_t i = 0; i < 45; ++i)
		{
			mess[i+1] = ticketInChar[i];
		}
		
		/*
		
		Utils::LoadAddress(mess, ClientAddress, 1);
		Utils::LoadAddress(mess, details.second, 5);
		Utils::InsertNumberToCharTable(mess, details.first, 9, 12);
//		mess[9] = 8;	//port
//		mess[10] = 8;
//		mess[11] = 8;
//		mess[12] = 9;
		mess[13] = idService; //id uslugi

		//czas waznosci
		//od 14 do 29
		std::string validateTime = std::to_string(10);
		
		Utils::InsertStringToCharTable(mess, validateTime, 14, 29);
		
		//pole kontroli kryptograficznej  - napisz to wszytko w utils
		std::string ticketMessageAsString = Utils::TicketMessageToString(mess);
		
		unsigned char hash[16];
		unsigned char* checkSum = SHA1(reinterpret_cast<const unsigned char*>(ticketMessageAsString.c_str()), ticketMessageAsString.size(), hash);
		std::cout << "checksum: \n";
		for(unsigned int i = 0; i < 16 ; ++i)
		{
			mess[30+i] = (int)checkSum[i];
			std::cout << (int)checkSum[i] << "\n";
		}
		
		*/
		std::cout << "Ticket: ";
		for(unsigned int i = 0; i < 46 ; ++i)
		{
			std::cout << (int)mess[i] << " ";
		}
		
		std::cout << "\n";
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

bool TicketServer::CheckClientInDatabase(unsigned char * data)
{
		std::fstream file;
		file.open("database", std::ios::in);

		assert(file.good());

		std::string clientAddress = Utils::ToString(data, 1, 5);
		std::string clientLogin = Utils::ToStr(data, 5, 35);
		std::string clientPassword;
		
		for(unsigned int i = 0; i < 20; ++i)
		{
			clientPassword += std::to_string((int)data[i + 35]);
		}
		
		std::string addr;
		std::string login;
		std::string pass;

		bool isClientAuthorized = false;
		while (getline(file, addr))//nie ma sprawdzania nazwy serwera oraz nazwy uslugi(baza tez ich nie uwzglednia)
		{
			getline(file, login);
			getline(file, pass);
			
			std::cout << addr << std::endl;
			std::cout << login << std::endl;
			std::cout << pass << std::endl;

			//for dev time we dont want to check address
			if(/*clientAddress == addr && */clientLogin == login && clientPassword == pass)
			{
				isClientAuthorized = true;
				break;
			}

			getline(file, addr);
		}

		file.close();

		//service authorization
//		if(isClientAuthorized)
//			isClientAuthorized = serviceDataBaseManager.IsServiceInDataBase(data[55], data[56]);

		if(isClientAuthorized)
			std::cout << "Found client in database" <<std::endl;
		else
			std::cout << "Not Found Client in database" << std::endl;

		return isClientAuthorized;
}

bool TicketServer::InitBroadcastSocket(int serviceID)
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
	broadcastAddress.sin_port = htons( PORT + serviceID );

	return true;
}

bool TicketServer::SendBroadcastMessage(int serviceID)
{
	unsigned char message[5] {static_cast<unsigned char>(serviceID)};
	Utils::LoadAddress(message, TICKET_SERVER_ADDRESS, 1);
	if(sendto(broadcastSocket, message, 5, 0, (struct sockaddr *) &broadcastAddress, sizeof(broadcastAddress)) != 5)
	{
		std::cerr << "Error while sending broadcast message";
		return false;
	}
	return true;
}
