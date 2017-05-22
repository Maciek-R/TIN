#include "ticketserver.h"

TicketServer::TicketServer()
{
	initClients();
	createMainSocket();
	bindMainSocket();	
	listenMainSocket();
}
void TicketServer::createMainSocket()
{
	if((mainSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("Creating Server socket error");
		exit(EXIT_FAILURE);
	}
	//setting multiple connections
	if(setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt))<0)
	{
		perror("Setting sockopt error");
		exit(EXIT_FAILURE);
	}

}
void TicketServer::initClients()
{
	for(int i=0; i<MAX_CLIENTS; ++i)
		clientSockets[i]=0;
}
void TicketServer::bindMainSocket()
{
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);	


	if(bind(mainSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("Binding Error");
		exit(EXIT_FAILURE);
	}
}

void TicketServer::listenMainSocket()
{	
	if (listen(mainSocket, 3) < 0)
	{
		perror("Listen Error");
		exit(EXIT_FAILURE);
	}
	
	addrlen = sizeof(address);
	printf("Listen on port %d\n", PORT);
}

void TicketServer::run()
{
	printf("Waiting for connections\n");

	while(TRUE)
	{
		FD_ZERO(&readfds);
		FD_SET(mainSocket, &readfds);
		
		max_sd = mainSocket;	
	
		for(int i=0; i<MAX_CLIENTS; ++i)
		{
			sd = clientSockets[i];
			//if socket exists
			if(sd > 0)
				FD_SET(sd, &readfds);
			if(sd > max_sd)
				max_sd = sd;
		}

		int activity = select(max_sd+1, &readfds, NULL, NULL, NULL);
	
		if((activity < 0) && (errno!=EINTR))
		{
			printf("Error in select");
		}
		//new Connection
		if(FD_ISSET(mainSocket, &readfds))
		{
			AcceptNewConnection();
		}
		//message or disconnect
		else
		{
			for(int i=0; i<MAX_CLIENTS; ++i)
			{
				sd = clientSockets[i];
				if(FD_ISSET(sd, &readfds))
				{
					int valread = read( sd , buffer, 1024);
					//Somebody disconnected , get his details and print
					if (valread == 0)
					{
					    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
					    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
					      
					    close( sd );
					    clientSockets[i] = 0;
					}
					//message
					else
					{	
					    buffer[valread] = '\0';
					    printf("Message from Client nr %d: %s\n", i, buffer);

						Ticket ticket;
						char* buff = ticket.ToCharArray();
					   
						if( send(sd , buff , strlen(buff) , 0 ) != strlen(buff))
							perror("Error while sending message to client");
						else
							puts("Message to client send successfully");
					}
				}
			}		
		
		}
	}


}

void TicketServer::AcceptNewConnection()
{
	int newSocket;
	newSocket = accept(mainSocket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
	if(newSocket < 0)
	{
		perror("Accept Error");
		exit(EXIT_FAILURE);
	}
	printf("New Connection, socket fd is %d, ip is: %s, port: %d\n", newSocket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
	
	SendMessage(newSocket, message);
	SetNewSocket(newSocket);
}
void TicketServer::SendMessage(int socket, char * message)
{
	if(send(socket, message, strlen(message), 0)!= strlen(message))
		perror("Error while sending message to client");
	else
		puts("Message send successfully");
}
void TicketServer::SetNewSocket(int socket)
{
	for(int i=0; i<MAX_CLIENTS; ++i)
	{
		if(clientSockets[i]==0)
		{
			clientSockets[i] = socket;
			break;
		}
	}
}