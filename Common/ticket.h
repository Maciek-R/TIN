#ifndef TICKET_H
#define TICKET_H

#include <string>
#include "../Common/utils.h"
#include <vector>

class Ticket
{
public:
	Ticket();
	void SetClientAddress(std::string);
	void SetServiceAddress(std::string);
	void SetServicePort(int);
	void SetServiceId(unsigned char);
	unsigned char * GetAsBuffor(int &size);
	void SetValidateTime(std::string time);
	void SetCheckSum(std::vector<int> checkSum);
	

private:
	std::string clientAddress;
	std::string serviceAddress;
	int servicePort;
	char serviceId;
	std::string validateTime; //in seconds
	std::vector<int> checkSum;
};

#endif
