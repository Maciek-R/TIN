#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
  
#define TRUE   1
#define FALSE  0
#define PORT 8888
 
#include "client.h"

Client::Client()
{
	if( (mainSocket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{
		perror("Creating socket error");
		exit(EXIT_FAILURE);
	}
	connectToServer();
}

void Client::connectToServer()
{
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons( PORT );

	if(connect(mainSocket, (struct sockaddr *)&address, sizeof address)==-1)
	{
		perror("Connecting error");
		exit(EXIT_FAILURE);
	}

	readInitMessage();
	run();
}
void Client::readInitMessage()
{
	if((valread = read(mainSocket, buffer, 1024))==0)
	{
		perror("Receiving Message Error");		
	}
	else{
		buffer[valread] = '\0';
		printf("Got %d bytes\n", valread);
		printf("From Server: %s", buffer);
	}
}
void Client::run()
{	
	while(fgets(buffer, 51, stdin) != NULL)
	{	
		if(write(mainSocket, buffer, sizeof buffer) == -1)
			perror("error while sending message");
	}	

	close(mainSocket);
}
      
