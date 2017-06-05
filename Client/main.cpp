#include "client.h"
#include <iostream>



int main(int argc , char** argv)
{

//	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

//	if(sock == 0)
//	{
//		std::cout << "socket failed with error: \n";
//		return 1;
//	}

//	unsigned short Port = 27015;

//	sockaddr_in dest;
//	dest.sin_family = AF_INET;
//	dest.sin_addr.s_addr = inet_addr("127.0.0.1");
//	dest.sin_port = htons(Port);

//	Utils::sendudp("Hello from client!", dest, sock);

//	sockaddr_in RecvAddr;
//	int recvaddrlen = sizeof(RecvAddr);

//	std::cout << "Received message from the server: " << Utils::recvudp(sock, 1024, RecvAddr, recvaddrlen) << "\n";

//	std::cout << "Closing socket...\n";

//	close(sock);




	Client client; 

	int serviceID;
	while(true)
	{
		std::cout << "\t\tMENU\n"
				  << "Enter number to execute corresponding request\n\n"
				  << "\t1) TCP echo\n"
				  << "\t2) TCP time\n"
				  << "\t3) UDP echo\n"
				  << "\t4) UDP time\n"
				  << "\t0) Quit\n";
		std::cin >> serviceID;

		if(serviceID == 0)
		{
			std::cout << "Exitting\n";
			break;
		}
		else if(serviceID < 0 || serviceID > 4)
		{
			std::cout << "Invalid choice\n";
			continue;
		}

		client.RunService(serviceID);
	}
	
	return 0;
} 
