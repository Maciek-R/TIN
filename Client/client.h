#ifndef CLIENT_H
#define CLIENT_H

class Client
{
public:
	Client();
	void Run();
private:
	const int PORT;
	int mainSocket;
	int valread;
	struct sockaddr_in address;
	char buffer[1024];

	void ConnectToServer();
	void ReadInitMessage();	
};

#endif
