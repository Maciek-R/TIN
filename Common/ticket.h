#ifndef TICKET_H
#define TICKET_H

#include <string>
#include "../Common/utils.h"

class Ticket
{
public:
	Ticket();
	void SetClientAddress(std::string);
	void SetServiceAddress(std::string);
	void SetServicePort(int);
	void SetServiceId(unsigned char);
	unsigned char * GetAsBuffor(int &size);
	void SetValidateTime(int time);
	void SetCheckSum(std::string checkSum);
	

private:
	std::string clientAddress;
	std::string serviceAddress;
	int servicePort;
	char serviceId;
	int validateTime; //in seconds
	std::string checkSum;
};

#endif
