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
	
	void Serialize(unsigned char* serializedTicket);
	
	void GenerateCheckSum();
	
	bool IsEmpty();

	std::string GenerateTicketInString();
private:
	std::string clientAddress;
	std::string serviceAddress;
	int servicePort;
	int serviceId;
	int validTime;
	std::vector<int> checkSum;
	bool isEmpty;

};

#endif
