#include "ticket.h"
#include <iostream>

Ticket::Ticket()
{}

Ticket::Ticket(char* buffer)
{
}


void Ticket::SetClientAddress(std::string address)
{
	clientAddress = address;
}
void Ticket::SetServiceAddress(std::string address)
{
	serviceAddress = address;
}
void Ticket::SetServicePort(int port)
{
	servicePort = port;
}
void Ticket::SetServiceId(unsigned char id)
{
	serviceId = id;
}
unsigned char * Ticket::GetAsBuffor(int &size)
{
	size = 45;
	unsigned char * mess = new unsigned char[size];	//rozmiar sie zmieni potem
	Utils::LoadAddress(mess, clientAddress, 0);
	Utils::LoadAddress(mess, serviceAddress, 4);

	mess[8] = 8;	//tu zmienic
	mess[9] = 8;	//na metode ktora zamienia inta na 4 bajty
	mess[10] = 8;
	mess[11] = 9;

	mess[12] = serviceId;
	
	std::string validateTime = std::to_string(10);
	Utils::InsertStringToCharTable(mess, validateTime, 14, 29);

	for(unsigned int i = 0; i < 16 ; ++i)
	{
		mess[30+i] = checkSum[i];
		//std::cout << "chuj: " << checkSum[i] << "\n";
	}

	return mess;
}

void Ticket::SetValidateTime(std::string time)
{
	validateTime = time;
	std::cout << "Time set: " << time << "\n";
}

void Ticket::SetCheckSum(std::vector<int> checkSum)
{
	this->checkSum = checkSum;
}

unsigned char* Ticket::Serialize()
{
	unsigned char * result = new unsigned char[45];
	
	Utils::LoadAddress(result, clientAddress, 0);
	Utils::LoadAddress(result, serviceAddress, 4);
	Utils::InsertNumberToCharTable(result, servicePort, 8, 12); // check it
	result[12] = serviceId;
	Utils::InsertNumberToCharTableWithTerm(result, validTime, 14, 29);
	
	for(unsigned int i = 0; i < 16 ; ++i)
	{
		result[30+i] = checkSum[i];
	}
	
	return result;
}
