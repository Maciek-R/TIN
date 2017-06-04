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
	std::string GetClientAddress();
	
	void SetServiceAddress(std::string);
	std::string GetServiceAddress();
	
	void SetServicePort(int);
	int GetServicePort();
	
	void SetServiceId(unsigned char);
	int GetServiceId();
	
	unsigned char * GetAsBuffor(int &size);
	
	void SetCheckSum(std::vector<int> checkSum);
	std::vector<int> GetCheckSum();
	
	void SetValidTime(int validTime);
	int GetValidTime();
	
	unsigned char* Serialize();
	
	void GenerateCheckSum();
	
	std::string GenerateTicketInString();

private:
	std::string clientAddress;
	std::string serviceAddress;
	int servicePort;
	int serviceId;
	//std::string validateTime; //in seconds
	int validTime;
	std::vector<int> checkSum;
	
};

#endif
