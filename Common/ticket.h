#ifndef TICKET_H
#define TICKET_H

#include <string>
#include "../Common/utils.h"
#include <openssl/sha.h>
#include <vector>

class Ticket
{
public:
	Ticket();
	Ticket(unsigned char *buffer);
	void SetClientAddress(std::string);
	void SetServiceAddress(std::string);
	void SetServicePort(int);
	void SetServiceId(unsigned char);
	//unsigned char * GetAsBuffor(int &size);
	void SetValidateTime(std::string time);
	void SetCheckSum(std::vector<int> checkSum);
	
	void SetValidTime(int validTime);
	
	unsigned char* Serialize();
	void GenerateCheckSum();

	std::string GenerateTicketInString();
private:
	std::string clientAddress;
	std::string serviceAddress;
	int servicePort;
	int serviceId;
	std::string validateTime; //in seconds
	int validTime;
	std::vector<int> checkSum;
};

#endif
