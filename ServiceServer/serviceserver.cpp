#include "serviceserver.h"
#include <vector>
#include <iostream>

ServiceServer::ServiceServer()
	: PORT{8889}, SERVICE_ADDRESS_1{"127.0.0.1"}, opt{true}
{
	InitClients();
	CreateMainSocket();
	BindMainSocket();
	ListenMainSocket();
}
void ServiceServer::CreateMainSocket()
{
	if((mainSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		std::cout << "Creating Server socket error\n";
		exit(EXIT_FAILURE);
	}

	int option = static_cast<int>(opt);
	if(setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
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
			std::cout << "Error in select\n";
		}
		//new Connection
		if(FD_ISSET(mainSocket, &readfds))
		{
			AcceptNewConnection();
		}
		//message or disconnect
		else
		{
			for(std::size_t i = 0; i < clientSockets.size(); ++i)
			{
				sd = clientSockets[i];
				if(FD_ISSET(sd, &readfds))
				{
					int bytesRead = read( sd , buffer, 1024);
					//Somebody disconnected , get his details and print
					if (bytesRead == 0)
					{
						getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
						std::cout << "Host disconnected\n\tIP: " << inet_ntoa(address.sin_addr)<< "\n\tport: " <<  ntohs(address.sin_port) << '\n';

						close( sd );
						clientSockets[i] = 0;
					}
					//message
					else
					{	
						buffer[bytesRead] = '\0';
						std::cout << "Message from Client nr " <<  i  << ":\t" << buffer<<"\n";

						//Ticket ticket;
						//const char* buff = ticket.ToString().c_str();

						//if( send(sd , buff , strlen(buff) , 0 ) != strlen(buff))
						if(write(sd, buffer, sizeof buffer) == -1)
							std::cerr << "Error while sending message to client\n";
						else
							std::cout << "Echo Message send successfully\n";
					}
				}
			}
		
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
	
	//SendMessage(newSocket, message.c_str());
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

bool ServiceServer::AuthorizeClient(std::string data)
{
	//tutaj bedzie sprawdzanie waznosci biletu

	return true;
}

std::string ServiceServer::ToString(unsigned char * buff, int from, int to)
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
